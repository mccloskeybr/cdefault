#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "../cdefault_geometry.h"

static void DrawIntersectManifold(Circle2 circle, IntersectManifold2 manifold, V3 color) {
  Line2 pen;
  pen.start = circle.center_point;
  V2MultF32(&pen.end, &manifold.normal, manifold.penetration);
  V2AddV2(&pen.end, &pen.start, &pen.end);
  DrawLineV(pen.start, pen.end, 5, color);
}

int main(void) {
  TimeInit();
  ASSERT(WindowInit(1280, 720, "collision"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  Circle2 circle;
  circle.center_point.x = 0;
  circle.center_point.y = 0;
  circle.radius = 50;

  Circle2 other_circle;
  other_circle.center_point.x = 200;
  other_circle.center_point.y = 300;
  other_circle.radius = 100;

  Aabb2 aabb;
  aabb.center_point.x = 500;
  aabb.center_point.y = 300;
  aabb.size.x = 200;
  aabb.size.y = 100;

  Tri2 tri;
  tri.points[0] = (V2) { 700, 250 };
  tri.points[1] = (V2) { 800, 250 };
  tri.points[2] = (V2) { 750, 400 };

  Line2 line;
  line.start.x = 850;
  line.start.y = 200;
  line.end.x = 950;
  line.end.y = 400;

  Ray2 ray;
  ray.start.x = 1000;
  ray.start.y = 200;
  ray.dir.x = 0;
  ray.dir.y = 1;

  V2 hull_points[6] = {
    {1100, 250},
    {1150, 250},
    {1200, 300},
    {1150, 350},
    {1100, 350},
    {1050, 300},
  };
  ConvexHull2 hull;
  hull.points = (V2*) hull_points;
  hull.points_len = 6;

  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &mouse_pos);
    circle.center_point = mouse_pos;

    DrawCircleV(other_circle.center_point, other_circle.radius, V3_BLUE);
    DrawRectangleV(aabb.center_point, aabb.size, V3_BLUE);
    DrawTriangleV(tri.points[0], tri.points[1], tri.points[2], V3_BLUE);
    DrawLineV(line.start, line.end, 5, V3_BLUE);

    V2 ray_end;
    V2MultF32(&ray_end, &ray.dir, 5000);
    V2AddV2(&ray_end, &ray.start, &ray_end);
    DrawLineV(ray.start, ray_end, 5, V3_BLUE);

    DrawConvexHullV(hull.points, hull.points_len, V3_BLUE);

    DrawCircleV(circle.center_point, circle.radius, V3_WHITE);

    V2 enter, exit;
    IntersectManifold2 manifold;
    if (Circle2IntersectCircle2(&circle, &other_circle, &manifold)) {
      DrawIntersectManifold(circle, manifold, V3_GREEN);
    }
    if (Circle2IntersectAabb2(&circle, &aabb, &manifold)) {
      DrawIntersectManifold(circle, manifold, V3_GREEN);
    }
    if (Circle2IntersectTri2(&circle, &tri, &manifold)) {
      DrawIntersectManifold(circle, manifold, V3_GREEN);
    }
    if (Circle2IntersectLine2(&circle, &line, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Circle2IntersectRay2(&circle, &ray, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Circle2IntersectConvexHull2(&circle, &hull, &manifold)) {
      DrawIntersectManifold(circle, manifold, V3_GREEN);
    }

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
