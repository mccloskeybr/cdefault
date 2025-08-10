#ifndef CDEFAULT_3DGEOMETRY_H_
#define CDEFAULT_3DGEOMETRY_H_

#include "cdefault_math.h"
#include "cdefault_std.h"

///////////////////////////////////////////////////////////////////////////////
// NOTE: Aabb3
///////////////////////////////////////////////////////////////////////////////

typedef struct Aabb3 Aabb3;
struct Aabb3 {
  V3 center_point;
  V3 size;
};

void Aabb3FromMinMax(const V3* min, const V3* max, Aabb3* out_aabb3);
void Aabb3GetMinMax(const Aabb3* aabb, V3* out_min, V3* out_max);
void Aabb3RelativeNormal(const Aabb3* aabb, const V3* pt, V3* normal);
B32 Aabb3ContainsPoint(const Aabb3* aabb, const V3* pos);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Tri3
///////////////////////////////////////////////////////////////////////////////

typedef struct Tri3 Tri3;
struct Tri3 {
  V3 points[3];
};

void Tri3Offset(const Tri3* src, const V3* offset, Tri3* out_dest);
B32 Tri3CreatePlane3(const Tri3* t, struct Plane3* out_plane);
B32 Tri3ContainsPoint(const Tri3* t, const V3* point);

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

void Ray3DirInv(const Ray3* r, V3* out_dir_inv);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Plane3
///////////////////////////////////////////////////////////////////////////////

typedef struct Plane3 Plane3;
struct Plane3 {
  V3 normal;
  F32 d;
};

void Plane3WithAnchor(const V3* anchor, const V3* normal, Plane3* out_plane);
void Plane3Basis(const Plane3* plane, V3* out_u, V3* out_v);
B32 Plane3IsPointAbove(const Plane3* plane, const V3* point);
B32 Plane3ContainsPoint(const Plane3* plane, const V3* point);
void Plane3CreatePoint(const Plane3* plane, V3* out_point);
void Plane3Flip(const Plane3* plane, Plane3* out_flipped_plane);
B32 Plane3ApproxEq(const Plane3* a, const Plane3* b);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Intersection
///////////////////////////////////////////////////////////////////////////////

B32 IntersectionRay3Plane3(const Ray3* ray, const Plane3* plane, V3* out_intersect_point);
B32 IntersectionRay3Aabb3(const Ray3* ray, const Aabb3* aabb, V3* out_enter_point, V3* out_exit_point);
B32 IntersectionRay3Tri3(const Ray3* ray, const Tri3* tri, V3* out_intersect_point);

typedef struct IntersectionManifold3 IntersectionManifold3;
struct IntersectionManifold3 {
  V3 normal;
  F32 penetration;
};

B32 IntersectionSphere3Sphere3(const Sphere3* a, const Sphere3* b, IntersectionManifold3* out_manifold);
B32 IntersectionSphere3Aabb3(const Sphere3* a, const Aabb3* b, IntersectionManifold3* out_manifold);
B32 IntersectionAabb3Aabb3(const Aabb3* a, const Aabb3* b, IntersectionManifold3* out_manifold);

#endif // CDEFAULT_3DGEOMETRY_H_

#ifdef CDEFAULT_3DGEOMETRY_IMPLEMENTATION
#undef CDEFAULT_3DGEOMETRY_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// NOTE: Aabb3 implementation
///////////////////////////////////////////////////////////////////////////////

void Aabb3FromMinMax(const V3* min, const V3* max, Aabb3* out_aabb3) {
  DEBUG_ASSERT(min->x < max->x);
  DEBUG_ASSERT(min->y < max->y);
  DEBUG_ASSERT(min->z < max->z);
  out_aabb3->center_point.x = (min->x + max->x) / 2.0f;
  out_aabb3->center_point.y = (min->y + max->y) / 2.0f;
  out_aabb3->center_point.z = (min->z + max->z) / 2.0f;
  out_aabb3->size.x = max->x - min->x;
  out_aabb3->size.y = max->y - min->y;
  out_aabb3->size.z = max->z - min->z;
}

void Aabb3GetMinMax(const Aabb3* aabb, V3* out_min, V3* out_max) {
  out_min->x = aabb->center_point.x - (aabb->size.x / 2.0f);
  out_min->y = aabb->center_point.y - (aabb->size.y / 2.0f);
  out_min->z = aabb->center_point.z - (aabb->size.z / 2.0f);
  out_max->x = aabb->center_point.x + (aabb->size.x / 2.0f);
  out_max->y = aabb->center_point.y + (aabb->size.y / 2.0f);
  out_max->z = aabb->center_point.z + (aabb->size.z / 2.0f);
}

// https://stackoverflow.com/questions/75811830/determine-the-face-of-a-cube-intersected-by-a-ray-in-a-minecraft-like-game
void Aabb3RelativeNormal(const Aabb3* aabb, const V3* pt, V3* out_normal) {
  V3 relative_p;
  V3SubV3(pt, &aabb->center_point, &relative_p);
  F32 p_max = MAX3(relative_p.x, relative_p.y, relative_p.z);
  F32 p_min = MIN3(relative_p.x, relative_p.y, relative_p.z);
  F32 abs_p_min = F32Abs(p_min);
  if (p_max == relative_p.x && p_max > abs_p_min) { *out_normal = V3_X_POS; }
  if (p_min == relative_p.x && p_max < abs_p_min) { *out_normal = V3_X_NEG; }
  if (p_max == relative_p.y && p_max > abs_p_min) { *out_normal = V3_Y_POS; }
  if (p_min == relative_p.y && p_max < abs_p_min) { *out_normal = V3_Y_NEG; }
  if (p_max == relative_p.z && p_max > abs_p_min) { *out_normal = V3_Z_POS; }
  if (p_min == relative_p.z && p_max < abs_p_min) { *out_normal = V3_Z_NEG; }
  UNREACHABLE();
}

B32 Aabb3ContainsPoint(const Aabb3* aabb, const V3* pos) {
  V3 min;
  V3 max;
  Aabb3GetMinMax(aabb, &min, &max);
  return ((min.x <= pos->x && pos->x <= max.x) &&
          (min.y <= pos->y && pos->y <= max.y) &&
          (min.z <= pos->z && pos->z <= max.z));
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Tri3 implementation
///////////////////////////////////////////////////////////////////////////////

void Tri3Offset(const Tri3* src, const V3* offset, Tri3* out_dest) {
  for (U32 i = 0; i < 3; i++) {
    V3AddV3(&src->points[i], offset, &out_dest->points[i]);
  }
}

B32 Tri3CreatePlane3(const Tri3* t, Plane3* out_plane) {
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

  Plane3WithAnchor(&t->points[0], &tri_norm, out_plane);
  return true;
}

// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html
B32 Tri3ContainsPoint(const Tri3* t, const V3* point) {
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

///////////////////////////////////////////////////////////////////////////////
// NOTE: Ray3 implementation
///////////////////////////////////////////////////////////////////////////////

void Ray3DirInv(const Ray3* r, V3* out_dir_inv) {
  out_dir_inv->x = 1.0f / r->dir.x;
  out_dir_inv->y = 1.0f / r->dir.y;
  out_dir_inv->z = 1.0f / r->dir.z;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Plane3 implementation
///////////////////////////////////////////////////////////////////////////////

// https://mathinsight.org/distance_point_plane
void Plane3WithAnchor(const V3* anchor, const V3* normal, Plane3* out_plane) {
  out_plane->normal = *normal;
  out_plane->d = V3DotV3(anchor, normal);
}

// https://math.stackexchange.com/questions/1702572/how-to-find-the-basis-of-a-plane-or-a-line
// NOTE: *not* orthonormal.
void Plane3Basis(const Plane3* plane, V3* out_u, V3* out_v) {
  if (plane->normal.x != 0) {
    *out_u = (V3) {(plane->d - plane->normal.z) / plane->normal.x, 0, 1};
    *out_v = (V3) {(plane->d - plane->normal.y) / plane->normal.x, 1, 0};
  } else if (plane->normal.y != 0) {
    *out_u = (V3) {1, (plane->d - plane->normal.x) / plane->normal.y, 0};
    *out_v = (V3) {0, (plane->d - plane->normal.z) / plane->normal.y, 1};
  } else if (plane->normal.z != 0) {
    *out_u = (V3) {1, 0, (plane->d - plane->normal.x) / plane->normal.z};
    *out_v = (V3) {0, 1, (plane->d - plane->normal.y) / plane->normal.z};
  } else { UNREACHABLE(); }
}

B32 Plane3IsPointAbove(const Plane3* plane, const V3* point) {
  V3 point_on_plane, vec;
  Plane3CreatePoint(plane, &point_on_plane);
  V3SubV3(point, &point_on_plane, &vec);
  return V3DotV3(&plane->normal, &vec) > 0;
}

B32 Plane3ContainsPoint(const Plane3* plane, const V3* point) {
  return V3DotV3(&plane->normal, point) == plane->d;
}

// https://math.stackexchange.com/questions/723937/find-the-point-on-a-plane-3x-4y-z-1-that-is-closest-to-1-0-1
void Plane3CreatePoint(const Plane3* plane, V3* out_point) {
  F32 t = plane->d / V3DotV3(&plane->normal, &plane->normal);
  V3MultF32(&plane->normal, t, out_point);
}

void Plane3Flip(const Plane3* plane, Plane3* out_flipped_plane) {
  V3MultF32(&plane->normal, -1, &out_flipped_plane->normal);
  out_flipped_plane->d = plane->d * -1;
}

B32 Plane3ApproxEq(const Plane3* a, const Plane3* b) {
  return V3ApproxEq(&a->normal, &b->normal) && F32ApproxEq(a->d, b->d);
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: Intersection implementation
///////////////////////////////////////////////////////////////////////////////

B32 IntersectionRay3Plane3(const Ray3* ray, const Plane3* plane, V3* out_intersect_point) {
  V3 point_on_plane, ray_to_plane;
  Plane3CreatePoint(plane, &point_on_plane);
  F32 denom = V3DotV3(&plane->normal, &ray->dir);
  if (denom == 0) { return false; }
  V3SubV3(&point_on_plane, &ray->start, &ray_to_plane);
  F32 t = (V3DotV3(&ray_to_plane, &plane->normal)) / denom;
  if (t < 0) { return false; }
  if (out_intersect_point != NULL) {
    V3MultF32(&ray->dir, t, out_intersect_point);
    V3AddV3(&ray->start, out_intersect_point, out_intersect_point);
  }
  return true;
}

// https://tavianator.com/2011/ray_box.html
B32 IntersectionRay3Aabb3(const Ray3* ray, const Aabb3* aabb, V3* out_enter_point, V3* out_exit_point) {
  V3 aabb_min, aabb_max;
  Aabb3GetMinMax(aabb, &aabb_min, &aabb_max);
  V3 ray_dir_inv;
  Ray3DirInv(ray, &ray_dir_inv);

  V3 t_1;
  V3SubV3(&aabb_min, &ray->start, &t_1);
  V3HadamardV3(&t_1, &ray_dir_inv, &t_1);
  V3 t_2;
  V3SubV3(&aabb_max, &ray->start, &t_2);
  V3HadamardV3(&t_2, &ray_dir_inv, &t_2);

  F32 t_min = MAX3(MIN(t_1.x, t_2.x), MIN(t_1.y, t_2.y), MIN(t_1.z, t_2.z));
  F32 t_max = MIN3(MAX(t_1.x, t_2.x), MAX(t_1.y, t_2.y), MAX(t_1.z, t_2.z));
  if (t_max < t_min) { return false; }

  if (out_enter_point != NULL) {
    V3MultF32(&ray->dir, t_min, out_enter_point);
    V3AddV3(&ray->start, out_enter_point, out_enter_point);
  }
  if (out_exit_point != NULL) {
    V3MultF32(&ray->dir, t_max, out_exit_point);
    V3AddV3(&ray->start, out_exit_point, out_exit_point);
  }
  return true;
}

B32 IntersectionRay3Tri3(const Ray3* ray, const Tri3* tri, V3* out_intersect_point) {
  Plane3 tri_plane;
  if (!Tri3CreatePlane3(tri, &tri_plane)) {
    return false;
  }
  V3 plane_intersection;
  if (!IntersectionRay3Plane3(ray, &tri_plane, &plane_intersection)) {
    return false;
  }
  if (!Tri3ContainsPoint(tri, &plane_intersection)) {
    return false;
  }
  if (out_intersect_point != NULL) {
    *out_intersect_point = plane_intersection;
  }
  return true;
}

B32 IntersectionSphere3Sphere3(const Sphere3* a, const Sphere3* b, IntersectionManifold3* out_manifold) {
  V3 center_diff;
  V3SubV3(&b->center_point, &a->center_point, &center_diff);
  F32 min_distance = a->radius + b->radius;
  F32 distance = V3Length(&center_diff);
  if (min_distance < distance) { return false; }

  if (out_manifold != NULL) {
    if (V3LengthSq(&center_diff) == 0) { center_diff.z = 1.0f; }
    V3Normalize(&center_diff, &out_manifold->normal);
    out_manifold->penetration = min_distance - distance;
  }
  return true;
}

B32 IntersectionSphere3Aabb3(const Sphere3* a, const Aabb3* b, IntersectionManifold3* out_manifold) {
  V3 rel_center, b_half_size;
  V3SubV3(&a->center_point, &b->center_point, &rel_center);
  V3MultF32(&b->size, 0.5f, &b_half_size);
  if ((F32Abs(rel_center.x) - a->radius) > b_half_size.x ||
      (F32Abs(rel_center.y) - a->radius) > b_half_size.y ||
      (F32Abs(rel_center.z) - a->radius) > b_half_size.z) {
    return false;
  }

  V3 rel_closest_point;
  rel_closest_point.x = CLAMP(-b_half_size.x, rel_center.x, +b_half_size.x);
  rel_closest_point.y = CLAMP(-b_half_size.y, rel_center.y, +b_half_size.y);
  rel_closest_point.z = CLAMP(-b_half_size.z, rel_center.z, +b_half_size.z);

  V3 to_closest_point;
  V3SubV3(&rel_closest_point, &rel_center, &to_closest_point);
  F32 dist = V3Length(&to_closest_point);
  if (dist > a->radius) { return false; }

  if (out_manifold != NULL) {
    V3 normal;
    V3SubV3(&rel_closest_point, &rel_center, &normal);
    if (V3LengthSq(&normal) == 0) {
      normal = rel_center;
      if (V3LengthSq(&normal) == 0) { normal.z = 1; }
    }
    V3Normalize(&normal, &out_manifold->normal);
    out_manifold->penetration = a->radius - dist;
  }
  return true;
}

B32 IntersectionAabb3Aabb3(const Aabb3* a, const Aabb3* b, IntersectionManifold3* out_manifold) {
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

  if (out_manifold != NULL) {
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

    out_manifold->normal = min_axis;
    out_manifold->penetration = penetration;
  }
  return true;
}

#endif // CDEFAULT_3DGEOMETRY_IMPLEMENTATION
