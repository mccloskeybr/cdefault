#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  DEBUG_ASSERT(DirSetCurrentToExeDir());
  DEBUG_ASSERT(AudioInit());

  Sound sound;
  DEBUG_ASSERT(SoundOpenFile(&sound, Str8Lit("../data/test.wav")));

  AudioStreamSpec spec;
  MEMORY_ZERO_STRUCT(&spec);
  spec.device_handle = AUDIO_DEFAULT_DEVICE;
  spec.channels  = sound.channels;
  spec.frequency = sound.frequency;
  spec.format    = (AudioStreamFormat) sound.format;
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
