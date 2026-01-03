#ifndef CDEFAULT_SOUND_H_
#define CDEFAULT_SOUND_H_

#include "cdefault_io.h"
#include "cdefault_std.h"

// TODO: test WAV PCM24, PCM32, F32
// TODO: audio format conversion?
// TODO: switch api to be around samples rather than raw bytes?
// TODO: support more sound file types

// API for loading sound files, e.g. off of disk.
// Since sounds are usually large, the API is fashioned around ingesting sized buffers of audio data.
//
// The following file formats are supported:
// -   WAV: PCM 16, 32, float formats.
//
// e.g.
//
// // Load the sound.
// Sound sound;
// DEBUG_ASSERT(SoundOpenFile(&sound, "data/my_sound.wav"));
//
// // Open an audio stream.
// AudioInit();
// AudioStreamSpec spec;
// spec.channels  = sound.channels;
// spec.frequency = sound.frequency;
// spec.format    = sound.format;
// AudioStreamHandle stream;
// DEBUG_ASSERT(AudioStreamOpen(&stream, spec));
//
// U8* buffer;
// U32 buffer_size, bytes_read;
// while (true) {
//   DEBUG_ASSERT(AudioStreamAcquireBuffer(stream, &buffer, &buffer_size));
//   DEBUG_ASSERT(SoundGetSamples(&sound, buffer, buffer_size, &bytes_read));
//   // NOTE: sound buffer has been exhausted once we fail to read a whole buffer.
//   if (bytes_read < buffer_size) { DEBUG_ASSERT(SoundRestart(&sound)); }
//   DEBUG_ASSERT(AudioStreamReleaseBuffer(stream, buffer, bytes_read));
//   DEBUG_ASSERT(AudioStreamWait(stream));
// }
//
// SoundClose(&sound);
// AudioStreamClose(stream);
// AudioDeinit();

typedef enum SoundFileType SoundFileType;
enum SoundFileType {
  SoundFileType_WAV,
};

// NOTE: This should match with AudioStreamFormat for simplicity.
typedef enum SoundFormat SoundFormat;
enum SoundFormat {
  SoundFormat_F32,
  SoundFormat_S16,
  SoundFormat_S24,
  SoundFormat_S32,
};

typedef struct Sound Sound;
struct Sound {
  SoundFileType type;
  SoundFormat format;
  U32 channels;
  U32 frequency;

  FileHandle* file;
  U32 sound_offset;
  U32 samples_byte_size;
  U32 samples_pos;
};

B32 SoundOpenFile(Sound* sound, String8 file_path);
B32 SoundOpenFileHandle(Sound* sound, FileHandle* file);
B32 SoundGetSamplesInterleaved(Sound* sound, U8* sample_bytes, S32 sample_bytes_size, S32* sample_bytes_read);
B32 SoundRestart(Sound* sound);
B32 SoundClose(Sound* sound);

B32 SoundOpenWav(Sound* sound, FileHandle* file);
B32 SoundGetSamplesInterleavedWav(Sound* sound, U8* sample_bytes, S32 sample_bytes_size, S32* sample_bytes_read);

#endif // CDEFAULT_SOUND_H_

#ifdef CDEFAULT_SOUND_IMPLEMENTATION
#undef CDEFAULT_SOUND_IMPLEMENTATION

#define SOUND_LOG_OUT_OF_CHARS() LOG_ERROR("[FONT] Ran out of characters in image file.")

// https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
#define SOUND_TRY_PARSE(eval) if (!eval) { SOUND_LOG_OUT_OF_CHARS(); return false; }
B32 SoundOpenWav(Sound* sound, FileHandle* file) {
  MEMORY_ZERO_STRUCT(sound);

  sound->file = file;
  sound->type = SoundFileType_WAV;
  if (!FileHandleSeek(sound->file, 0, FileSeekPos_Begin)) {
    LOG_ERROR("[SOUND] Unexpectedly unable to seek to file start for WAV file.");
    return false;
  }

  // NOTE: 128 is just an arbitrarily large amount that should contain all header versions.
  U8 bytes[128];
  S32 bytes_read;
  if (!FileHandleRead(sound->file, bytes, STATIC_ARRAY_SIZE(bytes), &bytes_read)) { return false; }

  BinStream s;
  BinStreamInit(&s, bytes, bytes_read);

  String8 tag;
  SOUND_TRY_PARSE(BinStreamPullStr8(&s, 4, &tag));
  if (!Str8Eq(tag, Str8Lit("RIFF"))) { return false; }

  SOUND_TRY_PARSE(BinStreamSkip(&s, 4, sizeof(U8)));

  SOUND_TRY_PARSE(BinStreamPullStr8(&s, 4, &tag));
  if (!Str8Eq(tag, Str8Lit("WAVE"))) { return false; }
  SOUND_TRY_PARSE(BinStreamPullStr8(&s, 4, &tag));
  if (!Str8Eq(tag, Str8Lit("fmt "))) { return false; }

  // NOTE: chunk_size can be variable depending on the WAV version.
  U32 chunk_size;
  U16 bin_fmt, bits_per_sample;
  SOUND_TRY_PARSE(BinStreamPullU32LE(&s, &chunk_size));
  SOUND_TRY_PARSE(BinStreamPullU16LE(&s, &bin_fmt));
  SOUND_TRY_PARSE(BinStreamPullU16LE(&s, (U16*) &sound->channels));
  SOUND_TRY_PARSE(BinStreamPullU32LE(&s, &sound->frequency));
  SOUND_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U32)));
  SOUND_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U16)));
  SOUND_TRY_PARSE(BinStreamPullU16LE(&s, &bits_per_sample));

  // TODO: handle WAVE_FORMAT_EXTENSIBLE
  switch (bin_fmt) {
    case 1:  {
      switch (bits_per_sample) {
        case 16: { sound->format = SoundFormat_S16; } break;
        case 24: { sound->format = SoundFormat_S24; } break;
        case 32: { sound->format = SoundFormat_S32; } break;
        default: {
          LOG_ERROR("[SOUND] Unrecognized PCM bits per sample: %d", bits_per_sample);
          return false;
        } break;
      }
    } break;
    case 3:  { sound->format = SoundFormat_F32; } break;
    default: {
      LOG_ERROR("[SOUND] Invalid or unsupported WAV format detected: %d", bin_fmt);
      return false;
    } break;
  }

  SOUND_TRY_PARSE(BinStreamSeek(&s, 20 + chunk_size));

  // NOTE: extra chunk may be here for non-PCM formats.
  SOUND_TRY_PARSE(BinStreamPullStr8(&s, 4, &tag));
  if (Str8Eq(tag, Str8Lit("fact"))) {
    SOUND_TRY_PARSE(BinStreamSkip(&s, 8, sizeof(U8)));
    SOUND_TRY_PARSE(BinStreamPullStr8(&s, 4, &tag));
  }
  if (!Str8Eq(tag, Str8Lit("data"))) { return false; }

  SOUND_TRY_PARSE(BinStreamPullU32LE(&s, &sound->samples_byte_size));
  sound->sound_offset = s.pos;
  sound->samples_pos = 0;
  if (!FileHandleSeek(sound->file, sound->sound_offset, FileSeekPos_Begin)) {
    LOG_ERROR("[SOUND] Unexpectedly unable to seek to sound start for WAV file.");
    return false;
  }
  return true;
}

B32 SoundGetSamplesInterleavedWav(Sound* sound, U8* sample_bytes, S32 sample_bytes_size, S32* sample_bytes_read) {
  DEBUG_ASSERT(sound->type == SoundFileType_WAV);
  if (sound->samples_pos >= sound->samples_byte_size) {
    *sample_bytes_read = 0;
    return true;
  }
  if (!FileHandleRead(sound->file, sample_bytes, sample_bytes_size, sample_bytes_read)) { return false; }
  if (sound->samples_pos + *sample_bytes_read >= sound->samples_byte_size) {
    *sample_bytes_read = sound->samples_byte_size - sound->samples_pos;
  }
  sound->samples_pos += *sample_bytes_read;
  return true;
}

B32 SoundGetSamplesInterleaved(Sound* sound, U8* sample_bytes, S32 sample_bytes_size, S32* sample_bytes_read) {
  switch (sound->type) {
    case SoundFileType_WAV: { return SoundGetSamplesInterleavedWav(sound, sample_bytes, sample_bytes_size, sample_bytes_read); } break;
    default:                { UNREACHABLE(); return -1; } break;
  }
}

B32 SoundOpenFile(Sound* sound, String8 file_path) {
  FileHandle* file;
  if (!FileHandleOpen(&file, file_path, FileMode_Read)) { return false; }
  return SoundOpenFileHandle(sound, file);
}

B32 SoundOpenFileHandle(Sound* sound, FileHandle* file) {
  if (SoundOpenWav(sound, file)) { return true; }
  return false;
}

B32 SoundRestart(Sound* sound) {
  if (!FileHandleSeek(sound->file, sound->sound_offset, FileSeekPos_Begin)) { return false; }
  sound->samples_pos = 0;
  return true;
}

B32 SoundClose(Sound* sound) {
  return FileHandleClose(sound->file);
}

#undef SOUND_LOG_OUT_OF_CHARS

#endif // CDEFAULT_SOUND_IMPLEMENTATION
