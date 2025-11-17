#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

// TODO: 2d unit tests
// TODO: ray3 attribute tests
// TODO: line3 attribute tests

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

int main(void) {
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
  LogTestReport();
  return 0;
}
