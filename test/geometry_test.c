#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

void Ray3IntersectRay3Test() {
  V3 a_start, a_dir, b_start, b_dir, intersect;

  // NOTE: basic intersection
  a_start = V3Assign(0, 0, 0); a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(1, 1, 0); b_dir = V3Assign(0, -1, 0);
  EXPECT_TRUE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(1, 0, 0));

  // NOTE: skew
  a_start = V3Assign(0, 0, 0); a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(0, 1, 1); b_dir = V3Assign(0, 1, 0);
  EXPECT_FALSE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));

  // NOTE: parallel
  a_start = V3Assign(0, 0, 0); a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(0, 1, 0); b_dir = V3Assign(1, 0, 0);
  EXPECT_FALSE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));

  // NOTE: collinear overlapping
  a_start = V3Assign(0, 0, 0); a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(2, 0, 0); b_dir = V3Assign(1, 0, 0);
  EXPECT_TRUE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(2, 0, 0));

  // NOTE: collinear opposing directions
  // TODO: this one case is inconsistent with the api contract, should return {0, 0, 0}
  a_start = V3Assign(0, 0, 0); a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(2, 0, 0); b_dir = V3Assign(-1, 0, 0);
  EXPECT_TRUE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(2, 0, 0));

  // NOTE: collinear disjoint
  a_start = V3Assign(0, 0, 0);  a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(-2, 0, 0); b_dir = V3Assign(-1, 0, 0);
  EXPECT_FALSE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));

  a_start = V3Assign(0,0,0); a_dir = V3Assign(1,0,0);
  b_start = V3Assign(0,0,0); b_dir = V3Assign(0,1,0);
  EXPECT_TRUE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(0,0,0));
}

int main(void) {
  RUN_TEST(Ray3IntersectRay3Test);
  LogTestReport();
  return 0;
}
