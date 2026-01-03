#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  TimeInit();
  ASSERT(WindowInit(1280, 720, "collision"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  V2 line_start, line_end;
  line_start.x = 0;
  line_start.y = 1;
  line_end.x = 0;
  line_end.y = 0;

  V2 circle_center;
  F32 circle_radius;
  circle_center.x = 200;
  circle_center.y = 300;
  circle_radius = 100;

  V2 aabb_center, aabb_size;
  aabb_center.x = 500;
  aabb_center.y = 300;
  aabb_size.x = 200;
  aabb_size.y = 100;

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

  V2 other_line_start, other_line_end;
  other_line_start.x = 850;
  other_line_start.y = 200;
  other_line_end.x = 950;
  other_line_end.y = 400;

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

    if (WindowIsMouseButtonJustPressed(MouseButton_Left)) {
      line_start = mouse_pos;
    }
    line_end = mouse_pos;

    DrawCircleV(circle_center, circle_radius, V3_BLUE);
    DrawRectangleV(aabb_center, aabb_size, V3_BLUE);
    DrawTriangleV(tri_points[0], tri_points[1], tri_points[2], V3_BLUE);
    DrawLineV(other_line_start, other_line_end, 5, V3_BLUE);
    DrawConvexHullV(hull_points, STATIC_ARRAY_SIZE(hull_points), V3_BLUE);

    obb_angle_rad += 0.05f;
    V2 obb_hull[4];
    ConvexHull2FromObb2(obb_hull, &obb_center, &obb_size, obb_angle_rad);
    DrawConvexHullV(obb_hull, STATIC_ARRAY_SIZE(obb_hull), V3_BLUE);

    V2 ray_end;
    V2MultF32(&ray_end, &ray_dir, 5000);
    V2AddV2(&ray_end, &ray_start, &ray_end);
    DrawLineV(ray_start, ray_end, 5, V3_BLUE);

    DrawLineV(line_start, line_end, 5, V3_WHITE);

    V2 enter, exit;
    if (Line2IntersectCircle2(&line_start, &line_end, &circle_center, circle_radius, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Line2IntersectAabb2(&line_start, &line_end, &aabb_center, &aabb_size, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Line2IntersectObb2(&line_start, &line_end, &obb_center, &obb_size, obb_angle_rad, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Line2IntersectTri2(&line_start, &line_end, tri_points, &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }
    if (Line2IntersectLine2(&line_start, &line_end, &other_line_start, &other_line_end, &enter)) {
      DrawCircleV(enter, 10, V3_GREEN);
    }
    if (Line2IntersectRay2(&line_start, &line_end, &ray_start, &ray_dir, &enter)) {
      DrawCircleV(enter, 10, V3_GREEN);
    }
    if (Line2IntersectConvexHull2(&line_start, &line_end, hull_points, STATIC_ARRAY_SIZE(hull_points), &enter, &exit)) {
      DrawCircleV(enter, 10, V3_GREEN);
      DrawRingV(exit, 10, 5, V3_RED);
    }

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
