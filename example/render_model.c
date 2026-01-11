#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  TimeInit();
  DirSetCurrentToExeDir();
  ASSERT(WindowInit(1280, 720, "physics"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  Arena* arena = ArenaAllocate();

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  Model model;
  // DEBUG_ASSERT(ModelLoadFile(arena, &model, Str8Lit("../data/suzanne.obj")));
  DEBUG_ASSERT(ModelLoadFile(arena, &model, Str8Lit("../data/computer.glb")));
  U32 model_handle = RendererRegisterModel(&model);

  Camera* camera = RendererCamera3D();
  camera->pos = (V3) { 0, 0, 3 };

  F32 obj_theta = 0;
  while(!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V3 cam_pos_delta = V3MultF32(camera->look_dir, dt_s);
    if (WindowIsKeyPressed(Key_S)) {
      camera->pos = V3SubV3(camera->pos, cam_pos_delta);
    } else if (WindowIsKeyPressed(Key_W)) {
      camera->pos = V3AddV3(camera->pos, cam_pos_delta);
    }
    if (WindowIsKeyPressed(Key_A)) {
      camera->look_dir = V3RotateAroundAxis(camera->look_dir, camera->up_dir, +0.1f);
    } else if (WindowIsKeyPressed(Key_D)) {
      camera->look_dir = V3RotateAroundAxis(camera->look_dir, camera->up_dir, -0.1f);
    }
    V3 cam_vert_delta = V3MultF32(V3_Y_POS, dt_s);
    if (WindowIsKeyPressed(Key_Shift)) {
      camera->pos = V3AddV3(camera->pos, cam_vert_delta);
    } else if (WindowIsKeyPressed(Key_Control)) {
      camera->pos = V3SubV3(camera->pos, cam_vert_delta);
    }

    V4 rot_1 = V4RotateAroundAxis(V3_X_POS, +obj_theta);
    V4 rot_2 = V4RotateAroundAxis(V3_Y_POS, -obj_theta);
    V4 rot_3 = V4RotateAroundAxis(V3_Z_POS, +obj_theta);
    V4 rot_4 = V4QuatMulV4(rot_1, rot_2);
    // V4Normalize(&rot_4, &rot_4);
    rot_4 = V4QuatMulV4(rot_4, rot_3);
    rot_4 = V4Normalize(rot_4);

    DrawTetrahedronV(V3Assign(-0.5f, -0.5f, -0.5f), V3Assign(0.5f, -0.5f, -0.5f), V3Assign(0, -0.5f, 0.5f), V3Assign(0, -1.0f, 0), V3_RED);

    DrawLine3V(V3Assign(-0.5f, 0, 0), V3Assign(0.5f, 0, 0), V3_RED);

    // RendererEnableWireframe();
    // DrawSphere(-0.5f, 0, 0, 0, 0, 0, 1, 0.5f, 1, 0, 0);
    // DrawCube(-0.5f, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0);
    DrawTetrahedronV(V3Assign(-0.5f, -0.5f, -0.5f), V3Assign(0.5f, -0.5f, -0.5f), V3Assign(0, -0.5f, 0.5f), V3Assign(0, -1.0f, 0), V3_BLACK);
    DrawModelV(model_handle, (V3) { -0.5f, 0, 0 }, rot_4, (V3) { 0.1f, 0.1f, 0.1f});
    DrawModelV(model_handle, (V3) { +0.5f, 0, 0 }, rot_4, (V3) { 0.1f, 0.1f, 0.1f});
    // RendererDisableWireframe();

    obj_theta += 0.01f;

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  ArenaRelease(arena);
  return 0;
}
