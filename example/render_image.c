#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

int main(void) {
  TimeInit();
  ASSERT(WindowInit(1920, 1080, "render image example"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  S32 image_width, image_height, image_n_channels;
  U8* image_bytes = stbi_load(
      "Z:\\cdefault\\example\\data\\leia.png",
      &image_width, &image_height, &image_n_channels, 0);
  U32 image_handle;
  RendererRegisterImage(&image_handle, image_bytes, image_width, image_height);
  stbi_image_free(image_bytes);

  F32 theta = 0;
  F32 scale = 0;
  while (!WindowShouldClose()) {
    DrawImageRot(image_handle, 750, 500, image_width * scale * 5, image_height * scale * 5, theta);
    theta += 0.05f;
    scale = F32Sin(TimeSecondsSinceStart());

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
