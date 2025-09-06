#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "../cdefault_geometry.h"

int main(void) {
  ASSERT(WindowInit(1920, 1080, "render example"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  Tri2 tri;
  tri.points[0] = (V2) {200, 200};
  tri.points[1] = (V2) {400, 200};
  tri.points[2] = (V2) {300, 300};

  V3 red = { 1, 0, 0 };
  V3 blue = { 0, 0, 1 };
  while (!WindowShouldClose()) {
    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &mouse_pos);

    V3 color = red;
    if (Tri2ContainsPoint(&tri, &mouse_pos)) { color = blue; }

    DrawTriangleV(tri.points[0], tri.points[1], tri.points[2], color);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
