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

  Line2 line;
  line.start.x = 0;
  line.start.y = 1;
  line.end.x = 0;
  line.end.y = 0;

  Circle2 circle;
  circle.center_point.x = 200;
  circle.center_point.y = 300;
  circle.radius = 100;

  Aabb2 aabb;
  aabb.center_point.x = 500;
  aabb.center_point.y = 300;
  aabb.size.x = 200;
  aabb.size.y = 100;

  Tri2 tri;
  tri.points[0] = (V2) { 700, 250 };
  tri.points[1] = (V2) { 800, 250 };
  tri.points[2] = (V2) { 750, 400 };

  Line2 other_line;
  other_line.start.x = 850;
  other_line.start.y = 200;
  other_line.end.x = 950;
  other_line.end.y = 400;

  Ray2 ray;
  ray.start.x = 1000;
  ray.start.y = 200;
  ray.dir.x = 0;
  ray.dir.y = 1;

  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &mouse_pos);

    if (WindowIsMouseButtonJustPressed(MouseButton_Left)) {
      line.start = mouse_pos;
    }
    line.end = mouse_pos;

    DrawCircleV(circle.center_point, circle.radius, V3_BLUE);
    DrawRectangleV(aabb.center_point, aabb.size, V3_BLUE);
    DrawTriangleV(tri.points[0], tri.points[1], tri.points[2], V3_BLUE);
    DrawLineV(other_line.start, other_line.end, 5, V3_BLUE);

    V2 ray_end;
    V2MultF32(&ray_end, &ray.dir, 5000);
    V2AddV2(&ray_end, &ray.start, &ray_end);
    DrawLineV(ray.start, ray_end, 5, V3_BLUE);

    DrawLineV(line.start, line.end, 5, V3_WHITE);

    V2 enter, exit;
    if (Line2IntersectCircle2(&line, &circle, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Line2IntersectAabb2(&line, &aabb, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Line2IntersectTri2(&line, &tri, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Line2IntersectLine2(&line, &other_line, &enter)) {
      DrawCircleV(enter, 10, V3_GREEN);
    }
    if (Line2IntersectRay2(&line, &ray, &enter)) {
      DrawCircleV(enter, 10, V3_GREEN);
    }

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
