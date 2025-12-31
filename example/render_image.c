#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

int main(void) {
  TimeInit();
  DirSetCurrentToExeDir();
  ASSERT(WindowInit(1920, 1080, "render image example"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  Arena* arena = ArenaAllocate();


  Image image;
  // DEBUG_ASSERT(ImageLoadFile(arena, &image, ImageFormat_RGBA, Str8Lit("../data/16bpp.bmp")));
  DEBUG_ASSERT(ImageLoadFile(arena, &image, ImageFormat_RGBA, Str8Lit("../data/leia.png")));
  DEBUG_ASSERT(ImageDumpBmp(&image, Str8Lit("../data/TEST_2.bmp")));
  U32 image_handle = RendererRegisterImage(&image);

  ArenaRelease(arena);

  F32 theta = 0;
  // F32 scale = 1;
  F32 scale = 0.25f;
  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    DrawImageRot(image_handle, 750, 500, image.width * scale * 25, image.height * scale * 25, theta);
    // theta += 0.05f;
    // scale = F32Sin(TimeSecondsSinceStart());

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
