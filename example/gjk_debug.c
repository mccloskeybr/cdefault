#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

#define OBJ_MOVE_SPEED 5.0f
#define GJK_POINT_COLOR V3_RED
#define OBJ_OFFSET (V3){0, +3, -20}
#define GJK_OFFSET (V3){0, -2, -15}

void DrawPoint(V3 pos, V3 offset, V3 color, B32 wireframe) {
  V3AddV3(&pos, &pos, &offset);
  DrawSphereV(pos, V4_QUAT_IDENT, 0.05f, color);
  if (wireframe) {
    RendererEnableWireframe();
    DrawSphereV(pos, V4_QUAT_IDENT, 0.05f, V3_BLACK);
    RendererDisableWireframe();
  }
}

void DrawLine(V3 start, V3 end, V3 offset, V3 color) {
  V3AddV3(&start, &start, &offset);
  V3AddV3(&end, &end, &offset);
  DrawLine3V(start, end, color);
}

void DrawObj(U32 handle, V3 center, V3 offset, V3 color, B32 wireframe) {
  V3AddV3(&center, &center, &offset);
  DrawModelColorV(handle, center, V4_QUAT_IDENT, V3_ONES, color);
  if (wireframe) {
    RendererEnableWireframe();
    DrawModelColorV(handle, center, V4_QUAT_IDENT, V3_ONES, V3_BLACK);
    RendererDisableWireframe();
  }
}

void DrawPointCloud(V3* points, U32 points_size, V3 offset, V3 color, B32 wireframe) {
  for (U32 i = 0; i < points_size; i++) { DrawPoint(points[i], offset, color, wireframe); }
}

int main(void) {
  TimeInit();
  DirSetCurrentToExeDir();
  ASSERT(WindowInit(1280, 720, "physics"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  Arena* arena = ArenaAllocate();

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  Model a_obj, b_obj;
  DEBUG_ASSERT(ModelLoadFile(arena, &a_obj, Str8Lit("../data/sphere.obj")));
  U32 a_mesh_handle = RendererRegisterModel(&a_obj);
  DEBUG_ASSERT(ModelLoadFile(arena, &b_obj, Str8Lit("../data/cube.obj")));
  U32 b_mesh_handle = RendererRegisterModel(&b_obj);
  DEBUG_ASSERT(a_obj.meshes->next == NULL);
  DEBUG_ASSERT(b_obj.meshes->next == NULL);

  V3  a_center = V3Assign(-5, 0, 0);
  V3  b_center = V3Assign(+5, 0, 0);
  U32 a_points_size = a_obj.meshes->vertices_size;
  U32 b_points_size = b_obj.meshes->vertices_size;
  V3* a_points = ARENA_PUSH_ARRAY(arena, V3, a_points_size);
  V3* b_points = ARENA_PUSH_ARRAY(arena, V3, b_points_size);
  U32 minkowski_diff_points_size = a_points_size * b_points_size;
  V3* minkowski_diff_points = ARENA_PUSH_ARRAY(arena, V3, minkowski_diff_points_size);

  while(!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    // NOTE: move obj a based on keyboard input
    if (WindowIsKeyPressed(Key_W)) {
      a_center.z -= OBJ_MOVE_SPEED * dt_s;
    } else if (WindowIsKeyPressed(Key_S)) {
      a_center.z += OBJ_MOVE_SPEED * dt_s;
    }
    if (WindowIsKeyPressed(Key_A)) {
      a_center.x -= OBJ_MOVE_SPEED * dt_s;
    } else if (WindowIsKeyPressed(Key_D)) {
      a_center.x += OBJ_MOVE_SPEED * dt_s;
    }
    if (WindowIsKeyPressed(Key_Shift)) {
      a_center.y += OBJ_MOVE_SPEED * dt_s;
    } else if (WindowIsKeyPressed(Key_Control)) {
      a_center.y -= OBJ_MOVE_SPEED * dt_s;
    }

    // NOTE: update points
    for (U32 i = 0; i < a_points_size; i++) { V3AddV3(&a_points[i], &a_obj.meshes->points[i], &a_center); }
    for (U32 i = 0; i < b_points_size; i++) { V3AddV3(&b_points[i], &b_obj.meshes->points[i], &b_center); }
    for (U32 i = 0; i < a_points_size; i++) {
      for (U32 j = 0; j < b_points_size; j++) {
        V3SubV3(&minkowski_diff_points[(i * b_points_size) + j], &a_points[i], &b_points[j]);
      }
    }

    IntersectManifold3 manifold;
    if (ConvexHull3IntersectConvexHull3(a_points, a_points_size, b_points, b_points_size, &manifold)) {
      V3 manifold_end;
      V3MultF32(&manifold_end, &manifold.normal, manifold.penetration);
      DrawLine(V3_ZEROES, manifold_end, GJK_OFFSET, V3_BLUE);
    }

    // NOTE: render
    DrawObj(a_mesh_handle, a_center, OBJ_OFFSET, V3_RED, true);
    DrawObj(b_mesh_handle, b_center, OBJ_OFFSET, V3_BLUE, true);
    DrawPointCloud(minkowski_diff_points, minkowski_diff_points_size, GJK_OFFSET, V3_GREEN, true);
    DrawPoint(V3Assign(0, 0, 0), GJK_OFFSET, V3_WHITE, true);

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);
    WindowSwapBuffers();
    WindowFlushEvents();
  }

  ArenaRelease(arena);
  return 0;
}
