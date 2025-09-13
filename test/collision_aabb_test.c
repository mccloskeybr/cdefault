#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "../cdefault_geometry.h"

static void DrawIntersectManifold(Aabb2 aabb, IntersectManifold2 manifold, V3 color) {
  Line2 pen;
  pen.start = aabb.center_point;
  V2MultF32(&pen.end, &manifold.normal, manifold.penetration);
  V2AddV2(&pen.end, &pen.start, &pen.end);
  DrawLineV(pen.start, pen.end, 5, color);
}

int main(void) {
  TimeInit();
  ASSERT(WindowInit(1280, 720, "collision"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  Aabb2 aabb;
  aabb.center_point.x = 0;
  aabb.center_point.y = 0;
  aabb.size.x = 50;
  aabb.size.y = 75;

  Circle2 circle;
  circle.center_point.x = 200;
  circle.center_point.y = 300;
  circle.radius = 100;

  Aabb2 other_aabb;
  other_aabb.center_point.x = 500;
  other_aabb.center_point.y = 300;
  other_aabb.size.x = 200;
  other_aabb.size.y = 100;

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

  V2 other_hull_points[6] = {
    {1100, 250},
    {1150, 250},
    {1200, 300},
    {1150, 350},
    {1100, 350},
    {1050, 300},
  };
  ConvexHull2 hull;
  hull.points = (V2*) other_hull_points;
  hull.points_len = 6;

  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &mouse_pos);
    aabb.center_point = mouse_pos;

    DrawCircleV(circle.center_point, circle.radius, V3_BLUE);
    DrawRectangleV(other_aabb.center_point, other_aabb.size, V3_BLUE);
    DrawTriangleV(tri.points[0], tri.points[1], tri.points[2], V3_BLUE);
    DrawLineV(line.start, line.end, 5, V3_BLUE);

    V2 ray_end;
    V2MultF32(&ray_end, &ray.dir, 5000);
    V2AddV2(&ray_end, &ray.start, &ray_end);
    DrawLineV(ray.start, ray_end, 5, V3_BLUE);

    DrawConvexHullV(hull.points, hull.points_len, V3_BLUE);

    DrawRectangleV(aabb.center_point, aabb.size, V3_WHITE);

    V2 enter, exit;
    IntersectManifold2 manifold;
    if (Aabb2IntersectCircle2(&aabb, &circle, &manifold)) {
      DrawIntersectManifold(aabb, manifold, V3_GREEN);
    }
    if (Aabb2IntersectAabb2(&aabb, &other_aabb, &manifold)) {
      DrawIntersectManifold(aabb, manifold, V3_GREEN);
    }
    if (Aabb2IntersectTri2(&aabb, &tri, &manifold)) {
      DrawIntersectManifold(aabb, manifold, V3_GREEN);
    }
    if (Aabb2IntersectLine2(&aabb, &line, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Aabb2IntersectRay2(&aabb, &ray, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Aabb2IntersectConvexHull2(&aabb, &hull, &manifold)) {
      DrawIntersectManifold(aabb, manifold, V3_GREEN);
    }

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
