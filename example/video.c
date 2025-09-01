#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_VIDEO_IMPLEMENTATION
#include "../cdefault_video.h"

int main(void) {
  WindowInitOpts opts;
  MEMORY_ZERO_STRUCT(&opts);
  opts.title = "video example";
  ASSERT(WindowInit(opts));

  F32 x = 500;
  while (true) {
    DrawRectangle(x, 500, 10, 20, 0, 0, 1);
    WindowSwapBuffers();
    x += 0.04f;
  }

  WindowDeinit();
  return 0;
}
