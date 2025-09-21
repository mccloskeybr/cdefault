#ifndef CDEFAULT_SOUND_H_
#define CDEFAULT_SOUND_H_

#include "cdefault_io.h"
#include "cdefault_std.h"

// API for loading sound files, e.g. off of disk.
// Since sounds are large, the API is fashioned around ingesting sized buffers of audio data.
//
// The following file formats are supported:
// -   WAV: PCM 16, 32, float formats.
//
// E.g.
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

// TODO: test WAV PCM24, PCM32, F32
// TODO: audio format conversion?
// TODO: switch api to be around samples rather than raw bytes?
// TODO: support more sound file types

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

  FileHandle file;
  U32 sound_offset;
  U32 samples_byte_size;
  U32 samples_pos;
};

B32 SoundOpenFile(Sound* sound, U8* file_path);
B32 SoundOpenFileHandle(Sound* sound, FileHandle file);
B32 SoundGetSamplesInterleaved(Sound* sound, U8* sample_bytes, U32 sample_bytes_size, U32* sample_bytes_read);
B32 SoundRestart(Sound* sound);
B32 SoundClose(Sound* sound);

B32 SoundOpenWav(Sound* sound, FileHandle file);
B32 SoundGetSamplesInterleavedWav(Sound* sound, U8* sample_bytes, U32 sample_bytes_size, U32* sample_bytes_read);

#endif // CDEFAULT_SOUND_H_

#ifdef CDEFAULT_SOUND_IMPLEMENTATION
#undef CDEFAULT_SOUND_IMPLEMENTATION

// https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
B32 SoundOpenWav(Sound* sound, FileHandle file) {
  sound->file = file;
  sound->type = SoundFileType_WAV;
  if (!FileHandleSeek(&sound->file, 0, FileSeekPos_Begin)) {
    LOG_ERROR("[SOUND] Unexpectedly unable to seek to file start for WAV file.");
    return false;
  }

  B32 success = false;
  Arena* arena = ArenaAllocate();
  // NOTE: 128 is just an arbitrarily large amount that should contain all header versions.
  // TODO: debug assertions may be thrown if the file is malformed, do we care?
  U8* header = ARENA_PUSH_ARRAY(arena, U8, 128);
  U32 bytes_read;
  if (!FileHandleRead(&sound->file, header, 128, &bytes_read)) { goto sound_open_wav_exit; }

  BinReader r;
  BinReaderInit(&r, header, bytes_read);
  if (!(BinReader8(&r) == 'R' && BinReader8(&r) == 'I' &&
        BinReader8(&r) == 'F' && BinReader8(&r) == 'F')) { goto sound_open_wav_exit; }
  BinReaderSkip(&r, 4, sizeof(U8));
  if (!(BinReader8(&r) == 'W' && BinReader8(&r) == 'A' &&
        BinReader8(&r) == 'V' && BinReader8(&r) == 'E')) { goto sound_open_wav_exit; }
  if (!(BinReader8(&r) == 'f' && BinReader8(&r) == 'm' &&
        BinReader8(&r) == 't' && BinReader8(&r) == ' ')) { goto sound_open_wav_exit; }

  // NOTE: chunk_size can be variable depending on the WAV version.
  U32 chunk_size      = BinReader32LE(&r);
  U16 bin_fmt         = BinReader16LE(&r);
  sound->channels     = BinReader16LE(&r);
  sound->frequency    = BinReader32LE(&r);
  BinReader32LE(&r); // avg_byte_per_sec
  BinReader16LE(&r); // block_align
  U16 bits_per_sample = BinReader16LE(&r);

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
      goto sound_open_wav_exit;
    } break;
  }

  BinReaderSetPos(&r, 20 + chunk_size);
  // NOTE: extra chunk may be here for non-PCM (float, WAVE_FORMAT_EXTENSIBLE) formats.
  U8 c0 = BinReader8(&r), c1 = BinReader8(&r), c2 = BinReader8(&r), c3 = BinReader8(&r);
  if (c0 == 'f' && c1 == 'a' && c2 == 'c' && c3 == 't') {
    BinReaderSkip(&r, 8, sizeof(U8));
    c0 = BinReader8(&r); c1 = BinReader8(&r); c2 = BinReader8(&r); c3 = BinReader8(&r);
  }

  if (!(c0 == 'd' && c1 == 'a' && c2 == 't' && c3 == 'a')) { goto sound_open_wav_exit; }
  sound->samples_byte_size = BinReader32LE(&r);
  sound->sound_offset = r.pos;
  sound->samples_pos = 0;
  if (!FileHandleSeek(&sound->file, sound->sound_offset, FileSeekPos_Begin)) {
    LOG_ERROR("[SOUND] Unexpectedly unable to seek to sound start for WAV file.");
    goto sound_open_wav_exit;
  }
  success = true;
sound_open_wav_exit:
  ArenaRelease(arena);
  return success;
}

B32 SoundGetSamplesInterleavedWav(Sound* sound, U8* sample_bytes, U32 sample_bytes_size, U32* sample_bytes_read) {
  if (sound->samples_pos >= sound->samples_byte_size) {
    *sample_bytes_read = 0;
    return true;
  }
  if (!FileHandleRead(&sound->file, sample_bytes, sample_bytes_size, sample_bytes_read)) { return false; }
  if (sound->samples_pos + *sample_bytes_read >= sound->samples_byte_size) {
    *sample_bytes_read = sound->samples_byte_size - sound->samples_pos;
  }
  sound->samples_pos += *sample_bytes_read;
  return true;
}

B32 SoundGetSamplesInterleaved(Sound* sound, U8* sample_bytes, U32 sample_bytes_size, U32* sample_bytes_read) {
  switch (sound->type) {
    case SoundFileType_WAV: { return SoundGetSamplesInterleavedWav(sound, sample_bytes, sample_bytes_size, sample_bytes_read); } break;
    default:                { UNREACHABLE(); return -1; } break;
  }
}

B32 SoundOpenFile(Sound* sound, U8* file_path) {
  FileHandle file;
  if (!FileHandleOpen(&file, file_path, FileMode_Read)) { return false; }
  return SoundOpenFileHandle(sound, file);
}

B32 SoundOpenFileHandle(Sound* sound, FileHandle file) {
  if (SoundOpenWav(sound, file)) { return true; }
  return false;
}

B32 SoundRestart(Sound* sound) {
  if (!FileHandleSeek(&sound->file, sound->sound_offset, FileSeekPos_Begin)) { return false; }
  sound->samples_pos = 0;
  return true;
}

B32 SoundClose(Sound* sound) {
  return FileHandleClose(&sound->file);
}

#endif // CDEFAULT_SOUND_IMPLEMENTATION
