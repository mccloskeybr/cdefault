#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

B32 SortS8AscTest(void) {
  S8 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_ASC(S8, arr, 10);
  S8 expected_arr[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  TEST_EXPECT(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
  return true;
}

B32 SortS64AscTest(void) {
  S64 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_ASC(S64, arr, 10);
  S64 expected_arr[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  TEST_EXPECT(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
  return true;
}

B32 SortF32AscTest(void) {
  F32 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_ASC(F32, arr, 10);
  F32 expected_arr[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  TEST_EXPECT(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
  return true;
}

B32 SortString8AscTest(void) {
  String8 arr[5];
  arr[0] = String8CreateStatic("banana");
  arr[1] = String8CreateStatic("apricot");
  arr[2] = String8CreateStatic("apple");
  arr[3] = String8CreateStatic("cherry");
  arr[4] = String8CreateStatic("apple");
  SORT_ASC(String8, arr, 5);
  String8 expected_arr[5];
  expected_arr[0] = String8CreateStatic("apple");
  expected_arr[1] = String8CreateStatic("apple");
  expected_arr[2] = String8CreateStatic("apricot");
  expected_arr[3] = String8CreateStatic("banana");
  expected_arr[4] = String8CreateStatic("cherry");
  for (U32 i = 0; i < 5; i++) {
    TEST_EXPECT(String8Equals(arr[i], expected_arr[i]));
  }
  return true;
}

B32 SortS8DescTest(void) {
  S8 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_DESC(S8, arr, 10);
  S8 expected_arr[10] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
  TEST_EXPECT(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
  return true;
}

B32 SortS64DescTest(void) {
  S64 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_DESC(S64, arr, 10);
  S64 expected_arr[10] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
  TEST_EXPECT(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
  return true;
}

B32 SortF32DescTest(void) {
  F32 arr[10] = { 3, 6, 2, 0, 8, 9, 1, 7, 5, 4 };
  SORT_DESC(F32, arr, 10);
  F32 expected_arr[10] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
  TEST_EXPECT(IS_MEMORY_EQUAL_STATIC_ARRAY(arr, expected_arr));
  return true;
}

B32 SortString8DescTest(void) {
  String8 arr[5];
  arr[0] = String8CreateStatic("banana");
  arr[1] = String8CreateStatic("apricot");
  arr[2] = String8CreateStatic("apple");
  arr[3] = String8CreateStatic("cherry");
  arr[4] = String8CreateStatic("apple");
  SORT_DESC(String8, arr, 5);
  String8 expected_arr[5];
  expected_arr[0] = String8CreateStatic("cherry");
  expected_arr[1] = String8CreateStatic("banana");
  expected_arr[2] = String8CreateStatic("apricot");
  expected_arr[3] = String8CreateStatic("apple");
  expected_arr[4] = String8CreateStatic("apple");
  for (U32 i = 0; i < 5; i++) {
    TEST_EXPECT(String8Equals(arr[i], expected_arr[i]));
  }
  return true;
}

int main(void) {
  TEST(SortS8AscTest());
  TEST(SortS64AscTest());
  TEST(SortF32AscTest());
  TEST(SortString8AscTest());
  TEST(SortS8DescTest());
  TEST(SortS64DescTest());
  TEST(SortF32DescTest());
  TEST(SortString8DescTest());
  return 0;
}
