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

  M4 p;
  // M4Orthographic(-10, 10, -10, 10, 0.01f, 10.0f, &p);
  M4Perspective(F32_PI / 2.0f, 16.0f / 9.0f, 0.01f, 100.0f, &p);
  DrawSetPerspective(p);

  while (true) {
    DrawRectangle(-0.5, -0.5, 1, 1, 0, 0, 1);
    WindowSwapBuffers();
  }

  WindowDeinit();
  return 0;
}
