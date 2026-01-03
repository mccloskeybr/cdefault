#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

void SortS8AscTest(void) {
  S8 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_ASC(S8, arr, 10);
  S8 expected_arr[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  EXPECT_TRUE(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
}

void SortS64AscTest(void) {
  S64 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_ASC(S64, arr, 10);
  S64 expected_arr[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  EXPECT_TRUE(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
}

void SortF32AscTest(void) {
  F32 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_ASC(F32, arr, 10);
  F32 expected_arr[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  EXPECT_TRUE(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
}

void SortStr8AscTest(void) {
  String8 arr[5];
  arr[0] = Str8Lit("banana");
  arr[1] = Str8Lit("apricot");
  arr[2] = Str8Lit("apple");
  arr[3] = Str8Lit("cherry");
  arr[4] = Str8Lit("apple");
  SORT_ASC(String8, arr, 5);
  String8 expected_arr[5];
  expected_arr[0] = Str8Lit("apple");
  expected_arr[1] = Str8Lit("apple");
  expected_arr[2] = Str8Lit("apricot");
  expected_arr[3] = Str8Lit("banana");
  expected_arr[4] = Str8Lit("cherry");
  for (U32 i = 0; i < 5; i++) {
    EXPECT_TRUE(Str8Eq(arr[i], expected_arr[i]));
  }
}

void SortS8DescTest(void) {
  S8 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_DESC(S8, arr, 10);
  S8 expected_arr[10] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
  EXPECT_TRUE(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
}

void SortS64DescTest(void) {
  S64 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_DESC(S64, arr, 10);
  S64 expected_arr[10] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
  EXPECT_TRUE(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
}

void SortF32DescTest(void) {
  F32 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_DESC(F32, arr, 10);
  F32 expected_arr[10] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
  EXPECT_TRUE(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
}

void SortStr8DescTest(void) {
  String8 arr[5];
  arr[0] = Str8Lit("banana");
  arr[1] = Str8Lit("apricot");
  arr[2] = Str8Lit("apple");
  arr[3] = Str8Lit("cherry");
  arr[4] = Str8Lit("apple");
  SORT_DESC(String8, arr, 5);
  String8 expected_arr[5];
  expected_arr[0] = Str8Lit("cherry");
  expected_arr[1] = Str8Lit("banana");
  expected_arr[2] = Str8Lit("apricot");
  expected_arr[3] = Str8Lit("apple");
  expected_arr[4] = Str8Lit("apple");
  for (U32 i = 0; i < 5; i++) {
    EXPECT_TRUE(Str8Eq(arr[i], expected_arr[i]));
  }
}

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  RUN_TEST(SortS8AscTest);
  RUN_TEST(SortS64AscTest);
  RUN_TEST(SortF32AscTest);
  RUN_TEST(SortStr8AscTest);
  RUN_TEST(SortS8DescTest);
  RUN_TEST(SortS64DescTest);
  RUN_TEST(SortF32DescTest);
  RUN_TEST(SortStr8DescTest);
  LogTestReport();
  return 0;
}
