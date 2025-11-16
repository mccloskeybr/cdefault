#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

void Ray3IntersectLine3Test() {
  V3 ray_start, ray_dir, line_start, line_end, intersect;

  // NOTE: ray hits
  ray_start  = V3Assign(1, -1, 0); ray_dir = V3Assign(0, 1, 0);
  line_start = V3Assign(0, 0, 0); line_end = V3Assign(2, 0, 0);
  EXPECT_TRUE(Ray3IntersectLine3(&ray_start, &ray_dir, &line_start, &line_end, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(1, 0, 0));

  // NOTE: ray misses
  ray_start  = V3Assign(3, 0, 0); ray_dir  = V3Assign(1, 0, 0);   // moving away
  line_start = V3Assign(0, 0, 0); line_end = V3Assign(2, 0, 0);
  EXPECT_FALSE(Ray3IntersectLine3(&ray_start, &ray_dir, &line_start, &line_end, NULL));

  // NOTE: intersection before ray
  ray_start  = V3Assign(11, 0, 0); ray_dir  = V3Assign(1, 0, 0);  // pointing +X
  line_start = V3Assign(0, 0, 0);  line_end = V3Assign(10, 0, 0);
  EXPECT_FALSE(Ray3IntersectLine3(&ray_start, &ray_dir, &line_start, &line_end, NULL));

  // NOTE: intersection @ endpoint
  ray_start  = V3Assign(2, -1, 0); ray_dir  = V3Assign(0, 1, 0);
  line_start = V3Assign(0, 0, 0);  line_end = V3Assign(2, 0, 0);
  EXPECT_TRUE(Ray3IntersectLine3(&ray_start, &ray_dir, &line_start, &line_end, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(2, 0, 0));

  // NOTE: collinear + overlapping, intersection at ray_start
  ray_start  = V3Assign(1, 0, 0); ray_dir  = V3Assign(1, 0, 0);
  line_start = V3Assign(0, 0, 0); line_end = V3Assign(5, 0, 0);
  EXPECT_TRUE(Ray3IntersectLine3(&ray_start, &ray_dir, &line_start, &line_end, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(1, 0, 0));

  // NOTE: ray intersects infinite line but intersection is outside the segment bounds
  ray_start  = V3Assign(5, 1, 0); ray_dir  = V3Assign(0, -1, 0);
  line_start = V3Assign(0, 0, 0); line_end = V3Assign(4, 0, 0);
  EXPECT_FALSE(Ray3IntersectLine3(&ray_start, &ray_dir, &line_start, &line_end, NULL));

  // NOTE: zero-length line
  ray_start  = V3Assign(0, 0, 0); ray_dir  = V3Assign(1, 0, 0);
  line_start = V3Assign(2, 2, 2); line_end = V3Assign(2, 2, 2);
  EXPECT_FALSE(Ray3IntersectLine3(&ray_start, &ray_dir, &line_start, &line_end, NULL));
}

void Ray3IntersectRay3Test() {
  V3 a_start, a_dir, b_start, b_dir, intersect;

  // NOTE: skew intersect
  a_start = V3Assign(0, 0, 0); a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(1, 1, 0); b_dir = V3Assign(0, -1, 0);
  EXPECT_TRUE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(1, 0, 0));

  // NOTE: skew no intersect
  a_start = V3Assign(0, 0, 0); a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(0, 1, 1); b_dir = V3Assign(0, 1, 0);
  EXPECT_FALSE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));

  // NOTE: skew intersect behind ray
  a_start = V3Assign(0, 0, 0); a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(1, 1, 0); b_dir = V3Assign(0, 1, 0);
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
  a_start = V3Assign(0, 0, 0); a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(2, 0, 0); b_dir = V3Assign(-1, 0, 0);
  EXPECT_TRUE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(0, 0, 0));

  // NOTE: collinear disjoint
  a_start = V3Assign(0, 0, 0);  a_dir = V3Assign(1, 0, 0);
  b_start = V3Assign(-2, 0, 0); b_dir = V3Assign(-1, 0, 0);
  EXPECT_FALSE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));

  // NOTE: same starting point
  a_start = V3Assign(0,0,0); a_dir = V3Assign(1,0,0);
  b_start = V3Assign(0,0,0); b_dir = V3Assign(0,1,0);
  EXPECT_TRUE(Ray3IntersectRay3(&a_start, &a_dir, &b_start, &b_dir, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(0,0,0));
}

void Line3IntersectLine3Test() {
  V3 a_start, a_end, b_start, b_end, intersect;

  // NOTE: skew lines intersect
  a_start = V3Assign(0, 0, 0);  a_end = V3Assign(2, 0, 0);
  b_start = V3Assign(1, -1, 0); b_end = V3Assign(1, 1, 0);
  EXPECT_TRUE(Line3IntersectLine3(&a_start, &a_end, &b_start, &b_end, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(1, 0, 0));

  // NOTE: skew lines no intersect
  a_start = V3Assign(0, 0, 0); a_end = V3Assign(1, 0, 0);
  b_start = V3Assign(0, 1, 1); b_end = V3Assign(0, -1, 1);
  EXPECT_FALSE(Line3IntersectLine3(&a_start, &a_end, &b_start, &b_end, &intersect));

  // NOTE: parallel but disjoint
  a_start = V3Assign(0, 0, 0); a_end = V3Assign(2, 0, 0);
  b_start = V3Assign(0, 1, 0); b_end = V3Assign(2, 1, 0);
  EXPECT_FALSE(Line3IntersectLine3(&a_start, &a_end, &b_start, &b_end, &intersect));

  // NOTE: collinear overlapping
  a_start = V3Assign(0, 0, 0); a_end = V3Assign(4, 0, 0);
  b_start = V3Assign(2, 0, 0); b_end = V3Assign(6, 0, 0);
  EXPECT_TRUE(Line3IntersectLine3(&a_start, &a_end, &b_start, &b_end, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(2, 0, 0));

  // NOTE: intersection @ shared endpoint
  a_start = V3Assign(-1, 0, 0); a_end = V3Assign(1, 0, 0);
  b_start = V3Assign(1, 0, 0);  b_end = V3Assign(1, 1, 0);
  EXPECT_TRUE(Line3IntersectLine3(&a_start, &a_end, &b_start, &b_end, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(1, 0, 0));

  // NOTE: intersection exists on infinite lines, but outside segment B
  a_start = V3Assign(0, 0, 0); a_end = V3Assign(10, 0, 0);
  b_start = V3Assign(5, 1, 0); b_end = V3Assign(5, 3, 0);
  EXPECT_FALSE(Line3IntersectLine3(&a_start, &a_end, &b_start, &b_end, &intersect));

  // NOTE: zero-length segment
  a_start = V3Assign(0, 0, 0); a_end = V3Assign(1, 0, 0);
  b_start = V3Assign(2, 2, 2); b_end = V3Assign(2, 2, 2);
  EXPECT_FALSE(Line3IntersectLine3(&a_start, &a_end, &b_start, &b_end, &intersect));
}

void Line3IntersectRay3Test() {
  V3 line_start, line_end, ray_start, ray_dir, intersect;

  // NOTE: ray hits segment
  line_start = V3Assign(0, 0, 0); line_end = V3Assign(2, 0, 0);
  ray_start = V3Assign(1, -1, 0); ray_dir  = V3Assign(0, 1, 0);
  EXPECT_TRUE(Line3IntersectRay3(&line_start, &line_end, &ray_start, &ray_dir, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(1, 0, 0));

  // NOTE: ray misses segment
  line_start = V3Assign(0, 0, 0); line_end = V3Assign(1, 0, 0);
  ray_start = V3Assign(2, 1, 0);  ray_dir  = V3Assign(0, -1, 0);
  EXPECT_FALSE(Line3IntersectRay3(&line_start, &line_end, &ray_start, &ray_dir, &intersect));

  // NOTE: intersection at ray T < 0
  line_start = V3Assign(0, 0, 0); line_end = V3Assign(10, 0, 0);
  ray_start = V3Assign(5, 1, 0) ; ray_dir  = V3Assign(0, 1, 0);
  EXPECT_FALSE(Line3IntersectRay3(&line_start, &line_end, &ray_start, &ray_dir, &intersect));

  // NOTE: intersect @ line endpoint
  line_start = V3Assign(0, 0, 0); line_end = V3Assign(2, 0, 0);
  ray_start = V3Assign(2, -1, 0);  ray_dir = V3Assign(0, 1, 0);
  EXPECT_TRUE(Line3IntersectRay3(&line_start, &line_end, &ray_start, &ray_dir, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(2, 0, 0));

  // NOTE: collinear and overlapping
  line_start = V3Assign(0, 0, 0); line_end = V3Assign(5, 0, 0);
  ray_start = V3Assign(-2, 0, 0); ray_dir  = V3Assign(1, 0, 0);
  EXPECT_TRUE(Line3IntersectRay3(&line_start, &line_end, &ray_start, &ray_dir, &intersect));
  EXPECT_V3_APPROX_EQ(intersect, V3Assign(0, 0, 0));

  // NOTE: zero-length line — should return false
  line_start = V3Assign(1, 1, 1); line_end = V3Assign(1, 1, 1);
  ray_start = V3Assign(0, 0, 0);  ray_dir  = V3Assign(1, 1, 1);
  EXPECT_FALSE(Line3IntersectRay3(&line_start, &line_end, &ray_start, &ray_dir, &intersect));
}

int main(void) {
  RUN_TEST(Ray3IntersectLine3Test);
  RUN_TEST(Ray3IntersectRay3Test);
  RUN_TEST(Line3IntersectLine3Test);
  RUN_TEST(Line3IntersectRay3Test);
  LogTestReport();
  return 0;
}
