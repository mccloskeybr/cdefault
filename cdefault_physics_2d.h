#ifndef CDEFAULT_PHYSICS2_H_
#define CDEFAULT_PHYSICS2_H_

#include "cdefault_geometry.h"
#include "cdefault_math.h"
#include "cdefault_std.h"

// NOTE: The physics system revolves around colliders and collisions as the core data type.
// Colliders can have arbitrary types / datums attached to them, rigid bodies are a first-class
// supported example of this. Each datum must have the Collider2SubtypeHeader as its first field.
// Pointers provided to Collider2SetSubtype(...) must be stable and outlive the Collider2.
//
// #define COLLIDER2_PLAYER_COLLIDER 123
// struct PlayerCollider {
//   Collider2SubtypeHeader header;
//   U32 is_ethereal;
// }
//
// void PlayerRigidBodyResolver(DynamicArray* /*Collision2*/ collisions) {
//   for (U32 i = 0; i < collisions->size; i++) {
//     // NOTE: collision->a and collision->b subtypes will match what is registered via Physics2RegisterResolver.
//     Collision2* collision = (Collision2*) DynamicArrayGet(collisions, i);
//     PlayerCollider* player     = Collider2GetSubtype(collision->a, COLLIDER2_PLAYER_COLLIDER);
//     PlayerCollider* rigid_body = Collider2GetSubtype(collision->b, COLLIDER2_RIGID_BODY);
//     if (!player_collider->is_ethereal) { GotoGameOverState(); }
//   }
// }
//
// Physics2RegisterResolver(COLLIDER2_PLAYER_COLLIDER, COLLIDER2_RIGID_BODY, PlayerRigidBodyResolver);
#define COLLIDER2_RIGID_BODY ((U32) ~0)
typedef struct Collider2SubtypeHeader Collider2SubtypeHeader;
struct Collider2SubtypeHeader {
  U32 type; // NOTE: The types listed above are reserved.
  Collider2SubtypeHeader* next;
};

typedef enum RigidBody2Type RigidBody2Type;
enum RigidBody2Type {
  RigidBody2Type_Dynamic, // NOTE: Movable / influenced by external forces.
  RigidBody2Type_Static,  // NOTE: Immovable / not influenced by external forces.
};

typedef struct RigidBody2Opts RigidBody2Opts;
struct RigidBody2Opts {
  RigidBody2Type type;
  F32 restitution;
  F32 mass;
  F32 static_friction;
  F32 dynamic_friction;
};

typedef struct RigidBody2 RigidBody2;

// NOTE: Colliders can only collide with other colliders that share a group bit.
// COLLIDER2_GROUP_ALL is the default value for all registered colliders.
#define COLLIDER2_GROUP_ALL  ~0
#define COLLIDER2_GROUP_NONE  0
typedef struct Collider2 Collider2;

// NOTE: Resolvers define the behavior when 2 colliders of specific defined types collide.
typedef struct Collision2 Collision2;
struct Collision2 {
  Collider2* a;
  Collider2* b;
  IntersectManifold2 manifold;
};
typedef void Collision2Resolver_Fn(DynamicArray* /* Collision2 */ collisions);

// NOTE: Init must be called before any other function.
void Physics2Init();
void Physics2Deinit();
void Physics2SetGravity(V2 gravity);
void Physics2RegisterResolver(U32 type_a, U32 type_b, Collision2Resolver_Fn* resolver); // NOTE: Resolvers can only be registered 1x per type pair.

// NOTE: This must be called, e.g. 1x per frame. A fixed timestep can be implemented in the user's program.
void Physics2Update(F32 dt_s);

Collider2* Physics2RegisterColliderCircle(V2 center, F32 radius);
Collider2* Physics2RegisterColliderAabb(V2 center, V2 size);
void Physics2DeregisterCollider(Collider2* collider);

// TODO: some way to get shape information back out from the collider
// TODO: i think prefer exposing the collider2 and rigidbody2 structs directly, but is there a clean way?
void Collider2SetSubtype(Collider2* collider, Collider2SubtypeHeader* subtype);
RigidBody2* Collider2SetRigidBody(Collider2* collider, RigidBody2Opts rigid_body_opts);
Collider2SubtypeHeader* Collider2GetSubtype(Collider2* collider, U32 type);
B32  Collider2RemoveSubtype(Collider2* colluder, U32 type);
V2*  Collider2Center(Collider2* collider);
U32* Collider2Group(Collider2* collider);

V2*  RigidBody2Force(RigidBody2* rigid_body);
V2*  RigidBody2Velocity(RigidBody2* rigid_body);
void RigidBody2UpdateOpts(RigidBody2* rigid_body, RigidBody2Opts opts);
RigidBody2Opts RigidBody2GetOpts(RigidBody2* rigid_body);

// TODO: expose as dynamic configs?
// NOTE: How much play to allow for in RigidBody2 x RigidBody2 collisions.
#ifndef PHYSICS2_PENETRATION_SLOP
#define PHYSICS2_PENETRATION_SLOP 0.05f
#endif

// NOTE: How much separation to apply per RigidBody2 x RigidBody2 resolution instance.
#ifndef PHYSICS2_POS_CORRECTION_PCT
#define PHYSICS2_POS_CORRECTION_PCT 0.9f
#endif

// NOTE: How many iterations to apply for collision detection and resolution.
// Lower values may lead to instability among RigidBody2 x RigidBody2 collision clusters.
#ifndef PHYSICS2_ITERATIONS
#define PHYSICS2_ITERATIONS 5
#endif

// NOTE: How many iterations to apply for RigidBody2 x RigidBody2 collisions.
// Lower values may lead to instability among collision clusters.
#ifndef PHYSICS2_RIGID_BODY_ITERATIONS
#define PHYSICS2_RIGID_BODY_ITERATIONS 4
#endif

#endif // CDEFAULT_PHYSICS2_H_

#ifdef CDEFAULT_PHYSICS2_IMPLEMENTATION
#undef CDEFAULT_PHYSICS2_IMPLEMENTATION

typedef enum Collider2Type Collider2Type;
enum Collider2Type {
  Collider2Type_Circle,
  Collider2Type_Aabb,
};

typedef struct Collider2 Collider2;
struct Collider2 {
  U32 group;
  V2 center;
  F32 broad_circle_radius;
  Collider2Type type;
  union {
    F32 circle_radius;
    V2 aabb_size;
  };
  Collider2SubtypeHeader* subtypes;
  Collider2* prev;
  Collider2* next;
};

typedef struct RigidBody2 RigidBody2;
struct RigidBody2 {
  Collider2SubtypeHeader header;
  RigidBody2Type type;
  F32 restitution;
  F32 mass_inv;
  F32 static_friction;
  F32 dynamic_friction;
  V2  force;
  V2  velocity;
  Collider2* collider;
  RigidBody2* prev;
  RigidBody2* next;
};

typedef struct ResolverEntry ResolverEntry;
struct ResolverEntry {
  U32 type_a;
  U32 type_b;
  DynamicArray collisions;
  Collision2Resolver_Fn* fn;
  ResolverEntry* next;
};

typedef struct Physics2Context Physics2Context;
struct Physics2Context {
  Arena*         collider_pool;
  Collider2*     collider_head;
  Collider2*     collider_tail;
  Collider2*     collider_free_list;
  Arena*         rigid_body_pool;
  RigidBody2*    rigid_body_free_list;
  RigidBody2*    rigid_body_head;
  RigidBody2*    rigid_body_tail;
  V2             rigid_body_gravity;
  Arena*         resolver_pool;
  ResolverEntry* resolvers;
};
static Physics2Context _cdef_phys_2d_context;

static Collider2* Collider2Allocate() {
  Physics2Context* c = &_cdef_phys_2d_context;
  Collider2* collider;
  if (c->collider_free_list != NULL) {
    collider = c->collider_free_list;
    SLL_STACK_POP(c->collider_free_list, next);
  } else {
    collider = ARENA_PUSH_STRUCT(c->collider_pool, Collider2);
  }
  MEMORY_ZERO_STRUCT(collider);
  DLL_PUSH_BACK(c->collider_head, c->collider_tail, collider, prev, next);
  collider->group = COLLIDER2_GROUP_ALL;
  return collider;
}

static B32 Collider2IntersectBroad(Collider2* a, Collider2* b, IntersectManifold2* manifold) {
  return Circle2IntersectCircle2(&a->center, a->broad_circle_radius, &b->center, b->broad_circle_radius, manifold);
}

static B32 Collider2IntersectNarrow(Collider2* a, Collider2* b, IntersectManifold2* manifold) {
  switch (a->type) {
    case Collider2Type_Circle: {
      switch (b->type) {
        case Collider2Type_Circle: return Circle2IntersectCircle2(&a->center, a->circle_radius, &b->center, b->circle_radius, manifold);
        case Collider2Type_Aabb:   return Circle2IntersectAabb2(&a->center, a->circle_radius, &b->center, &b->aabb_size, manifold);
      }
    } break;
    case Collider2Type_Aabb: {
      switch (b->type) {
        case Collider2Type_Circle: return Aabb2IntersectCircle2(&a->center, &a->aabb_size, &b->center, b->circle_radius, manifold);
        case Collider2Type_Aabb:   return Aabb2IntersectAabb2(&a->center, &a->aabb_size, &b->center, &b->aabb_size, manifold);
      }
    } break;
  }
  UNREACHABLE();
  return false;
}

static void Physics2RigidBodyUpdate(F32 dt_s) {
  Physics2Context* c = &_cdef_phys_2d_context;
  for (RigidBody2* rigid_body = c->rigid_body_tail; rigid_body != NULL; rigid_body = rigid_body->next) {
    if (rigid_body->type != RigidBody2Type_Dynamic) { continue; }

    V2 moment_acceleration, moment_velocity;
    V2MultF32(&moment_acceleration, &rigid_body->force, rigid_body->mass_inv);
    V2AddV2(&moment_acceleration, &moment_acceleration, &c->rigid_body_gravity);
    V2MultF32(&moment_acceleration, &moment_acceleration, dt_s);
    V2AddV2(&rigid_body->velocity, &rigid_body->velocity, &moment_acceleration);
    V2MultF32(&moment_velocity, &rigid_body->velocity, dt_s);
    V2AddV2(&rigid_body->collider->center, &rigid_body->collider->center, &moment_velocity);

    MEMORY_ZERO_STRUCT(&rigid_body->force);
  }
}

static void Physics2RigidBodyResolver(DynamicArray* collisions) {
  for (U32 i = 0; i < collisions->size; i++) {
    Collision2* collision = (Collision2*) DynamicArrayGet(collisions, i);
    RigidBody2* a_rigid_body = (RigidBody2*) Collider2GetSubtype(collision->a, COLLIDER2_RIGID_BODY);
    RigidBody2* b_rigid_body = (RigidBody2*) Collider2GetSubtype(collision->b, COLLIDER2_RIGID_BODY);
    if (a_rigid_body->type == RigidBody2Type_Static && b_rigid_body->type == RigidBody2Type_Static) {
      DynamicArraySwapRemove(collisions, i);
      i--;
    }
  }

  for (U32 i = 0; i < collisions->size * PHYSICS2_RIGID_BODY_ITERATIONS; i++) {
    // NOTE: find collision with max penetration
    Collision2* max = (Collision2*) DynamicArrayGet(collisions, 0);
    for (U32 j = 1; j < collisions->size; j++) {
      Collision2* test = (Collision2*) DynamicArrayGet(collisions, j);
      if (test->manifold.penetration > max->manifold.penetration) { max = test; }
    }
    if (max->manifold.penetration <= PHYSICS2_PENETRATION_SLOP) { break; }

    IntersectManifold2* manifold = &max->manifold;
    Collider2* a = max->a;
    Collider2* b = max->b;
    RigidBody2* a_rigid_body = (RigidBody2*) Collider2GetSubtype(a, COLLIDER2_RIGID_BODY);
    RigidBody2* b_rigid_body = (RigidBody2*) Collider2GetSubtype(b, COLLIDER2_RIGID_BODY);

    // NOTE: Separate bodies
    F32 pen_ratio = PHYSICS2_POS_CORRECTION_PCT * MAX(manifold->penetration - PHYSICS2_PENETRATION_SLOP, 0);
    pen_ratio /= a_rigid_body->mass_inv + b_rigid_body->mass_inv;
    F32 a_pen = a_rigid_body->mass_inv * pen_ratio;
    F32 b_pen = b_rigid_body->mass_inv * pen_ratio;

    V2 a_separation;
    V2MultF32(&a_separation, &manifold->normal, a_pen);
    V2AddV2(&a->center, &a->center, &a_separation);

    V2 b_separation;
    V2MultF32(&b_separation, &manifold->normal, b_pen);
    V2SubV2(&b->center, &b->center, &b_separation);

    // NOTE: Apply impulse
    V2 relative_velocity;
    V2SubV2(&relative_velocity, &a_rigid_body->velocity, &b_rigid_body->velocity);
    F32 separating_velocity = V2DotV2(&relative_velocity, &manifold->normal);
    if (separating_velocity <= 0) {
      V2 impulse;
      F32 e = a_rigid_body->restitution * b_rigid_body->restitution;
      F32 j = (-(1.0f + e) * separating_velocity) / (a_rigid_body->mass_inv + b_rigid_body->mass_inv);
      V2MultF32(&impulse, &manifold->normal, j);

      // NOTE: Apply friction
      V2 tangent;
      V2MultF32(&tangent, &manifold->normal, separating_velocity);
      V2SubV2(&tangent, &relative_velocity, &tangent);
      if (V2LengthSq(&tangent) > 0) { V2Normalize(&tangent, &tangent); }
      F32 jt = (-1.0f * V2DotV2(&relative_velocity, &tangent)) / (a_rigid_body->mass_inv + b_rigid_body->mass_inv);
      V2 static_friction  = { a_rigid_body->static_friction, b_rigid_body->static_friction };
      V2 dynamic_friction = { a_rigid_body->dynamic_friction, b_rigid_body->dynamic_friction };
      F32 mu_static  = V2Length(&static_friction);
      F32 mu_dynamic = V2Length(&dynamic_friction);
      V2 friction_impulse;
      if (F32Abs(jt) < j * mu_static) { V2MultF32(&friction_impulse, &tangent, jt);              }
      else                            { V2MultF32(&friction_impulse, &tangent, -j * mu_dynamic); }
      V2AddV2(&impulse, &impulse, &friction_impulse);

      if (a_rigid_body->type == RigidBody2Type_Dynamic) {
        V2 a_dv;
        V2MultF32(&a_dv, &impulse, a_rigid_body->mass_inv);
        V2AddV2(&a_rigid_body->velocity, &a_rigid_body->velocity, &a_dv);
      }
      if (b_rigid_body->type == RigidBody2Type_Dynamic) {
        V2 b_dv;
        V2MultF32(&b_dv, &impulse, b_rigid_body->mass_inv);
        V2SubV2(&b_rigid_body->velocity, &b_rigid_body->velocity, &b_dv);
      }
    }

    // NOTE: Update object position across known collisions
    for (U32 j = 0; j < collisions->size; j++) {
      Collision2* test = (Collision2*) DynamicArrayGet(collisions, j);
      if (test->a == a) {
        test->manifold.penetration -= V2DotV2(&test->manifold.normal, &a_separation);
      } else if (test->a == b) {
        test->manifold.penetration += V2DotV2(&test->manifold.normal, &b_separation);
      }
      if (test->b == a) {
        test->manifold.penetration += V2DotV2(&test->manifold.normal, &a_separation);
      } else if (test->b == b) {
        test->manifold.penetration -= V2DotV2(&test->manifold.normal, &b_separation);
      }
    }
  }
}

void Physics2Init() {
  Physics2Context* c = &_cdef_phys_2d_context;
  MEMORY_ZERO_STRUCT(c);
  c->collider_pool = ArenaAllocate();
  c->rigid_body_pool = ArenaAllocate();
  c->resolver_pool = ArenaAllocate();
  Physics2RegisterResolver(COLLIDER2_RIGID_BODY, COLLIDER2_RIGID_BODY, Physics2RigidBodyResolver);
}

void Physics2Deinit() {
  Physics2Context* c = &_cdef_phys_2d_context;
  for (ResolverEntry* resolver = c->resolvers; resolver != NULL; resolver = resolver->next) {
    DynamicArrayDeinit(&resolver->collisions);
  }
  ArenaRelease(c->collider_pool);
  ArenaRelease(c->rigid_body_pool);
  ArenaRelease(c->resolver_pool);
}

void Physics2Update(F32 dt_s) {
  Physics2Context* c = &_cdef_phys_2d_context;
  Physics2RigidBodyUpdate(dt_s);

  for (U32 iteration = 0; iteration < PHYSICS2_ITERATIONS; iteration++) {
    for (Collider2* a = c->collider_tail; a->next != NULL; a = a->next) {
      for (Collider2* b = a->next; b != NULL; b = b->next) {
        if ((a->group | b->group) == 0) { continue; }

        IntersectManifold2 manifold;
        if (!Collider2IntersectBroad(a, b, &manifold)) { continue; }
        if (!Collider2IntersectNarrow(a, b, &manifold)) { continue; }

        Collision2 collision;
        collision.a = a;
        collision.b = b;
        collision.manifold = manifold;

        for (Collider2SubtypeHeader* a_subtype = a->subtypes; a_subtype != NULL; a_subtype = a_subtype->next) {
          for (Collider2SubtypeHeader* b_subtype = b->subtypes; b_subtype != NULL; b_subtype = a_subtype->next) {
            for (ResolverEntry* resolver = c->resolvers; resolver != NULL; resolver = resolver->next) {
              if (a_subtype->type == resolver->type_a && b_subtype->type == resolver->type_b) {
                DynamicArrayPushBack(&resolver->collisions, (U8*) &collision);
              } else if (a_subtype->type == resolver->type_b && b_subtype->type == resolver->type_a) {
                SWAP(Collider2*, collision.a, collision.b);
                V2MultF32(&collision.manifold.normal, &collision.manifold.normal, -1);
                DynamicArrayPushBack(&resolver->collisions, (U8*) &collision);
              }
            }
          }
        }
      }
    }

    for (ResolverEntry* resolver = c->resolvers; resolver != NULL; resolver = resolver->next) {
      if (resolver->collisions.size == 0) { continue; }
      resolver->fn(&resolver->collisions);
      DynamicArrayClear(&resolver->collisions);
    }
  }
}

void Physics2SetGravity(V2 gravity) {
  Physics2Context* c = &_cdef_phys_2d_context;
  c->rigid_body_gravity = gravity;
}

void Physics2RegisterResolver(U32 type_a, U32 type_b, Collision2Resolver_Fn* resolver) {
  Physics2Context* c = &_cdef_phys_2d_context;
  // NOTE: Trip if the resolver has already been registered.
  for (ResolverEntry* entry = c->resolvers; entry != NULL; entry = entry->next) {
    if (entry->fn == resolver) {
      DEBUG_ASSERT(!(entry->type_a == type_a && entry->type_b == type_b));
      DEBUG_ASSERT(!(entry->type_a == type_b && entry->type_b == type_a));
    }
  }

  ResolverEntry* entry = ARENA_PUSH_STRUCT(c->resolver_pool, ResolverEntry);
  entry->type_a = type_a;
  entry->type_b = type_b;
  DynamicArrayInit(&entry->collisions, sizeof(Collision2), 10);
  entry->fn = resolver;
  SLL_STACK_PUSH(c->resolvers, entry, next);
}

Collider2* Physics2RegisterColliderCircle(V2 center, F32 radius) {
  DEBUG_ASSERT(radius > 0);
  Collider2* collider = Collider2Allocate();
  collider->type = Collider2Type_Circle;
  collider->center = center;
  collider->broad_circle_radius = radius;
  collider->circle_radius = radius;
  return collider;
}

Collider2* Physics2RegisterColliderAabb(V2 center, V2 size) {
  DEBUG_ASSERT(size.x > 0 && size.y > 0);
  Collider2* collider = Collider2Allocate();
  collider->type = Collider2Type_Aabb;
  collider->center = center;
  Aabb2GetEnclosingCircle2(&center, &size, &collider->broad_circle_radius);
  collider->aabb_size = size;
  return collider;
}

void Physics2DeregisterCollider(Collider2* collider) {
  Physics2Context* c = &_cdef_phys_2d_context;
  DLL_REMOVE(c->collider_head, c->collider_tail, collider, prev, next);
  SLL_STACK_PUSH(c->collider_free_list, collider, next);
}

void Collider2SetSubtype(Collider2* collider, Collider2SubtypeHeader* subtype) {
  Collider2RemoveSubtype(collider, subtype->type);
  SLL_STACK_PUSH(collider->subtypes, subtype, next);
}

RigidBody2* Collider2SetRigidBody(Collider2* collider, RigidBody2Opts rigid_body_opts) {
  Physics2Context* c = &_cdef_phys_2d_context;
  DEBUG_ASSERT(rigid_body_opts.restitution >= 0);
  DEBUG_ASSERT(rigid_body_opts.mass > 0 || rigid_body_opts.type == RigidBody2Type_Static);
  DEBUG_ASSERT(rigid_body_opts.static_friction >= 0);
  DEBUG_ASSERT(rigid_body_opts.dynamic_friction >= 0);

  RigidBody2* rigid_body;
  if (c->rigid_body_free_list != NULL) {
    rigid_body = c->rigid_body_free_list;
    SLL_STACK_POP(c->rigid_body_free_list, next);
  } else {
    rigid_body = ARENA_PUSH_STRUCT(c->rigid_body_pool, RigidBody2);
  }
  MEMORY_ZERO_STRUCT(rigid_body);

  DLL_PUSH_BACK(c->rigid_body_head, c->rigid_body_tail, rigid_body, prev, next);
  rigid_body->header.type = COLLIDER2_RIGID_BODY;
  rigid_body->collider = collider;
  RigidBody2UpdateOpts(rigid_body, rigid_body_opts);
  Collider2SetSubtype(collider, &rigid_body->header);

  return rigid_body;
}

Collider2SubtypeHeader* Collider2GetSubtype(Collider2* collider, U32 type) {
  for (Collider2SubtypeHeader* subtype = collider->subtypes; subtype != NULL; subtype = subtype->next) {
    if (subtype->type == type) { return subtype; }
  }
  return NULL;
}

B32 Collider2RemoveSubtype(Collider2* collider, U32 type) {
  Physics2Context* c = &_cdef_phys_2d_context;
  if (collider->subtypes == NULL) { return false; }
  if (collider->subtypes->type == type) {
    collider->subtypes = collider->subtypes->next;
    return true;
  }
  for (Collider2SubtypeHeader* subtype = collider->subtypes; subtype->next != NULL; subtype = subtype->next) {
    if (subtype->next->type != type) { continue; }
    if (type == COLLIDER2_RIGID_BODY) {
      DLL_REMOVE(c->rigid_body_head, c->rigid_body_tail, (RigidBody2*) subtype->next, prev, next);
      SLL_STACK_PUSH(c->rigid_body_free_list, (RigidBody2*) subtype->next, next);
    }
    subtype->next = subtype->next->next;
    return true;
  }
  return false;
}

V2* Collider2Center(Collider2* collider) {
  return &collider->center;
}

U32* Collider2Group(Collider2* collider) {
  return &collider->group;
}

V2* RigidBody2Force(RigidBody2* rigid_body) {
  return &rigid_body->force;
}

V2* RigidBody2Velocity(RigidBody2* rigid_body) {
  return &rigid_body->velocity;
}

void RigidBody2UpdateOpts(RigidBody2* rigid_body, RigidBody2Opts opts) {
  rigid_body->type = opts.type;
  rigid_body->restitution = opts.restitution;
  rigid_body->static_friction = opts.static_friction;
  rigid_body->dynamic_friction = opts.dynamic_friction;
  if (rigid_body->type == RigidBody2Type_Dynamic) {
    rigid_body->mass_inv = 1.0f / opts.mass;
  } else {
    rigid_body->mass_inv = 0.0f;
  }
}

RigidBody2Opts RigidBody2GetOpts(RigidBody2* rigid_body) {
  RigidBody2Opts opts;
  MEMORY_ZERO_STRUCT(&opts);
  opts.type = rigid_body->type;
  opts.restitution = rigid_body->restitution;
  opts.static_friction = rigid_body->static_friction;
  opts.dynamic_friction = rigid_body->dynamic_friction;
  if (opts.type == RigidBody2Type_Dynamic) {
    opts.mass = 1.0f / rigid_body->mass_inv;
  }
  return opts;
}

#endif // CDEFAULT_PHYSICS2_IMPLEMENTATION
