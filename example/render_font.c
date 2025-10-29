#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_IO_IMPLEMENTATION
#include "../cdefault_io.h"
#define CDEFAULT_IMAGE_IMPLEMENTATION
#include "../cdefault_image.h"

#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"

U32 TEST = 0;

#define CDEFAULT_FONT_IMPLEMENTATION
#include "../cdefault_font.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "third_party/stb_truetype.h"

// 955  230 true
// 955  166 false
// <fake point>
// 997  108 false
// 1041 95  true
// 1000 -23 true
// 932  -14 false
// 830  55  false
// 807  125 true
// 750  52  false
// 579  -23 false

int main(void) {
  DEBUG_ASSERT(WindowInit(1920, 1080, "render image example"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  DirSetCurrentToExeDir();
  Arena* arena = ArenaAllocate();

  String8 file_data;
  DEBUG_ASSERT(FileReadAll(arena, (U8*) "../data/firacode.ttf", &file_data.str, &file_data.size));

#if 1
  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }
    if (WindowIsKeyJustPressed(Key_Space)) {
      TEST++;
    }

    Image font_image;
    DEBUG_ASSERT(FontBakeBitmap(arena, file_data.str, file_data.size, 32, &font_image, 500, 500));

    WindowSwapBuffers();
    WindowFlushEvents();
  }
#else

  stbtt_bakedchar cdata[96];
  unsigned char stb_bitmap[512 * 512];
  stbtt_BakeFontBitmap(file_data.str, 0, 32, stb_bitmap, 512, 512, 'a', 'z' - 'a', cdata);

#endif

  return 0;
}
