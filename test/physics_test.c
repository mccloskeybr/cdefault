#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "../cdefault_geometry.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"
#define CDEFAULT_PHYSICS2_IMPLEMENTATION
#include "../cdefault_physics_2d.h"

#define CIRCLE_RADIUS 20
#define AABB_SIZE (V2) {40, 40}

#define COLOR_RED  (V3) { 1, 0, 0 }
#define COLOR_BLUE (V3) { 0, 0, 1 }
#define COLOR_BLACK (V3) { 0, 0, 0 }

Collider2* circle_colliders[255];
U32 next_circle_collider = 0;
Collider2* obb_colliders[255];
U32 next_obb_collider = 0;

static void DrawVelocityArrow(Collider2* c) {
  RigidBody2* rb = (RigidBody2*) Collider2GetSubtype(c, COLLIDER2_RIGID_BODY);
  V2* start_pos = Collider2Center(c);
  V2 end_pos;
  V2AddV2(&end_pos, start_pos, RigidBody2Velocity(rb));
  DrawLineV(*start_pos, end_pos, 5, (V3) { 0.5f, 0.0f, 0.5f });

  V2 start = *Collider2Center(c);
  V2 end = (V2) { F32Cos(*Collider2AngleRad(c)), F32Sin(*Collider2AngleRad(c)) };
  V2MultF32(&end, &end, CIRCLE_RADIUS);
  V2AddV2(&end, &end, &start);
  DrawLineV(start, end, 5, COLOR_BLACK);
}

static void AddColliderCircle(V2 pos) {
  RigidBody2Opts opts;
  opts.type = RigidBody2Type_Dynamic;
  opts.fix_angle = false;
  opts.restitution = 0.5f;
  opts.mass = 50;
  opts.static_friction = 0.6f;
  opts.dynamic_friction = 0.4f;
  Collider2* c = Physics2RegisterColliderCircle(pos, CIRCLE_RADIUS, 0);
  Collider2SetRigidBody(c, opts);
  circle_colliders[next_circle_collider++] = c;
}

static void AddColliderObb(V2 pos) {
  RigidBody2Opts opts;
  opts.type = RigidBody2Type_Dynamic;
  opts.fix_angle = false;
  opts.restitution = 0.5f;
  opts.mass = 50;
  opts.static_friction = 0.6f;
  opts.dynamic_friction = 0.4f;
  Collider2* c = Physics2RegisterColliderObb(pos, AABB_SIZE, 0);
  Collider2SetRigidBody(c, opts);
  obb_colliders[next_obb_collider++] = c;
}

int main(void) {
  TimeInit();
  ASSERT(WindowInit(1280, 720, "physics"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  Physics2Init(5);
  Physics2SetGravity((V2) { 0, -90.8f });

  V2 g_pos;
  g_pos.x = 500;
  g_pos.y = 50;
  V2 g_size;
  g_size.x = 10000;
  g_size.y = 100;
  RigidBody2Opts g_rb_opts;
  g_rb_opts.type = RigidBody2Type_Static;
  g_rb_opts.fix_angle = true;
  g_rb_opts.restitution = 0.5f;
  g_rb_opts.static_friction = 0.6f;
  g_rb_opts.dynamic_friction = 0.4f;
  Collider2* ground = Physics2RegisterColliderObb(g_pos, g_size, 0);
  Collider2SetRigidBody(ground, g_rb_opts);

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  while(!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &mouse_pos);
    if (WindowIsMouseButtonJustPressed(MouseButton_Left)) {
      AddColliderCircle(mouse_pos);
    } else if (WindowIsMouseButtonJustPressed(MouseButton_Right)) {
      AddColliderObb(mouse_pos);
    }

    Physics2Update(dt_s);

    DrawRectangleV(*Collider2Center(ground), g_size, COLOR_BLUE);
    for (U32 i = 0; i < next_circle_collider; i++) {
      Collider2* c = circle_colliders[i];
      DrawCircleV(*Collider2Center(c), CIRCLE_RADIUS, COLOR_RED);
      DrawVelocityArrow(c);
    }
    for (U32 i = 0; i < next_obb_collider; i++) {
      Collider2* c = obb_colliders[i];
      DrawRectangleRotV(*Collider2Center(c), AABB_SIZE, *Collider2AngleRad(c), COLOR_RED);
      DrawVelocityArrow(c);
    }

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);

    WindowSwapBuffers();
    WindowFlushEvents();
  }
  return 0;
}
