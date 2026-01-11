#ifndef CDEFAULT_PHYSICS3_H_
#define CDEFAULT_PHYSICS3_H_

#include "cdefault_geometry.h"
#include "cdefault_math.h"
#include "cdefault_std.h"

// TODO: sleep for non-moving dynamic rigid bodies

// NOTE: The physics system revolves around colliders and collisions as the core data type.
// Colliders can have arbitrary types / datums attached to them, rigid bodies are a first-class
// supported example of this. Each datum must have the Collider3SubtypeHeader as its first field.
// Pointers provided to Collider3SetSubtype(...) must be stable and outlive the Collider3.
//
// #define COLLIDER3_PLAYER_COLLIDER 123
// struct PlayerCollider {
//   Collider3SubtypeHeader header;
//   U32 is_ethereal;
// }
#define COLLIDER3_RIGID_BODY ((U32) ~0)
typedef struct Collider3SubtypeHeader Collider3SubtypeHeader;
struct Collider3SubtypeHeader {
  U32 type; // NOTE: The types listed above are reserved.
  Collider3SubtypeHeader* next;
};

// TODO: support more collider types.
// NOTE: All collider types *must* store the center as a V3 as the first member.
typedef struct Collider3Sphere Collider3Sphere;
struct Collider3Sphere {
  V3  center;
  F32 radius;
};

typedef struct Collider3Rect Collider3Rect;
struct Collider3Rect {
  V3 center;
  V3 size;
};

typedef struct Collider3ConvexHull Collider3ConvexHull;
struct Collider3ConvexHull {
  V3 center;
  U32 points_size;
  V3* points_local; // NOTE: points in local space (centered around center).
  V3* points_world; // NOTE: points in world space (rotated and translated).
  Arena* arena;
};

typedef enum Collider3Type Collider3Type;
enum Collider3Type {
  Collider3Type_Sphere,
  Collider3Type_ConvexHull,
};

// NOTE: Colliders can only collide with other colliders that share a group bit.
// COLLIDER3_GROUP_ALL is the default value for all registered colliders.
#define COLLIDER3_GROUP_ALL  ~0
#define COLLIDER3_GROUP_NONE  0
typedef struct Collider3 Collider3;
struct Collider3 {
  Collider3Type type;
  union {
    V3                  center;
    Collider3Sphere     sphere;
    Collider3ConvexHull convex_hull;
  };
  F32 broad_sphere_radius; // TODO: switch to aabb?
  U32 group;
  Collider3SubtypeHeader* subtypes;
};

typedef enum RigidBody3Type RigidBody3Type;
enum RigidBody3Type {
  RigidBody3Type_Dynamic, // NOTE: Movable / influenced by external forces.
  RigidBody3Type_Static,  // NOTE: Immovable / not influenced by external forces.
};

// TODO: allow rigid bodies to sleep.
typedef struct RigidBody3 RigidBody3;
struct RigidBody3 {
  Collider3SubtypeHeader header;
  RigidBody3Type type;
  F32 mass_inv;
  F32 restitution;
  F32 static_friction;
  F32 dynamic_friction;
  V3  force;
  V3  velocity;
  Collider3* collider;
};

// NOTE: Resolvers define the behavior when 2 colliders of specific defined types collide. It is modeled as
// a retained-mode callback invoked 1x per physics iteration (so it may be called multiple times per frame,
// per user-provided configuration). All detected collisions are passed to the resolver for inspection.
// The RigidBody3 x RigidBody3 resolver is registered automatically.
//
// void PlayerRigidBodyResolver(Collision3* collisions, U32 collisions_size) {
//   for (U32 i = 0; i < collisions_size; i++) {
//     // NOTE: collision.a and collision.b subtypes will match what is registered via Physics3RegisterResolver.
//     PlayerCollider* player     = Collider3GetSubtype(collisions[i].a, COLLIDER3_PLAYER_COLLIDER);
//     PlayerCollider* rigid_body = Collider3GetSubtype(collisions[i].b, COLLIDER3_RIGID_BODY);
//     if (!player_collider->is_ethereal) { GotoGameOverState(); }
//   }
// }
// Physics3RegisterResolver(COLLIDER3_PLAYER_COLLIDER, COLLIDER3_RIGID_BODY, PlayerRigidBodyResolver);
typedef struct Collision3 Collision3;
struct Collision3 {
  Collider3* a;
  Collider3* b;
  IntersectManifold3 manifold;
};
typedef void Collision3Resolver_Fn(Collision3* collisions, U32 collisions_size);

// NOTE: Init must be called before any other function.
void Physics3Init(U32 iterations);
void Physics3Deinit();
void Physics3ConfigRigidBodies(U32 iterations, F32 penetration_slop, F32 pos_correction_pct);
void Physics3SetGravity(V3 gravity);
void Physics3RegisterResolver(U32 type_a, U32 type_b, Collision3Resolver_Fn* resolver); // NOTE: Resolvers can only be registered 1x per type pair.

// NOTE: This must be called, e.g. 1x per frame. A fixed timestep can be implemented in the user's program.
void Physics3Update(F32 dt_s);

Collider3* Physics3RegisterColliderSphere(V3 center, F32 radius);
Collider3* Physics3RegisterColliderRect(V3 center, V3 size);
Collider3* Physics3RegisterColliderConvexHull(V3* points, U32 points_size); // NOTE: points is copied. TODO: separate fn for w/ center?
void Physics3DeregisterCollider(Collider3* collider); // NOTE: Must be called after all subtypes are deinitialized.
void Collider3SetSubtype(Collider3* collider, Collider3SubtypeHeader* subtype);
B32  Collider3RemoveSubtype(Collider3* collider, U32 type);
Collider3SubtypeHeader* Collider3GetSubtype(Collider3* collider, U32 type);

// TODO: material-specific rigid body setup functions (for e.g. restitution, friction coeffs)
// TODO: rigid body functions
// TODO: AddForce fn that takes a force and a point, breaks into force / torque components.
// NOTE: Rigid body registration automatically attaches the rigid body to the collider.
RigidBody3* Physics3RegisterRigidBodyStatic(Collider3* collider);
RigidBody3* Physics3RegisterRigidBodyDynamic(Collider3* collider, F32 mass);
RigidBody3* Physics3RegisterRigidBody(Collider3* collider); // NOTE: expects user to complete initialization.
void Physics3DeregisterRigidBody(RigidBody3* rigid_body);

#endif // CDEFAULT_PHYSICS3_H_

#ifdef CDEFAULT_PHYSICS3_IMPLEMENTATION
#undef CDEFAULT_PHYSICS3_IMPLEMENTATION

typedef struct Collider3Internal Collider3Internal;
struct Collider3Internal {
  Collider3 collider;
  Collider3Internal* prev; // TODO: remove
  Collider3Internal* next;
};

typedef struct RigidBody3Internal RigidBody3Internal;
struct RigidBody3Internal {
  RigidBody3 rigid_body;
  RigidBody3Internal* prev; // TODO: remove
  RigidBody3Internal* next;
};

typedef struct Physics3ResolverEntry Physics3ResolverEntry;
struct Physics3ResolverEntry {
  U32 type_a;
  U32 type_b;
  Arena* collisions_arena;
  Collision3* collisions;
  U32 collisions_size;
  U32 collisions_capacity;
  Collision3Resolver_Fn* fn;
  Physics3ResolverEntry* next;
};

typedef struct Physics3Context Physics3Context;
struct Physics3Context {
  U32 iterations;
  F32 rigid_body_penetration_slop;
  F32 rigid_body_iter_pos_correction_pct;
  U32 rigid_body_iterations;
  Arena*                 collider_pool;
  Collider3Internal*     collider_head;
  Collider3Internal*     collider_tail;
  Collider3Internal*     collider_free_list;
  Arena*                 rigid_body_pool;
  RigidBody3Internal*    rigid_body_free_list;
  RigidBody3Internal*    rigid_body_head;
  RigidBody3Internal*    rigid_body_tail;
  V3                     rigid_body_gravity;
  Arena*                 resolver_pool;
  Physics3ResolverEntry* resolvers; // TODO: LRU for hot resolvers? or allow priority to be specified?
};
static Physics3Context _cdef_phys_3d_context;

static Collider3* Collider3Allocate() {
  Physics3Context* c = &_cdef_phys_3d_context;
  Collider3Internal* collider;
  if (c->collider_free_list != NULL) {
    collider = c->collider_free_list;
    SLL_STACK_POP(c->collider_free_list, next);
  } else {
    collider = ARENA_PUSH_STRUCT(c->collider_pool, Collider3Internal);
  }
  MEMORY_ZERO_STRUCT(collider);
  DLL_PUSH_BACK(c->collider_head, c->collider_tail, collider, prev, next);
  return &collider->collider;
}

static void Collider3ConvexHullUpdateWorldPoints(Collider3* collider) {
  DEBUG_ASSERT(collider->type == Collider3Type_ConvexHull);
  MEMORY_COPY_ARRAY(collider->convex_hull.points_world, collider->convex_hull.points_local, collider->convex_hull.points_size);
  ConvexHull3Offset(collider->convex_hull.points_world, collider->convex_hull.points_size, collider->convex_hull.center);
}

static B32 Collider3IntersectBroad(Collider3* a, Collider3* b, IntersectManifold3* manifold) {
  return Sphere3IntersectSphere3(a->center, a->broad_sphere_radius, b->center, b->broad_sphere_radius, manifold);
}

static B32 Collider3IntersectNarrow(Collider3* a, Collider3* b, IntersectManifold3* manifold) {
  switch (a->type) {
    case Collider3Type_Sphere: {
      switch (b->type) {
        case Collider3Type_Sphere: {
          return Sphere3IntersectSphere3(a->sphere.center, a->sphere.radius, b->sphere.center, b->sphere.radius, manifold);
        } break;
        case Collider3Type_ConvexHull: {
          Collider3ConvexHullUpdateWorldPoints(b);
          B32 result = Sphere3IntersectConvexHull3(a->sphere.center, a->sphere.radius, b->convex_hull.points_world, b->convex_hull.points_size, manifold);
          return result;
        }
      }
    } break;
    case Collider3Type_ConvexHull: {
      Collider3ConvexHullUpdateWorldPoints(a);
      switch (b->type) {
        case Collider3Type_Sphere: {
          return ConvexHull3IntersectSphere3(a->convex_hull.points_world, a->convex_hull.points_size, b->sphere.center, b->sphere.radius, manifold);
        } break;
        case Collider3Type_ConvexHull: {
          Collider3ConvexHullUpdateWorldPoints(b);
          return ConvexHull3IntersectConvexHull3(a->convex_hull.points_world, a->convex_hull.points_size, b->convex_hull.points_world, b->convex_hull.points_size, manifold);
        } break;
      }
    } break;
  }
  UNREACHABLE();
  return false;
}

static RigidBody3* RigidBody3Allocate() {
  Physics3Context* c = &_cdef_phys_3d_context;
  RigidBody3Internal* rigid_body_internal;
  if (c->rigid_body_free_list != NULL) {
    rigid_body_internal = c->rigid_body_free_list;
    SLL_STACK_POP(c->rigid_body_free_list, next);
  } else {
    rigid_body_internal = ARENA_PUSH_STRUCT(c->rigid_body_pool, RigidBody3Internal);
  }
  MEMORY_ZERO_STRUCT(rigid_body_internal);
  DLL_PUSH_BACK(c->rigid_body_head, c->rigid_body_tail, rigid_body_internal, prev, next);
  return &rigid_body_internal->rigid_body;
}

static void RigidBody3CommonInit(RigidBody3* rigid_body) {
  rigid_body->header.type = COLLIDER3_RIGID_BODY;
  rigid_body->restitution = 0.2f;
  rigid_body->static_friction = 0.2f;
  rigid_body->dynamic_friction = 0.2f;
}

static void Physics3RigidBodyUpdate(F32 dt_s) {
  Physics3Context* c = &_cdef_phys_3d_context;
  for (RigidBody3Internal* rigid_body_internal = c->rigid_body_head; rigid_body_internal != NULL; rigid_body_internal = rigid_body_internal->next) {
    RigidBody3* rigid_body = &rigid_body_internal->rigid_body;
    if (rigid_body->type != RigidBody3Type_Dynamic) { continue; }

    V3 moment_acceleration = V3MultF32(rigid_body->force, rigid_body->mass_inv);
    moment_acceleration = V3AddV3(moment_acceleration, c->rigid_body_gravity);
    moment_acceleration = V3MultF32(moment_acceleration, dt_s);
    rigid_body->velocity = V3AddV3(rigid_body->velocity, moment_acceleration);
    V3 moment_velocity = V3MultF32(rigid_body->velocity, dt_s);
    rigid_body->collider->center = V3AddV3(rigid_body->collider->center, moment_velocity);

    MEMORY_ZERO_STRUCT(&rigid_body->force);
  }
}

static void Physics3RigidBodyResolver(Collision3* collisions, U32 collisions_size) {
  Physics3Context* c = &_cdef_phys_3d_context;
  for (U32 i = 0; i < collisions_size; i++) {
    Collision3* test = &collisions[i];
    RigidBody3* a_rigid_body = (RigidBody3*) Collider3GetSubtype(test->a, COLLIDER3_RIGID_BODY);
    RigidBody3* b_rigid_body = (RigidBody3*) Collider3GetSubtype(test->b, COLLIDER3_RIGID_BODY);
    // NOTE: remove static / static collisions
    if (a_rigid_body->type == RigidBody3Type_Static && b_rigid_body->type == RigidBody3Type_Static) {
      DA_SWAP_REMOVE_EX(collisions, collisions_size, i);
      i--;
      continue;
    }
    // NOTE: remove collisions that are too small
    if (test->manifold.penetration < c->rigid_body_penetration_slop) {
      DA_SWAP_REMOVE_EX(collisions, collisions_size, i);
      i--;
      continue;
    }
  }

  for (U32 iter = 0; iter < c->rigid_body_iterations; iter++) {
    for (U32 i = 0; i < collisions_size; i++) {
      // NOTE: find collision with max penetration
      U32 max_idx = 0;
      for (U32 j = 1; j < collisions_size; j++) {
        if (collisions[j].manifold.penetration > collisions[max_idx].manifold.penetration) {
          max_idx = j;
        }
      }
      Collision3* max = &collisions[max_idx];
      if (max->manifold.penetration <= c->rigid_body_penetration_slop) { break; }
      IntersectManifold3* manifold = &max->manifold;
      Collider3* a = max->a;
      Collider3* b = max->b;
      RigidBody3* a_rigid_body = (RigidBody3*) Collider3GetSubtype(a, COLLIDER3_RIGID_BODY);
      RigidBody3* b_rigid_body = (RigidBody3*) Collider3GetSubtype(b, COLLIDER3_RIGID_BODY);

      // NOTE: separate bodies
      F32 pen_ratio = c->rigid_body_iter_pos_correction_pct * MAX(manifold->penetration - c->rigid_body_penetration_slop, 0);
      pen_ratio /= a_rigid_body->mass_inv + b_rigid_body->mass_inv;
      V3 a_separation = V3_ZEROES;
      if (a_rigid_body->type == RigidBody3Type_Dynamic) {
        F32 a_pen = a_rigid_body->mass_inv * pen_ratio;
        a_separation = V3MultF32(manifold->normal, a_pen);
        a->center = V3AddV3(a->center, a_separation);
      }
      V3 b_separation = V3_ZEROES;
      if (b_rigid_body->type == RigidBody3Type_Dynamic) {
        F32 b_pen = b_rigid_body->mass_inv * pen_ratio;
        b_separation = V3MultF32(manifold->normal, b_pen);
        b->center = V3SubV3(b->center, b_separation);
      }

      // NOTE: determine impulse
      V3 relative_velocity = V3SubV3(a_rigid_body->velocity, b_rigid_body->velocity);
      F32 separating_velocity = V3DotV3(relative_velocity, manifold->normal);
      if (separating_velocity > 0) {
        // NOTE: remove from collisions, otherwise future iters will repeat the separation step
        DA_SWAP_REMOVE_EX(collisions, collisions_size, max_idx);
        continue;
      }
      F32 e = a_rigid_body->restitution * b_rigid_body->restitution;
      F32 j_num   = -(1.0f + e) * separating_velocity;
      F32 j_denom = (a_rigid_body->mass_inv + b_rigid_body->mass_inv);
      DEBUG_ASSERT(j_denom != 0);
      F32 j = j_num / j_denom;
      V3 impulse = V3MultF32(manifold->normal, j);

      // NOTE: determine tangent / friction impulse
      V3 tangent = V3SubV3(relative_velocity, V3MultF32(manifold->normal, separating_velocity));
      if (V3LengthSq(tangent) > 0) { tangent = V3Normalize(tangent); }
      F32 jt_num   = -1.0f * V3DotV3(relative_velocity, tangent);
      F32 jt_denom = a_rigid_body->mass_inv + b_rigid_body->mass_inv;
      DEBUG_ASSERT(jt_denom != 0);
      F32 jt = jt_num / jt_denom;
      V2 static_friction  = V2Assign(a_rigid_body->static_friction, b_rigid_body->static_friction);
      V2 dynamic_friction = V2Assign(a_rigid_body->dynamic_friction, b_rigid_body->dynamic_friction);
      F32 mu_static       = V2Length(static_friction);
      F32 mu_dynamic      = V2Length(dynamic_friction);
      V3 friction_impulse;
      if (F32Abs(jt) <= j * mu_static) { friction_impulse = V3MultF32(tangent, jt);              }
      else                             { friction_impulse = V3MultF32(tangent, -j * mu_dynamic); }
      impulse = V3AddV3(impulse, friction_impulse);

      // NOTE: apply impulse
      if (a_rigid_body->type == RigidBody3Type_Dynamic) {
        V3 a_dv = V3MultF32(impulse, a_rigid_body->mass_inv);
        a_rigid_body->velocity = V3AddV3(a_rigid_body->velocity, a_dv);
      }
      if (b_rigid_body->type == RigidBody3Type_Dynamic) {
        V3 b_dv = V3MultF32(impulse, b_rigid_body->mass_inv);
        b_rigid_body->velocity = V3SubV3(b_rigid_body->velocity, b_dv);
      }

      // NOTE: Update collisions
      for (U32 k = 0; k < collisions_size; k++) {
        Collision3* test = &collisions[k];
        if (test->a == a) {
          test->manifold.penetration -= V3DotV3(test->manifold.normal, a_separation);
        } else if (test->a == b) {
          test->manifold.penetration += V3DotV3(test->manifold.normal, b_separation);
        }
        if (test->b == a) {
          test->manifold.penetration += V3DotV3(test->manifold.normal, a_separation);
        } else if (test->b == b) {
          test->manifold.penetration -= V3DotV3(test->manifold.normal, b_separation);
        }
      }
    }
  }
}

void Physics3Init(U32 iterations) {
  Physics3Context* c = &_cdef_phys_3d_context;
  MEMORY_ZERO_STRUCT(c);
  c->iterations = iterations;
  c->rigid_body_penetration_slop = 0.01f;
  c->rigid_body_iter_pos_correction_pct = 0.9f;
  c->rigid_body_iterations = 2;
  c->collider_pool = ArenaAllocate();
  c->rigid_body_pool = ArenaAllocate();
  c->resolver_pool = ArenaAllocate();
  Physics3RegisterResolver(COLLIDER3_RIGID_BODY, COLLIDER3_RIGID_BODY, Physics3RigidBodyResolver);
}

void Physics3Deinit() {
  Physics3Context* c = &_cdef_phys_3d_context;
  for (Physics3ResolverEntry* resolver = c->resolvers; resolver != NULL; resolver = resolver->next) {
    ArenaRelease(resolver->collisions_arena);
  }
  ArenaRelease(c->collider_pool);
  ArenaRelease(c->rigid_body_pool);
  ArenaRelease(c->resolver_pool);
}

void Physics3ConfigRigidBodies(U32 iterations, F32 penetration_slop, F32 pos_correction_pct) {
  Physics3Context* c = &_cdef_phys_3d_context;
  c->rigid_body_iterations = iterations;
  c->rigid_body_penetration_slop = penetration_slop;
  c->rigid_body_iter_pos_correction_pct = pos_correction_pct;
}

void Physics3Update(F32 dt_s) {
  Physics3Context* c = &_cdef_phys_3d_context;

  for (U32 iteration = 0; iteration < c->iterations; iteration++) {
    Physics3RigidBodyUpdate(dt_s / c->iterations);
    for (Collider3Internal* a_internal = c->collider_head; a_internal != NULL; a_internal = a_internal->next) {
      for (Collider3Internal* b_internal = a_internal->next; b_internal != NULL; b_internal = b_internal->next) {
        Collider3* a = &a_internal->collider;
        Collider3* b = &b_internal->collider;
        DEBUG_ASSERT(a != b);
        if ((a->group | b->group) == 0) { continue; }

        // TODO: separate? acceleration structure? maybe just presort along one axis?
        // TODO: multithread?
        IntersectManifold3 manifold;
        if (!Collider3IntersectBroad(a, b, &manifold)) { continue; }
        if (!Collider3IntersectNarrow(a, b, &manifold)) { continue; }

        Collision3 collision;
        collision.a = a;
        collision.b = b;
        collision.manifold = manifold;

        for (Collider3SubtypeHeader* a_subtype = a->subtypes; a_subtype != NULL; a_subtype = a_subtype->next) {
          for (Collider3SubtypeHeader* b_subtype = b->subtypes; b_subtype != NULL; b_subtype = a_subtype->next) {
            for (Physics3ResolverEntry* resolver = c->resolvers; resolver != NULL; resolver = resolver->next) {
              if (a_subtype->type == resolver->type_a && b_subtype->type == resolver->type_b) {
                DA_PUSH_BACK_EX(resolver->collisions_arena, resolver->collisions, resolver->collisions_size, resolver->collisions_capacity, collision);
                break;
              } else if (a_subtype->type == resolver->type_b && b_subtype->type == resolver->type_a) {
                SWAP(Collider3*, collision.a, collision.b);
                collision.manifold.normal = V3Negate(collision.manifold.normal);
                DA_PUSH_BACK_EX(resolver->collisions_arena, resolver->collisions, resolver->collisions_size, resolver->collisions_capacity, collision);
                break;
              }
            }
          }
        }
      }
    }

    for (Physics3ResolverEntry* resolver = c->resolvers; resolver != NULL; resolver = resolver->next) {
      resolver->fn(resolver->collisions, resolver->collisions_size);
      resolver->collisions_size = 0;
    }
  }
}

void Physics3SetGravity(V3 gravity) {
  Physics3Context* c = &_cdef_phys_3d_context;
  c->rigid_body_gravity = gravity;
}

void Physics3RegisterResolver(U32 type_a, U32 type_b, Collision3Resolver_Fn* resolver) {
  Physics3Context* c = &_cdef_phys_3d_context;
  // NOTE: Trip if the resolver has already been registered.
  for (Physics3ResolverEntry* entry = c->resolvers; entry != NULL; entry = entry->next) {
    if (entry->fn == resolver) {
      DEBUG_ASSERT(!(entry->type_a == type_a && entry->type_b == type_b && entry->fn == resolver));
      DEBUG_ASSERT(!(entry->type_a == type_b && entry->type_b == type_a && entry->fn == resolver));
    }
  }

  Physics3ResolverEntry* entry = ARENA_PUSH_STRUCT(c->resolver_pool, Physics3ResolverEntry);
  MEMORY_ZERO_STRUCT(entry);
  entry->type_a = type_a;
  entry->type_b = type_b;
  entry->collisions_arena = ArenaAllocate();
  entry->fn = resolver;
  SLL_STACK_PUSH(c->resolvers, entry, next);
}

Collider3* Physics3RegisterColliderSphere(V3 center, F32 radius) {
  DEBUG_ASSERT(radius > 0);
  Collider3* collider = Collider3Allocate();
  collider->group = COLLIDER3_GROUP_ALL;
  collider->type = Collider3Type_Sphere;
  collider->sphere.center = center;
  collider->sphere.radius = radius;
  collider->broad_sphere_radius = radius;
  return collider;
}

Collider3* Physics3RegisterColliderRect(V3 center, V3 size) {
  DEBUG_ASSERT(size.x > 0 && size.y > 0);
  V3 rect_points[8];
  ConvexHull3FromAabb3(rect_points, NULL, center, size);
  return Physics3RegisterColliderConvexHull(rect_points, 8);
}

Collider3* Physics3RegisterColliderConvexHull(V3* points, U32 points_size) {
  Collider3* collider = Collider3Allocate();
  collider->group = COLLIDER3_GROUP_ALL;
  collider->type = Collider3Type_ConvexHull;
  collider->convex_hull.arena = ArenaAllocate();
  collider->convex_hull.points_local = ARENA_PUSH_ARRAY(collider->convex_hull.arena, V3, points_size);
  collider->convex_hull.points_world = ARENA_PUSH_ARRAY(collider->convex_hull.arena, V3, points_size);
  MEMORY_COPY_ARRAY(collider->convex_hull.points_local, points, points_size);
  MEMORY_COPY_ARRAY(collider->convex_hull.points_world, points, points_size);
  collider->convex_hull.points_size = points_size;
  ConvexHull3GetEnclosingSphere3(points, points_size, &collider->convex_hull.center, &collider->broad_sphere_radius);
  V3 neg_center = V3Negate(collider->convex_hull.center);
  ConvexHull3Offset(collider->convex_hull.points_local, collider->convex_hull.points_size, neg_center);
  return collider;
}

void Physics3DeregisterCollider(Collider3* collider) {
  Physics3Context* c = &_cdef_phys_3d_context;
  if (collider->type == Collider3Type_ConvexHull) { ArenaRelease(collider->convex_hull.arena); }
  Collider3Internal* c_internal = (Collider3Internal*) collider;
  DLL_REMOVE(c->collider_head, c->collider_tail, c_internal, prev, next);
  SLL_STACK_PUSH(c->collider_free_list, c_internal, next);
}

void Collider3SetSubtype(Collider3* collider, Collider3SubtypeHeader* subtype) {
  Collider3RemoveSubtype(collider, subtype->type);
  SLL_STACK_PUSH(collider->subtypes, subtype, next);
}

Collider3SubtypeHeader* Collider3GetSubtype(Collider3* collider, U32 type) {
  for (Collider3SubtypeHeader* subtype = collider->subtypes; subtype != NULL; subtype = subtype->next) {
    if (subtype->type == type) { return subtype; }
  }
  return NULL;
}

B32 Collider3RemoveSubtype(Collider3* collider, U32 type) {
  Physics3Context* c = &_cdef_phys_3d_context;
  if (collider->subtypes == NULL) { return false; }
  if (collider->subtypes->type == type) {
    collider->subtypes = collider->subtypes->next;
    return true;
  }
  for (Collider3SubtypeHeader* subtype = collider->subtypes; subtype->next != NULL; subtype = subtype->next) {
    if (subtype->next->type != type) { continue; }
    if (type == COLLIDER3_RIGID_BODY) {
      DLL_REMOVE(c->rigid_body_head, c->rigid_body_tail, (RigidBody3Internal*) subtype->next, prev, next);
      SLL_STACK_PUSH(c->rigid_body_free_list, (RigidBody3Internal*) subtype->next, next);
    }
    subtype->next = subtype->next->next;
    return true;
  }
  return false;
}

RigidBody3* Physics3RegisterRigidBody(Collider3* collider) {
  RigidBody3* rigid_body = RigidBody3Allocate();
  Collider3SetSubtype(collider, &rigid_body->header);
  return rigid_body;
}

RigidBody3* Physics3RegisterRigidBodyStatic(Collider3* collider) {
  RigidBody3* rigid_body = Physics3RegisterRigidBody(collider);
  RigidBody3CommonInit(rigid_body);
  rigid_body->type = RigidBody3Type_Static;
  rigid_body->collider = collider;
  rigid_body->mass_inv = 0;
  return rigid_body;
}

RigidBody3* Physics3RegisterRigidBodyDynamic(Collider3* collider, F32 mass) {
  RigidBody3* rigid_body = Physics3RegisterRigidBody(collider);
  RigidBody3CommonInit(rigid_body);
  rigid_body->type = RigidBody3Type_Dynamic;
  rigid_body->collider = collider;
  DEBUG_ASSERT(mass > 0);
  rigid_body->mass_inv = 1.0f / mass;
  return rigid_body;
}

void Physics3DeregisterRigidBody(RigidBody3* rigid_body) {
  Physics3Context* c = &_cdef_phys_3d_context;
  RigidBody3Internal* rigid_body_internal = (RigidBody3Internal*) rigid_body;
  DLL_REMOVE(c->rigid_body_head, c->rigid_body_tail, rigid_body_internal, prev, next);
  SLL_STACK_PUSH(c->rigid_body_free_list, rigid_body_internal, next);
}

#endif // CDEFAULT_PHYSICS3_IMPLEMENTATION
