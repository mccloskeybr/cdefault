#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

void V2Test(void) {
  V2 x = V2Assign(1, 2);
  V2 y = V2Assign(3, 4);
  V2 z;

  z = V2AddV2(x, y);
  EXPECT_V2_EQ(z, V2Assign(4, 6));

  z = V2AddF32(x, 1);
  EXPECT_V2_EQ(z, V2Assign(2, 3));

  z = V2SubV2(x, y);
  EXPECT_V2_EQ(z, V2Assign(-2, -2));

  z = V2SubF32(x, 1);
  EXPECT_V2_EQ(z, V2Assign(0, 1));

  z = V2MultF32(x, 2);
  EXPECT_V2_EQ(z, V2Assign(2, 4));

  z = V2DivF32(x, 2);
  EXPECT_V2_EQ(z, V2Assign(0.5f, 1));

  z = V2Normalize(V2Assign(100, 0));
  EXPECT_V2_EQ(z, V2Assign(1, 0));

  z = V2Normalize(V2Assign(0, 100));
  EXPECT_V2_EQ(z, V2Assign(0, 1));

  EXPECT_F32_EQ(V2DotV2(x, y), 11);
  EXPECT_F32_EQ(V2InnerMultV2(x, y), 11);
  EXPECT_F32_EQ(V2LengthSq(x), 5);
  EXPECT_F32_EQ(V2Length(x), F32Sqrt(5));
}

void V3Test(void) {
  V3 x = V3Assign(1, 2, 3);
  V3 y = V3Assign(4, 5, 6);
  V3 z;

  z = V3AddV3(x, y);
  EXPECT_V3_EQ(z, V3Assign(5, 7, 9));

  z = V3AddF32(x, 1);
  EXPECT_V3_EQ(z, V3Assign(2, 3, 4));

  z = V3SubV3(x, y);
  EXPECT_V3_EQ(z, V3Assign(-3, -3, -3));

  z = V3SubF32(x, 1);
  EXPECT_V3_EQ(z, V3Assign(0, 1, 2));

  z = V3MultF32(x, 2);
  EXPECT_V3_EQ(z, V3Assign(2, 4, 6));

  z = V3DivF32(x, 2);
  EXPECT_V3_EQ(z, V3Assign(0.5f, 1, 1.5f));

  z = V3Normalize(V3Assign(100, 0, 0));
  EXPECT_V3_EQ(z, V3Assign(1, 0, 0));

  z = V3Normalize(V3Assign(0, 100, 0));
  EXPECT_V3_EQ(z, V3Assign(0, 1, 0));

  z = V3Normalize(V3Assign(0, 0, 100));
  EXPECT_V3_EQ(z, V3Assign(0, 0, 1));

  z = V3CrossV3(x, y);
  EXPECT_V3_EQ(z, V3Assign(-3, 6, -3));

  EXPECT_F32_EQ(V3DotV3(x, y), 32);
  EXPECT_F32_EQ(V3InnerMultV3(x, y), 32);
  EXPECT_F32_EQ(V3LengthSq(x), 14);
  EXPECT_F32_EQ(V3Length(x), F32Sqrt(14));
  EXPECT_F32_EQ(V3MinValue(x), 1);
  EXPECT_F32_EQ(V3MaxValue(x), 3);

  z = V3Assign(2, 4, 6);
  EXPECT_TRUE(V3IsBetween(x, V3_ZEROES, z));
  z = V3Assign(2, 4, 7);
  EXPECT_FALSE(V3IsBetween(x, V3_ZEROES, z));

  x = V3Assign(0, 0, 1);
  y = V3Assign(0, 0, 1);
  z = V3Project(x, y);
  EXPECT_V3_EQ(z, V3Assign(0, 0, 1));

  x = V3Assign(0, 1, 0);
  z = V3Project(x, y);
  EXPECT_V3_EQ(z, V3Assign(0, 0, 0));

  x = V3Assign(0, 0.5, 0.5);
  z = V3Project(x, y);
  EXPECT_V3_EQ(z, V3Assign(0, 0, 0.5f));

  x = V3Assign(1, -1, 0.5);
  z = V3Clamp(x, -0.5, 0.5);
  EXPECT_V3_EQ(z, V3Assign(0.5f, -0.5f, 0.5f));

  x = V3Assign(1, 0, 0);
  y = V3Assign(0, 1, 0);
  EXPECT_F32_EQ(V3AngleBetween(x, y), F32_PI / 2.0f);

  x = V3Assign(0, 0, 0);
  y = V3Assign(1, 0, 0);
  z = V3Lerp(x, y, 0.5);
  EXPECT_V3_EQ(z, V3Assign(0.5f, 0, 0));

  z = V3Lerp(x, y, 0.75);
  EXPECT_V3_EQ(z, V3Assign(0.75f, 0, 0));

  x = V3Assign(0, 0, 1);
  z = V3RotateAroundAxis(x, V3_X_POS, F32_PI / 2.0f);
  EXPECT_V3_APPROX_EQ(z, V3Assign(0, -1, 0));
}

void V4Test(void) {
  V4 x = V4Assign(1, 2, 3, 4);
  V4 y = V4Assign(5, 6, 7, 8);
  V4 z;

  z = V4AddV4(x, y);
  EXPECT_V4_EQ(z, V4Assign(6, 8, 10, 12));

  z = V4AddF32(x, 1);
  EXPECT_V4_EQ(z, V4Assign(2, 3, 4, 5));

  z = V4SubV4(x, y);
  EXPECT_V4_EQ(z, V4Assign(-4, -4, -4, -4));

  z = V4SubF32(x, 1);
  EXPECT_V4_EQ(z, V4Assign(0, 1, 2, 3));

  z = V4MultF32(x, 2);
  EXPECT_V4_EQ(z, V4Assign(2, 4, 6, 8));

  z = V4DivF32(x, 2);
  EXPECT_V4_EQ(z, V4Assign(0.5f, 1, 1.5f, 2));

  z = V4Normalize(V4Assign(100, 0, 0, 0));
  EXPECT_V4_EQ(z, V4Assign(1, 0, 0, 0));

  z = V4Normalize(V4Assign(0, 100, 0, 0));
  EXPECT_V4_EQ(z, V4Assign(0, 1, 0, 0));

  z = V4Normalize(V4Assign(0, 0, 100, 0));
  EXPECT_V4_EQ(z, V4Assign(0, 0, 1, 0));

  z = V4Normalize(V4Assign(0, 0, 0, 100));
  EXPECT_V4_EQ(z, V4Assign(0, 0, 0, 1));

  EXPECT_F32_EQ(V4DotV4(x, y), 70);
  EXPECT_F32_EQ(V4InnerMultV4(x, y), 70);
  EXPECT_F32_EQ(V4LengthSq(x), 30);
  EXPECT_F32_EQ(V4Length(x), F32Sqrt(30));

  x = V4Assign(0, 0, 0, 0);
  y = V4Assign(1, 0, 0, 0);
  z = V4Lerp(x, y, 0.5);
  EXPECT_V4_EQ(z, V4Assign(0.5f, 0, 0, 0));

  z = V4Lerp(x, y, 0.75);
  EXPECT_V4_EQ(z, V4Assign(0.75f, 0, 0, 0));
}

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  RUN_TEST(V2Test);
  RUN_TEST(V3Test);
  RUN_TEST(V4Test);
  LogTestReport();
  return 0;
}
