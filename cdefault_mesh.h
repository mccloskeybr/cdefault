#ifndef CDEFAULT_MESH_H_
#define CDEFAULT_MESH_H_

#include "cdefault_std.h"
#include "cdefault_math.h"

typedef struct Mesh Mesh;
struct Mesh {
  V3*  points;
  U32  points_size;
  V3*  normals;
  U32  normals_size;
  V2*  uvs;
  U32  uvs_size;
};

B32 MeshLoad(Arena* arena, Mesh* mesh, U8* file_data, U32 file_data_size);

#endif // CDEFAULT_MESH_H_

#ifdef CDEFAULT_MESH_IMPLEMENTATION
#undef CDEFAULT_MESH_IMPLEMENTATION

// todo
// just load the points normals uvs raw in order, do indexing in renderer
B32 MeshLoad(Arena* arena, Mesh* mesh, U8* file_data, U32 file_data_size) {
  MEMORY_ZERO_STRUCT(mesh);
  Arena* temp_arena = ArenaAllocate();
  String8 file_str = String8Create(file_data, file_data_size);
  String8List file_str_parts = String8Split(temp_arena, &file_str, '\n');
  for (String8ListNode* node = file_str_parts.head; node != NULL; node = node->next) {
    if (String8StartsWith(&node->string, String8CreateCString("v "))) {
      mesh->points_size++;
    } else if (String8StartsWith(&node->string, String8CreateCString("vn "))) {
      mesh->normals_size++;
    } else if (String8StartsWith(&node->string, String8CreateCString("vt "))) {
      mesh->uvs_size++;
    }
  }
  V3* points  = ARENA_PUSH_ARRAY(arena, V3, mesh->points_size);
  V3* normals = ARENA_PUSH_ARRAY(arena, V3, mesh->normals_size);
  V2* uvs     = ARENA_PUSH_ARRAY(arena, V2, mesh->uvs_size);
  U32 points_i = 0;
  U32 normals_i = 0;
  U32 uvs_i = 0;
  for (String8ListNode* node = file_str_parts.head; node != NULL; node = node->next) {
    String8List line_parts = String8Split(temp_arena, &node->string, String8CreateCString(" "));
    if (String8StartsWith(&node->string, String8CreateCString("v "))) {
    } else if (String8StartsWith(&node->string, String8CreateCString("vn "))) {
    } else if (String8StartsWith(&node->string, String8CreateCString("vt "))) {
    } else if (String8StartsWith(&node->string, String8CreateCString("f "))) {
    }
  }
  ArenaRelease(temp_arena);
}

#endif // CDEFAULT_MESH_IMPLEMENTATION
