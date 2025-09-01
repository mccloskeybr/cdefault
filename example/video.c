#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_VIDEO_IMPLEMENTATION
#include "../cdefault_video.h"

int main(void) {
  WindowInitOpts opts;
  MEMORY_ZERO_STRUCT(&opts);
  opts.width = 1920;
  opts.height = 1080;
  opts.title = "video example";
  opts.clear_rgb = 0x6495ed;
  ASSERT(WindowInit(opts));

  WindowFullscreen(false);
  WindowFullscreen(false);

  float x = 0.0f;
  while (true) {
    // DrawCircle(800, 300, 100, 0, 0, 1);
    // DrawLine(300, 300, 500, 600, 5, 1, 0, 0);
    DrawRoundedRectangleRot(300, 300, 500, 500, 100, x, 0, 0, 1);
    x += 0.0001f;

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
