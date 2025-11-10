#ifndef CDEFAULT_JSON_H_
#define CDEFAULT_JSON_H_

#include "cdefault_std.h"

// TODO: truncate string when logging / better error messages?
// TODO: metaprogramming for struct -> json serialization / deserialization?

// NOTE: this is a simple json serialization / deserialization library, specifically for self-described schemas (rather than parsing arbitrary json blobs).
// it encourages reading and writing json values in a straightforward, procedural way.
//
// e.g. Reading { "key": "value", "inner": { "x": 1, "y": 2 } }
//
// JsonObject json; JsonParse(arena, &json, json_string);
// JsonObjectGetString(&json, Str8Lit("key"), &value); // NOTE: Get methods return false if key is not present.
// JsonObject inner;
// JsonObjectGetObject(&json, Str8Lit("inner"), &inner);
// F32 x, y;
// JsonObjectGetNumber(&inner, Str8Lit("x"), &x);
// JsonObjectGetNumber(&inner, Str8Lit("y"), &y);
//
// e.g. Writing { "key": "value", "inner": { "x": 1, "y": 2 } }
//
// JsonObject json;
// JsonObjectPush(arena, &json, Str8Lit("key"), Str8Lit("value"));
// JsonObject* inner = JsonObjectCreateObject(arena, &json, Str8Lit("inner");
// JsonObjectPushNumber(arena, inner, Str8Lit("x"), 1);
// JsonObjectPushNumber(arena, inner, Str8Lit("y"), 2);
// JsonToString(arena, json, &json_string, true);
//
// there are extra convenience funcs for cdefault native types, and the api can be easily extended for additional types. e.g.
//
// typedef struct MyStruct MyStruct;
// struct MyStruct { F32 x, y; }
//
// JsonValue JsonValueMyStruct(Arena* arena, MyStruct my_struct) {
//   JsonValue result = JsonValueObjectEmpty();
//   JsonObjectPushNumber(arena, &result.object, Str8Lit("x"), my_struct.x);
//   JsonObjectPushNumber(arena, &result.object, Str8Lit("y"), my_struct.y);
//   return result;
// }
// B32 JsonValueGetMyStruct(JsonValue* value, MyStruct* result) {
//   JsonObject obj;
//   if (!JsonValueGetObject(value, &obj)                     { return false; } // value is not an object, e.g. "key": "string, not an object"
//   if (!JsonObjectGetNumber(obj, Str8Lit("x"), &result->x)) { return false; } // value has no 'x' property.
//   if (!JsonObjectGetNumber(obj, Str8Lit("y"), &result->y)) { return false; } // value has no 'y' property.
//  return true;
// }
//
// To read:  DEBUG_ASSERT(JsonValueGetMyStruct(JsonObjectGet(json_object, Str8Lit("my_key")), &my_struct));
// To write: JsonObjectPushValue(arena, &json_object, Str8Lit("my_key"), JsonValueMyStruct(arena, my_struct));

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
  struct JsonObjectNode* head;
  struct JsonObjectNode* tail;
};

typedef struct JsonArray JsonArray;
struct JsonArray {
  struct JsonArrayNode* head;
  struct JsonArrayNode* tail;
};

typedef struct JsonValue JsonValue;
struct JsonValue {
  JsonValueKind kind;
  union {
    String8 string;
    F32 number;
    B32 boolean;
    JsonObject object;
    JsonArray array;
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

JsonValue JsonValueString(String8 string);
JsonValue JsonValueNumber(F32 number);
JsonValue JsonValueBoolean(B32 boolean);
JsonValue JsonValueNull();
JsonValue JsonValueObject(JsonObject object);
JsonValue JsonValueObjectEmpty();
JsonValue JsonValueArray(JsonArray array);
JsonValue JsonValueArrayEmpty();
JsonValue JsonValueV2(Arena* arena, F32 v2[2]);
JsonValue JsonValueV3(Arena* arena, F32 v3[3]);
JsonValue JsonValueV4(Arena* arena, F32 v4[4]);

B32 JsonValueGetString(JsonValue* value, String8* result);
B32 JsonValueGetNumber(JsonValue* value, F32* result);
B32 JsonValueGetBool(JsonValue* value, B32* result);
B32 JsonValueGetNull(JsonValue* value);
B32 JsonValueGetObject(JsonValue* value, JsonObject* result);
B32 JsonValueGetArray(JsonValue* value, JsonArray* result);
B32 JsonValueGetV2(JsonValue* value, F32 (*result)[2]);
B32 JsonValueGetV3(JsonValue* value, F32 (*result)[3]);
B32 JsonValueGetV4(JsonValue* value, F32 (*result)[4]);

// NOTE: json objects store values in a linked list internally. this makes lookups via the Get API slow for sufficiently large objects.
// you might prefer some other scheme if performance is important, such as hardcoding the layout of the object in your logic directly:
//   JsonObjectNode* node = json_object.head;
//   DEBUG_ASSERT(JsonValueGetString(JsonObjectNext(&node, Str8Lit("my_string")), &struct.my_string));
//   DEBUG_ASSERT(JsonValueGetNumber(JsonObjectNext(&node, Str8Lit("my_float")),  &struct.my_float));
JsonValue* JsonObjectNext(JsonObjectNode** curr, String8 expected_key); // NOTE: Returns NULL if the key does not match.
JsonValue* JsonObjectGet(JsonObject object, String8 key);               // NOTE: Returns NULL if the key is not present.
B32 JsonObjectGetString(JsonObject object, String8 key, String8* value);
B32 JsonObjectGetNumber(JsonObject object, String8 key, F32* value);
B32 JsonObjectGetBool(JsonObject object, String8 key, B32* value);
B32 JsonObjectGetNull(JsonObject object, String8 key);
B32 JsonObjectGetObject(JsonObject object, String8 key, JsonObject* value);
B32 JsonObjectGetArray(JsonObject object, String8 key, JsonArray* value);
B32 JsonObjectGetV2(JsonObject object, String8 key, F32 (*value)[2]);
B32 JsonObjectGetV3(JsonObject object, String8 key, F32 (*value)[3]);
B32 JsonObjectGetV4(JsonObject object, String8 key, F32 (*value)[4]);

JsonObject* JsonObjectCreateObject(Arena* arena, JsonObject* object, String8 key);
JsonArray*  JsonObjectCreateArray(Arena* arena, JsonObject* object, String8 key);
void JsonObjectPushValue(Arena* arena, JsonObject* object, String8 key, JsonValue value);
void JsonObjectPushString(Arena* arena, JsonObject* object, String8 key, String8 value);
void JsonObjectPushNumber(Arena* arena, JsonObject* object, String8 key, F32 value);
void JsonObjectPushBool(Arena* arena, JsonObject* object, String8 key, B32 value);
void JsonObjectPushNull(Arena* arena, JsonObject* object, String8 key);
void JsonObjectPushObject(Arena* arena, JsonObject* object, String8 key, JsonObject value);
void JsonObjectPushArray(Arena* arena, JsonObject* object, String8 key, JsonArray value);
void JsonObjectPushV2(Arena* arena, JsonObject* object, String8 key, F32 v[2]);
void JsonObjectPushV3(Arena* arena, JsonObject* object, String8 key, F32 v[3]);
void JsonObjectPushV4(Arena* arena, JsonObject* object, String8 key, F32 v[4]);

// NOTE: json arrays are stored as a linked list internally. prefer iterating their contents like e.g.:
// JsonArrayNode* curr = json_array.head;
// JsonValue* value    = NULL;
// while (value = JsonArrayNext(&curr)) { ... }
U32 JsonArraySize(JsonArray array);
JsonValue* JsonArrayNext(JsonArrayNode** curr);
JsonValue* JsonArrayGet(JsonArray array, S32 index); // NOTE: returns NULL if index is OOB.

JsonObject* JsonArrayCreateObject(Arena* arena, JsonArray* array);
JsonArray*  JsonArrayCreateArray(Arena* arena, JsonArray* array);
void JsonArrayPushValue(Arena* arena, JsonArray* array, JsonValue value);
void JsonArrayPushString(Arena* arena, JsonArray* array, String8 value);
void JsonArrayPushNumber(Arena* arena, JsonArray* array, F32 value);
void JsonArrayPushBool(Arena* arena, JsonArray* array, B32 value);
void JsonArrayPushNull(Arena* arena, JsonArray* array);
void JsonArrayPushObject(Arena* arena, JsonArray* array, JsonObject value);
void JsonArrayPushArray(Arena* arena, JsonArray* array, JsonArray value);
void JsonArrayPushV2(Arena* arena, JsonArray* array, F32 v[2]);
void JsonArrayPushV3(Arena* arena, JsonArray* array, F32 v[3]);
void JsonArrayPushV4(Arena* arena, JsonArray* array, F32 v[4]);

#endif // CDEFAULT_JSON_H_

#ifdef CDEFAULT_JSON_IMPLEMENTATION
#undef CDEFAULT_JSON_IMPLEMENTATION

static B32  JsonValueParse(Arena* arena, JsonValue* value, String8* orig_str, String8* json_str);
static B32  JsonObjectParse(Arena* arena, JsonObject* object, String8* orig_str, String8* json_str);
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
      SLL_QUEUE_PUSH_BACK(value->array.head, value->array.tail, node, next);

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
    SLL_QUEUE_PUSH_BACK(object->head, object->tail, node, next);

    if (!JsonExpectChar(orig_str, json_str, '\"')) { goto json_object_parse_end; }
    U8* key_str = json_str->str;
    S32 key_end = Str8Find(*json_str, 0, Str8Lit("\""));
    if (key_end == -1) {
      JSON_LOG_ERROR(orig_str, json_str, "No end to json key");
      goto json_object_parse_end;
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
      for (JsonArrayNode* curr = value->array.head; curr != NULL; curr = curr->next) {
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

  for (JsonObjectNode* curr = object->head; curr != NULL; curr = curr->next) {
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

JsonValue JsonValueString(String8 string) {
  JsonValue result;
  MEMORY_ZERO_STRUCT(&result);
  result.kind = JsonValueKind_String;
  result.string = string;
  return result;
}

JsonValue JsonValueNumber(F32 number) {
  JsonValue result;
  MEMORY_ZERO_STRUCT(&result);
  result.kind = JsonValueKind_Number;
  result.number = number;
  return result;
}

JsonValue JsonValueBoolean(B32 boolean) {
  JsonValue result;
  MEMORY_ZERO_STRUCT(&result);
  result.kind = JsonValueKind_Boolean;
  result.boolean = boolean;
  return result;
}

JsonValue JsonValueNull() {
  JsonValue result;
  MEMORY_ZERO_STRUCT(&result);
  result.kind = JsonValueKind_Null;
  return result;
}

JsonValue JsonValueObject(JsonObject object) {
  JsonValue result;
  MEMORY_ZERO_STRUCT(&result);
  result.kind = JsonValueKind_Object;
  result.object = object;
  return result;
}

JsonValue JsonValueObjectEmpty() {
  JsonValue result;
  MEMORY_ZERO_STRUCT(&result);
  result.kind = JsonValueKind_Object;
  return result;
}

JsonValue JsonValueArray(JsonArray array) {
  JsonValue result;
  MEMORY_ZERO_STRUCT(&result);
  result.kind = JsonValueKind_Array;
  result.array = array;
  return result;
}

JsonValue JsonValueArrayEmpty() {
  JsonValue result;
  MEMORY_ZERO_STRUCT(&result);
  result.kind = JsonValueKind_Array;
  return result;
}

JsonValue JsonValueV2(Arena* arena, F32 v2[2]) {
  JsonValue result = JsonValueObjectEmpty();
  JsonObjectPushNumber(arena, &result.object, Str8Lit("x"), v2[0]);
  JsonObjectPushNumber(arena, &result.object, Str8Lit("y"), v2[1]);
  return result;
}

JsonValue JsonValueV3(Arena* arena, F32 v3[3]) {
  JsonValue result = JsonValueObjectEmpty();
  JsonObjectPushNumber(arena, &result.object, Str8Lit("x"), v3[0]);
  JsonObjectPushNumber(arena, &result.object, Str8Lit("y"), v3[1]);
  JsonObjectPushNumber(arena, &result.object, Str8Lit("z"), v3[2]);
  return result;
}

JsonValue JsonValueV4(Arena* arena, F32 v4[4]) {
  JsonValue result = JsonValueObjectEmpty();
  JsonObjectPushNumber(arena, &result.object, Str8Lit("x"), v4[0]);
  JsonObjectPushNumber(arena, &result.object, Str8Lit("y"), v4[1]);
  JsonObjectPushNumber(arena, &result.object, Str8Lit("z"), v4[2]);
  JsonObjectPushNumber(arena, &result.object, Str8Lit("w"), v4[3]);
  return result;
}

B32 JsonValueGetString(JsonValue* value, String8* result) {
  if (value == NULL)                       { return false; }
  if (value->kind != JsonValueKind_String) { return false; }
  *result = value->string;
  return true;
}

B32 JsonValueGetNumber(JsonValue* value, F32* result) {
  if (value == NULL)                       { return false; }
  if (value->kind != JsonValueKind_Number) { return false; }
  *result = value->number;
  return true;
}

B32 JsonValueGetBool(JsonValue* value, B32* result) {
  if (value == NULL)                        { return false; }
  if (value->kind != JsonValueKind_Boolean) { return false; }
  *result = value->boolean;
  return true;
}

B32 JsonValueGetNull(JsonValue* value) {
  if (value == NULL)                     { return false; }
  if (value->kind != JsonValueKind_Null) { return false; }
  return true;
}

B32 JsonValueGetObject(JsonValue* value, JsonObject* result) {
  if (value == NULL)                       { return false; }
  if (value->kind != JsonValueKind_Object) { return false; }
  *result = value->object;
  return true;
}

B32 JsonValueGetArray(JsonValue* value, JsonArray* result) {
  if (value == NULL)                      { return false; }
  if (value->kind != JsonValueKind_Array) { return false; }
  *result = value->array;
  return true;
}

B32 JsonValueGetV2(JsonValue* value, F32 (*result)[2]) {
  JsonObject v_obj;
  if (!JsonValueGetObject(value, &v_obj))                       { return false; }
  if (!JsonObjectGetNumber(v_obj, Str8Lit("x"), &(*result)[0])) { return false; }
  if (!JsonObjectGetNumber(v_obj, Str8Lit("y"), &(*result)[1])) { return false; }
  return true;
}

B32 JsonValueGetV3(JsonValue* value, F32 (*result)[3]) {
  JsonObject v_obj;
  if (!JsonValueGetObject(value, &v_obj))                       { return false; }
  if (!JsonObjectGetNumber(v_obj, Str8Lit("x"), &(*result)[0])) { return false; }
  if (!JsonObjectGetNumber(v_obj, Str8Lit("y"), &(*result)[1])) { return false; }
  if (!JsonObjectGetNumber(v_obj, Str8Lit("z"), &(*result)[2])) { return false; }
  return true;
}

B32 JsonValueGetV4(JsonValue* value, F32 (*result)[4]) {
  JsonObject v_obj;
  if (!JsonValueGetObject(value, &v_obj))                       { return false; }
  if (!JsonObjectGetNumber(v_obj, Str8Lit("x"), &(*result)[0])) { return false; }
  if (!JsonObjectGetNumber(v_obj, Str8Lit("y"), &(*result)[1])) { return false; }
  if (!JsonObjectGetNumber(v_obj, Str8Lit("z"), &(*result)[2])) { return false; }
  if (!JsonObjectGetNumber(v_obj, Str8Lit("w"), &(*result)[3])) { return false; }
  return true;
}

JsonValue* JsonObjectNext(JsonObjectNode** curr, String8 expected_key) {
  if (*curr == NULL)                       { return NULL; }
  if (!Str8Eq((*curr)->key, expected_key)) { return NULL; }
  JsonValue* value = &(*curr)->value;
  *curr = (*curr)->next;
  return value;
}

JsonValue* JsonObjectGet(JsonObject object, String8 key) {
  for (JsonObjectNode* node = object.head; node != NULL; node = node->next) {
    if (Str8Eq(node->key, key)) { return &node->value; }
  }
  return NULL;
}

B32 JsonObjectGetString(JsonObject object, String8 key, String8* value) {
  return JsonValueGetString(JsonObjectGet(object, key), value);
}

B32 JsonObjectGetNumber(JsonObject object, String8 key, F32* value) {
  return JsonValueGetNumber(JsonObjectGet(object, key), value);
}

B32 JsonObjectGetBool(JsonObject object, String8 key, B32* value) {
  return JsonValueGetBool(JsonObjectGet(object, key), value);
}

B32 JsonObjectGetNull(JsonObject object, String8 key) {
  return JsonValueGetNull(JsonObjectGet(object, key));
}

B32 JsonObjectGetObject(JsonObject object, String8 key, JsonObject* value) {
  return JsonValueGetObject(JsonObjectGet(object, key), value);
}

B32 JsonObjectGetArray(JsonObject object, String8 key, JsonArray* value) {
  return JsonValueGetArray(JsonObjectGet(object, key), value);
}

B32 JsonObjectGetV2(JsonObject object, String8 key, F32 (*value)[2]) {
  return JsonValueGetV2(JsonObjectGet(object, key), value);
}

B32 JsonObjectGetV3(JsonObject object, String8 key, F32 (*value)[3]) {
  return JsonValueGetV3(JsonObjectGet(object, key), value);
}

B32 JsonObjectGetV4(JsonObject object, String8 key, F32 (*value)[4]) {
  return JsonValueGetV4(JsonObjectGet(object, key), value);
}

static JsonValue* JsonObjectCreateValue(Arena* arena, JsonObject* object, String8 key) {
  JsonObjectNode* node = ARENA_PUSH_STRUCT(arena, JsonObjectNode);
  MEMORY_ZERO_STRUCT(node);
  SLL_QUEUE_PUSH_BACK(object->head, object->tail, node, next);
  node->key = key;
  return &node->value;
}

JsonObject* JsonObjectCreateObject(Arena* arena, JsonObject* object, String8 key) {
  JsonValue* value = JsonObjectCreateValue(arena, object, key);
  *value = JsonValueObjectEmpty();
  return &value->object;
}

JsonArray* JsonObjectCreateArray(Arena* arena, JsonObject* object, String8 key) {
  JsonValue* value = JsonObjectCreateValue(arena, object, key);
  *value = JsonValueArrayEmpty();
  return &value->array;
}

void JsonObjectPushValue(Arena* arena, JsonObject* object, String8 key, JsonValue value) {
  JsonValue* json_value = JsonObjectCreateValue(arena, object, key);
  *json_value = value;
}

void JsonObjectPushString(Arena* arena, JsonObject* object, String8 key, String8 value) {
  JsonObjectPushValue(arena, object, key, JsonValueString(value));
}

void JsonObjectPushNumber(Arena* arena, JsonObject* object, String8 key, F32 value) {
  JsonObjectPushValue(arena, object, key, JsonValueNumber(value));
}

void JsonObjectPushBool(Arena* arena, JsonObject* object, String8 key, B32 value) {
  JsonObjectPushValue(arena, object, key, JsonValueBoolean(value));
}

void JsonObjectPushNull(Arena* arena, JsonObject* object, String8 key) {
  JsonObjectPushValue(arena, object, key, JsonValueNull());
}

void JsonObjectPushObject(Arena* arena, JsonObject* object, String8 key, JsonObject value) {
  JsonObjectPushValue(arena, object, key, JsonValueObject(value));
}

void JsonObjectPushArray(Arena* arena, JsonObject* object, String8 key, JsonArray value) {
  JsonObjectPushValue(arena, object, key, JsonValueArray(value));
}

void JsonObjectPushV2(Arena* arena, JsonObject* object, String8 key, F32 v[2]) {
  JsonObjectPushValue(arena, object, key, JsonValueV2(arena, v));
}

void JsonObjectPushV3(Arena* arena, JsonObject* object, String8 key, F32 v[3]) {
  JsonObjectPushValue(arena, object, key, JsonValueV3(arena, v));
}

void JsonObjectPushV4(Arena* arena, JsonObject* object, String8 key, F32 v[4]) {
  JsonObjectPushValue(arena, object, key, JsonValueV4(arena, v));
}

U32 JsonArraySize(JsonArray array) {
  U32 size = 0;
  for (JsonArrayNode* curr = array.head; curr != NULL; curr = curr->next) { size++; }
  return size;
}

JsonValue* JsonArrayNext(JsonArrayNode** curr) {
  if (*curr == NULL) { return NULL; }
  JsonValue* value = &(*curr)->value;
  *curr = (*curr)->next;
  return value;
}

JsonValue* JsonArrayGet(JsonArray array, S32 index) {
  if (index < 0) { return NULL; }
  JsonArrayNode* curr = array.head;
  for (S32 i = 0; i < index; i++) {
    if (curr == NULL) { return NULL; }
    curr = curr->next;
  }
  return &curr->value;
}

static JsonValue* JsonArrayCreateValue(Arena* arena, JsonArray* array) {
  JsonArrayNode* node = ARENA_PUSH_STRUCT(arena, JsonArrayNode);
  MEMORY_ZERO_STRUCT(node);
  SLL_QUEUE_PUSH_BACK(array->head, array->tail, node, next);
  return &node->value;
}

JsonObject* JsonArrayCreateObject(Arena* arena, JsonArray* array) {
  JsonValue* value = JsonArrayCreateValue(arena, array);
  *value = JsonValueObjectEmpty();
  return &value->object;
}

JsonArray* JsonArrayCreateArray(Arena* arena, JsonArray* array) {
  JsonValue* value = JsonArrayCreateValue(arena, array);
  *value = JsonValueArrayEmpty();
  return &value->array;
}

void JsonArrayPushValue(Arena* arena, JsonArray* array, JsonValue value) {
  JsonValue* json_value = JsonArrayCreateValue(arena, array);
  *json_value = value;
}

void JsonArrayPushString(Arena* arena, JsonArray* array, String8 value) {
  JsonArrayPushValue(arena, array, JsonValueString(value));
}

void JsonArrayPushNumber(Arena* arena, JsonArray* array, F32 value) {
  JsonArrayPushValue(arena, array, JsonValueNumber(value));
}

void JsonArrayPushBool(Arena* arena, JsonArray* array, B32 value) {
  JsonArrayPushValue(arena, array, JsonValueBoolean(value));
}

void JsonArrayPushNull(Arena* arena, JsonArray* array) {
  JsonArrayPushValue(arena, array, JsonValueNull());
}

void JsonArrayPushObject(Arena* arena, JsonArray* array, JsonObject value) {
  JsonArrayPushValue(arena, array, JsonValueObject(value));
}

void JsonArrayPushArray(Arena* arena, JsonArray* array, JsonArray value) {
  JsonArrayPushValue(arena, array, JsonValueArray(value));
}

void JsonArrayPushV2(Arena* arena, JsonArray* array, F32 v[2]) {
  JsonArrayPushValue(arena, array, JsonValueV2(arena, v));
}

void JsonArrayPushV3(Arena* arena, JsonArray* array, F32 v[3]) {
  JsonArrayPushValue(arena, array, JsonValueV3(arena, v));
}

void JsonArrayPushV4(Arena* arena, JsonArray* array, F32 v[4]) {
  JsonArrayPushValue(arena, array, JsonValueV4(arena, v));
}

#undef JSON_LOG_ERROR
#undef JSON_LOG_ERROR_EX
#endif // CDEFAULT_JSON_IMPLEMENTATION
