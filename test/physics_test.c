#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "../cdefault_geometry.h"
#define CDEFAULT_PHYSICS_2D_IMPLEMENTATION
#include "../cdefault_physics_2d.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"

#define CIRCLE_RADIUS 50
#define INPUT_FORCE 500.0f
#define COLOR_RED  (V3) { 1, 0, 0 }
#define COLOR_BLUE (V3) { 0, 0, 1 }

int main(void) {
  TimeInit();
  ASSERT(WindowInit(1280, 720, "physics"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  Physics2DInit();
  Physics2DSetGravity((V2) { 0, -9.8f });

  V2 a_pos;
  a_pos.x = 75;
  a_pos.y = 300;
  RigidBody2Opts a_rb_opts;
  a_rb_opts.type = RigidBody2Type_Dynamic;
  a_rb_opts.restitution = 0.98f;
  a_rb_opts.mass = 1;
  a_rb_opts.static_friction = 0.00f;
  a_rb_opts.dynamic_friction = 0.00f;
  Collider2* a = Physics2DRegisterColliderCircle(a_pos, CIRCLE_RADIUS);
  RigidBody2* a_rb = Collider2SetRigidBody(a, a_rb_opts);

  V2 c_pos;
  c_pos.x = 100;
  c_pos.y = 500;
  RigidBody2Opts c_rb_opts;
  c_rb_opts.type = RigidBody2Type_Dynamic;
  c_rb_opts.restitution = 0.01f;
  c_rb_opts.mass = 1000;
  c_rb_opts.static_friction = 0.00f;
  c_rb_opts.dynamic_friction = 0.00f;
  Collider2* c = Physics2DRegisterColliderCircle(c_pos, CIRCLE_RADIUS);
  Collider2SetRigidBody(c, c_rb_opts);

  V2 b_pos;
  b_pos.x = 500;
  b_pos.y = 100;
  V2 b_size;
  b_size.x = 1000;
  b_size.y = 100;
  RigidBody2Opts b_rb_opts;
  b_rb_opts.type = RigidBody2Type_Static;
  b_rb_opts.restitution = 0.2f;
  b_rb_opts.static_friction = 0.2f;
  b_rb_opts.dynamic_friction = 0.2f;
  Collider2* b = Physics2DRegisterColliderAabb(b_pos, b_size);
  Collider2SetRigidBody(b, b_rb_opts);

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  while(!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    MEMORY_ZERO_STRUCT(RigidBody2Force(a_rb));
    if (WindowIsKeyPressed(Key_W)) {
      RigidBody2Force(a_rb)->y = INPUT_FORCE;
    } else if (WindowIsKeyPressed(Key_S)) {
      RigidBody2Force(a_rb)->y = -INPUT_FORCE;
    }
    if (WindowIsKeyPressed(Key_A)) {
      RigidBody2Force(a_rb)->x = -INPUT_FORCE;
    } else if (WindowIsKeyPressed(Key_D)) {
      RigidBody2Force(a_rb)->x = INPUT_FORCE;
    }

    Physics2DUpdate(dt_s);

    DrawCircleV(*Collider2Position(a), CIRCLE_RADIUS, COLOR_RED);
    DrawCircleV(*Collider2Position(c), CIRCLE_RADIUS, COLOR_BLUE);
    DrawRectangleV(*Collider2Position(b), b_size, COLOR_BLUE);

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);

    WindowSwapBuffers();
    WindowFlushEvents();
  }
  return 0;
}
