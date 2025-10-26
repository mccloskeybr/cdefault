#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_JSON_IMPLEMENTATION
#include "../cdefault_json.h"
#define CDEFAULT_TEST_IMPLEMENTATION
#include "../cdefault_test.h"

static Arena* arena;

void JsonParseEmptyTest() {
  JsonObject json;
  EXPECT_TRUE(JsonParse(arena, &json, Str8Lit("{ }")));
  EXPECT_PTR_NULL(json.nodes_head);
  EXPECT_PTR_NULL(json.nodes_tail);
}

void JsonParseStringTest() {
  JsonObject json;
  String8 json_str;
  String8 value;

  json_str = Str8Lit("{ \"key\" : \"value\" }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetString(json, Str8Lit("key"), &value));
  EXPECT_STR8_EQ(Str8Lit("value"), value);

  json_str = Str8Lit("{ \"key\" : \"\" }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetString(json, Str8Lit("key"), &value));
  EXPECT_STR8_EQ(Str8Lit(""), value);
}

void JsonParseNumberTest() {
  JsonObject json;
  String8 json_str;
  F32 value;

  json_str = Str8Lit("{ \"key\" : 9876 }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetNumber(json, Str8Lit("key"), &value));
  EXPECT_F32_APPROX_EQ(value, 9876);

  json_str = Str8Lit("{ \"key\" : -1 }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetNumber(json, Str8Lit("key"), &value));
  EXPECT_F32_APPROX_EQ(value, -1);

  json_str = Str8Lit("{ \"key\" : +1 }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetNumber(json, Str8Lit("key"), &value));
  EXPECT_F32_APPROX_EQ(value, 1);

  json_str = Str8Lit("{ \"key\" : 123.456 }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetNumber(json, Str8Lit("key"), &value));
  EXPECT_F32_APPROX_EQ(value, 123.456f);
}

void JsonParseBoolTest() {
  JsonObject json;
  String8 json_str;
  B32 value;

  json_str = Str8Lit("{ \"key\" : true }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetBool(json, Str8Lit("key"), &value));
  EXPECT_TRUE(value);

  json_str = Str8Lit("{ \"key\" : false }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetBool(json, Str8Lit("key"), &value));
  EXPECT_FALSE(value);
}

void JsonParseNullTest() {
  JsonObject json;
  String8 json_str;

  json_str = Str8Lit("{ \"key\" : null }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetNull(json, Str8Lit("key")));
}

void JsonParseObjectTest() {
  JsonObject json;
  String8 json_str;
  JsonObject value;
  String8 inner_value;

  json_str = Str8Lit("{ \"key\" : { \"inner_key\" : \"inner_value\", \"inner_key\": \"other_value\" }}");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetObject(json, Str8Lit("key"), &value));
  EXPECT_TRUE(JsonObjectGetString(value, Str8Lit("inner_key"), &inner_value));
  EXPECT_STR8_EQ(inner_value, Str8Lit("inner_value"));
}

void JsonParseArrayTest() {
  JsonObject json;
  String8 json_str;
  JsonArray array;

  json_str = Str8Lit("{ \"key\" : [\"value_1\", 3, true ] }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetArray(json, Str8Lit("key"), &array));
  EXPECT_U32_EQ(JsonArraySize(array), 3);

  String8 str_val;
  EXPECT_TRUE(JsonArrayGetString(array, 0, &str_val));
  EXPECT_STR8_EQ(str_val, Str8Lit("value_1"));

  F32 num_val;
  EXPECT_TRUE(JsonArrayGetNumber(array, 1, &num_val));
  EXPECT_F32_APPROX_EQ(num_val, 3);

  B32 bool_val;
  EXPECT_TRUE(JsonArrayGetBool(array, 2, &bool_val));
  EXPECT_TRUE(bool_val);
}

void JsonParseMultiObjectTest() {
  JsonObject json;
  String8 json_str;
  String8 str_val;
  F32 num_val;

  json_str = Str8Lit("{ \"key_1\" : \"value\", \"key_2\": 5 }");
  EXPECT_TRUE(JsonParse(arena, &json, json_str));
  EXPECT_TRUE(JsonObjectGetString(json, Str8Lit("key_1"), &str_val));
  EXPECT_STR8_EQ(str_val, Str8Lit("value"))
  EXPECT_TRUE(JsonObjectGetNumber(json, Str8Lit("key_2"), &num_val));
  EXPECT_F32_APPROX_EQ(num_val, 5);
}

void JsonConstructEmptyTest() {
  JsonObject json;
  MEMORY_ZERO_STRUCT(&json);
  String8 json_str;
  JsonToString(arena, json, &json_str, false);
  EXPECT_STR8_EQ(json_str, Str8Lit("{}"));
}

void JsonConstructStringTest() {
  JsonObject json;
  MEMORY_ZERO_STRUCT(&json);
  JsonObjectPushString(arena, &json, Str8Lit("key"), Str8Lit("value"));

  String8 value;
  EXPECT_TRUE(JsonObjectGetString(json, Str8Lit("key"), &value));
  EXPECT_STR8_EQ(value, Str8Lit("value"));
}

void JsonConstructNumberTest() {
  JsonObject json;
  MEMORY_ZERO_STRUCT(&json);
  JsonObjectPushNumber(arena, &json, Str8Lit("key"), -456.123f);

  F32 value;
  EXPECT_TRUE(JsonObjectGetNumber(json, Str8Lit("key"), &value));
  EXPECT_F32_APPROX_EQ(value, -456.123f);
}

void JsonConstructObjectTest() {
  JsonObject json;
  MEMORY_ZERO_STRUCT(&json);
  JsonObject* inner_obj = JsonObjectCreateObject(arena, &json, Str8Lit("key"));
  JsonObjectPushString(arena, inner_obj, Str8Lit("inner_key"), Str8Lit("inner_value"));

  JsonObject test_inner_obj;
  EXPECT_TRUE(JsonObjectGetObject(json, Str8Lit("key"), &test_inner_obj));
  String8 value;
  EXPECT_TRUE(JsonObjectGetString(test_inner_obj, Str8Lit("inner_key"), &value));
  EXPECT_STR8_EQ(value, Str8Lit("inner_value"));
}

void JsonConstructArrayTest() {
  JsonObject json;
  MEMORY_ZERO_STRUCT(&json);
  JsonArray* array = JsonObjectCreateArray(arena, &json, Str8Lit("key"));
  JsonArrayPushString(arena, array, Str8Lit("inner_value"));

  JsonArray test_array;
  EXPECT_TRUE(JsonObjectGetArray(json, Str8Lit("key"), &test_array));
  String8 value;
  EXPECT_TRUE(JsonArrayGetString(test_array, 0, &value));
  EXPECT_STR8_EQ(value, Str8Lit("inner_value"));
}

void JsonConstructBoolTest() {
  JsonObject json;
  MEMORY_ZERO_STRUCT(&json);
  JsonObjectPushBool(arena, &json, Str8Lit("key"), true);

  B32 value;
  EXPECT_TRUE(JsonObjectGetBool(json, Str8Lit("key"), &value));
  EXPECT_TRUE(value);
}

void JsonConstructNullTest() {
  JsonObject json;
  MEMORY_ZERO_STRUCT(&json);
  JsonObjectPushNull(arena, &json, Str8Lit("key"));

  EXPECT_TRUE(JsonObjectGetNull(json, Str8Lit("key")));
}

int main(void) {
  arena = ArenaAllocate();
  RUN_TEST(JsonParseEmptyTest);
  RUN_TEST(JsonParseStringTest);
  RUN_TEST(JsonParseNumberTest);
  RUN_TEST(JsonParseBoolTest);
  RUN_TEST(JsonParseNullTest);
  RUN_TEST(JsonParseObjectTest);
  RUN_TEST(JsonParseArrayTest);
  RUN_TEST(JsonParseMultiObjectTest);
  RUN_TEST(JsonConstructEmptyTest);
  RUN_TEST(JsonConstructStringTest);
  RUN_TEST(JsonConstructNumberTest);
  RUN_TEST(JsonConstructObjectTest);
  RUN_TEST(JsonConstructArrayTest);
  RUN_TEST(JsonConstructBoolTest);
  RUN_TEST(JsonConstructNullTest);
  LogTestReport();
  return 0;
}
