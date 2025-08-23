#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_AUDIO_IMPLEMENTATION
#include "../cdefault_audio.h"

#define SAMPLE_RATE 44100.0f
#define FREQUENCY 261.626f

static F32 t = 0;
static void GenerateSineWave(F32* buffer, U64 num_samples, F32 amplitude) {
  for (U64 i = 0; i < num_samples; i++) {
    buffer[i] = amplitude * F32Sin(2.0f * F32_PI * FREQUENCY * ((F32)t / SAMPLE_RATE));
    t++;
  }
}

int main(void) {
  ASSERT(AudioInit());
  while(true) {
    S32 default_device;
    if (!AudioGetDefaultDevice(&default_device)) { continue; }

    U8* buffer = NULL;
    U32 buffer_size = 0;
    if (!AudioDeviceGetBuffer(default_device, &buffer, &buffer_size)) { continue; }

    GenerateSineWave((F32*) buffer, buffer_size / sizeof(F32), 0.05f);
    if (!AudioDevicePlayBuffer(default_device)) { continue; }
    if (!AudioDeviceWait(default_device)) { continue; }
  }
  AudioDeinit();
}
