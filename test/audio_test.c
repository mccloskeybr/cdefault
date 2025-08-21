#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_AUDIO_IMPLEMENTATION
#include "../cdefault_audio.h"

#define SAMPLE_RATE 44100.0f
#define FREQUENCY 261.626f

static void GenerateSineWave(F32* buffer, U64 num_samples, F32 amplitude) {
  for (U64 i = 0; i < num_samples; i++) {
    F32 t = ((F32)i) / SAMPLE_RATE;
    buffer[i] = amplitude * F32Sin(2.0f * F32_PI * FREQUENCY * t);
  }
}

int main(void) {
  ASSERT(AudioInit());
  while(true) {
    S32 default_device = AudioGetDefaultDevice();

    U8* buffer = NULL;
    U32 buffer_size = 0;
    if (!AudioDeviceMaybeGetBuffer(default_device, &buffer, &buffer_size)) {
      continue;
    }
    ASSERT(buffer != NULL);
    ASSERT(buffer_size > 0);

    // TODO: why is this massive conversion needed
    GenerateSineWave((F32*) buffer, buffer_size / sizeof(F32), 0.05f);
    AudioDevicePlayBuffer(default_device);
  }
  AudioDeinit();
}
