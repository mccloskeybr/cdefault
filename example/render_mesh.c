#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"
#define CDEFAULT_IO_IMPLEMENTATION
#include "../cdefault_io.h"
#define CDEFAULT_IMAGE_IMPLEMENTATION
#include "../cdefault_image.h"
#define CDEFAULT_MESH_IMPLEMENTATION
#include "../cdefault_mesh.h"

int main(void) {
  TimeInit();
  DirSetCurrentToExeDir();
  ASSERT(WindowInit(1280, 720, "physics"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  Arena* arena = ArenaAllocate();

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  U32 image_handle, mesh_handle;
  Image image;
  DEBUG_ASSERT(ImageLoadFile(arena, &image, ImageFormat_RGBA, Str8Lit("../data/16bpp.bmp")));
  RendererRegisterImage(&image_handle, image.data, image.width, image.height);
  Mesh obj;
  DEBUG_ASSERT(MeshLoadFile(arena, &obj, Str8Lit("../data/teapot.obj")));
  RendererRegisterMesh(&mesh_handle, image_handle, obj.points, obj.normals, obj.uvs, obj.vertices_size, obj.indices, obj.indices_size);

  Camera* camera = RendererCamera3D();
  camera->pos = (V3) { 0, 0, 3 };

  F32 obj_theta = 0;
  while(!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V3 cam_pos_delta;
    V3MultF32(&cam_pos_delta, &camera->look_dir, dt_s);
    if (WindowIsKeyPressed(Key_S)) {
      V3SubV3(&camera->pos, &camera->pos, &cam_pos_delta);
    } else if (WindowIsKeyPressed(Key_W)) {
      V3AddV3(&camera->pos, &camera->pos, &cam_pos_delta);
    }
    if (WindowIsKeyPressed(Key_A)) {
      V3RotateAroundAxis(&camera->look_dir, &camera->look_dir, &camera->up_dir, +0.1f);
      // V3RotateAroundAxis(&camera->up_dir, &camera->up_dir, &camera->look_dir, -0.1f);
    } else if (WindowIsKeyPressed(Key_D)) {
      V3RotateAroundAxis(&camera->look_dir, &camera->look_dir, &camera->up_dir, -0.1f);
      // V3RotateAroundAxis(&camera->up_dir, &camera->up_dir, &camera->look_dir, +0.1f);
    }

    V4 rot_1, rot_2, rot_3, rot_4;
    V4RotateAroundAxis(&rot_1, &V3_X_POS, +obj_theta);
    V4RotateAroundAxis(&rot_2, &V3_Y_POS, -obj_theta);
    V4RotateAroundAxis(&rot_3, &V3_Z_POS, +obj_theta);
    V4QuatMulV4(&rot_4, &rot_1, &rot_2);
    // V4Normalize(&rot_4, &rot_4);
    V4QuatMulV4(&rot_4, &rot_4, &rot_3);
    V4Normalize(&rot_4, &rot_4);
    DrawMesh(mesh_handle, (V3) { -0.5f, 0, 0 }, rot_4, (V3) { 0.1f, 0.1f, 0.1f});
    DrawMesh(mesh_handle, (V3) { +0.5f, 0, 0 }, rot_4, (V3) { 0.1f, 0.1f, 0.1f});
    obj_theta += 0.01f;

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  ArenaRelease(arena);
  return 0;
}
