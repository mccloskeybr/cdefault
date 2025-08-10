#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_3DGEOMETRY_IMPLEMENTATION
#include "../cdefault_3dgeo.h"

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

  F32 c;
  c = V2DotV2(&x, &y);
  TEST_EXPECT(c == 11);
  c = V2InnerMultV2(&x, &y);
  TEST_EXPECT(c == 11);
  c = V2LengthSq(&x);
  TEST_EXPECT(c == 5);
  c = V2Length(&x);
  TEST_EXPECT(c == F32Sqrt(5));

  return true;
}

// TODO: cover all fns
B32 V3Test(void) {
  V3 x = { 1, 2, 3 };
  V3 y = { 4, 5, 6 };
  V3 zeros = V3Splat(0);

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

  F32 c;
  c = V3DotV3(&x, &y);
  TEST_EXPECT(c == 32);
  c = V3InnerMultV3(&x, &y);
  TEST_EXPECT(c == 32);
  c = V3LengthSq(&x);
  TEST_EXPECT(c == 14);
  c = V3Length(&x);
  TEST_EXPECT(c == F32Sqrt(14));
  c = V3MinValue(&x);
  TEST_EXPECT(c == 1);
  c = V3MaxValue(&x);
  TEST_EXPECT(c == 3);

  z = (V3) {2, 4, 6};
  TEST_EXPECT(V3IsBetween(&x, &zeros, &z));
  z = (V3) {2, 4, 7};
  TEST_EXPECT(!V3IsBetween(&x, &zeros, &z));

  return true;
}

int main(void) {
  TEST(V2Test());
  TEST(V3Test());
  return 0;
}
