#ifndef CDEFAULT_GEOMETRY_H_
#define CDEFAULT_GEOMETRY_H_

#include "cdefault_math.h"
#include "cdefault_std.h"
#include "cdefault_io.h"

// TODO: harden routines against e.g. division by 0
// TODO: finish rounding out 3d
// TODO: debug assertions for e.g. dirs, etc are normal
// TODO: plug in validations where appropriate
// TODO: support face intersections for flat shapes in 3d?
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

B32  Line2Eq(V2 a_start, V2 a_end, V2 b_start, V2 b_end);
B32  Line2ApproxEq(V2 a_start, V2 a_end, V2 b_start, V2 b_end);
void Line2Offset(V2* start, V2* end, V2 offset);
void Line2RotateAroundPoint(V2* start, V2* end, V2 point, F32 angle_rad);
B32  Line2ContainsPoint(V2 start, V2 end, V2 point);
F32  Line2GetLength(V2 start, V2 end);
F32  Line2GetLengthSq(V2 start, V2 end);
V2   Line2GetDir(V2 start, V2 end);
V2   Line2GetMidpoint(V2 line_start, V2 line_end);
V2   Line2GetClosestPoint(V2 start, V2 end, V2 point);
V2   Line2GetNormalIn(V2 start, V2 end);  // NOTE: points into CCW center. TODO: V2 fn instead?
V2   Line2GetNormalOut(V2 start, V2 end); // NOTE: points away from CCW center. TODO: V2 fn instead?
B32  Line2IntersectLine2(V2 a_start, V2 a_end, V2 b_start, V2 b_end, V2* intersect_point);
B32  Line2IntersectRay2(V2 line_start, V2 line_end, V2 ray_start, V2 ray_dir, V2* intersect_point);
B32  Line2IntersectTri2(V2 line_start, V2 line_end, V2 tri_points[3], V2 tri_offset, V2* enter_point, V2* exit_point);
B32  Line2IntersectAabb2(V2 line_start, V2 line_end, V2 aabb_center, V2 aabb_size, V2* enter_point, V2* exit_point);
B32  Line2IntersectObb2(V2 line_start, V2 line_end, V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2* enter_point, V2* exit_point);
B32  Line2IntersectCircle2(V2 line_start, V2 line_end, V2 circle_center, F32 circle_radius, V2* enter_point, V2* exit_point);
B32  Line2IntersectConvexHull2(V2 line_start, V2 line_end, V2* hull_points, U32 hull_points_size, V2 hull_offset, V2* enter_point, V2* exit_point);

B32  Ray2Validate(V2 ray_dir);
B32  Ray2Eq(V2 a_start, V2 a_dir, V2 b_start, V2 b_dir);
B32  Ray2ApproxEq(V2 a_start, V2 a_dir, V2 b_start, V2 b_dir);
void Ray2RotateAroundPoint(V2* ray_start, V2* ray_dir, V2 point, F32 angle_rad);
B32  Ray2ContainsPoint(V2 ray_start, V2 ray_dir, V2 point);
V2   Ray2GetDirInv(V2 ray_dir); // NOTE: careful about axis aligned dirs.
B32  Ray2IntersectLine2(V2 ray_start, V2 ray_dir, V2 line_start, V2 line_end, V2* intersect_point);
B32  Ray2IntersectRay2(V2 a_start, V2 a_dir, V2 b_start, V2 b_dir, V2* intersect_point);
B32  Ray2IntersectTri2(V2 ray_start, V2 ray_dir, V2 tri_points[3], V2 tri_offset, V2* enter_point, V2* exit_point);
B32  Ray2IntersectAabb2(V2 ray_start, V2 ray_dir, V2 aabb_center, V2 aabb_size, V2* enter_point, V2* exit_point);
B32  Ray2IntersectObb2(V2 ray_start, V2 ray_dir, V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2* enter_point, V2* exit_point);
B32  Ray2IntersectCircle2(V2 ray_start, V2 ray_dir, V2 circle_center, F32 circle_radius, V2* enter_point, V2* exit_point);
B32  Ray2IntersectConvexHull2(V2 ray_start, V2 ray_dir, V2* hull_points, U32 hull_points_size, V2 hull_offset, V2* enter_point, V2* exit_point);

// TODO: scale for shapes?

B32  Tri2Validate(V2 tri_points[3]);
B32  Tri2Eq(V2 a_points[3], V2 b_points[3]);
B32  Tri2ApproxEq(V2 a_points[3], V2 b_points[3]);
void Tri2Offset(V2 tri_points[3], V2 offset);
void Tri2RotateAroundPoint(V2 tri_points[3], V2 point, F32 angle_rad);
void Tri2SetCenter(V2 tri_points[3], V2 center);
V2   Tri2GetCenter(V2 tri_points[3]);
void Tri2GetEnclosingCircle2(V2 tri_points[3], V2 tri_offset, V2* circle_center, F32* circle_radius);
void Tri2GetEnclosingAabb2(V2 tri_points[3], V2 tri_offset, V2* aabb_center, V2* aabb_size);
B32  Tri2ContainsPoint(V2 tri_points[3], V2 tri_offset, V2 point);
B32  Tri2IntersectLine2(V2 tri_points[3], V2 tri_offset, V2 line_start, V2 line_end, V2* enter_point, V2* exit_point);
B32  Tri2IntersectRay2(V2 tri_points[3], V2 tri_offset, V2 ray_start, V2 ray_dir, V2* enter_point, V2* exit_point);
B32  Tri2IntersectTri2(V2 a_points[3], V2 a_offset, V2 b_points[3], V2 b_offset, IntersectManifold2* manifold);
B32  Tri2IntersectAabb2(V2 tri_points[3], V2 tri_offset, V2 aabb_center, V2 aabb_size, IntersectManifold2* manifold);
B32  Tri2IntersectObb2(V2 tri_points[3], V2 tri_offset, V2 obb_center, V2 obb_size, F32 obb_angle_rad, IntersectManifold2* manifold);
B32  Tri2IntersectCircle2(V2 tri_points[3], V2 tri_offset, V2 circle_center, F32 circle_radius, IntersectManifold2* manifold);
B32  Tri2IntersectConvexHull2(V2 tri_points[3], V2 tri_offset, V2* hull_points, U32 hull_points_size, V2 hull_offset, IntersectManifold2* manifold);

B32  Aabb2Validate(V2 aabb_size);
B32  Aabb2Eq(V2 a_center, V2 a_size, V2 b_center, V2 b_size);
B32  Aabb2ApproxEq(V2 a_center, V2 a_size, V2 b_center, V2 b_size);
void Aabb2FromMinMax(V2* aabb_center, V2* aabb_size, V2 min, V2 max);
void Aabb2FromTopLeft(V2* aabb_center, V2 top_left_point, V2 size);
void Aabb2GetMinMax(V2 aabb_center, V2 aabb_size, V2* min, V2* max);
void Aabb2GetEnclosingCircle2(V2 aabb_center, V2 aabb_size, F32* circle_radius);
void Aabb2RotateAroundPoint(V2* aabb_center, V2 point, F32 angle_rad);
B32  Aabb2ContainsPoint(V2 aabb_center, V2 aabb_size, V2 point);
B32  Aabb2IntersectLine2(V2 aabb_center, V2 aabb_size, V2 line_start, V2 line_end, V2* enter_point, V2* exit_point);
B32  Aabb2IntersectRay2(V2 aabb_center, V2 aabb_size, V2 ray_start, V2 ray_dir, V2* enter_point, V2* exit_point);
B32  Aabb2IntersectTri2(V2 aabb_center, V2 aabb_size, V2 tri_points[3], V2 tri_offset, IntersectManifold2* manifold);
B32  Aabb2IntersectAabb2(V2 a_center, V2 a_size, V2 b_center, V2 b_size, IntersectManifold2* manifold);
B32  Aabb2IntersectObb2(V2 aabb_center, V2 aabb_size, V2 obb_center, V2 obb_size, F32 obb_angle_rad, IntersectManifold2* manifold);
B32  Aabb2IntersectCircle2(V2 aabb_center, V2 aabb_size, V2 circle_center, F32 circle_radius, IntersectManifold2* manifold);
B32  Aabb2IntersectConvexHull2(V2 aabb_center, V2 aabb_size, V2* hull_points, U32 hull_points_size, V2 hull_offset, IntersectManifold2* manifold);

// NOTE: Most OBB fns convert to a convex hull, so it may be more performant to use that natively.
B32  Obb2Validate(V2 obb_size);
B32  Obb2Eq(V2 a_center, V2 a_size, F32 a_angle_rad, V2 b_center, V2 b_size, F32 b_angle_rad);
B32  Obb2ApproxEq(V2 a_center, V2 a_size, F32 a_angle_rad, V2 b_center, V2 b_size, F32 b_angle_rad);
void Obb2RotateAroundPoint(V2* obb_center, F32* obb_angle_rad, V2 point, F32 angle_rad);
void Obb2GetEnclosingCircle2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, F32* circle_radius);
void Obb2GetEnclosingAabb2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2* abb_size);
B32  Obb2ContainsPoint(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 point);
B32  Obb2IntersectLine2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 line_start, V2 line_end, V2* enter_point, V2* exit_point);
B32  Obb2IntersectRay2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 ray_start, V2 ray_dir, V2* enter_point, V2* exit_point);
B32  Obb2IntersectTri2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 tri_points[3], V2 tri_offset, IntersectManifold2* manifold);
B32  Obb2IntersectAabb2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 aabb_center, V2 aabb_size, IntersectManifold2* manifold);
B32  Obb2IntersectObb2(V2 a_center, V2 a_size, F32 a_angle_rad, V2 b_center, V2 b_size, F32 b_angle_rad, IntersectManifold2* manifold);
B32  Obb2IntersectCircle2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 circle_center, F32 circle_radius, IntersectManifold2* manifold);
B32  Obb2IntersectConvexHull2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2* hull_points, U32 hull_points_size, V2 hull_offset, IntersectManifold2* manifold);

B32  Circle2Validate(F32 circle_radius);
B32  Circle2Eq(V2 a_center, F32 a_radius, V2 b_center, F32 b_radius);
B32  Circle2ApproxEq(V2 a_center, F32 a_radius, V2 b_center, F32 b_radius);
void Circle2GetEnclosingAabb2(F32 circle_radius, V2* aabb_size);
B32  Circle2ContainsPoint(V2 circle_center, F32 circle_radius, V2 point);
void Circle2RotateAroundPoint(V2* circle_center, V2 point, F32 angle_rad);
B32  Circle2IntersectLine2(V2 circle_center, F32 circle_radius, V2 line_start, V2 line_end, V2* enter_point, V2* exit_point);
B32  Circle2IntersectRay2(V2 circle_center, F32 circle_radius, V2 ray_start, V2 ray_dir, V2* enter_point, V2* exit_point);
B32  Circle2IntersectTri2(V2 circle_center, F32 circle_radius, V2 tri_points[3], V2 tri_offset, IntersectManifold2* manifold);
B32  Circle2IntersectAabb2(V2 circle_center, F32 circle_radius, V2 aabb_center, V2 aabb_size, IntersectManifold2* manifold);
B32  Circle2IntersectObb2(V2 circle_center, F32 circle_radius, V2 obb_center, V2 obb_size, F32 obb_angle_rad, IntersectManifold2* manifold);
B32  Circle2IntersectCircle2(V2 a_center, F32 a_radius, V2 b_center, F32 b_radius, IntersectManifold2* manifold);
B32  Circle2IntersectConvexHull2(V2 circle_center, F32 circle_radius, V2* hull_points, U32 hull_points_size, V2 hull_offset, IntersectManifold2* manifold);

// TODO: flatten / triangulate fns
B32  ConvexHull2Validate(U32 hull_points_size);
B32  ConvexHull2Eq(V2* a_points, U32 a_points_size, V2* b_points, U32 b_points_size);
B32  ConvexHull2ApproxEq(V2* a_points, U32 a_points_size, V2* b_points, U32 b_points_size);
void ConvexHull2FromAabb2(V2 aabb_points[4], V2 aabb_center, V2 aabb_size);
void ConvexHull2FromObb2(V2 hull_points[4], V2 obb_center, V2 obb_size, F32 obb_angle_rad);
void ConvexHull2Offset(V2* hull_points, U32 hull_points_size, V2 offset);
void ConvexHull2RotateAroundPoint(V2* hull_points, U32 hull_points_size, V2 point, F32 angle_rad);
V2   ConvexHull2GetCenter(V2* hull_points, U32 hull_points_size);
void ConvexHull2SetCenter(V2* hull_points, U32 hull_points_size, V2 center);
void ConvexHull2GetEnclosingCircle2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2* circle_center, F32* circle_radius);
void ConvexHull2GetEnclosingAabb2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2* aabb_center, V2* aabb_size);
B32  ConvexHull2ContainsPoint(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 point);
B32  ConvexHull2IntersectLine2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 line_start, V2 line_end, V2* enter_point, V2* exit_point);
B32  ConvexHull2IntersectRay2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 ray_start, V2 ray_dir, V2* enter_point, V2* exit_point);
B32  ConvexHull2IntersectTri2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 tri_points[3], V2 tri_offset, IntersectManifold2* manifold);
B32  ConvexHull2IntersectAabb2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 aabb_center, V2 aabb_size, IntersectManifold2* manifold);
B32  ConvexHull2IntersectObb2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 obb_center, V2 obb_size, F32 obb_angle_rad, IntersectManifold2* manifold);
B32  ConvexHull2IntersectCircle2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 circle_center, F32 circle_radius, IntersectManifold2* manifold);
B32  ConvexHull2IntersectConvexHull2(V2* a_points, U32 a_points_size, V2 a_offset, V2* b_points, U32 b_points_size, V2 b_offset, IntersectManifold2* manifold);

////////////////////////////////////////////////////////////////////////////////
// 3D shapes
////////////////////////////////////////////////////////////////////////////////

// TODO: include contact points
typedef struct IntersectManifold3 IntersectManifold3;
struct IntersectManifold3 {
  V3  normal;
  F32 penetration;
};

B32  Line3Eq(V3 a_start, V3 a_end, V3 b_start, V3 b_end);
B32  Line3ApproxEq(V3 a_start, V3 a_end, V3 b_start, V3 b_end);
void Line3Offset(V3* start, V3* end, V3 offset);
void Line3RotateAroundPoint(V3* start, V3* end, V3 point, M3 rot);
F32  Line3GetLength(V3 start, V3 end);
F32  Line3GetLengthSq(V3 start, V3 end);
V3   Line3GetDir(V3 start, V3 end);
V3   Line3GetMidpoint(V3 line_start, V3 line_end);
V3   Line3GetClosestPoint(V3 start, V3 end, V3 point);
B32  Line3ContainsPoint(V3 start, V3 end, V3 point);
B32  Line3IntersectLine3(V3 a_start, V3 a_end, V3 b_start, V3 b_end, V3* intersect_point);
B32  Line3IntersectRay3(V3 line_start, V3 line_end, V3 ray_start, V3 ray_dir, V3* intersect_point);
B32  Line3IntersectPlane3(V3 line_start, V3 line_end, V3 plane_normal, F32 plane_d, V3* intersect_point);
B32  Line3IntersectTri3(V3 line_start, V3 line_end, V3 tri_points[3], V3 tri_offset, V3* intersect_point);
B32  Line3IntersectConvexPolygon3(V3 line_start, V3 line_end, V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* intersect_point);
B32  Line3IntersectSphere3(V3 line_start, V3 line_end, V3 sphere_center, F32 sphere_radius, V3* enter_point, V3* exit_point);
B32  Line3IntersectAabb3(V3 line_start, V3 line_end, V3 aabb_center, V3 aabb_size, V3* enter_point, V3* exit_point);
B32  Line3IntersectConvexHull3(V3 line_start, V3 line_end, V3* hull_points, U32 hull_points_size, U32* hull_indices, U32 hull_indices_size, V3 hull_offset, V3* enter_point, V3* exit_point);
B32  Line3IntersectMesh3(V3 line_start, V3 line_end, V3* mesh_points, U32 mesh_points_size, U32* mesh_indices, U32 mesh_indices_size, V3 mesh_offset, V3* enter_point, V3* exit_point);

B32  Ray3Validate(V3 ray_dir);
B32  Ray3Eq(V3 a_start, V3 a_dir, V3 b_start, V3 b_dir);
B32  Ray3ApproxEq(V3 a_start, V3 a_dir, V3 b_start, V3 b_dir);
void Ray3RotateAroundPoint(V3* ray_start, V3* ray_dir, V3 point, M3 rot);
B32  Ray3ContainsPoint(V3 ray_start, V3 ray_dir, V3 point);
V3   Ray3GetDirInv(V3 ray_dir);
B32  Ray3IntersectLine3(V3 ray_start, V3 ray_dir, V3 line_start, V3 line_end, V3* intersect_point);
B32  Ray3IntersectRay3(V3 a_start, V3 a_dir, V3 b_start, V3 b_dir, V3* intersect_point);
B32  Ray3IntersectPlane3(V3 ray_start, V3 ray_dir, V3 plane_normal, F32 plane_d, V3* intersect_point);
B32  Ray3IntersectTri3(V3 ray_start, V3 ray_dir, V3 tri_points[3], V3 tri_offset, V3* intersect_point);
B32  Ray3IntersectConvexPolygon3(V3 ray_start, V3 ray_dir, V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* intersect_point);
B32  Ray3IntersectSphere3(V3 ray_start, V3 ray_dir, V3 sphere_center, F32 sphere_radius, V3* enter_point, V3* exit_point);
B32  Ray3IntersectAabb3(V3 ray_start, V3 ray_dir, V3 aabb_center, V3 aabb_size, V3* enter_point, V3* exit_point);
B32  Ray3IntersectConvexHull3(V3 ray_start, V3 ray_dir, V3* hull_points, U32 hull_points_size, U32* hull_indices, U32 hull_indices_size, V3 hull_offset, V3* enter_point, V3* exit_point);
B32  Ray3IntersectMesh3(V3 ray_start, V3 ray_dir, V3* mesh_points, U32 mesh_points_size, U32* mesh_indices, U32 mesh_indices_size, V3 mesh_offset, V3* enter_point, V3* exit_point);

// TODO: plane contains flat shape fns
B32  Plane3Validate(V3 plane_normal);
B32  Plane3Eq(V3 a_normal, F32 a_d, V3 b_normal, F32 b_d);
B32  Plane3ApproxEq(V3 a_normal, F32 a_d, V3 b_normal, F32 b_d);
void Plane3FromAnchor(V3 plane_normal, F32* plane_d, V3 anchor); // NOTE: determines d given the normal and a point on the plane.
B32  Plane3FromTri3(V3* plane_normal, F32* plane_d, V3 tri_points[3], V3 tri_offset);
B32  Plane3FromConvexPolygon3(V3* plane_normal, F32* plane_d, V3* polygon_points, U32 polygon_points_size, V3 polygon_offset);
B32  Plane3IsPointAbove(V3 plane_normal, F32 plane_d, V3 point);
B32  Plane3IsPointBelow(V3 plane_normal, F32 plane_d, V3 point);
B32  Plane3ContainsPoint(V3 plane_normal, F32 plane_d, V3 point);
void Plane3RotateAroundPoint(V3* plane_normal, F32* plane_d, V3 point, M3 rot);
V3   Plane3CreatePointOnPlane(V3 plane_normal, F32 plane_d);
void Plane3Offset(V3* plane_normal, F32* plane_d, V3 offset);
void Plane3Flip(V3* plane_normal, F32* plane_d);
B32  Plane3IntersectLine3(V3 plane_normal, F32 plane_d, V3 line_start, V3 line_end, V3* intersect_point);
B32  Plane3IntersectRay3(V3 plane_normal, F32 plane_d, V3 ray_start, V3 ray_dir, V3* intersect_point);
B32  Plane3IntersectPlane3(V3 a_normal, F32 a_d, V3 b_normal, F32 b_d, V3* intersect_point, V3* intersect_dir); // NOTE: returns false on coincident planes.
B32  Plane3IntersectTri3(V3 plane_normal, F32 plane_d, V3 tri_points[3], V3 tri_offset, V3* intersect_start, V3* intersect_end);
B32  Plane3IntersectConvexPolygon3(V3 plane_normal, F32 plane_d, V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* intersect_start, V3* intersect_end);
// TODO: intersect sphere & others

B32  Tri3Validate(V3 tri_points[3]);
B32  Tri3Eq(V3 a_points[3], V3 b_points[3]);
B32  Tri3ApproxEq(V3 a_points[3], V3 b_points[3]);
void Tri3Offset(V3 tri_points[3], V3 offset);
void Tri3SetCenter(V3 tri_points[3], V3 center);
V3   Tri3GetCenter(V3 tri_points[3]);
V3   Tri3GetNormal(V3 tri_points[3]);
void Tri3GetEnclosingAabb3(V3 tri_points[3], V3 tri_offset, V3* aabb_center, V3* aabb_size);
void Tri3GetEnclosingSphere3(V3 tri_points[3], V3 tri_offset, V3* sphere_center, F32* sphere_radius);
void Tri3RotateAroundPoint(V3 tri_points[3], V3 point, M3 rot);
B32  Tri3ContainsPoint(V3 tri_points[3], V3 tri_offset, V3 point);
B32  Tri3IntersectLine3(V3 tri_points[3], V3 tri_offset, V3 line_start, V3 line_end, V3* intersect_point);
B32  Tri3IntersectRay3(V3 tri_points[3], V3 tri_offset, V3 ray_start, V3 ray_dir, V3* intersect_point);
B32  Tri3IntersectPlane3(V3 tri_points[3], V3 tri_offset, V3 plane_normal, F32 plane_d, V3* intersect_start, V3* intersect_end);
B32  Tri3IntersectTri3(V3 a_points[3], V3 a_offset, V3 b_points[3], V3 b_offset, V3* intersect_start, V3* intersect_end);
B32  Tri3IntersectConvexPolygon3(V3 tri_points[3], V3 tri_offset, V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* intersect_start, V3* intersect_end);
// TODO: intersect sphere & others

// TODO: more flat 3d shapes?
// TODO: rotation for flat & non-flat 3d shapes?

B32  ConvexPolygon3Validate(U32 polygon_points_size);
B32  ConvexPolygon3Eq(V3* a_points, U32 a_points_size, V3* b_points, U32 b_points_size);
B32  ConvexPolygon3ApproxEq(V3* a_points, U32 a_points_size, V3* b_points, U32 b_points_size);
void ConvexPolygon3Offset(V3* polygon_points, U32 polygon_points_size, V3 offset);
V3   ConvexPolygon3GetCenter(V3* polygon_points, U32 polygon_points_size);
void ConvexPolygon3SetCenter(V3* polygon_points, U32 polygon_points_size, V3 center);
void ConvexPolygon3GetEnclosingAabb3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* aabb_center, V3* aabb_size);
void ConvexPolygon3GetEnclosingSphere3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* sphere_center, F32* sphere_radius);
void ConvexPolygon3RotateAroundPoint(V3* polygon_points, U32 polygon_points_size, V3 point, M3 rot);
B32  ConvexPolygon3ContainsPoint(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3 point);
B32  ConvexPolygon3IntersectLine3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3 line_start, V3 line_end, V3* intersect_point);
B32  ConvexPolygon3IntersectRay3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3 ray_start, V3 ray_dir, V3* intersect_point);
B32  ConvexPolygon3IntersectPlane3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3 plane_normal, F32 plane_d, V3* intersect_start, V3* intersect_end);
B32  ConvexPolygon3IntersectTri3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3 tri_points[3], V3 tri_offset, V3* intersect_start, V3* intersect_end);
B32  ConvexPolygon3IntersectConvexPolygon3(V3* a_points, U32 a_points_size, V3 a_offset, V3* b_points, U32 b_points_size, V3 b_offset, V3* intersect_start, V3* intersect_end);
// TODO: intersect sphere & others

B32  Aabb3Validate(V3 aabb_size);
V3   Aabb3RelativeNormal(V3 center_point, V3 point);
B32  Aabb3Eq(V3 a_center, V3 a_size, V3 b_center, V3 b_size);
B32  Aabb3ApproxEq(V3 a_center, V3 a_size, V3 b_center, V3 b_size);
void Aabb3RotateAroundPoint(V3* aabb_center, V3 point, M3 rot);
void Aabb3FromMinMax(V3* aabb_center, V3* aabb_size, V3 min, V3 max);
void Aabb3GetMinMax(V3 aabb_center, V3 aabb_size, V3* min, V3* max);
void Aabb3GetEnclosingSphere3(V3 aabb_center, V3 aabb_size, F32* sphere_radius);
B32  Aabb3ContainsPoint(V3 aabb_center, V3 aabb_size, V3 point);
B32  Aabb3IntersectLine3(V3 aabb_center, V3 aabb_size, V3 line_start, V3 line_end, V3* enter_point, V3* exit_point);
B32  Aabb3IntersectRay3(V3 aabb_center, V3 aabb_size, V3 ray_start, V3 ray_dir, V3* enter_point, V3* exit_point);
B32  Aabb3IntersectAabb3(V3 a_center, V3 a_size, V3 b_center, V3 b_size, IntersectManifold3* manifold);
B32  Aabb3IntersectSphere3(V3 aabb_center, V3 aabb_size, V3 sphere_center, F32 sphere_radius, IntersectManifold3* manifold);
B32  Aabb3IntersectConvexHull3(V3 aabb_center, V3 aabb_size, V3* hull_points, U32 hull_points_size, V3 hull_offset, IntersectManifold3* manifold);
// TODO: intersect plane etc

B32  Sphere3Validate(F32 sphere_radius);
B32  Sphere3Eq(V3 a_center, F32 a_radius, V3 b_center, F32 b_radius);
B32  Sphere3ApproxEq(V3 a_center, F32 a_radius, V3 b_center, F32 b_radius);
void Sphere3RotateAroundPoint(V3* sphere_center, V3 point, M3 rot);
void Sphere3GetEnclosingAabb3(F32 sphere_radius, V3* aabb_size);
B32  Sphere3ContainsPoint(V3 sphere_center, F32 sphere_radius, V3 point);
B32  Sphere3IntersectLine3(V3 sphere_center, F32 sphere_radius, V3 line_start, V3 line_end, V3* enter_point, V3* exit_point);
B32  Sphere3IntersectRay3(V3 sphere_center, F32 sphere_radius, V3 ray_start, V3 ray_dir, V3* enter_point, V3* exit_point);
B32  Sphere3IntersectSphere3(V3 a_center, F32 a_radius, V3 b_center, F32 b_radius, IntersectManifold3* manifold);
B32  Sphere3IntersectAabb3(V3 sphere_center, F32 sphere_radius, V3 aabb_center, V3 aabb_size, IntersectManifold3* manifold);
B32  Sphere3IntersectConvexHull3(V3 sphere_center, F32 sphere_radius, V3* hull_points, U32 hull_points_size, V3 hull_offset, IntersectManifold3* manifold);
// TODO: intersect plane etc

// TODO: triangulate given flattened convex hull. quickhull
B32  ConvexHull3Validate(U32 hull_points_size);
B32  ConvexHull3Eq(V3* a_points, U32 a_points_size, U32* a_indices, U32 a_indices_size, V3* b_points, U32 b_points_size, U32* b_indices, U32 b_indices_size);
B32  ConvexHull3ApproxEq(V3* a_points, U32 a_points_size, U32* a_indices, U32 a_indices_size, V3* b_points, U32 b_points_size, U32* b_indices, U32 b_indices_size);
void ConvexHull3Offset(V3* hull_points, U32 hull_points_size, V3 offset);
void ConvexHull3RotateAroundPoint(V3* hull_points, U32 hull_points_size, V3 point, M3 rot);
void ConvexHull3Flatten(Arena* arena, V3* src_points, U32 src_points_size, V3** dest_points, U32* dest_points_size); // NOTE: removes duplicate points
void ConvexHull3FromAabb3(V3 hull_points[8], U32 hull_indices[36], V3 aabb_center, V3 aabb_size);
V3   ConvexHull3GetCenter(V3* hull_points, U32 hull_points_size);
void ConvexHull3SetCenter(V3* hull_points, U32 hull_points_size, V3 center);
void ConvexHull3GetEnclosingAabb3(V3* hull_points, U32 hull_points_size, V3 hull_offset, V3* aabb_center, V3* aabb_size);
void ConvexHull3GetEnclosingSphere3(V3* hull_points, U32 hull_points_size, V3 hull_offset, V3* sphere_center, F32* sphere_radius);
B32  ConvexHull3ContainsPoint(V3* hull_points, U32 hull_points_size, U32* hull_indices, U32 hull_indices_size, V3 hull_offset, V3 point);
B32  ConvexHull3IntersectLine3(V3* hull_points, U32 hull_points_size, U32* hull_indices, U32 hull_indices_size, V3 hull_offset, V3 line_start, V3 line_end, V3* enter_point, V3* exit_point);
B32  ConvexHull3IntersectRay3(V3* hull_points, U32 hull_points_size, U32* hull_indices, U32 hull_indices_size, V3 hull_offset, V3 ray_start, V3 ray_dir, V3* enter_point, V3* exit_point);
B32  ConvexHull3IntersectAabb3(V3* hull_points, U32 hull_points_size, V3 hull_offset, V3 aabb_center, V3 aabb_size, IntersectManifold3* manifold);
B32  ConvexHull3IntersectSphere3(V3* hull_points, U32 hull_points_size, V3 hull_offset, V3 sphere_center, F32 sphere_radius, IntersectManifold3* manifold);
B32  ConvexHull3IntersectConvexHull3(V3* a_points, U32 a_points_size, V3 a_offset, V3* b_points, U32 b_points_size, V3 b_offset, IntersectManifold3* manifold);

B32  Mesh3Validate(U32 mesh_points_size, U32 indices_size);
B32  Mesh3Eq(V3* a_points, U32 a_points_size, U32* a_indices, U32 a_indices_size, V3* b_points, U32 b_points_size, U32* b_indices, U32 b_indices_size);
B32  Mesh3ApproxEq(V3* a_points, U32 a_points_size, U32* a_indices, U32 a_indices_size, V3* b_points, U32 b_points_size, U32* b_indices, U32 b_indices_size);
void Mesh3Offset(V3* mesh_points, U32 mesh_points_size, V3 offset);
void Mesh3RotateAroundPoint(V3* mesh_points, U32 mesh_points_size, V3 point, M3 rot);
void Mesh3FromAabb3(V3 mesh_points[8], U32 mesh_indices[36], V3 aabb_center, V3 aabb_size);
V3   Mesh3GetCenter(V3* mesh_points, U32 mesh_points_size);
void Mesh3SetCenter(V3* mesh_points, U32 mesh_points_size, V3 center);
void Mesh3GetEnclosingAabb3(V3* mesh_points, U32 mesh_points_size, V3 mesh_offset, V3* aabb_center, V3* aabb_size);
void Mesh3GetEnclosingSphere3(V3* mesh_points, U32 mesh_points_size, V3 mesh_offset, V3* sphere_center, F32* sphere_radius);
B32  Mesh3IntersectLine3(V3* mesh_points, U32 mesh_points_size, U32* mesh_indices, U32 mesh_indices_size, V3 mesh_offset, V3 line_start, V3 line_end, V3* enter_point, V3* exit_point);
B32  Mesh3IntersectRay3(V3* mesh_points, U32 mesh_points_size, U32* mesh_indices, U32 mesh_indices_size, V3 mesh_offset, V3 ray_start, V3 ray_dir, V3* enter_point, V3* exit_point);

// TODO: OBB3
// TODO: capsule

// NOTE: The GJK implementation is exposed in the event that you want to define other shapes outside of what is supported here.
// GjkSupportContext should be constructed and passed into the GjkIntersection3(...). For example:
//
// GjkSupportParamsConvexHull3 a_params = { ... };
// GjkSupportContext a_context = { GjkSupportConvexHull3, &a_params };
// GjkSupportParamsConvexHull3 b_params = { ... };
// GjkSupportContext b_context = { GjkSupportConvexHull3, &b_params };
// GjkIntersection3(&a_contex, &b_context, &manifold);

// NOTE: The support function should find the maximal vertex in the shape along the given direction.
typedef V3 GjkSupport_Fn(void* user_data, V3 search_dir);
// NOTE user_data is passed directly into the provided support_fn. It is intended for you to define your own struct / params to pass
// necessary shape data in to achieve the support fn's goals.
typedef struct GjkSupportContext GjkSupportContext;
struct GjkSupportContext { GjkSupport_Fn* support_fn; void* user_data; };

typedef struct GjkSupportParamsConvexHull3 GjkSupportParamsConvexHull3;
struct GjkSupportParamsConvexHull3 { V3* points; U32 points_size; V3 offset; };
V3 GjkSupportConvexHull3(void* user_data, V3 search_dir);

typedef struct GjkSupportParamsSphere3 GjkSupportParamsSphere3;
struct GjkSupportParamsSphere3 { V3 center; F32 radius; };
V3 GjkSupportSphere3(void* user_data, V3 search_dir);

// NOTE: GJK implementation for 3D convex shape intersection. Uses EPA to determine intersection manifolds.
// TODO: allow a cached simplex to be passed in.
B32 GjkIntersection3(GjkSupportContext* a, GjkSupportContext* b, IntersectManifold3* manifold);

#endif // CDEFAULT_GEOMETRY_H_

#ifdef CDEFAULT_GEOMETRY_IMPLEMENTATION
#undef CDEFAULT_GEOMETRY_IMPLEMENTATION

B32 Line2Eq(V2 a_start, V2 a_end, V2 b_start, V2 b_end) {
  return V2Eq(a_start, b_start) && V2Eq(a_end, b_end);
}

B32 Line2ApproxEq(V2 a_start, V2 a_end, V2 b_start, V2 b_end) {
  return V2ApproxEq(a_start, b_start) && V2ApproxEq(a_end, b_end);
}

void Line2Offset(V2* start, V2* end, V2 offset) {
  *start = V2AddV2(*start, offset);
  *end   = V2AddV2(*end, offset);
}

void Line2RotateAroundPoint(V2* start, V2* end, V2 point, F32 angle_rad) {
  F32 s = F32Sin(angle_rad);
  F32 c = F32Cos(angle_rad);
  *start = V2RotateAroundV2Ex(*start, point, s, c);
  *end   = V2RotateAroundV2Ex(*end, point, s, c);
}

B32 Line2ContainsPoint(V2 start, V2 end, V2 point) {
  if (V2Eq(start, end)) { return V2Eq(start, point); }
  V2 line     = V2SubV2(end, start);
  V2 line_dir = V2Normalize(line);
  if (!Ray2ContainsPoint(start, line_dir, point)) { return false; }
  if (V2LengthSq(line) < V2LengthSq(V2SubV2(point, start))) { return false; }
  return true;
}

V2 Line2GetMidpoint(V2 start, V2 end) {
  return V2DivF32(V2AddV2(start, end), 2);
}

F32 Line2GetLength(V2 start, V2 end) {
  return F32Sqrt(Line2GetLengthSq(start, end));
}

F32 Line2GetLengthSq(V2 start, V2 end) {
  return V2LengthSq(V2SubV2(end, start));
}

V2 Line2GetDir(V2 start, V2 end) {
  return V2Normalize(V2SubV2(end, start));
}

V2 Line2GetClosestPoint(V2 start, V2 end, V2 point) {
  V2 ab = V2SubV2(end, start);
  V2 ap = V2SubV2(point, start);
  F32 t = V2DotV2(ap, ab) / V2LengthSq(ab);
  if (t < 0)      { return start;                            }
  else if (t > 1) { return end;                              }
  else            { return V2AddV2(start, V2MultF32(ab, t)); }
}

V2 Line2GetNormalIn(V2 start, V2 end) {
  V2 delta = V2SubV2(end, start);
  return V2Normalize(V2Assign(-delta.y, delta.x));
}

V2 Line2GetNormalOut(V2 start, V2 end) {
  V2 delta = V2SubV2(end, start);
  return V2Normalize(V2Assign(delta.y, -delta.x));
}

B32 Line2IntersectLine2(V2 a_start, V2 a_end, V2 b_start, V2 b_end, V2* intersect_point) {
  if (UNLIKELY(Line2GetLengthSq(b_start, b_end) == 0)) { return false; }
  V2 b_dir = Line2GetDir(b_start, b_end);

  V2 i;
  if (!Line2IntersectRay2(a_start, a_end, b_start, b_dir, &i)) { return false; }
  if (Line2GetLengthSq(b_start, b_end) < Line2GetLengthSq(b_start, i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Line2IntersectRay2(V2 line_start, V2 line_end, V2 ray_start, V2 ray_dir, V2* intersect_point) {
  if (UNLIKELY(Line2GetLengthSq(line_start, line_end) == 0)) { return false; }
  V2 line_dir = Line2GetDir(line_start, line_end);

  V2 i;
  if (!Ray2IntersectRay2(line_start, line_dir, ray_start, ray_dir, &i)) { return false; }
  if (Line2GetLengthSq(line_start, line_end) < Line2GetLengthSq(line_start, i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Line2IntersectTri2(V2 line_start, V2 line_end, V2 tri_points[3], V2 tri_offset, V2* enter_point, V2* exit_point) {
  if (UNLIKELY(Line2GetLengthSq(line_start, line_end) == 0)) { return false; }
  V2 line_dir = Line2GetDir(line_start, line_end);

  V2 enter, exit;
  if (!Ray2IntersectTri2(line_start, line_dir, tri_points, tri_offset, &enter, &exit)) { return false; }

  F32 length_sq = Line2GetLengthSq(line_start, line_end);
  if (length_sq < Line2GetLengthSq(line_start, enter)) { return false; }
  if (length_sq < Line2GetLengthSq(line_start, exit))  { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL)  { *exit_point = exit;   }
  return true;
}

B32 Line2IntersectAabb2(V2 line_start, V2 line_end, V2 aabb_center, V2 aabb_size, V2* enter_point, V2* exit_point) {
  if (UNLIKELY(Line2GetLengthSq(line_start, line_end) == 0)) { return false; }
  V2 line_dir = Line2GetDir(line_start, line_end);

  V2 enter, exit;
  if (!Ray2IntersectAabb2(line_start, line_dir, aabb_center, aabb_size, &enter, &exit)) { return false; }

  F32 length_sq = Line2GetLengthSq(line_start, line_end);
  if (length_sq < Line2GetLengthSq(line_start, enter)) { return false; }
  if (length_sq < Line2GetLengthSq(line_start, exit))  { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL) { *exit_point = exit; }
  return true;
}

B32 Line2IntersectObb2(V2 line_start, V2 line_end, V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2* enter_point, V2* exit_point) {
  return Obb2IntersectLine2(obb_center, obb_size, obb_angle_rad, line_start, line_end, enter_point, exit_point);
}

B32 Line2IntersectCircle2(V2 line_start, V2 line_end, V2 circle_center, F32 circle_radius, V2* enter_point, V2* exit_point) {
  F32 length_sq = Line2GetLengthSq(line_start, line_end);
  if (UNLIKELY(length_sq == 0)) { return false; }
  V2 line_dir = Line2GetDir(line_start, line_end);

  V2 enter, exit;
  if (!Ray2IntersectCircle2(line_start, line_dir, circle_center, circle_radius, &enter, &exit)) { return false; }

  if (length_sq < Line2GetLengthSq(line_start, enter)) { return false; }
  if (length_sq < Line2GetLengthSq(line_start, exit))  { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL)  { *exit_point = exit;   }
  return true;
}

B32 Line2IntersectConvexHull2(V2 line_start, V2 line_end, V2* hull_points, U32 hull_points_size, V2 hull_offset, V2* enter_point, V2* exit_point) {
  if (UNLIKELY(Line2GetLengthSq(line_start, line_end) == 0)) { return false; }
  V2 line_dir = Line2GetDir(line_start, line_end);;

  V2 enter, exit;
  if (!Ray2IntersectConvexHull2(line_start, line_dir, hull_points, hull_points_size, hull_offset, &enter, &exit)) { return false; }

  F32 length_sq = Line2GetLengthSq(line_start, line_end);
  if (length_sq < Line2GetLengthSq(line_start, enter)) { return false; }
  if (length_sq < Line2GetLengthSq(line_start, exit))  { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL)  { *exit_point = exit; }
  return true;
}

B32 Ray2Validate(V2 ray_dir) {
  return F32ApproxEq(V2LengthSq(ray_dir), 1);
}

B32 Ray2Eq(V2 a_start, V2 a_dir, V2 b_start, V2 b_dir) {
  DEBUG_ASSERT(Ray2Validate(a_dir) && Ray2Validate(b_dir));
  return V2Eq(a_start, b_start) && V2Eq(a_dir, b_dir);
}

B32 Ray2ApproxEq(V2 a_start, V2 a_dir, V2 b_start, V2 b_dir) {
  DEBUG_ASSERT(Ray2Validate(a_dir) && Ray2Validate(b_dir));
  return V2ApproxEq(a_start, b_start) && V2ApproxEq(a_dir, b_dir);
}

void Ray2RotateAroundPoint(V2* ray_start, V2* ray_dir, V2 point, F32 angle_rad) {
  DEBUG_ASSERT(Ray2Validate(*ray_dir));
  F32 s = F32Sin(angle_rad);
  F32 c = F32Cos(angle_rad);
  *ray_start = V2RotateAroundV2Ex(*ray_start, point, s, c);
  *ray_dir   = V2RotateAroundV2Ex(*ray_dir, V2_ZEROES, s, c);
}

B32 Ray2ContainsPoint(V2 ray_start, V2 ray_dir, V2 point) {
  DEBUG_ASSERT(Ray2Validate(ray_dir));
  V2 to_point = V2SubV2(point, ray_start);
  if (!F32ApproxEq(V2CrossV2(to_point, ray_dir), 0)) { return false; }
  if (V2DotV2(to_point, ray_dir) < 0)                { return false; }
  return true;
}

V2 Ray2GetDirInv(V2 ray_dir) {
  DEBUG_ASSERT(Ray2Validate(ray_dir));
  return V2Assign(1.0f / ray_dir.x, 1.0f / ray_dir.y);
}

B32 Ray2IntersectLine2(V2 ray_start, V2 ray_dir, V2 line_start, V2 line_end, V2* intersect_point) {
  DEBUG_ASSERT(Ray2Validate(ray_dir));
  V2 line_dir = Line2GetDir(line_start, line_end);

  V2 i;
  if (!Ray2IntersectRay2(ray_start, ray_dir, line_start, line_dir, &i))         { return false; }
  if (Line2GetLengthSq(line_start, line_end) < Line2GetLengthSq(line_start, i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Ray2IntersectRay2(V2 a_start, V2 a_dir, V2 b_start, V2 b_dir, V2* intersect_point) {
  DEBUG_ASSERT(Ray2Validate(a_dir) && Ray2Validate(b_dir));
  F32 dir_cross = V2CrossV2(b_dir, a_dir);
  if (F32ApproxEq(dir_cross, 0)) {
    // NOTE: parallel or collinear
    V2 d_start      = V2SubV2(b_start, a_start);
    F32 start_cross = V2CrossV2(d_start, a_dir);
    if (!F32ApproxEq(start_cross, 0)) { return false; }

    // NOTE: determine if there's overlap on collinear intersection
    F32 t = +V2DotV2(a_dir, d_start);
    F32 u = -V2DotV2(b_dir, d_start);
    if (t < 0 && u < 0) { return false; }
    if (intersect_point != NULL) {
      if (u > 0) { *intersect_point = a_start; }
      else       { *intersect_point = b_start; }
    }
    return true;
  } else {
    F32 d_x = b_start.x - a_start.x;
    F32 d_y = b_start.y - a_start.y;
    F32 u = ((d_y * b_dir.x) - (d_x * b_dir.y)) / dir_cross;
    F32 v = ((d_y * a_dir.x) - (d_x * a_dir.y)) / dir_cross;
    if (u < 0 || v < 0) { return false; }
    if (intersect_point != NULL) { *intersect_point = V2AddV2(a_start, V2MultF32(a_dir, u)); }
    return true;
  }
}

B32 Ray2IntersectTri2(V2 ray_start, V2 ray_dir, V2 tri_points[3], V2 tri_offset, V2* enter_point, V2* exit_point) {
  return Ray2IntersectConvexHull2(ray_start, ray_dir, (V2*) tri_points, 3, tri_offset, enter_point, exit_point);
}

B32 Ray2IntersectAabb2(V2 ray_start, V2 ray_dir, V2 aabb_center, V2 aabb_size, V2* enter_point, V2* exit_point) {
  DEBUG_ASSERT(Ray2Validate(ray_dir));
  V2 aabb_min, aabb_max;
  Aabb2GetMinMax(aabb_center, aabb_size, &aabb_min, &aabb_max);

  F32 t_min = F32_MIN;
  F32 t_max = F32_MAX;
  for (U32 i = 0; i < 2; i++) {
    if (ray_dir.e[i] == 0) {
      // NOTE: parallel
      if (ray_start.e[i] < aabb_min.e[i] || ray_start.e[i] > aabb_max.e[i]) {
        return false;
      }
    } else {
      F32 t_0 = (aabb_min.e[i] - ray_start.e[i]) / ray_dir.e[i];
      F32 t_1 = (aabb_max.e[i] - ray_start.e[i]) / ray_dir.e[i];
      if (t_0 > t_1) { SWAP(F32, t_0, t_1); }
      t_min = MAX(t_min, t_0);
      t_max = MIN(t_max, t_1);
    }
  }
  if (t_max < t_min) { return false; }

  if (t_min < 0) {
    if (t_max < 0) { return false; }
    t_min = t_max;
  }

  if (enter_point != NULL) { *enter_point = V2AddV2(ray_start, V2MultF32(ray_dir, t_min)); }
  if (exit_point != NULL)  { *exit_point  = V2AddV2(ray_start, V2MultF32(ray_dir, t_max)); }
  return true;
}

B32 Ray2IntersectObb2(V2 ray_start, V2 ray_dir, V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2* enter_point, V2* exit_point) {
  return Obb2IntersectRay2(obb_center, obb_size, obb_angle_rad, ray_start, ray_dir, enter_point, exit_point);
}

B32 Ray2IntersectCircle2(V2 ray_start, V2 ray_dir, V2 circle_center, F32 circle_radius, V2* enter_point, V2* exit_point) {
  DEBUG_ASSERT(Ray2Validate(ray_dir));
  V2 rel_center        = V2SubV2(circle_center, ray_start);
  F32 to_perp_center   = V2DotV2(rel_center, ray_dir);
  F32 d2               = V2LengthSq(rel_center) - (to_perp_center * to_perp_center);
  F32 circle_radius_sq = circle_radius * circle_radius;
  if (d2 > circle_radius_sq) { return false; }
  if (enter_point == NULL && exit_point == NULL) { return true; }
  F32 center_to_edge = F32Sqrt(circle_radius_sq - d2);
  F32 t0 = to_perp_center - center_to_edge;
  F32 t1 = to_perp_center + center_to_edge;
  if (t1 < t0) { SWAP(F32, t0, t1); }
  if (t0 < 0) {
    if (t1 < 0) { return false; }
    t0 = t1;
  }
  if (enter_point != NULL) { *enter_point = V2AddV2(ray_start, V2MultF32(ray_dir, t0)); }
  if (exit_point != NULL)  { *exit_point  = V2AddV2(ray_start, V2MultF32(ray_dir, t1)); }
  return true;
}

B32 Ray2IntersectConvexHull2(V2 ray_start, V2 ray_dir, V2* hull_points, U32 hull_points_size, V2 hull_offset, V2* enter_point, V2* exit_point) {
  DEBUG_ASSERT(Ray2Validate(ray_dir));
  B32 result = false;
  F32 min_sq = F32_MAX;
  F32 max_sq = F32_MIN;
  for (U32 i = 0; i < hull_points_size; i++) {
    V2 hull_a = V2AddV2(hull_points[i], hull_offset);
    V2 hull_b = V2AddV2(hull_points[(i + 1) % hull_points_size], hull_offset);
    V2 intersect;
    if (!Ray2IntersectLine2(ray_start, ray_dir, hull_a, hull_b, &intersect)) {
      continue;
    }
    result = true;
    if (enter_point == NULL && exit_point == NULL) { break; }

    F32 length_sq = Line2GetLengthSq(ray_start, intersect);
    if ((enter_point != NULL) && (length_sq < min_sq)) {
      min_sq = length_sq;
      *enter_point = intersect;
    }
    if ((exit_point != NULL) && (length_sq > max_sq)) {
      max_sq = length_sq;
      *exit_point = intersect;
    }
  }
  return result;
}

B32 Tri2Validate(V2 tri_points[3]) {
  return !V2Eq(tri_points[0], tri_points[1]) &&
         !V2Eq(tri_points[0], tri_points[2]) &&
         !V2Eq(tri_points[1], tri_points[2]);
}

B32 Tri2Eq(V2 a_points[3], V2 b_points[3]) {
  DEBUG_ASSERT(Tri2Validate(a_points) && Tri2Validate(b_points));
  return ConvexHull2Eq(a_points, 3, b_points, 3);
}

B32 Tri2ApproxEq(V2 a_points[3], V2 b_points[3]) {
  DEBUG_ASSERT(Tri2Validate(a_points) && Tri2Validate(b_points));
  return ConvexHull2ApproxEq(a_points, 3, b_points, 3);
}

void Tri2Offset(V2 tri_points[3], V2 offset) {
  ConvexHull2Offset(tri_points, 3, offset);
}

void Tri2RotateAroundPoint(V2 tri_points[3], V2 point, F32 angle_rad) {
  ConvexHull2RotateAroundPoint(tri_points, 3, point, angle_rad);
}

void Tri2SetCenter(V2 tri_points[3], V2 center) {
  ConvexHull2SetCenter(tri_points, 3, center);
}

V2 Tri2GetCenter(V2 tri_points[3]) {
  return ConvexHull2GetCenter((V2*) tri_points, 3);
}

void Tri2GetEnclosingCircle2(V2 tri_points[3], V2 tri_offset, V2* circle_center, F32* circle_radius) {
  ConvexHull2GetEnclosingCircle2((V2*) tri_points, 3, tri_offset, circle_center, circle_radius);
}

void Tri2GetEnclosingAabb2(V2 tri_points[3], V2 tri_offset, V2* aabb_center, V2* aabb_size) {
  ConvexHull2GetEnclosingAabb2((V2*) tri_points, 3, tri_offset, aabb_center, aabb_size);
}

B32 Tri2ContainsPoint(V2 tri_points[3], V2 tri_offset, V2 point) {
  return ConvexHull2ContainsPoint((V2*) tri_points, 3, tri_offset, point);
}

B32 Tri2IntersectLine2(V2 tri_points[3], V2 tri_offset, V2 line_start, V2 line_end, V2* enter_point, V2* exit_point) {
  return Line2IntersectTri2(line_start, line_end, tri_points, tri_offset, enter_point, exit_point);
}

B32 Tri2IntersectRay2(V2 tri_points[3], V2 tri_offset, V2 ray_start, V2 ray_dir, V2* enter_point, V2* exit_point) {
  return Ray2IntersectTri2(ray_start, ray_dir, tri_points, tri_offset, enter_point, exit_point);
}

B32 Tri2IntersectTri2(V2 a_points[3], V2 a_offset, V2 b_points[3], V2 b_offset, IntersectManifold2* manifold) {
  return ConvexHull2IntersectConvexHull2((V2*) a_points, 3, a_offset, (V2*) b_points, 3, b_offset, manifold);
}

B32 Tri2IntersectAabb2(V2 tri_points[3], V2 tri_offset, V2 aabb_center, V2 aabb_size, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectConvexHull2((V2*) tri_points, 3, tri_offset, aabb_points, 4, V2_ZEROES, manifold);
}

B32 Tri2IntersectObb2(V2 tri_points[3], V2 tri_offset, V2 obb_center, V2 obb_size, F32 obb_angle_rad, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectConvexHull2((V2*) tri_points, 3, tri_offset, (V2*) obb_points, 4, V2_ZEROES, manifold);
}

B32 Tri2IntersectCircle2(V2 tri_points[3], V2 tri_offset, V2 circle_center, F32 circle_radius, IntersectManifold2* manifold) {
  return ConvexHull2IntersectCircle2((V2*) tri_points, 3, tri_offset, circle_center, circle_radius, manifold);
}

B32 Tri2IntersectConvexHull2(V2 tri_points[3], V2 tri_offset, V2* hull_points, U32 hull_points_size, V2 hull_offset, IntersectManifold2* manifold) {
  return ConvexHull2IntersectConvexHull2((V2*) tri_points, 3, tri_offset, hull_points, hull_points_size, hull_offset, manifold);
}

B32 Aabb2Validate(V2 aabb_size) {
  return aabb_size.x >= 0 && aabb_size.y >= 0;
}

B32 Aabb2Eq(V2 a_center, V2 a_size, V2 b_center, V2 b_size) {
  DEBUG_ASSERT(Aabb2Validate(a_size) && Aabb2Validate(b_size));
  return V2Eq(a_center, b_center) && V2Eq(a_size, b_size);
}

B32 Aabb2ApproxEq(V2 a_center, V2 a_size, V2 b_center, V2 b_size) {
  DEBUG_ASSERT(Aabb2Validate(a_size) && Aabb2Validate(b_size));
  return V2ApproxEq(a_center, b_center) && V2ApproxEq(a_size, b_size);
}

void Aabb2FromMinMax(V2* aabb_center, V2* aabb_size, V2 min, V2 max) {
  DEBUG_ASSERT(min.x < max.x);
  DEBUG_ASSERT(min.y < max.y);
  *aabb_center = V2DivF32(V2AddV2(min, max), 2);
  *aabb_size   = V2SubV2(max, min);
}

void Aabb2FromTopLeft(V2* aabb_center, V2 top_left_point, V2 size) {
  V2 h_size = V2DivF32(size, 2);
  aabb_center->x = top_left_point.x + h_size.x;
  aabb_center->y = top_left_point.y - h_size.y;
}

void Aabb2GetMinMax(V2 aabb_center, V2 aabb_size, V2* min, V2* max) {
  V2 h_size = V2DivF32(aabb_size, 2.0f);
  *min = V2SubV2(aabb_center, h_size);
  *max = V2AddV2(aabb_center, h_size);
}

void Aabb2GetEnclosingCircle2(V2 aabb_center, V2 aabb_size, F32* circle_radius) {
  V2 min, max;
  Aabb2GetMinMax(aabb_center, aabb_size, &min, &max);
  *circle_radius = V2Length(V2SubV2(max, aabb_center));
}

void Aabb2RotateAroundPoint(V2* aabb_center, V2 point, F32 angle_rad) {
  *aabb_center = V2RotateAroundV2(*aabb_center, point, angle_rad);
}

B32 Aabb2ContainsPoint(V2 aabb_center, V2 aabb_size, V2 point) {
  V2 min, max;
  Aabb2GetMinMax(aabb_center, aabb_size, &min, &max);
  return (min.x <= point.x && point.x <= max.x) &&
         (min.y <= point.y && point.y <= max.y);
}

B32 Aabb2IntersectLine2(V2 aabb_center, V2 aabb_size, V2 line_start, V2 line_end, V2* enter_point, V2* exit_point) {
  return Line2IntersectAabb2(line_start, line_end, aabb_center, aabb_size, enter_point, exit_point);
}

B32 Aabb2IntersectRay2(V2 aabb_center, V2 aabb_size, V2 ray_start, V2 ray_dir, V2* enter_point, V2* exit_point) {
  return Ray2IntersectAabb2(ray_start, ray_dir, aabb_center, aabb_size, enter_point, exit_point);
}

B32 Aabb2IntersectTri2(V2 aabb_center, V2 aabb_size, V2 tri_points[3], V2 tri_offset, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectConvexHull2((V2*) aabb_points, 4, V2_ZEROES, (V2*) tri_points, 3, tri_offset, manifold);
}

B32 Aabb2IntersectAabb2(V2 a_center, V2 a_size, V2 b_center, V2 b_size, IntersectManifold2* manifold) {
  V2 a_points[4], b_points[4];
  ConvexHull2FromAabb2(a_points, a_center, a_size);
  ConvexHull2FromAabb2(b_points, b_center, b_size);
  return ConvexHull2IntersectConvexHull2((V2*) a_points, 4, V2_ZEROES, (V2*) b_points, 4, V2_ZEROES, manifold);
}

B32 Aabb2IntersectObb2(V2 aabb_center, V2 aabb_size, V2 obb_center, V2 obb_size, F32 obb_angle_rad, IntersectManifold2* manifold) {
  V2 aabb_points[4], obb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectConvexHull2((V2*) aabb_points, 4, V2_ZEROES, (V2*) obb_points, 4, V2_ZEROES, manifold);
}

B32 Aabb2IntersectCircle2(V2 aabb_center, V2 aabb_size, V2 circle_center, F32 circle_radius, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectCircle2((V2*) aabb_points, 4, V2_ZEROES, circle_center, circle_radius, manifold);
}

B32 Aabb2IntersectConvexHull2(V2 aabb_center, V2 aabb_size, V2* hull_points, U32 hull_points_size, V2 hull_offset, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectConvexHull2(aabb_points, 4, V2_ZEROES, hull_points, hull_points_size, hull_offset, manifold);
}

B32 Obb2Validate(V2 obb_size) {
  return obb_size.x >= 0 && obb_size.y >= 0;
}

B32 Obb2Eq(V2 a_center, V2 a_size, F32 a_angle_rad, V2 b_center, V2 b_size, F32 b_angle_rad) {
  return V2Eq(a_center, b_center) &&
         V2Eq(a_size, b_size) &&
         a_angle_rad == b_angle_rad;
}

B32 Obb2ApproxEq(V2 a_center, V2 a_size, F32 a_angle_rad, V2 b_center, V2 b_size, F32 b_angle_rad) {
  return V2ApproxEq(a_center, b_center) &&
         V2ApproxEq(a_size, b_size) &&
         F32ApproxEq(a_angle_rad, b_angle_rad);
}

void Obb2RotateAroundPoint(V2* obb_center, F32* obb_angle_rad, V2 point, F32 angle_rad) {
  *obb_center = V2RotateAroundV2(*obb_center, point, angle_rad);
  *obb_angle_rad += angle_rad;
}

void Obb2GetEnclosingCircle2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, F32* circle_radius) {
  V2 temp, obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  ConvexHull2GetEnclosingCircle2((V2*) obb_points, 4, V2_ZEROES, &temp, circle_radius);
}

void Obb2GetEnclosingAabb2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2* aabb_size) {
  V2 obb_points[4], temp;
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  ConvexHull2GetEnclosingAabb2((V2*) obb_points, 4, V2_ZEROES, &temp, aabb_size);
}

B32 Obb2ContainsPoint(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 point) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2ContainsPoint((V2*) obb_points, 4, V2_ZEROES, point);
}

B32 Obb2IntersectLine2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 line_start, V2 line_end, V2* enter_point, V2* exit_point) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectLine2((V2*) obb_points, 4, V2_ZEROES, line_start, line_end, enter_point, exit_point);
}

B32 Obb2IntersectRay2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 ray_start, V2 ray_dir, V2* enter_point, V2* exit_point) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectRay2((V2*) obb_points, 4, V2_ZEROES, ray_start, ray_dir, enter_point, exit_point);
}

B32 Obb2IntersectTri2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 tri_points[3], V2 tri_offset, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectConvexHull2(obb_points, 4, V2_ZEROES, tri_points, 3, tri_offset, manifold);
}

B32 Obb2IntersectAabb2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 aabb_center, V2 aabb_size, IntersectManifold2* manifold) {
  V2 obb_points[4], aabb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectConvexHull2((V2*) obb_points, 4, V2_ZEROES, (V2*) aabb_points, 4, V2_ZEROES, manifold);
}

B32 Obb2IntersectObb2(V2 a_center, V2 a_size, F32 a_angle_rad, V2 b_center, V2 b_size, F32 b_angle_rad, IntersectManifold2* manifold) {
  V2 a_points[4], b_points[4];
  ConvexHull2FromObb2(a_points, a_center, a_size, a_angle_rad);
  ConvexHull2FromObb2(b_points, b_center, b_size, b_angle_rad);
  return ConvexHull2IntersectConvexHull2((V2*) a_points, 4, V2_ZEROES, (V2*) b_points, 4, V2_ZEROES, manifold);
}

B32 Obb2IntersectCircle2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2 circle_center, F32 circle_radius, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectCircle2((V2*) obb_points, 4, V2_ZEROES, circle_center, circle_radius, manifold);
}

B32 Obb2IntersectConvexHull2(V2 obb_center, V2 obb_size, F32 obb_angle_rad, V2* hull_points, U32 hull_points_size, V2 hull_offset, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectConvexHull2(obb_points, 4, V2_ZEROES, hull_points, hull_points_size, hull_offset, manifold);
}

B32 Circle2Validate(F32 circle_radius) {
  return circle_radius >= 0;
}

B32 Circle2Eq(V2 a_center, F32 a_radius, V2 b_center, F32 b_radius) {
  DEBUG_ASSERT(Circle2Validate(a_radius) && Circle2Validate(b_radius));
  return V2Eq(a_center, b_center) && a_radius == b_radius;
}

B32 Circle2ApproxEq(V2 a_center, F32 a_radius, V2 b_center, F32 b_radius) {
  DEBUG_ASSERT(Circle2Validate(a_radius) && Circle2Validate(b_radius));
  return V2ApproxEq(a_center, b_center) && F32ApproxEq(a_radius, b_radius);
}

void Circle2GetEnclosingAabb2(F32 circle_radius, V2* aabb_size) {
  *aabb_size = V2Splat(circle_radius * 2);
}

B32 Circle2ContainsPoint(V2 circle_center, F32 circle_radius, V2 point) {
  return V2LengthSq(V2SubV2(point, circle_center)) <= (circle_radius * circle_radius);
}

void Circle2RotateAroundPoint(V2* circle_center, V2 point, F32 angle_rad) {
  *circle_center = V2RotateAroundV2(*circle_center, point, angle_rad);
}

B32 Circle2IntersectLine2(V2 circle_center, F32 circle_radius, V2 line_start, V2 line_end, V2* enter_point, V2* exit_point) {
  return Line2IntersectCircle2(line_start, line_end, circle_center, circle_radius, enter_point, exit_point);
}

B32 Circle2IntersectRay2(V2 circle_center, F32 circle_radius, V2 ray_start, V2 ray_dir, V2* enter_point, V2* exit_point) {
  return Ray2IntersectCircle2(ray_start, ray_dir, circle_center, circle_radius, enter_point, exit_point);
}

B32 Circle2IntersectTri2(V2 circle_center, F32 circle_radius, V2 tri_points[3], V2 tri_offset, IntersectManifold2* manifold) {
  return Circle2IntersectConvexHull2(circle_center, circle_radius, (V2*) tri_points, 3, tri_offset, manifold);
}

B32 Circle2IntersectAabb2(V2 circle_center, F32 circle_radius, V2 aabb_center, V2 aabb_size, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return Circle2IntersectConvexHull2(circle_center, circle_radius, (V2*) aabb_points, 4, V2_ZEROES, manifold);
}

B32 Circle2IntersectObb2(V2 circle_center, F32 circle_radius, V2 obb_center, V2 obb_size, F32 obb_angle_rad, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return Circle2IntersectConvexHull2(circle_center, circle_radius, (V2*) obb_points, 4, V2_ZEROES, manifold);
}

B32 Circle2IntersectCircle2(V2 a_center, F32 a_radius, V2 b_center, F32 b_radius, IntersectManifold2* manifold) {
  V2 center_diff   = V2SubV2(a_center, b_center);
  F32 min_distance = a_radius + b_radius;
  F32 distance     = V2Length(center_diff);
  if (min_distance < distance) { return false; }
  if (manifold != NULL) {
    if (distance == 0) { center_diff.y = 1.0f; }
    manifold->normal              = V2Normalize(center_diff);
    manifold->penetration         = min_distance - distance;
    manifold->contact_points[0]   = V2AddV2(a_center, V2MultF32(manifold->normal, -a_radius));
    manifold->contact_points_size = 1;
  }
  return true;
}

static void Circle2ProjectAxis(V2 circle_center, F32 circle_radius, V2 axis, F32* min, F32* max) {
  F32 circle_center_proj = V2DotV2(circle_center, axis);
  *min = circle_center_proj - circle_radius;
  *max = circle_center_proj + circle_radius;
}

static void ConvexHull2ProjectAxis(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 axis, F32* min, F32* max) {
  F32 proj = V2DotV2(V2AddV2(hull_points[0], hull_offset), axis);
  *min = proj;
  *max = proj;
  for (U32 i = 1; i < hull_points_size; i++) {
    V2 hull_point = V2AddV2(hull_points[i], hull_offset);
    proj = V2DotV2(hull_point, axis);
    if (proj < *min) { *min = proj; }
    if (proj > *max) { *max = proj; }
  }
}

// NOTE: this is the same as ConvexHull2IntersectCircle2 with different manifold calculations.
B32 Circle2IntersectConvexHull2(V2 circle_center, F32 circle_radius, V2* hull_points, U32 hull_points_size, V2 hull_offset, IntersectManifold2* manifold) {
  V2 hull_center = ConvexHull2GetCenter(hull_points, hull_points_size);
  V2 rel_center = V2SubV2(hull_center, circle_center);

  F32 min_penetration = F32_MAX;
  for (U32 i = 0; i < hull_points_size; i++) {
    V2 start = V2AddV2(hull_points[i], hull_offset);
    V2 end   = V2AddV2(hull_points[(i + 1) % hull_points_size], hull_offset);
    DEBUG_ASSERT(Line2GetLengthSq(start, end) > 0);
    V2 axis  = Line2GetNormalOut(start, end);

    F32 hull_min, hull_max, circle_min, circle_max;
    ConvexHull2ProjectAxis(hull_points, hull_points_size, hull_offset, axis, &hull_min, &hull_max);
    Circle2ProjectAxis(circle_center, circle_radius, axis, &circle_min, &circle_max);
    F32 hull_overlap = MIN(hull_max, circle_max) - MAX(hull_min, circle_min);
    if (hull_overlap <= 0) { return false; }

    V2 to_circle = V2Normalize(V2SubV2(hull_points[i], circle_center));
    ConvexHull2ProjectAxis(hull_points, hull_points_size, hull_offset, to_circle, &hull_min, &hull_max);
    Circle2ProjectAxis(circle_center, circle_radius, to_circle, &circle_min, &circle_max);
    F32 circle_overlap = MIN(hull_max, circle_max) - MAX(hull_min, circle_min);
    if (circle_overlap <= 0) { return false; }

    if (manifold != NULL && hull_overlap < min_penetration) {
      min_penetration       = hull_overlap;
      manifold->penetration = hull_overlap;
      manifold->normal      = axis;
    }
  }
  if (manifold != NULL) {
    if (V2DotV2(rel_center, manifold->normal) > 0) { manifold->normal = V2Negate(manifold->normal); }
    manifold->normal              = V2Normalize(manifold->normal);
    manifold->contact_points[0]   = V2AddV2(circle_center, V2MultF32(manifold->normal, -circle_radius));
    manifold->contact_points_size = 1;
  }
  return true;
}

// NOTE: Finds the furthest vertex and edge in the given convex hull along the provided normal.
static void ConvexHull2MaxFeatureAlongNormal(
    V2* points, U32 points_size, V2 center, V2 normal,
    V2** feature, V2** start, V2** end) {
  U32 max_feature_idx = 0;
  F32 max_proj        = 0;
  for (U32 i = 0; i < points_size; i++) {
    F32 proj = V2DotV2(normal, V2SubV2(points[i], center));
    if (proj > max_proj) {
      max_proj = proj;
      max_feature_idx = i;
    }
  }

  *feature        = &points[max_feature_idx];
  V2* left_start  = &points[(max_feature_idx + points_size - 1) % points_size];
  V2* right_start = &points[(max_feature_idx + 1) % points_size];
  V2 left         = V2Normalize(V2SubV2(**feature, *left_start));
  V2 right        = V2Normalize(V2SubV2(**feature, *right_start));

  if (V2DotV2(left, normal) <= V2DotV2(right, normal)) {
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
  F32 d_0 = V2DotV2(normal, start) - t;
  F32 d_1 = V2DotV2(normal, end)   - t;
  if (d_0 >= 0) { clipped_points[(*clipped_points_size)++] = start; }
  if (d_1 >= 0) { clipped_points[(*clipped_points_size)++] = end;   }
  // NOTE: will be negative iff d_0 and d_1 have different signs.
  if (d_0 * d_1 < 0) {
    DEBUG_ASSERT(*clipped_points_size < 2);
    V2 edge = V2SubV2(end, start);
    F32 x   = d_0 / (d_0 - d_1);
    edge = V2AddV2(start, V2MultF32(edge, x));
    clipped_points[(*clipped_points_size)++] = edge;
  }
}

B32 ConvexHull2Validate(U32 hull_points_size) {
  return hull_points_size > 2;
}

B32 ConvexHull2Eq(V2* a_points, U32 a_points_size, V2* b_points, U32 b_points_size) {
  if (a_points_size != b_points_size) { return false; }
  for (U32 i = 0; i < a_points_size; i++) {
    if (!V2Eq(a_points[i], b_points[i])) { return false; }
  }
  return true;
}

B32 ConvexHull2ApproxEq(V2* a_points, U32 a_points_size, V2* b_points, U32 b_points_size) {
  if (a_points_size != b_points_size) { return false; }
  for (U32 i = 0; i < a_points_size; i++) {
    if (!V2ApproxEq(a_points[i], b_points[i])) { return false; }
  }
  return true;
}

void ConvexHull2FromAabb2(V2 hull_points[4], V2 aabb_center, V2 aabb_size) {
  V2 min, max;
  Aabb2GetMinMax(aabb_center, aabb_size, &min, &max);
  hull_points[0] = min;
  hull_points[1] = V2Assign(max.x, min.y);
  hull_points[2] = max;
  hull_points[3] = V2Assign(min.x, max.y);
}

void ConvexHull2FromObb2(V2 hull_points[4], V2 obb_center, V2 obb_size, F32 obb_angle_rad) {
  ConvexHull2FromAabb2(hull_points, obb_center, obb_size);
  ConvexHull2RotateAroundPoint((V2*) hull_points, 4, obb_center, obb_angle_rad);
}

void ConvexHull2Offset(V2* hull_points, U32 hull_points_size, V2 offset) {
  for (U32 i = 0; i < hull_points_size; i++) {
    hull_points[i] = V2AddV2(hull_points[i], offset);
  }
}

void ConvexHull2RotateAroundPoint(V2* hull_points, U32 hull_points_size, V2 point, F32 angle_rad) {
  F32 s = F32Sin(angle_rad);
  F32 c = F32Cos(angle_rad);
  for (U32 i = 0; i < hull_points_size; i++) {
    hull_points[i] = V2RotateAroundV2Ex(hull_points[i], point, s, c);
  }
}

V2 ConvexHull2GetCenter(V2* hull_points, U32 hull_points_size) {
  V2 result = V2_ZEROES;
  for (U32 i = 0; i < hull_points_size; i++) {
    result = V2AddV2(result, hull_points[i]);
  }
  result = V2DivF32(result, hull_points_size);
  return result;
}

void ConvexHull2SetCenter(V2* hull_points, U32 hull_points_size, V2 center) {
  V2 curr_center = ConvexHull2GetCenter(hull_points, hull_points_size);
  V2 offset = V2SubV2(center, curr_center);
  ConvexHull2Offset(hull_points, hull_points_size, offset);
}

void ConvexHull2GetEnclosingCircle2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2* circle_center, F32* circle_radius) {
  *circle_center = ConvexHull2GetCenter(hull_points, hull_points_size);
  F32 max_dist = F32_MIN;
  for (U32 i = 0; i < hull_points_size; i++) {
    V2 point = V2AddV2(hull_points[i], hull_offset);
    F32 dist_sq = V2LengthSq(V2SubV2(point, *circle_center));
    if (max_dist < dist_sq) { max_dist = dist_sq; }
  }
  *circle_radius = F32Sqrt(max_dist);
}

void ConvexHull2GetEnclosingAabb2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2* aabb_center, V2* aabb_size) {
  *aabb_center = ConvexHull2GetCenter(hull_points, hull_points_size);
  F32 x_min = F32_MAX;
  F32 y_min = F32_MAX;
  F32 x_max = F32_MIN;
  F32 y_max = F32_MIN;
  for (U32 i = 0; i < hull_points_size; i++) {
    V2 to_pt = V2SubV2(V2AddV2(hull_points[i], hull_offset), *aabb_center);
    if (to_pt.x < x_min) { x_min = to_pt.x; }
    if (to_pt.y < y_min) { y_min = to_pt.y; }
    if (to_pt.x > x_max) { x_max = to_pt.x; }
    if (to_pt.y > y_max) { y_max = to_pt.y; }
  }
  aabb_size->x = x_max - x_min;
  aabb_size->y = y_max - y_min;
}

B32 ConvexHull2ContainsPoint(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 point) {
  for (U32 i = 0; i < hull_points_size; i++) {
    V2 a  = V2AddV2(hull_points[i], hull_offset);
    V2 b  = V2AddV2(hull_points[(i + 1) % hull_points_size], hull_offset);
    V2 ab = V2SubV2(b, a);
    V2 ap = V2SubV2(point, a);
    F32 cross = V2CrossV2(ab, ap);
    if (cross < 0) { return false; }
  }
  return true;
}

B32 ConvexHull2IntersectLine2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 line_start, V2 line_end, V2* enter_point, V2* exit_point) {
  return Line2IntersectConvexHull2(line_start, line_end, hull_points, hull_points_size, hull_offset, enter_point, exit_point);
}

B32 ConvexHull2IntersectRay2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 ray_start, V2 ray_dir, V2* enter_point, V2* exit_point) {
  return Ray2IntersectConvexHull2(ray_start, ray_dir, hull_points, hull_points_size, hull_offset, enter_point, exit_point);
}

B32 ConvexHull2IntersectTri2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 tri_points[3], V2 tri_offset, IntersectManifold2* manifold) {
  return ConvexHull2IntersectConvexHull2(hull_points, hull_points_size, hull_offset, (V2*) tri_points, 3, tri_offset, manifold);
}

B32 ConvexHull2IntersectAabb2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 aabb_center, V2 aabb_size, IntersectManifold2* manifold) {
  V2 aabb_points[4];
  ConvexHull2FromAabb2(aabb_points, aabb_center, aabb_size);
  return ConvexHull2IntersectConvexHull2(hull_points, hull_points_size, hull_offset, (V2*) aabb_points, 4, V2_ZEROES, manifold);
}

B32 ConvexHull2IntersectObb2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 obb_center, V2 obb_size, F32 obb_angle_rad, IntersectManifold2* manifold) {
  V2 obb_points[4];
  ConvexHull2FromObb2(obb_points, obb_center, obb_size, obb_angle_rad);
  return ConvexHull2IntersectConvexHull2(hull_points, hull_points_size, hull_offset, (V2*) obb_points, 4, V2_ZEROES, manifold);
}

// NOTE: this is the same as Circle2IntersectConvexHull2 with different manifold calculations.
B32 ConvexHull2IntersectCircle2(V2* hull_points, U32 hull_points_size, V2 hull_offset, V2 circle_center, F32 circle_radius, IntersectManifold2* manifold) {
  V2 hull_center = V2AddV2(ConvexHull2GetCenter(hull_points, hull_points_size), hull_offset);
  V2 rel_center  = V2SubV2(hull_center, circle_center);

  F32 min_penetration = F32_MAX;
  for (U32 i = 0; i < hull_points_size; i++) {
    V2 start = V2AddV2(hull_points[i], hull_offset);
    V2 end   = V2AddV2(hull_points[(i + 1) % hull_points_size], hull_offset);
    DEBUG_ASSERT(Line2GetLengthSq(start, end) > 0);
    V2 axis   = Line2GetNormalOut(start, end);

    F32 hull_min, hull_max, circle_min, circle_max;
    ConvexHull2ProjectAxis(hull_points, hull_points_size, hull_offset, axis, &hull_min, &hull_max);
    Circle2ProjectAxis(circle_center, circle_radius, axis, &circle_min, &circle_max);
    F32 hull_overlap = MIN(hull_max, circle_max) - MAX(hull_min, circle_min);
    if (hull_overlap <= 0) { return false; }

    V2 to_circle = V2Normalize(V2SubV2(hull_points[i], circle_center));
    ConvexHull2ProjectAxis(hull_points, hull_points_size, hull_offset, to_circle, &hull_min, &hull_max);
    Circle2ProjectAxis(circle_center, circle_radius, to_circle, &circle_min, &circle_max);
    F32 circle_overlap = MIN(hull_max, circle_max) - MAX(hull_min, circle_min);
    if (circle_overlap <= 0) { return false; }

    if (manifold != NULL && hull_overlap < min_penetration) {
      min_penetration       = hull_overlap;
      manifold->penetration = hull_overlap;
      manifold->normal      = axis;
    }
  }

  if (manifold != NULL) {
    if (V2DotV2(rel_center, manifold->normal) < 0) { manifold->normal = V2Negate(manifold->normal); }
    manifold->normal = V2Normalize(manifold->normal);

    V2 *i_start, *i_end, *feature;
    V2 i_normal_inv = V2Negate(manifold->normal);
    ConvexHull2MaxFeatureAlongNormal(hull_points, hull_points_size, hull_center, i_normal_inv, &feature, &i_start, &i_end);
    manifold->contact_points[0] = Line2GetClosestPoint(*i_start, *i_end, circle_center);
    manifold->contact_points_size = 1;
  }
  return true;
}

B32 ConvexHull2IntersectConvexHull2(V2* a_points, U32 a_points_size, V2 a_offset, V2* b_points, U32 b_points_size, V2 b_offset, IntersectManifold2* manifold) {
  V2 a_center   = V2AddV2(ConvexHull2GetCenter(a_points, a_points_size), a_offset);
  V2 b_center   = V2AddV2(ConvexHull2GetCenter(b_points, b_points_size), b_offset);
  V2 rel_center = V2SubV2(a_center, b_center);

  F32 min_penetration = F32_MAX;
  for (U32 shape_idx = 0; shape_idx < 2; shape_idx++) {
    V2* points      = a_points;
    U32 points_size = a_points_size;
    V2  offset      = a_offset;
    if (shape_idx == 1) {
      points      = b_points;
      points_size = b_points_size;
      offset      = b_offset;
    }

    for (U32 i = 0; i < points_size; i++) {
      V2 start = V2AddV2(points[i], offset);
      V2 end   = V2AddV2(points[(i + 1) % points_size], offset);
      DEBUG_ASSERT(Line2GetLengthSq(start, end) > 0);
      V2 axis = Line2GetNormalOut(start, end);

      F32 a_min, a_max, b_min, b_max;
      ConvexHull2ProjectAxis(a_points, a_points_size, a_offset, axis, &a_min, &a_max);
      ConvexHull2ProjectAxis(b_points, b_points_size, b_offset, axis, &b_min, &b_max);
      F32 overlap = MIN(a_max, b_max) - MAX(a_min, b_min);
      if (overlap < 0) { return false; }

      if (manifold != NULL && overlap < min_penetration) {
        min_penetration       = overlap;
        manifold->penetration = overlap;
        manifold->normal      = axis;
      }
    }
  }
  // https://dyn4j.org/2011/11/contact-points-using-clipping/
  if (manifold != NULL) {
    if (V2DotV2(rel_center, manifold->normal) < 0) { manifold->normal = V2Negate(manifold->normal); }
    manifold->normal = V2Normalize(manifold->normal);

    V2 i_normal_inv = V2Negate(manifold->normal);
    V2 *a_i_start, *a_i_end, *b_i_start, *b_i_end, *a_feature, *b_feature;
    ConvexHull2MaxFeatureAlongNormal(a_points, a_points_size, a_center, i_normal_inv, &a_feature, &a_i_start, &a_i_end);
    ConvexHull2MaxFeatureAlongNormal(b_points, b_points_size, b_center, manifold->normal, &b_feature, &b_i_start, &b_i_end);

    V2 ref_edge     = V2Normalize(V2SubV2(*b_i_end, *b_i_start));
    V2 ref_edge_inv = V2Negate(ref_edge);
    V2 ref_normal   = Line2GetNormalOut(*b_i_start, *b_i_end);

    V2 c_points[2];
    U32 c_points_size;
    F32 t_0 = +V2DotV2(ref_edge, *b_i_start);
    ConvexHull2Clip(*a_i_start, *a_i_end, ref_edge, t_0, c_points, &c_points_size);
    if (c_points_size > 1) {
      F32 t_1 = -V2DotV2(ref_edge, *b_i_end);
      ConvexHull2Clip(c_points[0], c_points[1], ref_edge_inv, t_1, c_points, &c_points_size);
    }

    F32 max = V2DotV2(ref_normal, *b_feature);
    manifold->contact_points_size = 0;
    for (U32 i = 0; i < c_points_size; i++) {
      V2 test = c_points[i];
      if (V2DotV2(ref_normal, test) - max >= 0) {
        manifold->contact_points[manifold->contact_points_size++] = test;
      }
    }
  }
  return true;
}

B32 Line3Eq(V3 a_start, V3 a_end, V3 b_start, V3 b_end) {
  return V3Eq(a_start, b_start) && V3Eq(a_end, b_end);
}

B32 Line3ApproxEq(V3 a_start, V3 a_end, V3 b_start, V3 b_end) {
  return V3ApproxEq(a_start, b_start) && V3ApproxEq(a_end, b_end);
}

void Line3Offset(V3* start, V3* end, V3 offset) {
  *start = V3AddV3(*start, offset);
  *end   = V3AddV3(*end, offset);
}

void Line3RotateAroundPoint(V3* start, V3* end, V3 point, M3 rot) {
  *start = V3RotateAroundV3M3(*start, point, rot);
  *end   = V3RotateAroundV3M3(*end, point, rot);
}

F32 Line3GetLength(V3 start, V3 end) {
  return F32Sqrt(Line3GetLengthSq(start, end));
}

F32 Line3GetLengthSq(V3 start, V3 end) {
  return V3LengthSq(V3SubV3(end, start));
}

V3 Line3GetDir(V3 start, V3 end) {
  return V3Normalize(V3SubV3(end, start));
}

V3 Line3GetMidpoint(V3 line_start, V3 line_end) {
  return V3DivF32(V3AddV3(line_start, line_end), 2);
}

V3 Line3GetClosestPoint(V3 start, V3 end, V3 point) {
  V3 ab = V3SubV3(end, start);
  V3 ap = V3SubV3(point, start);
  F32 t = V3DotV3(ap, ab) / V3LengthSq(ab);
  if (t < 0)      { return start;                            }
  else if (t > 1) { return end;                              }
  else            { return V3AddV3(start, V3MultF32(ab, t)); }
}

B32 Line3ContainsPoint(V3 start, V3 end, V3 point) {
  // if (V3Eq(start, end)) { return V3Eq(start, point); }
  V3 line = V3SubV3(end, start);
  V3 line_dir = V3Normalize(line);
  if (!Ray3ContainsPoint(start, line_dir, point))           { return false; }
  if (V3LengthSq(line) < V3LengthSq(V3SubV3(point, start))) { return false; }
  return true;
}

B32 Line3IntersectLine3(V3 a_start, V3 a_end, V3 b_start, V3 b_end, V3* intersect_point) {
  if (UNLIKELY(Line3GetLengthSq(b_start, b_end) == 0)) { return false; }
  V3 b_dir = Line3GetDir(b_start, b_end);

  V3 i;
  if (!Line3IntersectRay3(a_start, a_end, b_start, b_dir, &i)) { return false; }
  if (Line3GetLengthSq(b_start, b_end) < Line3GetLengthSq(b_start, i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Line3IntersectRay3(V3 line_start, V3 line_end, V3 ray_start, V3 ray_dir, V3* intersect_point) {
  if (UNLIKELY(Line3GetLengthSq(line_start, line_end) == 0)) { return false; }
  V3 line_dir = Line3GetDir(line_start, line_end);

  V3 i;
  if (!Ray3IntersectRay3(line_start, line_dir, ray_start, ray_dir, &i)) { return false; }
  if (Line3GetLengthSq(line_start, line_end) < Line3GetLengthSq(line_start, i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Line3IntersectPlane3(V3 line_start, V3 line_end, V3 plane_normal, F32 plane_d, V3* intersect_point) {
  return Plane3IntersectLine3(plane_normal, plane_d, line_start, line_end, intersect_point);
}

B32 Line3IntersectTri3(V3 line_start, V3 line_end, V3 tri_points[3], V3 tri_offset, V3* intersect_point) {
  return Tri3IntersectLine3(tri_points, tri_offset, line_start, line_end, intersect_point);
}

B32 Line3IntersectConvexPolygon3(V3 line_start, V3 line_end, V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* intersect_point) {
  return ConvexPolygon3IntersectLine3(polygon_points, polygon_points_size, polygon_offset, line_start, line_end, intersect_point);
}

B32 Line3IntersectSphere3(V3 line_start, V3 line_end, V3 sphere_center, F32 sphere_radius, V3* enter_point, V3* exit_point) {
  F32 length_sq = Line3GetLengthSq(line_start, line_end);
  if (UNLIKELY(length_sq == 0)) { return false; }
  V3 line_dir = Line3GetDir(line_start, line_end);

  V3 enter, exit;
  if (!Ray3IntersectSphere3(line_start, line_dir, sphere_center, sphere_radius, &enter, &exit)) { return false; }

  if (length_sq < Line3GetLengthSq(line_start, enter)) { return false; }
  if (length_sq < Line3GetLengthSq(line_start, exit))  { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL)  { *exit_point = exit;   }
  return true;
}

B32 Line3IntersectAabb3(V3 line_start, V3 line_end, V3 aabb_center, V3 aabb_size, V3* enter_point, V3* exit_point) {
  F32 length_sq = Line3GetLengthSq(line_start, line_end);
  if (UNLIKELY(length_sq == 0)) { return false; }
  V3 line_dir = Line3GetDir(line_start, line_end);

  V3 enter, exit;
  if (!Ray3IntersectAabb3(line_start, line_dir, aabb_center, aabb_size, &enter, &exit)) { return false; }

  if (length_sq < Line3GetLengthSq(line_start, enter)) { return false; }
  if (length_sq < Line3GetLengthSq(line_start, exit))  { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL)  { *exit_point = exit;   }
  return true;
}

B32 Line3IntersectConvexHull3(V3 line_start, V3 line_end, V3* mesh_points, U32 mesh_points_size, U32* mesh_indices, U32 mesh_indices_size, V3 mesh_offset, V3* enter_point, V3* exit_point) {
  return Line3IntersectMesh3(line_start, line_end, mesh_points, mesh_points_size, mesh_indices, mesh_indices_size, mesh_offset, enter_point, exit_point);
}

B32 Line3IntersectMesh3(V3 line_start, V3 line_end, V3* mesh_points, U32 mesh_points_size, U32* mesh_indices, U32 mesh_indices_size, V3 mesh_offset, V3* enter_point, V3* exit_point) {
  F32 length_sq = Line3GetLengthSq(line_start, line_end);
  if (UNLIKELY(length_sq == 0)) { return false; }
  V3 line_dir = Line3GetDir(line_start, line_end);

  V3 enter, exit;
  if (!Ray3IntersectMesh3(line_start, line_dir, mesh_points, mesh_points_size, mesh_indices, mesh_indices_size, mesh_offset, &enter, &exit)) { return false; }

  if (length_sq < Line3GetLengthSq(line_start, enter)) { return false; }
  if (length_sq < Line3GetLengthSq(line_start, exit))  { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL)  { *exit_point = exit;   }
  return true;
}

B32 Ray3Validate(V3 ray_dir) {
  return F32ApproxEq(V3LengthSq(ray_dir), 1);
}

B32 Ray3Eq(V3 a_start, V3 a_dir, V3 b_start, V3 b_dir) {
  return V3Eq(a_start, b_start) && V3Eq(a_dir, b_dir);
}

B32 Ray3ApproxEq(V3 a_start, V3 a_dir, V3 b_start, V3 b_dir) {
  return V3ApproxEq(a_start, b_start) && V3ApproxEq(a_dir, b_dir);
}

B32 Ray3ContainsPoint(V3 ray_start, V3 ray_dir, V3 point) {
  V3 to_point = V3SubV3(point, ray_start);
  V3 cross   = V3CrossV3(to_point, ray_dir);
  if (!F32ApproxEq(V3LengthSq(cross), 0)) { return false; }
  if (V3DotV3(to_point, ray_dir) < 0) { return false; }
  return true;
}

void Ray3RotateAroundPoint(V3* ray_start, V3* ray_dir, V3 point, M3 rot) {
  *ray_start = V3RotateAroundV3M3(*ray_start, point, rot);
  *ray_dir   = M3MultV3(rot, *ray_dir);
}

V3 Ray3GetDirInv(V3 ray_dir) {
  V3 result;
  result.x = 1.0f / ray_dir.x;
  result.y = 1.0f / ray_dir.y;
  result.z = 1.0f / ray_dir.z;
  return result;
}

B32 Ray3IntersectLine3(V3 ray_start, V3 ray_dir, V3 line_start, V3 line_end, V3* intersect_point) {
  if (UNLIKELY(Line3GetLengthSq(line_start, line_end) == 0)) { return false; }
  V3 line_dir = Line3GetDir(line_start, line_end);

  V3 i;
  if (!Ray3IntersectRay3(ray_start, ray_dir, line_start, line_dir, &i)) { return false; }
  if (Line3GetLengthSq(line_start, line_end) < Line3GetLengthSq(line_start, i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Ray3IntersectRay3(V3 a_start, V3 a_dir, V3 b_start, V3 b_dir, V3* intersect_point) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(a_dir), 1));
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(b_dir), 1));

  V3 d_start  = V3SubV3(b_start, a_start);
  F32 dir_dot = V3DotV3(a_dir, b_dir);
  F32 denom   = 1.0f - (dir_dot * dir_dot);
  if (F32ApproxEq(denom, 0)) {
    // NOTE: parallel or collinear case
    // NOTE: is parallel non-intersecting
    V3 start_cross = V3CrossV3(d_start, a_dir);
    if (!F32ApproxEq(V3LengthSq(start_cross), 0)) { return false; }
    V3 dir_cross = V3CrossV3(a_dir, b_dir);
    if (!F32ApproxEq(V3LengthSq(dir_cross), 0)) { return false; }

    // NOTE: is collinear, determine if there's overlap
    F32 t = +V3DotV3(a_dir, d_start);
    F32 u = -V3DotV3(b_dir, d_start);
    if (t < 0 && u < 0) { return false; }

    if (intersect_point != NULL) {
      // NOTE: favor intersection from A's perspective, u > 0 --> B pointing at A.
      if (u > 0) { *intersect_point = a_start; }
      else       { *intersect_point = b_start; }
    }
    return true;
  } else {
    // NOTE: skew case, find closest point on each ray to other ray
    F32 a = V3DotV3(a_dir, d_start);
    F32 b = V3DotV3(b_dir, d_start);
    F32 t = (a - (dir_dot * b)) / denom;
    F32 u = ((dir_dot * a) - b) / denom;
    if (t < 0 || u < 0) { return false; }

    V3 a_closest = V3AddV3(V3MultF32(a_dir, t), a_start);
    V3 b_closest = V3AddV3(V3MultF32(b_dir, u), b_start);
    if (!V3ApproxEq(a_closest, b_closest)) { return false; }

    if (intersect_point != NULL) { *intersect_point = a_closest; }
    return true;
  }
}

B32 Ray3IntersectPlane3(V3 ray_start, V3 ray_dir, V3 plane_normal, F32 plane_d, V3* intersect_point) {
  return Plane3IntersectRay3(plane_normal, plane_d, ray_start, ray_dir, intersect_point);
}

B32 Ray3IntersectTri3(V3 ray_start, V3 ray_dir, V3 tri_points[3], V3 tri_offset, V3* intersect_point) {
  return Tri3IntersectRay3(tri_points, tri_offset, ray_start, ray_dir, intersect_point);
}

B32 Ray3IntersectConvexPolygon3(V3 ray_start, V3 ray_dir, V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* intersect_point) {
  return ConvexPolygon3IntersectRay3(polygon_points, polygon_points_size, polygon_offset, ray_start, ray_dir, intersect_point);
}

B32 Ray3IntersectSphere3(V3 ray_start, V3 ray_dir, V3 sphere_center, F32 sphere_radius, V3* enter_point, V3* exit_point) {
  V3 rel_center        = V3SubV3(sphere_center, ray_start);
  F32 to_perp_center   = V3DotV3(rel_center, ray_dir);
  F32 d2               = V3LengthSq(rel_center) - (to_perp_center * to_perp_center);
  F32 sphere_radius_sq = sphere_radius * sphere_radius;
  if (d2 > sphere_radius_sq) { return false; }

  F32 center_to_edge = F32Sqrt(sphere_radius_sq - d2);
  F32 t0 = to_perp_center - center_to_edge;
  F32 t1 = to_perp_center + center_to_edge;
  if (t1 < t0) { SWAP(F32, t0, t1); }
  if (t0 < 0) {
    if (t1 < 0) { return false; }
    t0 = t1;
  }
  if (enter_point != NULL) { *enter_point = V3AddV3(ray_start, V3MultF32(ray_dir, t0)); }
  if (exit_point != NULL)  { *exit_point  = V3AddV3(ray_start, V3MultF32(ray_dir, t1)); }
  return true;
}

B32 Ray3IntersectAabb3(V3 ray_start, V3 ray_dir, V3 aabb_center, V3 aabb_size, V3* enter_point, V3* exit_point) {
  V3  aabb_points[8];
  U32 aabb_indices[36];
  ConvexHull3FromAabb3(aabb_points, aabb_indices, aabb_center, aabb_size);
  return Ray3IntersectConvexHull3(ray_start, ray_dir, aabb_points, 8, aabb_indices, 36, V3_ZEROES, enter_point, exit_point);
}

B32 Ray3IntersectConvexHull3(V3 ray_start, V3 ray_dir, V3* hull_points, U32 hull_points_size, U32* hull_indices, U32 hull_indices_size, V3 hull_offset, V3* enter_point, V3* exit_point) {
  return Ray3IntersectMesh3(ray_start, ray_dir, hull_points, hull_points_size, hull_indices, hull_indices_size, hull_offset, enter_point, exit_point);
}

B32 Ray3IntersectMesh3(V3 ray_start, V3 ray_dir, V3* mesh_points, U32 mesh_points_size, U32* mesh_indices, U32 mesh_indices_size, V3 mesh_offset, V3* enter_point, V3* exit_point) {
  DEBUG_ASSERT(mesh_indices_size % 3 == 0);
  V3  i_min, i_max;
  F32 i_min_dist = F32_MAX;
  F32 i_max_dist = F32_MIN;
  for (U32 i = 0; i < mesh_indices_size; i += 3) {
    U32 i0 = mesh_indices[i + 0];
    U32 i1 = mesh_indices[i + 1];
    U32 i2 = mesh_indices[i + 2];
    DEBUG_ASSERT(i0 < mesh_points_size && i1 < mesh_points_size && i2 < mesh_points_size);
    V3 tri[3] = { mesh_points[i0], mesh_points[i1], mesh_points[i2] };
    V3 i;
    if (Ray3IntersectTri3(ray_start, ray_dir, tri, mesh_offset, &i)) {
      if (enter_point == NULL && exit_point == NULL) { return true; }
      F32 d = V3DotV3(ray_dir, V3SubV3(i, ray_start));
      if (d < i_min_dist) {
        i_min_dist = d;
        i_min = i;
      }
      if (d > i_max_dist) {
        i_max_dist = d;
        i_max = i;
      }
    }
  }
  if (i_min_dist == F32_MAX) { return false; }
  if (enter_point != NULL) { *enter_point = i_min; }
  if (exit_point != NULL)  { *exit_point  = i_max; }
  return true;
}

B32 Plane3Validate(V3 plane_normal) {
  return F32ApproxEq(V3LengthSq(plane_normal), 1);
}

B32 Plane3Eq(V3 a_normal, F32 a_d, V3 b_normal, F32 b_d) {
  return V3Eq(a_normal, b_normal) && a_d == b_d;
}

B32 Plane3ApproxEq(V3 a_normal, F32 a_d, V3 b_normal, F32 b_d) {
  return V3ApproxEq(a_normal, b_normal) && F32ApproxEq(a_d, b_d);
}

void Plane3FromAnchor(V3 plane_normal, F32* plane_d, V3 anchor) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(plane_normal), 1));
  *plane_d = V3DotV3(anchor, plane_normal);
}

B32 Plane3FromTri3(V3* plane_normal, F32* plane_d, V3 tri_points[3], V3 tri_offset) {
  V3 a = V3SubV3(V3AddV3(tri_points[1], tri_offset), V3AddV3(tri_points[0], tri_offset));
  V3 b = V3SubV3(V3AddV3(tri_points[2], tri_offset), V3AddV3(tri_points[0], tri_offset));
  *plane_normal = V3CrossV3(a, b);
  if (F32ApproxEq(V3LengthSq(*plane_normal), 0)) { return false; }
  *plane_normal = V3Normalize(*plane_normal);
  Plane3FromAnchor(*plane_normal, plane_d, V3AddV3(tri_points[0], tri_offset));
  return true;
}

B32 Plane3FromConvexPolygon3(V3* plane_normal, F32* plane_d, V3* polygon_points, U32 polygon_points_size, V3 polygon_offset) {
  DEBUG_ASSERT(polygon_points_size >= 3);
  return Plane3FromTri3(plane_normal, plane_d, polygon_points, polygon_offset);
}

B32 Plane3IsPointAbove(V3 plane_normal, F32 plane_d, V3 point) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(plane_normal), 1));
  V3 point_on_plane = Plane3CreatePointOnPlane(plane_normal, plane_d);
  V3 vec            = V3SubV3(point, point_on_plane);
  return V3DotV3(plane_normal, vec) > 0;
}

B32 Plane3IsPointBelow(V3 plane_normal, F32 plane_d, V3 point) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(plane_normal), 1));
  V3 point_on_plane = Plane3CreatePointOnPlane(plane_normal, plane_d);
  V3 vec            = V3SubV3(point, point_on_plane);
  return V3DotV3(plane_normal, vec) < 0;
}

B32 Plane3ContainsPoint(V3 plane_normal, F32 plane_d, V3 point) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(plane_normal), 1));
  return F32ApproxEq(V3DotV3(plane_normal, point), plane_d);
}

void Plane3RotateAroundPoint(V3* plane_normal, F32* plane_d, V3 point, M3 rot) {
  V3 vec = Plane3CreatePointOnPlane(*plane_normal, *plane_d);
  vec    = V3RotateAroundV3M3(vec, point, rot);
  *plane_normal = M3MultV3(rot, *plane_normal);
  *plane_d      = V3DotV3(*plane_normal, vec);
}

// https://math.stackexchange.com/questions/723937/find-the-point-on-a-plane-3x-4y-z-1-that-is-closest-to-1-0-1
V3 Plane3CreatePointOnPlane(V3 plane_normal, F32 plane_d) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(plane_normal), 1));
  F32 t = plane_d / V3DotV3(plane_normal, plane_normal);
  return V3MultF32(plane_normal, t);
}

void Plane3Offset(V3* plane_normal, F32* plane_d, V3 offset) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(*plane_normal), 1));
  *plane_d -= V3DotV3(*plane_normal, offset);
}

void Plane3Flip(V3* plane_normal, F32* plane_d) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(*plane_normal), 1));
  *plane_normal = V3Negate(*plane_normal);
  *plane_d      = *plane_d * -1;
}

B32 Plane3IntersectLine3(V3 plane_normal, F32 plane_d, V3 line_start, V3 line_end, V3* intersect_point) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(plane_normal), 1));
  if (UNLIKELY(Line3GetLengthSq(line_start, line_end) == 0)) { return false; }
  V3 line_dir = Line3GetDir(line_start, line_end);

  V3 i;
  if (!Plane3IntersectRay3(plane_normal, plane_d, line_start, line_dir, &i)) { return false; }
  if (Line3GetLengthSq(line_start, line_end) < Line3GetLengthSq(line_start, i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Plane3IntersectRay3(V3 plane_normal, F32 plane_d, V3 ray_start, V3 ray_dir, V3* intersect_point) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(plane_normal), 1));
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(ray_dir), 1));
  F32 denom = V3DotV3(plane_normal, ray_dir);
  if (F32ApproxEq(denom, 0)) { return false; }
  F32 t = (plane_d - V3DotV3(plane_normal, ray_start)) / denom;
  if (t < 0) { return false; }
  if (intersect_point != NULL) { *intersect_point = V3AddV3(ray_start, V3MultF32(ray_dir, t)); }
  return true;
}

B32 Plane3IntersectPlane3(V3 a_normal, F32 a_d, V3 b_normal, F32 b_d, V3* intersect_point, V3* intersect_dir) {
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(a_normal), 1));
  DEBUG_ASSERT(F32ApproxEq(V3LengthSq(b_normal), 1));

  V3  dir        = V3CrossV3(a_normal, b_normal);
  F32 dir_len_sq = V3LengthSq(dir);
  if (F32ApproxEq(dir_len_sq, 0)) { return false; }

  if (intersect_dir != NULL) { *intersect_dir = dir; }
  if (intersect_point != NULL) {
    V3 b_norm_cross_dir = V3CrossV3(b_normal, dir);
    V3 dir_cross_a_norm = V3CrossV3(dir, a_normal);

    V3 t1 = V3MultF32(b_norm_cross_dir, a_d);
    V3 t2 = V3MultF32(dir_cross_a_norm, b_d);
    *intersect_point = V3DivF32(V3AddV3(t1, t2), dir_len_sq);
  }
  return true;
}

B32 Plane3IntersectTri3(V3 plane_normal, F32 plane_d, V3 tri_points[3], V3 tri_offset, V3* intersect_start, V3* intersect_end) {
  return Tri3IntersectPlane3(tri_points, tri_offset, plane_normal, plane_d, intersect_start, intersect_end);
}

B32 Plane3IntersectConvexPolygon3(V3 plane_normal, F32 plane_d, V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* intersect_start, V3* intersect_end) {
  return ConvexPolygon3IntersectPlane3(polygon_points, polygon_points_size, polygon_offset, plane_normal, plane_d, intersect_start, intersect_end);
}

B32 Tri3Validate(V3 tri_points[3]) {
  return !V3Eq(tri_points[0], tri_points[1]) &&
         !V3Eq(tri_points[0], tri_points[2]) &&
         !V3Eq(tri_points[1], tri_points[2]);
}

B32 Tri3Eq(V3 a_points[3], V3 b_points[3]) {
  return ConvexPolygon3Eq(a_points, 3, b_points, 3);
}

B32 Tri3ApproxEq(V3 a_points[3], V3 b_points[3]) {
  return ConvexPolygon3ApproxEq(a_points, 3, b_points, 3);
}

void Tri3Offset(V3 tri_points[3], V3 offset) {
  ConvexPolygon3Offset(tri_points, 3, offset);
}

void Tri3SetCenter(V3 tri_points[3], V3 center) {
  ConvexPolygon3SetCenter(tri_points, 3, center);
}

V3 Tri3GetCenter(V3 tri_points[3]) {
  return ConvexPolygon3GetCenter(tri_points, 3);
}

V3 Tri3GetNormal(V3 tri_points[3]) {
  V3 ab = V3SubV3(tri_points[1], tri_points[0]);
  V3 ac = V3SubV3(tri_points[2], tri_points[0]);
  return V3Normalize(V3CrossV3(ab, ac));
}

void Tri3GetEnclosingAabb3(V3 tri_points[3], V3 tri_offset, V3* aabb_center, V3* aabb_size) {
  ConvexPolygon3GetEnclosingAabb3(tri_points, 3, tri_offset, aabb_center, aabb_size);
}

void Tri3GetEnclosingSphere3(V3 tri_points[3], V3 tri_offset, V3* sphere_center, F32* sphere_radius) {
  ConvexPolygon3GetEnclosingSphere3(tri_points, 3, tri_offset, sphere_center, sphere_radius);
}

void Tri3RotateAroundPoint(V3 tri_points[3], V3 point, M3 rot) {
  ConvexPolygon3RotateAroundPoint(tri_points, 3, point, rot);
}

B32 Tri3ContainsPoint(V3 tri_points[3], V3 tri_offset, V3 point) {
  return ConvexPolygon3ContainsPoint(tri_points, 3, tri_offset, point);
}

B32 Tri3IntersectLine3(V3 tri_points[3], V3 tri_offset, V3 line_start, V3 line_end, V3* intersect_point) {
  return ConvexPolygon3IntersectLine3(tri_points, 3, tri_offset, line_start, line_end, intersect_point);
}

B32 Tri3IntersectRay3(V3 tri_points[3], V3 tri_offset, V3 ray_start, V3 ray_dir, V3* intersect_point) {
  return ConvexPolygon3IntersectRay3(tri_points, 3, tri_offset, ray_start, ray_dir, intersect_point);
}

B32 Tri3IntersectPlane3(V3 tri_points[3], V3 tri_offset, V3 plane_normal, F32 plane_d, V3* intersect_start, V3* intersect_end) {
  return ConvexPolygon3IntersectPlane3(tri_points, 3, tri_offset, plane_normal, plane_d, intersect_start, intersect_end);
}

B32 Tri3IntersectTri3(V3 a_points[3], V3 a_offset, V3 b_points[3], V3 b_offset, V3* intersect_start, V3* intersect_end) {
  return ConvexPolygon3IntersectTri3(a_points, 3, a_offset, b_points, b_offset, intersect_start, intersect_end);
}

B32 Tri3IntersectConvexPolygon3(V3 tri_points[3], V3 tri_offset, V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* intersect_start, V3* intersect_end) {
  return ConvexPolygon3IntersectConvexPolygon3(tri_points, 3, tri_offset, polygon_points, polygon_points_size, polygon_offset, intersect_start, intersect_end);
}

B32 ConvexPolygon3Validate(U32 polygon_points_size) {
  return polygon_points_size > 2;
}

B32 ConvexPolygon3Eq(V3* a_points, U32 a_points_size, V3* b_points, U32 b_points_size) {
  if (a_points_size != b_points_size) { return false; }
  for (U32 i = 0; i < a_points_size; i++) {
    if (!V3Eq(a_points[i], b_points[i])) { return false; }
  }
  return true;
}

B32 ConvexPolygon3ApproxEq(V3* a_points, U32 a_points_size, V3* b_points, U32 b_points_size) {
  if (a_points_size != b_points_size) { return false; }
  for (U32 i = 0; i < a_points_size; i++) {
    if (!V3ApproxEq(a_points[i], b_points[i])) { return false; }
  }
  return true;
}

void ConvexPolygon3Offset(V3* polygon_points, U32 polygon_points_size, V3 offset) {
  for (U32 i = 0; i < polygon_points_size; i++) {
    polygon_points[i] = V3AddV3(polygon_points[i], offset);
  }
}

V3 ConvexPolygon3GetCenter(V3* polygon_points, U32 polygon_points_size) {
  V3 center = V3_ZEROES;
  for (U32 i = 0; i < polygon_points_size; i++) {
    center = V3AddV3(center, polygon_points[i]);
  }
  return V3DivF32(center, polygon_points_size);
}

void ConvexPolygon3SetCenter(V3* polygon_points, U32 polygon_points_size, V3 center) {
  V3 curr_center = ConvexPolygon3GetCenter(polygon_points, polygon_points_size);
  V3 offset      = V3SubV3(center, curr_center);
  ConvexPolygon3Offset(polygon_points, polygon_points_size, offset);
}

void ConvexPolygon3GetEnclosingAabb3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* aabb_center, V3* aabb_size) {
  ConvexHull3GetEnclosingAabb3(polygon_points, polygon_points_size, polygon_offset, aabb_center, aabb_size);
}

void ConvexPolygon3GetEnclosingSphere3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3* sphere_center, F32* sphere_radius) {
  ConvexHull3GetEnclosingSphere3(polygon_points, polygon_points_size, polygon_offset, sphere_center, sphere_radius);
}

void ConvexPolygon3RotateAroundPoint(V3* polygon_points, U32 polygon_points_size, V3 point, M3 rot) {
  for (U32 i = 0; i < polygon_points_size; i++) {
    polygon_points[i] = V3RotateAroundV3M3(polygon_points[i], point, rot);
  }
}

// NOTE: verifies that point lies within the extrusion of the polygon along its normal
static B32 ConvexPolygon3ExtrusionContainsPoint(V3* polygon_points, U32 polygon_points_size, V3 polygon_normal, V3 point) {
  for (U32 i = 0; i < polygon_points_size; i++) {
    V3 edge_start = polygon_points[i];
    V3 edge_end   = polygon_points[(i + 1) % polygon_points_size];
    V3 edge       = V3SubV3(edge_end, edge_start);
    V3 to_point   = V3SubV3(point, edge_start);
    V3 cross      = V3CrossV3(edge, to_point);
    if (V3DotV3(cross, polygon_normal) < 0) { return false; }
  }
  return true;
}

B32 ConvexPolygon3ContainsPoint(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3 point) {
  V3 plane_normal; F32 plane_d;
  if (!Plane3FromConvexPolygon3(&plane_normal, &plane_d, polygon_points, polygon_points_size, polygon_offset)) { return false; }
  if (!Plane3ContainsPoint(plane_normal, plane_d, point)) { return false; }
  if (!ConvexPolygon3ExtrusionContainsPoint(polygon_points, polygon_points_size, plane_normal, point)) { return false; }
  return true;
}

B32 ConvexPolygon3IntersectLine3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3 line_start, V3 line_end, V3* intersect_point) {
  if (UNLIKELY(Line3GetLengthSq(line_start, line_end) == 0)) { return false; }
  V3 line_dir = Line3GetDir(line_start, line_start);

  V3 i;
  if (!ConvexPolygon3IntersectRay3(polygon_points, polygon_points_size, polygon_offset, line_start, line_dir, &i)) { return false; }
  if (Line3GetLengthSq(line_start, line_end) < Line3GetLengthSq(line_start, i)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 ConvexPolygon3IntersectRay3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3 ray_start, V3 ray_dir, V3* intersect_point) {
  V3 plane_normal, i; F32 plane_d;
  if (!Plane3FromConvexPolygon3(&plane_normal, &plane_d, polygon_points, polygon_points_size, polygon_offset)) { return false; }
  if (!Plane3IntersectRay3(plane_normal, plane_d, ray_start, ray_dir, &i)) { return false; }
  if (!ConvexPolygon3ExtrusionContainsPoint(polygon_points, polygon_points_size, plane_normal, i)) { return false; }
  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 ConvexPolygon3IntersectPlane3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3 plane_normal, F32 plane_d, V3* intersect_start, V3* intersect_end) {
  V3 i_start, i_end;
  U8 num_intersections = 0;
  for (U32 i = 0; i < polygon_points_size; i++) {
    V3  edge_start = V3AddV3(polygon_points[i], polygon_offset);
    V3  edge_end   = V3AddV3(polygon_points[(i + 1) % polygon_points_size], polygon_offset);
    F32 d_start    = V3DotV3(plane_normal, edge_start) - plane_d;
    F32 d_end      = V3DotV3(plane_normal, edge_end)   - plane_d;

    if ((d_start > 0 && d_end < 0) || (d_start < 0 && d_end > 0)) {
      // NOTE: edge crosses plane
      V3 edge = V3SubV3(edge_end, edge_start);
      F32 t   = d_start / (d_start - d_end);
      V3 hit  = V3AddV3(edge_start, V3MultF32(edge, t));
      num_intersections++;
      if (num_intersections == 1) { i_start = hit; }
      else                        { i_end = hit; break; }
    } else if (F32ApproxEq(d_start, 0)) {
      // NOTE: edge start lies exactly on plane
      num_intersections++;
      if (num_intersections == 1) { i_start = edge_start; }
      else                        { i_end   = edge_start; break; }
    }
  }
  DEBUG_ASSERT(num_intersections <= 2);
  if (num_intersections == 0) { return false;    }
  if (num_intersections == 1) { i_end = i_start; }
  if (intersect_start != NULL) { *intersect_start = i_start; }
  if (intersect_end != NULL)   { *intersect_end   = i_end;   }
  return true;
}

B32 ConvexPolygon3IntersectTri3(V3* polygon_points, U32 polygon_points_size, V3 polygon_offset, V3 tri_points[3], V3 tri_offset, V3* intersect_start, V3* intersect_end) {
  return ConvexPolygon3IntersectConvexPolygon3(polygon_points, polygon_points_size, polygon_offset, tri_points, 3, tri_offset, intersect_start, intersect_end);
}

B32 ConvexPolygon3IntersectConvexPolygon3(V3* a_points, U32 a_points_size, V3 a_offset, V3* b_points, U32 b_points_size, V3 b_offset, V3* intersect_start, V3* intersect_end) {
  V3 a_normal, b_normal; F32 a_d, b_d;
  if (!Plane3FromConvexPolygon3(&a_normal, &a_d, a_points, a_points_size, a_offset)) { return false; }
  if (!Plane3FromConvexPolygon3(&b_normal, &b_d, b_points, b_points_size, b_offset)) { return false; }

  // TODO: currently, this fails when the polgyons are on the same plane. it probably shouldn't, but this complicates this fn since it assumes edge intersections only.
  // can probably extend the gjk implementation to clip the incident faces and generate contact points.
  V3 i_point, i_dir, a_clip_start, a_clip_end, b_clip_start, b_clip_end;
  if (!Plane3IntersectPlane3(a_normal, a_d, b_normal, b_d, &i_point, &i_dir)) { return false; }
  if (!ConvexPolygon3IntersectPlane3(a_points, a_points_size, a_offset, b_normal, b_d, &a_clip_start, &a_clip_end)) { return false; }
  if (!ConvexPolygon3IntersectPlane3(b_points, b_points_size, b_offset, a_normal, a_d, &b_clip_start, &b_clip_end)) { return false; }

  F32 t0_a    = V3DotV3(i_dir, a_clip_start);
  F32 t1_a    = V3DotV3(i_dir, a_clip_end);
  F32 t0_b    = V3DotV3(i_dir, b_clip_start);
  F32 t1_b    = V3DotV3(i_dir, b_clip_end);
  F32 t_start = MAX(MIN(t0_a, t1_a), MIN(t0_b, t1_b));
  F32 t_end   = MIN(MAX(t0_a, t1_a), MAX(t0_b, t1_b));
  if (t_end < t_start) { return false; }

  if (intersect_start != NULL) { *intersect_start = V3AddV3(i_point, V3MultF32(i_dir, t_start)); }
  if (intersect_end != NULL)   { *intersect_end   = V3AddV3(i_point, V3MultF32(i_dir, t_end));   }
  return true;
}

B32 Aabb3Validate(V3 aabb_size) {
  return aabb_size.x >= 0 && aabb_size.y >= 0 && aabb_size.z >= 0;
}

// https://stackoverflow.com/questions/75811830/determine-the-face-of-a-cube-intersected-by-a-ray-in-a-minecraft-like-game
V3 Aabb3RelativeNormal(V3 center_point, V3 point) {
  V3 relative_p = V3SubV3(point, center_point);
  F32 p_max = V3MaxValue(relative_p);
  F32 p_min = V3MinValue(relative_p);
  F32 abs_p_min = F32Abs(p_min);
  if (p_max == relative_p.x && p_max > abs_p_min)  { return V3_X_POS; }
  if (p_min == relative_p.x && p_max < abs_p_min)  { return V3_X_NEG; }
  if (p_max == relative_p.y && p_max > abs_p_min)  { return V3_Y_POS; }
  if (p_min == relative_p.y && p_max < abs_p_min)  { return V3_Y_NEG; }
  if (p_max == relative_p.z && p_max >= abs_p_min) { return V3_Z_POS; }
  if (p_min == relative_p.z && p_max <= abs_p_min) { return V3_Z_NEG; }
  UNREACHABLE();
  return V3_ZEROES;
}

B32 Aabb3Eq(V3 a_center, V3 a_size, V3 b_center, V3 b_size) {
  return V3Eq(a_center, b_center) && V3Eq(a_size, b_size);
}

B32 Aabb3ApproxEq(V3 a_center, V3 a_size, V3 b_center, V3 b_size) {
  return V3Eq(a_center, b_center) && V3Eq(a_size, b_size);
}

void Aabb3RotateAroundPoint(V3* aabb_center, V3 point, M3 rot) {
  *aabb_center = V3RotateAroundV3M3(*aabb_center, point, rot);
}

void Aabb3FromMinMax(V3* aabb_center, V3* aabb_size, V3 min, V3 max) {
  DEBUG_ASSERT(min.x < max.x);
  DEBUG_ASSERT(min.y < max.y);
  DEBUG_ASSERT(min.z < max.z);
  *aabb_center = V3DivF32(V3AddV3(min, max), 2);
  *aabb_size   = V3SubV3(max, min);
}

void Aabb3GetMinMax(V3 aabb_center, V3 aabb_size, V3* min, V3* max) {
  V3 half_size = V3DivF32(aabb_size, 2.0f);
  *min = V3SubV3(aabb_center, half_size);
  *max = V3AddV3(aabb_center, half_size);
}

void Aabb3GetEnclosingSphere3(V3 aabb_center, V3 aabb_size, F32* sphere_radius) {
  V3 min, max;
  Aabb3GetMinMax(aabb_center, aabb_size, &min, &max);
  *sphere_radius = V3Length(V3SubV3(max, aabb_center));
}

B32 Aabb3ContainsPoint(V3 aabb_center, V3 aabb_size, V3 point) {
  V3 min, max;
  Aabb3GetMinMax(aabb_center, aabb_size, &min, &max);
  return (min.x <= point.x && point.x <= max.x) &&
         (min.y <= point.y && point.y <= max.y) &&
         (min.z <= point.z && point.z <= max.z);
}

B32 Aabb3IntersectLine3(V3 aabb_center, V3 aabb_size, V3 line_start, V3 line_end, V3* enter_point, V3* exit_point) {
  return Line3IntersectAabb3(line_start, line_end, aabb_center, aabb_size, enter_point, exit_point);
}

B32 Aabb3IntersectRay3(V3 aabb_center, V3 aabb_size, V3 ray_start, V3 ray_dir, V3* enter_point, V3* exit_point) {
  return Ray3IntersectAabb3(ray_start, ray_dir, aabb_center, aabb_size, enter_point, exit_point);
}

B32 Aabb3IntersectAabb3(V3 a_center, V3 a_size, V3 b_center, V3 b_size, IntersectManifold3* manifold) {
  V3 a_half = V3DivF32(a_size, 2);
  V3 b_half = V3DivF32(b_size, 2);

  V3 dist    = V3Abs(V3SubV3(a_center, b_center));
  V3 overlap = V3SubV3(V3AddV3(a_half, b_half), dist);
  if (overlap.x <= 0 || overlap.y <= 0 || overlap.z <= 0) { return false; }

  if (manifold == NULL) { return true; }
  U32 axis = 0;
  manifold->penetration = overlap.x;
  if (overlap.y < manifold->penetration) {
    axis = 1;
    manifold->penetration = overlap.y;
  }
  if (overlap.z < manifold->penetration) {
    axis = 2;
    manifold->penetration = overlap.z;
  }
  switch (axis) {
    case 0:  { manifold->normal = (dist.x < 0) ? V3_X_NEG : V3_X_POS; } break;
    case 1:  { manifold->normal = (dist.y < 0) ? V3_Y_NEG : V3_Y_POS; } break;
    case 2:  { manifold->normal = (dist.z < 0) ? V3_Z_NEG : V3_Z_POS; } break;
    default: { UNREACHABLE(); } break;
  }
  return true;
}

B32 Aabb3IntersectSphere3(V3 aabb_center, V3 aabb_size, V3 sphere_center, F32 sphere_radius, IntersectManifold3* manifold) {
  // TODO: likely don't need to use GJK here. will get a more accurate manifold if implemented.
  V3 aabb_points[8];
  ConvexHull3FromAabb3(aabb_points, NULL, aabb_center, aabb_size);
  return ConvexHull3IntersectSphere3(aabb_points, 8, V3_ZEROES, sphere_center, sphere_radius, manifold);
}

B32 Aabb3IntersectConvexHull3(V3 aabb_center, V3 aabb_size, V3* hull_points, U32 hull_points_size, V3 hull_offset, IntersectManifold3* manifold) {
  V3 aabb_points[8];
  ConvexHull3FromAabb3(aabb_points, NULL, aabb_center, aabb_size);
  return ConvexHull3IntersectConvexHull3(aabb_points, 8, V3_ZEROES, hull_points, hull_points_size, hull_offset, manifold);
}

B32 Sphere3Validate(F32 sphere_radius) {
  return sphere_radius >= 0;
}

B32 Sphere3Eq(V3 a_center, F32 a_radius, V3 b_center, F32 b_radius) {
  return V3Eq(a_center, b_center) && a_radius == b_radius;
}

B32 Sphere3ApproxEq(V3 a_center, F32 a_radius, V3 b_center, F32 b_radius) {
  return V3ApproxEq(a_center, b_center) && F32ApproxEq(a_radius, b_radius);
}

void Sphere3RotateAroundPoint(V3* sphere_center, V3 point, M3 rot) {
  *sphere_center = V3RotateAroundV3M3(*sphere_center, point, rot);
}

void Sphere3GetEnclosingAabb3(F32 sphere_radius, V3* aabb_size) {
  *aabb_size = V3Splat(sphere_radius * 2);
}

B32 Sphere3ContainsPoint(V3 sphere_center, F32 sphere_radius, V3 point) {
  V3 to_pt = V3SubV3(point, sphere_center);
  return V3LengthSq(to_pt) < (sphere_radius * sphere_radius);
}

B32 Sphere3IntersectLine3(V3 sphere_center, F32 sphere_radius, V3 line_start, V3 line_end, V3* enter_point, V3* exit_point) {
  return Line3IntersectSphere3(line_start, line_end, sphere_center, sphere_radius, enter_point, exit_point);
}

B32 Sphere3IntersectRay3(V3 sphere_center, F32 sphere_radius, V3 ray_start, V3 ray_dir, V3* enter_point, V3* exit_point) {
  return Ray3IntersectSphere3(ray_start, ray_dir, sphere_center, sphere_radius, enter_point, exit_point);
}

B32 Sphere3IntersectSphere3(V3 a_center, F32 a_radius, V3 b_center, F32 b_radius, IntersectManifold3* manifold) {
  V3 d_center      = V3SubV3(a_center, b_center);
  F32 min_distance = a_radius + b_radius;
  F32 distance     = V3Length(d_center);
  if (min_distance < distance) { return false; }
  if (manifold != NULL) {
    if (distance == 0) { d_center.z = 1.0f; }
    manifold->normal      = V3Normalize(d_center);
    manifold->penetration = min_distance - distance;
  }
  return true;
}

B32 Sphere3IntersectAabb3(V3 sphere_center, F32 sphere_radius, V3 aabb_center, V3 aabb_size, IntersectManifold3* manifold) {
  V3 aabb_points[8];
  ConvexHull3FromAabb3(aabb_points, NULL, aabb_center, aabb_size);
  return Sphere3IntersectConvexHull3(sphere_center, sphere_radius, aabb_points, 8, V3_ZEROES, manifold);
}

B32 Sphere3IntersectConvexHull3(V3 sphere_center, F32 sphere_radius, V3* hull_points, U32 hull_points_size, V3 hull_offset, IntersectManifold3* manifold) {
  GjkSupportContext a;
  GjkSupportParamsSphere3 a_params;
  a_params.center = sphere_center;
  a_params.radius = sphere_radius;
  a.support_fn = GjkSupportSphere3;
  a.user_data = &a_params;

  GjkSupportContext b;
  GjkSupportParamsConvexHull3 b_params;
  b_params.points = hull_points;
  b_params.points_size = hull_points_size;
  b_params.offset = hull_offset;
  b.support_fn = GjkSupportConvexHull3;
  b.user_data = &b_params;

  return GjkIntersection3(&a, &b, manifold);
}

B32 ConvexHull3Validate(U32 hull_points_size) {
  return hull_points_size > 2;
}

B32 ConvexHull3Eq(V3* a_points, U32 a_points_size, U32* a_indices, U32 a_indices_size, V3* b_points, U32 b_points_size, U32* b_indices, U32 b_indices_size) {
  return Mesh3Eq(a_points, a_points_size, a_indices, a_indices_size, b_points, b_points_size, b_indices, b_indices_size);
}

B32 ConvexHull3ApproxEq(V3* a_points, U32 a_points_size, U32* a_indices, U32 a_indices_size, V3* b_points, U32 b_points_size, U32* b_indices, U32 b_indices_size) {
  return Mesh3ApproxEq(a_points, a_points_size, a_indices, a_indices_size, b_points, b_points_size, b_indices, b_indices_size);
}

void ConvexHull3Offset(V3* hull_points, U32 hull_points_size, V3 offset) {
  Mesh3Offset(hull_points, hull_points_size, offset);
}

void ConvexHull3RotateAroundPoint(V3* hull_points, U32 hull_points_size, V3 point, M3 rot) {
  Mesh3RotateAroundPoint(hull_points, hull_points_size, point, rot);
}

void ConvexHull3Flatten(Arena* arena, V3* src_points, U32 src_points_size, V3** dest_points, U32* dest_points_size) {
  *dest_points             = NULL;
  *dest_points_size        = 0;
  U32 dest_points_capacity = 0;
  for (U32 i = 0; i < src_points_size; i++) {
    V3  curr         = src_points[i];
    B32 is_duplicate = false;
    for (U32 j = 0; j < *dest_points_size; j++) {
      V3 test = (*dest_points)[j];
      if (V3Eq(curr, test)) { is_duplicate = true; break; }
    }
    if (!is_duplicate) {
      DA_PUSH_BACK_EX(arena, *dest_points, *dest_points_size, dest_points_capacity, curr);
    }
  }
  // NOTE: pop excess
  ARENA_POP_ARRAY(arena, V3, dest_points_capacity - *dest_points_size);
}

void ConvexHull3FromAabb3(V3 hull_points[8], U32 hull_indices[36], V3 aabb_center, V3 aabb_size) {
  Mesh3FromAabb3(hull_points, hull_indices, aabb_center, aabb_size);
}

V3 ConvexHull3GetCenter(V3* hull_points, U32 hull_points_size) {
  return Mesh3GetCenter(hull_points, hull_points_size);
}

void ConvexHull3SetCenter(V3* hull_points, U32 hull_points_size, V3 center) {
  Mesh3SetCenter(hull_points, hull_points_size, center);
}

void ConvexHull3GetEnclosingAabb3(V3* hull_points, U32 hull_points_size, V3 hull_offset, V3* aabb_center, V3* aabb_size) {
  Mesh3GetEnclosingAabb3(hull_points, hull_points_size, hull_offset, aabb_center, aabb_size);
}

void ConvexHull3GetEnclosingSphere3(V3* hull_points, U32 hull_points_size, V3 hull_offset, V3* sphere_center, F32* sphere_radius) {
  Mesh3GetEnclosingSphere3(hull_points, hull_points_size, hull_offset, sphere_center, sphere_radius);
}

B32 ConvexHull3ContainsPoint(V3* hull_points, U32 hull_points_size, U32* hull_indices, U32 hull_indices_size, V3 hull_offset, V3 point) {
  DEBUG_ASSERT(hull_indices_size % 3 == 0);
  for (U32 i = 0; i < hull_indices_size; i+=3) {
    U32 i0 = hull_indices[i + 0];
    U32 i1 = hull_indices[i + 1];
    U32 i2 = hull_indices[i + 2];
    DEBUG_ASSERT(i0 < hull_points_size && i1 < hull_points_size && i2 < hull_points_size);
    V3 tri[3] = { hull_points[i0], hull_points[i1], hull_points[i2] };
    V3 plane_normal; F32 plane_d;
    if (!Plane3FromTri3(&plane_normal, &plane_d, tri, hull_offset)) { return false; }
    if (V3DotV3(plane_normal, point) + plane_d > 0)   { return false; }
  }
  return true;
}

B32 ConvexHull3IntersectLine3(V3* hull_points, U32 hull_points_size, U32* hull_indices, U32 hull_indices_size, V3 hull_offset, V3 line_start, V3 line_end, V3* enter_point, V3* exit_point) {
  return Line3IntersectConvexHull3(line_start, line_end, hull_points, hull_points_size, hull_indices, hull_indices_size, hull_offset, enter_point, exit_point);
}

B32 ConvexHull3IntersectRay3(V3* hull_points, U32 hull_points_size, U32* hull_indices, U32 hull_indices_size, V3 hull_offset, V3 ray_start, V3 ray_dir, V3* enter_point, V3* exit_point) {
  return Ray3IntersectConvexHull3(ray_start, ray_dir, hull_points, hull_points_size, hull_indices, hull_indices_size, hull_offset, enter_point, exit_point);
}

B32 ConvexHull3IntersectAabb3(V3* hull_points, U32 hull_points_size, V3 hull_offset, V3 aabb_center, V3 aabb_size, IntersectManifold3* manifold) {
  V3 aabb_points[8];
  ConvexHull3FromAabb3(aabb_points, NULL, aabb_center, aabb_size);
  return ConvexHull3IntersectConvexHull3(hull_points, hull_points_size, hull_offset, aabb_points, 8, V3_ZEROES, manifold);
}

B32 ConvexHull3IntersectSphere3(V3* hull_points, U32 hull_points_size, V3 hull_offset, V3 sphere_center, F32 sphere_radius, IntersectManifold3* manifold) {
  GjkSupportContext a;
  GjkSupportParamsConvexHull3 a_params;
  a_params.points = hull_points;
  a_params.points_size = hull_points_size;
  a_params.offset = hull_offset;
  a.support_fn = GjkSupportConvexHull3;
  a.user_data = &a_params;

  GjkSupportContext b;
  GjkSupportParamsSphere3 b_params;
  b_params.center = sphere_center;
  b_params.radius = sphere_radius;
  b.support_fn = GjkSupportSphere3;
  b.user_data = &b_params;

  return GjkIntersection3(&a, &b, manifold);
}


B32 ConvexHull3IntersectConvexHull3(V3* a_points, U32 a_points_size, V3 a_offset, V3* b_points, U32 b_points_size, V3 b_offset, IntersectManifold3* manifold) {
  GjkSupportContext a;
  GjkSupportParamsConvexHull3 a_params;
  a_params.points = a_points;
  a_params.points_size = a_points_size;
  a_params.offset = a_offset;
  a.support_fn = GjkSupportConvexHull3;
  a.user_data = &a_params;

  GjkSupportContext b;
  GjkSupportParamsConvexHull3 b_params;
  b_params.points = b_points;
  b_params.points_size = b_points_size;
  b_params.offset = b_offset;
  b.support_fn = GjkSupportConvexHull3;
  b.user_data = &b_params;

  return GjkIntersection3(&a, &b, manifold);
}

B32 Mesh3Validate(U32 mesh_points_size, U32 indices_size) {
  return (mesh_points_size > 2) && (indices_size > 0) && (indices_size % 3 == 0);
}

B32 Mesh3Eq(V3* a_points, U32 a_points_size, U32* a_indices, U32 a_indices_size, V3* b_points, U32 b_points_size, U32* b_indices, U32 b_indices_size) {
  if (a_indices != NULL) {
    if (a_indices_size != b_indices_size) { return false; }
    for (U32 i = 0; i < a_indices_size; i++) {
      if (a_indices[i] != b_indices[i]) { return false; }
    }
  }
  if (a_points_size != b_points_size) { return false; }
  for (U32 i = 0; i < a_points_size; i++) {
    if (!V3Eq(a_points[i], b_points[i])) { return false; }
  }
  return true;
}

B32 Mesh3ApproxEq(V3* a_points, U32 a_points_size, U32* a_indices, U32 a_indices_size, V3* b_points, U32 b_points_size, U32* b_indices, U32 b_indices_size) {
  if (a_indices != NULL) {
    if (a_indices_size != b_indices_size) { return false; }
    for (U32 i = 0; i < a_indices_size; i++) {
      if (a_indices[i] != b_indices[i]) { return false; }
    }
  }
  if (a_points_size != b_points_size) { return false; }
  for (U32 i = 0; i < a_points_size; i++) {
    if (!V3ApproxEq(a_points[i], b_points[i])) { return false; }
  }
  return true;
}

void Mesh3Offset(V3* mesh_points, U32 mesh_points_size, V3 offset) {
  for (U32 i = 0; i < mesh_points_size; i++) {
    mesh_points[i] = V3AddV3(mesh_points[i], offset);
  }
}

void Mesh3RotateAroundPoint(V3* mesh_points, U32 mesh_points_size, V3 point, M3 rot) {
  for (U32 i = 0; i < mesh_points_size; i++) {
    mesh_points[i] = V3RotateAroundV3M3(mesh_points[i], point, rot);
  }
}

void Mesh3FromAabb3(V3 mesh_points[8], U32 mesh_indices[36], V3 aabb_center, V3 aabb_size) {
  V3 min, max;
  Aabb3GetMinMax(aabb_center, aabb_size, &min, &max);
  mesh_points[0] = V3Assign(min.x, min.y, min.z);
  mesh_points[1] = V3Assign(max.x, min.y, min.z);
  mesh_points[2] = V3Assign(max.x, max.y, min.z);
  mesh_points[3] = V3Assign(min.x, max.y, min.z);

  mesh_points[4] = V3Assign(min.x, min.y, max.z);
  mesh_points[5] = V3Assign(max.x, min.y, max.z);
  mesh_points[6] = V3Assign(max.x, max.y, max.z);
  mesh_points[7] = V3Assign(min.x, max.y, max.z);

  if (mesh_indices != NULL) {
    U32 static_mesh_indices[36] = {
      0, 1, 2,   0, 2, 3,
      4, 6, 5,   4, 7, 6,
      3, 2, 6,   3, 6, 7,
      0, 5, 1,   0, 4, 5,
      1, 5, 6,   1, 6, 2,
      0, 3, 7,   0, 7, 4,
    };
    MEMORY_MOVE_STATIC_ARRAY(mesh_indices, static_mesh_indices);
  }
}

V3 Mesh3GetCenter(V3* mesh_points, U32 mesh_points_size) {
  V3 center = V3_ZEROES;
  for (U32 i = 0; i < mesh_points_size; i++) {
    center = V3AddV3(center, mesh_points[i]);
  }
  return V3DivF32(center, mesh_points_size);
}

void Mesh3SetCenter(V3* mesh_points, U32 mesh_points_size, V3 center) {
  V3 curr_center = ConvexHull3GetCenter(mesh_points, mesh_points_size);
  V3 offset = V3SubV3(center, curr_center);
  ConvexHull3Offset(mesh_points, mesh_points_size, offset);
}

void Mesh3GetEnclosingAabb3(V3* mesh_points, U32 mesh_points_size, V3 mesh_offset, V3* aabb_center, V3* aabb_size) {
  *aabb_center = ConvexHull3GetCenter(mesh_points, mesh_points_size);
  V3 max = V3Splat(F32_MIN);
  V3 min = V3Splat(F32_MAX);
  for (U32 i = 0; i < mesh_points_size; i++) {
    V3 to_pt = V3SubV3(V3AddV3(mesh_points[i], mesh_offset), *aabb_center);
    min.x = MIN(min.x, to_pt.x);
    min.y = MIN(min.y, to_pt.y);
    min.z = MIN(min.z, to_pt.z);
    max.x = MAX(max.x, to_pt.x);
    max.y = MAX(max.y, to_pt.y);
    max.z = MAX(max.z, to_pt.z);
  }
  *aabb_size = V3SubV3(max, min);
}

void Mesh3GetEnclosingSphere3(V3* mesh_points, U32 mesh_points_size, V3 mesh_offset, V3* sphere_center, F32* sphere_radius) {
  *sphere_center = ConvexHull3GetCenter(mesh_points, mesh_points_size);
  F32 max_dist = F32_MIN;
  for (U32 i = 0; i < mesh_points_size; i++) {
    V3 to_pt    = V3SubV3(V3AddV3(mesh_points[i], mesh_offset), *sphere_center);
    F32 dist_sq = V3LengthSq(to_pt);
    if (max_dist < dist_sq) { max_dist = dist_sq; }
  }
  *sphere_radius = F32Sqrt(max_dist);
}

B32 Mesh3IntersectLine3(V3* mesh_points, U32 mesh_points_size, U32* mesh_indices, U32 mesh_indices_size, V3 mesh_offset, V3 line_start, V3 line_end, V3* enter_point, V3* exit_point) {
  return Line3IntersectMesh3(line_start, line_end, mesh_points, mesh_points_size, mesh_indices, mesh_indices_size, mesh_offset, enter_point, exit_point);
}

B32 Mesh3IntersectRay3(V3* mesh_points, U32 mesh_points_size, U32* mesh_indices, U32 mesh_indices_size, V3 mesh_offset, V3 ray_start, V3 ray_dir, V3* enter_point, V3* exit_point) {
  return Ray3IntersectMesh3(ray_start, ray_dir, mesh_points, mesh_points_size, mesh_indices, mesh_indices_size, mesh_offset, enter_point, exit_point);
}

typedef struct EpaPolytopeFace EpaPolytopeFace;
struct EpaPolytopeFace {
  V3  points[3];
  V3  normal;
  F32 distance;
};

typedef struct EpaPolytope EpaPolytope;
struct EpaPolytope {
  EpaPolytopeFace* data;
  U32 size;
  U32 capacity;
};

typedef struct EpaHorizonLine EpaHorizonLine;
struct EpaHorizonLine {
  V3  points[2];
  U32 count;
};

typedef struct EpaHorizon EpaHorizon;
struct EpaHorizon {
  EpaHorizonLine* data;
  U32 size;
  U32 capacity;
};

V3 GjkSupportConvexHull3(void* user_data, V3 search_dir) {
  GjkSupportParamsConvexHull3* params = (GjkSupportParamsConvexHull3*) user_data;
  V3  max_point;
  F32 max_point_dot = F32_MIN;
  for (U32 i = 0; i < params->points_size; i++) {
    V3  test     = V3AddV3(params->points[i], params->offset);
    F32 test_dot = V3DotV3(V3AddV3(test, params->offset), search_dir);
    if (test_dot > max_point_dot) {
      max_point     = test;
      max_point_dot = test_dot;
    }
  }
  DEBUG_ASSERT(max_point_dot != F32_MIN);
  return max_point;
}

V3 GjkSupportSphere3(void* user_data, V3 search_dir) {
  DEBUG_ASSERT(V3LengthSq(search_dir) > 0);
  GjkSupportParamsSphere3* params = (GjkSupportParamsSphere3*) user_data;
  V3 result = V3Normalize(search_dir);
  result    = V3MultF32(result, params->radius);
  result    = V3AddV3(result, params->center);
  return result;
}

static void EpaPolytopeAddFace(Arena* arena, EpaPolytope* polytope, V3 p0, V3 p1, V3 p2) {
  // NOTE: cache information like the normal and distance since we check it regularly for EPA.
  EpaPolytopeFace face;
  face.points[0] = p0;
  face.points[1] = p1;
  face.points[2] = p2;

  // DEBUG_ASSERT(Tri3Validate(face.points));
  if (!Tri3Validate(face.points)) {
    face.distance = 0;
    face.normal = V3_ZEROES;
  } else {
    face.normal   = Tri3GetNormal(face.points);
    face.distance = V3DotV3(face.normal, face.points[0]);
    if (face.distance < 0) {
      face.distance = -1 * face.distance;
      face.normal   = V3Negate(face.normal);
    }
  }
  DA_PUSH_BACK(arena, polytope, face);
}

static void EpaHorizonAddLine(Arena* arena, EpaHorizon* horizon, V3 p0, V3 p1) {
  // NOTE: for the horizon, we only want to keep track of edges which join a triangle with a normal that faces the new point with one that does not.
  // this is tracked through a count on each edge. edges meet this criteria if they are only observed a single time. others join 2 triangles that
  // face the new point. then, when creating the superset / union polytope, we only consider an edge with a count == 1, as we know this criteria is met.
  // e.g.
  //
  //    b------c
  //   / \   /
  //  /   \ /
  // a-----d
  //
  // imagine some support point e that extends in Z+ from the line bd. we want to keep lines ad, dc, cb, ba, and remove the line bd.
  // when adding points to this data structure, bd will be counted twice when we add tris adb and bdc.

  for (U32 i = 0; i < horizon->size; i++) {
    EpaHorizonLine* test = &horizon->data[i];
    if ((V3Eq(test->points[0], p0) && V3Eq(test->points[1], p1)) ||
        (V3Eq(test->points[1], p0) && V3Eq(test->points[0], p1))) {
      test->count += 1;
      return;
    }
  }

  EpaHorizonLine line;
  line.points[0] = p0;
  line.points[1] = p1;
  line.count = 1;
  DA_PUSH_BACK(arena, horizon, line);
}

static void GjkSimplexAddPoint(V3 simplex_points[4], U32* simplex_points_size, V3 point) {
  DEBUG_ASSERT(*simplex_points_size < 4);
  // NOTE: simplex points are ordered (first -> last) from newest to oldest.
  for (U32 i = *simplex_points_size; i > 0; i--) { simplex_points[i] = simplex_points[i - 1]; }
  simplex_points[0] = point;
  *simplex_points_size += 1;
}

static B32 GjkSimplexIterLine(V3* simplex_points, U32* simplex_points_size, V3* search_dir) {
  DEBUG_ASSERT(*simplex_points_size == 2);
  V3 a = simplex_points[0];
  V3 b = simplex_points[1];

  V3 ab = V3SubV3(b, a);
  V3 ao = V3Negate(a);

  if (V3DotV3(ab, ao) > 0) {
    // NOTE: origin projects onto the line ab
    *search_dir = V3CrossV3(V3CrossV3(ab, ao), ab);
    return false;

  } else {
    // NOTE: origin does not project onto the line, search towards newly added point.
    simplex_points[0] = a;
    *simplex_points_size = 1;
    *search_dir = ao;
    return false;
  }
}

static B32 GjkSimplexIterTriangle(V3* simplex_points, U32* simplex_points_size, V3* search_dir) {
  DEBUG_ASSERT(*simplex_points_size == 3);
  V3 a = simplex_points[0];
  V3 b = simplex_points[1];
  V3 c = simplex_points[2];

  V3 ab  = V3SubV3(b, a);
  V3 ac  = V3SubV3(c, a);
  V3 ao  = V3Negate(a);
  V3 abc = V3CrossV3(ab, ac);

  V3 abc_cross_ac = V3CrossV3(abc, ac);
  V3 ab_cross_abc = V3CrossV3(ab, abc);

  if (V3DotV3(abc_cross_ac, ao) > 0) {
    if (V3DotV3(ac, ao) > 0) {
      // NOTE: origin is closest to ac
      simplex_points[0] = a;
      simplex_points[1] = c;
      *simplex_points_size = 2;
      *search_dir = V3CrossV3(V3CrossV3(ac, ao), ac);
      return false;

    } else {
      // NOTE: origin is closest to ab or a
      simplex_points[0] = a;
      simplex_points[1] = b;
      *simplex_points_size = 2;
      return GjkSimplexIterLine(simplex_points, simplex_points_size, search_dir);

    }
  } else if (V3DotV3(ab_cross_abc, ao) > 0) {
    // NOTE: origin is closest to ab or a
    simplex_points[0] = a;
    simplex_points[1] = b;
    *simplex_points_size = 2;
    return GjkSimplexIterLine(simplex_points, simplex_points_size, search_dir);

  } else if (V3DotV3(abc, ao) > 0) {
    // NOTE: origin is closest to triangle abc
    *search_dir = abc;
    return false;

  } else {
    // NOTE: origin is closest to triange acb
    simplex_points[0] = a;
    simplex_points[1] = c;
    simplex_points[2] = b;
    *simplex_points_size = 3;
    *search_dir = V3Negate(abc);
    return false;
  }
}

static B32 GjkSimplexIterTetrahedron(V3* simplex_points, U32* simplex_points_size, V3* search_dir) {
  DEBUG_ASSERT(*simplex_points_size == 4);
  V3 a = simplex_points[0];
  V3 b = simplex_points[1];
  V3 c = simplex_points[2];
  V3 d = simplex_points[3];

  V3 ab = V3SubV3(b, a);
  V3 ac = V3SubV3(c, a);
  V3 ad = V3SubV3(d, a);
  V3 ao = V3Negate(a);

  V3 abc = V3CrossV3(ab, ac);
  V3 acd = V3CrossV3(ac, ad);
  V3 adb = V3CrossV3(ad, ab);

  if (V3DotV3(abc, ao) > 0) {
    // NOTE: origin closest to abc
    simplex_points[0] = a;
    simplex_points[1] = b;
    simplex_points[2] = c;
    *simplex_points_size = 3;
    return GjkSimplexIterTriangle(simplex_points, simplex_points_size, search_dir);

  } else if (V3DotV3(acd, ao) > 0) {
    // NOTE: origin closest to acd
    simplex_points[0] = a;
    simplex_points[1] = c;
    simplex_points[2] = d;
    *simplex_points_size = 3;
    return GjkSimplexIterTriangle(simplex_points, simplex_points_size, search_dir);

  } else if (V3DotV3(adb, ao) > 0) {
    // NOTE: origin closest to adb
    simplex_points[0] = a;
    simplex_points[1] = d;
    simplex_points[2] = b;
    *simplex_points_size = 3;
    return GjkSimplexIterTriangle(simplex_points, simplex_points_size, search_dir);

  } else {
    // NOTE: origin is fully enclosed in the tetrahedron
    return true;
  }
}

static V3 GjkSupport(GjkSupportContext* a, GjkSupportContext* b, V3 search_dir) {
  V3 a_max_point = a->support_fn(a->user_data, search_dir);
  V3 b_min_point = b->support_fn(b->user_data, V3Negate(search_dir));
  return V3SubV3(a_max_point, b_min_point);
}

// https://winter.dev/articles/gjk-algorithm
// https://winter.dev/articles/epa-algorithm
// https://github.com/kujukuju/KodaPhysics/
// https://github.com/Another-Ghost/3D-Collision-Detection-and-Resolution-Using-GJK-and-EPA/blob/master/CSC8503/CSC8503Common/GJK.cpp
#define GJK_MAX_ITERATIONS 64
#define EPA_MAX_ITERATIONS 64
B32 GjkIntersection3(GjkSupportContext* a, GjkSupportContext* b, IntersectManifold3* manifold) {
  // NOTE: determine intersection using GJK
  B32 gjk_success = false;
  V3  simplex_points[4];
  MEMORY_ZERO_STATIC_ARRAY(simplex_points);
  U32 simplex_points_size = 0;

  V3 search_dir = V3_Y_POS;
  V3 support = GjkSupport(a, b, search_dir);
  GjkSimplexAddPoint((V3*) simplex_points, &simplex_points_size, support);
  search_dir = V3Negate(search_dir);

  // NOTE: GJK works by building a simplex (tetrahedron) surrounding the origin (perspective of the minkowski difference), which is only
  // possible when the two shapes are intersecting. so, we iteratively add vertices to this tetrahedron based on the "support" fn, which
  // finds extreme vertices along a given search direction. the search direction is also updated each iteration to look towards the origin
  // from the perspective of the most recently added vertex.
  for (U32 iter = 0; iter < GJK_MAX_ITERATIONS; iter++) {
    support = GjkSupport(a, b, search_dir);
    if (V3DotV3(support, search_dir) < 0) {
      // NOTE: no intersection.
      goto gjk_exit;
    }
    GjkSimplexAddPoint((V3*) simplex_points, &simplex_points_size, support);

    B32 contains_origin = false;
    switch (simplex_points_size) {
      case 2:  { contains_origin = GjkSimplexIterLine(simplex_points, &simplex_points_size, &search_dir);        } break;
      case 3:  { contains_origin = GjkSimplexIterTriangle(simplex_points, &simplex_points_size, &search_dir);    } break;
      case 4:  { contains_origin = GjkSimplexIterTetrahedron(simplex_points, &simplex_points_size, &search_dir); } break;
      default: { UNREACHABLE(); } break;
    }

    if (contains_origin) {
      gjk_success = true;
      goto gjk_exit;
    }
  }

gjk_exit:
  if (!gjk_success)     { return false; }
  if (manifold == NULL) { return true;  }
  DEBUG_ASSERT(simplex_points_size == 4);

  // NOTE: determine manifold penetration and normal using EPA
  Arena* horizon_arena = ArenaAllocate();
  EpaHorizon horizon;
  MEMORY_ZERO_STRUCT(&horizon);

  // NOTE: convert from GJK simplex to EPA polytope. this can technically fail if the simplex is ill-formed, but that shouldn't be possible given well-formed inputs.
  // TODO: this likely fails when passing flat shapes in? should investigate further.
  Arena* polytope_arena = ArenaAllocate();
  EpaPolytope polytope;
  MEMORY_ZERO_STRUCT(&polytope);
  EpaPolytopeAddFace(polytope_arena, &polytope, simplex_points[0], simplex_points[1], simplex_points[2]);
  EpaPolytopeAddFace(polytope_arena, &polytope, simplex_points[0], simplex_points[3], simplex_points[1]);
  EpaPolytopeAddFace(polytope_arena, &polytope, simplex_points[0], simplex_points[2], simplex_points[3]);
  EpaPolytopeAddFace(polytope_arena, &polytope, simplex_points[1], simplex_points[3], simplex_points[2]);

  // NOTE: EPA works by iteratively expanding the GJK simplex in a polytope to find the closest face of the minkowski difference to the origin.
  // we do this by finding the current face that is closest to the origin and attempting to expand it further into the minkowski difference hull.
  // once we've found a side that can't be expanded any further, we've found the target face.
  search_dir = V3_ZEROES;
  F32 min_distance = 0;
  for (U32 iter = 0; iter < EPA_MAX_ITERATIONS; iter++) {
    // NOTE: find current closest face to origin.
    EpaPolytopeFace* closest_face = &polytope.data[0];
    for (U32 i = 1; i < polytope.size; i++) {
      EpaPolytopeFace* test = &polytope.data[i];
      if (test->distance < closest_face->distance) { closest_face = test; }
    }
    // NOTE: converged on some search direction, may happen when evaluating EPA on a continuous surface like a sphere.
    // In this case, there's likely some better "true" value, but whatever this is close enough.
    if (V3Eq(search_dir, closest_face->normal)) { goto epa_exit; }
    search_dir   = closest_face->normal;
    min_distance = closest_face->distance;

    support = GjkSupport(a, b, search_dir);
    F32 support_distance = V3DotV3(search_dir, support);
    if (support_distance - min_distance < 1e-5f) {
      // NOTE: success
      goto epa_exit;
    }

    // NOTE: some faces are pointing at new point, meaning there may be a vertex on the face that is inside the new superset polytope.
    // so, we need to remove these faces / reconstruct the new polytope from the polyline describing their horizon.
    ArenaClear(horizon_arena);
    MEMORY_ZERO_STRUCT(&horizon);
    for (U32 i = 0; i < polytope.size; i++) {
      EpaPolytopeFace* face = &polytope.data[i];
      V3 face_to_support = V3SubV3(support, face->points[0]);
      F32 face_norm_dot_support = V3DotV3(face->normal, face_to_support);
      if (face_norm_dot_support - face->distance < 1e-5f) { continue; }

      EpaHorizonAddLine(horizon_arena, &horizon, face->points[0], face->points[1]);
      EpaHorizonAddLine(horizon_arena, &horizon, face->points[1], face->points[2]);
      EpaHorizonAddLine(horizon_arena, &horizon, face->points[2], face->points[0]);
      DA_SWAP_REMOVE(&polytope, i);
      i--;
    }

    // NOTE: reconstruct the polytope -- all points on the horizon connect to the new support point.
    for (U32 i = 0; i < horizon.size; i++) {
      EpaHorizonLine* line = &horizon.data[i];
      // NOTE: if line->count > 1, we saw the line more than once when constructing the horizon. this means the point is not actually on
      // the horizon, e.g. joining two offending triangles that are enclosed by the horizon. see comment in EpaHorizonLineAdd.
      if (line->count > 1) { continue; }
      EpaPolytopeAddFace(polytope_arena, &polytope, line->points[0], line->points[1], support);
    }
  }

epa_exit:
  ArenaRelease(polytope_arena);
  ArenaRelease(horizon_arena);

  // NOTE: EPA can fail in certain cases, e.g. when one object is entirely contained inside of the other, or if the support fn is continuous.
  // In these cases, the behavior here is to just use the most recent search dir w/ minimum distance when EPA iterations are exhausted or it converges.
  // Otherwise, the correct values are populated here when the EPA loop exits.
  manifold->normal = V3Negate(search_dir);
  manifold->penetration = min_distance;

  return true;
}

#endif // CDEFAULT_GEOMETRY_IMPLEMENTATION
