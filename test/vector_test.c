#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "../cdefault_geometry.h"
#define CDEFAULT_TEST_IMPLEMENTATION
#include "../cdefault_test.h"

void V2Test(void) {
  V2 x = V2Lit(1, 2);
  V2 y = V2Lit(3, 4);
  V2 z;

  V2AddV2(&z, &x, &y);
  EXPECT_V2_EQ(z, V2Lit(4, 6));

  V2AddF32(&z, &x, 1);
  EXPECT_V2_EQ(z, V2Lit(2, 3));

  V2SubV2(&z, &x, &y);
  EXPECT_V2_EQ(z, V2Lit(-2, -2));

  V2SubF32(&z, &x, 1);
  EXPECT_V2_EQ(z, V2Lit(0, 1));

  V2MultF32(&z, &x, 2);
  EXPECT_V2_EQ(z, V2Lit(2, 4));

  V2DivF32(&z, &x, 2);
  EXPECT_V2_EQ(z, V2Lit(0.5f, 1));

  z = V2Lit(100, 0);
  V2Normalize(&z, &z);
  EXPECT_V2_EQ(z, V2Lit(1, 0));

  z = V2Lit(0, 100);
  V2Normalize(&z, &z);
  EXPECT_V2_EQ(z, V2Lit(0, 1));

  EXPECT_F32_EQ(V2DotV2(&x, &y), 11);
  EXPECT_F32_EQ(V2InnerMultV2(&x, &y), 11);
  EXPECT_F32_EQ(V2LengthSq(&x), 5);
  EXPECT_F32_EQ(V2Length(&x), F32Sqrt(5));
}

void V3Test(void) {
  V3 x = V3Lit(1, 2, 3);
  V3 y = V3Lit(4, 5, 6);
  V3 z;

  V3AddV3(&z, &x, &y);
  EXPECT_V3_EQ(z, V3Lit(5, 7, 9));

  V3AddF32(&z, &x, 1);
  EXPECT_V3_EQ(z, V3Lit(2, 3, 4));

  V3SubV3(&z, &x, &y);
  EXPECT_V3_EQ(z, V3Lit(-3, -3, -3));

  V3SubF32(&z, &x, 1);
  EXPECT_V3_EQ(z, V3Lit(0, 1, 2));

  V3MultF32(&z, &x, 2);
  EXPECT_V3_EQ(z, V3Lit(2, 4, 6));

  V3DivF32(&z, &x, 2);
  EXPECT_V3_EQ(z, V3Lit(0.5f, 1, 1.5f));

  z = V3Lit(100, 0, 0);
  V3Normalize(&z, &z);
  EXPECT_V3_EQ(z, V3Lit(1, 0, 0));

  z = V3Lit(0, 100, 0);
  V3Normalize(&z, &z);
  EXPECT_V3_EQ(z, V3Lit(0, 1, 0));

  z = V3Lit(0, 0, 100);
  V3Normalize(&z, &z);
  EXPECT_V3_EQ(z, V3Lit(0, 0, 1));

  V3CrossV3(&z, &x, &y);
  EXPECT_V3_EQ(z, V3Lit(-3, 6, -3));

  EXPECT_F32_EQ(V3DotV3(&x, &y), 32);
  EXPECT_F32_EQ(V3InnerMultV3(&x, &y), 32);
  EXPECT_F32_EQ(V3LengthSq(&x), 14);
  EXPECT_F32_EQ(V3Length(&x), F32Sqrt(14));
  EXPECT_F32_EQ(V3MinValue(&x), 1);
  EXPECT_F32_EQ(V3MaxValue(&x), 3);

  V3 zeros = { 0, 0, 0 };
  z = V3Lit(2, 4, 6);
  EXPECT_TRUE(V3IsBetween(&x, &zeros, &z));
  z = V3Lit(2, 4, 7);
  EXPECT_FALSE(V3IsBetween(&x, &zeros, &z));

  x = V3Lit(0, 0, 1);
  y = V3Lit(0, 0, 1);
  V3Project(&z, &x, &y);
  EXPECT_V3_EQ(z, V3Lit(0, 0, 1));

  x = V3Lit(0, 1, 0);
  V3Project(&z, &x, &y);
  EXPECT_V3_EQ(z, V3Lit(0, 0, 0));

  x = V3Lit(0, 0.5, 0.5);
  V3Project(&z, &x, &y);
  EXPECT_V3_EQ(z, V3Lit(0, 0, 0.5f));

  x = V3Lit(1, -1, 0.5);
  V3Clamp(&z, &x, -0.5, 0.5);
  EXPECT_V3_EQ(z, V3Lit(0.5f, -0.5f, 0.5f));

  x = V3Lit(1, 0, 0);
  y = V3Lit(0, 1, 0);
  EXPECT_F32_EQ(V3AngleBetween(&x, &y), F32_PI / 2.0f);

  x = V3Lit(0, 0, 0);
  y = V3Lit(1, 0, 0);
  V3Lerp(&z, &x, &y, 0.5);
  EXPECT_V3_EQ(z, V3Lit(0.5f, 0, 0));

  V3Lerp(&z, &x, &y, 0.75);
  EXPECT_V3_EQ(z, V3Lit(0.75f, 0, 0));

  x = V3Lit(0, 0, 1);
  V3RotateAroundAxis(&z, &x, &V3_X_POS, F32_PI / 2.0f);
  EXPECT_V3_APPROX_EQ(z, V3Lit(0, -1, 0));
}

void V4Test(void) {
  V4 x = V4Lit(1, 2, 3, 4);
  V4 y = V4Lit(5, 6, 7, 8);
  V4 z;

  V4AddV4(&z, &x, &y);
  EXPECT_V4_EQ(z, V4Lit(6, 8, 10, 12));

  V4AddF32(&z, &x, 1);
  EXPECT_V4_EQ(z, V4Lit(2, 3, 4, 5));

  V4SubV4(&z, &x, &y);
  EXPECT_V4_EQ(z, V4Lit(-4, -4, -4, -4));

  V4SubF32(&z, &x, 1);
  EXPECT_V4_EQ(z, V4Lit(0, 1, 2, 3));

  V4MultF32(&z, &x, 2);
  EXPECT_V4_EQ(z, V4Lit(2, 4, 6, 8));

  V4DivF32(&z, &x, 2);
  EXPECT_V4_EQ(z, V4Lit(0.5f, 1, 1.5f, 2));

  z = V4Lit(100, 0, 0, 0);
  V4Normalize(&z, &z);
  EXPECT_V4_EQ(z, V4Lit(1, 0, 0, 0));

  z = V4Lit(0, 100, 0, 0);
  V4Normalize(&z, &z);
  EXPECT_V4_EQ(z, V4Lit(0, 1, 0, 0));

  z = V4Lit(0, 0, 100, 0);
  V4Normalize(&z, &z);
  EXPECT_V4_EQ(z, V4Lit(0, 0, 1, 0));

  z = V4Lit(0, 0, 0, 100);
  V4Normalize(&z, &z);
  EXPECT_V4_EQ(z, V4Lit(0, 0, 0, 1));

  EXPECT_F32_EQ(V4DotV4(&x, &y), 70);
  EXPECT_F32_EQ(V4InnerMultV4(&x, &y), 70);
  EXPECT_F32_EQ(V4LengthSq(&x), 30);
  EXPECT_F32_EQ(V4Length(&x), F32Sqrt(30));

  x = V4Lit(0, 0, 0, 0);
  y = V4Lit(1, 0, 0, 0);
  V4Lerp(&z, &x, &y, 0.5);
  EXPECT_V4_EQ(z, V4Lit(0.5f, 0, 0, 0));

  V4Lerp(&z, &x, &y, 0.75);
  EXPECT_V4_EQ(z, V4Lit(0.75f, 0, 0, 0));
}

int main(void) {
  RUN_TEST(V2Test);
  RUN_TEST(V3Test);
  RUN_TEST(V4Test);
  LogTestReport();
  return 0;
}
