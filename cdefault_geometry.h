#ifndef CDEFAULT_GEOMETRY_H_
#define CDEFAULT_GEOMETRY_H_

#include "cdefault_math.h"
#include "cdefault_std.h"

// TODO: capsule?
// TODO: 3d match 2d api
// TODO: 3d api largely untested, likely doesn't work
// TODO: break concave hull into convex hull

// NOTE: Collections of points (e.g. Tri2, ConvexHull2) must follow CCW winding order.
// NOTE: For intersection routines, may pass NULL. Intersection manifolds provide information
// for discrete collision resolution assuming that whatever the shape is colliding with is static.

////////////////////////////////////////////////////////////////////////////////
// 2D shapes
////////////////////////////////////////////////////////////////////////////////

typedef struct IntersectManifold2 IntersectManifold2;
struct IntersectManifold2 {
  V2  normal;
  F32 penetration;
  V2  contact_points[2];
  U32 contact_points_size;
};

B32  Line2Eq(V2* a_start, V2* a_end, V2* b_start, V2* b_end);
B32  Line2ApproxEq(V2* a_start, V2* a_end, V2* b_start, V2* b_end);
void Line2Offset(V2* start, V2* end, V2* offset);
void Line2RotateAboutPoint(V2* start, V2* end, V2* point, F32 angle_rad);
F32  Line2GetLength(V2* start, V2* end);
F32  Line2GetLengthSq(V2* start, V2* end);
void Line2GetMidpoint(V2* line_start, V2* line_end, V2* midpoint);
void Line2GetClosestPoint(V2* start, V2* end, V2* point, V2* closest);
void Line2GetNormalIn(V2* start, V2* end, V2* normal);  // NOTE: points into CCW center. TODO: V2 fn instead?
void Line2GetNormalOut(V2* start, V2* end, V2* normal); // NOTE: points away from CCW center. TODO: V2 fn instead?
B32  Line2IntersectLine2(V2* a_start, V2* a_end, V2* b_start, V2* b_end, V2* intersect_point);
B32  Line2IntersectRay2(V2* line_start, V2* line_end, V2* ray_start, V2* ray_dir, V2* intersect_point);
B32  Line2IntersectTri2(V2* line_start, V2* line_end, V2 tri_points[3], V2* enter_point, V2* exit_point);
B32  Line2IntersectAabb2(V2* line_start, V2* line_end, V2* aabb_center, V2* aabb_size, V2* enter_point, V2* exit_point);
B32  Line2IntersectObb2(V2* line_start, V2* line_end, V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* enter_point, V2* exit_point);
B32  Line2IntersectCircle2(V2* line_start, V2* line_end, V2* circle_center, F32 circle_radius, V2* enter_point, V2* exit_point);
B32  Line2IntersectConvexHull2(V2* line_start, V2* line_end, V2* hull_points, U32 hull_points_size, V2* enter_point, V2* exit_point);

B32  Ray2Eq(V2* a_start, V2* a_dir, V2* b_start, V2* b_dir);
B32  Ray2ApproxEq(V2* a_start, V2* a_dir, V2* b_start, V2* b_dir);
void Ray2RotateAboutPoint(V2* ray_start, V2* ray_dir, V2* point, F32 angle_rad);
void Ray2GetDirInv(V2* ray_dir, V2* dir_inv);
B32  Ray2IntersectLine2(V2* ray_start, V2* ray_dir, V2* line_start, V2* line_end, V2* intersect_point);
B32  Ray2IntersectRay2(V2* a_start, V2* a_dir, V2* b_start, V2* b_dir, V2* intersect_point);
B32  Ray2IntersectTri2(V2* ray_start, V2* ray_dir, V2 tri_points[3], V2* enter_point, V2* exit_point);
B32  Ray2IntersectAabb2(V2* ray_start, V2* ray_dir, V2* aabb_center, V2* aabb_size, V2* enter_point, V2* exit_point);
B32  Ray2IntersectObb2(V2* ray_start, V2* ray_dir, V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* enter_point, V2* exit_point);
B32  Ray2IntersectCircle2(V2* ray_start, V2* ray_dir, V2* circle_center, F32 circle_radius, V2* enter_point, V2* exit_point);
B32  Ray2IntersectConvexHull2(V2* ray_start, V2* ray_dir, V2* hull_points, U32 hull_points_size, V2* enter_point, V2* exit_point);

B32  Tri2Eq(V2 a_points[3], V2 b_points[3]);
B32  Tri2ApproxEq(V2 a_points[3], V2 b_points[3]);
void Tri2Offset(V2 tri_points[3], V2* offset);
void Tri2RotateAboutPoint(V2 tri_points[3], V2* point, F32 angle_rad);
void Tri2SetCenter(V2 tri_points[3], V2* center);
void Tri2GetCenter(V2 tri_points[3], V2* center);
void Tri2GetEnclosingCircle2(V2 tri_points[3], V2* circle_center, F32* circle_radius);
void Tri2GetEnclosingAabb2(V2 tri_points[3], V2* aabb_center, V2* aabb_size);
B32  Tri2ContainsPoint(V2 tri_points[3], V2* point);
B32  Tri2IntersectLine2(V2 tri_points[3], V2* line_start, V2* line_end, V2* enter_point, V2* exit_point);
B32  Tri2IntersectRay2(V2 tri_points[3], V2* ray_start, V2* ray_dir, V2* enter_point, V2* exit_point);
B32  Tri2IntersectTri2(V2 a_points[3], V2 b_points[3], IntersectManifold2* manifold);
B32  Tri2IntersectAabb2(V2 tri_points[3], V2* aabb_center, V2* aabb_size, IntersectManifold2* manifold);
B32  Tri2IntersectObb2(V2 tri_points[3], V2* obb_center, V2* obb_size, F32 obb_angle_rad, IntersectManifold2* manifold);
B32  Tri2IntersectCircle2(V2 tri_points[3], V2* circle_center, F32 circle_radius, IntersectManifold2* manifold);
B32  Tri2IntersectConvexHull2(V2 tri_points[3], V2* hull_points, U32 hull_points_size, IntersectManifold2* manifold);

B32  Aabb2Eq(V2* a_center, V2* a_size, V2* b_center, V2* b_size);
B32  Aabb2ApproxEq(V2* a_center, V2* a_size, V2* b_center, V2* b_size);
void Aabb2FromMinMax(V2* aabb_center, V2* aabb_size, V2* min, V2* max);
void Aabb2FromTopLeft(V2* aabb_center, V2* top_left_point, V2* size);
void Aabb2GetMinMax(V2* aabb_center, V2* aabb_size, V2* min, V2* max);
void Aabb2GetEnclosingCircle2(V2* aabb_center, V2* aabb_size, F32* circle_radius);
void Aabb2RotateAboutPoint(V2* aabb_center, V2* aabb_size, V2* point, F32 angle_rad); // TODO can be removed with point rotate about point
B32  Aabb2ContainsPoint(V2* aabb_center, V2* aabb_size, V2* point);
B32  Aabb2IntersectLine2(V2* aabb_center, V2* aabb_size, V2* line_start, V2* line_end, V2* enter_point, V2* exit_point);
B32  Aabb2IntersectRay2(V2* aabb_center, V2* aabb_size, V2* ray_start, V2* ray_dir, V2* enter_point, V2* exit_point);
B32  Aabb2IntersectTri2(V2* aabb_center, V2* aabb_size, V2 tri_points[3], IntersectManifold2* manifold);
B32  Aabb2IntersectAabb2(V2* a_center, V2* a_size, V2* b_center, V2* b_size, IntersectManifold2* manifold);
B32  Aabb2IntersectObb2(V2* aabb_center, V2* aabb_size, V2* obb_center, V2* obb_size, F32 obb_angle_rad, IntersectManifold2* manifold);
B32  Aabb2IntersectCircle2(V2* aabb_center, V2* aabb_size, V2* circle_center, F32 circle_radius, IntersectManifold2* manifold);
B32  Aabb2IntersectConvexHull2(V2* aabb_center, V2* aabb_size, V2* hull_points, U32 hull_points_size, IntersectManifold2* manifold);

B32  Obb2Eq(V2* a_center, V2* a_size, F32 a_angle_rad, V2* b_center, V2* b_size, F32 b_angle_rad);
B32  Obb2ApproxEq(V2* a_center, V2* a_size, F32 a_angle_rad, V2* b_center, V2* b_size, F32 b_angle_rad);
void Obb2RotateAboutPoint(V2* obb_center, F32* obb_angle_rad, V2* point, F32 angle_rad);
void Obb2GetEnclosingCircle2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, F32* circle_radius);
void Obb2GetEnclosingAabb2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* abb_size);
B32  Obb2ContainsPoint(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* point);
B32  Obb2IntersectLine2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* line_start, V2* line_end, V2* enter_point, V2* exit_point);
B32  Obb2IntersectRay2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* ray_start, V2* ray_dir, V2* enter_point, V2* exit_point);
B32  Obb2IntersectTri2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2 tri_points[3], IntersectManifold2* manifold);
B32  Obb2IntersectAabb2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* aabb_center, V2* aabb_size, IntersectManifold2* manifold);
B32  Obb2IntersectObb2(V2* a_center, V2* a_size, F32 a_angle_rad, V2* b_center, V2* b_size, F32 b_angle_rad, IntersectManifold2* manifold);
B32  Obb2IntersectCircle2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* circle_center, F32 circle_radius, IntersectManifold2* manifold);
B32  Obb2IntersectConvexHull2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* hull_points, U32 hull_points_size, IntersectManifold2* manifold);

B32  Circle2Eq(V2* a_center, F32 a_radius, V2* b_center, F32 b_radius);
B32  Circle2ApproxEq(V2* a_center, F32 a_radius, V2* b_center, F32 b_radius);
void Circle2GetEnclosingAabb2(F32 circle_radius, V2* aabb_size);
B32  Circle2ContainsPoint(V2* circle_center, F32 circle_radius, V2* point);
B32  Circle2IntersectLine2(V2* circle_center, F32 circle_radius, V2* line_start, V2* line_end, V2* enter_point, V2* exit_point);
B32  Circle2IntersectRay2(V2* circle_center, F32 circle_radius, V2* ray_start, V2* ray_dir, V2* enter_point, V2* exit_point);
B32  Circle2IntersectTri2(V2* circle_center, F32 circle_radius, V2 tri_points[3], IntersectManifold2* manifold);
B32  Circle2IntersectAabb2(V2* circle_center, F32 circle_radius, V2* aabb_center, V2* aabb_size, IntersectManifold2* manifold);
B32  Circle2IntersectObb2(V2* circle_center, F32 circle_radius, V2* obb_center, V2* obb_size, F32 obb_angle_rad, IntersectManifold2* manifold);
B32  Circle2IntersectCircle2(V2* a_center, F32 a_radius, V2* b_center, F32 b_radius, IntersectManifold2* manifold);
B32  Circle2IntersectConvexHull2(V2* circle_center, F32 circle_radius, V2* hull_points, U32 hull_points_size, IntersectManifold2* manifold);

B32  ConvexHull2Eq(V2* a_points, U32 a_points_size, V2* b_points, U32 b_points_size);
B32  ConvexHull2ApproxEq(V2* a_points, U32 a_points_size, V2* b_points, U32 b_points_size);
void ConvexHull2FromAabb2(V2 aabb_points[4], V2* aabb_center, V2* aabb_size);
void ConvexHull2FromObb2(V2 hull_points[4], V2* obb_center, V2* obb_size, F32 obb_angle_rad);
void ConvexHull2Offset(V2* hull_points, U32 hull_points_size, V2* offset);
void ConvexHull2RotateAboutPoint(V2* hull_points, U32 hull_points_size, V2* point, F32 angle_rad);
void ConvexHull2GetCenter(V2* hull_points, U32 hull_points_size, V2* center);
void ConvexHull2GetCenter(V2* hull_points, U32 hull_points_size, V2* center);
void ConvexHull2GetEnclosingCircle2(V2* hull_points, U32 hull_points_size, V2* circle_center, F32* circle_radius);
void ConvexHull2GetEnclosingAabb2(V2* hull_points, U32 hull_points_size, V2* aabb_center, V2* aabb_size);
B32  ConvexHull2ContainsPoint(V2* hull_points, U32 hull_points_size, V2* point);
B32  ConvexHull2IntersectLine2(V2* hull_points, U32 hull_points_size, V2* line_start, V2* line_end, V2* enter_point, V2* exit_point);
B32  ConvexHull2IntersectRay2(V2* hull_points, U32 hull_points_size, V2* ray_start, V2* ray_dir, V2* enter_point, V2* exit_point);
B32  ConvexHull2IntersectTri2(V2* hull_points, U32 hull_points_size, V2 tri_points[3], IntersectManifold2* manifold);
B32  ConvexHull2IntersectAabb2(V2* hull_points, U32 hull_points_size, V2* aabb_center, V2* aabb_size, IntersectManifold2* manifold);
B32  ConvexHull2IntersectObb2(V2* hull_points, U32 hull_points_size, V2* obb_center, V2* obb_size, F32 obb_angle_rad, IntersectManifold2* manifold);
B32  ConvexHull2IntersectCircle2(V2* hull_points, U32 hull_points_size, V2* circle_center, F32 circle_radius, IntersectManifold2* manifold);
B32  ConvexHull2IntersectConvexHull2(V2* a_points, U32 a_points_size, V2* b_points, U32 b_points_size, IntersectManifold2* manifold);

////////////////////////////////////////////////////////////////////////////////
// 3D shapes
////////////////////////////////////////////////////////////////////////////////

typedef struct Ray3 Ray3;
struct Ray3 { V3 start; V3 dir; };

typedef struct Plane3 Plane3;
struct Plane3 { V3 normal; F32 d; };

typedef struct Tri3 Tri3;
struct Tri3 { V3 points[3]; };

typedef struct Aabb3 Aabb3;
struct Aabb3 { V3 center_point; V3 size; };

typedef struct Sphere3 Sphere3;
struct Sphere3 { V3 center_point; F32 radius; };

typedef struct IntersectManifold3 IntersectManifold3;
struct IntersectManifold3 { V3 normal; F32 penetration; };

B32  Line3Eq(V3* a_start, V3* a_end, V3* b_start, V3* b_end);
B32  Line3ApproxEq(V3* a_start, V3* a_end, V3* b_start, V3* b_end);
void Line3Offset(V3* start, V3* end, V3* offset);
F32  Line3GetLength(V3* start, V3* end);
F32  Line3GetLengthSq(V3* start, V3* end);
void Line3GetMidpoint(V3* line_start, V3* line_end, V3* midpoint);
void Line3GetClosestPoint(V3* start, V3* end, V3* point, V3* closest);
B32  Line3IntersectLine3(V3* a_start, V3* a_end, V3* b_start, V3* b_end, V3* intersect_point);
B32  Line3IntersectRay3(V3* line_start, V3* line_end, V3* ray_start, V3* ray_dir, V3* intersect_point);

void Ray3DirInv(Ray3* r, V3* dir_inv);
B32  Ray3IntersectLine3(V3* ray_start, V3* ray_dir, V3* line_start, V3* line_end, V3* intersect_point);
B32  Ray3IntersectRay3(V3* a_start, V3* a_dir, V3* b_start, V3* b_dir, V3* intersect_point);
B32  Ray3IntersectPlane3(Ray3* ray, Plane3* plane, V3* intersect_point);
B32  Ray3IntersectAabb3(Ray3* ray, Aabb3* aabb, V3* enter_point, V3* exit_point);
B32  Ray3IntersectTri3(Ray3* ray, Tri3* tri, V3* intersect_point);
B32  Ray3IntersectSphere3(Ray3* ray, Sphere3* sphere, V3* enter_point, V3* exit_point);

void Plane3WithAnchor(Plane3* plane, V3* anchor, V3* normal);
void Plane3Basis(Plane3* plane, V3* u, V3* v);
B32  Plane3IsPointAbove(Plane3* plane, V3* point);
B32  Plane3ContainsPoint(Plane3* plane, V3* point);
void Plane3CreatePoint(Plane3* plane, V3* point);
void Plane3Flip(Plane3* flipped_plane, Plane3* plane);
B32  Plane3ApproxEq(Plane3* a, Plane3* b);

void Tri3Offset(Tri3* dest, Tri3* src, V3* offset);
B32  Tri3CreatePlane3(Tri3* t, Plane3* plane);
B32  Tri3ContainsPoint(Tri3* t, V3* point);

void CubeRelativeNormal(V3* cube_center_point, V3* pt, V3* normal);
void Aabb3FromMinMax(Aabb3* aabb, V3* min, V3* max);
void Aabb3GetMinMax(Aabb3* aabb, V3* min, V3* max);
B32  Aabb3ContainsPoint(Aabb3* aabb, V3* point);
B32  Aabb3IntersectAabb3(Aabb3* a, Aabb3* b, IntersectManifold3* manifold);

B32  Sphere3ContainsPoint(Sphere3* sphere, V3* point);
B32  Sphere3IntersectSphere3(Sphere3* a, Sphere3* b, IntersectManifold3* manifold);
B32  Sphere3IntersectAabb3(Sphere3* a, Aabb3* b, IntersectManifold3* manifold);

#endif // CDEFAULT_GEOMETRY_H_

#ifdef CDEFAULT_GEOMETRY_IMPLEMENTATION
#undef CDEFAULT_GEOMETRY_IMPLEMENTATION

// TODO make public, in V2?
static void PointRotateAboutPoint(V2* a, V2* p, F32 s, F32 c) {
  V2 rel_a, new_a;
  V2SubV2(&rel_a, a, p);
  new_a.x = rel_a.x * c - rel_a.y * s;
  new_a.y = rel_a.x * s + rel_a.y * c;
  V2AddV2(&new_a, &new_a, p);
  *a = new_a;
}

B32 Line2Eq(V2* a_start, V2* a_end, V2* b_start, V2* b_end) {
  return V2Eq(a_start, b_start) && V2Eq(a_end, b_end);
}

B32 Line2ApproxEq(V2* a_start, V2* a_end, V2* b_start, V2* b_end) {
  return V2ApproxEq(a_start, b_start) && V2ApproxEq(a_end, b_end);
}

void Line2Offset(V2* start, V2* end, V2* offset) {
  V2AddV2(start, start, offset);
  V2AddV2(end, end, offset);
}

void Line2RotateAboutPoint(V2* start, V2* end, V2* point, F32 angle_rad) {
  F32 s = F32Sin(angle_rad);
  F32 c = F32Cos(angle_rad);
  PointRotateAboutPoint(start, point, s, c);
  PointRotateAboutPoint(end, point, s, c);
}

void Line2GetMidpoint(V2* start, V2* end, V2* midpoint) {
  V2AddV2(midpoint, start, end);
  V2MultF32(midpoint, midpoint, 0.5f);
}

F32 Line2GetLength(V2* start, V2* end) {
  return F32Sqrt(Line2GetLengthSq(start, end));
}

F32 Line2GetLengthSq(V2* start, V2* end) {
  V2 v;
  V2SubV2(&v, end, start);
  return V2LengthSq(&v);
}

void Line2GetClosestPoint(V2* start, V2* end, V2* point, V2* closest) {
  V2* a = start;
  V2* b = end;
  V2 ab, ap;
  V2SubV2(&ab, b, a);
  V2SubV2(&ap, point, a);
  F32 t = V2DotV2(&ap, &ab) / V2LengthSq(&ab);
  if (t < 0) {
    *closest = *a;
  } else if (t > 1) {
    *closest = *b;
  } else {
    V2MultF32(&ab, &ab, t);
    V2AddV2(closest, a, &ab);
  }
}

void Line2GetNormalIn(V2* start, V2* end, V2* normal) {
  V2 delta;
  V2SubV2(&delta, end, start);
  normal->x = -delta.y;
  normal->y = delta.x;
  V2Normalize(normal, normal);
}

void Line2GetNormalOut(V2* start, V2* end, V2* normal) {
  V2 delta;
  V2SubV2(&delta, end, start);
  normal->x = delta.y;
  normal->y = -delta.x;
  V2Normalize(normal, normal);
}

B32 Line2IntersectLine2(V2* a_start, V2* a_end, V2* b_start, V2* b_end, V2* intersect_point) {
  if (UNLIKELY(Line2GetLengthSq(b_start, b_end) == 0)) { return false; }
  V2 b_dir;
  V2SubV2(&b_dir, b_end, b_start);
  V2Normalize(&b_dir, &b_dir);

  V2 i;
  if (!Line2IntersectRay2(a_start, a_end, b_start, &b_dir, &i)) { return false; }
  if (Line2GetLengthSq(b_start, b_end) < Line2GetLengthSq(b_start, &i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Line2IntersectRay2(V2* line_start, V2* line_end, V2* ray_start, V2* ray_dir, V2* intersect_point) {
  if (UNLIKELY(Line2GetLengthSq(line_start, line_end) == 0)) { return false; }
  V2 line_dir;
  V2SubV2(&line_dir, line_end, line_start);
  V2Normalize(&line_dir, &line_dir);

  V2 i;
  if (!Ray2IntersectRay2(line_start, &line_dir, ray_start, ray_dir, &i)) { return false; }
  if (Line2GetLengthSq(line_start, line_end) < Line2GetLengthSq(line_start, &i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Line2IntersectTri2(V2* line_start, V2* line_end, V2 tri_points[3], V2* enter_point, V2* exit_point) {
  if (UNLIKELY(Line2GetLengthSq(line_start, line_end) == 0)) { return false; }

  V2 line_dir;
  V2SubV2(&line_dir, line_end, line_start);
  V2Normalize(&line_dir, &line_dir);

  V2 enter, exit;
  if (!Ray2IntersectTri2(line_start, &line_dir, tri_points, &enter, &exit)) { return false; }

  F32 length_sq = Line2GetLengthSq(line_start, line_end);
  if (length_sq < Line2GetLengthSq(line_start, &enter)) { return false; }
  if (length_sq < Line2GetLengthSq(line_start, &exit)) { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL) { *exit_point = exit; }
  return true;
}

B32 Line2IntersectAabb2(V2* line_start, V2* line_end, V2* aabb_center, V2* aabb_size, V2* enter_point, V2* exit_point) {
  if (UNLIKELY(Line2GetLengthSq(line_start, line_end) == 0)) { return false; }

  V2 line_dir;
  V2SubV2(&line_dir, line_end, line_start);
  V2Normalize(&line_dir, &line_dir);

  V2 enter, exit;
  if (!Ray2IntersectAabb2(line_start, &line_dir, aabb_center, aabb_size, &enter, &exit)) { return false; }

  F32 length_sq = Line2GetLengthSq(line_start, line_end);
  if (length_sq < Line2GetLengthSq(line_start, &enter)) { return false; }
  if (length_sq < Line2GetLengthSq(line_start, &exit)) { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL) { *exit_point = exit; }
  return true;
}

B32 Line2IntersectObb2(V2* line_start, V2* line_end, V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* enter_point, V2* exit_point) {
  return Obb2IntersectLine2(obb_center, obb_size, obb_angle_rad, line_start, line_end, enter_point, exit_point);
}

B32 Line2IntersectCircle2(V2* line_start, V2* line_end, V2* circle_center, F32 circle_radius, V2* enter_point, V2* exit_point) {
  if (UNLIKELY(Line2GetLengthSq(line_start, line_end) == 0)) { return false; }

  V2 line_dir;
  V2SubV2(&line_dir, line_end, line_start);
  V2Normalize(&line_dir, &line_dir);

  V2 enter, exit;
  if (!Ray2IntersectCircle2(line_start, &line_dir, circle_center, circle_radius, &enter, &exit)) { return false; }

  F32 length_sq = Line2GetLengthSq(line_start, line_end);
  if (length_sq < Line2GetLengthSq(line_start, &enter)) { return false; }
  if (length_sq < Line2GetLengthSq(line_start, &exit)) { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL) { *exit_point = exit; }
  return true;
}

B32 Line2IntersectConvexHull2(V2* line_start, V2* line_end, V2* hull_points, U32 hull_points_size, V2* enter_point, V2* exit_point) {
  if (UNLIKELY(Line2GetLengthSq(line_start, line_end) == 0)) { return false; }

  V2 line_dir;
  V2SubV2(&line_dir, line_end, line_start);
  V2Normalize(&line_dir, &line_dir);

  V2 enter, exit;
  if (!Ray2IntersectConvexHull2(line_start, &line_dir, hull_points, hull_points_size, &enter, &exit)) { return false; }

  F32 length_sq = Line2GetLengthSq(line_start, line_end);
  if (length_sq < Line2GetLengthSq(line_start, &enter)) { return false; }
  if (length_sq < Line2GetLengthSq(line_start, &exit)) { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL) { *exit_point = exit; }
  return true;
}

B32 Ray2Eq(V2* a_start, V2* a_dir, V2* b_start, V2* b_dir) {
  return V2Eq(a_start, b_start) && V2Eq(a_dir, b_dir);
}

B32 Ray2ApproxEq(V2* a_start, V2* a_dir, V2* b_start, V2* b_dir) {
  return V2ApproxEq(a_start, b_start) && V2ApproxEq(a_dir, b_dir);
}

void Ray2RotateAboutPoint(V2* ray_start, V2* ray_dir, V2* point, F32 angle_rad) {
  F32 s = F32Sin(angle_rad);
  F32 c = F32Cos(angle_rad);
  V2 origin;
  MEMORY_ZERO_STRUCT(&origin);
  PointRotateAboutPoint(ray_start, point, s, c);
  PointRotateAboutPoint(ray_dir, &origin, s, c);
}

void Ray2GetDirInv(V2* ray_dir, V2* dir_inv) {
  dir_inv->x = 1.0f / ray_dir->x;
  dir_inv->y = 1.0f / ray_dir->y;
}

B32 Ray2IntersectLine2(V2* ray_start, V2* ray_dir, V2* line_start, V2* line_end, V2* intersect_point) {
  V2 line_dir;
  V2SubV2(&line_dir, line_end, line_start);
  V2Normalize(&line_dir, &line_dir);

  V2 i;
  if (!Ray2IntersectRay2(ray_start, ray_dir, line_start, &line_dir, &i)) { return false; }
  if (Line2GetLengthSq(line_start, line_end) < Line2GetLengthSq(line_start, &i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Ray2IntersectRay2(V2* a_start, V2* a_dir, V2* b_start, V2* b_dir, V2* intersect_point) {
  F32 cross = V2CrossV2(b_dir, a_dir);
  if (cross == 0) { return false; }
  F32 d_x = b_start->x - a_start->x;
  F32 d_y = b_start->y - a_start->y;
  F32 u = ((d_y * b_dir->x) - (d_x * b_dir->y)) / cross;
  F32 v = ((d_y * a_dir->x) - (d_x * a_dir->y)) / cross;
  if (u < 0 || v < 0) { return false; }
  if (intersect_point != NULL) {
    V2MultF32(intersect_point, a_dir, u);
    V2AddV2(intersect_point, a_start, intersect_point);
  }
  return true;
}

B32 Ray2IntersectTri2(V2* ray_start, V2* ray_dir, V2 tri_points[3], V2* enter_point, V2* exit_point) {
  return Ray2IntersectConvexHull2(ray_start, ray_dir, (V2*) tri_points, 3, enter_point, exit_point);
}

B32 Ray2IntersectAabb2(V2* ray_start, V2* ray_dir, V2* aabb_center, V2* aabb_size, V2* enter_point, V2* exit_point) {
  V2 aabb_min, aabb_max, ray_dir_inv;
  Aabb2GetMinMax(aabb_center, aabb_size, &aabb_min, &aabb_max);
  Ray2GetDirInv(ray_dir, &ray_dir_inv);

  V2 t_1, t_2;
  V2SubV2(&t_1, &aabb_min, ray_start);
  V2HadamardV2(&t_1, &t_1, &ray_dir_inv);
  V2SubV2(&t_2, &aabb_max, ray_start);
  V2HadamardV2(&t_2, &t_2, &ray_dir_inv);

  F32 t_min = MAX(MIN(t_1.x, t_2.x), MIN(t_1.y, t_2.y));
  F32 t_max = MIN(MAX(t_1.x, t_2.x), MAX(t_1.y, t_2.y));
  if (t_max < t_min) { return false; }

  if (t_min < 0) {
    if (t_max < 0) { return false; }
    t_min = t_max;
  }

  if (enter_point != NULL) {
    V2MultF32(enter_point, ray_dir, t_min);
    V2AddV2(enter_point, ray_start, enter_point);
  }
  if (exit_point != NULL) {
    V2MultF32(exit_point, ray_dir, t_max);
    V2AddV2(exit_point, ray_start, exit_point);
  }
  return true;
}

B32 Ray2IntersectObb2(V2* ray_start, V2* ray_dir, V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* enter_point, V2* exit_point) {
  return Obb2IntersectRay2(obb_center, obb_size, obb_angle_rad, ray_start, ray_dir, enter_point, exit_point);
}

B32 Ray2IntersectCircle2(V2* ray_start, V2* ray_dir, V2* circle_center, F32 circle_radius, V2* enter_point, V2* exit_point) {
  V2 rel_center;
  V2SubV2(&rel_center, circle_center, ray_start);
  F32 to_perp_center = V2DotV2(&rel_center, ray_dir);
  F32 d2 = V2DotV2(&rel_center, &rel_center) - (to_perp_center * to_perp_center);
  if (d2 > circle_radius * circle_radius) { return false; }
  if (enter_point == NULL && exit_point == NULL) { return true; }
  F32 center_to_edge = F32Sqrt(circle_radius * circle_radius - d2);
  F32 t0 = to_perp_center - center_to_edge;
  F32 t1 = to_perp_center + center_to_edge;
  if (t1 < t0) { SWAP(F32, t0, t1); }
  if (t0 < 0) {
    if (t1 < 0) { return false; }
    t0 = t1;
  }
  if (enter_point != NULL) {
    V2MultF32(enter_point, ray_dir, t0);
    V2AddV2(enter_point, ray_start, enter_point);
  }
  if (exit_point != NULL) {
    V2MultF32(exit_point, ray_dir, t1);
    V2AddV2(exit_point, ray_start, exit_point);
  }
  return true;
}

B32 Ray2IntersectConvexHull2(V2* ray_start, V2* ray_dir, V2* hull_points, U32 hull_points_size, V2* enter_point, V2* exit_point) {
  B32 result = false;
  F32 min_sq = F32_MAX;
  F32 max_sq = F32_MIN;
  for (U32 i = 0; i < hull_points_size; i++) {
    V2 intersect;
    if (!Ray2IntersectLine2(ray_start, ray_dir, &hull_points[i], &hull_points[(i + 1) % hull_points_size], &intersect)) {
      continue;
    }
    result = true;
    if (enter_point == NULL && exit_point == NULL) { continue; }
    F32 length_sq = Line2GetLengthSq(ray_start, &intersect);
    if (enter_point != NULL && length_sq < min_sq) {
      min_sq = length_sq;
      *enter_point = intersect;
    }
    if (exit_point != NULL && length_sq > max_sq) {
      max_sq = length_sq;
      *exit_point = intersect;
    }
  }
  return result;
}

B32 Tri2Eq(V2 a_points[3], V2 b_points[3]) {
  return V2Eq(&a_points[0], &b_points[0]) &&
         V2Eq(&a_points[1], &b_points[1]) &&
         V2Eq(&a_points[2], &b_points[2]);
}

B32 Tri2ApproxEq(V2 a_points[3], V2 b_points[3]) {
  return V2ApproxEq(&a_points[0], &b_points[0]) &&
         V2ApproxEq(&a_points[1], &b_points[1]) &&
         V2ApproxEq(&a_points[2], &b_points[2]);
}

void Tri2Offset(V2 tri_points[3], V2* offset) {
  V2AddV2(&tri_points[0], &tri_points[0], offset);
  V2AddV2(&tri_points[1], &tri_points[1], offset);
  V2AddV2(&tri_points[2], &tri_points[2], offset);
}

void Tri2RotateAboutPoint(V2 tri_points[3], V2* point, F32 angle_rad) {
  F32 s = F32Sin(angle_rad);
  F32 c = F32Cos(angle_rad);
  for (U32 i = 0; i < 3; i++) {
    PointRotateAboutPoint(&tri_points[i], point, s, c);
  }
}

void Tri2SetCenter(V2 tri_points[3], V2* center) {
  V2 curr_center, offset;
  Tri2GetCenter(tri_points, &curr_center);
  V2SubV2(&offset, center, &curr_center);
  Tri2Offset(tri_points, &offset);
}

void Tri2GetCenter(V2 tri_points[3], V2* center) {
  ConvexHull2GetCenter((V2*) tri_points, 3, center);
}

void Tri2GetEnclosingCircle2(V2 tri_points[3], V2* circle_center, F32* circle_radius) {
  ConvexHull2GetEnclosingCircle2((V2*) tri_points, 3, circle_center, circle_radius);
}

void Tri2GetEnclosingAabb2(V2 tri_points[3], V2* aabb_center, V2* aabb_size) {
  ConvexHull2GetEnclosingAabb2((V2*) tri_points, 3, aabb_center, aabb_size);
}

B32 Tri2ContainsPoint(V2 tri_points[3], V2* point) {
  return ConvexHull2ContainsPoint((V2*) tri_points, 3, point);
}

B32 Tri2IntersectLine2(V2 tri_points[3], V2* line_start, V2* line_end, V2* enter_point, V2* exit_point) {
  return Line2IntersectTri2(line_start, line_end, tri_points, enter_point, exit_point);
}

B32 Tri2IntersectRay2(V2 tri_points[3], V2* ray_start, V2* ray_dir, V2* enter_point, V2* exit_point) {
  return Ray2IntersectTri2(ray_start, ray_dir, tri_points, enter_point, exit_point);
}

B32 Tri2IntersectTri2(V2 a_points[3], V2 b_points[3], IntersectManifold2* manifold) {
  return ConvexHull2IntersectConvexHull2((V2*) a_points, 3, (V2*) b_points, 3, manifold);
}

B32 Tri2IntersectAabb2(V2 tri_points[3], V2* aabb_center, V2* aabb_size, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectConvexHull2((V2*) tri_points, 3, aabb_points, 4, manifold);
}

B32 Tri2IntersectObb2(V2 tri_points[3], V2* obb_center, V2* obb_size, F32 obb_angle_rad, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectConvexHull2((V2*) tri_points, 3, (V2*) obb_points, 4, manifold);
}

B32 Tri2IntersectCircle2(V2 tri_points[3], V2* circle_center, F32 circle_radius, IntersectManifold2* manifold) {
  return ConvexHull2IntersectCircle2((V2*) tri_points, 3, circle_center, circle_radius, manifold);
}

B32 Tri2IntersectConvexHull2(V2 tri_points[3], V2* hull_points, U32 hull_points_size, IntersectManifold2* manifold) {
  return ConvexHull2IntersectConvexHull2((V2*) tri_points, 3, hull_points, hull_points_size, manifold);
}

B32 Aabb2Eq(V2* a_center, V2* a_size, V2* b_center, V2* b_size) {
  return V2Eq(a_center, b_center) && V2Eq(a_size, b_size);
}

B32 Aabb2ApproxEq(V2* a_center, V2* a_size, V2* b_center, V2* b_size) {
  return V2ApproxEq(a_center, b_center) && V2ApproxEq(a_size, b_size);
}

void Aabb2FromMinMax(V2* aabb_center, V2* aabb_size, V2* min, V2* max) {
  DEBUG_ASSERT(min->x < max->x);
  DEBUG_ASSERT(min->y < max->y);
  aabb_center->x = (min->x + max->x) / 2.0f;
  aabb_center->y = (min->y + max->y) / 2.0f;
  aabb_size->x   = max->x - min->x;
  aabb_size->y   = max->y - min->y;
}

void Aabb2FromTopLeft(V2* aabb_center, V2* top_left_point, V2* size) {
  aabb_center->x = top_left_point->x + size->x / 2.0f;
  aabb_center->y = top_left_point->y - size->y / 2.0f;
}

void Aabb2GetMinMax(V2* aabb_center, V2* aabb_size, V2* min, V2* max) {
  V2 half_size;
  V2DivF32(&half_size, aabb_size, 2.0f);
  min->x = aabb_center->x - half_size.x;
  min->y = aabb_center->y - half_size.y;
  max->x = aabb_center->x + half_size.x;
  max->y = aabb_center->y + half_size.y;
}

void Aabb2GetEnclosingCircle2(V2* aabb_center, V2* aabb_size, F32* circle_radius) {
  V2 min, max, rel_center;
  Aabb2GetMinMax(aabb_center, aabb_size, &min, &max);
  V2SubV2(&rel_center, &max, aabb_center);
  *circle_radius = V2Length(&rel_center);
}

void Aabb2RotateAboutPoint(V2* aabb_center, V2* UNUSED(aabb_size), V2* point, F32 angle_rad) {
  PointRotateAboutPoint(aabb_center, point, F32Sin(angle_rad), F32Cos(angle_rad));
}

B32 Aabb2ContainsPoint(V2* aabb_center, V2* aabb_size, V2* point) {
  V2 min, max;
  Aabb2GetMinMax(aabb_center, aabb_size, &min, &max);
  return (min.x <= point->x && point->x <= max.x) &&
         (min.y <= point->y && point->y <= max.y);
}

B32 Aabb2IntersectLine2(V2* aabb_center, V2* aabb_size, V2* line_start, V2* line_end, V2* enter_point, V2* exit_point) {
  return Line2IntersectAabb2(line_start, line_end, aabb_center, aabb_size, enter_point, exit_point);
}

B32 Aabb2IntersectRay2(V2* aabb_center, V2* aabb_size, V2* ray_start, V2* ray_dir, V2* enter_point, V2* exit_point) {
  return Ray2IntersectAabb2(ray_start, ray_dir, aabb_center, aabb_size, enter_point, exit_point);
}

B32 Aabb2IntersectTri2(V2* aabb_center, V2* aabb_size, V2 tri_points[3], IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectConvexHull2((V2*) aabb_points, 4, (V2*) tri_points, 3, manifold);
}

B32 Aabb2IntersectAabb2(V2* a_center, V2* a_size, V2* b_center, V2* b_size, IntersectManifold2* manifold) {
  V2 a_points[4], b_points[4];
  ConvexHull2FromAabb2(a_points, a_center, a_size);
  ConvexHull2FromAabb2(b_points, b_center, b_size);
  return ConvexHull2IntersectConvexHull2((V2*) a_points, 4, (V2*) b_points, 4, manifold);
}

B32 Aabb2IntersectObb2(V2* aabb_center, V2* aabb_size, V2* obb_center, V2* obb_size, F32 obb_angle_rad, IntersectManifold2* manifold) {
  V2 aabb_points[4], obb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectConvexHull2((V2*) aabb_points, 4, (V2*) obb_points, 4, manifold);
}

B32 Aabb2IntersectCircle2(V2* aabb_center, V2* aabb_size, V2* circle_center, F32 circle_radius, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectCircle2((V2*) aabb_points, 4, circle_center, circle_radius, manifold);
}

B32 Aabb2IntersectConvexHull2(V2* aabb_center, V2* aabb_size, V2* hull_points, U32 hull_points_size, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectConvexHull2(aabb_points, 4, hull_points, hull_points_size, manifold);
}

B32 Obb2Eq(V2* a_center, V2* a_size, F32 a_angle_rad, V2* b_center, V2* b_size, F32 b_angle_rad) {
  return V2Eq(a_center, b_center) &&
         V2Eq(a_size, b_size) &&
         a_angle_rad == b_angle_rad;
}

B32 Obb2ApproxEq(V2* a_center, V2* a_size, F32 a_angle_rad, V2* b_center, V2* b_size, F32 b_angle_rad) {
  return V2ApproxEq(a_center, b_center) &&
         V2ApproxEq(a_size, b_size) &&
         F32ApproxEq(a_angle_rad, b_angle_rad);
}

void Obb2RotateAboutPoint(V2* obb_center, F32* obb_angle_rad, V2* point, F32 angle_rad) {
  PointRotateAboutPoint(obb_center, point, F32Sin(angle_rad), F32Cos(angle_rad));
  *obb_angle_rad += angle_rad;
}

// SPEEDUP: Lots of OBB fns convert to a convex hull; may be better to cache the translation.
void Obb2GetEnclosingCircle2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, F32* circle_radius) {
  V2 temp, obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  ConvexHull2GetEnclosingCircle2((V2*) obb_points, 4, &temp, circle_radius);
}

void Obb2GetEnclosingAabb2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* aabb_size) {
  V2 obb_points[4], temp;
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  ConvexHull2GetEnclosingAabb2((V2*) obb_points, 4, &temp, aabb_size);
}

B32 Obb2ContainsPoint(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* point) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2ContainsPoint((V2*) obb_points, 4, point);
}

B32 Obb2IntersectLine2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* line_start, V2* line_end, V2* enter_point, V2* exit_point) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectLine2((V2*) obb_points, 4, line_start, line_end, enter_point, exit_point);
}

B32 Obb2IntersectRay2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* ray_start, V2* ray_dir, V2* enter_point, V2* exit_point) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectRay2((V2*) obb_points, 4, ray_start, ray_dir, enter_point, exit_point);
}

B32 Obb2IntersectTri2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2 tri_points[3], IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectConvexHull2(obb_points, 4, tri_points, 3, manifold);
}

B32 Obb2IntersectAabb2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* aabb_center, V2* aabb_size, IntersectManifold2* manifold) {
  V2 obb_points[4], aabb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectConvexHull2((V2*) obb_points, 4, (V2*) aabb_points, 4, manifold);
}

B32 Obb2IntersectObb2(V2* a_center, V2* a_size, F32 a_angle_rad, V2* b_center, V2* b_size, F32 b_angle_rad, IntersectManifold2* manifold) {
  V2 a_points[4], b_points[4];
  ConvexHull2FromObb2(a_points, a_center, a_size, a_angle_rad);
  ConvexHull2FromObb2(b_points, b_center, b_size, b_angle_rad);
  return ConvexHull2IntersectConvexHull2((V2*) a_points, 4, (V2*) b_points, 4, manifold);
}

B32 Obb2IntersectCircle2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* circle_center, F32 circle_radius, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectCircle2((V2*) obb_points, 4, circle_center, circle_radius, manifold);
}

B32 Obb2IntersectConvexHull2(V2* obb_center, V2* obb_size, F32 obb_angle_rad, V2* hull_points, U32 hull_points_size, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectConvexHull2(obb_points, 4, hull_points, hull_points_size, manifold);
}

B32 Circle2Eq(V2* a_center, F32 a_radius, V2* b_center, F32 b_radius) {
  return V2Eq(a_center, b_center) && a_radius == b_radius;
}

B32 Circle2ApproxEq(V2* a_center, F32 a_radius, V2* b_center, F32 b_radius) {
  return V2ApproxEq(a_center, b_center) && F32ApproxEq(a_radius, b_radius);
}

void Circle2GetEnclosingAabb2(F32 circle_radius, V2* aabb_size) {
  aabb_size->x = circle_radius * 2;
  aabb_size->y = aabb_size->x;
}

B32 Circle2ContainsPoint(V2* circle_center, F32 circle_radius, V2* point) {
  V2 to_pt;
  V2SubV2(&to_pt, point, circle_center);
  return V2LengthSq(&to_pt) < (circle_radius * circle_radius);
}

B32 Circle2IntersectLine2(V2* circle_center, F32 circle_radius, V2* line_start, V2* line_end, V2* enter_point, V2* exit_point) {
  return Line2IntersectCircle2(line_start, line_end, circle_center, circle_radius, enter_point, exit_point);
}

B32 Circle2IntersectRay2(V2* circle_center, F32 circle_radius, V2* ray_start, V2* ray_dir, V2* enter_point, V2* exit_point) {
  return Ray2IntersectCircle2(ray_start, ray_dir, circle_center, circle_radius, enter_point, exit_point);
}

B32 Circle2IntersectTri2(V2* circle_center, F32 circle_radius, V2 tri_points[3], IntersectManifold2* manifold) {
  return Circle2IntersectConvexHull2(circle_center, circle_radius, (V2*) tri_points, 3, manifold);
}

B32 Circle2IntersectAabb2(V2* circle_center, F32 circle_radius, V2* aabb_center, V2* aabb_size, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return Circle2IntersectConvexHull2(circle_center, circle_radius, (V2*) aabb_points, 4, manifold);
}

B32 Circle2IntersectObb2(V2* circle_center, F32 circle_radius, V2* obb_center, V2* obb_size, F32 obb_angle_rad, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return Circle2IntersectConvexHull2(circle_center, circle_radius, (V2*) obb_points, 4, manifold);
}

B32 Circle2IntersectCircle2(V2* a_center, F32 a_radius, V2* b_center, F32 b_radius, IntersectManifold2* manifold) {
  V2 center_diff;
  V2SubV2(&center_diff, a_center, b_center);
  F32 min_distance = a_radius + b_radius;
  F32 distance = V2Length(&center_diff);
  if (min_distance < distance) { return false; }
  if (manifold != NULL) {
    if (V2LengthSq(&center_diff) == 0) { center_diff.y = 1.0f; }
    V2Normalize(&manifold->normal, &center_diff);
    manifold->penetration = min_distance - distance;
    V2MultF32(&manifold->contact_points[0], &manifold->normal, -a_radius);
    V2AddV2(&manifold->contact_points[0], &manifold->contact_points[0], a_center);
    manifold->contact_points_size = 1;
  }
  return true;
}

static void Circle2ProjectAxis(V2* circle_center, F32 circle_radius, V2* axis, F32* min, F32* max) {
  F32 circle_center_proj = V2DotV2(circle_center, axis);
  *min = circle_center_proj - circle_radius;
  *max = circle_center_proj + circle_radius;
}

static void ConvexHull2ProjectAxis(V2* hull_points, U32 hull_points_size, V2* axis, F32* min, F32* max) {
  DEBUG_ASSERT(hull_points_size > 0);
  F32 proj = V2DotV2(&hull_points[0], axis);
  *min = proj;
  *max = proj;
  for (U32 i = 1; i < hull_points_size; i++) {
    proj = V2DotV2(&hull_points[i], axis);
    if (proj < *min) { *min = proj; }
    if (proj > *max) { *max = proj; }
  }
}

// NOTE: this is the same as ConvexHull2IntersectCircle2 with different manifold calculations.
B32 Circle2IntersectConvexHull2(V2* circle_center, F32 circle_radius, V2* hull_points, U32 hull_points_size, IntersectManifold2* manifold) {
  V2 hull_center, rel_center;
  ConvexHull2GetCenter(hull_points, hull_points_size, &hull_center);
  V2SubV2(&rel_center, &hull_center, circle_center);

  F32 min_penetration = F32_MAX;
  for (U32 i = 0; i < hull_points_size; i++) {
    V2* start = &hull_points[i];
    V2* end   = &hull_points[(i + 1) % hull_points_size];
    DEBUG_ASSERT(Line2GetLengthSq(start, end) > 0);
    V2 axis;
    Line2GetNormalOut(start, end, &axis);

    F32 hull_min, hull_max, circle_min, circle_max;
    ConvexHull2ProjectAxis(hull_points, hull_points_size, &axis, &hull_min, &hull_max);
    Circle2ProjectAxis(circle_center, circle_radius, &axis, &circle_min, &circle_max);
    F32 hull_overlap = MIN(hull_max, circle_max) - MAX(hull_min, circle_min);
    if (hull_overlap <= 0) { return false; }

    V2 to_circle;
    V2SubV2(&to_circle, &hull_points[i], circle_center);
    V2Normalize(&to_circle, &to_circle);
    ConvexHull2ProjectAxis(hull_points, hull_points_size, &to_circle, &hull_min, &hull_max);
    Circle2ProjectAxis(circle_center, circle_radius, &to_circle, &circle_min, &circle_max);
    F32 circle_overlap = MIN(hull_max, circle_max) - MAX(hull_min, circle_min);
    if (circle_overlap <= 0) { return false; }

    if (manifold != NULL && hull_overlap < min_penetration) {
      min_penetration = hull_overlap;
      manifold->penetration = hull_overlap;
      manifold->normal = axis;
    }
  }
  if (manifold != NULL) {
    if (V2DotV2(&rel_center, &manifold->normal) > 0) { V2MultF32(&manifold->normal, &manifold->normal, -1); }
    V2Normalize(&manifold->normal, &manifold->normal);

    V2MultF32(&manifold->contact_points[0], &manifold->normal, -circle_radius);
    V2AddV2(&manifold->contact_points[0], &manifold->contact_points[0], circle_center);
    manifold->contact_points_size = 1;
  }
  return true;
}

// NOTE: Finds the furthest vertex and edge in the given convex hull along the provided normal.
static void ConvexHull2MaxFeatureAlongNormal(
    V2* points, U32 points_size, V2* center, V2* normal,
    V2** feature, V2** start, V2** end) {
  U32 max_feature_idx = 0;
  F32 max_proj = 0;
  for (U32 i = 0; i < points_size; i++) {
    V2 test;
    V2SubV2(&test, &points[i], center);
    F32 proj = V2DotV2(normal, &test);
    if (proj > max_proj) {
      max_proj = proj;
      max_feature_idx = i;
    }
  }

  *feature        = &points[max_feature_idx];
  V2* left_start  = &points[(max_feature_idx + points_size - 1) % points_size];
  V2* right_start = &points[(max_feature_idx + 1) % points_size];
  V2 left, right;
  V2SubV2(&left, *feature, left_start);
  V2SubV2(&right, *feature, right_start);
  V2Normalize(&left,  &left);
  V2Normalize(&right, &right);

  if (V2DotV2(&left, normal) <= V2DotV2(&right, normal)) {
    *start = *feature;
    *end   = left_start;
  } else {
    *start = right_start;
    *end   = *feature;
  }
}

// NOTE: Clips start and end along the ray described by normal and t.
static void ConvexHull2Clip(V2 start, V2 end, V2 normal, F32 t, V2 clipped_points[2], U32* clipped_points_size) {
  *clipped_points_size = 0;
  F32 d_0 = V2DotV2(&normal, &start) - t;
  F32 d_1 = V2DotV2(&normal, &end)   - t;
  if (d_0 >= 0) { clipped_points[(*clipped_points_size)++] = start; }
  if (d_1 >= 0) { clipped_points[(*clipped_points_size)++] = end;   }
  // NOTE: will be negative iff d_0 and d_1 have different signs.
  if (d_0 * d_1 < 0) {
    DEBUG_ASSERT(*clipped_points_size < 2);
    V2 edge;
    V2SubV2(&edge, &end, &start);
    F32 x = d_0 / (d_0 - d_1);
    V2MultF32(&edge, &edge, x);
    V2AddV2(&edge, &edge, &start);
    clipped_points[(*clipped_points_size)++] = edge;
  }
}

B32 ConvexHull2Eq(V2* a_points, U32 a_points_size, V2* b_points, U32 b_points_size) {
  if (a_points_size != b_points_size) { return false; }
  for (U32 i = 0; i < a_points_size; i++) {
    if (!V2Eq(&a_points[i], &b_points[i])) { return false; }
  }
  return true;
}

B32 ConvexHull2ApproxEq(V2* a_points, U32 a_points_size, V2* b_points, U32 b_points_size) {
  if (a_points_size != b_points_size) { return false; }
  for (U32 i = 0; i < a_points_size; i++) {
    if (!V2ApproxEq(&a_points[i], &b_points[i])) { return false; }
  }
  return true;
}

void ConvexHull2FromAabb2(V2 hull_points[4], V2* aabb_center, V2* aabb_size) {
  V2 min, max;
  Aabb2GetMinMax(aabb_center, aabb_size, &min, &max);
  hull_points[0] = (V2) { min.x, min.y };
  hull_points[1] = (V2) { max.x, min.y };
  hull_points[2] = (V2) { max.x, max.y };
  hull_points[3] = (V2) { min.x, max.y };
}

void ConvexHull2FromObb2(V2 hull_points[4], V2* obb_center, V2* obb_size, F32 obb_angle_rad) {
  ConvexHull2FromAabb2(hull_points, obb_center, obb_size);
  ConvexHull2RotateAboutPoint((V2*) hull_points, 4, obb_center, obb_angle_rad);
}

void ConvexHull2Offset(V2* hull_points, U32 hull_points_size, V2* offset) {
  for (U32 i = 0; i < hull_points_size; i++) {
    V2AddV2(&hull_points[i], &hull_points[i], offset);
  }
}

void ConvexHull2RotateAboutPoint(V2* hull_points, U32 hull_points_size, V2* point, F32 angle_rad) {
  F32 s = F32Sin(angle_rad);
  F32 c = F32Cos(angle_rad);
  for (U32 i = 0; i < hull_points_size; i++) {
    PointRotateAboutPoint(&hull_points[i], point, s, c);
  }
}

void ConvexHull2SetCenter(V2* hull_points, U32 hull_points_size, V2* center) {
  V2 curr_center, offset;
  ConvexHull2GetCenter(hull_points, hull_points_size, &curr_center);
  V2SubV2(&offset, center, &curr_center);
  ConvexHull2Offset(hull_points, hull_points_size, &offset);
}

void ConvexHull2GetCenter(V2* hull_points, U32 hull_points_size, V2* center) {
  center->x = 0;
  center->y = 0;
  for (U32 i = 0; i < hull_points_size; i++) {
    center->x += hull_points[i].x;
    center->y += hull_points[i].y;
  }
  center->x /= hull_points_size;
  center->y /= hull_points_size;
}

void ConvexHull2GetEnclosingCircle2(V2* hull_points, U32 hull_points_size, V2* circle_center, F32* circle_radius) {
  ConvexHull2GetCenter(hull_points, hull_points_size, circle_center);
  V2 to_pt;
  F32 max_dist = F32_MIN;
  for (U32 i = 0; i < hull_points_size; i++) {
    V2SubV2(&to_pt, &hull_points[i], circle_center);
    F32 dist_sq = V2LengthSq(&to_pt);
    if (max_dist < dist_sq) { max_dist = dist_sq; }
  }
  *circle_radius = F32Sqrt(max_dist);
}

void ConvexHull2GetEnclosingAabb2(V2* hull_points, U32 hull_points_size, V2* aabb_center, V2* aabb_size) {
  ConvexHull2GetCenter(hull_points, hull_points_size, aabb_center);
  F32 x_min = F32_MAX;
  F32 y_min = F32_MAX;
  F32 x_max = F32_MIN;
  F32 y_max = F32_MIN;
  V2 to_pt;
  for (U32 i = 0; i < hull_points_size; i++) {
    V2SubV2(&to_pt, &hull_points[i], aabb_center);
    if (to_pt.x < x_min) { x_min = to_pt.x; }
    if (to_pt.y < y_min) { y_min = to_pt.y; }
    if (to_pt.x > x_max) { x_max = to_pt.x; }
    if (to_pt.y > y_max) { y_max = to_pt.y; }
  }
  aabb_size->x = x_max - x_min;
  aabb_size->y = y_max - y_min;
}

B32 ConvexHull2ContainsPoint(V2* hull_points, U32 hull_points_size, V2* point) {
  for (U32 i = 0; i < hull_points_size; i++) {
    V2* a = &hull_points[i];
    V2* b = &hull_points[(i + 1) % hull_points_size];
    V2 ab, ap;
    V2SubV2(&ab, b, a);
    V2SubV2(&ap, point, a);
    F32 cross = V2CrossV2(&ab, &ap);
    if (cross < 0) { return false; }
  }
  return true;
}

B32 ConvexHull2IntersectLine2(V2* hull_points, U32 hull_points_size, V2* line_start, V2* line_end, V2* enter_point, V2* exit_point) {
  return Line2IntersectConvexHull2(line_start, line_end, hull_points, hull_points_size, enter_point, exit_point);
}

B32 ConvexHull2IntersectRay2(V2* hull_points, U32 hull_points_size, V2* ray_start, V2* ray_dir, V2* enter_point, V2* exit_point) {
  return Ray2IntersectConvexHull2(ray_start, ray_dir, hull_points, hull_points_size, enter_point, exit_point);
}

B32 ConvexHull2IntersectTri2(V2* hull_points, U32 hull_points_size, V2 tri_points[3], IntersectManifold2* manifold) {
  return ConvexHull2IntersectConvexHull2(hull_points, hull_points_size, (V2*) tri_points, 3, manifold);
}

B32 ConvexHull2IntersectAabb2(V2* hull_points, U32 hull_points_size, V2* aabb_center, V2* aabb_size, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectConvexHull2(hull_points, hull_points_size, (V2*) aabb_points, 4, manifold);
}

B32 ConvexHull2IntersectObb2(V2* hull_points, U32 hull_points_size, V2* obb_center, V2* obb_size, F32 obb_angle_rad, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectConvexHull2(hull_points, hull_points_size, (V2*) obb_points, 4, manifold);
}

// NOTE: this is the same as Circle2IntersectConvexHull2 with different manifold calculations.
B32 ConvexHull2IntersectCircle2(V2* hull_points, U32 hull_points_size, V2* circle_center, F32 circle_radius, IntersectManifold2* manifold) {
  V2 hull_center, rel_center;
  ConvexHull2GetCenter(hull_points, hull_points_size, &hull_center);
  V2SubV2(&rel_center, &hull_center, circle_center);

  F32 min_penetration = F32_MAX;
  for (U32 i = 0; i < hull_points_size; i++) {
    V2* start = &hull_points[i];
    V2* end   = &hull_points[(i + 1) % hull_points_size];
    DEBUG_ASSERT(Line2GetLengthSq(start, end) > 0);
    V2 axis;
    Line2GetNormalOut(start, end, &axis);

    F32 hull_min, hull_max, circle_min, circle_max;
    ConvexHull2ProjectAxis(hull_points, hull_points_size, &axis, &hull_min, &hull_max);
    Circle2ProjectAxis(circle_center, circle_radius, &axis, &circle_min, &circle_max);
    F32 hull_overlap = MIN(hull_max, circle_max) - MAX(hull_min, circle_min);
    if (hull_overlap <= 0) { return false; }

    V2 to_circle;
    V2SubV2(&to_circle, &hull_points[i], circle_center);
    V2Normalize(&to_circle, &to_circle);
    ConvexHull2ProjectAxis(hull_points, hull_points_size, &to_circle, &hull_min, &hull_max);
    Circle2ProjectAxis(circle_center, circle_radius, &to_circle, &circle_min, &circle_max);
    F32 circle_overlap = MIN(hull_max, circle_max) - MAX(hull_min, circle_min);
    if (circle_overlap <= 0) { return false; }

    if (manifold != NULL && hull_overlap < min_penetration) {
      min_penetration = hull_overlap;
      manifold->penetration = hull_overlap;
      manifold->normal = axis;
    }
  }

  if (manifold != NULL) {
    if (V2DotV2(&rel_center, &manifold->normal) < 0) { V2MultF32(&manifold->normal, &manifold->normal, -1); }
    V2Normalize(&manifold->normal, &manifold->normal);

    V2 *i_start, *i_end, *feature, i_normal_inv;
    V2MultF32(&i_normal_inv, &manifold->normal, -1);
    ConvexHull2MaxFeatureAlongNormal(hull_points, hull_points_size, &hull_center, &i_normal_inv, &feature, &i_start, &i_end);
    Line2GetClosestPoint(i_start, i_end, circle_center, &manifold->contact_points[0]);
    manifold->contact_points_size = 1;
  }
  return true;
}

B32 ConvexHull2IntersectConvexHull2(V2* a_points, U32 a_points_size, V2* b_points, U32 b_points_size, IntersectManifold2* manifold) {
  V2 a_center, b_center, rel_center;
  ConvexHull2GetCenter(a_points, a_points_size, &a_center);
  ConvexHull2GetCenter(b_points, b_points_size, &b_center);
  V2SubV2(&rel_center, &a_center, &b_center);

  F32 min_penetration = F32_MAX;
  for (U32 shape_idx = 0; shape_idx < 2; shape_idx++) {
    V2* points      = a_points;
    U32 points_size = a_points_size;
    if (shape_idx == 1) {
      points      = b_points;
      points_size = b_points_size;
    }

    for (U32 i = 0; i < points_size; i++) {
      V2* start = &points[i];
      V2* end   = &points[(i + 1) % points_size];
      DEBUG_ASSERT(Line2GetLengthSq(start, end) > 0);
      V2 axis;
      Line2GetNormalOut(start, end, &axis);

      F32 a_min, a_max, b_min, b_max;
      ConvexHull2ProjectAxis(a_points, a_points_size, &axis, &a_min, &a_max);
      ConvexHull2ProjectAxis(b_points, b_points_size, &axis, &b_min, &b_max);
      F32 overlap = MIN(a_max, b_max) - MAX(a_min, b_min);
      if (overlap <= 0) { return false; }

      if (manifold != NULL && overlap < min_penetration) {
        min_penetration = overlap;
        manifold->penetration = overlap;
        manifold->normal = axis;
      }
    }
  }
  // https://dyn4j.org/2011/11/contact-points-using-clipping/
  if (manifold != NULL) {
    if (V2DotV2(&rel_center, &manifold->normal) < 0) { V2MultF32(&manifold->normal, &manifold->normal, -1); }
    V2Normalize(&manifold->normal, &manifold->normal);

    V2 *a_i_start, *a_i_end, *b_i_start, *b_i_end, *a_feature, *b_feature, i_normal_inv;
    V2MultF32(&i_normal_inv, &manifold->normal, -1);
    ConvexHull2MaxFeatureAlongNormal(a_points, a_points_size, &a_center, &i_normal_inv, &a_feature, &a_i_start, &a_i_end);
    ConvexHull2MaxFeatureAlongNormal(b_points, b_points_size, &b_center, &manifold->normal, &b_feature, &b_i_start, &b_i_end);

    V2 ref_edge, ref_edge_inv, ref_normal;
    V2SubV2(&ref_edge, b_i_end, b_i_start);
    V2Normalize(&ref_edge, &ref_edge);
    V2MultF32(&ref_edge_inv, &ref_edge, -1);
    Line2GetNormalOut(b_i_start, b_i_end, &ref_normal);

    V2 c_points[2];
    U32 c_points_size;
    F32 t_0 = +V2DotV2(&ref_edge, b_i_start);
    ConvexHull2Clip(*a_i_start, *a_i_end, ref_edge, t_0, c_points, &c_points_size);
    if (c_points_size > 1) {
      F32 t_1 = -V2DotV2(&ref_edge, b_i_end);
      ConvexHull2Clip(c_points[0], c_points[1], ref_edge_inv, t_1, c_points, &c_points_size);
    }

    F32 max = V2DotV2(&ref_normal, b_feature);
    manifold->contact_points_size = 0;
    for (U32 i = 0; i < c_points_size; i++) {
      V2 test = c_points[i];
      if (V2DotV2(&ref_normal, &test) - max >= 0) {
        manifold->contact_points[manifold->contact_points_size++] = test;
      }
    }
  }
  return true;
}

B32 Line3Eq(V3* a_start, V3* a_end, V3* b_start, V3* b_end) {
  return V3Eq(a_start, b_start) && V3Eq(a_end, b_end);
}

B32 Line3ApproxEq(V3* a_start, V3* a_end, V3* b_start, V3* b_end) {
  return V3ApproxEq(a_start, b_start) && V3ApproxEq(a_end, b_end);
}

void Line3Offset(V3* start, V3* end, V3* offset) {
  V3AddV3(start, start, offset);
  V3AddV3(end, end, offset);
}

F32 Line3GetLength(V3* start, V3* end) {
  return F32Sqrt(Line3GetLengthSq(start, end));
}

F32 Line3GetLengthSq(V3* start, V3* end) {
  V3 v;
  V3SubV3(&v, end, start);
  return V3LengthSq(&v);
}

void Line3GetMidpoint(V3* line_start, V3* line_end, V3* midpoint) {
  V3AddV3(midpoint, line_start, line_end);
  V3MultF32(midpoint, midpoint, 0.5f);
}

void Line3GetClosestPoint(V3* start, V3* end, V3* point, V3* closest) {
  V3* a = start;
  V3* b = end;
  V3 ab, ap;
  V3SubV3(&ab, b, a);
  V3SubV3(&ap, point, a);
  F32 t = V3DotV3(&ap, &ab) / V3LengthSq(&ab);
  if (t < 0) {
    *closest = *a;
  } else if (t > 1) {
    *closest = *b;
  } else {
    V3MultF32(&ab, &ab, t);
    V3AddV3(closest, a, &ab);
  }
}

B32 Line3IntersectLine3(V3* a_start, V3* a_end, V3* b_start, V3* b_end, V3* intersect_point) {
  if (UNLIKELY(Line3GetLengthSq(b_start, b_end) == 0)) { return false; }
  V3 b_dir;
  V3SubV3(&b_dir, b_end, b_start);
  V3Normalize(&b_dir, &b_dir);

  V3 i;
  if (!Line3IntersectRay3(a_start, a_end, b_start, &b_dir, &i)) { return false; }
  if (Line3GetLengthSq(b_start, b_end) < Line3GetLengthSq(b_start, &i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Line3IntersectRay3(V3* line_start, V3* line_end, V3* ray_start, V3* ray_dir, V3* intersect_point) {
  if (UNLIKELY(Line3GetLengthSq(line_start, line_end) == 0)) { return false; }
  V3 line_dir;
  V3SubV3(&line_dir, line_end, line_start);
  V3Normalize(&line_dir, &line_dir);

  V3 i;
  if (!Ray3IntersectRay3(line_start, &line_dir, ray_start, ray_dir, &i)) { return false; }
  if (Line3GetLengthSq(line_start, line_end) < Line3GetLengthSq(line_start, &i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Ray3IntersectLine3(V3* ray_start, V3* ray_dir, V3* line_start, V3* line_end, V3* intersect_point) {
  if (UNLIKELY(Line3GetLengthSq(line_start, line_end) == 0)) { return false; }
  V3 line_dir;
  V3SubV3(&line_dir, line_end, line_start);
  V3Normalize(&line_dir, &line_dir);

  V3 i;
  if (!Ray3IntersectRay3(ray_start, ray_dir, line_start, &line_dir, &i)) { return false; }
  if (Line3GetLengthSq(line_start, line_end) < Line3GetLengthSq(line_start, &i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Ray3IntersectRay3(V3* a_start, V3* a_dir, V3* b_start, V3* b_dir, V3* intersect_point) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(a_dir), 1));
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(b_dir), 1));

  V3 d_start;
  V3SubV3(&d_start, b_start, a_start);
  F32 dir_dot = V3DotV3(a_dir, b_dir);
  F32 denom   = 1.0f - (dir_dot * dir_dot);
  if (F32ApproxEq(denom, 0)) {
    // NOTE: parallel or collinear case
    // NOTE: is parallel non-intersecting
    V3 start_cross, dir_cross;
    V3CrossV3(&start_cross, &d_start, a_dir);
    if (!F32ApproxEq(V3LengthSq(&start_cross), 0)) { return false; }
    V3CrossV3(&dir_cross, a_dir, b_dir);
    if (!F32ApproxEq(V3LengthSq(&dir_cross), 0)) { return false; }

    // NOTE: is collinear, determine if there's overlap
    F32 t = V3DotV3(a_dir, &d_start);
    F32 u = -V3DotV3(b_dir, &d_start);
    if (t < 0 && u < 0) { return false; }

    if (intersect_point != NULL) {
      // NOTE: favor intersection from A's perspective, u > 0 --> B pointing at A.
      if (u > 0) { *intersect_point = *a_start; }
      else       { *intersect_point = *b_start; }
    }
    return true;
  } else {
    // NOTE: skew case, find closest point on each ray to other ray
    F32 a = V3DotV3(a_dir, &d_start);
    F32 b = V3DotV3(b_dir, &d_start);
    F32 t = (a - (dir_dot * b)) / denom;
    F32 u = ((dir_dot * a) - b) / denom;
    if (t < 0 || u < 0) { return false; }
    V3 a_closest, b_closest;
    V3MultF32(&a_closest, a_dir, t);
    V3AddV3(&a_closest, &a_closest, a_start);
    V3MultF32(&b_closest, b_dir, u);
    V3AddV3(&b_closest, &b_closest, b_start);
    if (!V3ApproxEq(&a_closest, &b_closest)) { return false; }

    if (intersect_point != NULL) { *intersect_point = a_closest; }
    return true;
  }
}

void Ray3DirInv(Ray3* r, V3* dir_inv) {
  dir_inv->x = 1.0f / r->dir.x;
  dir_inv->y = 1.0f / r->dir.y;
  dir_inv->z = 1.0f / r->dir.z;
}

B32 Ray3IntersectPlane3(Ray3* ray, Plane3* plane, V3* intersect_point) {
  V3 point_on_plane, ray_to_plane;
  Plane3CreatePoint(plane, &point_on_plane);
  F32 denom = V3DotV3(&plane->normal, &ray->dir);
  if (denom == 0) { return false; }
  V3SubV3(&point_on_plane, &ray->start, &ray_to_plane);
  F32 t = (V3DotV3(&ray_to_plane, &plane->normal)) / denom;
  if (t < 0) { return false; }
  if (intersect_point != NULL) {
    V3MultF32(intersect_point, &ray->dir, t);
    V3AddV3(intersect_point, &ray->start, intersect_point);
  }
  return true;
}

// https://tavianator.com/2011/ray_box.html
B32 Ray3IntersectAabb3(Ray3* ray, Aabb3* aabb, V3* enter_point, V3* exit_point) {
  V3 aabb_min, aabb_max;
  Aabb3GetMinMax(aabb, &aabb_min, &aabb_max);
  V3 ray_dir_inv;
  Ray3DirInv(ray, &ray_dir_inv);

  V3 t_1, t_2;
  V3SubV3(&t_1, &aabb_min, &ray->start);
  V3HadamardV3(&t_1, &t_1, &ray_dir_inv);
  V3SubV3(&t_2, &aabb_max, &ray->start);
  V3HadamardV3(&t_2, &t_2, &ray_dir_inv);

  F32 t_min = MAX3(MIN(t_1.x, t_2.x), MIN(t_1.y, t_2.y), MIN(t_1.z, t_2.z));
  F32 t_max = MIN3(MAX(t_1.x, t_2.x), MAX(t_1.y, t_2.y), MAX(t_1.z, t_2.z));
  if (t_max < t_min) { return false; }

  if (t_min < 0) {
    if (t_max < 0) { return false; }
    t_min = t_max;
  }

  if (enter_point != NULL) {
    V3MultF32(enter_point, &ray->dir, t_min);
    V3AddV3(enter_point, &ray->start, enter_point);
  }
  if (exit_point != NULL) {
    V3MultF32(exit_point, &ray->dir, t_max);
    V3AddV3(exit_point, &ray->start, exit_point);
  }
  return true;
}

B32 Ray3IntersectTri3(Ray3* ray, Tri3* tri, V3* intersect_point) {
  Plane3 tri_plane;
  if (!Tri3CreatePlane3(tri, &tri_plane)) { return false; }
  V3 plane_intersection;
  if (!Ray3IntersectPlane3(ray, &tri_plane, &plane_intersection)) { return false; }
  if (!Tri3ContainsPoint(tri, &plane_intersection)) { return false; }
  if (intersect_point != NULL) { *intersect_point = plane_intersection; }
  return true;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
B32 Ray3IntersectSphere3(Ray3* ray, Sphere3* sphere, V3* enter_point, V3* exit_point) {
  V3 rel_center;
  V3SubV3(&rel_center, &sphere->center_point, &ray->start);
  F32 to_perp_center = V3DotV3(&rel_center, &ray->dir);
  F32 d2 = V3DotV3(&rel_center, &rel_center) - (to_perp_center * to_perp_center);
  if (d2 > sphere->radius * sphere->radius) { return false; }
  if (enter_point == NULL && exit_point == NULL) { return true; }
  F32 center_to_edge = F32Sqrt(sphere->radius * sphere->radius - d2);
  F32 t0 = to_perp_center - center_to_edge;
  F32 t1 = to_perp_center + center_to_edge;
  if (t1 < t0) { SWAP(F32, t0, t1); }
  if (t0 < 0) {
    if (t1 < 0) { return false; }
    t0 = t1;
  }
  if (enter_point != NULL) {
    V3MultF32(enter_point, &ray->dir, t0);
    V3AddV3(enter_point, &ray->start, enter_point);
  }
  if (exit_point != NULL) {
    V3MultF32(exit_point, &ray->dir, t1);
    V3AddV3(exit_point, &ray->start, exit_point);
  }
  return true;
}

// https://mathinsight.org/distance_point_plane
void Plane3WithAnchor(Plane3* plane, V3* anchor, V3* normal) {
  plane->normal = *normal;
  plane->d = V3DotV3(anchor, normal);
}

// https://math.stackexchange.com/questions/1702572/how-to-find-the-basis-of-a-plane-or-a-line
// NOTE: *not* orthonormal.
void Plane3Basis(Plane3* plane, V3* u, V3* v) {
  if (plane->normal.x != 0) {
    *u = (V3) {(plane->d - plane->normal.z) / plane->normal.x, 0, 1};
    *v = (V3) {(plane->d - plane->normal.y) / plane->normal.x, 1, 0};
  } else if (plane->normal.y != 0) {
    *u = (V3) {1, (plane->d - plane->normal.x) / plane->normal.y, 0};
    *v = (V3) {0, (plane->d - plane->normal.z) / plane->normal.y, 1};
  } else if (plane->normal.z != 0) {
    *u = (V3) {1, 0, (plane->d - plane->normal.x) / plane->normal.z};
    *v = (V3) {0, 1, (plane->d - plane->normal.y) / plane->normal.z};
  } else { UNREACHABLE(); }
}

B32 Plane3IsPointAbove(Plane3* plane, V3* point) {
  V3 point_on_plane, vec;
  Plane3CreatePoint(plane, &point_on_plane);
  V3SubV3(point, &point_on_plane, &vec);
  return V3DotV3(&plane->normal, &vec) > 0;
}

B32 Plane3ContainsPoint(Plane3* plane, V3* point) {
  return V3DotV3(&plane->normal, point) == plane->d;
}

// https://math.stackexchange.com/questions/723937/find-the-point-on-a-plane-3x-4y-z-1-that-is-closest-to-1-0-1
void Plane3CreatePoint(Plane3* plane, V3* point) {
  F32 t = plane->d / V3DotV3(&plane->normal, &plane->normal);
  V3MultF32(point, &plane->normal, t);
}

void Plane3Flip(Plane3* flipped_plane, Plane3* plane) {
  V3MultF32(&flipped_plane->normal, &plane->normal, -1);
  flipped_plane->d = plane->d * -1;
}

B32 Plane3ApproxEq(Plane3* a, Plane3* b) {
  return V3ApproxEq(&a->normal, &b->normal) && F32ApproxEq(a->d, b->d);
}

void Tri3Offset(Tri3* dest, Tri3* src, V3* offset) {
  V3AddV3(&src->points[0], offset, &dest->points[0]);
  V3AddV3(&src->points[1], offset, &dest->points[1]);
  V3AddV3(&src->points[2], offset, &dest->points[2]);
}

B32 Tri3CreatePlane3(Tri3* t, Plane3* plane) {
  V3 a, b;
  V3SubV3(&t->points[1], &t->points[0], &a);
  V3SubV3(&t->points[2], &t->points[0], &b);

  // NOTE: tri points are too close to each other
  if (F32ApproxEq(V3LengthSq(&a), 0) || F32ApproxEq(V3LengthSq(&b), 0)) {
    return false;
  }
  V3Normalize(&a, &a);
  V3Normalize(&b, &b);

  V3 tri_norm;
  V3CrossV3(&a, &b, &tri_norm);
  // NOTE: tri points along the same line
  if (V3LengthSq(&tri_norm) == 0) { return false; }
  V3Normalize(&tri_norm, &tri_norm);

  Plane3WithAnchor(plane, &t->points[0], &tri_norm);
  return true;
}

// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html
B32 Tri3ContainsPoint(Tri3* t, V3* point) {
  V3 a, b, c;
  V3SubV3(&t->points[0], point, &a);
  V3SubV3(&t->points[1], point, &b);
  V3SubV3(&t->points[2], point, &c);

  V3 u, v, w;
  V3CrossV3(&b, &c, &u);
  V3CrossV3(&c, &a, &v);
  V3CrossV3(&a, &b, &w);

  if (V3DotV3(&a, &v) < 0) { return false; }
  if (V3DotV3(&u, &w) < 0) { return false; }
  return true;
}

// https://stackoverflow.com/questions/75811830/determine-the-face-of-a-cube-intersected-by-a-ray-in-a-minecraft-like-game
void CubeRelativeNormal(V3* cube_center_point, V3* pt, V3* normal) {
  V3 relative_p;
  V3SubV3(&relative_p, pt, cube_center_point);
  F32 p_max = MAX3(relative_p.x, relative_p.y, relative_p.z);
  F32 p_min = MIN3(relative_p.x, relative_p.y, relative_p.z);
  F32 abs_p_min = F32Abs(p_min);
  if (p_max == relative_p.x && p_max > abs_p_min)  { *normal = V3_X_POS; return; }
  if (p_min == relative_p.x && p_max < abs_p_min)  { *normal = V3_X_NEG; return; }
  if (p_max == relative_p.y && p_max > abs_p_min)  { *normal = V3_Y_POS; return; }
  if (p_min == relative_p.y && p_max < abs_p_min)  { *normal = V3_Y_NEG; return; }
  if (p_max == relative_p.z && p_max >= abs_p_min) { *normal = V3_Z_POS; return; }
  if (p_min == relative_p.z && p_max <= abs_p_min) { *normal = V3_Z_NEG; return; }
  UNREACHABLE();
}

void Aabb3FromMinMax(Aabb3* aabb3, V3* min, V3* max) {
  DEBUG_ASSERT(min->x < max->x);
  DEBUG_ASSERT(min->y < max->y);
  DEBUG_ASSERT(min->z < max->z);
  aabb3->center_point.x = (min->x + max->x) / 2.0f;
  aabb3->center_point.y = (min->y + max->y) / 2.0f;
  aabb3->center_point.z = (min->z + max->z) / 2.0f;
  aabb3->size.x = max->x - min->x;
  aabb3->size.y = max->y - min->y;
  aabb3->size.z = max->z - min->z;
}

void Aabb3GetMinMax(Aabb3* aabb, V3* min, V3* max) {
  min->x = aabb->center_point.x - (aabb->size.x / 2.0f);
  min->y = aabb->center_point.y - (aabb->size.y / 2.0f);
  min->z = aabb->center_point.z - (aabb->size.z / 2.0f);
  max->x = aabb->center_point.x + (aabb->size.x / 2.0f);
  max->y = aabb->center_point.y + (aabb->size.y / 2.0f);
  max->z = aabb->center_point.z + (aabb->size.z / 2.0f);
}

B32 Aabb3ContainsPoint(Aabb3* aabb, V3* point) {
  V3 min, max;
  Aabb3GetMinMax(aabb, &min, &max);
  return ((min.x <= point->x && point->x <= max.x) &&
          (min.y <= point->y && point->y <= max.y) &&
          (min.z <= point->z && point->z <= max.z));
}

B32 Sphere3ContainsPoint(Sphere3* sphere, V3* point) {
  V3 to_pt;
  V3SubV3(&to_pt, point, &sphere->center_point);
  return V3LengthSq(&to_pt) < (sphere->radius * sphere->radius);
}

B32 Sphere3IntersectSphere3(Sphere3* a, Sphere3* b, IntersectManifold3* manifold) {
  V3 center_diff;
  V3SubV3(&center_diff, &a->center_point, &b->center_point);
  F32 min_distance = a->radius + b->radius;
  F32 distance = V3Length(&center_diff);
  if (min_distance < distance) { return false; }

  if (manifold != NULL) {
    if (V3LengthSq(&center_diff) == 0) { center_diff.z = 1.0f; }
    V3Normalize(&manifold->normal, &center_diff);
    manifold->penetration = min_distance - distance;
  }
  return true;
}

B32 Sphere3IntersectAabb3(Sphere3* a, Aabb3* b, IntersectManifold3* manifold) {
  V3 min, max;
  Aabb3GetMinMax(b, &min, &max);

  V3 rel_closest_point;
  rel_closest_point.x = CLAMP(min.x, a->center_point.x, max.x);
  rel_closest_point.y = CLAMP(min.y, a->center_point.y, max.y);
  rel_closest_point.z = CLAMP(min.z, a->center_point.z, max.z);

  V3 to_closest_point;
  V3SubV3(&to_closest_point, &a->center_point, &rel_closest_point);
  F32 d_sq = V3LengthSq(&to_closest_point);
  F32 r_sq = a->radius * a->radius;

  if (d_sq > 0) {
    // NOTE: Outside the AABB
    if (d_sq >= r_sq) { return false; }
    if (manifold != NULL) {
      manifold->penetration = a->radius - F32Sqrt(d_sq);
      manifold->normal = to_closest_point;
      DEBUG_ASSERT(V3Length(&manifold->normal) > 0);
      V3Normalize(&manifold->normal, &manifold->normal);
    }
    return true;
  } else {
    // NOTE: Inside the AABB.
    F32 dx_left  = a->center_point.x - min.x;
    F32 dx_right = max.x - a->center_point.x;
    F32 dy_bot   = a->center_point.y - min.y;
    F32 dy_top   = max.y - a->center_point.y;
    F32 dz_in    = a->center_point.z - min.z;
    F32 dz_out   = max.z - a->center_point.z;

    F32 min = dx_left;
    V3 normal = V3_X_NEG;
    if (dx_right < min) {
      min = dx_right;
      normal = V3_X_POS;
    }
    if (dy_bot < min) {
      min = dy_bot;
      normal = V3_Y_NEG;
    }
    if (dy_top < min) {
      min = dy_top;
      normal = V3_Y_POS;
    }
    if (dz_in < min) {
      min = dz_in;
      normal = V3_Z_NEG;
    }
    if (dz_out < min) {
      min = dz_out;
      normal = V3_Z_POS;
    }

    manifold->normal = normal;
    manifold->penetration = a->radius + min;
    return true;
  }
}

B32 Aabb3IntersectAabb3(Aabb3* a, Aabb3* b, IntersectManifold3* manifold) {
  V3 a_min, a_max;
  Aabb3GetMinMax(a, &a_min, &a_max);
  V3 b_min, b_max;
  Aabb3GetMinMax(b, &b_min, &b_max);

  if (!(a_max.x > b_min.x &&
        a_min.x < b_max.x &&
        a_max.y > b_min.y &&
        a_min.y < b_max.y &&
        a_max.z > b_min.z &&
        a_min.z < b_max.z)) {
    return false;
  }

  if (manifold != NULL) {
    F32 penetration = F32_MAX;
    F32 penetration_test = 0;
    V3 min_axis = {};
#define CDEFAULT_PENETRATION_TEST(test, axis) \
      penetration_test = (test);              \
      if (penetration_test < penetration) {   \
        penetration = penetration_test;       \
        min_axis = (axis);                    \
      }
    CDEFAULT_PENETRATION_TEST(b_max.x - a_min.x, V3_X_POS);
    CDEFAULT_PENETRATION_TEST(a_max.x - b_min.x, V3_X_NEG);
    CDEFAULT_PENETRATION_TEST(b_max.y - a_min.y, V3_Y_POS);
    CDEFAULT_PENETRATION_TEST(a_max.y - b_min.y, V3_Y_NEG);
    CDEFAULT_PENETRATION_TEST(b_max.z - a_min.z, V3_Z_POS);
    CDEFAULT_PENETRATION_TEST(a_max.z - b_min.z, V3_Z_NEG);
#undef CDEFAULT_PENETRATION_TEST
    manifold->normal = min_axis;
    manifold->penetration = penetration;
  }
  return true;
}

#endif // CDEFAULT_GEOMETRY_IMPLEMENTATION
