#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "../cdefault_geometry.h"

int main(void) {
  TimeInit();
  ASSERT(WindowInit(1280, 720, "collision"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &mouse_pos);

    Circle2 circle;
    circle.radius = 50;
    circle.center_point = mouse_pos;

    Tri2 tri;
    tri.points[0].x = 100;
    tri.points[0].y = 100;
    tri.points[1].x = 400;
    tri.points[1].y = 100;
    tri.points[2].x = 300;
    tri.points[2].y = 300;

    V3 tri_color = V3_BLUE;
    IntersectManifold2 manifold;
    if (IntersectCircle2Tri2(&manifold, &circle, &tri)) {
      tri_color = V3_RED;
    }
    V2 i_vec;
    V2MultF32(&i_vec, &manifold.normal, manifold.penetration);
    V2 end;
    V2AddV2(&end, &circle.center_point, &i_vec);

    DrawCircleV(circle.center_point, circle.radius, V3_GREEN);
    DrawTriangleV(tri.points[0], tri.points[1], tri.points[2], tri_color);
    DrawLineV(circle.center_point, end, 5, V3_WHITE);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
