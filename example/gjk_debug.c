#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

#define OBJ_MOVE_SPEED 5.0f
#define GJK_POINT_COLOR V3_RED
#define OBJ_OFFSET (V3){0, +3, -20}
#define GJK_OFFSET (V3){0, -3, -20}

void DrawPoint(V3 pos, V3 offset, V3 color) {
  V3AddV3(&pos, &pos, &offset);
  DrawSphereV(pos, V4_QUAT_IDENT, 0.1f, color);
  RendererEnableWireframe();
  DrawSphereV(pos, V4_QUAT_IDENT, 0.1f, V3_BLACK);
  RendererDisableWireframe();
}

void DrawLine(V3 start, V3 end, V3 offset, V3 color) {
  V3AddV3(&start, &start, &offset);
  V3AddV3(&end, &end, &offset);
  DrawLine3V(start, end, color);
}

void DrawObj(U32 handle, V3 center, V3 offset, V3 color) {
  V3AddV3(&center, &center, &offset);
  DrawMeshColorV(handle, center, V4_QUAT_IDENT, V3_ONES, color);
  RendererEnableWireframe();
  DrawMeshColorV(handle, center, V4_QUAT_IDENT, V3_ONES, V3_BLACK);
  RendererDisableWireframe();
}

void DrawPointCloud(V3* points, U32 points_size, V3 offset, V3 color) {
  for (U32 i = 0; i < points_size; i++) { DrawPoint(points[i], offset, color); }
}

static V3 TEST_GjkSupport(V3* a_points, U32 a_points_size, V3* b_points, U32 b_points_size, V3 d) {
  DEBUG_ASSERT(a_points_size >= 3);
  DEBUG_ASSERT(b_points_size >= 3);

  V3* a_max_point     = NULL;
  F32 a_max_point_dot = F32_MIN;
  for (U32 i = 0; i < a_points_size; i++) {
    V3* test     = &a_points[i];
    F32 test_dot = V3DotV3(test, &d);
    if (test_dot > a_max_point_dot) {
      a_max_point     = test;
      a_max_point_dot = test_dot;
    }
  }

  V3* b_min_point     = NULL;
  F32 b_min_point_dot = F32_MAX;
  for (U32 i = 0; i < b_points_size; i++) {
    V3* test     = &b_points[i];
    F32 test_dot = V3DotV3(test, &d);
    if (test_dot < b_min_point_dot) {
      b_min_point     = test;
      b_min_point_dot = test_dot;
    }
  }

  V3 result;
  V3SubV3(&result, a_max_point, b_min_point);
  return result;
}

void TEST_ConvexHull3IntersectConvexHull3(V3* a_points, U32 a_points_size, V3* b_points, U32 b_points_size, IntersectManifold3* manifold) {
  DEBUG_ASSERT(a_points_size >= 3);
  DEBUG_ASSERT(b_points_size >= 3);

  // NOTE: determine intersection using GJK
  V3  simplex_points[4];
  U32 simplex_points_size = 0;
  MEMORY_ZERO_STATIC_ARRAY(simplex_points);
  V3 dir = V3_X_POS;
  simplex_points[simplex_points_size++] = TEST_GjkSupport(a_points, a_points_size, b_points, b_points_size, dir);
  V3MultF32(&dir, &dir, -1);

  while (true) {
    V3 test = TEST_GjkSupport(a_points, a_points_size, b_points, b_points_size, dir);
    if (V3DotV3(&test, &dir) < 0) { return; }
    simplex_points[simplex_points_size++] = test;
    switch (simplex_points_size) {
      case 2: {
        // NOTE: next direction should be from line to origin
        V3 ba, oa, cross;
        V3SubV3(&ba, &simplex_points[1], &simplex_points[0]);
        V3MultF32(&oa, &simplex_points[0], -1);
        V3CrossV3(&cross, &ba, &oa);
        V3CrossV3(&dir, &cross, &dir);
      } break;
      case 3: {
        // NOTE: next direction should be triangle normal
        V3 ba, ca, oa;
        V3SubV3(&ba, &simplex_points[1], &simplex_points[0]);
        V3SubV3(&ca, &simplex_points[2], &simplex_points[0]);
        V3CrossV3(&dir, &ca, &ba);
        V3MultF32(&oa, &simplex_points[0], -1);
        if (V3DotV3(&dir, &oa) < 0) { V3MultF32(&dir, &dir, -1); }
      } break;
      case 4: {
        // NOTE: determine if any polyhedra face normal excludes the origin,
        // use it to iterate / find the next direction to travel. if all include
        // origin, then we've found an intersection.
        V3 da, db, dc, od;
        V3SubV3(&da, &simplex_points[3], &simplex_points[0]);
        V3SubV3(&db, &simplex_points[3], &simplex_points[1]);
        V3SubV3(&dc, &simplex_points[3], &simplex_points[2]);
        V3MultF32(&od, &simplex_points[3], -1);

        V3 abd_norm, bcd_norm, cad_norm;
        V3CrossV3(&abd_norm, &da, &db);
        V3CrossV3(&bcd_norm, &db, &dc);
        V3CrossV3(&cad_norm, &dc, &da);

        if (V3DotV3(&abd_norm, &od) > 0) {
          DA_SWAP_REMOVE_EX(simplex_points, simplex_points_size, 2);
          dir = abd_norm;
        } else if (V3DotV3(&bcd_norm, &od) > 0) {
          DA_SWAP_REMOVE_EX(simplex_points, simplex_points_size, 0);
          dir = bcd_norm;
        } else if (V3DotV3(&cad_norm, &od) > 0) {
          DA_SWAP_REMOVE_EX(simplex_points, simplex_points_size, 1);
          dir = cad_norm;
        } else {
          goto gjk_found_intersection;
        }
      } break;
      default: UNREACHABLE();
    }
  }
gjk_found_intersection:
  for (U32 i = 0; i < simplex_points_size; i++) {
    DrawPoint(simplex_points[i], GJK_OFFSET, GJK_POINT_COLOR);
    for (U32 j = 0; j < simplex_points_size; j++) {
      DrawLine(simplex_points[i], simplex_points[j], GJK_OFFSET, GJK_POINT_COLOR);
    }
  }

  // NOTE: determine manifold penetration and normal using EPA
  if (manifold == NULL) { return; }
  DEBUG_ASSERT(simplex_points_size == 4);
  manifold->normal      = V3_Z_POS;
  manifold->penetration = 0;

  Arena* temp_arena        = ArenaAllocate();
  V3* epa_simplex          = NULL;
  U32 epa_simplex_size     = 0;
  U32 epa_simplex_capacity = 0;
  DA_COPY_EX(temp_arena, simplex_points, simplex_points_size, 4, epa_simplex, epa_simplex_size, epa_simplex_capacity);

  // TODO: proper 3D EPA finds distance using faces, not edges... this goes into inf loop sometimes...
  while (true) {
    // NOTE: find closest edge in simplex to origin
    V3  closest_norm;
    U32 closest_idx  = 0;
    F32 closest_dist = F32_MAX;
    for (U32 i = 0; i < epa_simplex_size; i++) {
      U32 j = (i + 1) % epa_simplex_size;
      V3* a = &epa_simplex[i];
      V3* b = &epa_simplex[j];

      V3 edge, cross, edge_origin;
      V3SubV3(&edge, b, a);
      V3CrossV3(&cross, &edge, a);
      V3CrossV3(&edge_origin, &cross, &edge);

      // NOTE: edge contains origin, e.g. if intersecting along face / edge / corner
      if (V3LengthSq(&edge_origin) == 0) { goto epa_found_manifold; }
      V3Normalize(&edge_origin, &edge_origin);

      F32 dot = V3DotV3(&edge_origin, a);
      if (dot < closest_dist) {
        closest_dist = dot;
        closest_norm = edge_origin;
        closest_idx  = j;
      }
    }

    V3  test = TEST_GjkSupport(a_points, a_points_size, b_points, b_points_size, closest_norm);
    F32 dist = V3DotV3(&test, &closest_norm);
    if (F32ApproxEq(dist, closest_dist)) {
      manifold->normal      = closest_norm;
      manifold->penetration = dist;
      goto epa_found_manifold;
    }
    DA_INSERT_EX(temp_arena, epa_simplex, epa_simplex_size, epa_simplex_capacity, test, closest_idx);
  }
epa_found_manifold:
  ArenaRelease(temp_arena);
  return;
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

  U32 a_mesh_handle, b_mesh_handle;
  Mesh a_obj, b_obj;
  DEBUG_ASSERT(MeshLoadFile(arena, &a_obj, Str8Lit("../data/cube.obj")));
  RendererRegisterMesh(&a_mesh_handle, 0, a_obj.points, a_obj.normals, a_obj.uvs, a_obj.vertices_size, a_obj.indices, a_obj.indices_size);
  DEBUG_ASSERT(MeshLoadFile(arena, &b_obj, Str8Lit("../data/cube.obj")));
  RendererRegisterMesh(&b_mesh_handle, 0, b_obj.points, b_obj.normals, b_obj.uvs, b_obj.vertices_size, b_obj.indices, b_obj.indices_size);

  V3  a_center = V3Assign(-5, 0, 0);
  V3  b_center = V3Assign(+5, 0, 0);
  U32 a_points_size = a_obj.vertices_size;
  U32 b_points_size = b_obj.vertices_size;
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
    for (U32 i = 0; i < a_points_size; i++) { V3AddV3(&a_points[i], &a_obj.points[i], &a_center); }
    for (U32 i = 0; i < b_points_size; i++) { V3AddV3(&b_points[i], &b_obj.points[i], &b_center); }
    for (U32 i = 0; i < a_points_size; i++) {
      for (U32 j = 0; j < b_points_size; j++) {
        V3SubV3(&minkowski_diff_points[(i * a_points_size) + j], &a_points[i], &b_points[j]);
      }
    }

    TEST_ConvexHull3IntersectConvexHull3(a_points, a_points_size, b_points, b_points_size, NULL);

    // NOTE: render
    DrawObj(a_mesh_handle, a_center, OBJ_OFFSET, V3_RED);
    DrawObj(b_mesh_handle, b_center, OBJ_OFFSET, V3_BLUE);
    DrawPointCloud(minkowski_diff_points, minkowski_diff_points_size, GJK_OFFSET, V3_GREEN);
    DrawPoint(V3Assign(0, 0, 0), GJK_OFFSET, V3_WHITE);

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);
    WindowSwapBuffers();
    WindowFlushEvents();
  }

  ArenaRelease(arena);
  return 0;
}
