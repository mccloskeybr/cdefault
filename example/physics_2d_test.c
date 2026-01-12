#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

#define CIRCLE_RADIUS 20
#define RECT_SIZE (V2) {40, 40}

#define COLOR_RED  (V3) { 1, 0, 0 }
#define COLOR_BLUE (V3) { 0, 0, 1 }
#define COLOR_BLACK (V3) { 0, 0, 0 }

Collider2* colliders[255];
U32 next_collider = 0;

static void DrawVelocityArrow(Collider2* c) {
  RigidBody2* rb = (RigidBody2*) Collider2GetSubtype(c, COLLIDER2_RIGID_BODY);
  V2 start_pos = c->center;
  V2 end_pos = V2AddV2(start_pos, rb->velocity);
  DrawLineV(start_pos, end_pos, 5, V3Assign(0.5f, 0.0f, 0.5f));

  V2 start = c->center;
  V2 end = V2Assign(F32Cos(c->angle_rad), F32Sin(c->angle_rad));
  end = V2AddV2(start, V2MultF32(end, CIRCLE_RADIUS));
  DrawLineV(start, end, 5, COLOR_BLACK);
}

static void AddColliderCircle(V2 pos) {
  Collider2* c = Physics2RegisterCollider();
  RigidBody2* rb = Physics2RegisterRigidBody(c);
  Collider2SetCircle(c, pos, CIRCLE_RADIUS);
  RigidBody2SetDynamic(rb, 50);
  colliders[next_collider++] = c;
}

static void AddColliderRect(V2 pos) {
  Collider2* c = Physics2RegisterCollider();
  RigidBody2* rb = Physics2RegisterRigidBody(c);
  Collider2SetRect(c, pos, RECT_SIZE);
  RigidBody2SetDynamic(rb, 50);
  colliders[next_collider++] = c;
}

static void AddColliderConvexHull(V2 pos) {
  V2 hull_points[6] = {
    {-25 + pos.x, -25 + pos.y},
    {+25 + pos.x, -25 + pos.y},
    {+50 + pos.x, +0  + pos.y},
    {+25 + pos.x, +25 + pos.y},
    {-25 + pos.x, +25 + pos.y},
    {-50 + pos.x, +0  + pos.y},
  };
  Collider2* c = Physics2RegisterCollider();
  RigidBody2* rb = Physics2RegisterRigidBody(c);
  Collider2SetConvexHull(c, hull_points, 6);
  RigidBody2SetDynamic(rb, 50);
  colliders[next_collider++] = c;
}

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  TimeInit();
  ASSERT(WindowInit(1280, 720, "physics 2d"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  RendererDisableDepthTest();
  Physics2Init(5);
  Physics2SetGravity(V2Assign(0, -90.8f));

  V2 g_pos  = V2Assign(500, 50);
  V2 g_size = V2Assign(10000, 100);
  Collider2* ground_c = Physics2RegisterCollider();
  RigidBody2* ground_rb = Physics2RegisterRigidBody(ground_c);
  Collider2SetRect(ground_c, g_pos, g_size);
  RigidBody2SetStatic(ground_rb);
  ground_rb->fix_angle = true;

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  while(!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V2 mouse_pos = WindowGetMousePositionV();
    if (WindowIsMouseButtonJustPressed(MouseButton_Left)) {
      AddColliderCircle(mouse_pos);
    } else if (WindowIsMouseButtonJustPressed(MouseButton_Right)) {
      // AddColliderRect(mouse_pos);
      AddColliderConvexHull(mouse_pos);
    }

    Physics2Update(dt_s);

    DrawRectangleV(ground_c->center, ground_c->rect.size, COLOR_BLUE);
    for (U32 i = 0; i < next_collider; i++) {
      Collider2* c = colliders[i];
      DrawRingV(c->center, c->broad_circle_radius, 1, COLOR_BLUE);
      switch (c->type) {
        case Collider2Type_Circle: {
          DrawCircleV(c->circle.center, c->circle.radius, COLOR_RED);
        } break;
        case Collider2Type_Rect: {
          DrawRectangleRotV(c->rect.center, c->rect.size, c->angle_rad, COLOR_RED);
        } break;
        case Collider2Type_ConvexHull: {
          DrawConvexHullV(c->convex_hull.points_world, c->convex_hull.points_size, COLOR_RED);
        } break;
      }
      DrawVelocityArrow(c);
    }

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);

    WindowSwapBuffers();
    WindowFlushEvents();
  }
  return 0;
}
