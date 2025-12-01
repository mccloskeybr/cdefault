#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

#define SPHERE_RADIUS 5
#define RECT_SIZE (V3) {5, 5, 5}
#define GROUND_SIZE (V3) { 10000, 5, 10000 }

Collider3* colliders[255];
U32 next_collider = 0;

static void AddColliderSphere(V3 pos) {
  Collider3* c = Physics3RegisterColliderSphere(pos, SPHERE_RADIUS);
  Physics3RegisterRigidBodyDynamic(c, 50);
  colliders[next_collider++] = c;
}

static void AddColliderRect(V3 pos) {
  Collider3* c = Physics3RegisterColliderRect(pos, RECT_SIZE);
  Physics3RegisterRigidBodyDynamic(c, 50);
  colliders[next_collider++] = c;
}

int main(void) {
  TimeInit();
  ASSERT(WindowInit(1280, 720, "physics 3d"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  Physics3Init(5);
  RendererEnableDepthTest();
  Physics3SetGravity((V3) { 0, -90.8f, 0 });

  Camera* camera = RendererCamera3D();
  camera->pos = V3Assign(0, 20, 50);
  // V3SubV3(&camera->look_dir, &V3_ZEROES, &camera->pos);
  V3Normalize(&camera->look_dir, &camera->look_dir);

  Collider3* ground = Physics3RegisterColliderRect(V3_ZEROES, GROUND_SIZE);
  Physics3RegisterRigidBodyStatic(ground);

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  while(!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    if (WindowIsMouseButtonJustPressed(MouseButton_Left)) {
      AddColliderSphere(V3Assign(2.0f, 50, 0));
    } else if (WindowIsMouseButtonJustPressed(MouseButton_Right)) {
      AddColliderRect(V3Assign(0, 50, 0));
    }

    Physics3Update(dt_s);

    DrawCubeV(ground->center, V4_QUAT_IDENT, GROUND_SIZE, V3_BLUE);
    for (U32 i = 0; i < next_collider; i++) {
      Collider3* c = colliders[i];
      switch (c->type) {
        case Collider3Type_Sphere: {
          DrawSphereV(c->sphere.center, V4_QUAT_IDENT, c->sphere.radius, V3_RED);
          RendererEnableWireframe();
          DrawSphereV(c->sphere.center, V4_QUAT_IDENT, c->sphere.radius, V3_BLACK);
          RendererDisableWireframe();
        } break;
        case Collider3Type_ConvexHull: {
          DrawCubeV(c->center, V4_QUAT_IDENT, RECT_SIZE, V3_RED);
          RendererEnableWireframe();
          DrawCubeV(c->center, V4_QUAT_IDENT, RECT_SIZE, V3_BLACK);
          RendererDisableWireframe();
        } break;
      }
    }

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);

    WindowSwapBuffers();
    WindowFlushEvents();
  }
  return 0;
}
