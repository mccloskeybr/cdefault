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
  opts.clear_rgb = 0x6495ed;
  ASSERT(WindowInit(opts));

  while (true) {
    DrawRectangle(300, 300, 500, 500, 0, 0, 1);
    DrawCircle(800, 300, 100, 0, 0, 1);
    WindowSwapBuffers();
  }

  WindowDeinit();
  return 0;
}
