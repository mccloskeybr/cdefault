#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

// TODO: test 2d contact points from intersections
// TODO: ray3 attribute tests
// TODO: line3 attribute tests

void Line2EqTest() {
  V2 a0, a1, b0, b1;

  a0 = V2Assign(0, 0); a1 = V2Assign(1, 1);
  b0 = V2Assign(0, 0); b1 = V2Assign(1, 1);
  EXPECT_TRUE(Line2Eq(&a0, &a1, &b0, &b1));
  EXPECT_TRUE(Line2ApproxEq(&a0, &a1, &b0, &b1));

  a0 = V2Assign(0, 0); a1 = V2Assign(1, 1);
  b0 = V2Assign(0, 0); b1 = V2Assign(1, 2);
  EXPECT_FALSE(Line2Eq(&a0, &a1, &b0, &b1));
  EXPECT_FALSE(Line2ApproxEq(&a0, &a1, &b0, &b1));
}

void Line2MutateTest() {
  V2 a, b;

  // NOTE: offset
  a = V2Assign(1, 2); b = V2Assign(4, 6);
  V2 offset = V2Assign(2, -1);
  Line2Offset(&a, &b, &offset);
  EXPECT_V2_APPROX_EQ(a, V2Assign(3, 1));
  EXPECT_V2_APPROX_EQ(b, V2Assign(6, 5));

  // NOTE: rot about a
  a = V2Assign(0, 0); b = V2Assign(1, 0);
  V2 rotate_point = V2Assign(0, 0);
  Line2RotateAboutPoint(&a, &b, &rotate_point, F32_PI / 2.0f);
  EXPECT_V2_APPROX_EQ(a, V2Assign(0, 0));
  EXPECT_V2_APPROX_EQ(b, V2Assign(0, 1));

  // NOTE: rot about midpoint
  a = V2Assign(2, 2); b = V2Assign(4, 2);
  rotate_point = V2Assign(3, 2);
  Line2RotateAboutPoint(&a, &b, &rotate_point, F32_PI / 2.0f);
  EXPECT_V2_APPROX_EQ(a, V2Assign(3, 1));
  EXPECT_V2_APPROX_EQ(b, V2Assign(3, 3));
}

void Line2QueryTest() {
  V2 a = V2Assign(0, 0), b = V2Assign(4, 6);

  V2 midpoint;
  Line2GetMidpoint(&a, &b, &midpoint);
  EXPECT_V2_APPROX_EQ(midpoint, V2Assign(2, 3));

  V2 closest;
  a = V2Assign(0, 0); b = V2Assign(10, 0);
  // NOTE: above middle
  V2 point = V2Assign(5, 4);
  Line2GetClosestPoint(&a, &b, &point, &closest);
  EXPECT_V2_APPROX_EQ(closest, V2Assign(5, 0));

  // NOTE: after end
  point = V2Assign(20, 3);
  Line2GetClosestPoint(&a, &b, &point, &closest);
  EXPECT_V2_APPROX_EQ(closest, V2Assign(10, 0));

  // NOTE: before start
  point = V2Assign(-5, -2);
  Line2GetClosestPoint(&a, &b, &point, &closest);
  EXPECT_V2_APPROX_EQ(closest, V2Assign(0, 0));

  V2 normal;
  a = V2Assign(0, 0); b = V2Assign(1, 0);
  Line2GetNormalIn(&a, &b, &normal);
  EXPECT_V2_APPROX_EQ(normal, V2Assign(0, 1));
  Line2GetNormalOut(&a, &b, &normal);
  EXPECT_V2_APPROX_EQ(normal, V2Assign(0, -1));

  a = V2Assign(0, 0); b = V2Assign(0, 1);
  Line2GetNormalIn(&a, &b, &normal);
  EXPECT_V2_APPROX_EQ(normal, V2Assign(-1, 0));
  Line2GetNormalOut(&a, &b, &normal);
  EXPECT_V2_APPROX_EQ(normal, V2Assign(1, 0));

  a = V2Assign(0, 0); b = V2Assign(0, 1);
  point = V2Assign(0, 0.5f);
  EXPECT_TRUE(Line2ContainsPoint(&a, &b, &point));
  point = V2Assign(1, 0.5f);
  EXPECT_FALSE(Line2ContainsPoint(&a, &b, &point));
}

void Line2IntersectLine2Test() {
  V2 a0, a1, b0, b1, ip;

  // NOTE: simple perpendicular intersection
  a0 = V2Assign(0, 0);  a1 = V2Assign(10, 0);
  b0 = V2Assign(5, -5); b1 = V2Assign(5, 5);
  EXPECT_TRUE(Line2IntersectLine2(&a0, &a1, &b0, &b1, &ip));
  EXPECT_V2_APPROX_EQ(ip, V2Assign(5, 0));

  // NOTE: skew intersection (not axis-aligned)
  a0 = V2Assign(0, 0);   a1 = V2Assign(3, 3);
  b0 = V2Assign(0, 3);   b1 = V2Assign(3, 0);
  EXPECT_TRUE(Line2IntersectLine2(&a0, &a1, &b0, &b1, &ip));
  EXPECT_V2_APPROX_EQ(ip, V2Assign(1.5f, 1.5f));

  // NOTE: parallel but non-collinear — no intersection
  a0 = V2Assign(0, 0);   a1 = V2Assign(5, 0);
  b0 = V2Assign(0, 1);   b1 = V2Assign(5, 1);
  EXPECT_FALSE(Line2IntersectLine2(&a0, &a1, &b0, &b1, &ip));

  // NOTE: collinear but non-overlapping — no intersection
  a0 = V2Assign(0, 0);   a1 = V2Assign(2, 0);
  b0 = V2Assign(3, 0);   b1 = V2Assign(5, 0);
  EXPECT_FALSE(Line2IntersectLine2(&a0, &a1, &b0, &b1, &ip));

  // NOTE: collinear and overlapping
  a0 = V2Assign(0, 0);   a1 = V2Assign(5, 0);
  b0 = V2Assign(3, 0);   b1 = V2Assign(8, 0);
  EXPECT_TRUE(Line2IntersectLine2(&a0, &a1, &b0, &b1, &ip));
  EXPECT_V2_APPROX_EQ(ip, V2Assign(3, 0));

  // NOTE: lines would intersect if extended, but segments do not
  a0 = V2Assign(0, 0);   a1 = V2Assign(1, 1);
  b0 = V2Assign(2, 0);   b1 = V2Assign(3, 1);
  EXPECT_FALSE(Line2IntersectLine2(&a0, &a1, &b0, &b1, &ip));
}

void Line2IntersectRay2Test() {
  V2 line0, line1, ray_start, ray_dir, ip;

  // NOTE: simple perpendicular intersection
  line0 = V2Assign(0, 0); line1 = V2Assign(10, 0);
  ray_start = V2Assign(5, -5);
  ray_dir   = V2Assign(0, 1);
  EXPECT_TRUE(Line2IntersectRay2(&line0, &line1, &ray_start, &ray_dir, &ip));
  EXPECT_V2_APPROX_EQ(ip, V2Assign(5, 0));

  // NOTE: ray pointing away — no intersection
  line0 = V2Assign(0, 0); line1 = V2Assign(10, 0);
  ray_start = V2Assign(5, -5);
  ray_dir   = V2Assign(0, -1);
  EXPECT_FALSE(Line2IntersectRay2(&line0, &line1, &ray_start, &ray_dir, &ip));

  // NOTE: ray intersects line extension but not the segment
  line0 = V2Assign(0, 0); line1 = V2Assign(2, 0);
  ray_start = V2Assign(5, 5);
  ray_dir   = V2Assign(-1, -1); V2Normalize(&ray_dir, &ray_dir);
  EXPECT_TRUE(Line2IntersectRay2(&line0, &line1, &ray_start, &ray_dir, &ip));
  EXPECT_V2_APPROX_EQ(ip, V2Assign(0, 0));

  // NOTE: ray is collinear with line but starting outside the segment
  line0 = V2Assign(0, 0); line1 = V2Assign(5, 0);
  ray_start = V2Assign(-2, 0);
  ray_dir   = V2Assign(1, 0);
  EXPECT_TRUE(Line2IntersectRay2(&line0, &line1, &ray_start, &ray_dir, &ip));
  EXPECT_V2_APPROX_EQ(ip, V2Assign(0, 0));

  // NOTE: collinear but ray points the wrong way
  line0 = V2Assign(0, 0); line1 = V2Assign(5, 0);
  ray_start = V2Assign(-2, 0);
  ray_dir   = V2Assign(-1, 0);
  EXPECT_FALSE(Line2IntersectRay2(&line0, &line1, &ray_start, &ray_dir, &ip));

  // NOTE: skew intersection
  line0 = V2Assign(0, 3); line1 = V2Assign(3, 0);
  ray_start = V2Assign(0, 0);
  ray_dir   = V2Assign(1, 1); V2Normalize(&ray_dir, &ray_dir);
  EXPECT_TRUE(Line2IntersectRay2(&line0, &line1, &ray_start, &ray_dir, &ip));
  EXPECT_V2_APPROX_EQ(ip, V2Assign(1.5f, 1.5f));

  // NOTE: ray would hit the infinite line, but outside the segment
  line0 = V2Assign(0, 0); line1 = V2Assign(1, 0);
  ray_start = V2Assign(2, 5);
  ray_dir   = V2Assign(0, -1);
  EXPECT_FALSE(Line2IntersectRay2(&line0, &line1, &ray_start, &ray_dir, &ip));
}

void Line2IntersectTri2Test() {
  V2 line0, line1, tri[3], enter, exit;

  // NOTE: simple through-triangle intersection
  line0 = V2Assign(-1, -1); line1 = V2Assign(6, 6);
  tri[0] = V2Assign(0, 0); tri[1] = V2Assign(5, 0); tri[2] = V2Assign(0, 5);
  EXPECT_TRUE(Line2IntersectTri2(&line0, &line1, tri, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(2.5f, 2.5f));

  // NOTE: line segment fully misses the triangle
  line0 = V2Assign(10, 10); line1 = V2Assign(20, 20);
  tri[0] = V2Assign(0, 0); tri[1] = V2Assign(5, 0); tri[2] = V2Assign(0, 5);
  EXPECT_FALSE(Line2IntersectTri2(&line0, &line1, tri, &enter, &exit));

  // NOTE: line segment touches exactly one vertex
  line0 = V2Assign(5, -5); line1 = V2Assign(5,  5);
  tri[0] = V2Assign(0, 0); tri[1] = V2Assign(5, 0); tri[2] = V2Assign(0, 5);
  EXPECT_TRUE(Line2IntersectTri2(&line0, &line1, tri, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(5, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(5, 0));

  // NOTE: line segment overlaps a triangle edge
  // TODO: in this case we should probably capture the full edge, instead of just one point.
  line0 = V2Assign(2, 0); line1 = V2Assign(4, 0);
  tri[0] = V2Assign(0, 0); tri[1] = V2Assign(5, 0); tri[2] = V2Assign(0, 5);
  EXPECT_TRUE(Line2IntersectTri2(&line0, &line1, tri, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(2, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(2, 0));

  // NOTE: line enters triangle but exits outside segment range
  line0 = V2Assign(-1, 2); line1 = V2Assign( 1, 2);
  tri[0] = V2Assign(0, 0); tri[1] = V2Assign(4, 0); tri[2] = V2Assign(0, 4);
  EXPECT_TRUE(Line2IntersectTri2(&line0, &line1, tri, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0, 2));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(0, 2));

  // NOTE: segment lies completely inside the triangle
  line0 = V2Assign(1, 1); line1 = V2Assign(4, 4);
  tri[0] = V2Assign(0, 0); tri[1] = V2Assign(4, 0); tri[2] = V2Assign(0, 4);
  EXPECT_TRUE(Line2IntersectTri2(&line0, &line1, tri, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(2, 2));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(2, 2));
}

void Line2IntersectAabb2Test() {
  V2 line0, line1, center, size, enter, exit;

  // NOTE: simple diagonal crossing
  line0  = V2Assign(-1, -1); line1 = V2Assign( 6,  6);
  center = V2Assign(2.5f, 2.5f); size = V2Assign(5, 5);
  EXPECT_TRUE(Line2IntersectAabb2(&line0, &line1, &center, &size, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(5, 5));

  // NOTE: no intersection (segment completely outside)
  line0  = V2Assign(10, 10); line1  = V2Assign(20, 20);
  center = V2Assign(2.5f, 2.5f); size   = V2Assign(5, 5);
  EXPECT_FALSE(Line2IntersectAabb2(&line0, &line1, &center, &size, &enter, &exit));

  // NOTE: segment touches exactly one corner (5,5)
  line0  = V2Assign(5, 5); line1  = V2Assign(10, 10);
  center = V2Assign(2.5f, 2.5f); size   = V2Assign(5, 5);
  EXPECT_TRUE(Line2IntersectAabb2(&line0, &line1, &center, &size, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(5, 5));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(5, 5));

  // NOTE: segment grazes AABB entering at (0,2), but does not exit before end
  line0  = V2Assign(-1, 2); line1  = V2Assign( 1, 2);
  center = V2Assign(2, 2); size   = V2Assign(4, 4);
  EXPECT_TRUE(Line2IntersectAabb2(&line0, &line1, &center, &size, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0, 2));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(0, 2));

  // NOTE: infinite line intersects, but segment does not reach AABB
  line0  = V2Assign(6, 2); line1  = V2Assign(7, 2);
  center = V2Assign(2.5f, 2.5f); size   = V2Assign(5, 5);
  EXPECT_FALSE(Line2IntersectAabb2(&line0, &line1, &center, &size, &enter, &exit));
}

void Line2IntersectCircle2Test() {
  V2 line0, line1, center, enter, exit;
  F32 radius;

  // NOTE: simple two-point intersection
  line0  = V2Assign(-10, 0); line1  = V2Assign( 10, 0);
  center = V2Assign(0, 0); radius = 5.0f;
  EXPECT_TRUE(Line2IntersectCircle2(&line0, &line1, &center, radius, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-5, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign( 5, 0));

  // NOTE: tangent line (touches circle at one point)
  line0  = V2Assign(-10, 5); line1  = V2Assign( 10, 5);
  center = V2Assign(0, 0); radius = 5.0f;
  EXPECT_TRUE(Line2IntersectCircle2(&line0, &line1, &center, radius, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0, 5));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(0, 5));

  // NOTE: no intersection — line segment outside circle
  line0  = V2Assign(6, 6); line1  = V2Assign(8, 8);
  center = V2Assign(0, 0); radius = 5.0f;
  EXPECT_FALSE(Line2IntersectCircle2(&line0, &line1, &center, radius, &enter, &exit));

  // NOTE: line passes through circle, but segment does not reach it
  line0  = V2Assign(6, 0); line1  = V2Assign(10, 0);
  center = V2Assign(0, 0); radius = 5.0f;
  EXPECT_FALSE(Line2IntersectCircle2(&line0, &line1, &center, radius, &enter, &exit));

  // NOTE: one endpoint inside, one outside
  line0  = V2Assign(0, 0); line1  = V2Assign(10, 0);
  center = V2Assign(0, 0); radius = 5.0f;
  EXPECT_TRUE(Line2IntersectCircle2(&line0, &line1, &center, radius, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(5, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(5, 0));
}

void Line2IntersectConvexHull2Test() {
  V2 line0, line1;
  V2 hull[8];
  V2 enter, exit;

  // NOTE: simple square hull crossing
  line0 = V2Assign(-1, -1); line1 = V2Assign( 6,  6);
  hull[0] = V2Assign(0, 0); hull[1] = V2Assign(5, 0); hull[2] = V2Assign(5, 5); hull[3] = V2Assign(0, 5);
  EXPECT_TRUE(Line2IntersectConvexHull2(&line0, &line1, hull, 4, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(5, 5));

  // NOTE: no intersection (segment entirely outside)
  line0 = V2Assign(10, 10); line1 = V2Assign(12, 12);
  hull[0] = V2Assign(0, 0); hull[1] = V2Assign(5, 0); hull[2] = V2Assign(5, 5); hull[3] = V2Assign(0, 5);
  EXPECT_FALSE(Line2IntersectConvexHull2(&line0, &line1, hull, 4, &enter, &exit));

  // NOTE: segment touches exactly one vertex
  line0 = V2Assign(5, 5); line1 = V2Assign(10, 5);
  hull[0] = V2Assign(0, 0); hull[1] = V2Assign(5, 0); hull[2] = V2Assign(5, 5); hull[3] = V2Assign(0, 5);
  EXPECT_TRUE(Line2IntersectConvexHull2(&line0, &line1, hull, 4, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(5, 5));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(5, 5));

  // NOTE: line segment overlaps an edge
  line0 = V2Assign(1, 0); line1 = V2Assign(3, 0);
  hull[0] = V2Assign(0, 0); hull[1] = V2Assign(5, 0); hull[2] = V2Assign(5, 5); hull[3] = V2Assign(0, 5);
  EXPECT_TRUE(Line2IntersectConvexHull2(&line0, &line1, hull, 4, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(1, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(1, 0));

  // NOTE: pentagon hull — diagonal intersection
  line0 = V2Assign(-10, 0); line1 = V2Assign( 10, 0);
  hull[0] = V2Assign(-3, -1); hull[1] = V2Assign( 0, -3); hull[2] = V2Assign( 3, -1); hull[3] = V2Assign( 2,  3); hull[4] = V2Assign(-2,  3);
  EXPECT_TRUE(Line2IntersectConvexHull2(&line0, &line1, hull, 5, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-2.75f, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign( 2.75f, 0));

  // NOTE: infinite line intersects hull, but segment does not
  line0 = V2Assign(6, 1); line1 = V2Assign(7, 1);
  hull[0] = V2Assign(0, 0); hull[1] = V2Assign(5, 0); hull[2] = V2Assign(5, 5); hull[3] = V2Assign(0, 5);
  EXPECT_FALSE(Line2IntersectConvexHull2(&line0, &line1, hull, 4, &enter, &exit));
}

void Ray2EqTest() {
  V2 a_start, a_dir, b_start, b_dir;

  // NOTE: identical rays
  a_start = V2Assign(0, 0); a_dir = V2Assign(1, 0);
  b_start = V2Assign(0, 0); b_dir = V2Assign(1, 0);
  EXPECT_TRUE(Ray2Eq(&a_start, &a_dir, &b_start, &b_dir));
  EXPECT_TRUE(Ray2ApproxEq(&a_start, &a_dir, &b_start, &b_dir));

  // NOTE: same direction, different start
  a_start = V2Assign(0, 0); a_dir = V2Assign(1, 0);
  b_start = V2Assign(1, 0); b_dir = V2Assign(1, 0);
  EXPECT_FALSE(Ray2Eq(&a_start, &a_dir, &b_start, &b_dir));
  EXPECT_FALSE(Ray2ApproxEq(&a_start, &a_dir, &b_start, &b_dir));

  // NOTE: same start, different direction
  a_start = V2Assign(0, 0); a_dir = V2Assign(1, 0);
  b_start = V2Assign(0, 0); b_dir = V2Assign(0, 1);
  EXPECT_FALSE(Ray2Eq(&a_start, &a_dir, &b_start, &b_dir));
  EXPECT_FALSE(Ray2ApproxEq(&a_start, &a_dir, &b_start, &b_dir));
}

void Ray2RotateAboutPointTest() {
  V2 start, dir, point;

  start = V2Assign(0, 0); dir = V2Assign(1, 0);
  point = V2Assign(0, 0);
  Ray2RotateAboutPoint(&start, &dir, &point, F32_PI / 2.0f);
  EXPECT_V2_APPROX_EQ(start, V2Assign(0, 0));
  EXPECT_V2_APPROX_EQ(dir,   V2Assign(0, 1));
}

void Ray2ContainsPointTest() {
  V2 start, dir, point;

  start = V2Assign(0, 0); dir = V2Assign(1, 0);
  point = V2Assign(0, 0);
  EXPECT_TRUE(Ray2ContainsPoint(&start, &dir, &point));
  point = V2Assign(5, 0);
  EXPECT_TRUE(Ray2ContainsPoint(&start, &dir, &point));
  point = V2Assign(-1, 0);
  EXPECT_FALSE(Ray2ContainsPoint(&start, &dir, &point));
}

void Ray2IntersectLine2Test() {
  V2 r0, rd, l0, l1, ip;

  // NOTE: simple perpendicular
  r0 = V2Assign(0, 0); rd = V2Assign(1, 0);
  l0 = V2Assign(5, -5); l1 = V2Assign(5, 5);
  EXPECT_TRUE(Ray2IntersectLine2(&r0, &rd, &l0, &l1, &ip));
  EXPECT_V2_APPROX_EQ(ip, V2Assign(5, 0));

  // NOTE: intersection behind ray start → no hit
  r0 = V2Assign(10, 0); rd = V2Assign(1, 0);
  l0 = V2Assign(5, -1); l1 = V2Assign(5, 1);
  EXPECT_FALSE(Ray2IntersectLine2(&r0, &rd, &l0, &l1, &ip));

  // NOTE: parallel → no intersection
  r0 = V2Assign(0, 0); rd = V2Assign(1, 0);
  l0 = V2Assign(0, 1); l1 = V2Assign(10, 1);
  EXPECT_FALSE(Ray2IntersectLine2(&r0, &rd, &l0, &l1, &ip));
}

void Ray2IntersectRay2Test() {
  V2 a0, ad, b0, bd, ip;

  // NOTE: rays intersect at +5 on x-axis
  a0 = V2Assign(0, 0); ad = V2Assign(1, 0);
  b0 = V2Assign(10, 0); bd = V2Assign(-1, 0);
  EXPECT_TRUE(Ray2IntersectRay2(&a0, &ad, &b0, &bd, &ip));
  EXPECT_V2_APPROX_EQ(ip, V2Assign(0, 0));

  // NOTE: intersection behind ray
  a0 = V2Assign(5, 5); ad = V2Assign(1, 1); V2Normalize(&ad, &ad);
  b0 = V2Assign(0, 0); bd = V2Assign(-1, -1); V2Normalize(&bd, &bd);
  EXPECT_FALSE(Ray2IntersectRay2(&a0, &ad, &b0, &bd, &ip));

  // NOTE: parallel → no intersection
  a0 = V2Assign(0, 0); ad = V2Assign(1, 0);
  b0 = V2Assign(0, 1); bd = V2Assign(1, 0);
  EXPECT_FALSE(Ray2IntersectRay2(&a0, &ad, &b0, &bd, &ip));
}

void Ray2IntersectTri2Test() {
  V2 r0, rd, enter, exit;
  V2 tri[3];

  // NOTE: hit through triangle center
  tri[0] = V2Assign(0, 0);
  tri[1] = V2Assign(5, 0);
  tri[2] = V2Assign(0, 5);
  r0 = V2Assign(-5, -5); rd = V2Assign(1, 1); V2Normalize(&rd, &rd);
  EXPECT_TRUE(Ray2IntersectTri2(&r0, &rd, tri, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0, 0));   // triangle centroid hit
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(2.5f, 2.5f));

  // NOTE: miss triangle
  r0 = V2Assign(10, 10); rd = V2Assign(1, 0);
  EXPECT_FALSE(Ray2IntersectTri2(&r0, &rd, tri, &enter, &exit));
}

void Ray2IntersectAabb2Test() {
  V2 r0, rd, c, s, enter, exit;

  c = V2Assign(0, 0); s = V2Assign(1, 1);

  // NOTE: straight hit
  r0 = V2Assign(-5, 0); rd = V2Assign(1, 0);
  EXPECT_TRUE(Ray2IntersectAabb2(&r0, &rd, &c, &s, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-0.5f, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(+0.5f, 0));

  // NOTE: miss
  r0 = V2Assign(-5, 5); rd = V2Assign(1, 0);
  EXPECT_FALSE(Ray2IntersectAabb2(&r0, &rd, &c, &s, &enter, &exit));
}

void Ray2IntersectObb2Test() {
  V2 r0, rd, c, s, enter, exit;

  c = V2Assign(0, 0); s = V2Assign(1, 1);

  // NOTE: unrotated OBB (just an AABB)
  r0 = V2Assign(-5, 0); rd = V2Assign(1, 0);
  EXPECT_TRUE(Ray2IntersectObb2(&r0, &rd, &c, &s, 0.0f, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-0.5f, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(+0.5f, 0));

  // NOTE: rotated 45 degrees — still intersects
  EXPECT_TRUE(Ray2IntersectObb2(&r0, &rd, &c, &s, F32_PI / 4.0f, &enter, &exit));
}

void Ray2IntersectCircle2Test() {
  V2 r0, rd, c, enter, exit;
  float radius = 2.0f;

  c = V2Assign(0, 0);

  // NOTE: hit through center
  r0 = V2Assign(-5, 0); rd = V2Assign(1, 0);
  EXPECT_TRUE(Ray2IntersectCircle2(&r0, &rd, &c, radius, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-2, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(+2, 0));

  // NOTE: miss
  r0 = V2Assign(-5, 5); rd = V2Assign(1, 0);
  EXPECT_FALSE(Ray2IntersectCircle2(&r0, &rd, &c, radius, &enter, &exit));
}

void Ray2IntersectConvexHull2Test() {
  V2 r0, rd, enter, exit;
  V2 hull[4];

  // Square hull
  hull[0] = V2Assign(-1, -1);
  hull[1] = V2Assign(+1, -1);
  hull[2] = V2Assign(+1, +1);
  hull[3] = V2Assign(-1, +1);

  // NOTE: hit square
  r0 = V2Assign(-5, 0); rd = V2Assign(1, 0);
  EXPECT_TRUE(Ray2IntersectConvexHull2(&r0, &rd, hull, 4, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-1, 0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign( 1, 0));

  // NOTE: miss
  r0 = V2Assign(-5, 5); rd = V2Assign(1, 1); V2Normalize(&rd, &rd);
  EXPECT_FALSE(Ray2IntersectConvexHull2(&r0, &rd, hull, 4, &enter, &exit));
}

void Tri2EqTest() {
  V2 a[3], b[3];

  a[0] = V2Assign(0,0); a[1] = V2Assign(1,0); a[2] = V2Assign(0,1);
  b[0] = V2Assign(0,0); b[1] = V2Assign(1,0); b[2] = V2Assign(0,1);
  EXPECT_TRUE(Tri2Eq(a, b));
  EXPECT_TRUE(Tri2ApproxEq(a, b));

  b[0] = V2Assign(2,2);
  EXPECT_FALSE(Tri2Eq(a, b));
  EXPECT_FALSE(Tri2ApproxEq(a, b));
}

void Tri2MutateTest() {
  V2 tri[3];

  // NOTE: offset
  tri[0] = V2Assign(0,0); tri[1] = V2Assign(1,0); tri[2] = V2Assign(0,1);
  V2 offset = V2Assign(2, -3);
  Tri2Offset(tri, &offset);
  EXPECT_V2_APPROX_EQ(tri[0], V2Assign(2, -3));
  EXPECT_V2_APPROX_EQ(tri[1], V2Assign(3, -3));
  EXPECT_V2_APPROX_EQ(tri[2], V2Assign(2, -2));

  // NOTE: rotate about point
  tri[0] = V2Assign(0,0); tri[1] = V2Assign(2,0); tri[2] = V2Assign(0,2);
  V2 origin = V2Assign(0,0);
  Tri2RotateAboutPoint(tri, &origin, F32_PI / 2.0f);
  EXPECT_V2_APPROX_EQ(tri[0], V2Assign(0,0));
  EXPECT_V2_APPROX_EQ(tri[1], V2Assign(0,2));
  EXPECT_V2_APPROX_EQ(tri[2], V2Assign(-2,0));

  // NOTE: set center
  tri[0] = V2Assign(0,0); tri[1] = V2Assign(2,0); tri[2] = V2Assign(0,2);
  V2 new_center = V2Assign(10, 10);
  Tri2SetCenter(tri, &new_center);

  V2 computed_center;
  Tri2GetCenter(tri, &computed_center);
  EXPECT_V2_APPROX_EQ(computed_center, new_center);
}

void Tri2QueryTest() {
  V2 tri[3];

  tri[0] = V2Assign(0,0);
  tri[1] = V2Assign(4,0);
  tri[2] = V2Assign(0,4);

  V2 center; Tri2GetCenter(tri, &center);
  EXPECT_V2_APPROX_EQ(center, V2Assign(4.0f/3.0f, 4.0f/3.0f));

  V2 c; F32 r;
  Tri2GetEnclosingCircle2(tri, &c, &r);
  EXPECT_TRUE(r > 2.0f && r < 3.0f);
  EXPECT_TRUE(c.x > 1 && c.x < 3);
  EXPECT_TRUE(c.y > 1 && c.y < 3);

  V2 aabb_c, aabb_s;
  Tri2GetEnclosingAabb2(tri, &aabb_c, &aabb_s);
  EXPECT_V2_APPROX_EQ(aabb_c, V2Assign(1.33333f, 1.33333f));
  EXPECT_V2_APPROX_EQ(aabb_s, V2Assign(4, 4));

  V2 p;
  p = V2Assign(1,1);
  EXPECT_TRUE(Tri2ContainsPoint(tri, &p));
  p = V2Assign(5,5);
  EXPECT_FALSE(Tri2ContainsPoint(tri, &p));
  p = V2Assign(2,0);
  EXPECT_TRUE(Tri2ContainsPoint(tri, &p));
}

void Tri2IntersectLine2Test() {
  V2 tri[3];

  tri[0] = V2Assign(0,0);
  tri[1] = V2Assign(4,0);
  tri[2] = V2Assign(0,4);

  V2 a, b, enter, exit;

  // NOTE: simple intersection - diagonal slice
  a = V2Assign(-1,-1);
  b = V2Assign(5, 5);
  EXPECT_TRUE(Tri2IntersectLine2(tri, &a, &b, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0,0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(2,2));

  // NOTE: no intersection
  a = V2Assign(10,10);
  b = V2Assign(20,20);
  EXPECT_FALSE(Tri2IntersectLine2(tri, &a, &b, &enter, &exit));
}

void Tri2IntersectRay2Test() {
  V2 tri[3];

  tri[0] = V2Assign(0,0);
  tri[1] = V2Assign(4,0);
  tri[2] = V2Assign(0,4);

  V2 r0, rd, enter, exit;

  // NOTE: hit from bottom-left
  r0 = V2Assign(-5,-5);
  V2Normalize(&rd, &(V2){1,1});
  EXPECT_TRUE(Tri2IntersectRay2(tri, &r0, &rd, &enter, &exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0,0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(2,2));

  // NOTE: miss
  r0 = V2Assign(-5,5);
  V2Normalize(&rd, &(V2){1,0});
  EXPECT_FALSE(Tri2IntersectRay2(tri, &r0, &rd, &enter, &exit));
}

void Tri2IntersectTri2Test() {
  V2 a[3], b[3];
  IntersectManifold2 m;

  // NOTE: overlapping
  a[0]=V2Assign(0,0); a[1]=V2Assign(4,0); a[2]=V2Assign(0,4);
  b[0]=V2Assign(1,1); b[1]=V2Assign(5,1); b[2]=V2Assign(1,5);

  EXPECT_TRUE(Tri2IntersectTri2(a,b,&m));
  EXPECT_TRUE(m.penetration > 0.0f);
  EXPECT_TRUE(V2Length(&m.normal) > 0.9f);

  // NOTE: not intersecting
  b[0]=V2Assign(10,10); b[1]=V2Assign(14,10); b[2]=V2Assign(10,14);
  EXPECT_FALSE(Tri2IntersectTri2(a,b,&m));
}

void Tri2IntersectAabb2Test() {
  V2 tri[3], c, s;
  IntersectManifold2 m;

  tri[0]=V2Assign(0,0); tri[1]=V2Assign(4,0); tri[2]=V2Assign(0,4);

  // NOTE: AABB overlapping
  c = V2Assign(1,1);
  s = V2Assign(1,1);
  EXPECT_TRUE(Tri2IntersectAabb2(tri, &c, &s, &m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: no overlap
  c = V2Assign(10,10);
  EXPECT_FALSE(Tri2IntersectAabb2(tri, &c, &s, &m));
}

void Tri2IntersectObb2Test() {
  V2 tri[3], c, s;
  IntersectManifold2 m;

  tri[0]=V2Assign(0,0); tri[1]=V2Assign(4,0); tri[2]=V2Assign(0,4);

  c = V2Assign(2,2);
  s = V2Assign(1,1);

  // NOTE: rotated OBB intersecting
  EXPECT_TRUE(Tri2IntersectObb2(tri, &c, &s, F32_PI/4.0f, &m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: far away
  c = V2Assign(10,10);
  EXPECT_FALSE(Tri2IntersectObb2(tri, &c, &s, 0.0f, &m));
}

void Tri2IntersectCircle2Test() {
  V2 tri[3], cc;
  IntersectManifold2 m;

  tri[0]=V2Assign(0,0);
  tri[1]=V2Assign(4,0);
  tri[2]=V2Assign(0,4);

  // NOTE: circle intersecting
  cc = V2Assign(1,1);
  EXPECT_TRUE(Tri2IntersectCircle2(tri, &cc, 2.0f, &m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: circle far away
  cc = V2Assign(10,10);
  EXPECT_FALSE(Tri2IntersectCircle2(tri, &cc, 2.0f, &m));
}

void Tri2IntersectConvexHull2Test() {
  V2 tri[3];
  V2 hull[4];
  IntersectManifold2 m;

  tri[0]=V2Assign(0,0);
  tri[1]=V2Assign(4,0);
  tri[2]=V2Assign(0,4);

  hull[0]=V2Assign(1,1);
  hull[1]=V2Assign(3,1);
  hull[2]=V2Assign(3,3);
  hull[3]=V2Assign(1,3);

  // NOTE: convex hull overlapping triangle
  EXPECT_TRUE(Tri2IntersectConvexHull2(tri, hull, 4, &m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: hull far away
  hull[0]=V2Assign(10,10);
  hull[1]=V2Assign(11,10);
  hull[2]=V2Assign(11,11);
  hull[3]=V2Assign(10,11);
  EXPECT_FALSE(Tri2IntersectConvexHull2(tri, hull, 4, &m));
}

void Aabb2EqTest() {
  V2 ac, as, bc, bs;

  // NOTE: identical
  ac = V2Assign(0,0); as = V2Assign(4,4);
  bc = V2Assign(0,0); bs = V2Assign(4,4);
  EXPECT_TRUE(Aabb2Eq(&ac, &as, &bc, &bs));
  EXPECT_TRUE(Aabb2ApproxEq(&ac, &as, &bc, &bs));

  // NOTE: different center
  bc = V2Assign(1,0);
  EXPECT_FALSE(Aabb2Eq(&ac, &as, &bc, &bs));
  EXPECT_FALSE(Aabb2ApproxEq(&ac, &as, &bc, &bs));

  // NOTE: different size
  bc = V2Assign(0,0); bs = V2Assign(3,4);
  EXPECT_FALSE(Aabb2Eq(&ac, &as, &bc, &bs));
  EXPECT_FALSE(Aabb2ApproxEq(&ac, &as, &bc, &bs));
}

void Aabb2ConstructionTest() {
  V2 c, s, mn, mx;

  // NOTE: FromMinMax
  mn = V2Assign(-1,-2);
  mx = V2Assign( 3, 2);
  Aabb2FromMinMax(&c, &s, &mn, &mx);
  EXPECT_V2_APPROX_EQ(c, V2Assign(1,0));   // midpoint
  EXPECT_V2_APPROX_EQ(s, V2Assign(4,4));   // full size

  // NOTE: FromTopLeft
  V2 top_left = V2Assign(-5,5);
  V2 size     = V2Assign(4,4); // full size
  Aabb2FromTopLeft(&c, &top_left, &size);
  EXPECT_V2_APPROX_EQ(c, V2Assign(-3,3));  // center = top-left + size*0.5
  EXPECT_V2_APPROX_EQ(s, V2Assign(4,4));

  // NOTE: GetMinMax
  Aabb2GetMinMax(&c, &s, &mn, &mx);
  EXPECT_V2_APPROX_EQ(mn, V2Assign(-5,1)); // center - size*0.5 = (-3 -2, 3 -2)
  EXPECT_V2_APPROX_EQ(mx, V2Assign(-1,5)); // center + size*0.5 = (-3 +2, 3 +2)
}

void Aabb2QueryTest() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,4); // full size → half-size = (2,2)

  // NOTE: enclosing circle
  F32 r;
  Aabb2GetEnclosingCircle2(&c, &s, &r);
  EXPECT_F32_APPROX_EQ(r, V2Length(&(V2){2,2}));

  // NOTE: contains point (inside)
  V2 p = V2Assign(1,1);
  EXPECT_TRUE(Aabb2ContainsPoint(&c,&s,&p));

  // NOTE: on edge
  p = V2Assign(2,0);
  EXPECT_TRUE(Aabb2ContainsPoint(&c,&s,&p));

  // NOTE: outside
  p = V2Assign(5,0);
  EXPECT_FALSE(Aabb2ContainsPoint(&c,&s,&p));
}

void Aabb2RotateAboutPointTest() {
  V2 c = V2Assign(2,0);
  V2 s = V2Assign(4,4);
  V2 pivot = V2Assign(0,0);

  Aabb2RotateAboutPoint(&c, &s, &pivot, F32_PI/2.0f);
  EXPECT_V2_APPROX_EQ(c, V2Assign(0,2));  // rotate (2,0) → (0,2)
  EXPECT_V2_APPROX_EQ(s, V2Assign(4,4));  // unchanged
}

void Aabb2IntersectLine2Test() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,4); // half-size = (2,2)
  V2 a, b, enter, exit;

  // NOTE: horizontal line passing through box
  a = V2Assign(-5,0);
  b = V2Assign(5,0);
  EXPECT_TRUE(Aabb2IntersectLine2(&c,&s,&a,&b,&enter,&exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-2,0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign( 2,0));

  // NOTE: miss
  a = V2Assign(-5,5);
  b = V2Assign( 5,5);
  EXPECT_FALSE(Aabb2IntersectLine2(&c,&s,&a,&b,&enter,&exit));
}

void Aabb2IntersectRay2Test() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,4); // half-size = (2,2)
  V2 r0, rd, enter, exit;

  // NOTE: direct hit
  r0 = V2Assign(-5,0);
  V2Normalize(&rd, &(V2){1,0});
  EXPECT_TRUE(Aabb2IntersectRay2(&c,&s,&r0,&rd,&enter,&exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-2,0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign( 2,0));

  // NOTE: miss above
  r0 = V2Assign(-5,5);
  V2Normalize(&rd, &(V2){1,0});
  EXPECT_FALSE(Aabb2IntersectRay2(&c,&s,&r0,&rd,&enter,&exit));
}

void Aabb2IntersectTri2Test() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,4); // half-size = (2,2)
  V2 tri[3];
  IntersectManifold2 m;

  // NOTE: intersects
  tri[0] = V2Assign(1,1);
  tri[1] = V2Assign(5,1);
  tri[2] = V2Assign(1,5);
  EXPECT_TRUE(Aabb2IntersectTri2(&c,&s,tri,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: separate
  tri[0] = V2Assign(10,10);
  tri[1] = V2Assign(12,10);
  tri[2] = V2Assign(10,12);
  EXPECT_FALSE(Aabb2IntersectTri2(&c,&s,tri,&m));
}

void Aabb2IntersectAabb2Test() {
  V2 ac = V2Assign(0,0), as = V2Assign(4,4);
  V2 bc, bs;
  IntersectManifold2 m;

  // NOTE: overlapping
  bc = V2Assign(3,0); bs = V2Assign(4,4);
  EXPECT_TRUE(Aabb2IntersectAabb2(&ac,&as,&bc,&bs,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: no intersection
  bc = V2Assign(10,0);
  EXPECT_FALSE(Aabb2IntersectAabb2(&ac,&as,&bc,&bs,&m));
}

void Aabb2IntersectObb2Test() {
  V2 ac = V2Assign(0,0), as = V2Assign(4,4);
  V2 bc = V2Assign(2,0), bs = V2Assign(2,2);
  IntersectManifold2 m;

  // NOTE: rotated intersecting
  EXPECT_TRUE(Aabb2IntersectObb2(&ac,&as,&bc,&bs,F32_PI/4.0f,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: far away
  bc = V2Assign(10,10);
  EXPECT_FALSE(Aabb2IntersectObb2(&ac,&as,&bc,&bs,0,&m));
}

void Aabb2IntersectCircle2Test() {
  V2 c = V2Assign(0,0), s = V2Assign(4,4);
  V2 cc;
  IntersectManifold2 m;

  // NOTE: intersecting
  cc = V2Assign(1,1);
  EXPECT_TRUE(Aabb2IntersectCircle2(&c,&s,&cc,2.0f,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: separate
  cc = V2Assign(10,10);
  EXPECT_FALSE(Aabb2IntersectCircle2(&c,&s,&cc,2.0f,&m));
}

void Aabb2IntersectConvexHull2Test() {
  V2 c = V2Assign(0,0), s = V2Assign(4,4);
  V2 hull[4];
  IntersectManifold2 m;

  // NOTE: touching/intersecting hull
  hull[0]=V2Assign(1,1);
  hull[1]=V2Assign(3,1);
  hull[2]=V2Assign(3,3);
  hull[3]=V2Assign(1,3);
  EXPECT_TRUE(Aabb2IntersectConvexHull2(&c,&s,hull,4,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: far away
  hull[0]=V2Assign(10,10);
  hull[1]=V2Assign(12,10);
  hull[2]=V2Assign(12,12);
  hull[3]=V2Assign(10,12);
  EXPECT_FALSE(Aabb2IntersectConvexHull2(&c,&s,hull,4,&m));
}

void Obb2EqTest() {
  V2 ac, as, bc, bs;

  // NOTE: identical
  ac = V2Assign(0,0); as = V2Assign(4,4);
  bc = V2Assign(0,0); bs = V2Assign(4,4);
  EXPECT_TRUE(Obb2Eq(&ac,&as,0,&bc,&bs,0));
  EXPECT_TRUE(Obb2ApproxEq(&ac,&as,0,&bc,&bs,0));

  // NOTE: different angle
  EXPECT_FALSE(Obb2Eq(&ac,&as,0,&bc,&bs,F32_PI/4.0f));
  EXPECT_FALSE(Obb2ApproxEq(&ac,&as,0,&bc,&bs,F32_PI/4.0f));

  // NOTE: different center
  bc = V2Assign(1,0);
  EXPECT_FALSE(Obb2Eq(&ac,&as,0,&bc,&bs,0));
  EXPECT_FALSE(Obb2ApproxEq(&ac,&as,0,&bc,&bs,0));

  // NOTE: different size
  bc = V2Assign(0,0); bs = V2Assign(3,4);
  EXPECT_FALSE(Obb2Eq(&ac,&as,0,&bc,&bs,0));
  EXPECT_FALSE(Obb2ApproxEq(&ac,&as,0,&bc,&bs,0));
}

void Obb2RotateAboutPointTest() {
  V2 c = V2Assign(2,0);
  F32 ang = 0.0f;
  V2 pivot = V2Assign(0,0);

  // NOTE: rotate 90° CCW around origin
  Obb2RotateAboutPoint(&c, &ang, &pivot, F32_PI/2.0f);
  EXPECT_V2_APPROX_EQ(c, V2Assign(0,2));
  EXPECT_F32_APPROX_EQ(ang, F32_PI/2.0f);
}

void Obb2GetEnclosingCircle2Test() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,6); // full size → half-size = (2,3)
  F32 r;

  // NOTE: circle radius = length of half-diagonal
  Obb2GetEnclosingCircle2(&c,&s,0,&r);
  EXPECT_F32_APPROX_EQ(r, V2Length(&(V2){2,3}));
}

void Obb2GetEnclosingAabb2Test() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,2); // half-size = (2,1)
  V2 aabb_size;

  // NOTE: rotate 90° → width/height swap
  Obb2GetEnclosingAabb2(&c,&s,F32_PI/2.0f,&aabb_size);
  EXPECT_V2_APPROX_EQ(aabb_size, V2Assign(2,4)); // full-size envelope
}

void Obb2ContainsPointTest() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,4); // half-size = (2,2)
  F32 ang = 0.0f;

  // NOTE: inside
  V2 p = V2Assign(1,1);
  EXPECT_TRUE(Obb2ContainsPoint(&c,&s,ang,&p));

  // NOTE: on edge
  p = V2Assign(2,0);
  EXPECT_TRUE(Obb2ContainsPoint(&c,&s,ang,&p));

  // NOTE: outside
  p = V2Assign(5,0);
  EXPECT_FALSE(Obb2ContainsPoint(&c,&s,ang,&p));

  // NOTE: rotated OBB
  ang = F32_PI/4.0f;
  p = V2Assign(1,1);
  EXPECT_TRUE(Obb2ContainsPoint(&c,&s,ang,&p));
}

void Obb2IntersectLine2Test() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,4); // half-size = (2,2)
  F32 ang = 0;
  V2 a, b, enter, exit;

  // NOTE: simple axis-aligned intersection
  a = V2Assign(-5,0);
  b = V2Assign( 5,0);
  EXPECT_TRUE(Obb2IntersectLine2(&c,&s,ang,&a,&b,&enter,&exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-2,0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign( 2,0));

  // NOTE: miss
  a = V2Assign(-5,5);
  b = V2Assign( 5,5);
  EXPECT_FALSE(Obb2IntersectLine2(&c,&s,ang,&a,&b,&enter,&exit));
}

void Obb2IntersectRay2Test() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,4); // half-size = (2,2)
  F32 ang = 0;
  V2 r0, rd, enter, exit;

  // NOTE: hit
  r0 = V2Assign(-5,0);
  V2Normalize(&rd, &(V2){1,0});
  EXPECT_TRUE(Obb2IntersectRay2(&c,&s,ang,&r0,&rd,&enter,&exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-2,0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign( 2,0));

  // NOTE: miss
  r0 = V2Assign(-5,5);
  V2Normalize(&rd, &(V2){1,0});
  EXPECT_FALSE(Obb2IntersectRay2(&c,&s,ang,&r0,&rd,&enter,&exit));
}

void Obb2IntersectTri2Test() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,4);
  F32 ang = 0;
  IntersectManifold2 m;
  V2 tri[3];

  // NOTE: intersecting
  tri[0]=V2Assign(1,1);
  tri[1]=V2Assign(5,1);
  tri[2]=V2Assign(1,5);
  EXPECT_TRUE(Obb2IntersectTri2(&c,&s,ang,tri,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: separated
  tri[0]=V2Assign(10,10);
  tri[1]=V2Assign(12,10);
  tri[2]=V2Assign(10,12);
  EXPECT_FALSE(Obb2IntersectTri2(&c,&s,ang,tri,&m));
}

void Obb2IntersectAabb2Test() {
  V2 oc = V2Assign(0,0);
  V2 os = V2Assign(4,4);
  F32 ang = 0;
  V2 ac, as;
  IntersectManifold2 m;

  // NOTE: overlapping
  ac = V2Assign(3,0); as = V2Assign(4,4);
  EXPECT_TRUE(Obb2IntersectAabb2(&oc,&os,ang,&ac,&as,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: no intersection
  ac = V2Assign(10,0);
  EXPECT_FALSE(Obb2IntersectAabb2(&oc,&os,ang,&ac,&as,&m));
}

void Obb2IntersectObb2Test() {
  V2 ac = V2Assign(0,0), as = V2Assign(4,4);
  F32 aa = 0;
  V2 bc = V2Assign(2,0), bs = V2Assign(4,4);
  F32 ba = F32_PI/4.0f;
  IntersectManifold2 m;

  // NOTE: rotated intersection
  EXPECT_TRUE(Obb2IntersectObb2(&ac,&as,aa,&bc,&bs,ba,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: separated
  bc = V2Assign(10,10);
  EXPECT_FALSE(Obb2IntersectObb2(&ac,&as,aa,&bc,&bs,ba,&m));
}

void Obb2IntersectCircle2Test() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,4);
  F32 ang = 0;
  IntersectManifold2 m;

  // NOTE: intersection
  V2 cc = V2Assign(1,1);
  EXPECT_TRUE(Obb2IntersectCircle2(&c,&s,ang,&cc,2.0f,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: separate
  cc = V2Assign(10,10);
  EXPECT_FALSE(Obb2IntersectCircle2(&c,&s,ang,&cc,2.0f,&m));
}

void Obb2IntersectConvexHull2Test() {
  V2 c = V2Assign(0,0);
  V2 s = V2Assign(4,4);
  F32 ang = 0;
  IntersectManifold2 m;
  V2 hull[4];

  // NOTE: intersecting
  hull[0]=V2Assign(1,1);
  hull[1]=V2Assign(3,1);
  hull[2]=V2Assign(3,3);
  hull[3]=V2Assign(1,3);
  EXPECT_TRUE(Obb2IntersectConvexHull2(&c,&s,ang,hull,4,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: far away
  hull[0]=V2Assign(10,10);
  hull[1]=V2Assign(12,10);
  hull[2]=V2Assign(12,12);
  hull[3]=V2Assign(10,12);
  EXPECT_FALSE(Obb2IntersectConvexHull2(&c,&s,ang,hull,4,&m));
}

void Circle2EqTest() {
  V2 ac, bc;

  // NOTE: identical
  ac = V2Assign(0,0);
  bc = V2Assign(0,0);
  EXPECT_TRUE(Circle2Eq(&ac, 5.0f, &bc, 5.0f));
  EXPECT_TRUE(Circle2ApproxEq(&ac, 5.0f, &bc, 5.0f));

  // NOTE: different radius
  EXPECT_FALSE(Circle2Eq(&ac, 5.0f, &bc, 4.0f));
  EXPECT_FALSE(Circle2ApproxEq(&ac, 5.0f, &bc, 4.0f));

  // NOTE: different center
  bc = V2Assign(1,0);
  EXPECT_FALSE(Circle2Eq(&ac, 5.0f, &bc, 5.0f));
  EXPECT_FALSE(Circle2ApproxEq(&ac, 5.0f, &bc, 5.0f));
}

void Circle2GetEnclosingAabb2Test() {
  V2 size;

  // NOTE: AABB size = diameter
  Circle2GetEnclosingAabb2(3.0f, &size);
  EXPECT_V2_APPROX_EQ(size, V2Assign(6.0f, 6.0f));
}

void Circle2ContainsPointTest() {
  V2 c = V2Assign(0,0);
  F32 r = 3.0f;

  // NOTE: inside
  V2 p = V2Assign(1,1);
  EXPECT_TRUE(Circle2ContainsPoint(&c, r, &p));

  // NOTE: on boundary
  p = V2Assign(3,0);
  EXPECT_TRUE(Circle2ContainsPoint(&c, r, &p));

  // NOTE: outside
  p = V2Assign(4,0);
  EXPECT_FALSE(Circle2ContainsPoint(&c, r, &p));
}

void Circle2IntersectLine2Test() {
  V2 c = V2Assign(0,0);
  F32 r = 3.0f;
  V2 a, b, enter, exit;

  // NOTE: horizontal line crossing center
  a = V2Assign(-5,0);
  b = V2Assign( 5,0);
  EXPECT_TRUE(Circle2IntersectLine2(&c,r,&a,&b,&enter,&exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-3,0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign( 3,0));

  // NOTE: miss
  a = V2Assign(-5,5);
  b = V2Assign( 5,5);
  EXPECT_FALSE(Circle2IntersectLine2(&c,r,&a,&b,&enter,&exit));
}

void Circle2IntersectRay2Test() {
  V2 c = V2Assign(0,0);
  F32 r = 3.0f;
  V2 r0, rd, enter, exit;

  // NOTE: hit from left
  r0 = V2Assign(-5,0);
  V2Normalize(&rd, &(V2){1,0});
  EXPECT_TRUE(Circle2IntersectRay2(&c,r,&r0,&rd,&enter,&exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(-3,0));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign( 3,0));

  // NOTE: ray pointing away
  r0 = V2Assign(-5,0);
  V2Normalize(&rd, &(V2){-1,0});
  EXPECT_FALSE(Circle2IntersectRay2(&c,r,&r0,&rd,&enter,&exit));

  // NOTE: miss
  r0 = V2Assign(-5,5);
  V2Normalize(&rd, &(V2){1,0});
  EXPECT_FALSE(Circle2IntersectRay2(&c,r,&r0,&rd,&enter,&exit));
}

void Circle2IntersectTri2Test() {
  V2 c = V2Assign(0,0);
  F32 r = 3.0f;
  IntersectManifold2 m;
  V2 tri[3];

  // NOTE: triangle overlaps circle
  tri[0] = V2Assign(1,1);
  tri[1] = V2Assign(4,1);
  tri[2] = V2Assign(1,4);
  EXPECT_TRUE(Circle2IntersectTri2(&c,r,tri,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: triangle far away
  tri[0] = V2Assign(10,10);
  tri[1] = V2Assign(12,10);
  tri[2] = V2Assign(10,12);
  EXPECT_FALSE(Circle2IntersectTri2(&c,r,tri,&m));
}

void Circle2IntersectAabb2Test() {
  V2 cc = V2Assign(0,0);
  F32 r = 3.0f;
  IntersectManifold2 m;
  V2 ac, as;

  // NOTE: overlapping
  ac = V2Assign(3,0);
  as = V2Assign(4,4);   // full size
  EXPECT_TRUE(Circle2IntersectAabb2(&cc,r,&ac,&as,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: separated
  ac = V2Assign(10,10);
  EXPECT_FALSE(Circle2IntersectAabb2(&cc,r,&ac,&as,&m));
}

void Circle2IntersectObb2Test() {
  V2 cc = V2Assign(0,0);
  F32 r = 3.0f;
  IntersectManifold2 m;

  V2 oc = V2Assign(2,0);
  V2 os = V2Assign(4,4); // full size
  F32 ang = F32_PI/4.0f;

  // NOTE: intersect
  EXPECT_TRUE(Circle2IntersectObb2(&cc,r,&oc,&os,ang,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: separated
  oc = V2Assign(10,10);
  EXPECT_FALSE(Circle2IntersectObb2(&cc,r,&oc,&os,ang,&m));
}

void Circle2IntersectCircle2Test() {
  V2 a = V2Assign(0,0);
  V2 b = V2Assign(4,0);
  IntersectManifold2 m;

  // NOTE: intersect (radii sum = 6)
  EXPECT_TRUE(Circle2IntersectCircle2(&a,3.0f,&b,3.0f,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: tangent (touching)
  b = V2Assign(6,0);
  EXPECT_TRUE(Circle2IntersectCircle2(&a,3.0f,&b,3.0f,&m));
  EXPECT_TRUE(m.penetration >= 0);

  // NOTE: separated
  b = V2Assign(10,0);
  EXPECT_FALSE(Circle2IntersectCircle2(&a,3.0f,&b,3.0f,&m));
}

void Circle2IntersectConvexHull2Test() {
  V2 c = V2Assign(0,0);
  F32 r = 3.0f;
  IntersectManifold2 m;
  V2 hull[4];

  // NOTE: overlapping hull
  hull[0] = V2Assign(1,1);
  hull[1] = V2Assign(3,1);
  hull[2] = V2Assign(3,3);
  hull[3] = V2Assign(1,3);
  EXPECT_TRUE(Circle2IntersectConvexHull2(&c,r,hull,4,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: far away
  hull[0] = V2Assign(10,10);
  hull[1] = V2Assign(12,10);
  hull[2] = V2Assign(12,12);
  hull[3] = V2Assign(10,12);
  EXPECT_FALSE(Circle2IntersectConvexHull2(&c,r,hull,4,&m));
}

void ConvexHull2EqTest() {
  V2 a[4], b[4];

  // NOTE: identical squares
  a[0]=V2Assign(0,0); a[1]=V2Assign(2,0); a[2]=V2Assign(2,2); a[3]=V2Assign(0,2);
  b[0]=V2Assign(0,0); b[1]=V2Assign(2,0); b[2]=V2Assign(2,2); b[3]=V2Assign(0,2);
  EXPECT_TRUE(ConvexHull2Eq(a,4,b,4));
  EXPECT_TRUE(ConvexHull2ApproxEq(a,4,b,4));

  // NOTE: different shape
  b[2]=V2Assign(3,2);
  EXPECT_FALSE(ConvexHull2Eq(a,4,b,4));
  EXPECT_FALSE(ConvexHull2ApproxEq(a,4,b,4));
}

void ConvexHull2FromAabb2Test() {
  V2 hc[4];
  V2 center = V2Assign(3,3);
  V2 size   = V2Assign(4,6);  // full size

  ConvexHull2FromAabb2(hc,&center,&size);

  // min/max
  V2 min = V2Assign(1,0);
  V2 max = V2Assign(5,6);

  EXPECT_V2_APPROX_EQ(hc[0], V2Assign(min.x,min.y));
  EXPECT_V2_APPROX_EQ(hc[1], V2Assign(max.x,min.y));
  EXPECT_V2_APPROX_EQ(hc[2], V2Assign(max.x,max.y));
  EXPECT_V2_APPROX_EQ(hc[3], V2Assign(min.x,max.y));
}

void ConvexHull2FromObb2Test() {
  V2 hc[4];
  V2 center = V2Assign(0,0);
  V2 size   = V2Assign(4,2); // full extent 4×2
  F32 ang   = F32_PI/2.0f;

  ConvexHull2FromObb2(hc,&center,&size,ang);

  // rectangle rotated 90° → width/height swap
  EXPECT_V2_APPROX_EQ(hc[0], V2Assign( 1, -2));
  EXPECT_V2_APPROX_EQ(hc[1], V2Assign( 1,  2));
  EXPECT_V2_APPROX_EQ(hc[2], V2Assign(-1,  2));
  EXPECT_V2_APPROX_EQ(hc[3], V2Assign(-1, -2));
}

void ConvexHull2OffsetTest() {
  V2 h[3];
  h[0]=V2Assign(0,0); h[1]=V2Assign(2,0); h[2]=V2Assign(1,2);

  V2 off = V2Assign(3,-1);
  ConvexHull2Offset(h,3,&off);

  EXPECT_V2_APPROX_EQ(h[0], V2Assign(3,-1));
  EXPECT_V2_APPROX_EQ(h[1], V2Assign(5,-1));
  EXPECT_V2_APPROX_EQ(h[2], V2Assign(4, 1));
}

void ConvexHull2RotateAboutPointTest() {
  V2 h[3], p;

  // NOTE: rotate triangle 90° about origin
  h[0]=V2Assign(1,0); h[1]=V2Assign(2,0); h[2]=V2Assign(1,1);
  p   = V2Assign(0,0);

  ConvexHull2RotateAboutPoint(h,3,&p,F32_PI/2.0f);

  EXPECT_V2_APPROX_EQ(h[0], V2Assign(0,1));
  EXPECT_V2_APPROX_EQ(h[1], V2Assign(0,2));
  EXPECT_V2_APPROX_EQ(h[2], V2Assign(-1,1));
}

void ConvexHull2CenterTest() {
  V2 h[4];
  h[0]=V2Assign(0,0);
  h[1]=V2Assign(4,0);
  h[2]=V2Assign(4,2);
  h[3]=V2Assign(0,2);

  // NOTE: center should be average of vertices = (2,1)
  V2 c;
  ConvexHull2GetCenter(h,4,&c);
  EXPECT_V2_APPROX_EQ(c, V2Assign(2,1));

  // NOTE: move center to (10,10)
  V2 newc = V2Assign(10,10);
  ConvexHull2SetCenter(h,4,&newc);

  EXPECT_V2_APPROX_EQ(h[0], V2Assign(8, 9));
  EXPECT_V2_APPROX_EQ(h[1], V2Assign(12,9));
  EXPECT_V2_APPROX_EQ(h[2], V2Assign(12,11));
  EXPECT_V2_APPROX_EQ(h[3], V2Assign(8,11));
}

void ConvexHull2GetEnclosingCircle2Test() {
  V2 h[4];
  h[0]=V2Assign(1,1);
  h[1]=V2Assign(5,1);
  h[2]=V2Assign(5,5);
  h[3]=V2Assign(1,5);

  V2 cc; F32 cr;
  ConvexHull2GetEnclosingCircle2(h,4,&cc,&cr);

  EXPECT_V2_APPROX_EQ(cc, V2Assign(3,3));  // center
  EXPECT_F32_APPROX_EQ(cr, 2.828427f);     // sqrt(2^2 + 2^2)
}

void ConvexHull2GetEnclosingAabb2Test() {
  V2 h[3];
  h[0]=V2Assign(1,2);
  h[1]=V2Assign(4,3);
  h[2]=V2Assign(2,6);

  V2 c, s;
  ConvexHull2GetEnclosingAabb2(h,3,&c,&s);

  V2 min = V2Assign(1,2); V2 max = V2Assign(4,6);

  EXPECT_V2_APPROX_EQ(c, V2Assign(2.33333f, 3.66667f));
  EXPECT_V2_APPROX_EQ(s, V2Assign(max.x-min.x, max.y-min.y)); // full size
}

void ConvexHull2ContainsPointTest() {
  V2 h[4];
  h[0]=V2Assign(0,0); h[1]=V2Assign(4,0);
  h[2]=V2Assign(4,4); h[3]=V2Assign(0,4);

  // inside
  V2 p = V2Assign(2,2);
  EXPECT_TRUE(ConvexHull2ContainsPoint(h,4,&p));

  // on boundary
  p = V2Assign(4,2);
  EXPECT_TRUE(ConvexHull2ContainsPoint(h,4,&p));

  // outside
  p = V2Assign(5,5);
  EXPECT_FALSE(ConvexHull2ContainsPoint(h,4,&p));
}

void ConvexHull2IntersectLine2Test() {
  V2 h[4];
  h[0]=V2Assign(0,0); h[1]=V2Assign(4,0);
  h[2]=V2Assign(4,4); h[3]=V2Assign(0,4);

  V2 a,b,enter,exit;

  // NOTE: horizontal line through center
  a=V2Assign(-2,2);
  b=V2Assign( 6,2);
  EXPECT_TRUE(ConvexHull2IntersectLine2(h,4,&a,&b,&enter,&exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0,2));
  EXPECT_V2_APPROX_EQ(exit,  V2Assign(4,2));

  // NOTE: miss
  a=V2Assign(-2,6);
  b=V2Assign(6,6);
  EXPECT_FALSE(ConvexHull2IntersectLine2(h,4,&a,&b,&enter,&exit));
}

void ConvexHull2IntersectRay2Test() {
  V2 h[4];
  h[0]=V2Assign(0,0); h[1]=V2Assign(4,0);
  h[2]=V2Assign(4,4); h[3]=V2Assign(0,4);

  V2 r0, rd, enter, exit;

  // NOTE: hit from left
  r0 = V2Assign(-5,2);
  V2Normalize(&rd, &(V2){1,0});
  EXPECT_TRUE(ConvexHull2IntersectRay2(h,4,&r0,&rd,&enter,&exit));
  EXPECT_V2_APPROX_EQ(enter, V2Assign(0,2));

  // NOTE: miss
  r0 = V2Assign(-5,6);
  V2Normalize(&rd, &(V2){1,0});
  EXPECT_FALSE(ConvexHull2IntersectRay2(h,4,&r0,&rd,&enter,&exit));

  // NOTE: ray pointing away
  r0 = V2Assign(6,2);
  V2Normalize(&rd, &(V2){1,0});
  EXPECT_FALSE(ConvexHull2IntersectRay2(h,4,&r0,&rd,&enter,&exit));
}

void ConvexHull2IntersectTri2Test() {
  V2 hull[4] = {
    V2Assign(0,0), V2Assign(4,0),
    V2Assign(4,4), V2Assign(0,4)
  };

  V2 tri[3];
  IntersectManifold2 m;

  // NOTE: triangle inside hull
  tri[0]=V2Assign(1,1); tri[1]=V2Assign(3,1); tri[2]=V2Assign(2,3);
  EXPECT_TRUE(ConvexHull2IntersectTri2(hull,4,tri,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: far away
  tri[0]=V2Assign(10,10);
  tri[1]=V2Assign(12,10);
  tri[2]=V2Assign(10,12);
  EXPECT_FALSE(ConvexHull2IntersectTri2(hull,4,tri,&m));
}

void ConvexHull2IntersectAabb2Test() {
  V2 hull[4] = {
    V2Assign(0,0), V2Assign(4,0),
    V2Assign(4,4), V2Assign(0,4)
  };

  V2 ac, as;
  IntersectManifold2 m;

  // NOTE: overlapping
  ac = V2Assign(3,3);
  as = V2Assign(4,4); // full size
  EXPECT_TRUE(ConvexHull2IntersectAabb2(hull,4,&ac,&as,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: separated
  ac = V2Assign(10,10);
  EXPECT_FALSE(ConvexHull2IntersectAabb2(hull,4,&ac,&as,&m));
}

void ConvexHull2IntersectObb2Test() {
  V2 hull[4] = {
    V2Assign(0,0), V2Assign(4,0),
    V2Assign(4,4), V2Assign(0,4)
  };

  V2 oc = V2Assign(3,3);
  V2 os = V2Assign(4,2); // full size
  F32 ang = F32_PI/4.0f;
  IntersectManifold2 m;

  EXPECT_TRUE(ConvexHull2IntersectObb2(hull,4,&oc,&os,ang,&m));
  EXPECT_TRUE(m.penetration > 0);

  oc = V2Assign(10,10);
  EXPECT_FALSE(ConvexHull2IntersectObb2(hull,4,&oc,&os,ang,&m));
}

void ConvexHull2IntersectCircle2Test() {
  V2 hull[4] = {
    V2Assign(0,0), V2Assign(4,0),
    V2Assign(4,4), V2Assign(0,4)
  };

  V2 cc; F32 r;
  IntersectManifold2 m;

  // NOTE: circle overlapping
  cc = V2Assign(3,2);
  r  = 2.0f;
  EXPECT_TRUE(ConvexHull2IntersectCircle2(hull,4,&cc,r,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: separate
  cc = V2Assign(10,10);
  EXPECT_FALSE(ConvexHull2IntersectCircle2(hull,4,&cc,r,&m));
}

void ConvexHull2IntersectConvexHull2Test() {
  V2 a[4], b[4];
  IntersectManifold2 m;

  // NOTE: no intersection
  a[0] = V2Assign(-1,-1); a[1] = V2Assign(1,-1); a[2] = V2Assign(1,1); a[3] = V2Assign(-1,1);
  b[0] = V2Assign(10,-1); b[1] = V2Assign(12,-1); b[2] = V2Assign(12,1); b[3] = V2Assign(10,1);
  EXPECT_FALSE(ConvexHull2IntersectConvexHull2(a,4,b,4,&m));

  // NOTE: touching at a single vertex
  a[0] = V2Assign(-1,-1); a[1] = V2Assign(1,-1); a[2] = V2Assign(1,1); a[3] = V2Assign(-1,1);
  b[0] = V2Assign(1,1);   b[1] = V2Assign(3,1);  b[2] = V2Assign(3,3); b[3] = V2Assign(1,3);
  EXPECT_TRUE(ConvexHull2IntersectConvexHull2(a,4,b,4,&m));
  EXPECT_TRUE(m.penetration >= 0);

  // NOTE: overlapping edges
  a[0] = V2Assign(-1,-1); a[1] = V2Assign(1,-1); a[2] = V2Assign(1,1); a[3] = V2Assign(-1,1);
  b[0] = V2Assign(0,-1); b[1] = V2Assign(2,-1); b[2] = V2Assign(2,1); b[3] = V2Assign(0,1);
  EXPECT_TRUE(ConvexHull2IntersectConvexHull2(a,4,b,4,&m));
  EXPECT_TRUE(m.penetration > 0);

  // NOTE: one hull inside the other
  a[0] = V2Assign(-2,-2); a[1] = V2Assign(2,-2); a[2] = V2Assign(2,2); a[3] = V2Assign(-2,2);
  b[0] = V2Assign(-0.5,-0.5); b[1] = V2Assign(0.5,-0.5); b[2] = V2Assign(0.5,0.5); b[3] = V2Assign(-0.5,0.5);
  EXPECT_TRUE(ConvexHull2IntersectConvexHull2(a,4,b,4,&m));
  EXPECT_TRUE(m.penetration > 0);
}

void Ray3ContainsPointTest() {
  V3 ray_start, ray_dir, point;

  ray_start = V3Assign(0, 0, 0); ray_dir = V3Assign(1, 0, 0);
  point = V3Assign(0, 0, 0);
  EXPECT_TRUE(Ray3ContainsPoint(&ray_start, &ray_dir, &point));
  point = V3Assign(10, 0, 0);
  EXPECT_TRUE(Ray3ContainsPoint(&ray_start, &ray_dir, &point));
  point = V3Assign(-10, 0, 0);
  EXPECT_FALSE(Ray3ContainsPoint(&ray_start, &ray_dir, &point));
}

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

void Ray3IntersectConvexHull3Test() {
  V3 ray_start, ray_dir;
  V3 enter, exit;

  V3  hull_points[8];
  U32 hull_indices[36];
  V3  hull_size = V3Assign(2, 2, 2);
  ConvexHull3FromAabb3(hull_points, hull_indices, &V3_ZEROES, &hull_size);

  // NOTE: ray hits hull front → exit at +x, enter at -x
  ray_start = V3Assign(-3, 0, 0); ray_dir = V3Assign(1, 0, 0);
  EXPECT_TRUE(Ray3IntersectConvexHull3(&ray_start, &ray_dir, hull_points, 8, hull_indices, 36, &enter, &exit));
  EXPECT_V3_APPROX_EQ(enter, V3Assign(-1, 0, 0));
  EXPECT_V3_APPROX_EQ(exit,  V3Assign( 1, 0, 0));

  // NOTE: ray misses (parallel, offset on Y)
  ray_start = V3Assign(-3, 3, 0); ray_dir = V3Assign(1, 0, 0);
  EXPECT_FALSE(Ray3IntersectConvexHull3(&ray_start, &ray_dir, hull_points, 8, hull_indices, 36, NULL, NULL));

  // NOTE: ray pointing away → miss (but hits when t < 0)
  ray_start = V3Assign(3, 0, 0); ray_dir = V3Assign(1, 0, 0);
  EXPECT_FALSE(Ray3IntersectConvexHull3(&ray_start, &ray_dir, hull_points, 8, hull_indices, 36, NULL, NULL));

  // NOTE: ray starts inside hull
  ray_start = V3Assign(0, 0, 0); ray_dir = V3Assign(0, 1, 0);
  EXPECT_TRUE(Ray3IntersectConvexHull3(&ray_start, &ray_dir, hull_points, 8, hull_indices, 36, &enter, &exit));
  EXPECT_V3_APPROX_EQ(enter, V3Assign(0, 1, 0));
  EXPECT_V3_APPROX_EQ(exit,  V3Assign(0, 1, 0));

  // NOTE: ray grazes face
  ray_start = V3Assign(-3, 1, -1); ray_dir = V3Assign(1, 0, 0);
  EXPECT_TRUE(Ray3IntersectConvexHull3(&ray_start, &ray_dir, hull_points, 8, hull_indices, 36, &enter, &exit));
  EXPECT_V3_APPROX_EQ(enter, V3Assign(-1, 1, -1));
  EXPECT_V3_APPROX_EQ(exit,  V3Assign(1, 1, -1));

  // NOTE: ray intersects infinite planes but misses actual hull region
  ray_start = V3Assign(0, 0, 3); ray_dir = V3Assign(0, 0, 1);
  EXPECT_FALSE(Ray3IntersectConvexHull3(&ray_start, &ray_dir, hull_points, 8, hull_indices, 36, NULL, NULL));
}

void Line3ContainsPointTest() {
  V3 start, end, point;
  start = V3Assign(0, 0, 0); end = V3Assign(10, 0, 0);
  point = V3Assign(0, 0, 0);
  EXPECT_TRUE(Line3ContainsPoint(&start, &end, &point));
  point = V3Assign(5, 0, 0);
  EXPECT_TRUE(Line3ContainsPoint(&start, &end, &point));
  point = V3Assign(15, 0, 0);
  EXPECT_FALSE(Line3ContainsPoint(&start, &end, &point));
  point = V3Assign(-5, 0, 0);
  EXPECT_FALSE(Line3ContainsPoint(&start, &end, &point));
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

void Plane3CreateTest() {
  V3 normal, anchor; F32 d;

  normal = V3Assign(0, 1, 0); anchor = V3Assign(0, 5, 0);
  Plane3FromAnchor(&normal, &d, &anchor);
  EXPECT_F32_APPROX_EQ(d, 5.0f);

  // NOTE: well-formed triangle
  V3 good_tri[3] = { V3Assign(0, 0, 0), V3Assign(1, 0, 0), V3Assign(0, 0, 1) };
  EXPECT_TRUE(Plane3FromTri3(&normal, &d, good_tri));
  EXPECT_V3_APPROX_EQ(normal, V3Assign(0, -1, 0));
  EXPECT_F32_APPROX_EQ(d, 0);

  // NOTE: degenerate triangle
  V3 bad_tri[3] = { V3Assign(0, 0, 0), V3Assign(0, 0, 0), V3Assign(0, 0, 0) };
  EXPECT_FALSE(Plane3FromTri3(&normal, &d, bad_tri));
}

void Plane3MutateTest() {
  V3 normal, offset; F32 d;

  normal = V3Assign(0, 1, 0); d = 5.0f;
  offset = V3Assign(0, -2, 0);
  Plane3Offset(&normal, &d, &offset);
  EXPECT_F32_APPROX_EQ(d, 7.0f);

  normal = V3Assign(0, 1, 0); d = 5.0f;
  Plane3Flip(&normal, &d);
  EXPECT_V3_APPROX_EQ(normal, V3Assign(0, -1, 0));
  EXPECT_F32_APPROX_EQ(d, -5);
}

void Plane3PointTest() {
  F32 d     = 0;
  V3 normal = V3Assign(0,  1, 0);
  V3 above  = V3Assign(0,  5, 0);
  V3 below  = V3Assign(0, -5, 0);
  V3 on     = V3Assign(0,  0, 0);

  EXPECT_TRUE (Plane3IsPointAbove(&normal, d, &above));
  EXPECT_FALSE(Plane3IsPointAbove(&normal, d, &below));
  EXPECT_FALSE(Plane3IsPointAbove(&normal, d, &on));

  EXPECT_FALSE(Plane3IsPointBelow(&normal, d, &above));
  EXPECT_TRUE (Plane3IsPointBelow(&normal, d, &below));
  EXPECT_FALSE(Plane3IsPointBelow(&normal, d, &on));

  EXPECT_TRUE(Plane3ContainsPoint(&normal, d, &on));
}

void Plane3IntersectLine3Test() {
  V3 plane_normal, line_start, line_end, intersect_point; F32 plane_d;

  // NOTE: simple intersection
  plane_normal = V3Assign(0, 1, 0); plane_d = 0;
  line_start = V3Assign(0, 5, 0); line_end = V3Assign(0, -5, 0);
  EXPECT_TRUE(Plane3IntersectLine3(&plane_normal, plane_d, &line_start, &line_end, &intersect_point));
  EXPECT_V3_APPROX_EQ(intersect_point, V3Assign(0, 0, 0));

  // NOTE: intersect @ endpoint
  plane_normal = V3Assign(0, 1, 0); plane_d = 2;
  line_start = V3Assign(0, 2, 0); line_end = V3Assign(0, 5, 0);
  EXPECT_TRUE(Plane3IntersectLine3(&plane_normal, plane_d, &line_start, &line_end, &intersect_point));
  EXPECT_V3_APPROX_EQ(intersect_point, V3Assign(0, 2, 0));

  // NOTE: segment above plane
  plane_normal = V3Assign(0, 1, 0); plane_d = 0;
  line_start = V3Assign(0, 5, 0); line_end = V3Assign(0, 1, 0);
  EXPECT_FALSE(Plane3IntersectLine3(&plane_normal, plane_d, &line_start, &line_end, &intersect_point));

  // NOTE: segment below plane
  plane_normal = V3Assign(0, 1, 0); plane_d = 0;
  line_start = V3Assign(0, -5, 0); line_end = V3Assign(0, -1, 0);
  EXPECT_FALSE(Plane3IntersectLine3(&plane_normal, plane_d, &line_start, &line_end, &intersect_point));

  // NOTE: segment parallel
  plane_normal = V3Assign(0, 0, 1); plane_d = 4;
  line_start = V3Assign(0, 0, 4); line_end = V3Assign(5, 5, 4);
  EXPECT_FALSE(Plane3IntersectLine3(&plane_normal, plane_d, &line_start, &line_end, &intersect_point));
}

void Plane3IntersectRay3Test() {
  V3  normal = V3Assign(0, 1, 0);
  F32 d      = 0.0f;

  V3 ray_start = V3Assign(0, 5, 0);
  V3 ray_dir   = V3Assign(0, -1, 0);
  V3 intersect_point;

  // NOTE: hit
  EXPECT_TRUE(Plane3IntersectRay3(&normal, d, &ray_start, &ray_dir, &intersect_point));
  EXPECT_V3_APPROX_EQ(intersect_point, V3Assign(0,0,0));

  // NOTE: pointing away from plane
  ray_dir = V3Assign(0,1,0);
  EXPECT_FALSE(Plane3IntersectRay3(&normal, d, &ray_start, &ray_dir, &intersect_point));

  // NOTE: parallel
  ray_dir = V3Assign(1,0,0);
  EXPECT_FALSE(Plane3IntersectRay3(&normal, d, &ray_start, &ray_dir, &intersect_point));
}

void Plane3IntersectPlane3Test() {
  V3 a_normal, b_normal; F32 a_d, b_d;
  V3 intersect_point, intersect_dir;

  // NOTE: orthogonal
  a_normal = V3Assign(0, 1, 0); a_d = 0;
  b_normal = V3Assign(1, 0, 0); b_d = 0;
  EXPECT_TRUE(Plane3IntersectPlane3(&a_normal, a_d, &b_normal, b_d, &intersect_point, &intersect_dir));
  EXPECT_V3_APPROX_EQ(intersect_point, V3Assign(0, 0, 0));
  EXPECT_V3_APPROX_EQ(intersect_dir, V3Assign(0, 0, -1));

  // NOTE: skewed
  a_normal = V3Assign(0, 1, 0); a_d = 2;
  b_normal = V3Assign(1, 0, 1);
  V3Normalize(&b_normal, &b_normal);
  b_d = 4.0f / V3Length(&b_normal);
  EXPECT_TRUE(Plane3IntersectPlane3(&a_normal, a_d, &b_normal, b_d, &intersect_point, &intersect_dir));
  V3 expected_dir; V3CrossV3(&expected_dir, &a_normal, &b_normal);
  EXPECT_V3_APPROX_EQ(intersect_dir, expected_dir);
  EXPECT_TRUE(Plane3ContainsPoint(&a_normal, a_d, &intersect_point));
  EXPECT_TRUE(Plane3ContainsPoint(&b_normal, b_d, &intersect_point));

  // NOTE: parallel
  a_normal = V3Assign(0, 1, 0); a_d = 1;
  b_normal = V3Assign(0, 1, 0); b_d = 5;
  EXPECT_FALSE(Plane3IntersectPlane3(&a_normal, a_d, &b_normal, b_d, &intersect_point, &intersect_dir));

  // NOTE: coincident
  a_normal = V3Assign(0, 1, 0); a_d = 1;
  b_normal = V3Assign(0, 1, 0); b_d = 1;
  EXPECT_FALSE(Plane3IntersectPlane3(&a_normal, a_d, &b_normal, b_d, &intersect_point, &intersect_dir));
}

void ConvexPolygon3IntersectPlane3Test() {
  V3 plane_normal; F32 plane_d;
  V3 polygon[4];
  V3 intersect_start, intersect_end;

  // NOTE: polygon above plane
  polygon[0] = V3Assign(1, 1, 1);
  polygon[1] = V3Assign(2, 1, 1);
  polygon[2] = V3Assign(2, 2, 1);
  polygon[3] = V3Assign(1, 2, 1);
  plane_normal = V3Assign(0, 0, 1); plane_d = 0.5f;
  EXPECT_FALSE(ConvexPolygon3IntersectPlane3(polygon, 4, &plane_normal, plane_d, &intersect_start, &intersect_end));

  // NOTE: polygon below plane
  polygon[0] = V3Assign(1, 1, 1);
  polygon[1] = V3Assign(2, 1, 1);
  polygon[2] = V3Assign(2, 2, 1);
  polygon[3] = V3Assign(1, 2, 1);
  plane_normal = V3Assign(0, 0, 1); plane_d = 2.0f;
  EXPECT_FALSE(ConvexPolygon3IntersectPlane3(polygon, 4, &plane_normal, plane_d, &intersect_start, &intersect_end));

  // NOTE: polygon crosses plane
  polygon[0] = V3Assign(1, 1, 1);
  polygon[1] = V3Assign(2, 1, 2);
  polygon[2] = V3Assign(2, 2, 2);
  polygon[3] = V3Assign(1, 2, 1);
  plane_normal = V3Assign(0, 0, 1); plane_d = 1.5f;
  EXPECT_TRUE(ConvexPolygon3IntersectPlane3(polygon, 4, &plane_normal, plane_d, &intersect_start, &intersect_end));
  EXPECT_V3_APPROX_EQ(intersect_start, V3Assign(1.5f, 1, 1.5f));
  EXPECT_V3_APPROX_EQ(intersect_end,   V3Assign(1.5f, 2, 1.5f));

  // NOTE: single point intersection
  polygon[0] = V3Assign(0, 0, 1);
  polygon[1] = V3Assign(1, 0, 1);
  polygon[2] = V3Assign(1, 1, 1);
  polygon[3] = V3Assign(0, 1, 2);
  plane_normal = V3Assign(0, 0, 1); plane_d = 2.0f;
  EXPECT_TRUE(ConvexPolygon3IntersectPlane3(polygon, 4, &plane_normal, plane_d, &intersect_start, &intersect_end));
  EXPECT_V3_APPROX_EQ(intersect_start, V3Assign(0, 1, 2));
  EXPECT_V3_APPROX_EQ(intersect_end,   V3Assign(0, 1, 2));
}

void ConvexPolygon3IntersectConvexPolygon3Test() {
  V3 intersect_start, intersect_end;

  // NOTE: non-intersecting
  V3 polyA1[3] = { V3Assign(0, 0, 0), V3Assign(1, 0, 0), V3Assign(0, 1, 0) };
  V3 polyB1[3] = { V3Assign(0, 0, 2), V3Assign(1, 0, 2), V3Assign(0, 1, 2) };
  EXPECT_FALSE(ConvexPolygon3IntersectConvexPolygon3(polyA1, 3, polyB1, 3, &intersect_start, &intersect_end));

  // NOTE: sharing an edge
  V3 polyA2[3] = { V3Assign(0, 0, 0), V3Assign(1, 0, 0), V3Assign(0, 1, 0) };
  V3 polyB2[3] = { V3Assign(0, 0, 0), V3Assign(0, 0, 1), V3Assign(0, 1, 0) };
  EXPECT_TRUE(ConvexPolygon3IntersectConvexPolygon3(polyA2, 3, polyB2, 3, &intersect_start, &intersect_end));
  EXPECT_V3_APPROX_EQ(intersect_start, V3Assign(0, 1, 0));
  EXPECT_V3_APPROX_EQ(intersect_end,   V3Assign(0, 0, 0));

  // NOTE: skew
  V3 polyA3[4] = { V3Assign(0, 0, 0), V3Assign(1, 0, 0), V3Assign(1, 1, 0), V3Assign(0, 1, 0) };
  V3 polyB3[4] = { V3Assign(0.25f, 0.25f, -1), V3Assign(0.75f, 0.75f, -1), V3Assign(0.75f, 0.75f, 1), V3Assign(0.25f, 0.25f, 1) };
  EXPECT_TRUE(ConvexPolygon3IntersectConvexPolygon3(polyA3, 4, polyB3, 4, &intersect_start, &intersect_end));
  EXPECT_V3_APPROX_EQ(intersect_start, V3Assign(0.25f, 0.25f, 0));
  EXPECT_V3_APPROX_EQ(intersect_end,   V3Assign(0.75f, 0.75f, 0));
}

void Aabb3IntersectAabb3Test() {
  V3 ca, sa, cb, sb;
  IntersectManifold3 m;

  // NOTE: no intersection
  ca = V3Assign(0,0,0);  sa = V3Assign(4,4,4);
  cb = V3Assign(10,0,0); sb = V3Assign(4,4,4);
  EXPECT_FALSE(Aabb3IntersectAabb3(&ca,&sa,&cb,&sb,&m));

  // NOTE: simple centered overlap on X axis
  ca = V3Assign(0,0,0); sa = V3Assign(4,4,4);
  cb = V3Assign(3,0,0); sb = V3Assign(4,4,4);
  EXPECT_TRUE(Aabb3IntersectAabb3(&ca,&sa,&cb,&sb,&m));
  EXPECT_F32_APPROX_EQ(m.penetration, 1.0f);
  EXPECT_V3_APPROX_EQ(m.normal, V3Assign(1,0,0));

  // NOTE: overlap on Y axis (A below B)
  ca = V3Assign(0,0,0);
  cb = V3Assign(0,1,0);
  EXPECT_TRUE(Aabb3IntersectAabb3(&ca,&sa,&cb,&sb,&m));
  EXPECT_F32_APPROX_EQ(m.penetration, 3.0f);
  EXPECT_V3_APPROX_EQ(m.normal, V3Assign(0,1,0));

  // NOTE: overlap on Z axis (A in front of B)
  ca = V3Assign(0,0,0);
  cb = V3Assign(0,0,1);
  EXPECT_TRUE(Aabb3IntersectAabb3(&ca,&sa,&cb,&sb,&m));
  EXPECT_F32_APPROX_EQ(m.penetration, 3.0f);
  EXPECT_V3_APPROX_EQ(m.normal, V3Assign(0,0,1));

  // NOTE: diagonal overlap → smallest axis is X
  ca = V3Assign(0,0,0); cb = V3Assign(1.5f, 1.0f, 0.5f);
  EXPECT_TRUE(Aabb3IntersectAabb3(&ca,&sa,&cb,&sb,&m));
  EXPECT_F32_APPROX_EQ(m.penetration, 2.5f);
  EXPECT_V3_APPROX_EQ(m.normal, V3Assign(1,0,0));

  // NOTE: A fully inside B (all overlaps positive)
  ca = V3Assign(0,0,0); sa = V3Assign(2,2,2);
  cb = V3Assign(0,0,0); sb = V3Assign(10,10,10);
  EXPECT_TRUE(Aabb3IntersectAabb3(&ca,&sa,&cb,&sb,&m));
  EXPECT_F32_APPROX_EQ(m.penetration, 6.0f);
  EXPECT_V3_APPROX_EQ(m.normal, V3Assign(1,0,0));
}

void Aabb3IntersectSphere3Test() {
  V3 rc, rs, sc;
  F32 sr;
  IntersectManifold3 m;

  // NOTE: miss
  rc = V3Assign(0, 0, 0); rs = V3Assign(2, 2, 2);
  sc = V3Assign(0, 0, 5); sr = 1;
  EXPECT_FALSE(Aabb3IntersectSphere3(&rc, &rs, &sc, sr, &m));

  // NOTE: hit
  rc = V3Assign(0, 0, 0); rs = V3Assign(2, 2, 2);
  sc = V3Assign(0, 0, 1); sr = 1;
  EXPECT_TRUE(Aabb3IntersectSphere3(&rc, &rs, &sc, sr, &m));
  // NOTE: not V3_Z_NEG because of EPA imprecision.
  EXPECT_V3_APPROX_EQ(m.normal, V3Assign(-0.22239f, 0.10352f, -0.96945f));
  EXPECT_F32_APPROX_EQ(m.penetration, 0.85058f);
}

void Aabb3IntersectConvexHull3Test() {
  V3 ac, as;
  V3 hull[8];
  IntersectManifold3 manifold;

  hull[0] = V3Assign(-1, -1, -1); hull[1] = V3Assign(1, -1, -1);
  hull[2] = V3Assign(1, 1, -1);   hull[3] = V3Assign(-1, 1, -1);
  hull[4] = V3Assign(-1, -1, 1);  hull[5] = V3Assign(1, -1, 1);
  hull[6] = V3Assign(1, 1, 1);    hull[7] = V3Assign(-1, 1, 1);

  // NOTE: no intersection
  ac = V3Assign(0, 0, 5);  as = V3Assign(1, 1, 1);
  EXPECT_FALSE(Aabb3IntersectConvexHull3(&ac, &as, hull, 8, &manifold));

  // NOTE: deep intersection
  ac = V3Assign(0, 0, 0.75f);  as = V3Assign(1, 1, 1);
  EXPECT_TRUE(Aabb3IntersectConvexHull3(&ac, &as, hull, 8, &manifold));
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0, 0, 1));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 0.75f);

  // NOTE: touching face
  ac = V3Assign(0, 0, 2);  as = V3Assign(2, 2, 2);
  EXPECT_TRUE(Aabb3IntersectConvexHull3(&ac, &as, hull, 8, &manifold));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 0.0f);
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0, 0, 1));

  // NOTE: a entirely in b
  ac = V3Assign(0, 0, 0);  as = V3Assign(1, 1, 1);
  EXPECT_TRUE(Aabb3IntersectConvexHull3(&ac, &as, hull, 8, &manifold));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 1.11803f);
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(-0.89443f, -0.00000f, 0.44721f));
}

void Sphere3IntersectLine3Test() {
  V3 a, b, enter, exit;

  V3 c = V3Assign(0, 0, 0);
  F32 rad = 2.0f;

  // NOTE: simple centered hit (line passes fully through sphere)
  a = V3Assign(-5, 0, 0); b = V3Assign(5, 0, 0);
  EXPECT_TRUE(Sphere3IntersectLine3(&c, rad, &a, &b, &enter, &exit));
  EXPECT_V3_APPROX_EQ(enter, V3Assign(-2, 0, 0));
  EXPECT_V3_APPROX_EQ(exit,  V3Assign( 2, 0, 0));

  // NOTE: offset hit (skimming through the sphere)
  a = V3Assign(-5, 1, 0); b = V3Assign(5, 1, 0);
  EXPECT_TRUE(Sphere3IntersectLine3(&c, rad, &a, &b, &enter, &exit));
  EXPECT_TRUE(enter.x < exit.x);
  EXPECT_F32_APPROX_EQ(enter.y, 1.0f);
  EXPECT_F32_APPROX_EQ(exit.y,  1.0f);

  // NOTE: tangential hit (line just grazes the sphere)
  a = V3Assign(-5, 2, 0); b = V3Assign(5, 2, 0);
  EXPECT_TRUE(Sphere3IntersectLine3(&c, rad, &a, &b, &enter, &exit));
  EXPECT_V3_APPROX_EQ(enter, exit);
  EXPECT_V3_APPROX_EQ(enter, V3Assign(0, 2, 0));

  // NOTE: line misses sphere entirely
  a = V3Assign(-5, 3, 0); b = V3Assign(5, 3, 0);
  EXPECT_FALSE(Sphere3IntersectLine3(&c, rad, &a, &b, &enter, &exit));

  // NOTE: line starts inside sphere and exits
  a = V3Assign(0.5f, 0, 0); b = V3Assign(5, 0, 0);
  EXPECT_TRUE(Sphere3IntersectLine3(&c, rad, &a, &b, &enter, &exit));
  EXPECT_V3_APPROX_EQ(enter, V3Assign(2.0f, 0, 0));
  EXPECT_V3_APPROX_EQ(exit,  V3Assign(2.0f, 0, 0));

  // NOTE: line ends inside sphere (enter outside, exit inside)
  a = V3Assign(-5, 0, 0); b = V3Assign(1, 0, 0);
  EXPECT_TRUE(Sphere3IntersectLine3(&c, rad, &a, &b, &enter, &exit));
  EXPECT_V3_APPROX_EQ(enter, V3Assign(-2, 0, 0));
  EXPECT_V3_APPROX_EQ(exit,  V3Assign(-2, 0, 0));
}

void Sphere3IntersectRay3Test() {
  V3 r0, d, enter, exit;

  V3 c = V3Assign(0, 0, 0);
  F32 rad = 2.0f;

  // NOTE: simple centered hit (ray aimed directly at sphere)
  r0 = V3Assign(-5, 0, 0); d  = V3Assign(1, 0, 0);
  EXPECT_TRUE(Sphere3IntersectRay3(&c, rad, &r0, &d, &enter, &exit));
  EXPECT_V3_APPROX_EQ(enter, V3Assign(-2, 0, 0));
  EXPECT_V3_APPROX_EQ(exit,  V3Assign( 2, 0, 0));

  // NOTE: offset hit (skimming through the sphere)
  r0 = V3Assign(-5, 1, 0); d  = V3Assign(1, 0, 0);
  EXPECT_TRUE(Sphere3IntersectRay3(&c, rad, &r0, &d, &enter, &exit));
  EXPECT_TRUE(enter.x < exit.x);
  EXPECT_F32_APPROX_EQ(enter.y, 1.0f);
  EXPECT_F32_APPROX_EQ(exit.y,  1.0f);

  // NOTE: tangential hit (ray just grazes the sphere)
  r0 = V3Assign(-5, 2, 0); d  = V3Assign(1, 0, 0);
  EXPECT_TRUE(Sphere3IntersectRay3(&c, rad, &r0, &d, &enter, &exit));
  EXPECT_V3_APPROX_EQ(enter, exit);  // tangent => same point
  EXPECT_V3_APPROX_EQ(enter, V3Assign(0, 2, 0));

  // NOTE: ray misses sphere entirely
  r0 = V3Assign(-5, 3, 0); d  = V3Assign(1, 0, 0);
  EXPECT_FALSE(Sphere3IntersectRay3(&c, rad, &r0, &d, &enter, &exit));

  // NOTE: ray starts inside sphere
  r0 = V3Assign(0.5f, 0, 0); d  = V3Assign(1, 0, 0);
  EXPECT_TRUE(Sphere3IntersectRay3(&c, rad, &r0, &d, &enter, &exit));
  EXPECT_V3_APPROX_EQ(enter, V3Assign(2.0f, 0, 0));
  EXPECT_V3_APPROX_EQ(exit,  V3Assign(2.0f, 0, 0));

  // NOTE: ray points away from sphere
  r0 = V3Assign(5, 0, 0); d  = V3Assign(1, 0, 0);
  EXPECT_FALSE(Sphere3IntersectRay3(&c, rad, &r0, &d, &enter, &exit));
}

void ConvexHull3FlattenTest() {
  V3 vs[] = {
    // apex
    {0.0f, 0.0f, 1.0f},
    // base
    {-1.0f, -1.0f, 0.0f},
    { 1.0f, -1.0f, 0.0f},
    { 1.0f,  1.0f, 0.0f},
    {-1.0f,  1.0f, 0.0f},
  };
  V3 pyramid_faces[] = {
      // sides
      vs[0], vs[1], vs[2],
      vs[0], vs[2], vs[3],
      vs[0], vs[3], vs[4],
      vs[0], vs[4], vs[1],
      // base
      vs[1], vs[2], vs[3],
      vs[1], vs[3], vs[4],
  };

  Arena* arena = ArenaAllocate();
  V3* points; U32 points_size;
  ConvexHull3Flatten(arena, pyramid_faces, STATIC_ARRAY_SIZE(pyramid_faces), &points, &points_size);
  EXPECT_U32_EQ(points_size, 5);
  EXPECT_V3_EQ(points[0], vs[0]);
  EXPECT_V3_EQ(points[1], vs[1]);
  EXPECT_V3_EQ(points[2], vs[2]);
  EXPECT_V3_EQ(points[3], vs[3]);
  EXPECT_V3_EQ(points[4], vs[4]);
  ArenaRelease(arena);
}

void ConvexHull3ContainsPointTest() {
  V3  hull_points[8], point;
  U32 hull_indices[36];
  V3 hull_size = V3Assign(2, 2, 2);
  ConvexHull3FromAabb3(hull_points, hull_indices, &(V3){0, 0, 0}, &hull_size);

  // NOTE: contains point
  point = V3Assign(0, 0, 0);
  EXPECT_TRUE(ConvexHull3ContainsPoint(hull_points, 8, hull_indices, 36, &point));

  // NOTE: point on face
  point = V3Assign(1, 0, 0);
  EXPECT_TRUE(ConvexHull3ContainsPoint(hull_points, 8, hull_indices, 36, &point));

  // NOTE: point on corner
  point = V3Assign(1, 1, 1);
  EXPECT_TRUE(ConvexHull3ContainsPoint(hull_points, 8, hull_indices, 36, &point));

  // NOTE: does not contain point
  point = V3Assign(100, 0, 0);
  EXPECT_FALSE(ConvexHull3ContainsPoint(hull_points, 8, hull_indices, 36, &point));
}

void ConvexHull3IntersectConvexHull3Test() {
  V3 a[8], b[8];
  V3 size = V3Assign(2, 2, 2);
  IntersectManifold3 manifold;

  // NOTE: not intersecting
  ConvexHull3FromAabb3(a, NULL, &(V3){0, 0, 0}, &size);
  ConvexHull3FromAabb3(b, NULL, &(V3){0, 0, 4}, &size);
  EXPECT_FALSE(ConvexHull3IntersectConvexHull3(a, 8, b, 8, &manifold));

  // NOTE: deep intersection
  ConvexHull3FromAabb3(a, NULL, &(V3){0, 0, 0}, &size);
  ConvexHull3FromAabb3(b, NULL, &(V3){0, 0, 1}, &size);
  EXPECT_TRUE(ConvexHull3IntersectConvexHull3(a, 8, b, 8, &manifold));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 1);
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0, 0, -1));

  // NOTE: intersecting one face
  ConvexHull3FromAabb3(a, NULL, &(V3){0, 0, 0}, &size);
  ConvexHull3FromAabb3(b, NULL, &(V3){0, 0, 2}, &size);
  EXPECT_TRUE(ConvexHull3IntersectConvexHull3(a, 8, b, 8, &manifold));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 0);
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0, 0, -1));

  // NOTE: intersecting one corner
  ConvexHull3FromAabb3(a, NULL, &(V3){0, 0, 0}, &size);
  ConvexHull3FromAabb3(b, NULL, &(V3){2, 2, 2}, &size);
  EXPECT_TRUE(ConvexHull3IntersectConvexHull3(a, 8, b, 8, &manifold));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 0);
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0, -1, 0));
}

void ConvexHull3IntersectSphere3Test() {
  V3 hull[8], sc;
  F32 r;
  ConvexHull3FromAabb3(hull, NULL, &V3_ZEROES, &V3_ONES);
  IntersectManifold3 manifold;

  // NOTE: miss
  sc = V3Assign(0, 0, 5); r = 1;
  EXPECT_FALSE(ConvexHull3IntersectSphere3(hull, 8, &sc, r, &manifold));

  // NOTE: intersection
  sc = V3Assign(0, 0, 1); r = 1;
  EXPECT_TRUE(ConvexHull3IntersectSphere3(hull, 8, &sc, r, &manifold));
  // NOTE: should be V3_Z_NEG, but not, due to EPA convergence memes.
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0.18434f, -0.31337f, -0.93157f));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 0.40127f);

  // NOTE: face intersection
  sc = V3Assign(0, 0, 1.5f); r = 1;
  EXPECT_TRUE(ConvexHull3IntersectSphere3(hull, 8, &sc, r, &manifold));
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0, 0, -1));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 0);

  // NOTE: sphere enclosed in hull
  sc = V3Assign(0, 0, 0); r = 0.25f;
  EXPECT_TRUE(ConvexHull3IntersectSphere3(hull, 8, &sc, r, &manifold));
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0.15726f, -0.10484f, 0.98198f));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 0.49099f);
}

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());

  RUN_TEST(Line2EqTest);
  RUN_TEST(Line2MutateTest);
  RUN_TEST(Line2QueryTest);
  RUN_TEST(Line2IntersectLine2Test);
  RUN_TEST(Line2IntersectRay2Test);
  RUN_TEST(Line2IntersectTri2Test);
  RUN_TEST(Line2IntersectAabb2Test);
  RUN_TEST(Line2IntersectCircle2Test);
  RUN_TEST(Line2IntersectConvexHull2Test);

  RUN_TEST(Ray2EqTest);
  RUN_TEST(Ray2RotateAboutPointTest);
  RUN_TEST(Ray2ContainsPointTest);
  RUN_TEST(Ray2IntersectLine2Test);
  RUN_TEST(Ray2IntersectRay2Test);
  RUN_TEST(Ray2IntersectTri2Test);
  RUN_TEST(Ray2IntersectAabb2Test);
  RUN_TEST(Ray2IntersectObb2Test);
  RUN_TEST(Ray2IntersectCircle2Test);
  RUN_TEST(Ray2IntersectConvexHull2Test);

  RUN_TEST(Tri2EqTest);
  RUN_TEST(Tri2MutateTest);
  RUN_TEST(Tri2QueryTest);
  RUN_TEST(Tri2IntersectLine2Test);
  RUN_TEST(Tri2IntersectRay2Test);
  RUN_TEST(Tri2IntersectTri2Test);
  RUN_TEST(Tri2IntersectAabb2Test);
  RUN_TEST(Tri2IntersectObb2Test);
  RUN_TEST(Tri2IntersectCircle2Test);
  RUN_TEST(Tri2IntersectConvexHull2Test);

  RUN_TEST(Aabb2EqTest);
  RUN_TEST(Aabb2ConstructionTest);
  RUN_TEST(Aabb2QueryTest);
  RUN_TEST(Aabb2RotateAboutPointTest);
  RUN_TEST(Aabb2IntersectLine2Test);
  RUN_TEST(Aabb2IntersectRay2Test);
  RUN_TEST(Aabb2IntersectTri2Test);
  RUN_TEST(Aabb2IntersectAabb2Test);
  RUN_TEST(Aabb2IntersectObb2Test);
  RUN_TEST(Aabb2IntersectCircle2Test);
  RUN_TEST(Aabb2IntersectConvexHull2Test);

  RUN_TEST(Obb2EqTest);
  RUN_TEST(Obb2RotateAboutPointTest);
  RUN_TEST(Obb2GetEnclosingCircle2Test);
  RUN_TEST(Obb2GetEnclosingAabb2Test);
  RUN_TEST(Obb2ContainsPointTest);
  RUN_TEST(Obb2IntersectLine2Test);
  RUN_TEST(Obb2IntersectRay2Test);
  RUN_TEST(Obb2IntersectTri2Test);
  RUN_TEST(Obb2IntersectAabb2Test);
  RUN_TEST(Obb2IntersectObb2Test);
  RUN_TEST(Obb2IntersectCircle2Test);
  RUN_TEST(Obb2IntersectConvexHull2Test);

  RUN_TEST(Circle2EqTest);
  RUN_TEST(Circle2GetEnclosingAabb2Test);
  RUN_TEST(Circle2ContainsPointTest);
  RUN_TEST(Circle2IntersectLine2Test);
  RUN_TEST(Circle2IntersectRay2Test);
  RUN_TEST(Circle2IntersectTri2Test);
  RUN_TEST(Circle2IntersectAabb2Test);
  RUN_TEST(Circle2IntersectObb2Test);
  RUN_TEST(Circle2IntersectCircle2Test);
  RUN_TEST(Circle2IntersectConvexHull2Test);

  RUN_TEST(ConvexHull2EqTest);
  RUN_TEST(ConvexHull2FromAabb2Test);
  RUN_TEST(ConvexHull2FromObb2Test);
  RUN_TEST(ConvexHull2OffsetTest);
  RUN_TEST(ConvexHull2RotateAboutPointTest);
  RUN_TEST(ConvexHull2CenterTest);
  RUN_TEST(ConvexHull2GetEnclosingCircle2Test);
  RUN_TEST(ConvexHull2GetEnclosingAabb2Test);
  RUN_TEST(ConvexHull2ContainsPointTest);
  RUN_TEST(ConvexHull2IntersectLine2Test);
  RUN_TEST(ConvexHull2IntersectRay2Test);
  RUN_TEST(ConvexHull2IntersectTri2Test);
  RUN_TEST(ConvexHull2IntersectAabb2Test);
  RUN_TEST(ConvexHull2IntersectObb2Test);
  RUN_TEST(ConvexHull2IntersectCircle2Test);
  RUN_TEST(ConvexHull2IntersectConvexHull2Test);

  RUN_TEST(Line3ContainsPointTest);
  RUN_TEST(Line3IntersectLine3Test);
  RUN_TEST(Line3IntersectRay3Test);

  RUN_TEST(Ray3ContainsPointTest);
  RUN_TEST(Ray3IntersectLine3Test);
  RUN_TEST(Ray3IntersectRay3Test);
  RUN_TEST(Ray3IntersectConvexHull3Test);

  RUN_TEST(Plane3CreateTest);
  RUN_TEST(Plane3MutateTest);
  RUN_TEST(Plane3PointTest);
  RUN_TEST(Plane3IntersectLine3Test);
  RUN_TEST(Plane3IntersectRay3Test);
  RUN_TEST(Plane3IntersectPlane3Test);

  RUN_TEST(ConvexPolygon3IntersectPlane3Test);
  RUN_TEST(ConvexPolygon3IntersectConvexPolygon3Test);

  RUN_TEST(Aabb3IntersectAabb3Test);
  RUN_TEST(Aabb3IntersectSphere3Test);
  RUN_TEST(Aabb3IntersectConvexHull3Test);

  RUN_TEST(Sphere3IntersectLine3Test);
  RUN_TEST(Sphere3IntersectRay3Test);

  RUN_TEST(ConvexHull3FlattenTest);
  RUN_TEST(ConvexHull3ContainsPointTest);
  RUN_TEST(ConvexHull3IntersectConvexHull3Test);
  RUN_TEST(ConvexHull3IntersectSphere3Test);

  LogTestReport();
  return 0;
}
