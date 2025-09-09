#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"

B32 M3TrivialOpTest(void) {
  M3 test = {
    10, 10, 10,
    10, 10, 10,
    10, 10, 10,
  };
  M3 other = {
    1, 2, 3,
    4, 5, 6,
    7, 8, 9,
  };

  M3 expected_add = {
    11, 12, 13,
    14, 15, 16,
    17, 18, 19,
  };
  M3 add;
  M3AddM3(&add, &test, &other);
  TEST_EXPECT(IS_MEMORY_EQUAL_STRUCT(&add, &expected_add));

  M3 expected_sub = {
    9, 8, 7,
    6, 5, 4,
    3, 2, 1,
  };
  M3 sub;
  M3SubM3(&sub, &test, &other);
  TEST_EXPECT(IS_MEMORY_EQUAL_STRUCT(&sub, &expected_sub));

  M3 expected_mf32 = {
    20, 20, 20,
    20, 20, 20,
    20, 20, 20,
  };
  M3 mf32;
  M3MultF32(&mf32, 2, &test);
  TEST_EXPECT(IS_MEMORY_EQUAL_STRUCT(&mf32, &expected_mf32));

  return true;
}

B32 M3TransposeTest(void) {
  M3 m = {
    1, 2, 3,
    4, 5, 6,
    7, 8, 9,
  };
  M3 expected_m_t = {
    1, 4, 7,
    2, 5, 8,
    3, 6, 9,
  };
  M3 m_t;
  M3Transpose(&m_t, &m);
  TEST_EXPECT(IS_MEMORY_EQUAL_STRUCT(&m_t, &expected_m_t));

  return true;
}

B32 M3MultTest(void) {
  M3 m = {
    1, 2, 3,
    4, 5, 6,
    7, 8, 9,
  };

  V3 v = { 1, 2, 3 };
  V3 expected_mult_v = { 14, 32, 50 };
  V3 mult_v;
  M3MultV3(&mult_v, &m, &v);
  TEST_EXPECT(IS_MEMORY_EQUAL_STRUCT(&mult_v, &expected_mult_v));

  M3 m2 = {
    3, 2, 1,
    6, 5, 4,
    9, 8, 7,
  };
  M3 expected_mult_m2 = {
     42,  36,  30,
     96,  81,  66,
    150, 126, 102,
  };
  M3 mult_m2;
  M3MultM3(&mult_m2, &m, &m2);
  TEST_EXPECT(IS_MEMORY_EQUAL_STRUCT(&mult_m2, &expected_mult_m2));

  return true;
}

B32 M3DetTest(void) {
  M3 test = {
    2, -3,  1,
    2,  0, -1,
    1,  4,  5,
  };
  TEST_EXPECT(M3Det(&test) == 49);
  return true;
}

B32 M3InvertTest(void) {
  M3 test = {
    1,  2, -1,
    2,  1,  2,
    -1, 2,  1,
  };
  M3 expected_test_inv = {
     0.1875f, 0.25f, -0.3125f,
       0.25f, -0.0f,    0.25f,
    -0.3125f, 0.25f,  0.1875f,
  };
  M3 test_inv;
  M3Invert(&test_inv, &test);
  TEST_EXPECT(IS_MEMORY_EQUAL_STRUCT(&test_inv, &expected_test_inv));

  return true;
}

B32 M4DetTest(void) {
  M4 test = {
    2, 1, -3, 2,
    0, -5, 1, 4,
    1, 0, 2, -6,
    7, 4, -1, 3,
  };
  TEST_EXPECT(M4Det(&test) == -588);

  test = (M4) {
    4,  3,  2, 2,
    0,  1, -3, 3,
    0, -1,  3, 3,
    0,  3,  1, 1,
  };
  TEST_EXPECT(M4Det(&test) == -240);

  test = (M4) {
    4,  3, 2,  2,
    0,  1, 0, -2,
    1, -1, 3,  3,
    2,  3, 1,  1,
  };
  TEST_EXPECT(M4Det(&test) == 30);

  test = (M4) {
    1, -2, 3,  2,
    2,  3, 1, -1,
    3,  3, 3,  3,
    -1, 4, 2,  1,
  };
  TEST_EXPECT(M4Det(&test) == -141);

  return true;
}

B32 M4InvertTest(void) {
  M4 test = {
    5, 6, 6, 8,
    2, 2, 2, 8,
    6, 6, 2, 8,
    2, 3, 6, 7,
  };
  M4 expected_test_inv = {
    -17,   -9,     12,  16,
    17,  8.75, -11.75, -16,
    -4, -2.25 ,  2.75,   4,
    1,   0.75,  -0.75,  -1,
  };
  M4 test_inv;
  M4Invert(&test_inv, &test);
  TEST_EXPECT(IS_MEMORY_EQUAL_STRUCT(&test_inv, &expected_test_inv));
  M4 ident_check;
  M4MultM4(&ident_check, &test, &test_inv);
  TEST_EXPECT(IS_MEMORY_EQUAL_STRUCT(&ident_check, &M4_IDENTITY));

  return true;
}

int main(void) {
  TEST(M3TrivialOpTest());
  TEST(M3TransposeTest());
  TEST(M3MultTest());
  TEST(M3DetTest());
  TEST(M3InvertTest());
  TEST(M4DetTest());
  TEST(M4InvertTest());
  return 0;
}
