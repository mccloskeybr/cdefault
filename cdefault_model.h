#ifndef CDEFAULT_MODEL_H_
#define CDEFAULT_MODEL_H_

#include "cdefault_std.h"
#include "cdefault_math.h"
#include "cdefault_image.h"
#include "cdefault_json.h"
#include "cdefault_io.h"

// NOTE: supports loading:
// - OBJ
// - GLB (v2.0)

// TODO:
// - OBJ support mtl files / textures
// - extend GLB to support GLTF files too

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

B32 ModelLoadObj(Arena* arena, Model* model, U8* file_data, U32 file_data_size);
B32 ModelLoadGlb(Arena* arena, Model* model, U8* file_data, U32 file_data_size);

#endif // CDEFAULT_MODEL_H_

#ifdef CDEFAULT_MODEL_IMPLEMENTATION
#undef CDEFAULT_MODEL_IMPLEMENTATION

#define MODEL_LOG_OUT_OF_CHARS() LOG_ERROR("[FONT] Ran out of characters in model file.")

B32 ModelLoadObj(Arena* arena, Model* model, U8* file_data, U32 file_data_size) {
  MEMORY_ZERO_STRUCT(model);
  Arena* temp_arena = ArenaAllocate();
  String8 file_str = Str8(file_data, file_data_size);
  String8List lines = Str8Split(temp_arena, file_str, '\n');
  U64 arena_pos = ArenaPos(arena);
  B32 success = false;

  // TODO: fast failure if file is not an obj file

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
        if (V3Eq(mesh->points[i], obj_points[point_idx])) {
          if (normal_idx == -1 || V3Eq(mesh->normals[i], obj_normals[normal_idx])) {
            if (uv_idx == -1 || V2Eq(mesh->uvs[i], obj_uvs[uv_idx])) {
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
      MEMORY_MOVE_ARRAY(new_normals, mesh->normals, mesh->vertices_size);
      mesh->normals = new_normals;
      to_pop += vertex_overfit * sizeof(V3);
    }
    if (obj_uvs_size > 0) {
      V2* new_uvs;
      if (obj_normals_size > 0) { new_uvs = (V2*) (mesh->normals + mesh->vertices_size); }
      else                      { new_uvs = (V2*) (mesh->points + mesh->vertices_size);  }
      MEMORY_MOVE_ARRAY(new_uvs, mesh->uvs, mesh->vertices_size);
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

typedef struct GltfBuffer GltfBuffer;
struct GltfBuffer {
  GltfBuffer* next;
  U8* bytes;
  U32 length;
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

#define BIN_CATCH MODEL_LOG_OUT_OF_CHARS(); return false;
static B32 GltfBufferParse(JsonValue* value, BinStream buffer, GltfBuffer* result) {
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

  BIN_TRY(BinStreamSeek(&buffer, byte_offset));
  MEMORY_ZERO_STRUCT(result);
  result->bytes  = BinStreamDecay(&buffer);
  result->length = byte_length;
  return true;
}
#undef BIN_CATCH

static B32 GltfImageParse(JsonValue* value, GltfBuffer* buffers, GltfBuffer* result) {
  JsonObject obj;
  if (!JsonValueGetObject(value, &obj)) {
    LOG_ERROR("[MESH] GLTF unable to parse 'image' object.");
    return false;
  }
  F32 buffer_idx;
  if (!JsonObjectGetNumber(obj, Str8Lit("bufferView"), &buffer_idx)) {
    LOG_ERROR("[MESH] GLTF image missing required 'bufferView'.");
    return false;
  }
  GltfBuffer* buffer;
  if (!GltfBufferGet(buffers, (U32) buffer_idx, &buffer)) {
    LOG_ERROR("[MESH] GLTF buffer view idx is OOB: %d", (U32) buffer_idx);
    return false;
  }

  MEMORY_ZERO_STRUCT(result);
  result->bytes  = buffer->bytes;
  result->length = buffer->length;
  return true;
}

static B32 GltfTextureParse(JsonValue* value, GltfBuffer* images, GltfBuffer* result) {
  JsonObject obj;
  if (!JsonValueGetObject(value, &obj)) {
    LOG_ERROR("[MESH] GLTF unable to parse 'texture' object.");
    return false;
  }
  F32 image_idx;
  if (!JsonObjectGetNumber(obj, Str8Lit("source"), &image_idx)) {
    LOG_ERROR("[MESH] GLTF texture missing required 'source'.");
    return false;
  }
  GltfBuffer* image;
  if (!GltfBufferGet(images, (U32) image_idx, &image)) {
    LOG_ERROR("[MESH] GLTF image idx is OOB: %d", (U32) image_idx);
    return false;
  }

  MEMORY_ZERO_STRUCT(result);
  result->bytes  = image->bytes;
  result->length = image->length;
  return true;
}

static B32 GltfMaterialParse(JsonValue* value, GltfBuffer* textures, GltfBuffer* result) {
  JsonObject obj, pbr_metallic_roughness, base_color_texture;
  if (!JsonValueGetObject(value, &obj)) {
    LOG_ERROR("[MESH] GLTF unable to parse 'material' object.");
    return false;
  }
  if (!JsonObjectGetObject(obj, Str8Lit("pbrMetallicRoughness"), &pbr_metallic_roughness)) {
    LOG_ERROR("[MESH] GLTF material missing required 'pbrMetallicRoughness'.");
    return false;
  }
  if (!JsonObjectGetObject(pbr_metallic_roughness, Str8Lit("baseColorTexture"), &base_color_texture)) {
    LOG_ERROR("[MESH] GLTF pbrMetallicRoughness missing required 'baseColorTexture'.");
    return false;
  }
  F32 texture_idx;
  if (!JsonObjectGetNumber(base_color_texture, Str8Lit("index"), &texture_idx)) {
    LOG_ERROR("[MESH] GLTF pbrMetallicRoughness missing required 'index'.");
    return false;
  }
  GltfBuffer* texture;
  if (!GltfBufferGet(textures, (U32) texture_idx, &texture)) {
    LOG_ERROR("[MESH] GLTF texture idx is OOB: %d", (U32) texture_idx);
    return false;
  }
  MEMORY_ZERO_STRUCT(result);
  result->bytes  = texture->bytes;
  result->length = texture->length;
  return true;
}

#define BIN_CATCH LOG_ERROR("[MESH] Ran out of characters in mesh GLTF file."); goto gltf_accessor_parse_exit;
static B32 GltfAccessorParse(Arena* arena, JsonValue* value, GltfBuffer* buffers, GltfAccessor* result) {
  B32 success = false;
  U64 arena_base = ArenaPos(arena);
  JsonObject obj;
  if (!JsonValueGetObject(value, &obj)) {
    LOG_ERROR("[MESH] GLTF unable to parse 'accessor' object.");
    goto gltf_accessor_parse_exit;
  }
  F32 buffer_idx, count, component_type;
  if (!JsonObjectGetNumber(obj, Str8Lit("bufferView"), &buffer_idx)) {
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

  MEMORY_ZERO_STRUCT(result);
  result->count = count;
  U32 component_count;
  if (Str8Eq(type, Str8Lit("SCALAR"))) {
    result->type = GltfAccessorType_Scalar;
    component_count = 1;
  } else if (Str8Eq(type, Str8Lit("VEC2"))) {
    result->type = GltfAccessorType_V2;
    component_count = 2;
  } else if (Str8Eq(type, Str8Lit("VEC3"))) {
    result->type = GltfAccessorType_V3;
    component_count = 3;
  } else if (Str8Eq(type, Str8Lit("VEC4"))) {
    result->type = GltfAccessorType_V4;
    component_count = 4;
  } else if (Str8Eq(type, Str8Lit("MAT2"))) {
    result->type = GltfAccessorType_M2;
    component_count = 4;
  } else if (Str8Eq(type, Str8Lit("MAT3"))) {
    result->type = GltfAccessorType_M3;
    component_count = 9;
  } else if (Str8Eq(type, Str8Lit("MAT4"))) {
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
  if (!GltfBufferGet(buffers, (U32) buffer_idx, &buffer)) {
    LOG_ERROR("[MESH] GLTF buffer view idx is OOB: %d", (U32) buffer_idx);
    goto gltf_accessor_parse_exit;
  }
  if (result->count * component_count * component_byte_size != buffer->length) {
    LOG_ERROR("[MESH] GLTF accessor size disagrees with bufferView byte count.");
    goto gltf_accessor_parse_exit;
  }
  BinStream s = BinStreamAssign(buffer->bytes, buffer->length);
  // TODO: better way to do this?
  for (U32 i = 0; i < result->count * component_count; i++) {
    switch (result->component_type) {
      case GltfAccessorComponentType_S8:  { BIN_TRY(BinStreamPullS8(&s, &result->s8_arr[i]));     } break;
      case GltfAccessorComponentType_U8:  { BIN_TRY(BinStreamPullU8(&s, &result->u8_arr[i]));     } break;
      case GltfAccessorComponentType_S16: { BIN_TRY(BinStreamPullS16LE(&s, &result->s16_arr[i])); } break;
      case GltfAccessorComponentType_U16: { BIN_TRY(BinStreamPullU16LE(&s, &result->u16_arr[i])); } break;
      case GltfAccessorComponentType_U32: { BIN_TRY(BinStreamPullU32LE(&s, &result->u32_arr[i])); } break;
      case GltfAccessorComponentType_F32: { BIN_TRY(BinStreamPullF32LE(&s, &result->f32_arr[i])); } break;
      default: UNREACHABLE();
    }
  }

  success = true;
gltf_accessor_parse_exit:
  if (!success) { ArenaPopTo(arena, arena_base); }
  return success;
}
#undef BIN_CATCH

static B32 GltfMeshParse(Arena* arena, JsonValue* value, GltfAccessor* accessors, GltfBuffer* materials, Mesh** result) {
  *result = NULL;
  B32 success = false;
  U64 arena_base = ArenaPos(arena);
  JsonObject obj;
  if (!JsonValueGetObject(value, &obj)) {
    LOG_ERROR("[MESH] GLTF mesh unable to parse 'mesh' object.");
    goto gltf_mesh_parse_exit;
  }
  JsonArray json_primitives_arr;
  if (!JsonObjectGetArray(obj, Str8Lit("primitives"), &json_primitives_arr)) {
    LOG_ERROR("[MESH] GLTF mesh missing required 'primitives'.");
    goto gltf_mesh_parse_exit;
  }
  JsonArrayNode* json_primitives_it = json_primitives_arr.head;
  JsonValue* json_primitives_value  = NULL;
  while ((json_primitives_value = JsonArrayNext(&json_primitives_it)) != NULL) {
    JsonObject json_primitives_obj, json_attributes;
    if (!JsonValueGetObject(json_primitives_value, &json_primitives_obj)) {
      LOG_ERROR("[MESH] Unable to parse 'primitives' object.");
      goto gltf_mesh_parse_exit;
    }
    if (!JsonObjectGetObject(json_primitives_obj, Str8Lit("attributes"), &json_attributes)) {
      LOG_ERROR("[MESH] GLTF mesh missing required 'attributes'.");
      goto gltf_mesh_parse_exit;
    }

    Mesh* mesh = ARENA_PUSH_STRUCT(arena, Mesh);
    MEMORY_ZERO_STRUCT(mesh);
    if (*result != NULL) { mesh->next = *result; }
    *result = mesh;

    // NOTE: indices
    F32 indices_idx;
    if (!JsonObjectGetNumber(json_primitives_obj, Str8Lit("indices"), &indices_idx)) {
      LOG_ERROR("[MESH] GLTF mesh missing required 'indices'.");
      goto gltf_mesh_parse_exit;
    }
    GltfAccessor* indices;
    if (!GltfAccessorGet(accessors, (U32) indices_idx, &indices)) {
      LOG_ERROR("[MESH] GLTF mesh unable to find 'indices' accessor %d", (U32) indices_idx);
      goto gltf_mesh_parse_exit;
    }
    if (indices->type != GltfAccessorType_Scalar) {
      LOG_ERROR("[MESH] GLTF mesh 'indices' unexpectedly not type V3.");
      goto gltf_mesh_parse_exit;
    }
    mesh->indices_size = indices->count;
    mesh->indices = ARENA_PUSH_ARRAY(arena, U32, indices->count);
    // TODO: better way to do this?
    for (U32 i = 0; i < mesh->indices_size; i++) {
      switch (indices->component_type) {
        case GltfAccessorComponentType_S8:  { mesh->indices[i] = (U32) indices->s8_arr[i];  } break;
        case GltfAccessorComponentType_U8:  { mesh->indices[i] = (U32) indices->u8_arr[i];  } break;
        case GltfAccessorComponentType_S16: { mesh->indices[i] = (U32) indices->s16_arr[i]; } break;
        case GltfAccessorComponentType_U16: { mesh->indices[i] = (U32) indices->u16_arr[i]; } break;
        case GltfAccessorComponentType_U32: { mesh->indices[i] = (U32) indices->u32_arr[i]; } break;
        default: {
          LOG_ERROR("[MESH] GLTF unexpected mesh indices attribute observed.");
          goto gltf_mesh_parse_exit;
        } break;
      }
    }

    // NOTE: positions
    F32 position_idx;
    if (!JsonObjectGetNumber(json_attributes, Str8Lit("POSITION"), &position_idx)) {
      LOG_ERROR("[MESH] GLTF mesh missing required 'POSITION'.");
      goto gltf_mesh_parse_exit;
    }
    GltfAccessor* position;
    if (!GltfAccessorGet(accessors, (U32) position_idx, &position)) {
      LOG_ERROR("[MESH] GLTF mesh unable to find 'position' accessor %d", (U32) position_idx);
      goto gltf_mesh_parse_exit;
    }
    if (position->type != GltfAccessorType_V3) {
      LOG_ERROR("[MESH] GLTF mesh 'position' unexpectedly not type V3.");
      goto gltf_mesh_parse_exit;
    }
    if (position->component_type != GltfAccessorComponentType_F32) {
      LOG_ERROR("[MESH] GLTF mesh 'position' component unexpectedly not type F32.");
      goto gltf_mesh_parse_exit;
    }
    mesh->vertices_size = position->count;
    mesh->points = ARENA_PUSH_ARRAY(arena, V3, position->count);
    MEMORY_MOVE_ARRAY(mesh->points, position->f32_arr, 3 * position->count);

    // NOTE: normals
    F32 normal_idx;
    if (JsonObjectGetNumber(json_attributes, Str8Lit("NORMAL"), &normal_idx)) {
      GltfAccessor* normal;
      if (!GltfAccessorGet(accessors, (U32) normal_idx, &normal)) {
        LOG_ERROR("[MESH] GLTF mesh unable to find 'normal' accessor %d", (U32) normal_idx);
        goto gltf_mesh_parse_exit;
      }
      if (normal->type != GltfAccessorType_V3) {
        LOG_ERROR("[MESH] GLTF mesh 'normal' unexpectedly not type V3.");
        goto gltf_mesh_parse_exit;
      }
      if (normal->component_type != GltfAccessorComponentType_F32) {
        LOG_ERROR("[MESH] GLTF mesh 'normal' component unexpectedly not type F32.");
        goto gltf_mesh_parse_exit;
      }
      if (normal->count != position->count) {
        LOG_ERROR("[MESH] GLTF mesh 'normal' does not have the expected size: %d (was: %d)", position->count, normal->count);
        goto gltf_mesh_parse_exit;
      }
      mesh->normals = ARENA_PUSH_ARRAY(arena, V3, normal->count);
      MEMORY_MOVE_ARRAY(mesh->normals, normal->f32_arr, 3 * normal->count);
    }

    // NOTE: UVs
    F32 texcoord_idx;
    if (JsonObjectGetNumber(json_attributes, Str8Lit("TEXCOORD_0"), &texcoord_idx)) {
      GltfAccessor* texcoord;
      if (!GltfAccessorGet(accessors, (U32) texcoord_idx, &texcoord)) {
        LOG_ERROR("[MESH] GLTF mesh unable to find 'texcoord' accessor %d", (U32) texcoord_idx);
        goto gltf_mesh_parse_exit;
      }
      if (texcoord->type != GltfAccessorType_V2) {
        LOG_ERROR("[MESH] GLTF mesh 'texcoord' component unexpectedly not type V2.");
        goto gltf_mesh_parse_exit;
      }
      if (texcoord->component_type != GltfAccessorComponentType_F32) {
        LOG_ERROR("[MESH] GLTF mesh 'texcoord' component unexpectedly not type F32.");
        goto gltf_mesh_parse_exit;
      }
      if (texcoord->count != position->count) {
        LOG_ERROR("[MESH] GLTF mesh 'texcoord' does not have the expected size: %d (was: %d)", position->count, texcoord->count);
        goto gltf_mesh_parse_exit;
      }
      mesh->uvs = ARENA_PUSH_ARRAY(arena, V2, texcoord->count);
      MEMORY_MOVE_ARRAY(mesh->uvs, texcoord->f32_arr, 2 * texcoord->count);
      // NOTE: correct image y coordinate (since cdefault coords are low -> high, not high -> low)
      for (U32 i = 0; i < texcoord->count; i++) { mesh->uvs[i].v = 1 - mesh->uvs[i].v; }
    }

    // NOTE: texture
    F32 material_idx;
    if (JsonObjectGetNumber(json_primitives_obj, Str8Lit("material"), &material_idx)) {
      GltfBuffer* material;
      if (!GltfBufferGet(materials, (U32) material_idx, &material)) {
        LOG_ERROR("[MESH] GLTF mesh unable to find 'material' %d", (U32) material_idx);
        goto gltf_mesh_parse_exit;
      }
      if (!ImageLoad(arena, &mesh->texture, ImageFormat_RGBA, material->bytes, material->length)) {
        LOG_ERROR("[MESH] GLTF failed to load texture.");
        goto gltf_mesh_parse_exit;
      }
    }
  }

  success = true;
gltf_mesh_parse_exit:
  if (!success) { ArenaPopTo(arena, arena_base); }
  return success;
}

#define BIN_CATCH MODEL_LOG_OUT_OF_CHARS(); goto mesh_load_glb_exit;
B32 ModelLoadGlb(Arena* arena, Model* model, U8* file_data, U32 file_data_size) {
  MEMORY_ZERO_STRUCT(model);
  U64 arena_base = ArenaPos(arena);
  Arena* temp_arena = ArenaAllocate();
  B32 success = false;

  BinStream s = BinStreamAssign(file_data, file_data_size);

  // NOTE: header
  U32 magic_number, version;
  BIN_TRY(BinStreamPullU32LE(&s, &magic_number));
  if (magic_number != 0x46546C67) { return false; }
  BIN_TRY(BinStreamPullU32LE(&s, &version));
  if (version != 2) {
    LOG_WARN("[MESH] For GLTF, only version 2 is supported, detected version: %d", version);
    goto mesh_load_glb_exit;
  }
  BIN_TRY(BinStreamSkip(&s, 1, sizeof(U32)));

  // NOTE: json blob
  U32 json_chunk_size, json_chunk_type;
  BIN_TRY(BinStreamPullU32LE(&s, &json_chunk_size));
  BIN_TRY(BinStreamPullU32LE(&s, &json_chunk_type));
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
  BIN_TRY(BinStreamSkip(&s, json_chunk_size, sizeof(U8)));
  if (BinStreamRemaining(&s) == 0) {
    LOG_ERROR("[MESH] GLTF loader only supports a single embedded bin chunk (no external chunks).");
    goto mesh_load_glb_exit;
  }
  U32 bin_chunk_size, bin_chunk_type;
  BIN_TRY(BinStreamPullU32LE(&s, &bin_chunk_size));
  BIN_TRY(BinStreamPullU32LE(&s, &bin_chunk_type));
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
  JsonValue* json_buffer   = NULL;
  GltfBuffer* buffers      = NULL;
  GltfBuffer* buffers_tail = NULL;
  while ((json_buffer = JsonArrayNext(&json_buffers_it)) != NULL) {
    GltfBuffer* buffer = ARENA_PUSH_STRUCT(temp_arena, GltfBuffer);
    if (!GltfBufferParse(json_buffer, bin, buffer)) { goto mesh_load_glb_exit; }
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

  JsonArray json_images;
  GltfBuffer* images = NULL;
  if (JsonObjectGetArray(json, Str8Lit("images"), &json_images)) {
    JsonArrayNode* json_images_it = json_images.head;
    JsonValue* json_image   = NULL;
    GltfBuffer* images_tail = NULL;
    while ((json_image = JsonArrayNext(&json_images_it)) != NULL) {
      GltfBuffer* image = ARENA_PUSH_STRUCT(temp_arena, GltfBuffer);
      if (!GltfImageParse(json_image, buffers, image)) { goto mesh_load_glb_exit; }
      SLL_QUEUE_PUSH_BACK(images, images_tail, image, next);
    }
  }

  JsonArray json_textures;
  GltfBuffer* textures = NULL;
  if (JsonObjectGetArray(json, Str8Lit("textures"), &json_textures)) {
    JsonArrayNode* json_textures_it = json_textures.head;
    JsonValue* json_texture   = NULL;
    GltfBuffer* textures_tail = NULL;
    while ((json_texture = JsonArrayNext(&json_textures_it)) != NULL) {
      GltfBuffer* texture = ARENA_PUSH_STRUCT(temp_arena, GltfBuffer);
      if (!GltfTextureParse(json_texture, images, texture)) { goto mesh_load_glb_exit; }
      SLL_QUEUE_PUSH_BACK(textures, textures_tail, texture, next);
    }
  }

  JsonArray json_materials;
  GltfBuffer* materials = NULL;
  if (JsonObjectGetArray(json, Str8Lit("materials"), &json_materials)) {
    JsonArrayNode* json_materials_it = json_materials.head;
    JsonValue* json_material   = NULL;
    GltfBuffer* materials_tail = NULL;
    while ((json_material = JsonArrayNext(&json_materials_it)) != NULL) {
      GltfBuffer* material = ARENA_PUSH_STRUCT(temp_arena, GltfBuffer);
      if (!GltfMaterialParse(json_material, textures, material)) { goto mesh_load_glb_exit; }
      SLL_QUEUE_PUSH_BACK(materials, materials_tail, material, next);
    }
  }

  JsonArray json_meshes;
  if (!JsonObjectGetArray(json, Str8Lit("meshes"), &json_meshes)) {
    LOG_ERROR("[MESH] GLTF missing required attribute 'meshes'.");
    goto mesh_load_glb_exit;
  }
  JsonArrayNode* json_meshes_it = json_meshes.head;
  JsonValue* json_mesh          = NULL;
  while ((json_mesh = JsonArrayNext(&json_meshes_it)) != NULL) {
    Mesh* mesh;
    if (!GltfMeshParse(arena, json_mesh, accessors, materials, &mesh)) { goto mesh_load_glb_exit; }
    SLL_STACK_PUSH(model->meshes, mesh, next);
  }

  success = true;
mesh_load_glb_exit:
  ArenaRelease(temp_arena);
  if (!success) { ArenaPopTo(arena, arena_base); }
  return success;
}
#undef BIN_CATCH

B32 ModelLoad(Arena* arena, Model* model, U8* file_data, U32 file_data_size) {
  if (ModelLoadGlb(arena, model, file_data, file_data_size)) { return true; }
  if (ModelLoadObj(arena, model, file_data, file_data_size)) { return true; }
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

#undef MODEL_LOG_OUT_OF_CHARS

#endif // CDEFAULT_MODEL_IMPLEMENTATION
