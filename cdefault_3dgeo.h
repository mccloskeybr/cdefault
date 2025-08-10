#ifndef CDEFAULT_3DGEOMETRY_H_
#define CDEFAULT_3DGEOMETRY_H_

#include "cdefault_math.h"
#include "cdefault_std.h"

///////////////////////////////////////////////////////////////////////////////
// NOTE: Rect3
///////////////////////////////////////////////////////////////////////////////

typedef struct Rect3 Rect3;
struct Rect3 {
  V3 center_point;
  V3 size;
};

Rect3 Rect3FromMinMax(V3 min, V3 max);
void Rect3GetMinMax(Rect3 rect, V3* min, V3* max);
V3 Rect3RelativeNormal(Rect3 rect, V3 p);
B32 Rect3ContainsPoint(Rect3 rect, V3 pos);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Tri3
///////////////////////////////////////////////////////////////////////////////

typedef struct Tri3 Tri3;
struct Tri3 {
  V3 points[3];
};

Tri3 Tri3Offset(Tri3 t, V3 offset);
B32 Tri3CreatePlane3(Tri3 t, struct Plane3* plane);
B32 Tri3ContainsPoint(Tri3 t, V3 point);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Sphere3
///////////////////////////////////////////////////////////////////////////////

typedef struct Sphere3 Sphere3;
struct Sphere3 {
  V3 center_point;
  F32 radius;
};

///////////////////////////////////////////////////////////////////////////////
// NOTE: Ray3
///////////////////////////////////////////////////////////////////////////////

typedef struct Ray3 Ray3;
struct Ray3 {
  V3 start;
  V3 dir;
};

V3 Ray3DirInv(Ray3 r);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Plane3
///////////////////////////////////////////////////////////////////////////////

typedef struct Plane3 Plane3;
struct Plane3 {
  V3 normal;
  F32 d;
};

Plane3 Plane3WithAnchor(V3 anchor, V3 normal);
void Plane3Basis(Plane3 p, V3* u, V3* v);
B32 Plane3IsPointAbove(Plane3 p, V3 point);
B32 Plane3ContainsPoint(Plane3 p, V3 point);
V3 Plane3CreatePoint(Plane3 p);
Plane3 Plane3Flip(Plane3 p);
B32 Plane3ApproxEq(Plane3 a, Plane3 b);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Intersection
///////////////////////////////////////////////////////////////////////////////

B32 IntersectionRay3Plane3(Ray3 ray, Plane3 plane, V3* intersect_point);
B32 IntersectionRay3Rect3(Ray3 ray, Rect3 rect, V3* enter_point, V3* exit_point);
B32 IntersectionRay3Tri3(Ray3 ray, Tri3 tri, V3* intersect_point);

typedef struct IntersectionManifold3 IntersectionManifold3;
struct IntersectionManifold3 {
  V3 normal;
  F32 penetration;
};

B32 IntersectionSphere3Sphere3(Sphere3 a, Sphere3 b, IntersectionManifold3* manifold);
B32 IntersectionSphere3Rect3(Sphere3 a, Rect3 b, IntersectionManifold3* manifold);
B32 IntersectionRect3Rect3(Rect3 a, Rect3 b, IntersectionManifold3* manifold);

#endif // CDEFAULT_3DGEOMETRY_H_

#ifdef CDEFAULT_3DGEOMETRY_IMPLEMENTATION
#undef CDEFAULT_3DGEOMETRY_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// NOTE: Rect3 implementation
///////////////////////////////////////////////////////////////////////////////

Rect3 Rect3FromMinMax(V3 min, V3 max) {
  DEBUG_ASSERT(min.x < max.x);
  DEBUG_ASSERT(min.y < max.y);
  DEBUG_ASSERT(min.z < max.z);
  Rect3 result;
  result.center_point.x = (min.x + max.x) / 2.0f;
  result.center_point.y = (min.y + max.y) / 2.0f;
  result.center_point.z = (min.z + max.z) / 2.0f;
  result.size.x = max.x - min.x;
  result.size.y = max.y - min.y;
  result.size.z = max.z - min.z;
  return result;
}

void Rect3GetMinMax(Rect3 rect, V3* min, V3* max) {
  min->x = rect.center_point.x - (rect.size.x / 2.0f);
  min->y = rect.center_point.y - (rect.size.y / 2.0f);
  min->z = rect.center_point.z - (rect.size.z / 2.0f);
  max->x = rect.center_point.x + (rect.size.x / 2.0f);
  max->y = rect.center_point.y + (rect.size.y / 2.0f);
  max->z = rect.center_point.z + (rect.size.z / 2.0f);
}

// https://stackoverflow.com/questions/75811830/determine-the-face-of-a-cube-intersected-by-a-ray-in-a-minecraft-like-game
V3 Rect3RelativeNormal(Rect3 rect, V3 p) {
  p = V3SubV3(p, rect.center_point);
  F32 p_max = MAX3(p.x, p.y, p.z);
  F32 p_min = MIN3(p.x, p.y, p.z);
  F32 abs_p_min = F32Abs(p_min);
  if (p_max == p.x && p_max > abs_p_min) { return V3_X_POS; }
  if (p_min == p.x && p_max < abs_p_min) { return V3_X_NEG; }
  if (p_max == p.y && p_max > abs_p_min) { return V3_Y_POS; }
  if (p_min == p.y && p_max < abs_p_min) { return V3_Y_NEG; }
  if (p_max == p.z && p_max > abs_p_min) { return V3_Z_POS; }
  if (p_min == p.z && p_max < abs_p_min) { return V3_Z_NEG; }
  UNREACHABLE(); return V3Splat(0);
}

B32 Rect3ContainsPoint(Rect3 rect, V3 pos) {
  V3 min;
  V3 max;
  Rect3GetMinMax(rect, &min, &max);
  return ((min.x <= pos.x && pos.x <= max.x) &&
          (min.y <= pos.y && pos.y <= max.y) &&
          (min.z <= pos.z && pos.z <= max.z));
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Tri3 implementation
///////////////////////////////////////////////////////////////////////////////

Tri3 Offset(Tri3 t, V3 offset) {
  Tri3 result;
  for (U32 i = 0; i < 3; i++) {
    result.points[i] = V3AddV3(t.points[i], offset);
  }
  return result;
}

B32 Tri3CreatePlane3(Tri3 t, Plane3* p) {
  V3 a = V3SubV3(t.points[1], t.points[0]);
  V3 b = V3SubV3(t.points[2], t.points[0]);

  // NOTE: tri points are too close to each other
  if (F32ApproxEq(V3LengthSq(a), 0) || F32ApproxEq(V3LengthSq(b), 0)) {
    return false;
  }
  a = V3Normalize(a);
  b = V3Normalize(b);

  V3 tri_norm = V3CrossV3(a, b);
  // NOTE: tri points along the same line
  if (V3LengthSq(tri_norm) == 0) { return false; }

  *p = Plane3WithAnchor(t.points[0], V3Normalize(tri_norm));
  return true;
}

// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html
B32 Tri3ContainsPoint(Tri3 t, V3 point) {
  V3 a = V3SubV3(t.points[0], point);
  V3 b = V3SubV3(t.points[1], point);
  V3 c = V3SubV3(t.points[2], point);

  V3 u = V3CrossV3(b, c);
  V3 v = V3CrossV3(c, a);
  V3 w = V3CrossV3(a, b);

  if (V3DotV3(a, v) < 0) { return false; }
  if (V3DotV3(u, w) < 0) { return false; }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Ray3 implementation
///////////////////////////////////////////////////////////////////////////////

V3 Ray3DirInv(Ray3 r) {
  V3 result;
  result.x = 1.0f / r.dir.x;
  result.y = 1.0f / r.dir.y;
  result.z = 1.0f / r.dir.z;
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Plane3 implementation
///////////////////////////////////////////////////////////////////////////////

// https://mathinsight.org/distance_point_plane
Plane3 Plane3WithAnchor(V3 anchor, V3 normal) {
  Plane3 result;
  result.normal = normal;
  result.d = V3DotV3(anchor, normal);
  return result;
}

// https://math.stackexchange.com/questions/1702572/how-to-find-the-basis-of-a-plane-or-a-line
// NOTE: *not* orthonormal.
void Plane3Basis(Plane3 p, V3* u, V3* v) {
  if (p.normal.x != 0) {
    *u = (V3) {(p.d - p.normal.z) / p.normal.x, 0, 1};
    *v = (V3) {(p.d - p.normal.y) / p.normal.x, 1, 0};
  } else if (p.normal.y != 0) {
    *u = (V3) {1, (p.d - p.normal.x) / p.normal.y, 0};
    *v = (V3) {0, (p.d - p.normal.z) / p.normal.y, 1};
  } else if (p.normal.z != 0) {
    *u = (V3) {1, 0, (p.d - p.normal.x) / p.normal.z};
    *v = (V3) {0, 1, (p.d - p.normal.y) / p.normal.z};
  } else { UNREACHABLE(); }
}

B32 Plane3IsPointAbove(Plane3 p, V3 pt) {
  V3 point_on_plane = Plane3CreatePoint(p);
  V3 vec = V3SubV3(pt, point_on_plane);
  return V3DotV3(p.normal, vec) > 0;
}

B32 Plane3ContainsPoint(Plane3 p, V3 pt) {
  return V3DotV3(p.normal, pt) == p.d;
}

// https://math.stackexchange.com/questions/723937/find-the-point-on-a-plane-3x-4y-z-1-that-is-closest-to-1-0-1
V3 Plane3CreatePoint(Plane3 p) {
  F32 t = p.d / V3DotV3(p.normal, p.normal);
  return V3MultF32(p.normal, t);
}

Plane3 Plane3Flip(Plane3 p) {
  Plane3 result;
  result.normal = V3MultF32(p.normal, -1);
  result.d = p.d * -1;
  return result;
}

B32 Plane3ApproxEq(Plane3 a, Plane3 b) {
  return V3ApproxEq(a.normal, b.normal) && F32ApproxEq(a.d, b.d);
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Intersection implementation
///////////////////////////////////////////////////////////////////////////////

B32 IntersectionRay3Plane3(Ray3 ray, Plane3 plane, V3* intersect_point) {
  V3 point_on_plane = Plane3CreatePoint(plane);
  F32 denom = V3DotV3(plane.normal, ray.dir);
  if (denom == 0) { return false; }
  F32 t = (V3DotV3(V3SubV3(point_on_plane, ray.start), plane.normal)) / denom;
  if (t < 0) { return false; }
  if (intersect_point != NULL) {
    *intersect_point = V3AddV3(ray.start, V3MultF32(ray.dir, t));
  }
  return true;
}

// https://tavianator.com/2011/ray_box.html
B32 IntersectionRay3Rect3(Ray3 ray, Rect3 rect, V3* enter_point, V3* exit_point) {
  V3 rect_min, rect_max;
  Rect3GetMinMax(rect, &rect_min, &rect_max);
  V3 ray_dir_inv = Ray3DirInv(ray);

  V3 t_1 = V3HadamardV3(V3SubV3(rect_min, ray.start), ray_dir_inv);
  V3 t_2 = V3HadamardV3(V3SubV3(rect_max, ray.start), ray_dir_inv);
  F32 t_min = MAX3(MIN(t_1.x, t_2.x), MIN(t_1.y, t_2.y), MIN(t_1.z, t_2.z));
  F32 t_max = MIN3(MAX(t_1.x, t_2.x), MAX(t_1.y, t_2.y), MAX(t_1.z, t_2.z));
  if (t_max < t_min) { return false; }

  if (enter_point != NULL) {
    *enter_point = V3AddV3(ray.start, V3MultF32(ray.dir, t_min));
  }
  if (exit_point != NULL) {
    *exit_point = V3AddV3(ray.start, V3MultF32(ray.dir, t_max));
  }
  return true;
}

B32 IntersectionRay3Tri3(Ray3 ray, Tri3 tri, V3* intersect_point) {
  Plane3 tri_plane;
  if (!Tri3CreatePlane3(tri, &tri_plane)) {
    return false;
  }
  V3 plane_intersection;
  if (!IntersectionRay3Plane3(ray, tri_plane, &plane_intersection)) {
    return false;
  }
  if (!Tri3ContainsPoint(tri, plane_intersection)) {
    return false;
  }
  if (intersect_point != NULL) {
    *intersect_point = plane_intersection;
  }
  return true;
}

B32 IntersectionSphere3Sphere3(Sphere3 a, Sphere3 b, IntersectionManifold3* manifold) {
  V3 center_diff = V3SubV3(b.center_point, a.center_point);
  F32 min_distance = a.radius + b.radius;
  F32 distance = V3Length(center_diff);
  if (min_distance < distance) { return false; }

  if (manifold != NULL) {
    if (V3LengthSq(center_diff) == 0) { center_diff.z = 1.0f; }
    manifold->normal = V3Normalize(center_diff),
    manifold->penetration = min_distance - distance;
  }
  return true;
}

B32 IntersectionSphere3Rect3(Sphere3 a, Rect3 b, IntersectionManifold3* manifold) {
  V3 rel_center = V3SubV3(a.center_point, b.center_point);
  V3 b_half_size = V3MultF32(b.size, 0.5f);
  if ((F32Abs(rel_center.x) - a.radius) > b_half_size.x ||
      (F32Abs(rel_center.y) - a.radius) > b_half_size.y ||
      (F32Abs(rel_center.z) - a.radius) > b_half_size.z) {
    return false;
  }

  V3 rel_closest_point;
  rel_closest_point.x = CLAMP(-b_half_size.x, rel_center.x, +b_half_size.x);
  rel_closest_point.y = CLAMP(-b_half_size.y, rel_center.y, +b_half_size.y);
  rel_closest_point.z = CLAMP(-b_half_size.z, rel_center.z, +b_half_size.z);

  F32 dist = V3Length(V3SubV3(rel_closest_point, rel_center));
  if (dist > a.radius) { return false; }

  if (manifold != NULL) {
    V3 normal = V3SubV3(rel_closest_point, rel_center);
    if (V3LengthSq(normal) == 0) {
      normal = rel_center;
      if (V3LengthSq(normal) == 0) { normal.z = 1; }
    }
    manifold->normal = V3Normalize(normal);
    manifold->penetration = a.radius - dist;
  }
  return true;
}

B32 IntersectionRect3Rect3(Rect3 a, Rect3 b, IntersectionManifold3* manifold) {
  V3 a_min, a_max;
  Rect3GetMinMax(a, &a_min, &a_max);
  V3 b_min, b_max;
  Rect3GetMinMax(b, &b_min, &b_max);

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

#endif // CDEFAULT_3DGEOMETRY_IMPLEMENTATION
