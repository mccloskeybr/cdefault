#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_VIDEO_IMPLEMENTATION
#include "../cdefault_video.h"
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

int main(void) {
  ASSERT(WindowInit(1920, 1080, "video example"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  S32 image_width, image_height, image_n_channels;
  U8* image_data = stbi_load(
      "Z:\\cdefault\\example\\data\\leia.png",
      &image_width, &image_height, &image_n_channels, 0);
  ASSERT(image_data != NULL);
  U32 image_handle;
  RendererRegisterImage(&image_handle, image_data, image_width, image_height);
  stbi_image_free(image_data);

  V2 pos = { 0, 0 };
  V2 size = { 100, 100 };
  V3 color = { 1, 0.5, 0 };

  B32 toggle_fullscreen = false;
  while (!WindowShouldClose()) {
    if (WindowIsKeyJustPressed(Key_Space)) {
      WindowFullscreen(toggle_fullscreen);
      toggle_fullscreen = !toggle_fullscreen;
    }
    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &pos);

    DrawImage(image_handle, 500, 500, image_width, image_height);
    DrawRoundedRectangleV(pos, size, 30, color);
    DrawLine(10, 10, 400, 100, 20, 1, 0, 1);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
