#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"

int main(void) {
  ASSERT(WindowInit(1920, 1080, "render example"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  V3 red = { 1, 0, 0 };
  V3 blue = { 0, 0, 1 };
  F32 theta = 0.0f;
  while (!WindowShouldClose()) {
    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &mouse_pos);

    DrawRectangleRotV(mouse_pos, (V2) { 50, 50 }, theta, blue);
    theta += 0.005f;
    DrawTriangle(200, 200, 400, 200, 300, 300, red.r, red.g, red.b);
    DrawRectangle(600, 200, 100, 100, 1, 0, 0);
    DrawRectangle(600, 200, 90, 90, 0, 1, 0);
    DrawRoundedRectangle(600, 200, 100, 100, 20, 0, 0, 1);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
