#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  TimeInit();
  ASSERT(WindowInit(1280, 720, "collision"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  RendererDisableDepthTest();

  V2 ray_start = V2Assign(0, 0);
  V2 ray_dir   = V2Assign(0, 1);

  V2 circle_center  = V2Assign(200, 300);
  F32 circle_radius = 100;

  V2 aabb_center = V2Assign(500, 300);
  V2 aabb_size   = V2Assign(200, 100);

  V2 obb_center = V2Assign(500, 500);
  V2 obb_size   = V2Assign(100, 50);
  F32 obb_angle_rad = F32_PI / 2.0f;

  V2 tri_points[3];
  tri_points[0] = V2Assign(700, 250);
  tri_points[1] = V2Assign(800, 250);
  tri_points[2] = V2Assign(750, 400);

  V2 line_start = V2Assign(850, 200);
  V2 line_end   = V2Assign(950, 400);

  V2 other_ray_start = V2Assign(1000, 200);
  V2 other_ray_dir   = V2Assign(0, 1);

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

    V2 mouse_pos = WindowGetMousePositionV();
    if (WindowIsMouseButtonJustPressed(MouseButton_Left)) {
      ray_start = mouse_pos;
    }
    ray_dir = V2SubV2(mouse_pos, ray_start);
    if (V2LengthSq(ray_dir) == 0) {
      ray_dir.x = 0;
      ray_dir.y = 1;
    }
    ray_dir = V2Normalize(ray_dir);

    DrawCircleV(circle_center, circle_radius, V3_BLUE);
    DrawRectangleV(aabb_center, aabb_size, V3_BLUE);
    DrawTriangleV(tri_points[0], tri_points[1], tri_points[2], V3_BLUE);
    DrawLineV(line_start, line_end, 5, V3_BLUE);
    DrawConvexHullV(hull_points, STATIC_ARRAY_SIZE(hull_points), V3_BLUE);


    obb_angle_rad += 0.005f;
    V2 obb_hull[4];
    ConvexHull2FromObb2(obb_hull, obb_center, obb_size, obb_angle_rad);
    DrawConvexHullV(obb_hull, STATIC_ARRAY_SIZE(obb_hull), V3_BLUE);

    V2 other_ray_end = V2AddV2(other_ray_start, V2MultF32(other_ray_dir, 5000));
    DrawLineV(other_ray_start, other_ray_end, 5, V3_BLUE);

    V2 ray_end = V2AddV2(ray_start, V2MultF32(ray_dir, 5000));
    DrawLineV(ray_start, ray_end, 5, V3_WHITE);

    V2 enter, exit;
    if (Ray2IntersectCircle2(ray_start, ray_dir, circle_center, circle_radius, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Ray2IntersectAabb2(ray_start, ray_dir, aabb_center, aabb_size, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Ray2IntersectObb2(ray_start, ray_dir, obb_center, obb_size, obb_angle_rad, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Ray2IntersectTri2(ray_start, ray_dir, tri_points, V2_ZEROES, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Ray2IntersectLine2(ray_start, ray_dir, line_start, line_end, &enter)) {
      DrawCircleV(enter, 10, V3_GREEN);
    }
    if (Ray2IntersectRay2(ray_start, ray_dir, other_ray_start, other_ray_dir, &enter)) {
      DrawCircleV(enter, 10, V3_GREEN);
    }
    if (Ray2IntersectConvexHull2(ray_start, ray_dir, hull_points, STATIC_ARRAY_SIZE(hull_points), V2_ZEROES, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
