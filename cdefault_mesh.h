#ifndef CDEFAULT_MESH_H_
#define CDEFAULT_MESH_H_

#include "cdefault_std.h"
#include "cdefault_math.h"

typedef struct Mesh Mesh;
struct Mesh {
  // NOTE: may be null if not present.
  V3*  points;
  V3*  normals;
  V2*  uvs;
  U32  vertices_size;

  U32* indices;
  U32  indices_size;
};

B32 MeshLoadFile(Arena* arena, Mesh* mesh, U8* file_path);
B32 MeshLoad(Arena* arena, Mesh* mesh, U8* file_data, U32 file_data_size);

#endif // CDEFAULT_MESH_H_

#ifdef CDEFAULT_MESH_IMPLEMENTATION
#undef CDEFAULT_MESH_IMPLEMENTATION

static B32 MeshLoadObj(Arena* arena, Mesh* mesh, U8* file_data, U32 file_data_size) {
  MEMORY_ZERO_STRUCT(mesh);
  Arena* temp_arena = ArenaAllocate();
  String8 file_str = Str8(file_data, file_data_size);
  String8List lines = Str8Split(temp_arena, file_str, '\n');
  U64 arena_pos = ArenaPos(arena);
  B32 success = false;

  // NOTE: determine sizes.
  S32 obj_points_size   = 0;
  S32 obj_normals_size  = 0;
  S32 obj_uvs_size      = 0;
  U32 vertices_size_estimate = 0;
  for (String8ListNode* line = lines.head; line != NULL; line = line->next) {
    if      (Str8StartsWith(line->string, Str8Lit("v ")))  { obj_points_size++;           }
    else if (Str8StartsWith(line->string, Str8Lit("vn "))) { obj_normals_size++;          }
    else if (Str8StartsWith(line->string, Str8Lit("vt "))) { obj_uvs_size++;              }
    else if (Str8StartsWith(line->string, Str8Lit("f ")))  { vertices_size_estimate += 3; }
  }
  if (obj_points_size == 0) { goto mesh_load_obj_end; }

  // NTOE: load compressed vertex data.
  V3* obj_points      = ARENA_PUSH_ARRAY(temp_arena, V3, obj_points_size);
  V3* obj_normals     = ARENA_PUSH_ARRAY(temp_arena, V3, obj_normals_size);
  V2* obj_uvs         = ARENA_PUSH_ARRAY(temp_arena, V2, obj_uvs_size);
  S32 obj_points_idx  = 0;
  S32 obj_normals_idx = 0;
  S32 obj_uvs_idx     = 0;
#define NEXT_F32(f, node) \
    node = node->next;    \
    if (node == NULL) {   \
      LOG_ERROR("[MESH] OBJ file malformed; expected string where there was none. For line: %.*s", line->string.size, line->string.str); \
      goto mesh_load_obj_end; \
    }                     \
    Str8ToF32(node->string, &f);
  for (String8ListNode* line = lines.head; line != NULL; line = line->next) {
    String8List line_parts = Str8Split(temp_arena, line->string, ' ');
    String8ListNode* line_part = line_parts.head;
    if (Str8Eq(line_part->string, Str8Lit("v"))) {
      V3 point;
      NEXT_F32(point.x, line_part);
      NEXT_F32(point.y, line_part);
      NEXT_F32(point.z, line_part);
      obj_points[obj_points_idx++] = point;

    } else if (Str8Eq(line_part->string, Str8Lit("vn"))) {
      V3 normal;
      NEXT_F32(normal.x, line_part);
      NEXT_F32(normal.y, line_part);
      NEXT_F32(normal.z, line_part);
      obj_normals[obj_normals_idx++] = normal;

    } else if (Str8Eq(line_part->string, Str8Lit("vt"))) {
      V2 uv;
      NEXT_F32(uv.u, line_part);
      NEXT_F32(uv.v, line_part);
      if (line_part->next != NULL) {
        LOG_ERROR("[MESH] OBJ importer does not support 3-part texture coordinates. For line: %.*s", line->string.size, line->string.str);
        goto mesh_load_obj_end;
      }
      obj_uvs[obj_uvs_idx++] = uv;
    }
  }
  DEBUG_ASSERT(obj_points_idx == obj_points_size && obj_normals_idx == obj_normals_size && obj_uvs_idx == obj_uvs_size);
#undef NEXT_F32

  // NOTE: determine vertex data
  mesh->indices = ARENA_PUSH_ARRAY(arena, U32, vertices_size_estimate);
  mesh->points  = ARENA_PUSH_ARRAY(arena, V3, vertices_size_estimate);
  if (obj_normals_size > 0) { mesh->normals = ARENA_PUSH_ARRAY(arena, V3, vertices_size_estimate); }
  if (obj_uvs_size > 0)     { mesh->uvs = ARENA_PUSH_ARRAY(arena, V2, vertices_size_estimate); }
  for (String8ListNode* line = lines.head; line != NULL; line = line->next) {
    if (!Str8StartsWith(line->string, Str8Lit("f "))) { continue; }
    String8List line_parts = Str8Split(temp_arena, line->string, ' ');
    U32 face_vertices_size = 0;
    for (String8ListNode* line_part = line_parts.head->next; line_part != NULL; line_part = line_part->next) {
      String8List face_parts = Str8Split(temp_arena, line_part->string, '/');
      DEBUG_ASSERT(face_parts.head != NULL);

      S32 point_idx  = -1;
      S32 uv_idx     = -1;
      S32 normal_idx = -1;
      String8ListNode* face_part = face_parts.head;;
      Str8ToS32(face_part->string, &point_idx);
      point_idx -= 1;
      face_part = face_part->next;
      if (face_part != NULL) {
        if (face_part->string.size > 0) {
          Str8ToS32(face_part->string, &uv_idx);
          uv_idx -= 1;
        }
        face_part = face_part->next;
        if (face_part != NULL) {
          Str8ToS32(face_part->string, &normal_idx);
          normal_idx -= 1;
        }
      }
      DEBUG_ASSERT(point_idx < obj_points_size && normal_idx < obj_normals_size && uv_idx < obj_uvs_size);

      S32 duplicate_idx = -1;
      for (U32 i = 0; i < mesh->vertices_size; i++) {
        if (V3Eq(&mesh->points[i], &obj_points[point_idx])) {
          if (normal_idx == -1 || V3Eq(&mesh->normals[i], &obj_normals[normal_idx])) {
            if (uv_idx == -1 || V2Eq(&mesh->uvs[i], &obj_uvs[uv_idx])) {
              duplicate_idx = i;
              break;
            }
          }
        }
      }

      if (duplicate_idx == -1) {
        mesh->points[mesh->vertices_size] = obj_points[point_idx];
        if (normal_idx != -1) { mesh->normals[mesh->vertices_size] = obj_normals[normal_idx]; }
        if (uv_idx != -1)     { mesh->uvs[mesh->vertices_size] = obj_uvs[uv_idx];             }
        mesh->indices[mesh->indices_size] = mesh->vertices_size;
        mesh->vertices_size++;
      } else {
        mesh->indices[mesh->indices_size] = duplicate_idx;
      }
      mesh->indices_size++;

      if (face_vertices_size++ > 3) {
        LOG_ERROR("[MESH] OBJ importer does not support defining more than 3 vertices per face face. For line: %.*s", line->string.size, line->string.str);
        goto mesh_load_obj_end;
      }
    }
  }

  // NOTE: re-fit data allocations with final sizes
  DEBUG_ASSERT(mesh->indices_size == vertices_size_estimate);
  U32 vertex_overfit = vertices_size_estimate - mesh->vertices_size;
  if (vertex_overfit > 0) {
    U32 to_pop = vertex_overfit * sizeof(V3); // for points (always present)
    if (obj_normals_size > 0) {
      V3* new_normals = (V3*) mesh->points + mesh->vertices_size;
      MEMORY_MOVE(new_normals, mesh->normals, mesh->vertices_size * sizeof(V3));
      mesh->normals = new_normals;
      to_pop += vertex_overfit * sizeof(V3);
    }
    if (obj_uvs_size > 0) {
      V2* new_uvs;
      if (obj_normals_size > 0) { new_uvs = (V2*) (mesh->normals + mesh->vertices_size); }
      else                      { new_uvs = (V2*) (mesh->points + mesh->vertices_size);  }
      MEMORY_MOVE(new_uvs, mesh->uvs, mesh->vertices_size * sizeof(V2));
      mesh->uvs = new_uvs;
      to_pop += vertex_overfit * sizeof(V2);
    }
    ArenaPop(arena, vertex_overfit);
  }

  success = true;
mesh_load_obj_end:
  ArenaRelease(temp_arena);
  if (!success) { ArenaPopTo(arena, arena_pos); }
  return success;
}

B32 MeshLoad(Arena* arena, Mesh* mesh, U8* file_data, U32 file_data_size) {
  if (MeshLoadObj(arena, mesh, file_data, file_data_size)) { return true; }
  return false;
}

B32 MeshLoadFile(Arena* arena, Mesh* mesh, U8* file_path) {
  B32 success = false;
  U8* file_data;
  U32 file_data_size;
  Arena* file_arena = ArenaAllocate();
  if (!FileReadAll(file_arena, file_path, &file_data, &file_data_size, false)) { goto mesh_load_file_exit; }
  success = MeshLoad(arena, mesh, file_data, file_data_size);
mesh_load_file_exit:
  ArenaRelease(file_arena);
  return success;
}

#endif // CDEFAULT_MESH_IMPLEMENTATION
