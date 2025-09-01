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

  V3 red = {1, 0, 0};
  V3 green = {0, 1, 0};
  V3 blue = {0, 0, 1};

  V2 size = {10, 10};

  V2 p1 = { 50, 100 };
  V2 p2 = {500, 100};
  V2 p3 = { 75, 500 };

  while (true) {
    DrawRectangleV(p1, size, green);
    // DrawRectangleV(p2, size, green);
    // DrawRectangleV(p3, size, green);

    // DrawLineV(p1, p2, 10, red);
    // DrawLineV(p2, p3, 10, red);
    // DrawLineV(p3, p1, 10, red);

    // DrawTriangleV(p1, p2, p3, blue);

    V3 start;
    RendererCastRay(p1.x, p1.y, &start, NULL);
    DrawRectangleV((V2) {p1.x, p1.y}, (V2) {5, 5}, blue);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
