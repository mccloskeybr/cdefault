#ifndef CDEFAULT_MODEL_H_
#define CDEFAULT_MODEL_H_

#include "cdefault_std.h"
#include "cdefault_math.h"
#include "cdefault_image.h"
#include "cdefault_json.h"

typedef struct Mesh Mesh;
struct Mesh {
  Mesh* next;

  // NOTE: may be null if not present.
  V3*  points;
  V3*  normals;
  V2*  uvs;
  U32  vertices_size;

  U32* indices;
  U32  indices_size;

  Image texture;
};

typedef struct Model Model;
struct Model {
  Mesh* meshes;
};

B32 ModelLoadFile(Arena* arena, Model* model, String8 file_path);
B32 ModelLoad(Arena* arena, Model* model, U8* file_data, U32 file_data_size);

#endif // CDEFAULT_MODEL_H_

#ifdef CDEFAULT_MODEL_IMPLEMENTATION
#undef CDEFAULT_MODEL_IMPLEMENTATION

static B32 ModelLoadObj(Arena* arena, Model* model, U8* file_data, U32 file_data_size) {
  MEMORY_ZERO_STRUCT(model);
  Arena* temp_arena = ArenaAllocate();
  String8 file_str = Str8(file_data, file_data_size);
  String8List lines = Str8Split(temp_arena, file_str, '\n');
  U64 arena_pos = ArenaPos(arena);
  B32 success = false;

  // NOTE: OBJ files are only a single mesh.
  Mesh* mesh = ARENA_PUSH_STRUCT(arena, Mesh);
  MEMORY_ZERO_STRUCT(mesh);
  model->meshes = mesh;

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
  if (obj_uvs_size > 0)     { mesh->uvs = ARENA_PUSH_ARRAY(arena, V2, vertices_size_estimate);     }
  for (String8ListNode* line = lines.head; line != NULL; line = line->next) {
    if (!Str8StartsWith(line->string, Str8Lit("f "))) { continue; }
    String8List line_parts = Str8Split(temp_arena, line->string, ' ');
    U32 face_vertices_size = 0;
    for (String8ListNode* line_part = line_parts.head->next; line_part != NULL; line_part = line_part->next) {
      String8List face_parts = Str8Split(temp_arena, line_part->string, '/');
      DEBUG_ASSERT(face_parts.head != NULL);

      // NOTE: parse face point vertex/uv/normal
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

      // NOTE: search for duplicate vertex
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

      // NOTE: add vertex to lists
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

      // TODO: support this
      if (++face_vertices_size > 3) {
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

typedef struct GltfBuffer GltfBuffer;
struct GltfBuffer {
  GltfBuffer* next;
  U8* bytes;
  U32 length;
};

typedef enum GltfAccessorType GltfAccessorType;
enum GltfAccessorType {
  GltfAccessorType_Scalar,
  GltfAccessorType_V2,
  GltfAccessorType_V3,
  GltfAccessorType_V4,
  GltfAccessorType_M2,
  GltfAccessorType_M3,
  GltfAccessorType_M4,
};

typedef enum GltfAccessorComponentType GltfAccessorComponentType;
enum GltfAccessorComponentType {
  GltfAccessorComponentType_S8,
  GltfAccessorComponentType_U8,
  GltfAccessorComponentType_S16,
  GltfAccessorComponentType_U16,
  GltfAccessorComponentType_U32,
  GltfAccessorComponentType_F32,
};

typedef struct GltfAccessor GltfAccessor;
struct GltfAccessor {
  GltfAccessor* next;
  GltfAccessorType type;
  GltfAccessorComponentType component_type;
  U32 count;
  union {
    S8*  s8_arr;
    U8*  u8_arr;
    S16* s16_arr;
    U16* u16_arr;
    U32* u32_arr;
    F32* f32_arr;
  };
};

typedef struct GltfMeshPrimitives GltfMeshPrimitives;
struct GltfMeshPrimitives {
  GltfMeshPrimitives* next;
  GltfAccessor* position; // NOTE: V3 F32
  GltfAccessor* normal;   // NOTE: V3 F32
  GltfAccessor* texcoord; // NOTE: V2 F32
  GltfAccessor* indices;  // NOTE: Scalar
};

typedef struct GltfMesh GltfMesh;
struct GltfMesh {
  GltfMesh* next;
  String8 name;
  GltfMeshPrimitives* head;
  GltfMeshPrimitives* tail;
};

B32 GltfBufferGet(GltfBuffer* head, U32 index, GltfBuffer** result) {
  GltfBuffer* curr = head;
  while ((curr != NULL) && index > 0) {
    curr = curr->next;
    index--;
  }
  if (curr == NULL) { return false; }
  *result = curr;
  return true;
}

B32 GltfAccessorGet(GltfAccessor* head, U32 index, GltfAccessor** result) {
  GltfAccessor* curr = head;
  while ((curr != NULL) && index > 0) {
    curr = curr->next;
    index--;
  }
  if (curr == NULL) { return false; }
  *result = curr;
  return true;
}

#define MODEL_TRY_PARSE(eval) if (!eval) { LOG_ERROR("[MESH] Ran out of characters in mesh GLTF file."); return false; }
static B32 GltfBufferParse(JsonValue* value, BinStream buffer, GltfBuffer* result) {
  MEMORY_ZERO_STRUCT(result);
  JsonObject obj;
  if (!JsonValueGetObject(value, &obj)) {
    LOG_ERROR("[MESH] GLTF unable to parse 'bufferView' object.");
    return false;
  }
  F32 buffer_idx, byte_length, byte_offset;
  if (!JsonObjectGetNumber(obj, Str8Lit("buffer"), &buffer_idx)) {
    LOG_ERROR("[MESH] GLTF bufferView missing required 'buffer'.");
    return false;
  }
  if (!JsonObjectGetNumber(obj, Str8Lit("byteLength"), &byte_length)) {
    LOG_ERROR("[MESH] GLTF bufferView missing required 'byteLength'.");
    return false;
  }
  if (!JsonObjectGetNumber(obj, Str8Lit("byteOffset"), &byte_offset)) {
    LOG_ERROR("[MESH] GLTF bufferView missing required 'byteOffset'.");
    return false;
  }
  if (buffer_idx != 0) {
    LOG_ERROR("[MESH] GLTF reader only supports a single buffer.");
    return false;
  }

  MODEL_TRY_PARSE(BinStreamSeek(&buffer, byte_offset));
  result->bytes  = BinStreamDecay(&buffer);
  result->length = byte_length;
  return true;
}
#undef MODEL_TRY_PARSE

#define MODEL_TRY_PARSE(eval) if (!eval) { LOG_ERROR("[MESH] Ran out of characters in mesh GLTF file."); goto gltf_accessor_parse_exit; }
static B32 GltfAccessorParse(Arena* arena, JsonValue* value, GltfBuffer* buffers, GltfAccessor* result) {
  B32 success = false;
  U64 arena_base = ArenaPos(arena);
  MEMORY_ZERO_STRUCT(result);
  JsonObject obj;
  if (!JsonValueGetObject(value, &obj)) {
    LOG_ERROR("[MESH] GLTF unable to parse 'accessor' object.");
    goto gltf_accessor_parse_exit;
  }
  F32 buffer_view_idx, count, component_type;
  if (!JsonObjectGetNumber(obj, Str8Lit("bufferView"), &buffer_view_idx)) {
    LOG_ERROR("[MESH] GLTF accessor missing required 'bufferView'.");
    goto gltf_accessor_parse_exit;
  }
  if (!JsonObjectGetNumber(obj, Str8Lit("count"), &count)) {
    LOG_ERROR("[MESH] GLTF accessor missing required 'count'.");
    goto gltf_accessor_parse_exit;
  }
  if (!JsonObjectGetNumber(obj, Str8Lit("componentType"), &component_type)) {
    LOG_ERROR("[MESH] GLTF accessor missing required 'componentType'.");
    goto gltf_accessor_parse_exit;
  }
  String8 type;
  if (!JsonObjectGetString(obj, Str8Lit("type"), &type)) {
    LOG_ERROR("[MESH] GLTF accessor missing required 'type'.");
    goto gltf_accessor_parse_exit;
  }

  result->count = count;
  U32 component_count;
  if (Str8Eq(type, Str8Lit("SCALAR"))) {
    result->type = GltfAccessorType_Scalar;
    component_count = 1;
  } else if (Str8Eq(type, Str8Lit("VEC2"))) {
    result->type = GltfAccessorType_V2;
    component_count = 2;
  } else if (Str8Eq(type, Str8Lit("VEC3")))   {
    result->type = GltfAccessorType_V3;
    component_count = 3;
  } else if (Str8Eq(type, Str8Lit("VEC4")))   {
    result->type = GltfAccessorType_V4;
    component_count = 4;
  } else if (Str8Eq(type, Str8Lit("MAT2")))   {
    result->type = GltfAccessorType_M2;
    component_count = 4;
  } else if (Str8Eq(type, Str8Lit("MAT3")))   {
    result->type = GltfAccessorType_M3;
    component_count = 9;
  } else if (Str8Eq(type, Str8Lit("MAT4")))   {
    result->type = GltfAccessorType_M4;
    component_count = 16;
  } else {
    LOG_ERROR("[MESH] GLTF accessor unexpected type: %s.", type.size, type.str);
    goto gltf_accessor_parse_exit;
  }

  U32 component_byte_size;
  switch ((U32) component_type) {
    case 5120: {
      result->component_type = GltfAccessorComponentType_S8;
      result->s8_arr = ARENA_PUSH_ARRAY(arena, S8, result->count * component_count);
      component_byte_size = 1;
    } break;
    case 5121: {
      result->component_type = GltfAccessorComponentType_U8;
      result->u8_arr = ARENA_PUSH_ARRAY(arena, U8, result->count * component_count);
      component_byte_size = 1;
    } break;
    case 5122: {
      result->component_type = GltfAccessorComponentType_S16;
      result->s16_arr = ARENA_PUSH_ARRAY(arena, S16, result->count * component_count);
      component_byte_size = 2;
    } break;
    case 5123: {
      result->component_type = GltfAccessorComponentType_U16;
      result->u16_arr = ARENA_PUSH_ARRAY(arena, U16, result->count * component_count);
      component_byte_size = 2;
    } break;
    case 5125: {
      result->component_type = GltfAccessorComponentType_U32;
      result->u32_arr = ARENA_PUSH_ARRAY(arena, U32, result->count * component_count);
      component_byte_size = 4;
    } break;
    case 5126: {
      result->component_type = GltfAccessorComponentType_F32;
      result->f32_arr = ARENA_PUSH_ARRAY(arena, F32, result->count * component_count);
      component_byte_size = 4;
    } break;
    default: {
      LOG_ERROR("[MESH] GLTF unrecognized componentType %d.", (U32) component_type);
      goto gltf_accessor_parse_exit;
    } break;
  }

  GltfBuffer* buffer;
  if (!GltfBufferGet(buffers, (U32) buffer_view_idx, &buffer)) {
    LOG_ERROR("[MESH] GLTF buffer view idx is OOB: %d", (U32) buffer_view_idx);
    goto gltf_accessor_parse_exit;
  }
  if (result->count * component_count * component_byte_size != buffer->length) {
    LOG_ERROR("[MESH] GLTF accessor size disagrees with bufferView byte count.");
    goto gltf_accessor_parse_exit;
  }
  BinStream s = BinStreamAssign(buffer->bytes, buffer->length);
  for (U32 i = 0; i < result->count; i++) {
    for (U32 j = 0; j < component_count; j++) {
      switch (result->component_type) {
        case GltfAccessorComponentType_S8:  { MODEL_TRY_PARSE(BinStreamPullS8(&s, &result->s8_arr[(i * component_count) + j]));     } break;
        case GltfAccessorComponentType_U8:  { MODEL_TRY_PARSE(BinStreamPullU8(&s, &result->u8_arr[(i * component_count) + j]));     } break;
        case GltfAccessorComponentType_S16: { MODEL_TRY_PARSE(BinStreamPullS16LE(&s, &result->s16_arr[(i * component_count) + j])); } break;
        case GltfAccessorComponentType_U16: { MODEL_TRY_PARSE(BinStreamPullU16LE(&s, &result->u16_arr[(i * component_count) + j])); } break;
        case GltfAccessorComponentType_U32: { MODEL_TRY_PARSE(BinStreamPullU32LE(&s, &result->u32_arr[(i * component_count) + j])); } break;
        case GltfAccessorComponentType_F32: {
          U32 component;
          MODEL_TRY_PARSE(BinStreamPullU32LE(&s, &component));
          result->f32_arr[(i * component_count) + j] = *(F32*)&component;
        } break;
        default: UNREACHABLE();
      }
    }
  }

  success = true;
gltf_accessor_parse_exit:
  if (!success) { ArenaPopTo(arena, arena_base); }
  return success;
}
#undef MODEL_TRY_PARSE

static B32 GltfMeshPrimitivesParse(JsonValue* value, GltfAccessor* accessors, GltfMeshPrimitives* result) {
  MEMORY_ZERO_STRUCT(result);
  JsonObject obj, attributes;
  if (!JsonValueGetObject(value, &obj)) {
    LOG_ERROR("[MESH] GLTF unable to parse 'primitives' object.");
    return false;
  }
  if (!JsonObjectGetObject(obj, Str8Lit("attributes"), &attributes)) {
    LOG_ERROR("[MESH] GLTF mesh missing required 'attributes'.");
    return false;
  }

  F32 position, normal, texcoord, indices;
  if (!JsonObjectGetNumber(attributes, Str8Lit("POSITION"), &position)) {
    LOG_ERROR("[MESH] GLTF mesh missing required 'POSITION'.");
    return false;
  }
  if (!JsonObjectGetNumber(attributes, Str8Lit("NORMAL"), &normal)) {
    normal = -1;
  }
  if (!JsonObjectGetNumber(attributes, Str8Lit("TEXCOORD_0"), &texcoord)) {
    texcoord = -1;
  }
  if (!JsonObjectGetNumber(obj, Str8Lit("indices"), &indices)) {
    LOG_ERROR("[MESH] GLTF mesh missing required 'indices'.");
    return false;
  }

  // TODO: move validations out of here?
  if (!GltfAccessorGet(accessors, (U32) position, &result->position)) {
    LOG_ERROR("[MESH] GLTF mesh unable to find 'position' accessor %d", (U32) position);
    return false;
  }
  if (result->position->type != GltfAccessorType_V3) {
    LOG_ERROR("[MESH] GLTF mesh 'position' unexpectedly not type V3.");
    return false;
  }
  if (result->position->component_type != GltfAccessorComponentType_F32) {
    LOG_ERROR("[MESH] GLTF mesh 'position' component unexpectedly not type F32.");
    return false;
  }
  U32 expected_vertices_count = result->position->count;

  if (!GltfAccessorGet(accessors, (U32) indices, &result->indices)) {
    LOG_ERROR("[MESH] GLTF mesh unable to find 'indices' accessor %d", (U32) indices);
    return false;
  }
  if (result->indices->type != GltfAccessorType_Scalar) {
    LOG_ERROR("[MESH] GLTF mesh 'indices' unexpectedly not type V3.");
    return false;
  }

  if (normal >= 0) {
    if (!GltfAccessorGet(accessors, (U32) normal, &result->normal)) {
      LOG_ERROR("[MESH] GLTF mesh unable to find 'normal' accessor %d", (U32) normal);
      return false;
    }
    if (result->normal->type != GltfAccessorType_V3) {
      LOG_ERROR("[MESH] GLTF mesh 'normal' unexpectedly not type V3.");
      return false;
    }
    if (result->normal->component_type != GltfAccessorComponentType_F32) {
      LOG_ERROR("[MESH] GLTF mesh 'normal' component unexpectedly not type F32.");
      return false;
    }
    if (result->normal->count != expected_vertices_count) {
      LOG_ERROR("[MESH] GLTF mesh 'normal' does not have the expected size %d", (U32) expected_vertices_count);
      return false;
    }
  }

  if (texcoord >= 0) {
    if (!GltfAccessorGet(accessors, (U32) texcoord, &result->texcoord)) {
      LOG_ERROR("[MESH] GLTF mesh unable to find 'texcoord' accessor %d", (U32) texcoord);
      return false;
    }
    if (result->texcoord->type != GltfAccessorType_V2) {
      LOG_ERROR("[MESH] GLTF mesh 'indices' unexpectedly not type V2.");
      return false;
    }
    if (result->texcoord->component_type != GltfAccessorComponentType_F32) {
      LOG_ERROR("[MESH] GLTF mesh 'texcoord' component unexpectedly not type F32.");
      return false;
    }
    if (result->texcoord->count != expected_vertices_count) {
      LOG_ERROR("[MESH] GLTF mesh 'texcoord' does not have the expected size %d", (U32) expected_vertices_count);
      return false;
    }
  }

  return true;
}

static B32 GltfMeshParse(Arena* arena, JsonValue* value, GltfAccessor* accessors, GltfMesh* result) {
  MEMORY_ZERO_STRUCT(result);
  B32 success = false;
  U64 arena_base = ArenaPos(arena);
  JsonObject obj;
  if (!JsonValueGetObject(value, &obj)) {
    LOG_ERROR("[MESH] GLTF mesh unable to parse 'mesh' object.");
    goto json_value_get_gltf_mesh_exit;
  }
  JsonObjectGetString(obj, Str8Lit("name"), &result->name);
  JsonArray primitives;
  if (!JsonObjectGetArray(obj, Str8Lit("primitives"), &primitives)) {
    LOG_ERROR("[MESH] GLTF mesh missing required 'primitives'.");
    goto json_value_get_gltf_mesh_exit;
  }
  JsonArrayNode* primitives_arr = primitives.head;
  JsonValue* primitives_value   = NULL;
  while ((primitives_value = JsonArrayNext(&primitives_arr)) != NULL) {
    GltfMeshPrimitives* primitives = ARENA_PUSH_STRUCT(arena, GltfMeshPrimitives);
    if (!GltfMeshPrimitivesParse(primitives_value, accessors, primitives)) { goto json_value_get_gltf_mesh_exit; }
    SLL_QUEUE_PUSH_BACK(result->head, result->tail, primitives, next);
  }
  success = true;
json_value_get_gltf_mesh_exit:
  if (!success) { ArenaPopTo(arena, arena_base); }
  return success;
}

#define MODEL_TRY_PARSE(eval) if (!eval) { LOG_ERROR("[MESH] Ran out of characters in mesh GLTF file."); goto mesh_load_glb_exit; }
static B32 ModelLoadGlb(Arena* arena, Model* model, U8* file_data, U32 file_data_size) {
  MEMORY_ZERO_STRUCT(model);
  U64 arena_base = ArenaPos(arena);
  Arena* temp_arena = ArenaAllocate();
  B32 success = false;

  BinStream s = BinStreamAssign(file_data, file_data_size);

  // NOTE: header
  U32 magic_number, version;
  MODEL_TRY_PARSE(BinStreamPullU32LE(&s, &magic_number));
  if (magic_number != 0x46546C67) { return false; }
  MODEL_TRY_PARSE(BinStreamPullU32LE(&s, &version));
  if (version != 2) {
    LOG_WARN("[MESH] For GLTF, only version 2 is supported, detected version: %d", version);
    goto mesh_load_glb_exit;
  }
  MODEL_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U32)));

  // NOTE: json blob
  U32 json_chunk_size, json_chunk_type;
  MODEL_TRY_PARSE(BinStreamPullU32LE(&s, &json_chunk_size));
  MODEL_TRY_PARSE(BinStreamPullU32LE(&s, &json_chunk_type));
  if (json_chunk_type != 0x4E4F534A) {
    LOG_ERROR("[MESH] in GLTF file, JSON chunk expected but not observed.");
    goto mesh_load_glb_exit;
  }
  if (BinStreamRemaining(&s) < json_chunk_size) {
    LOG_ERROR("[MESH] in GLTF file, ran out of characters.");
    goto mesh_load_glb_exit;
  }
  String8 gltf_json_str8;
  gltf_json_str8.str  = BinStreamDecay(&s);
  gltf_json_str8.size = json_chunk_size;
  gltf_json_str8      = Str8TrimBack(gltf_json_str8);
  JsonObject json;
  if (!JsonParse(temp_arena, &json, gltf_json_str8)) {
    LOG_ERROR("[MESH] in GLTF file, failed to parse JSON chunk.");
    goto mesh_load_glb_exit;
  }

  // NOTE: bin blob
  MODEL_TRY_PARSE(BinStreamSkip(&s, json_chunk_size, sizeof(U8)));
  if (BinStreamRemaining(&s) == 0) {
    LOG_ERROR("[MESH] GLTF loader only supports a single embedded bin chunk (no external chunks).");
    goto mesh_load_glb_exit;
  }
  U32 bin_chunk_size, bin_chunk_type;
  MODEL_TRY_PARSE(BinStreamPullU32LE(&s, &bin_chunk_size));
  MODEL_TRY_PARSE(BinStreamPullU32LE(&s, &bin_chunk_type));
  if (bin_chunk_type != 0x004E4942) {
    LOG_ERROR("[MESH] in GLTF file, failed to parse bin chunk.");
    goto mesh_load_glb_exit;
  }
  BinStream bin = BinStreamAssign(BinStreamDecay(&s), bin_chunk_size);

  /*
  String8 test;
  JsonToString(temp_arena, json, &test, true);
  LOG_INFO("%.*s", test.size, test.str);
  */

  JsonArray json_buffers;
  if (!JsonObjectGetArray(json, Str8Lit("bufferViews"), &json_buffers)) {
    LOG_ERROR("[MESH] GLTF missing required attribute 'bufferViews'.");
    goto mesh_load_glb_exit;
  }
  JsonArrayNode* json_buffers_it = json_buffers.head;
  JsonValue* json_buffer_view    = NULL;
  GltfBuffer* buffers            = NULL;
  GltfBuffer* buffers_tail       = NULL;
  while ((json_buffer_view = JsonArrayNext(&json_buffers_it)) != NULL) {
    GltfBuffer* buffer = ARENA_PUSH_STRUCT(temp_arena, GltfBuffer);
    if (!GltfBufferParse(json_buffer_view, bin, buffer)) { goto mesh_load_glb_exit; }
    SLL_QUEUE_PUSH_BACK(buffers, buffers_tail, buffer, next);
  }

  JsonArray json_accessors;
  if (!JsonObjectGetArray(json, Str8Lit("accessors"), &json_accessors)) {
    LOG_ERROR("[MESH] GLTF missing required attribute 'meshes'.");
    goto mesh_load_glb_exit;
  }
  JsonArrayNode* json_accessors_it = json_accessors.head;
  JsonValue* json_accessor         = NULL;
  GltfAccessor* accessors          = NULL;
  GltfAccessor* accessors_tail     = NULL;
  while ((json_accessor = JsonArrayNext(&json_accessors_it)) != NULL) {
    GltfAccessor* accessor = ARENA_PUSH_STRUCT(temp_arena, GltfAccessor);
    if (!GltfAccessorParse(temp_arena, json_accessor, buffers, accessor)) { goto mesh_load_glb_exit; }
    SLL_QUEUE_PUSH_BACK(accessors, accessors_tail, accessor, next);
  }

  JsonArray json_meshes;
  if (!JsonObjectGetArray(json, Str8Lit("meshes"), &json_meshes)) {
    LOG_ERROR("[MESH] GLTF missing required attribute 'meshes'.");
    goto mesh_load_glb_exit;
  }
  JsonArrayNode* json_meshes_it = json_meshes.head;
  JsonValue* json_mesh          = NULL;
  GltfMesh* meshes              = NULL;
  while ((json_mesh = JsonArrayNext(&json_meshes_it)) != NULL) {
    GltfMesh* mesh = ARENA_PUSH_STRUCT(temp_arena, GltfMesh);
    if (!GltfMeshParse(temp_arena, json_mesh, accessors, mesh)) { goto mesh_load_glb_exit; }
    SLL_STACK_PUSH(meshes, mesh, next);
  }

  for (GltfMesh* gltf_mesh = meshes; gltf_mesh != NULL; gltf_mesh = gltf_mesh->next) {
    for (GltfMeshPrimitives* primitives = gltf_mesh->head; primitives != NULL; primitives = primitives->next) {
      Mesh* mesh = ARENA_PUSH_STRUCT(arena, Mesh);
      MEMORY_ZERO_STRUCT(mesh);
      SLL_STACK_PUSH(model->meshes, mesh, next);

      mesh->vertices_size = primitives->position->count;
      mesh->points = ARENA_PUSH_ARRAY(arena, V3, mesh->vertices_size);
      MEMORY_MOVE(mesh->points, primitives->position->f32_arr, sizeof(F32) * 3 * mesh->vertices_size);
      if (primitives->normal != NULL) {
        mesh->normals = ARENA_PUSH_ARRAY(arena, V3, mesh->vertices_size);
        MEMORY_MOVE(mesh->normals, primitives->normal->f32_arr, sizeof(F32) * 3 * mesh->vertices_size);
      }
      if (primitives->texcoord != NULL) {
        mesh->uvs = ARENA_PUSH_ARRAY(arena, V2, mesh->vertices_size);
        MEMORY_MOVE(mesh->uvs, primitives->texcoord->f32_arr, sizeof(F32) * 2 * mesh->vertices_size);
      }

      mesh->indices_size = primitives->indices->count;
      mesh->indices = ARENA_PUSH_ARRAY(arena, U32, primitives->indices->count);
      for (U32 i = 0; i < mesh->indices_size; i++) {
        switch (primitives->indices->component_type) {
          case GltfAccessorComponentType_S8:  { mesh->indices[i] = (U32) primitives->indices->s8_arr[i];  } break;
          case GltfAccessorComponentType_U8:  { mesh->indices[i] = (U32) primitives->indices->u8_arr[i];  } break;
          case GltfAccessorComponentType_S16: { mesh->indices[i] = (U32) primitives->indices->s16_arr[i]; } break;
          case GltfAccessorComponentType_U16: { mesh->indices[i] = (U32) primitives->indices->u16_arr[i]; } break;
          case GltfAccessorComponentType_U32: { mesh->indices[i] = (U32) primitives->indices->u32_arr[i]; } break;
          default: {
            LOG_ERROR("[MESH] GLTF unexpected mesh indices attribute observed.");
            goto mesh_load_glb_exit;
          } break;
        }
      }
    }
  }

  success = true;
mesh_load_glb_exit:
  ArenaRelease(temp_arena);
  if (!success) { ArenaPopTo(arena, arena_base); }
  return success;
}
#undef MODEL_TRY_PARSE

B32 ModelLoad(Arena* arena, Model* model, U8* file_data, U32 file_data_size) {
  if (ModelLoadObj(arena, model, file_data, file_data_size)) { return true; }
  if (ModelLoadGlb(arena, model, file_data, file_data_size)) { return true; }
  return false;
}

B32 ModelLoadFile(Arena* arena, Model* model, String8 file_path) {
  B32 success = false;
  String8 file_data;
  Arena* file_arena = ArenaAllocate();
  if (!FileReadAll(file_arena, file_path, &file_data.str, &file_data.size)) { goto mesh_load_file_exit; }
  success = ModelLoad(arena, model, file_data.str, file_data.size);
mesh_load_file_exit:
  ArenaRelease(file_arena);
  return success;
}

#endif // CDEFAULT_MODEL_IMPLEMENTATION
