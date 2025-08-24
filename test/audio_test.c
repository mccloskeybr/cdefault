#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_AUDIO_IMPLEMENTATION
#include "../cdefault_audio.h"

#define SAMPLE_RATE 44100.0f
#define FREQUENCY 261.0f

static F32 t = 0;
static void GenerateSineWave(F32* buffer, U64 num_samples, F32 amplitude) {
  for (U64 i = 0; i < num_samples; i += 1) {
    F32 sample = (F32) (amplitude * F32Sin(2.0f * F32_PI * FREQUENCY * ((F32)t / SAMPLE_RATE)));
    buffer[i + 0] = sample;
    t++;
  }
}

int main(void) {
  ASSERT(AudioInit());

  AudioStreamSpec spec;
  MEMORY_ZERO_STRUCT(&spec);
  spec.device_handle = 1;
  spec.channels = 2;
  spec.frequency = 44100;
  spec.format = AUDIO_FORMAT_F32;
  AudioStreamHandle stream;
  ASSERT(AudioStreamOpen(&stream, spec));

  U8* buffer;
  U32 buffer_size;
  while(true) {
    if (!AudioStreamAcquireBuffer(stream, &buffer, &buffer_size)) { continue; }
    GenerateSineWave((F32*) buffer, buffer_size / sizeof(F32), 0.05f);
    if (!AudioStreamReleaseBuffer(stream)) { continue; }
    if (!AudioStreamWait(stream)) { continue; }
  }

  AudioStreamClose(stream);
  AudioDeinit();
}
