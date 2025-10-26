#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_IO_IMPLEMENTATION
#include "../cdefault_io.h"
#define CDEFAULT_AUDIO_IMPLEMENTATION
#include "../cdefault_audio.h"
#define CDEFAULT_SOUND_IMPLEMENTATION
#include "../cdefault_sound.h"

int main(void) {
  DEBUG_ASSERT(DirSetCurrentToExeDir());
  DEBUG_ASSERT(AudioInit());

  Sound sound;
  DEBUG_ASSERT(SoundOpenFile(&sound, (U8*) "../data/test.wav"));

  AudioStreamSpec spec;
  MEMORY_ZERO_STRUCT(&spec);
  spec.device_handle = AUDIO_DEFAULT_DEVICE;
  spec.channels  = sound.channels;
  spec.frequency = sound.frequency;
  spec.format    = sound.format;
  AudioStreamHandle stream;
  DEBUG_ASSERT(AudioStreamOpen(&stream, spec));

  String8 buffer;
  S32 bytes_read;
  while(true) {
    DEBUG_ASSERT(AudioStreamAcquireBuffer(stream, &buffer.str, &buffer.size));
    DEBUG_ASSERT(SoundGetSamplesInterleaved(&sound, buffer.str, buffer.size, &bytes_read));
    if (bytes_read < buffer.size) { DEBUG_ASSERT(SoundRestart(&sound)); }
    DEBUG_ASSERT(AudioStreamReleaseBuffer(stream, buffer.str, bytes_read));
    DEBUG_ASSERT(AudioStreamWait(stream));
  }

  AudioStreamClose(stream);
  SoundClose(&sound);
  AudioDeinit();
}
