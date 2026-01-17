#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

void CharTest(void) {
  EXPECT_TRUE(CharIsUpper('A'));
  EXPECT_TRUE(CharIsUpper('Z'));
  EXPECT_FALSE(CharIsUpper('a'));
  EXPECT_FALSE(CharIsUpper('z'));
  EXPECT_FALSE(CharIsUpper('7'));

  EXPECT_TRUE(CharIsLower('a'));
  EXPECT_TRUE(CharIsLower('z'));
  EXPECT_FALSE(CharIsLower('A'));
  EXPECT_FALSE(CharIsLower('Z'));
  EXPECT_FALSE(CharIsLower('7'));

  EXPECT_TRUE(CharIsAlpha('a'));
  EXPECT_TRUE(CharIsAlpha('A'));
  EXPECT_FALSE(CharIsAlpha('7'));

  EXPECT_TRUE(CharIsDigitEx('7', 10));
  EXPECT_FALSE(CharIsDigitEx('a', 10));
  EXPECT_TRUE(CharIsDigitEx('a', 11));
  EXPECT_FALSE(CharIsDigitEx('A', 10));
  EXPECT_TRUE(CharIsDigitEx('A', 11));

  EXPECT_U8_EQ(CharToLower('A'), 'a');
  EXPECT_U8_EQ(CharToLower('a'), 'a');
  EXPECT_U8_EQ(CharToLower('7'), '7');
  EXPECT_U8_EQ(CharToUpper('A'), 'A');
  EXPECT_U8_EQ(CharToUpper('a'), 'A');
  EXPECT_U8_EQ(CharToUpper('7'), '7');
}

void CStrCopyTest(void) {
  Arena* arena = ArenaAllocate();
  U8 base[6] = "apple";
  U8* copy = CStrCopy(arena, base);
  EXPECT_CSTR_EQ(base, copy);
  EXPECT_U32_EQ(CStrSize(copy), 5);
}

void CStrSubstringTest(void) {
  Arena* arena = ArenaAllocate();
  U8 base[19] = "prefix test suffix";
  U8* substring = CStrSubstring(arena, base, 7, 11);
  EXPECT_CSTR_EQ(substring, "test");
}

void CStrConcatTest(void) {
  Arena* arena = ArenaAllocate();
  U8* actual = CStrConcat(arena, (U8*) "hello ", (U8*) "world");
  EXPECT_CSTR_EQ(actual, "hello world");
}

void CStrTrimTest(void) {
  Arena* arena = ArenaAllocate();
  U8* test = (U8*) "  test  ";
  EXPECT_CSTR_EQ(CStrTrimFront(arena, test), "test  ");
  EXPECT_CSTR_EQ(CStrTrimBack(arena, test), "  test");
  EXPECT_CSTR_EQ(CStrTrim(arena, test), "test");

  test = (U8*) "     ";
  EXPECT_CSTR_EQ(CStrTrimFront(arena, test), "");
  EXPECT_CSTR_EQ(CStrTrimBack(arena, test), "");
  EXPECT_CSTR_EQ(CStrTrim(arena, test), "");

  test = (U8*) "  c  ";
  EXPECT_CSTR_EQ(CStrTrimFront(arena, test), "c  ");
  EXPECT_CSTR_EQ(CStrTrimBack(arena, test), "  c");
  EXPECT_CSTR_EQ(CStrTrim(arena, test), "c");
}

void CStrReplaceAllTest(void) {
  Arena* arena = ArenaAllocate();
  U8* dest = CStrReplaceAll(arena, (U8*) "hello hi hello", (U8*) "hi", (U8*) "world");
  EXPECT_CSTR_EQ(dest, "hello world hello");
}

void CStrFindTest(void) {
  EXPECT_S32_EQ(CStrFind((U8*) "hello world world hello", 0, (U8*) "world"), 6);
}

void CStrFindReverseTest(void) {
  U8 haystack[24] = "hello world world hello";
  EXPECT_S32_EQ(CStrFindReverse(haystack, CStrSize(haystack) - 1, (U8*) "world"), 12);
}

void CStrStartsWithTest(void) {
  U8 haystack[24] = "hello world world hello";
  EXPECT_TRUE(CStrStartsWith(haystack, (U8*) "hello"));
  EXPECT_FALSE(CStrStartsWith(haystack, (U8*) "world"));
}

void CStrEndsWithTest(void) {
  U8 haystack[24] = "hello world world hello";
  EXPECT_TRUE(CStrEndsWith(haystack, (U8*) "hello"));
  EXPECT_FALSE(CStrEndsWith(haystack, (U8*) "world"));
}

void CStrToUpperTest(void) {
  U8 a[6] = "hello";
  CStrToUpper(a);
  EXPECT_CSTR_EQ(a, "HELLO");
}

void CStrToLowerTest(void) {
  U8 a[6] = "HELLO";
  CStrToLower(a);
  EXPECT_CSTR_EQ(a, "hello");
}

void Str8CreateTest(void) {
  String8 literal = Str8Lit("hello");
  EXPECT_U32_EQ(literal.size, 5);
  EXPECT_TRUE(MEMORY_IS_EQUAL_SIZE(literal.str, "hello", 5));

  literal = Str8CStr("hello");
  EXPECT_U32_EQ(literal.size, 5);
  EXPECT_TRUE(MEMORY_IS_EQUAL_SIZE(literal.str, "hello", 5));
}

void Str8TrimTest(void) {
  String8 test = Str8Lit("  test  ");
  EXPECT_STR8_EQ(Str8TrimFront(test), Str8Lit("test  "));
  EXPECT_STR8_EQ(Str8TrimBack(test), Str8Lit("  test"));
  EXPECT_STR8_EQ(Str8Trim(test), Str8Lit("test"));

  test = Str8Lit("     ");
  EXPECT_STR8_EQ(Str8TrimFront(test), Str8Lit(""));
  EXPECT_STR8_EQ(Str8TrimBack(test), Str8Lit(""));
  EXPECT_STR8_EQ(Str8Trim(test), Str8Lit(""));

  test = Str8Lit("  c  ");
  EXPECT_STR8_EQ(Str8TrimFront(test), Str8Lit("c  "));
  EXPECT_STR8_EQ(Str8TrimBack(test), Str8Lit("  c"));
  EXPECT_STR8_EQ(Str8Trim(test), Str8Lit("c"));
}

void Str8ReplaceAllTest(void) {
  Arena* arena = ArenaAllocate();
  String8 actual = Str8ReplaceAll(arena, Str8Lit("hello hi hello"), Str8Lit("hi"), Str8Lit("world"));
  EXPECT_STR8_EQ(actual, Str8Lit("hello world hello"));
}

void Str8ReplaceAllCharTest(void) {
  String8 str = Str8Lit("abba");
  EXPECT_TRUE(Str8ReplaceAllChar(&str, 'b', 'c'));
  EXPECT_STR8_EQ(str, Str8Lit("acca"));
}

void Str8ToF32Test(void) {
  F32 result;

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("1.2345"), &result), 6);
  EXPECT_F32_APPROX_EQ(result, 1.2345f);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("+1"), &result), 2);
  EXPECT_F32_APPROX_EQ(result, 1.0f);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("-502"), &result), 4);
  EXPECT_F32_APPROX_EQ(result, -502.0f);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("1x0"), &result), 1);
  EXPECT_F32_APPROX_EQ(result, 1);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("abc1.2"), &result), -1);
  EXPECT_F32_APPROX_EQ(result, 0);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("1e2"), &result), 3);
  EXPECT_F32_APPROX_EQ(result, 100);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("1.23e2"), &result), 6);
  EXPECT_F32_APPROX_EQ(result, 123);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("123.123456e2"), &result), 12);
  EXPECT_F32_APPROX_EQ(result, 12312.3456f);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("1e-2"), &result), 4);
  EXPECT_F32_APPROX_EQ(result, 0.01f);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("1.23e-2"), &result), 7);
  EXPECT_F32_APPROX_EQ(result, 0.0123f);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("-1.23e+2"), &result), 8);
  EXPECT_F32_APPROX_EQ(result, -123);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("1.23e0000002"), &result), 12);
  EXPECT_F32_APPROX_EQ(result, 123);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("1.23E2"), &result), 6);
  EXPECT_F32_APPROX_EQ(result, 123);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit("E2"), &result), -1);

  EXPECT_S32_EQ(Str8ToF32(Str8Lit(".1E2"), &result), 4);
  EXPECT_F32_APPROX_EQ(result, 10);
}

void Str8ToS32Test(void) {
  S32 result;

  EXPECT_S32_EQ(Str8ToS32(Str8Lit("123"), &result), 3);
  EXPECT_S32_EQ(result, 123);

  EXPECT_S32_EQ(Str8ToS32(Str8Lit("-456"), &result), 4);
  EXPECT_S32_EQ(result, -456);

  EXPECT_S32_EQ(Str8ToS32(Str8Lit("1x2"), &result), 1);
  EXPECT_S32_EQ(result, 1);

  EXPECT_S32_EQ(Str8ToS32(Str8Lit("x2"), &result), -1);
  EXPECT_S32_EQ(result, 0);
}

void Str8StartsWithTest(void) {
  String8 literal = Str8Lit("hello world");
  EXPECT_TRUE(Str8StartsWith(literal, Str8Lit("hello")));
  EXPECT_FALSE(Str8StartsWith(literal, Str8Lit("world")));
}

void Str8EndsWithTest(void) {
  String8 literal = Str8Lit("hello world");
  EXPECT_FALSE(Str8EndsWith(literal, Str8Lit("hello")));
  EXPECT_TRUE(Str8EndsWith(literal, Str8Lit("world")));
}

void Str8EqTest(void) {
  String8 literal = Str8Lit("hello world");
  EXPECT_STR8_EQ(literal, Str8Lit("hello world"));
  EXPECT_FALSE(Str8Eq(literal, Str8Lit("world"))); // TODO: NEQ test expects?
}

void Str8FindTest(void) {
  String8 literal = Str8Lit("hello world");
  String8 needle = Str8Lit("world");
  String8 needle_2 = Str8Lit("o");
  String8 not_present = Str8Lit("worlda");
  EXPECT_S32_EQ(Str8Find(literal, 0, needle), 6);
  EXPECT_S32_EQ(Str8Find(literal, 0, needle_2), 4);
  EXPECT_S32_EQ(Str8Find(literal, 0, not_present), -1);
  EXPECT_S32_EQ(Str8Find(literal, 7, needle), -1);
  EXPECT_S32_EQ(Str8Find(literal, 100, needle), -1);
}

void Str8FindReverseTest(void) {
  String8 literal = Str8Lit("hello world");
  String8 needle = Str8Lit("world");
  String8 needle_2 = Str8Lit("o");
  String8 not_present = Str8Lit("worlda");
  EXPECT_S32_EQ(Str8FindReverse(literal, 0, needle), 6);
  EXPECT_S32_EQ(Str8FindReverse(literal, 0, needle_2), 7);
  EXPECT_S32_EQ(Str8FindReverse(literal, 0, not_present), -1);
  EXPECT_S32_EQ(Str8FindReverse(literal, 7, needle), -1);
  EXPECT_S32_EQ(Str8FindReverse(literal, 100, needle), -1);
}

void Str8ConcatTest(void) {
  Arena* arena = ArenaAllocate();
  EXPECT_STR8_EQ(Str8Concat(arena, Str8Lit("hello "), Str8Lit("world")), Str8Lit("hello world"));
}

void Str8FormatTest(void) {
  Arena* arena = ArenaAllocate();
  String8 str, expected;

  str = Str8Format(arena, "hello%c%s", ' ', "world");
  expected = Str8Lit("hello world");
  EXPECT_STR8_EQ(str, expected);

  String8 world = Str8Lit("world");
  str = Str8Format(arena, "hello %.*s %.3s", world.size, world.str, "world");
  expected = Str8Lit("hello world wor");
  EXPECT_STR8_EQ(str, expected);

  str = Str8Format(arena, "hello %S", Str8Lit("world"));
  expected = Str8Lit("hello world");
  EXPECT_STR8_EQ(str, expected);

  str = Str8Format(arena, "%d %i %d %i", -5, -1, 0, 12345);
  expected = Str8Lit("-5 -1 0 12345");
  EXPECT_STR8_EQ(str, expected);

  str = Str8Format(arena, "%x %X %#x %#X", 255, 255, 255, 255);
  expected = Str8Lit("ff FF 0xff 0XFF");
  EXPECT_STR8_EQ(str, expected);

  str = Str8Format(arena, "%.2f %+.2f", 0.567f, 0.567f);
  expected = Str8Lit("0.56 +0.56");
  EXPECT_STR8_EQ(str, expected);

  str = Str8Format(arena, "%07.2f %-7.2f", 0.567f, 0.567f);
  expected = Str8Lit("0000.56 0.56   ");
  EXPECT_STR8_EQ(str, expected);

  str = Str8Format(arena, "%.2f %#.2f", 0.0f, 0.0f);
  expected = Str8Lit("0 0.00");
  EXPECT_STR8_EQ(str, expected);

  V4 test_v4 = V4Assign(1.11f, 2.22f, 3.33f, 4.44f);
  str = Str8Format(arena, "%.1V4", test_v4);
  expected = Str8Lit("{ 1.1, 2.2, 3.3, 4.4 }");
  EXPECT_STR8_EQ(str, expected);
}

void Str8ListBuildTest(void) {
  Arena* arena = ArenaAllocate();
  String8List list;
  MEMORY_ZERO_STRUCT(&list);
  Str8ListAppend(arena, &list, Str8Lit(" "));
  Str8ListPrepend(arena, &list, Str8Lit("hello"));
  Str8ListAppend(arena, &list, Str8Lit("world"));
  EXPECT_TRUE(Str8Eq(Str8ListJoin(arena, &list), Str8Lit("hello world")));
}

void Str8SplitTest(void) {
  String8 original = Str8Lit("hi hello world ");
  String8 expected = {0};
  String8ListNode* test = NULL;
  Arena* arena = ArenaAllocate();
  String8List list = Str8Split(arena, original, ' ');

  test = list.head;
  expected = Str8Lit("hi");
  EXPECT_STR8_EQ(test->string, expected);

  test = test->next;
  expected = Str8Lit("hello");
  EXPECT_STR8_EQ(test->string, expected);

  test = test->next;
  expected = Str8Lit("world");
  EXPECT_STR8_EQ(test->string, expected);
}

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  RUN_TEST(CharTest);
  RUN_TEST(CStrCopyTest);
  RUN_TEST(CStrSubstringTest);
  RUN_TEST(CStrConcatTest);
  RUN_TEST(CStrTrimTest);
  RUN_TEST(CStrReplaceAllTest);
  RUN_TEST(CStrFindTest);
  RUN_TEST(CStrFindReverseTest);
  RUN_TEST(CStrStartsWithTest);
  RUN_TEST(CStrEndsWithTest);
  RUN_TEST(CStrToUpperTest);
  RUN_TEST(CStrToLowerTest);
  RUN_TEST(Str8CreateTest);
  RUN_TEST(Str8TrimTest);
  RUN_TEST(Str8ReplaceAllTest);
  RUN_TEST(Str8ReplaceAllCharTest);
  RUN_TEST(Str8ToF32Test);
  RUN_TEST(Str8ToS32Test);
  RUN_TEST(Str8StartsWithTest);
  RUN_TEST(Str8EndsWithTest);
  RUN_TEST(Str8EqTest);
  RUN_TEST(Str8FindTest);
  RUN_TEST(Str8FindReverseTest);
  RUN_TEST(Str8ConcatTest);
  RUN_TEST(Str8FormatTest);
  RUN_TEST(Str8ListBuildTest);
  RUN_TEST(Str8SplitTest);
  LogTestReport();
  return 0;
}
