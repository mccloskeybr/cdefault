#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_AUDIO_IMPLEMENTATION
#include "../cdefault_audio.h"

int main(void) {
  ASSERT(AudioInit());
  AudioDeinit();
}
