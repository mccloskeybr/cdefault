#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"

B32 V2Test(void) {
  V2 x = { 1, 2 };
  V2 y = { 3, 4 };

  V2 z;
  z = V2AddV2(x, y);
  TEST_EXPECT(z.x == 4 && z.y == 6);
  z = V2AddF32(x, 1);
  TEST_EXPECT(z.x == 2 && z.y == 3);
  z = V2SubV2(x, y);
  TEST_EXPECT(z.x == -2 && z.y == -2);
  z = V2SubF32(x, 1);
  TEST_EXPECT(z.x == 0 && z.y == 1);
  z = V2MultF32(x, 2);
  TEST_EXPECT(z.x == 2 && z.y == 4);
  z = V2DivF32(x, 2);
  TEST_EXPECT(z.x == 0.5f && z.y == 1);
  z = V2Normalize((V2) {100, 0});
  TEST_EXPECT(z.x == 1 && z.y == 0);
  z = V2Normalize((V2) {0, 100});
  TEST_EXPECT(z.x == 0 && z.y == 1);

  F32 c;
  c = V2DotV2(x, y);
  TEST_EXPECT(c == 11);
  c = V2InnerMultV2(x, y);
  TEST_EXPECT(c == 11);
  c = V2LengthSq(x);
  TEST_EXPECT(c == 5);
  c = V2Length(x);
  TEST_EXPECT(c == F32Sqrt(5));

  return true;
}

// TODO: cover all fns
B32 V3Test(void) {
  V3 x = { 1, 2, 3 };
  V3 y = { 4, 5, 6 };

  V3 z;
  z = V3AddV3(x, y);
  TEST_EXPECT(z.x == 5 && z.y == 7 && z.z == 9);
  z = V3AddF32(x, 1);
  TEST_EXPECT(z.x == 2 && z.y == 3 && z.z == 4);
  z = V3SubV3(x, y);
  TEST_EXPECT(z.x == -3 && z.y == -3 && z.z == -3);
  z = V3SubF32(x, 1);
  TEST_EXPECT(z.x == 0 && z.y == 1 && z.z == 2);
  z = V3MultF32(x, 2);
  TEST_EXPECT(z.x == 2 && z.y == 4 && z.z == 6);
  z = V3DivF32(x, 2);
  TEST_EXPECT(z.x == 0.5f && z.y == 1 && z.z == 1.5f);
  z = V3Normalize((V3) {100, 0, 0});
  TEST_EXPECT(z.x == 1 && z.y == 0 && z.z == 0);
  z = V3Normalize((V3) {0, 100, 0});
  TEST_EXPECT(z.x == 0 && z.y == 1 && z.z == 0);
  z = V3Normalize((V3) {0, 0, 100});
  TEST_EXPECT(z.x == 0 && z.y == 0 && z.z == 1);
  z = V3CrossV3(x, y);
  TEST_EXPECT(z.x == -3 && z.y == 6 && z.z == -3);

  F32 c;
  c = V3DotV3(x, y);
  TEST_EXPECT(c == 32);
  c = V3InnerMultV3(x, y);
  TEST_EXPECT(c == 32);
  c = V3LengthSq(x);
  TEST_EXPECT(c == 14);
  c = V3Length(x);
  TEST_EXPECT(c == F32Sqrt(14));
  c = V3MinValue(x);
  TEST_EXPECT(c == 1);
  c = V3MaxValue(x);
  TEST_EXPECT(c == 3);

  TEST_EXPECT(V3IsBetween(x, V3Splat(0), (V3){2, 4, 6}));
  TEST_EXPECT(!V3IsBetween(x, V3Splat(0), (V3){2, 4, 7}));

  return true;
}

int main(void) {
  TEST(V2Test());
  TEST(V3Test());
  return 0;
}
