#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "../cdefault_geometry.h"

B32 V2Test(void) {
  V2 x = { 1, 2 };
  V2 y = { 3, 4 };
  V2 z;
  V2AddV2(&x, &y, &z);
  TEST_EXPECT(z.x == 4 && z.y == 6);
  V2AddF32(&x, 1, &z);
  TEST_EXPECT(z.x == 2 && z.y == 3);
  V2SubV2(&x, &y, &z);
  TEST_EXPECT(z.x == -2 && z.y == -2);
  V2SubF32(&x, 1, &z);
  TEST_EXPECT(z.x == 0 && z.y == 1);
  V2MultF32(&x, 2, &z);
  TEST_EXPECT(z.x == 2 && z.y == 4);
  V2DivF32(&x, 2, &z);
  TEST_EXPECT(z.x == 0.5f && z.y == 1);
  z = (V2) {100, 0};
  V2Normalize(&z, &z);
  TEST_EXPECT(z.x == 1 && z.y == 0);
  z = (V2) {0, 100};
  V2Normalize(&z, &z);
  TEST_EXPECT(z.x == 0 && z.y == 1);

  TEST_EXPECT(V2DotV2(&x, &y) == 11);
  TEST_EXPECT(V2InnerMultV2(&x, &y) == 11);
  TEST_EXPECT(V2LengthSq(&x) == 5);
  TEST_EXPECT(V2Length(&x) == F32Sqrt(5));

  return true;
}

B32 V3Test(void) {
  V3 x = { 1, 2, 3 };
  V3 y = { 4, 5, 6 };
  V3 z;
  V3AddV3(&x, &y, &z);
  TEST_EXPECT(z.x == 5 && z.y == 7 && z.z == 9);
  V3AddF32(&x, 1, &z);
  TEST_EXPECT(z.x == 2 && z.y == 3 && z.z == 4);
  V3SubV3(&x, &y, &z);
  TEST_EXPECT(z.x == -3 && z.y == -3 && z.z == -3);
  V3SubF32(&x, 1, &z);
  TEST_EXPECT(z.x == 0 && z.y == 1 && z.z == 2);
  V3MultF32(&x, 2, &z);
  TEST_EXPECT(z.x == 2 && z.y == 4 && z.z == 6);
  V3DivF32(&x, 2, &z);
  TEST_EXPECT(z.x == 0.5f && z.y == 1 && z.z == 1.5f);
  z = (V3) {100, 0, 0};
  V3Normalize(&z, &z);
  TEST_EXPECT(z.x == 1 && z.y == 0 && z.z == 0);
  z = (V3) {0, 100, 0};
  V3Normalize(&z, &z);
  TEST_EXPECT(z.x == 0 && z.y == 1 && z.z == 0);
  z = (V3) {0, 0, 100};
  V3Normalize(&z, &z);
  TEST_EXPECT(z.x == 0 && z.y == 0 && z.z == 1);
  V3CrossV3(&x, &y, &z);
  TEST_EXPECT(z.x == -3 && z.y == 6 && z.z == -3);

  TEST_EXPECT(V3DotV3(&x, &y) == 32);
  TEST_EXPECT(V3InnerMultV3(&x, &y) == 32);
  TEST_EXPECT(V3LengthSq(&x) == 14);
  TEST_EXPECT(V3Length(&x) == F32Sqrt(14));
  TEST_EXPECT(V3MinValue(&x) == 1);
  TEST_EXPECT(V3MaxValue(&x) == 3);

  V3 zeros = V3Splat(0);
  z = (V3) {2, 4, 6};
  TEST_EXPECT(V3IsBetween(&x, &zeros, &z));
  z = (V3) {2, 4, 7};
  TEST_EXPECT(!V3IsBetween(&x, &zeros, &z));

  x = (V3) {0, 0, 1};
  y = (V3) {0, 0, 1};
  V3Project(&x, &y, &z);
  TEST_EXPECT(z.x == 0 && z.y == 0 && z.z == 1);
  x = (V3) {0, 1, 0};
  V3Project(&x, &y, &z);
  TEST_EXPECT(z.x == 0 && z.y == 0 && z.z == 0);
  x = (V3) {0, 0.5, 0.5};
  V3Project(&x, &y, &z);
  TEST_EXPECT(z.x == 0 && z.y == 0 && z.z == 0.5);

  x = (V3) {0, 0, 1};
  V3RotateX(&x, -F32_PI / 2.0f, &z);
  TEST_EXPECT(F32ApproxEq(z.x, 0) && F32ApproxEq(z.y, +1) && F32ApproxEq(z.z, 0));
  V3RotateX(&x, +F32_PI / 2.0f, &z);
  TEST_EXPECT(F32ApproxEq(z.x, 0) && F32ApproxEq(z.y, -1) && F32ApproxEq(z.z, 0));
  V3RotateY(&x, -F32_PI / 2.0f, &z);
  TEST_EXPECT(F32ApproxEq(z.x, -1) && F32ApproxEq(z.y, 0) && F32ApproxEq(z.z, 0));
  V3RotateY(&x, +F32_PI / 2.0f, &z);
  TEST_EXPECT(F32ApproxEq(z.x, +1) && F32ApproxEq(z.y, 0) && F32ApproxEq(z.z, 0));
  x = (V3) {1, 0, 0};
  V3RotateZ(&x, -F32_PI / 2.0f, &z);
  TEST_EXPECT(F32ApproxEq(z.x, 0) && F32ApproxEq(z.y, -1) && F32ApproxEq(z.z, 0));
  V3RotateZ(&x, +F32_PI / 2.0f, &z);
  TEST_EXPECT(F32ApproxEq(z.x, 0) && F32ApproxEq(z.y, +1) && F32ApproxEq(z.z, 0));

  x = (V3) {1, -1, 0.5};
  V3Clamp(&x, -0.5, 0.5, &z);
  TEST_EXPECT(z.x == 0.5 && z.y == -0.5 && z.z == 0.5);

  x = (V3) {1, 0, 0};
  y = (V3) {0, 1, 0};
  TEST_EXPECT(V3AngleBetween(&x, &y) == F32_PI / 2.0f);

  x = (V3) {0, 0, 0};
  y = (V3) {1, 0, 0};
  V3Lerp(&x, &y, 0.5, &z);
  TEST_EXPECT(z.x == 0.5f && z.y == 0 && z.z == 0);
  V3Lerp(&x, &y, 0.75, &z);
  TEST_EXPECT(z.x == 0.75f && z.y == 0 && z.z == 0);

  x = (V3) {0, 0, 1};
  V3RotateAroundAxis(&x, &V3_X_POS, F32_PI / 2.0f, &z);
  TEST_EXPECT(F32ApproxEq(z.x, 0) && F32ApproxEq(z.y, -1) && F32ApproxEq(z.z, 0));

  return true;
}

B32 V4Test(void) {
  V4 x = { 1, 2, 3, 4 };
  V4 y = { 5, 6, 7, 8 };
  V4 z;
  V4AddV4(&x, &y, &z);
  TEST_EXPECT(z.x == 6 && z.y == 8 && z.z == 10 && z.w == 12);
  V4AddF32(&x, 1, &z);
  TEST_EXPECT(z.x == 2 && z.y == 3 && z.z == 4 && z.w == 5);
  V4SubV4(&x, &y, &z);
  TEST_EXPECT(z.x == -4 && z.y == -4 && z.z == -4 && z.w == -4);
  V4SubF32(&x, 1, &z);
  TEST_EXPECT(z.x == 0 && z.y == 1 && z.z == 2 && z.w == 3);
  V4MultF32(&x, 2, &z);
  TEST_EXPECT(z.x == 2 && z.y == 4 && z.z == 6 && z.w == 8);
  V4DivF32(&x, 2, &z);
  TEST_EXPECT(z.x == 0.5f && z.y == 1 && z.z == 1.5f && z.w == 2);
  z = (V4) {100, 0, 0, 0};
  V4Normalize(&z, &z);
  TEST_EXPECT(z.x == 1 && z.y == 0 && z.z == 0 && z.w == 0);
  z = (V4) {0, 100, 0, 0};
  V4Normalize(&z, &z);
  TEST_EXPECT(z.x == 0 && z.y == 1 && z.z == 0 && z.w == 0);
  z = (V4) {0, 0, 100, 0};
  V4Normalize(&z, &z);
  TEST_EXPECT(z.x == 0 && z.y == 0 && z.z == 1 && z.w == 0);
  z = (V4) {0, 0, 0, 100};
  V4Normalize(&z, &z);
  TEST_EXPECT(z.x == 0 && z.y == 0 && z.z == 0 && z.w == 1);

  TEST_EXPECT(V4DotV4(&x, &y) == 70);
  TEST_EXPECT(V4InnerMultV4(&x, &y) == 70);
  TEST_EXPECT(V4LengthSq(&x) == 30);
  TEST_EXPECT(V4Length(&x) == F32Sqrt(30));

  x = (V4) {0, 0, 0, 0};
  y = (V4) {1, 0, 0, 0};
  V4Lerp(&x, &y, 0.5, &z);
  TEST_EXPECT(z.x == 0.5f && z.y == 0 && z.z == 0 && z.w == 0);
  V4Lerp(&x, &y, 0.75, &z);
  TEST_EXPECT(z.x == 0.75f && z.y == 0 && z.z == 0 && z.w == 0);

  return true;
}

int main(void) {
  TEST(V2Test());
  TEST(V3Test());
  TEST(V4Test());
  return 0;
}
