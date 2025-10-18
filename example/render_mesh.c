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
  DEBUG_ASSERT(ImageLoadFile(arena, &image, ImageFormat_RGBA, (U8*) "../data/16bpp.bmp"));
  RendererRegisterImage(&image_handle, image.data, image.width, image.height);
  Mesh cube;
  DEBUG_ASSERT(MeshLoadFile(arena, &cube, (U8*) "../data/teapot.obj"));
  RendererRegisterMesh(&mesh_handle, image_handle, cube.points, cube.normals, cube.uvs, cube.vertices_size, cube.indices, cube.indices_size);

  M4 projection;
  M4Perspective(&projection, F32_PI / 4.0f, 16.0f / 9.0f, 0.01f, 1000.0f);
  RendererSetProjection(projection);

  F32 theta = 0;
  while(!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V4 rot_1, rot_2, rot_3;
    V4RotateAroundAxis(&rot_1, &V3_X_POS, theta);
    V4RotateAroundAxis(&rot_2, &V3_Y_POS, theta);
    V4QuatMulV4(&rot_3, &rot_1, &rot_2);
    DrawMesh(mesh_handle, (V3) { 0, 0, 0 }, rot_3);
    theta += 0.01f;

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  ArenaRelease(arena);
  return 0;
}
