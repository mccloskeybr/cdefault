#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "../cdefault_geometry.h"

static void DrawIntersectManifold(V2* start, IntersectManifold2 manifold, V3 color) {
  V2 end;
  V2MultF32(&end, &manifold.normal, manifold.penetration);
  V2AddV2(&end, start, &end);
  DrawLineV(*start, end, 5, color);
  for (U32 i = 0; i < manifold.contact_points_size; i++) {
    DrawRingV(manifold.contact_points[i], 10, 5, V3_RED);
  }
}

int main(void) {
  TimeInit();
  ASSERT(WindowInit(1280, 720, "collision"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  V2 aabb_center, aabb_size;
  aabb_center.x = 0;
  aabb_center.y = 0;
  aabb_size.x = 50;
  aabb_size.y = 75;

  V2 circle_center;
  F32 circle_radius;
  circle_center.x = 200;
  circle_center.y = 300;
  circle_radius = 100;

  V2 other_aabb_center, other_aabb_size;
  other_aabb_center.x = 500;
  other_aabb_center.y = 300;
  other_aabb_size.x = 200;
  other_aabb_size.y = 100;

  V2 obb_center, obb_size;
  F32 obb_angle_rad;
  obb_center.x = 500;
  obb_center.y = 500;
  obb_size.x = 100;
  obb_size.y = 50;
  obb_angle_rad = F32_PI / 4.0f;

  V2 tri_points[3];
  tri_points[0] = (V2) { 700, 250 };
  tri_points[1] = (V2) { 800, 250 };
  tri_points[2] = (V2) { 750, 400 };

  V2 line_start, line_end;
  line_start.x = 850;
  line_start.y = 200;
  line_end.x = 950;
  line_end.y = 400;

  V2 ray_start, ray_dir;
  ray_start.x = 1000;
  ray_start.y = 200;
  ray_dir.x = 0;
  ray_dir.y = 1;

  V2 hull_points[6] = {
    {1100, 250},
    {1150, 250},
    {1200, 300},
    {1150, 350},
    {1100, 350},
    {1050, 300},
  };

  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &mouse_pos);
    aabb_center = mouse_pos;

    DrawCircleV(circle_center, circle_radius, V3_BLUE);
    DrawRectangleV(other_aabb_center, other_aabb_size, V3_BLUE);
    DrawTriangleV(tri_points[0], tri_points[1], tri_points[2], V3_BLUE);
    DrawLineV(line_start, line_end, 5, V3_BLUE);
    DrawConvexHullV(hull_points, STATIC_ARRAY_SIZE(hull_points), V3_BLUE);

    obb_angle_rad += 0.05f;
    V2 obb_hull_points[4];
    ConvexHull2FromObb2(obb_hull_points, &obb_center, &obb_size, obb_angle_rad);
    DrawConvexHullV(obb_hull_points, STATIC_ARRAY_SIZE(obb_hull_points), V3_BLUE);

    V2 ray_end;
    V2MultF32(&ray_end, &ray_dir, 5000);
    V2AddV2(&ray_end, &ray_start, &ray_end);
    DrawLineV(ray_start, ray_end, 5, V3_BLUE);

    DrawRectangleV(aabb_center, aabb_size, V3_WHITE);

    V2 enter, exit;
    IntersectManifold2 manifold;
    if (Aabb2IntersectCircle2(&aabb_center, &aabb_size, &circle_center, circle_radius, &manifold)) {
      DrawIntersectManifold(&aabb_center, manifold, V3_GREEN);
    }
    if (Aabb2IntersectAabb2(&aabb_center, &aabb_size, &other_aabb_center, &other_aabb_size, &manifold)) {
      DrawIntersectManifold(&aabb_center, manifold, V3_GREEN);
    }
    if (Aabb2IntersectObb2(&aabb_center, &aabb_size, &obb_center, &obb_size, obb_angle_rad, &manifold)) {
      DrawIntersectManifold(&aabb_center, manifold, V3_GREEN);
    }
    if (Aabb2IntersectTri2(&aabb_center, &aabb_size, tri_points, &manifold)) {
      DrawIntersectManifold(&aabb_center, manifold, V3_GREEN);
    }
    if (Aabb2IntersectLine2(&aabb_center, &aabb_size, &line_start, &line_end, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Aabb2IntersectRay2(&aabb_center, &aabb_size, &ray_start, &ray_dir, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Aabb2IntersectConvexHull2(&aabb_center, &aabb_size, hull_points, STATIC_ARRAY_SIZE(hull_points), &manifold)) {
      DrawIntersectManifold(&aabb_center, manifold, V3_GREEN);
    }

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
