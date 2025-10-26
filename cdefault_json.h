#ifndef CDEFAULT_JSON_H_
#define CDEFAULT_JSON_H_

#include "cdefault_std.h"

typedef enum JsonValueKind JsonValueKind;
enum JsonValueKind {
  JsonValueKind_String,
  JsonValueKind_Number,
  JsonValueKind_Boolean,
  JsonValueKind_Null,
  JsonValueKind_Object,
  JsonValueKind_Array,
};

typedef struct JsonObject JsonObject;
struct JsonObject {
  struct JsonObjectNode* nodes_head;
  struct JsonObjectNode* nodes_tail;
};

typedef struct JsonArray JsonArray;
struct JsonArray {
  struct JsonArrayNode* nodes_head;
  struct JsonArrayNode* nodes_tail;
};

typedef struct JsonValue JsonValue;
struct JsonValue {
  JsonValueKind kind;
  union {
    String8 string;
    F32 number;
    JsonObject object;
    JsonArray array;
    B32 boolean;
  };
};

typedef struct JsonArrayNode JsonArrayNode;
struct JsonArrayNode {
  JsonValue value;
  JsonArrayNode* next;
};

typedef struct JsonObjectNode JsonObjectNode;
struct JsonObjectNode {
  String8 key;
  JsonValue value;
  JsonObjectNode* next;
};

// NOTE: The contents of json_str are expected to outlive the json object!
B32  JsonParse(Arena* arena, JsonObject* object, String8 json_str);
void JsonToString(Arena* arena, JsonObject object, String8* json_str, B32 pretty);

B32 JsonValueGetString(JsonValue* value, String8* result);
B32 JsonValueGetNumber(JsonValue* value, F32* result);
B32 JsonValueGetBool(JsonValue* value, B32* result);
B32 JsonValueGetNull(JsonValue* value);
B32 JsonValueGetObject(JsonValue* value, JsonObject* result);
B32 JsonValueGetArray(JsonValue* value, JsonArray* result);

JsonObjectNode* JsonObjectGet(JsonObject object, String8 key);
B32  JsonObjectGetString(JsonObject object, String8 key, String8* value);
B32  JsonObjectGetNumber(JsonObject object, String8 key, F32* value);
B32  JsonObjectGetBool(JsonObject object, String8 key, B32* value);
B32  JsonObjectGetNull(JsonObject object, String8 key);
B32  JsonObjectGetObject(JsonObject object, String8 key, JsonObject* value);
B32  JsonObjectGetArray(JsonObject object, String8 key, JsonArray* value);
void JsonObjectPushString(Arena* arena, JsonObject* object, String8 key, String8 value);
void JsonObjectPushNumber(Arena* arena, JsonObject* object, String8 key, F32 value);
void JsonObjectPushBool(Arena* arena, JsonObject* object, String8 key, B32 value);
void JsonObjectPushNull(Arena* arena, JsonObject* object, String8 key);
void JsonObjectPushObject(Arena* arena, JsonObject* object, String8 key, JsonObject value);
void JsonObjectPushArray(Arena* arena, JsonObject* object, String8 key, JsonArray value);
JsonObject* JsonObjectCreateObject(Arena* arena, JsonObject* object, String8 key);
JsonArray*  JsonObjectCreateArray(Arena* arena, JsonObject* object, String8 key);

S32 JsonArraySize(JsonArray array);
JsonValue* JsonArrayGet(JsonArray array, S32 index);
B32  JsonArrayGetString(JsonArray array, S32 index, String8* result);
B32  JsonArrayGetNumber(JsonArray array, S32 index, F32* result);
B32  JsonArrayGetBool(JsonArray array, S32 index, B32* result);
B32  JsonArrayGetNull(JsonArray array, S32 index);
B32  JsonArrayGetObject(JsonArray array, S32 index, JsonObject* result);
B32  JsonArrayGetArray(JsonArray array, S32 index, JsonArray* result);
void JsonArrayPushString(Arena* arena, JsonArray* array, String8 value);
void JsonArrayPushNumber(Arena* arena, JsonArray* array, F32 value);
void JsonArrayPushBool(Arena* arena, JsonArray* array, B32 value);
void JsonArrayPushNull(Arena* arena, JsonArray* array);
void JsonArrayPushObject(Arena* arena, JsonArray* array, JsonObject value);
void JsonArrayPushArray(Arena* arena, JsonArray* array, JsonArray value);
JsonObject* JsonArrayCreateObject(Arena* arena, JsonArray* array);
JsonArray*  JsonArrayCreateArray(Arena* arena, JsonArray* array);

#endif // CDEFAULT_JSON_H_

#ifdef CDEFAULT_JSON_IMPLEMENTATION
#undef CDEFAULT_JSON_IMPLEMENTATION

// TODO: truncate string when logging?

static B32 JsonValueParse(Arena* arena, JsonValue* value, String8* orig_str, String8* json_str);
static B32 JsonObjectParse(Arena* arena, JsonObject* object, String8* orig_str, String8* json_str);
static void JsonValueAppendToStr8List(Arena* arena, JsonValue* value, String8List* json_str_list, B32 pretty, S32 indent);
static void JsonObjectAppendToStr8List(Arena* arena, JsonObject* object, String8List* json_str_list, B32 pretty, S32 indent);

#define JSON_LOG_ERROR_EX(orig_str, curr_str, fmt, ...)                                                   \
  do {                                                                                                    \
    U64 _pos = orig_str->size - curr_str->size;                                                           \
    String8 _pre  = Str8(orig_str->str, _pos);                                                            \
    String8 _post = Str8Lit("");                                                                          \
    if (orig_str->size - 1 > _pos) { _post = Str8(orig_str->str + _pos + 1, orig_str->size - _pos - 1); } \
    LOG_ERROR("[JSON] " fmt ", at pos %lu: %.*s" ANSI_COLOR_RED "%c" ANSI_COLOR_RESET "%.*s", __VA_ARGS__, _pos, _pre.size, _pre.str, orig_str->str[_pos], _post.size, _post.str); \
  } while (0);
#define JSON_LOG_ERROR(orig_str, curr_str, err)    \
  JSON_LOG_ERROR_EX(orig_str, curr_str, "%s", err)

static void JsonStrAdvance(String8* json_str, S32 size) {
  DEBUG_ASSERT(size <= json_str->size);
  json_str->str  += size;
  json_str->size -= size;
}

static B32 JsonExpectNotEmpty(String8* orig, String8* json_str) {
  *json_str = Str8TrimFront(*json_str);
  if (json_str->size == 0) {
    JSON_LOG_ERROR(orig, json_str, "Expected json string to not be empty");
    return false;
  }
  return true;
}

static B32 JsonExpectChar(String8* orig, String8* json_str, U8 expectation) {
  if (!JsonExpectNotEmpty(orig, json_str)) { return false; }
  if (!Str8StartsWithChar(*json_str, expectation)) {
    JSON_LOG_ERROR_EX(orig, json_str, "Expected character: '%c', got '%c'", expectation, json_str->str[0]);
    return false;
  }
  JsonStrAdvance(json_str, 1);
  return true;
}

static B32 JsonValueParse(Arena* arena, JsonValue* value, String8* orig_str, String8* json_str) {
  MEMORY_ZERO_STRUCT(value);
  U64 arena_pos = ArenaPos(arena);
  B32 success = false;

  if (!JsonExpectNotEmpty(orig_str, json_str)) { goto json_value_parse_end; }
  F32 number;
  S32 num_end = Str8ToF32(*json_str, &number);
  if (num_end != -1) {
    value->kind = JsonValueKind_Number;
    value->number = number;
    JsonStrAdvance(json_str, num_end);

  } else if (json_str->str[0] == '{') {
    value->kind = JsonValueKind_Object;
    if (!JsonObjectParse(arena, &value->object, orig_str, json_str)) { goto json_value_parse_end; }

  } else if (json_str->str[0] == '[') {
    value->kind = JsonValueKind_Array;
    JsonStrAdvance(json_str, 1);
    while (true) {
      if (!JsonExpectNotEmpty(orig_str, json_str)) { goto json_value_parse_end; }
      if (json_str->str[0] == ']') {
        JsonStrAdvance(json_str, 1);
        break;
      }

      JsonArrayNode* node = ARENA_PUSH_STRUCT(arena, JsonArrayNode);
      SLL_QUEUE_PUSH_BACK(value->array.nodes_head, value->array.nodes_tail, node, next);

      if (!JsonValueParse(arena, &node->value, orig_str, json_str)) { goto json_value_parse_end; }

      if (!JsonExpectNotEmpty(orig_str, json_str)) { goto json_value_parse_end; }
      if (json_str->str[0] == ',') {
        JsonStrAdvance(json_str, 1);
        continue;
      } else if (json_str->str[0] == ']') {
        continue;
      } else {
        JSON_LOG_ERROR_EX(orig_str, json_str, "Expected end of array item character ',' or ']', got: '%c'", json_str->str[0]);
        goto json_value_parse_end;
      }
    }

  } else if (json_str->str[0] == '\"') {
    value->kind = JsonValueKind_String;
    S32 str_val_end = Str8Find(*json_str, 1, Str8Lit("\""));
    if (str_val_end == -1) {
      JSON_LOG_ERROR(orig_str, json_str, "No end to json string value");
      goto json_value_parse_end;
    }
    value->string = Str8(json_str->str + 1, str_val_end - 1);
    JsonStrAdvance(json_str, value->string.size + 2);

  } else if (Str8StartsWith(*json_str, Str8Lit("true"))) {
    value->kind = JsonValueKind_Boolean;
    value->boolean = true;
    JsonStrAdvance(json_str, 4);

  } else if (Str8StartsWith(*json_str, Str8Lit("false"))) {
    value->kind = JsonValueKind_Boolean;
    value->boolean = false;
    JsonStrAdvance(json_str, 5);

  } else if (Str8StartsWith(*json_str, Str8Lit("null"))) {
    value->kind = JsonValueKind_Null;
    JsonStrAdvance(json_str, 4);

  } else {
    JSON_LOG_ERROR(orig_str, json_str, "[JSON] Failed to parse json item from str");
    goto json_value_parse_end;
  }

  success = true;
json_value_parse_end:
  if (!success) { ArenaPopTo(arena, arena_pos); }
  return success;
}

static B32 JsonObjectParse(Arena* arena, JsonObject* object, String8* orig_str, String8* json_str) {
  MEMORY_ZERO_STRUCT(object);
  U64 base_pos = ArenaPos(arena);
  B32 success = false;

  if (!JsonExpectChar(orig_str, json_str, '{')) { goto json_object_parse_end; }
  while (true) {
    if (!JsonExpectNotEmpty(orig_str, json_str)) { goto json_object_parse_end; }
    if (json_str->str[0] == '}') {
      JsonStrAdvance(json_str, 1);
      break;
    }

    JsonObjectNode* node = ARENA_PUSH_STRUCT(arena, JsonObjectNode);
    SLL_QUEUE_PUSH_BACK(object->nodes_head, object->nodes_tail, node, next);

    if (!JsonExpectChar(orig_str, json_str, '\"')) { goto json_object_parse_end; }
    U8* key_str = json_str->str;
    S32 key_end = Str8Find(*json_str, 0, Str8Lit("\""));
    if (key_end == -1) {
      JSON_LOG_ERROR(orig_str, json_str, "No end to json key");
      return false;
    }
    node->key = Str8(key_str, key_end);
    JsonStrAdvance(json_str, key_end + 1);

    if (!JsonExpectChar(orig_str, json_str, ':')) { goto json_object_parse_end; }
    if (!JsonValueParse(arena, &node->value, orig_str, json_str)) { goto json_object_parse_end; }

    if (!JsonExpectNotEmpty(orig_str, json_str)) { goto json_object_parse_end; }
    if (json_str->str[0] == ',') {
      JsonStrAdvance(json_str, 1);
      continue;
    } else if (json_str->str[0] == '}') {
      continue;
    } else {
      JSON_LOG_ERROR_EX(orig_str, json_str, "Expected end of object item character ',' or '}', got: '%c'", json_str->str[0]);
      goto json_object_parse_end;
    }
  }
  success = true;
json_object_parse_end:
  if (!success) { ArenaPopTo(arena, base_pos); }
  return success;
}

B32 JsonParse(Arena* arena, JsonObject* object, String8 json_str) {
  String8 json_str_copy = json_str;
  return JsonObjectParse(arena, object, &json_str, &json_str_copy);
}

static void JsonValueAppendToStr8List(Arena* arena, JsonValue* value, String8List* json_str_list, B32 pretty, S32 indent) {
  String8ListNode* node;
  switch (value->kind) {
    case JsonValueKind_String: {
      node = ARENA_PUSH_STRUCT(arena, String8ListNode);
      Str8ListAppend(json_str_list, node);
      node->string = Str8Format(arena, "\"%.*s\"", value->string.size, value->string.str);
    } break;
    case JsonValueKind_Number: {
      node = ARENA_PUSH_STRUCT(arena, String8ListNode);
      Str8ListAppend(json_str_list, node);
      node->string = Str8Format(arena, "%f", value->number);
    } break;
    case JsonValueKind_Object: {
      JsonObjectAppendToStr8List(arena, &value->object, json_str_list, pretty, indent);
    } break;
    case JsonValueKind_Array: {
      node = ARENA_PUSH_STRUCT(arena, String8ListNode);
      Str8ListAppend(json_str_list, node);
      node->string = Str8Format(arena, "[%s", pretty ? "\n" : "");

      S32 array_indent = indent + 2;
      for (JsonArrayNode* curr = value->array.nodes_head; curr != NULL; curr = curr->next) {
        if (pretty) {
          node = ARENA_PUSH_STRUCT(arena, String8ListNode);
          Str8ListAppend(json_str_list, node);
          node->string = Str8Format(arena, "%*s", array_indent, "");
        }

        JsonValueAppendToStr8List(arena, &curr->value, json_str_list, pretty, array_indent);

        if (curr->next != NULL) {
          node = ARENA_PUSH_STRUCT(arena, String8ListNode);
          Str8ListAppend(json_str_list, node);
          node->string = Str8Format(arena, ",%s", pretty ? "\n" : " ");
        }
      }

      node = ARENA_PUSH_STRUCT(arena, String8ListNode);
      Str8ListAppend(json_str_list, node);
      node->string = Str8Format(arena, "%s%*s]", pretty ? "\n" : "", pretty ? indent : 0, "");
    } break;
    case JsonValueKind_Boolean: {
      node = ARENA_PUSH_STRUCT(arena, String8ListNode);
      Str8ListAppend(json_str_list, node);
      if (value->boolean) { node->string = Str8Lit("true");  }
      else                { node->string = Str8Lit("false"); }
    } break;
    case JsonValueKind_Null: {
      node = ARENA_PUSH_STRUCT(arena, String8ListNode);
      Str8ListAppend(json_str_list, node);
      node->string = Str8Lit("null");
    } break;
    default: UNREACHABLE();
  }
}

static void JsonObjectAppendToStr8List(Arena* arena, JsonObject* object, String8List* json_str_list, B32 pretty, S32 indent) {
  String8ListNode* node;
  node = ARENA_PUSH_STRUCT(arena, String8ListNode);
  Str8ListAppend(json_str_list, node);
  node->string = Str8Format(arena, "{%s", pretty ? "\n" : "");

  for (JsonObjectNode* curr = object->nodes_head; curr != NULL; curr = curr->next) {
    S32 value_indent = indent + 2;

    node = ARENA_PUSH_STRUCT(arena, String8ListNode);
    Str8ListAppend(json_str_list, node);
    node->string = Str8Format(arena, "%*s\"%.*s\": ", pretty ? value_indent : 0, "", curr->key.size, curr->key.str);

    JsonValueAppendToStr8List(arena, &curr->value, json_str_list, pretty, value_indent);

    if (curr->next != NULL) {
      node = ARENA_PUSH_STRUCT(arena, String8ListNode);
      Str8ListAppend(json_str_list, node);
      node->string = Str8Format(arena, ",%s", pretty ? "\n" : "");
    }
  }

  node = ARENA_PUSH_STRUCT(arena, String8ListNode);
  Str8ListAppend(json_str_list, node);
  node->string = Str8Format(arena, "%s%*s}", pretty ? "\n" : "", pretty ? indent : 0, "");
}

void JsonToString(Arena* arena, JsonObject object, String8* json_str, B32 pretty) {
  Arena* temp_arena = ArenaAllocate();
  String8List json_str_list;
  MEMORY_ZERO_STRUCT(&json_str_list);
  JsonObjectAppendToStr8List(temp_arena, &object, &json_str_list, pretty, 0);
  *json_str = Str8ListJoin(arena, &json_str_list);
  ArenaRelease(temp_arena);
}

B32 JsonValueGetString(JsonValue* value, String8* result) {
  if (value->kind != JsonValueKind_String) { return false; }
  *result = value->string;
  return true;
}

B32 JsonValueGetNumber(JsonValue* value, F32* result) {
  if (value->kind != JsonValueKind_Number) { return false; }
  *result = value->number;
  return true;
}

B32 JsonValueGetBool(JsonValue* value, B32* result) {
  if (value->kind != JsonValueKind_Boolean) { return false; }
  *result = value->boolean;
  return true;
}

B32 JsonValueGetNull(JsonValue* value) {
  if (value->kind != JsonValueKind_Null) { return false; }
  return true;
}

B32 JsonValueGetObject(JsonValue* value, JsonObject* result) {
  if (value->kind != JsonValueKind_Object) { return false; }
  *result = value->object;
  return true;
}

B32 JsonValueGetArray(JsonValue* value, JsonArray* result) {
  if (value->kind != JsonValueKind_Array) { return false; }
  *result = value->array;
  return true;
}

JsonObjectNode* JsonObjectGet(JsonObject object, String8 key) {
  for (JsonObjectNode* node = object.nodes_head; node != NULL; node = node->next) {
    if (Str8Eq(node->key, key)) { return node; }
  }
  return NULL;
}

B32 JsonObjectGetString(JsonObject object, String8 key, String8* value) {
  JsonObjectNode* node = JsonObjectGet(object, key);
  if (node == NULL) { return false; }
  return JsonValueGetString(&node->value, value);
}

B32 JsonObjectGetNumber(JsonObject object, String8 key, F32* value) {
  JsonObjectNode* node = JsonObjectGet(object, key);
  if (node == NULL) { return false; }
  return JsonValueGetNumber(&node->value, value);
}

B32 JsonObjectGetBool(JsonObject object, String8 key, B32* value) {
  JsonObjectNode* node = JsonObjectGet(object, key);
  if (node == NULL) { return false; }
  return JsonValueGetBool(&node->value, value);
}

B32 JsonObjectGetNull(JsonObject object, String8 key) {
  JsonObjectNode* node = JsonObjectGet(object, key);
  if (node == NULL) { return false; }
  return JsonValueGetNull(&node->value);
}

B32 JsonObjectGetObject(JsonObject object, String8 key, JsonObject* value) {
  JsonObjectNode* node = JsonObjectGet(object, key);
  if (node == NULL) { return false; }
  return JsonValueGetObject(&node->value, value);
}

B32 JsonObjectGetArray(JsonObject object, String8 key, JsonArray* value) {
  JsonObjectNode* node = JsonObjectGet(object, key);
  if (node == NULL) { return false; }
  return JsonValueGetArray(&node->value, value);
}

void JsonObjectPushString(Arena* arena, JsonObject* object, String8 key, String8 value) {
  JsonObjectNode* node = ARENA_PUSH_STRUCT(arena, JsonObjectNode);
  SLL_QUEUE_PUSH_BACK(object->nodes_head, object->nodes_tail, node, next);
  node->key = key;
  node->value.kind = JsonValueKind_String;
  node->value.string = value;
}

void JsonObjectPushNumber(Arena* arena, JsonObject* object, String8 key, F32 value) {
  JsonObjectNode* node = ARENA_PUSH_STRUCT(arena, JsonObjectNode);
  SLL_QUEUE_PUSH_BACK(object->nodes_head, object->nodes_tail, node, next);
  node->key = key;
  node->value.kind = JsonValueKind_Number;
  node->value.number = value;
}

void JsonObjectPushBool(Arena* arena, JsonObject* object, String8 key, B32 value) {
  JsonObjectNode* node = ARENA_PUSH_STRUCT(arena, JsonObjectNode);
  SLL_QUEUE_PUSH_BACK(object->nodes_head, object->nodes_tail, node, next);
  node->key = key;
  node->value.kind = JsonValueKind_Boolean;
  node->value.boolean = value;
}

void JsonObjectPushNull(Arena* arena, JsonObject* object, String8 key) {
  JsonObjectNode* node = ARENA_PUSH_STRUCT(arena, JsonObjectNode);
  SLL_QUEUE_PUSH_BACK(object->nodes_head, object->nodes_tail, node, next);
  node->key = key;
  node->value.kind = JsonValueKind_Null;
}

void JsonObjectPushObject(Arena* arena, JsonObject* object, String8 key, JsonObject value) {
  JsonObjectNode* node = ARENA_PUSH_STRUCT(arena, JsonObjectNode);
  SLL_QUEUE_PUSH_BACK(object->nodes_head, object->nodes_tail, node, next);
  node->key = key;
  node->value.kind = JsonValueKind_Object;
  node->value.object = value;
}

void JsonObjectPushArray(Arena* arena, JsonObject* object, String8 key, JsonArray value) {
  JsonObjectNode* node = ARENA_PUSH_STRUCT(arena, JsonObjectNode);
  SLL_QUEUE_PUSH_BACK(object->nodes_head, object->nodes_tail, node, next);
  node->key = key;
  node->value.kind = JsonValueKind_Array;
  node->value.array = value;
}

JsonObject* JsonObjectCreateObject(Arena* arena, JsonObject* object, String8 key) {
  JsonObjectNode* node = ARENA_PUSH_STRUCT(arena, JsonObjectNode);
  SLL_QUEUE_PUSH_BACK(object->nodes_head, object->nodes_tail, node, next);
  node->key = key;
  node->value.kind = JsonValueKind_Object;
  return &node->value.object;
}

JsonArray* JsonObjectCreateArray(Arena* arena, JsonObject* object, String8 key) {
  JsonObjectNode* node = ARENA_PUSH_STRUCT(arena, JsonObjectNode);
  SLL_QUEUE_PUSH_BACK(object->nodes_head, object->nodes_tail, node, next);
  node->key = key;
  node->value.kind = JsonValueKind_Array;
  return &node->value.array;
}

S32 JsonArraySize(JsonArray array) {
  S32 size = 0;
  for (JsonArrayNode* curr = array.nodes_head; curr != NULL; curr = curr->next) { size++; }
  return size;
}

JsonValue* JsonArrayGet(JsonArray array, S32 index) {
  if (index < 0) { return NULL; }
  JsonArrayNode* curr = array.nodes_head;
  for (S32 i = 0; i < index; i++) {
    if (curr == NULL) { return NULL; }
    curr = curr->next;
  }
  return &curr->value;
}

B32 JsonArrayGetString(JsonArray array, S32 index, String8* result) {
  JsonValue* value = JsonArrayGet(array, index);
  if (value == NULL) { return false; }
  return JsonValueGetString(value, result);
}

B32 JsonArrayGetNumber(JsonArray array, S32 index, F32* result) {
  JsonValue* value = JsonArrayGet(array, index);
  if (value == NULL) { return false; }
  return JsonValueGetNumber(value, result);
}

B32 JsonArrayGetBool(JsonArray array, S32 index, B32* result) {
  JsonValue* value = JsonArrayGet(array, index);
  if (value == NULL) { return false; }
  return JsonValueGetBool(value, result);
}

B32 JsonArrayGetNull(JsonArray array, S32 index) {
  JsonValue* value = JsonArrayGet(array, index);
  if (value == NULL) { return false; }
  return JsonValueGetNull(value);
}

B32 JsonArrayGetObject(JsonArray array, S32 index, JsonObject* result) {
  JsonValue* value = JsonArrayGet(array, index);
  if (value == NULL) { return false; }
  return JsonValueGetObject(value, result);
}

B32 JsonArrayGetArray(JsonArray array, S32 index, JsonArray* result) {
  JsonValue* value = JsonArrayGet(array, index);
  if (value == NULL) { return false; }
  return JsonValueGetArray(value, result);
}

void JsonArrayPushString(Arena* arena, JsonArray* array, String8 value) {
  JsonArrayNode* node = ARENA_PUSH_STRUCT(arena, JsonArrayNode);
  SLL_QUEUE_PUSH_BACK(array->nodes_head, array->nodes_tail, node, next);
  node->value.kind = JsonValueKind_String;
  node->value.string = value;
}

void JsonArrayPushNumber(Arena* arena, JsonArray* array, F32 value) {
  JsonArrayNode* node = ARENA_PUSH_STRUCT(arena, JsonArrayNode);
  SLL_QUEUE_PUSH_BACK(array->nodes_head, array->nodes_tail, node, next);
  node->value.kind = JsonValueKind_Number;
  node->value.number = value;
}

void JsonArrayPushBool(Arena* arena, JsonArray* array, B32 value) {
  JsonArrayNode* node = ARENA_PUSH_STRUCT(arena, JsonArrayNode);
  SLL_QUEUE_PUSH_BACK(array->nodes_head, array->nodes_tail, node, next);
  node->value.kind = JsonValueKind_Boolean;
  node->value.boolean = value;
}

void JsonArrayPushNull(Arena* arena, JsonArray* array) {
  JsonArrayNode* node = ARENA_PUSH_STRUCT(arena, JsonArrayNode);
  SLL_QUEUE_PUSH_BACK(array->nodes_head, array->nodes_tail, node, next);
  node->value.kind = JsonValueKind_Null;
}

void JsonArrayPushObject(Arena* arena, JsonArray* array, JsonObject value) {
  JsonArrayNode* node = ARENA_PUSH_STRUCT(arena, JsonArrayNode);
  SLL_QUEUE_PUSH_BACK(array->nodes_head, array->nodes_tail, node, next);
  node->value.kind = JsonValueKind_Object;
  node->value.object = value;
}

void JsonArrayPushArray(Arena* arena, JsonArray* array, JsonArray value) {
  JsonArrayNode* node = ARENA_PUSH_STRUCT(arena, JsonArrayNode);
  SLL_QUEUE_PUSH_BACK(array->nodes_head, array->nodes_tail, node, next);
  node->value.kind = JsonValueKind_Array;
  node->value.array = value;
}

JsonObject* JsonArrayCreateObject(Arena* arena, JsonArray* array) {
  JsonArrayNode* node = ARENA_PUSH_STRUCT(arena, JsonArrayNode);
  SLL_QUEUE_PUSH_BACK(array->nodes_head, array->nodes_tail, node, next);
  node->value.kind = JsonValueKind_Object;
  return &node->value.object;
}

JsonArray* JsonArrayCreateArray(Arena* arena, JsonArray* array) {
  JsonArrayNode* node = ARENA_PUSH_STRUCT(arena, JsonArrayNode);
  SLL_QUEUE_PUSH_BACK(array->nodes_head, array->nodes_tail, node, next);
  node->value.kind = JsonValueKind_Array;
  return &node->value.array;
}

#undef JSON_LOG_ERROR
#undef JSON_LOG_ERROR_EX
#endif // CDEFAULT_JSON_IMPLEMENTATION
