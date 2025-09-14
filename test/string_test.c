#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

B32 CharTest(void) {
  TEST_EXPECT(CharIsUpper('A'));
  TEST_EXPECT(CharIsUpper('Z'));
  TEST_EXPECT(!CharIsUpper('a'));
  TEST_EXPECT(!CharIsUpper('z'));
  TEST_EXPECT(!CharIsUpper('7'));

  TEST_EXPECT(CharIsLower('a'));
  TEST_EXPECT(CharIsLower('z'));
  TEST_EXPECT(!CharIsLower('A'));
  TEST_EXPECT(!CharIsLower('Z'));
  TEST_EXPECT(!CharIsLower('7'));

  TEST_EXPECT(CharIsAlpha('a'));
  TEST_EXPECT(CharIsAlpha('A'));
  TEST_EXPECT(!CharIsAlpha('7'));

  TEST_EXPECT(CharIsDigit('7', 10));
  TEST_EXPECT(!CharIsDigit('a', 10));
  TEST_EXPECT(CharIsDigit('a', 11));
  TEST_EXPECT(!CharIsDigit('A', 10));
  TEST_EXPECT(CharIsDigit('A', 11));

  TEST_EXPECT(CharToLower('A') == 'a');
  TEST_EXPECT(CharToLower('a') == 'a');
  TEST_EXPECT(CharToLower('7') == '7');
  TEST_EXPECT(CharToUpper('A') == 'A');
  TEST_EXPECT(CharToUpper('a') == 'A');
  TEST_EXPECT(CharToUpper('7') == '7');

  return true;
}

B32 CStringCopyTest(void) {
  Arena* arena = ArenaAllocate();
  U8 base[6] = "apple";
  U8* copy = CStringCopy(arena, base);
  TEST_EXPECT(CStringEquals(base, copy));
  TEST_EXPECT(CStringSize(copy) == 5);
  return true;
}

B32 CStringSubstringTest(void) {
  Arena* arena = ArenaAllocate();
  U8 base[19] = "prefix test suffix";
  U8 expected[5] = "test";
  U8* substring = CStringSubstring(arena, base, 7, 10);
  TEST_EXPECT(CStringEquals(substring, expected));
  return true;
}

B32 CStringConcatTest(void) {
  Arena* arena = ArenaAllocate();
  U8 a[7] = "hello ";
  U8 b[6] = "world";
  U8 expected[12] = "hello world";
  U8* concat = CStringConcat(arena, a, b);
  TEST_EXPECT(CStringEquals(concat, expected));
  return true;
}

B32 CStringTrimTest(void) {
  Arena* arena = ArenaAllocate();
  U8 a[9] = "  test  ";
  U8 expected[5] = "test";
  U8* trimmed = CStringTrim(arena, a);
  TEST_EXPECT(CStringEquals(trimmed, expected));
  return true;
}

B32 CStringFindTest(void) {
  U8 haystack[24] = "hello world world hello";
  U8 needle[6] = "world";
  S32 pos = CStringFind(haystack, 0, needle);
  TEST_EXPECT(pos == 6);
  return true;
}

B32 CStringFindReverseTest(void) {
  U8 haystack[24] = "hello world world hello";
  U8 needle[6] = "world";
  S32 pos = CStringFindReverse(haystack, CStringSize(haystack) - 1, needle);
  TEST_EXPECT(pos == 12);
  return true;
}

B32 CStringStartsWithTest(void) {
  U8 haystack[24] = "hello world world hello";
  TEST_EXPECT(CStringStartsWith(haystack, (U8*) "hello"));
  TEST_EXPECT(!CStringStartsWith(haystack, (U8*) "world"));
  return true;
}

B32 CStringEndsWithTest(void) {
  U8 haystack[24] = "hello world world hello";
  TEST_EXPECT(CStringEndsWith(haystack, (U8*) "hello"));
  TEST_EXPECT(!CStringEndsWith(haystack, (U8*) "world"));
  return true;
}

B32 CStringToUpperTest(void) {
  U8 a[6] = "hello";
  U8 expected[6] = "HELLO";
  CStringToUpper(a);
  TEST_EXPECT(CStringEquals(a, expected));
  return true;
}

B32 CStringToLowerTest(void) {
  U8 a[6] = "HELLO";
  U8 expected[6] = "hello";
  CStringToLower(a);
  TEST_EXPECT(CStringEquals(a, expected));
  return true;
}

B32 String8CreateTest(void) {
  String8 literal = String8CreateStatic("hello");
  TEST_EXPECT(literal.size == 5);
  TEST_EXPECT(IS_MEMORY_EQUAL(literal.str, "hello", 5));
  literal = String8CreateCString("hello");
  TEST_EXPECT(literal.size == 5);
  TEST_EXPECT(IS_MEMORY_EQUAL(literal.str, "hello", 5));
  return true;
}

B32 String8TrimTest(void) {
  String8 literal = String8CreateStatic("  test  ");
  String8 trimmed = String8Trim(&literal);
  String8 expected = String8CreateStatic("test");
  TEST_EXPECT(String8Equals(&trimmed, &expected));
  return true;
}

B32 String8StartsWithTest(void) {
  String8 literal = String8CreateStatic("hello world");
  String8 prefix = String8CreateStatic("hello");
  String8 not_prefix = String8CreateStatic("world");
  TEST_EXPECT(String8StartsWith(&literal, &prefix));
  TEST_EXPECT(!String8StartsWith(&literal, &not_prefix));
  return true;
}

B32 String8EndsWithTest(void) {
  String8 literal = String8CreateStatic("hello world");
  String8 suffix = String8CreateStatic("world");
  String8 not_suffix = String8CreateStatic("hello");
  TEST_EXPECT(String8EndsWith(&literal, &suffix));
  TEST_EXPECT(!String8EndsWith(&literal, &not_suffix));
  return true;
}

B32 String8EqualsTest(void) {
  String8 literal = String8CreateStatic("hello world");
  String8 same = String8CreateStatic("hello world");
  String8 not_same = String8CreateStatic("world");
  TEST_EXPECT(String8Equals(&literal, &same));
  TEST_EXPECT(!String8Equals(&literal, &not_same));
  return true;
}

B32 String8FindTest(void) {
  String8 literal = String8CreateStatic("hello world");
  String8 needle = String8CreateStatic("world");
  String8 needle_2 = String8CreateStatic("o");
  String8 not_present = String8CreateStatic("worlda");
  TEST_EXPECT(String8Find(&literal, 0, &needle) == 6);
  TEST_EXPECT(String8Find(&literal, 0, &needle_2) == 4);
  TEST_EXPECT(String8Find(&literal, 0, &not_present) == -1);
  TEST_EXPECT(String8Find(&literal, 7, &needle) == -1);
  TEST_EXPECT(String8Find(&literal, 100, &needle) == -1);
  return true;
}

B32 String8FindReverseTest(void) {
  String8 literal = String8CreateStatic("hello world");
  String8 needle = String8CreateStatic("world");
  String8 needle_2 = String8CreateStatic("o");
  String8 not_present = String8CreateStatic("worlda");
  TEST_EXPECT(String8FindReverse(&literal, 0, &needle) == 6);
  TEST_EXPECT(String8FindReverse(&literal, 0, &needle_2) == 7);
  TEST_EXPECT(String8FindReverse(&literal, 0, &not_present) == -1);
  TEST_EXPECT(String8FindReverse(&literal, 7, &needle) == -1);
  TEST_EXPECT(String8FindReverse(&literal, 100, &needle) == -1);
  return true;
}

B32 String8ConcatTest(void) {
  Arena* arena = ArenaAllocate();
  String8 a = String8CreateStatic("hello ");
  String8 b = String8CreateStatic("world");
  String8 c = String8Concat(arena, &a, &b);
  String8 expected = String8CreateStatic("hello world");
  TEST_EXPECT(String8Equals(&c, &expected));
  ArenaRelease(arena);
  return true;
}

B32 String8FormatTest(void) {
  Arena* arena = ArenaAllocate();
  String8 str = String8Format(arena, "hello %s %d", "world", 100);
  String8 expected = String8CreateStatic("hello world 100");
  TEST_EXPECT(String8Equals(&str, &expected));
  ArenaRelease(arena);
  return true;
}

B32 String8ListBuildTest(void) {
  String8ListNode* test = NULL;
  String8List list = {0};
  String8ListNode a = {0};
  a.string = String8CreateStatic("hello");
  String8ListNode b = {0};
  b.string = String8CreateStatic(" ");
  String8ListNode c = {0};
  c.string = String8CreateStatic("world");
  String8ListAppend(&list, &b);
  String8ListPrepend(&list, &a);
  String8ListAppend(&list, &c);
  test = list.tail;
  TEST_EXPECT(IS_MEMORY_EQUAL(test, &a, sizeof(String8ListNode)));
  test = test->next;
  TEST_EXPECT(IS_MEMORY_EQUAL(test, &b, sizeof(String8ListNode)));
  test = test->next;
  TEST_EXPECT(IS_MEMORY_EQUAL(test, &c, sizeof(String8ListNode)));
  Arena* arena = ArenaAllocate();
  String8 combined = String8ListJoin(arena, &list);
  String8 expected = String8CreateStatic("hello world");
  TEST_EXPECT(String8Equals(&combined, &expected));
  ArenaRelease(arena);
  return true;
}

B32 String8SplitTest(void) {
  String8 original = String8CreateStatic("hi hello world ");
  String8 expected = {0};
  String8ListNode* test = NULL;
  Arena* arena = ArenaAllocate();
  String8List list = String8Split(arena, &original, ' ');

  test = list.tail;
  expected = String8CreateStatic("hi");
  TEST_EXPECT(String8Equals(&test->string, &expected));

  test = test->next;
  expected = String8CreateStatic("hello");
  TEST_EXPECT(String8Equals(&test->string, &expected));

  test = test->next;
  expected = String8CreateStatic("world");
  TEST_EXPECT(String8Equals(&test->string, &expected));

  ArenaRelease(arena);
  return true;
}

int main(void) {
  TEST(CharTest());
  TEST(CStringCopyTest());
  TEST(CStringSubstringTest());
  TEST(CStringConcatTest());
  TEST(CStringTrimTest());
  TEST(CStringFindTest());
  TEST(CStringFindReverseTest());
  TEST(CStringStartsWithTest());
  TEST(CStringEndsWithTest());
  TEST(CStringToUpperTest());
  TEST(CStringToLowerTest());
  TEST(String8CreateTest());
  TEST(String8TrimTest());
  TEST(String8StartsWithTest());
  TEST(String8EndsWithTest());
  TEST(String8EqualsTest());
  TEST(String8FindTest());
  TEST(String8FindReverseTest());
  TEST(String8ConcatTest());
  TEST(String8FormatTest());
  TEST(String8ListBuildTest());
  TEST(String8SplitTest());
  return 0;
}
