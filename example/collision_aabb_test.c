#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

static void DrawIntersectManifold(V2 start, IntersectManifold2 manifold, V3 color) {
  V2 end = V2AddV2(start, V2MultF32(manifold.normal, manifold.penetration));
  DrawLineV(start, end, 5, color);
  for (U32 i = 0; i < manifold.contact_points_size; i++) {
    DrawRingV(manifold.contact_points[i], 10, 5, V3_RED);
  }
}

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  TimeInit();
  ASSERT(WindowInit(1280, 720, "collision"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  RendererDisableDepthTest();

  V2 aabb_center = V2Assign(0, 0);
  V2 aabb_size   = V2Assign(50, 75);

  V2 circle_center  = V2Assign(200, 300);
  F32 circle_radius = 100;

  V2 other_aabb_center = V2Assign(500, 300);
  V2 other_aabb_size   = V2Assign(200, 100);

  V2 obb_center = V2Assign(500, 500);
  V2 obb_size   = V2Assign(100, 50);
  F32 obb_angle_rad = F32_PI / 2.0f;

  V2 tri_points[3];
  tri_points[0] = V2Assign(700, 250);
  tri_points[1] = V2Assign(800, 250);
  tri_points[2] = V2Assign(750, 400);

  V2 line_start = V2Assign(850, 200);
  V2 line_end   = V2Assign(950, 400);

  V2 ray_start = V2Assign(1000, 200);
  V2 ray_dir   = V2Assign(0, 1);

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
    aabb_center = mouse_pos;

    DrawCircleV(circle_center, circle_radius, V3_BLUE);
    DrawRectangleV(other_aabb_center, other_aabb_size, V3_BLUE);
    DrawTriangleV(tri_points[0], tri_points[1], tri_points[2], V3_BLUE);
    DrawLineV(line_start, line_end, 5, V3_BLUE);
    DrawConvexHullV(hull_points, STATIC_ARRAY_SIZE(hull_points), V3_BLUE);

    obb_angle_rad += 0.05f;
    V2 obb_hull_points[4];
    ConvexHull2FromObb2(obb_hull_points, obb_center, obb_size, obb_angle_rad);
    DrawConvexHullV(obb_hull_points, STATIC_ARRAY_SIZE(obb_hull_points), V3_BLUE);

    V2 ray_end = V2AddV2(ray_start, V2MultF32(ray_dir, 5000));
    DrawLineV(ray_start, ray_end, 5, V3_BLUE);

    DrawRectangleV(aabb_center, aabb_size, V3_WHITE);

    V2 enter, exit;
    IntersectManifold2 manifold;
    if (Aabb2IntersectCircle2(aabb_center, aabb_size, circle_center, circle_radius, &manifold)) {
      DrawIntersectManifold(aabb_center, manifold, V3_GREEN);
    }
    if (Aabb2IntersectAabb2(aabb_center, aabb_size, other_aabb_center, other_aabb_size, &manifold)) {
      DrawIntersectManifold(aabb_center, manifold, V3_GREEN);
    }
    if (Aabb2IntersectObb2(aabb_center, aabb_size, obb_center, obb_size, obb_angle_rad, &manifold)) {
      DrawIntersectManifold(aabb_center, manifold, V3_GREEN);
    }
    if (Aabb2IntersectTri2(aabb_center, aabb_size, tri_points, &manifold)) {
      DrawIntersectManifold(aabb_center, manifold, V3_GREEN);
    }
    if (Aabb2IntersectLine2(aabb_center, aabb_size, line_start, line_end, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Aabb2IntersectRay2(aabb_center, aabb_size, ray_start, ray_dir, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Aabb2IntersectConvexHull2(aabb_center, aabb_size, hull_points, STATIC_ARRAY_SIZE(hull_points), &manifold)) {
      DrawIntersectManifold(aabb_center, manifold, V3_GREEN);
    }

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
