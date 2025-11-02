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

U32 TEST = 'z' - 'A' + 1;

#define CDEFAULT_FONT_IMPLEMENTATION
#include "../cdefault_font.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "third_party/stb_truetype.h"

int main(void) {
  DEBUG_ASSERT(WindowInit(1920, 1080, "render image example"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  DirSetCurrentToExeDir();
  Arena* arena = ArenaAllocate();

  String8 file_data;
  DEBUG_ASSERT(FileReadAll(arena, Str8Lit("../data/firacode.ttf"), &file_data.str, &file_data.size));
  U64 arena_pos = ArenaPos(arena);

  Font font;

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
    DEBUG_ASSERT(FontInit(&font, file_data.str, file_data.size));
    DEBUG_ASSERT(FontBakeBitmap(arena, &font, 64, &font_image, 1024, 1024));
    DEBUG_ASSERT(ImageDumpBmp(&font_image, Str8Lit("../data/TEST.bmp")));

    ArenaPopTo(arena, arena_pos);
    WindowSwapBuffers();
    WindowFlushEvents();
  }
#else

  stbtt_bakedchar cdata[96];
  unsigned char stb_bitmap[512 * 512];
  stbtt_BakeFontBitmap(file_data.str, 0, 32, stb_bitmap, 512, 512, 'A', 'z' - 'A', cdata);

#endif

  return 0;
}
