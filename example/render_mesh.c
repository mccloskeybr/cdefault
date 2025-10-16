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

int main(void) {
  TimeInit();
  DirSetCurrentToExeDir();
  ASSERT(WindowInit(1280, 720, "physics"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  Arena* arena = ArenaAllocate();

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  Image image;
  U32 image_handle, mesh_handle;
  DEBUG_ASSERT(ImageLoadFile(arena, &image, ImageFormat_RGBA, (U8*) "../data/16bpp.bmp"));
  RendererRegisterImage(&image_handle, image.data, image.width, image.height);

  V3 points[] = {
    {-0.1f, -0.1f,  0.1f},
    { 0.1f, -0.1f,  0.1f},
    { 0.1f,  0.1f,  0.1f},
    {-0.1f,  0.1f,  0.1f},
    {-0.1f, -0.1f, -0.1f},
    { 0.1f, -0.1f, -0.1f},
    { 0.1f,  0.1f, -0.1f},
    {-0.1f,  0.1f, -0.1f},
  };
  U32 indices[] = {
    // Front face
    0, 1, 2,
    0, 2, 3,
    // Back face (4, 7, 6, 5)
    4, 7, 6,
    4, 6, 5,
    // Left face (4, 0, 3, 7)
    4, 0, 3,
    4, 3, 7,
    // Right face (1, 5, 6, 2)
    1, 5, 6,
    1, 6, 2,
    // Top face (3, 2, 6, 7)
    3, 2, 6,
    3, 6, 7,
    // Bottom face (4, 5, 1, 0)
    4, 5, 1,
    4, 1, 0
  };
  RendererRegisterMesh(&mesh_handle, image_handle, points, 8, NULL, 0, NULL, 0, indices, 36);

  M4 projection;
  M4Perspective(&projection, F32_PI / 4.0f, 16.0f / 9.0f, 0.01f, 1000.0f);
  RendererSetProjection(projection);

  F32 theta = 0;
  while(!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V4 rot;
    V4RotateAroundAxis(&rot, &V3_Y_POS, theta);
    DrawMesh(mesh_handle, (V3) { 0, 0, 0 }, rot);
    theta += 0.01f;

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
    StopwatchReset(&frame_stopwatch);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  ArenaRelease(arena);
  return 0;
}
