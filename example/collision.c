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

  Ray2 ray;
  ray.start.x = 0;
  ray.start.y = 0;
  ray.dir.x = 0;
  ray.dir.y = 1;

  Circle2 circle;
  circle.center_point.x = 300;
  circle.center_point.y = 300;
  circle.radius = 100;

  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &mouse_pos);

    if (WindowIsMouseButtonJustPressed(MouseButton_Left)) {
      ray.start = mouse_pos;
    }
    V2SubV2(&ray.dir, &mouse_pos, &ray.start);
    if (V2LengthSq(&ray.dir) == 0) {
      ray.dir.x = 0;
      ray.dir.y = 1;
    }
    V2Normalize(&ray.dir, &ray.dir);

    V2 ray_end;
    V2MultF32(&ray_end, &ray.dir, 5000);
    V2AddV2(&ray_end, &ray.start, &ray_end);

    DrawLineV(ray.start, ray_end, 5, V3_WHITE);
    DrawCircleV(circle.center_point, circle.radius, V3_BLUE);

    V2 enter, exit;
    if (Ray2IntersectCircle2(&ray, &circle, &enter, &exit)) {
      DrawCircleV(enter, 5, V3_RED);
      DrawCircleV(exit, 5, V3_GREEN);
    }

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
