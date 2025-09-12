#ifndef CDEFAULT_GEOMETRY_H_
#define CDEFAULT_GEOMETRY_H_

#include "cdefault_math.h"
#include "cdefault_std.h"

// TODO: OBBs
// TODO: convex polygons, gjk
// TODO: 2/3d capsule?
// TODO: dumb fns, like equals / offsets for all.

// NOTE: Collections of points (e.g. Tri2) must follow CCW winding order.

////////////////////////////////////////////////////////////////////////////////
// 2D shapes
////////////////////////////////////////////////////////////////////////////////

typedef struct Line2 Line2;
struct Line2 { V2 start; V2 end; };

typedef struct Ray2 Ray2;
struct Ray2 { V2 start; V2 dir; };

typedef struct Tri2 Tri2;
struct Tri2 { V2 points[3]; };

typedef struct Aabb2 Aabb2;
struct Aabb2 { V2 center_point; V2 size; };

typedef struct Circle2 Circle2;
struct Circle2 { V2 center_point; F32 radius; };

typedef struct IntersectManifold2 IntersectManifold2;
struct IntersectManifold2 { V2 normal; F32 penetration; };

F32  Line2Length(Line2* line);
F32  Line2LengthSq(Line2* line);
void Line2ClosestPoint(Line2* line, V2* point, V2* closest);
B32  Line2IntersectLine2(Line2* line_0, Line2* line_1, V2* intersect_point);
B32  Line2IntersectRay2(Line2* line, Ray2* ray, V2* intersect_point);
B32  Line2IntersectTri2(Line2* line, Tri2* tri, V2* enter_point, V2* exit_point);
B32  Line2IntersectAabb2(Line2* line, Aabb2* aabb, V2* enter_point, V2* exit_point);
B32  Line2IntersectCircle2(Line2* line, Circle2* circle, V2* enter_point, V2* exit_point);

void Ray2DirInv(Ray2* ray, V2* dir_inv);
B32  Ray2IntersectLine2(Ray2* ray, Line2* line, V2* intersect_point);
B32  Ray2IntersectRay2(Ray2* ray_0, Ray2* ray_2, V2* intersect_point);
B32  Ray2IntersectTri2(Ray2* ray, Tri2* tri, V2* enter_point, V2* exit_point);
B32  Ray2IntersectAabb2(Ray2* ray, Aabb2* aabb, V2* enter_point, V2* exit_point);
B32  Ray2IntersectCircle2(Ray2* ray, Circle2* circle, V2* enter_point, V2* exit_point);

void Tri2Offset(Tri2* dest, Tri2* src, V2* offset);
B32  Tri2ContainsPoint(Tri2* src, V2* point);
B32  Tri2IntersectLine2(Tri2* tri, Line2* line, V2* enter_point, V2* exit_point);
B32  Tri2IntersectRay2(Tri2* tri, Ray2* ray, V2* enter_point, V2* exit_point);
B32  Tri2IntersectTri2(Tri2* tri_0, Tri2* tri_1, IntersectManifold2* manifold);
B32  Tri2IntersectAabb2(Tri2* tri, Aabb2* aabb, IntersectManifold2* manifold);
B32  Tri2IntersectCircle2(Tri2* tri, Circle2* circle, IntersectManifold2* manifold);

void Aabb2FromMinMax(Aabb2* aabb2, V2* min, V2* max);
void Aabb2FromTopLeft(Aabb2* aabb2, V2* point, V2* size);
void Aabb2GetMinMax(Aabb2* aabb2, V2* min, V2* max);
B32  Aabb2ContainsPoint(Aabb2* aabb2, V2* point);
B32  Aabb2IntersectLine2(Aabb2* aabb, Line2* line, V2* enter_point, V2* exit_point);
B32  Aabb2IntersectRay2(Aabb2* aabb, Ray2* ray, V2* enter_point, V2* exit_point);
B32  Aabb2IntersectTri2(Aabb2* aabb, Tri2* tri, IntersectManifold2* manifold);
B32  Aabb2IntersectAabb2(Aabb2* aabb_0, Aabb2* aabb_1, IntersectManifold2* manifold);
B32  Aabb2IntersectCircle2(Aabb2* aabb, Circle2* circle, IntersectManifold2* manifold);

B32  Circle2ContainsPoint(Circle2* circle, V2* point);
B32  Circle2IntersectLine2(Circle2* circle, Line2* line, V2* enter_point, V2* exit_point);
B32  Circle2IntersectRay2(Circle2* circle, Ray2* ray, V2* enter_point, V2* exit_point);
B32  Circle2IntersectTri2(Circle2* a, Tri2* b, IntersectManifold2* manifold);
B32  Circle2IntersectAabb2(Circle2* a, Aabb2* b, IntersectManifold2* manifold);
B32  Circle2IntersectCircle2(Circle2* a, Circle2* b, IntersectManifold2* manifold);

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

void Ray3FromRay2(Ray3* ray3, Ray2* ray2);
void Ray3DirInv(Ray3* r, V3* dir_inv);
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

void Tri3FromTri2(Tri3* tri3, Tri2* tri2);
void Tri3Offset(Tri3* dest, Tri3* src, V3* offset);
B32  Tri3CreatePlane3(Tri3* t, Plane3* plane);
B32  Tri3ContainsPoint(Tri3* t, V3* point);

void CubeRelativeNormal(V3* cube_center_point, V3* pt, V3* normal);
void Aabb3FromAabb2(Aabb3* aabb3, Aabb2* aabb2);
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

F32 Line2Length(Line2* line) {
  return F32Sqrt(Line2LengthSq(line));
}

F32 Line2LengthSq(Line2* line) {
  V2 v;
  V2SubV2(&v, &line->end, &line->start);
  return V2LengthSq(&v);
}

void Line2ClosestPoint(Line2* line, V2* point, V2* closest) {
  V2* a = &line->start;
  V2* b = &line->end;
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

// SPEEDUP: for all line intersections, we just convert to ray and then determine if the
// intersection point is beyond the segment's end. there's probably a more elegant way to do
// this that e.g. doesn't force collection of the ray intersection point unless asked for?
B32 Line2IntersectLine2(Line2* line_0, Line2* line_1, V2* intersect_point) {
  if (UNLIKELY(Line2LengthSq(line_1) == 0)) { return false; }

  Ray2 ray_1;
  ray_1.start = line_1->start;
  V2SubV2(&ray_1.dir, &line_1->end, &line_1->start);
  V2Normalize(&ray_1.dir, &ray_1.dir);

  V2 i;
  if (!Line2IntersectRay2(line_0, &ray_1, &i)) { return false; }

  Line2 test;
  test.start = line_1->start;
  test.end = i;
  if (Line2LengthSq(line_1) < Line2LengthSq(&test)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Line2IntersectRay2(Line2* line, Ray2* ray, V2* intersect_point) {
  if (UNLIKELY(Line2LengthSq(line) == 0)) { return false; }

  Ray2 ray_0;
  ray_0.start = line->start;
  V2SubV2(&ray_0.dir, &line->end, &line->start);
  V2Normalize(&ray_0.dir, &ray_0.dir);

  V2 i;
  if (!Ray2IntersectRay2(&ray_0, ray, &i)) { return false; }

  Line2 test;
  test.start = line->start;
  test.end = i;
  if (Line2LengthSq(line) < Line2LengthSq(&test)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Line2IntersectTri2(Line2* line, Tri2* tri, V2* enter_point, V2* exit_point) {
  if (UNLIKELY(Line2LengthSq(line) == 0)) { return false; }

  Ray2 ray;
  ray.start = line->start;
  V2SubV2(&ray.dir, &line->end, &line->start);
  V2Normalize(&ray.dir, &ray.dir);

  V2 enter, exit;
  if (!Ray2IntersectTri2(&ray, tri, &enter, &exit)) { return false; }

  Line2 test;
  test.start = line->start;
  test.end = enter;
  if (Line2LengthSq(line) < Line2LengthSq(&test)) { return false; }
  test.end = exit;
  if (Line2LengthSq(line) < Line2LengthSq(&test)) { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL) { *exit_point = exit; }
  return true;
}

B32 Line2IntersectAabb2(Line2* line, Aabb2* aabb, V2* enter_point, V2* exit_point) {
  if (UNLIKELY(Line2LengthSq(line) == 0)) { return false; }

  Ray2 ray;
  ray.start = line->start;
  V2SubV2(&ray.dir, &line->end, &line->start);
  V2Normalize(&ray.dir, &ray.dir);

  V2 enter, exit;
  if (!Ray2IntersectAabb2(&ray, aabb, &enter, &exit)) { return false; }

  Line2 test;
  test.start = line->start;
  test.end = enter;
  if (Line2LengthSq(line) < Line2LengthSq(&test)) { return false; }
  test.end = exit;
  if (Line2LengthSq(line) < Line2LengthSq(&test)) { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL) { *exit_point = exit; }
  return true;
}

B32 Line2IntersectCircle2(Line2* line, Circle2* circle, V2* enter_point, V2* exit_point) {
  if (UNLIKELY(Line2LengthSq(line) == 0)) { return false; }

  Ray2 ray;
  ray.start = line->start;
  V2SubV2(&ray.dir, &line->end, &line->start);
  V2Normalize(&ray.dir, &ray.dir);

  V2 enter, exit;
  if (!Ray2IntersectCircle2(&ray, circle, &enter, &exit)) { return false; }

  Line2 test;
  test.start = line->start;
  test.end = enter;
  if (Line2LengthSq(line) < Line2LengthSq(&test)) { return false; }
  test.end = exit;
  if (Line2LengthSq(line) < Line2LengthSq(&test)) { exit = enter; }

  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL) { *exit_point = exit; }
  return true;
}

void Ray2DirInv(Ray2* ray, V2* dir_inv) {
  dir_inv->x = 1.0f / ray->dir.x;
  dir_inv->y = 1.0f / ray->dir.y;
}

B32 Ray2IntersectLine2(Ray2* ray, Line2* line, V2* intersect_point) {
  Ray2 line_ray;
  line_ray.start = line->start;
  V2SubV2(&line_ray.dir, &line->end, &line->start);
  V2Normalize(&line_ray.dir, &line_ray.dir);

  V2 i;
  if (!Ray2IntersectRay2(ray, &line_ray, &i)) { return false; }

  Line2 test;
  test.start = line->start;
  test.end = i;
  if (Line2LengthSq(line) < Line2LengthSq(&test)) { return false; }

  if (intersect_point != NULL) { *intersect_point = i; }
  return true;
}

B32 Ray2IntersectRay2(Ray2* ray_0, Ray2* ray_1, V2* intersect_point) {
  F32 cross = V2CrossV2(&ray_1->dir, &ray_0->dir);
  if (cross == 0) { return false; }
  F32 d_x = ray_1->start.x - ray_0->start.x;
  F32 d_y = ray_1->start.y - ray_0->start.y;
  F32 u = ((d_y * ray_1->dir.x) - (d_x * ray_1->dir.y)) / cross;
  F32 v = ((d_y * ray_0->dir.x) - (d_x * ray_0->dir.y)) / cross;
  if (u < 0 || v < 0) { return false; }
  if (intersect_point != NULL) {
    V2MultF32(intersect_point, &ray_0->dir, u);
    V2AddV2(intersect_point, &ray_0->start, intersect_point);
  }
  return true;
}

B32 Ray2IntersectTri2(Ray2* ray, Tri2* tri, V2* enter_point, V2* exit_point) {
  Line2 p0p1, p1p2, p2p0;
  p0p1.start = tri->points[0]; p0p1.end = tri->points[1];
  p1p2.start = tri->points[1]; p1p2.end = tri->points[2];
  p2p0.start = tri->points[2]; p2p0.end = tri->points[0];
  V2 i_p0p1, i_p1p2, i_p2p0;
  B32 t_i_p0p1 = Ray2IntersectLine2(ray, &p0p1, &i_p0p1);
  B32 t_i_p1p2 = Ray2IntersectLine2(ray, &p1p2, &i_p1p2);
  B32 t_i_p2p0 = Ray2IntersectLine2(ray, &p2p0, &i_p2p0);
  if (!t_i_p0p1 && !t_i_p1p2 && !t_i_p2p0) { return false; }
  if (enter_point == NULL && exit_point == NULL) { return true; }
  F32 min = F32_MAX; F32 max = 0;
  V2 enter, exit;
  Line2 test; test.start = ray->start;
  F32 d;
#define CDEFAULT_PENETRATION_TEST(t, p)  \
  if (t) {                               \
    test.end = p;                        \
    d = Line2LengthSq(&test);            \
    if (d < min) { enter = p; min = d; } \
    if (d > max) { exit = p; max = d; }  \
  }
  CDEFAULT_PENETRATION_TEST(t_i_p0p1, i_p0p1);
  CDEFAULT_PENETRATION_TEST(t_i_p1p2, i_p1p2);
  CDEFAULT_PENETRATION_TEST(t_i_p2p0, i_p2p0);
#undef CDEFAULT_PENETRATION_TEST
  if (enter_point != NULL) { *enter_point = enter; }
  if (exit_point != NULL) { *exit_point = exit; }
  return true;
}

B32 Ray2IntersectAabb2(Ray2* ray, Aabb2* aabb, V2* enter_point, V2* exit_point) {
  V2 aabb_min, aabb_max;
  Aabb2GetMinMax(aabb, &aabb_min, &aabb_max);
  V2 ray_dir_inv;
  Ray2DirInv(ray, &ray_dir_inv);

  V2 t_1, t_2;
  V2SubV2(&t_1, &aabb_min, &ray->start);
  V2HadamardV2(&t_1, &t_1, &ray_dir_inv);
  V2SubV2(&t_2, &aabb_max, &ray->start);
  V2HadamardV2(&t_2, &t_2, &ray_dir_inv);

  F32 t_min = MAX(MIN(t_1.x, t_2.x), MIN(t_1.y, t_2.y));
  F32 t_max = MIN(MAX(t_1.x, t_2.x), MAX(t_1.y, t_2.y));
  if (t_max < t_min) { return false; }

  if (t_min < 0) {
    if (t_max < 0) { return false; }
    t_min = t_max;
  }

  if (enter_point != NULL) {
    V2MultF32(enter_point, &ray->dir, t_min);
    V2AddV2(enter_point, &ray->start, enter_point);
  }
  if (exit_point != NULL) {
    V2MultF32(exit_point, &ray->dir, t_max);
    V2AddV2(exit_point, &ray->start, exit_point);
  }
  return true;
}

B32 Ray2IntersectCircle2(Ray2* ray, Circle2* circle, V2* enter_point, V2* exit_point) {
  V2 rel_center;
  V2SubV2(&rel_center, &circle->center_point, &ray->start);
  F32 to_perp_center = V2DotV2(&rel_center, &ray->dir);
  F32 d2 = V2DotV2(&rel_center, &rel_center) - (to_perp_center * to_perp_center);
  if (d2 > circle->radius * circle->radius) { return false; }
  if (enter_point == NULL && exit_point == NULL) { return true; }
  F32 center_to_edge = F32Sqrt(circle->radius * circle->radius - d2);
  F32 t0 = to_perp_center - center_to_edge;
  F32 t1 = to_perp_center + center_to_edge;
  if (t1 < t0) { SWAP(F32, t0, t1); }
  if (t0 < 0) {
    if (t1 < 0) { return false; }
    t0 = t1;
  }
  if (enter_point != NULL) {
    V2MultF32(enter_point, &ray->dir, t0);
    V2AddV2(enter_point, &ray->start, enter_point);
  }
  if (exit_point != NULL) {
    V2MultF32(exit_point, &ray->dir, t1);
    V2AddV2(exit_point, &ray->start, exit_point);
  }
  return true;
}


void Tri2Offset(Tri2* dest, Tri2* src, V2* offset) {
  MEMORY_ZERO_STRUCT(dest);
  V2AddV2(&src->points[0], offset, &dest->points[0]);
  V2AddV2(&src->points[1], offset, &dest->points[1]);
  V2AddV2(&src->points[2], offset, &dest->points[2]);
}

B32 Tri2ContainsPoint(Tri2* src, V2* point) {
  V2* p = src->points;
  F32 denom = ((p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y));
  F32 a = ((p[1].y - p[2].y) * (point->x - p[2].x) + (p[2].x - p[1].x) * (point->y - p[2].y)) / denom;
  F32 b = ((p[2].y - p[0].y) * (point->x - p[2].x) + (p[0].x - p[2].x) * (point->y - p[2].y)) / denom;
  F32 c = 1 - a - b;
  return a >= 0 && b >= 0 && c >= 0;
}

B32 Tri2IntersectLine2(Tri2* tri, Line2* line, V2* enter_point, V2* exit_point) {
  return Line2IntersectTri2(line, tri, enter_point, exit_point);
}

B32 Tri2IntersectRay2(Tri2* tri, Ray2* ray, V2* enter_point, V2* exit_point) {
  return Ray2IntersectTri2(ray, tri, enter_point, exit_point);
}

B32 Tri2IntersectTri2(Tri2* tri_0, Tri2* tri_1, IntersectManifold2* manifold) {
  TODO();
  tri_0 = tri_0;
  tri_1 = tri_1;
  manifold = manifold;
  return false;
}

B32 Tri2IntersectAabb2(Tri2* tri, Aabb2* aabb, IntersectManifold2* manifold) {
  TODO();
  tri = tri;
  aabb = aabb;
  manifold = manifold;
  return false;
}

B32 Tri2IntersectCircle2(Tri2* tri, Circle2* circle, IntersectManifold2* manifold) {
  return Circle2IntersectTri2(circle, tri, manifold);
}

void Aabb2FromMinMax(Aabb2* aabb2, V2* min, V2* max) {
  DEBUG_ASSERT(min->x < max->x);
  DEBUG_ASSERT(min->y < max->y);
  aabb2->center_point.x = (min->x + max->x) / 2.0f;
  aabb2->center_point.y = (min->y + max->y) / 2.0f;
  aabb2->size.x = max->x - min->x;
  aabb2->size.y = max->y - min->y;
}

void Aabb2FromTopLeft(Aabb2* aabb2, V2* point, V2* size) {
  aabb2->center_point.x = point->x + size->x / 2.0f;
  aabb2->center_point.y = point->y - size->y / 2.0f;
  aabb2->size = *size;
}

void Aabb2GetMinMax(Aabb2* aabb2, V2* min, V2* max) {
  min->x = aabb2->center_point.x - (aabb2->size.x / 2.0f);
  min->y = aabb2->center_point.y - (aabb2->size.y / 2.0f);
  max->x = aabb2->center_point.x + (aabb2->size.x / 2.0f);
  max->y = aabb2->center_point.y + (aabb2->size.y / 2.0f);
}

B32 Aabb2ContainsPoint(Aabb2* aabb2, V2* point) {
  V2 min, max;
  Aabb2GetMinMax(aabb2, &min, &max);
  return (min.x <= point->x && point->x <= max.x) &&
         (min.y <= point->y && point->y <= max.y);
}

B32 Aabb2IntersectLine2(Aabb2* aabb, Line2* line, V2* enter_point, V2* exit_point) {
  return Line2IntersectAabb2(line, aabb, enter_point, exit_point);
}

B32 Aabb2IntersectRay2(Aabb2* aabb, Ray2* ray, V2* enter_point, V2* exit_point) {
  return Ray2IntersectAabb2(ray, aabb, enter_point, exit_point);
}

B32 Aabb2IntersectTri2(Aabb2* aabb, Tri2* tri, IntersectManifold2* manifold) {
  TODO();
  aabb = aabb;
  tri = tri;
  manifold = manifold;
  return false;
}

B32 Aabb2IntersectAabb2(Aabb2* a, Aabb2* b, IntersectManifold2* manifold) {
  V2 a_min, a_max;
  Aabb2GetMinMax(a, &a_min, &a_max);
  V2 b_min, b_max;
  Aabb2GetMinMax(b, &b_min, &b_max);

  if (!(a_max.x > b_min.x &&
        a_min.x < b_max.x &&
        a_max.y > b_min.y &&
        a_min.y < b_max.y)) {
    return false;
  }

  if (manifold != NULL) {
    F32 penetration = F32_MAX;
    F32 penetration_test = 0;
    V2 min_axis = {};
#define CDEFAULT_PENETRATION_TEST(test, axis) \
      penetration_test = (test);              \
      if (penetration_test < penetration) {   \
        penetration = penetration_test;       \
        min_axis = (axis);                    \
      }
    CDEFAULT_PENETRATION_TEST(b_max.x - a_min.x, V2_X_NEG);
    CDEFAULT_PENETRATION_TEST(a_max.x - b_min.x, V2_X_POS);
    CDEFAULT_PENETRATION_TEST(b_max.y - a_min.y, V2_Y_NEG);
    CDEFAULT_PENETRATION_TEST(a_max.y - b_min.y, V2_Y_POS);
#undef CDEFAULT_PENETRATION_TEST
    manifold->normal = min_axis;
    manifold->penetration = penetration;
  }
  return true;
}

B32 Aabb2IntersectCircle2(Aabb2* aabb, Circle2* circle, IntersectManifold2* manifold) {
  B32 result = Circle2IntersectAabb2(circle, aabb, manifold);
  if (manifold != NULL) { V2MultF32(&manifold->normal, &manifold->normal, -1); }
  return result;
}

B32 Circle2ContainsPoint(Circle2* circle, V2* point) {
  V2 to_pt;
  V2SubV2(&to_pt, point, &circle->center_point);
  return V2LengthSq(&to_pt) < (circle->radius * circle->radius);
}

B32 Circle2IntersectLine2(Circle2* circle, Line2* line, V2* enter_point, V2* exit_point) {
  return Line2IntersectCircle2(line, circle, enter_point, exit_point);
}

B32 Circle2IntersectRay2(Circle2* circle, Ray2* ray, V2* enter_point, V2* exit_point) {
  return Ray2IntersectCircle2(ray, circle, enter_point, exit_point);
}

B32 Circle2IntersectTri2(Circle2* a, Tri2* b, IntersectManifold2* manifold) {
  Line2 p0p1, p1p2, p2p0;
  p0p1.start = b->points[0]; p0p1.end = b->points[1];
  p1p2.start = b->points[1]; p1p2.end = b->points[2];
  p2p0.start = b->points[2]; p2p0.end = b->points[0];
  V2 p_p0p1, p_p1p2, p_p2p0;
  Line2ClosestPoint(&p0p1, &a->center_point, &p_p0p1);
  Line2ClosestPoint(&p1p2, &a->center_point, &p_p1p2);
  Line2ClosestPoint(&p2p0, &a->center_point, &p_p2p0);
  V2 to_p_p0p1, to_p_p1p2, to_p_p2p0;
  V2SubV2(&to_p_p0p1, &p_p0p1, &a->center_point);
  V2SubV2(&to_p_p1p2, &p_p1p2, &a->center_point);
  V2SubV2(&to_p_p2p0, &p_p2p0, &a->center_point);
  F32 to_p_p0p1_len_sq = V2LengthSq(&to_p_p0p1);
  F32 to_p_p1p2_len_sq = V2LengthSq(&to_p_p1p2);
  F32 to_p_p2p0_len_sq = V2LengthSq(&to_p_p2p0);

  V2 min_to_p = to_p_p0p1;
  F32 min_len_sq = to_p_p0p1_len_sq;
  if (to_p_p1p2_len_sq < min_len_sq) {
    min_to_p = to_p_p1p2;
    min_len_sq = to_p_p1p2_len_sq;
  }
  if (to_p_p2p0_len_sq < min_len_sq) {
    min_to_p = to_p_p2p0;
    min_len_sq = to_p_p2p0_len_sq;
  }

  F32 min_len = F32Sqrt(min_len_sq);
  if (Tri2ContainsPoint(b, &a->center_point)) {
    // SPEEDUP: wasting cycles when manifold == NULL here, but this is more readable for now.
    if (manifold != NULL) {
      manifold->penetration = a->radius + min_len;
      if (min_len != 0) {
        V2Normalize(&manifold->normal, &min_to_p);
      }
    }
    return true;
  } else if (min_len < a->radius) {
    if (manifold != NULL) {
      manifold->penetration = a->radius - min_len;
      if (min_len != 0) {
        V2Normalize(&manifold->normal, &min_to_p);
        V2MultF32(&manifold->normal, &manifold->normal, -1.0f);
      }
    }
    return true;
  } else {
    return false;
  }
}

B32 Circle2IntersectAabb2(Circle2* a, Aabb2* b, IntersectManifold2* manifold) {
  V2 min, max;
  Aabb2GetMinMax(b, &min, &max);

  V2 rel_closest_point;
  rel_closest_point.x = CLAMP(min.x, a->center_point.x, max.x);
  rel_closest_point.y = CLAMP(min.y, a->center_point.y, max.y);

  V2 to_closest_point;
  V2SubV2(&to_closest_point, &a->center_point, &rel_closest_point);
  F32 d_sq = V2LengthSq(&to_closest_point);
  F32 r_sq = a->radius * a->radius;

  if (d_sq > 0) {
    // NOTE: Outside the AABB
    if (d_sq >= r_sq) { return false; }
    if (manifold != NULL) {
      manifold->penetration = a->radius - F32Sqrt(d_sq);
      manifold->normal = to_closest_point;
      DEBUG_ASSERT(V2Length(&manifold->normal) > 0);
      V2Normalize(&manifold->normal, &manifold->normal);
    }
    return true;
  } else {
    // NOTE: Inside the AABB.
    F32 dx_left  = a->center_point.x - min.x;
    F32 dx_right = max.x - a->center_point.x;
    F32 dy_bot   = a->center_point.y - min.y;
    F32 dy_top   = max.y - a->center_point.y;

    F32 min = dx_left;
    V2 normal = V2_X_NEG;
    if (dx_right < min) {
      min = dx_right;
      normal = V2_X_POS;
    }
    if (dy_bot < min) {
      min = dy_bot;
      normal = V2_Y_NEG;
    }
    if (dy_top < min) {
      min = dy_top;
      normal = V2_Y_POS;
    }

    manifold->normal = normal;
    manifold->penetration = a->radius + min;
    return true;
  }
}

B32 Circle2IntersectCircle2(Circle2* a, Circle2* b, IntersectManifold2* manifold) {
  V2 center_diff;
  V2SubV2(&center_diff, &a->center_point, &b->center_point);
  F32 min_distance = a->radius + b->radius;
  F32 distance = V2Length(&center_diff);
  if (min_distance < distance) { return false; }

  if (manifold != NULL) {
    if (V2LengthSq(&center_diff) == 0) { center_diff.y = 1.0f; }
    V2Normalize(&manifold->normal, &center_diff);
    manifold->penetration = min_distance - distance;
  }
  return true;
}

void Ray3FromRay2(Ray3* ray3, Ray2* ray2) {
  V3FromV2(&ray3->start, &ray2->start);
  V3FromV2(&ray3->dir, &ray2->dir);
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

void Tri3FromTri2(Tri3* tri3, Tri2* tri2) {
  V3FromV2(&tri3->points[0], &tri2->points[0]);
  V3FromV2(&tri3->points[1], &tri2->points[1]);
  V3FromV2(&tri3->points[2], &tri2->points[2]);
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

void Aabb3FromAabb2(Aabb3* aabb3, Aabb2* aabb2) {
  V3FromV2(&aabb3->center_point, &aabb2->center_point);
  V3FromV2(&aabb3->size, &aabb2->size);
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
    CDEFAULT_PENETRATION_TEST(b_max.x - a_min.x, V3_X_NEG);
    CDEFAULT_PENETRATION_TEST(a_max.x - b_min.x, V3_X_POS);
    CDEFAULT_PENETRATION_TEST(b_max.y - a_min.y, V3_Y_NEG);
    CDEFAULT_PENETRATION_TEST(a_max.y - b_min.y, V3_Y_POS);
    CDEFAULT_PENETRATION_TEST(b_max.z - a_min.z, V3_Z_NEG);
    CDEFAULT_PENETRATION_TEST(a_max.z - b_min.z, V3_Z_POS);
#undef CDEFAULT_PENETRATION_TEST
    manifold->normal = min_axis;
    manifold->penetration = penetration;
  }
  return true;
}

#endif // CDEFAULT_GEOMETRY_IMPLEMENTATION
