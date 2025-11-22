#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

// TODO: finish 2d unit tests
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

static void MakeCube(V3 cube[8], V3 center, F32 half_extent) {
  cube[0] = V3Assign(center.x - half_extent, center.y - half_extent, center.z - half_extent);
  cube[1] = V3Assign(center.x + half_extent, center.y - half_extent, center.z - half_extent);
  cube[2] = V3Assign(center.x + half_extent, center.y + half_extent, center.z - half_extent);
  cube[3] = V3Assign(center.x - half_extent, center.y + half_extent, center.z - half_extent);
  cube[4] = V3Assign(center.x - half_extent, center.y - half_extent, center.z + half_extent);
  cube[5] = V3Assign(center.x + half_extent, center.y - half_extent, center.z + half_extent);
  cube[6] = V3Assign(center.x + half_extent, center.y + half_extent, center.z + half_extent);
  cube[7] = V3Assign(center.x - half_extent, center.y + half_extent, center.z + half_extent);
}

void ConvexHull3IntersectConvexHull3Test() {
  V3 a[8], b[8];
  IntersectManifold3 manifold;

  // NOTE: not intersecting
  MakeCube(a, V3Assign(0, 0, 0), 1);
  MakeCube(b, V3Assign(0, 0, 4), 1);
  EXPECT_FALSE(ConvexHull3IntersectConvexHull3(a, 8, b, 8, &manifold));

  // NOTE: deep intersection
  MakeCube(a, V3Assign(0, 0, 0), 1);
  MakeCube(b, V3Assign(0, 0, 1), 1);
  EXPECT_TRUE(ConvexHull3IntersectConvexHull3(a, 8, b, 8, &manifold));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 1);
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0, 0, 1));

  // NOTE: intersecting one face
  MakeCube(a, V3Assign(0, 0, 0), 1);
  MakeCube(b, V3Assign(0, 0, 2), 1);
  EXPECT_TRUE(ConvexHull3IntersectConvexHull3(a, 8, b, 8, &manifold));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 0);
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0, 0, 1));

  // NOTE: intersecting one corner
  MakeCube(a, V3Assign(0, 0, 0), 1);
  MakeCube(b, V3Assign(2, 2, 2), 1);
  EXPECT_TRUE(ConvexHull3IntersectConvexHull3(a, 8, b, 8, &manifold));
  EXPECT_F32_APPROX_EQ(manifold.penetration, 0);
  EXPECT_V3_APPROX_EQ(manifold.normal, V3Assign(0, 0, 1));
}

int main(void) {
  RUN_TEST(Line2EqTest);
  RUN_TEST(Line2MutateTest);
  RUN_TEST(Line2QueryTest);
  RUN_TEST(Line2IntersectLine2Test);
  RUN_TEST(Line2IntersectRay2Test);
  RUN_TEST(Line2IntersectTri2Test);
  RUN_TEST(Line2IntersectAabb2Test);
  RUN_TEST(Line2IntersectCircle2Test);
  RUN_TEST(Line2IntersectConvexHull2Test);
  RUN_TEST(Ray3IntersectLine3Test);
  RUN_TEST(Ray3IntersectRay3Test);
  RUN_TEST(Line3IntersectLine3Test);
  RUN_TEST(Line3IntersectRay3Test);
  RUN_TEST(Plane3CreateTest);
  RUN_TEST(Plane3MutateTest);
  RUN_TEST(Plane3PointTest);
  RUN_TEST(Plane3IntersectLine3Test);
  RUN_TEST(Plane3IntersectRay3Test);
  RUN_TEST(Plane3IntersectPlane3Test);
  RUN_TEST(ConvexPolygon3IntersectPlane3Test);
  RUN_TEST(ConvexPolygon3IntersectConvexPolygon3Test);
  RUN_TEST(ConvexHull3IntersectConvexHull3Test);
  LogTestReport();
  return 0;
}
