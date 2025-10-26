#ifndef CDEFAULT_TEST_H_
#define CDEFAULT_TEST_H_

#include "cdefault_std.h"

// Example setup:
// void MyTest1() {
//   U32 x = 1;
//   EXPECT_U32_EQ(x, 1); // Will succeed.
// }
// void MyTest2() {
//   U32 x = 1;
//   EXPECT_U32_EQ(x, 2); // Will fail.
// }
// int main(void) {
//   RUN_TEST(MyTest1);
//   RUN_TEST(MyTest2);
//   LogTestReport();
// }

// NOTE: All test functions must have this signature.
typedef void Test_Fn();

void LogTestReport(); // NOTE: Prints the final test report to stdout. Must be called in main.
#define RUN_TEST(test) _RunTest(Str8CStr(FILENAME), Str8Lit(#test), test) // NOTE: Each test must be invoked in main via RUN_TEST(test).

// NOTE: These must be called from test functions directly, to signify expectations / behavior on error.
// NOTE: Failed expectations will exit the test immediately.
#define EXPECT_TRUE(result)         EXPECT_BASE(result, "Expected true, but got false: %s", #result)
#define EXPECT_FALSE(result)        EXPECT_BASE(!(result), "Expected false, but got true: %s", #result)
#define EXPECT_S8_EQ(a, b)          EXPECT_BASE((a) == (b), "Expected S8s equal, but got: %d, %d", (a), (b))
#define EXPECT_S16_EQ(a, b)         EXPECT_BASE((a) == (b), "Expected S16s equal, but got: %d, %d", (a), (b))
#define EXPECT_S32_EQ(a, b)         EXPECT_BASE((a) == (b), "Expected S32s equal, but got: %d, %d", (a), (b))
#define EXPECT_S64_EQ(a, b)         EXPECT_BASE((a) == (b), "Expected S64s equal, but got: %ld, %ld", (a), (b))
#define EXPECT_U8_EQ(a, b)          EXPECT_BASE((a) == (b), "Expected U8s equal, but got: %u, %u", (a), (b))
#define EXPECT_U16_EQ(a, b)         EXPECT_BASE((a) == (b), "Expected U16s equal, but got: %u, %u", (a), (b))
#define EXPECT_U32_EQ(a, b)         EXPECT_BASE((a) == (b), "Expected U32s equal, but got: %u, %u", (a), (b))
#define EXPECT_U64_EQ(a, b)         EXPECT_BASE((a) == (b), "Expected U64s equal, but got: %lu, %lu", (a), (b))
#define EXPECT_F32_EQ(a, b)         EXPECT_BASE((a) == (b), "Expected F32s equal, but got: %.5f, %.5f", (a), (b))
#define EXPECT_F32_APPROX_EQ(a, b)  EXPECT_BASE(F32ApproxEq(a, b), "Expected F32s approximately equal, but got: %.5f, %.5f", (a), (b))
#define EXPECT_F64_EQ(a, b)         EXPECT_BASE((a) == (b), "Expected F32s equal, but got: %.5lf, %.5lf", (a), (b))
#define EXPECT_F64_APPROX_EQ(a, b)  EXPECT_BASE(F64ApproxEq(a, b), "Expected F32s approximately equal, but got: %.5lf, %.5lf", (a), (b))
#define EXPECT_PTR_EQ(a, b)         EXPECT_BASE((a) == (b), "Expected pointers equal, but got: %p, %p", a, b)
#define EXPECT_PTR_NULL(a)          EXPECT_BASE((a) == NULL, "Expected NULL, but got: %p", a)
#define EXPECT_PTR_NOT_NULL(a)      EXPECT_BASE((a) != NULL, "Expected not NULL, but got: %p", a)
#define EXPECT_CSTR_EQ(a, b)        EXPECT_BASE(CStrEq((U8*) (a), (U8*) (b)), "Expected C Strings equal, but got: '%s', '%s'", (a), (b))
#define EXPECT_STR8_EQ(a, b)        EXPECT_BASE(Str8Eq(a, b), "Expected String8s equal, but got: '%.*s', '%.*s'", (a).size, (a).str, (b).size, (b).str)
#define EXPECT_V2_EQ(a, b)          EXPECT_BASE((a).x == (b).x && (a).y == (b).y, "Expected V2s equal, but got: { %.5f, %.5f }, { %.5f, %.5f }", (a).x, (a).y, (b).x, (b).y)
#define EXPECT_V2_APPROX_EQ(a, b)   EXPECT_BASE(F32ApproxEq((a).x, (b).x) && F32ApproxEq((a).y, (b).y), "Expected V2s approximately equal, but got: { %.5f, %.5f }, { %.5f, %.5f }", (a).x, (a).y, (b).x, (b).y)
#define EXPECT_V3_EQ(a, b)          EXPECT_BASE((a).x == (b).x && (a).y == (b).y && (a).z == (b).z, "Expected V3s equal, but got: { %.5f, %.5f, %.5f }, { %.5f, %.5f, %.5f }", (a).x, (a).y, (a).z, (b).x, (b).y, (b).z)
#define EXPECT_V3_APPROX_EQ(a, b)   EXPECT_BASE(F32ApproxEq((a).x, (b).x) && F32ApproxEq((a).y, (b).y) && F32ApproxEq((a).z, (b).z), "Expected V3s approximately equal, but got: { %.5f, %.5f, %.5f }, { %.5f, %.5f, %.5f }", (a).x, (a).y, (a).z, (b).x, (b).y, (b).z)
#define EXPECT_V4_EQ(a, b)          EXPECT_BASE((a).x == (b).x && (a).y == (b).y && (a).z == (b).z && (a).w == (b).w, "Expected V4s equal, but got: { %.5f, %.5f, %.5f, %.5f }, { %.5f, %.5f, %.5f, %.5f }", (a).x, (a).y, (x).z, (a).w, (b).x, (b).y, (b).z, (y).w)
#define EXPECT_V4_APPROX_EQ(a, b)   EXPECT_BASE(F32ApproxEq((a).x, (b).x) && F32ApproxEq((a).y, (b).y) && F32ApproxEq((a).z, (b).z) && F32ApproxEq((a).w, (b).w), "Expected V4s approximately equal, but got: { %.5f, %.5f, %.5f, %.5f }, { %.5f, %.5f, %.5f, %.5f }", (a).x, (a).y, (x).z, (a).w, (b).x, (b).y, (b).z, (y).w)
#define FAIL(...)                   EXPECT_BASE(false, __VA_ARGS__)
#define PASS()                      return

// NOTE: Custom expectations can use EXPECT_BASE as a foundation.
#define EXPECT_BASE(result, ...)                            \
  if (!(result)) {                                          \
    TestContext* _c = &_cdef_test_context;                  \
    _c->failed = true;                                      \
    _c->error_message = Str8Format(_c->arena, __VA_ARGS__); \
    _c->file = (U8*) __FILE__;                              \
    _c->line = __LINE__;                                    \
    return;                                                 \
  }

void _RunTest(String8 runner_file_name, String8 test_name, Test_Fn* test_fn);

#endif // CDEFAULT_TEST_H_

#ifdef CDEFAULT_TEST_IMPLEMENTATION
#undef CDEFAULT_TEST_IMPLEMENTATION

typedef struct TestContext TestContext;
struct TestContext {
  Arena* arena;
  B32 is_initialized;
  U32 num_pass;
  U32 num_fail;
  String8 runner_file_name;
  String8List report;

  B32 failed;
  String8 error_message;
  U8* file;
  U32 line;
};
static TestContext _cdef_test_context;

static void TestInit(String8 runner_file_name) {
  TestContext* c = &_cdef_test_context;
  MEMORY_ZERO_STRUCT(c);
  c->arena = ArenaAllocate();
  c->runner_file_name = runner_file_name;
  c->is_initialized = true;
}

void _RunTest(String8 runner_file_name, String8 test_name, Test_Fn* test_fn) {
  TestContext* c = &_cdef_test_context;
  if (!c->is_initialized) { TestInit(runner_file_name); }

  test_fn();
  if (c->failed) {
    c->num_fail++;

    String8ListNode* test_result = ARENA_PUSH_STRUCT(c->arena, String8ListNode);
    test_result->string = Str8Format(c->arena, "[" ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "]: %.*s\n", test_name.size, test_name.str);
    Str8ListAppend(&c->report, test_result);

    String8ListNode* error_entry = ARENA_PUSH_STRUCT(c->arena, String8ListNode);
    error_entry->string = Str8Format(c->arena, "\t%s:%d - %.*s\n", c->file, c->line, c->error_message.size, c->error_message.str);
    Str8ListAppend(&c->report, error_entry);
    c->failed = false;

  } else {
    c->num_pass++;

    String8ListNode* test_result = ARENA_PUSH_STRUCT(c->arena, String8ListNode);
    test_result->string = Str8Format(c->arena, "[" ANSI_COLOR_GREEN "PASS" ANSI_COLOR_RESET "]: %.*s\n", test_name.size, test_name.str);
    Str8ListAppend(&c->report, test_result);
  }
}

void LogTestReport() {
  TestContext* c = &_cdef_test_context;

  String8ListNode* report_prefix = ARENA_PUSH_STRUCT(c->arena, String8ListNode);
  report_prefix->string = Str8Format(c->arena, ANSI_COLOR_YELLOW "%.*s\n" ANSI_COLOR_RESET, c->runner_file_name.size, c->runner_file_name.str);
  Str8ListPrepend(&c->report, report_prefix);

  String8ListNode* report_suffix = ARENA_PUSH_STRUCT(c->arena, String8ListNode);
  report_suffix->string = Str8Format(c->arena, "Passed: " ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET ", Failed: " ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, c->num_pass, c->num_fail);
  Str8ListAppend(&c->report, report_suffix);

  String8 report = Str8ListJoin(c->arena, &c->report);
  LOG_INFO("%.*s", report.size, report.str);
}

#endif // CDEFAULT_TEST_IMPLEMENTATION
