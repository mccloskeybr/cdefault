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
  ASSERT(WindowInit(1920, 1080, "render image example"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  Arena* arena = ArenaAllocate();

  Image image;
  DEBUG_ASSERT(ImageLoadFile(arena, &image, ImageFormat_RGBA, (U8*) "../data/16bpp.bmp"));
  U32 image_handle;
  RendererRegisterImage(&image_handle, image.data, image.width, image.height);

  ArenaRelease(arena);

  F32 theta = 0;
  F32 scale = 1;
  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    DrawImageRot(image_handle, 750, 500, image.width * scale * 25, image.height * scale * 25, theta);
    theta += 0.05f;
    scale = F32Sin(TimeSecondsSinceStart());

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
