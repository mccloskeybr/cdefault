#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_IO_IMPLEMENTATION
#include "../cdefault_io.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "third_party/stb_truetype.h"

#define ATLAS_WIDTH  2048
#define ATLAS_HEIGHT 2048
#define FONT_SIZE    200.0f

U32 font_atlas;
U8  atlas_bitmap[ATLAS_WIDTH * ATLAS_HEIGHT];
stbtt_bakedchar cdata['~' - ' '];

void DrawString(String8 text, F32 x, F32 y) {
  DrawLine(0, y, 3000, y, 3, 0, 0, 1);
  DrawCircle(x, y, 10, 1, 0, 0);
  for (S32 i = 0; i < text.size; i++) {
    U8 c = text.str[i];

    stbtt_aligned_quad q;
    stbtt_GetBakedQuad(cdata, ATLAS_WIDTH, ATLAS_HEIGHT, c - 32, &x, &y, &q, 1);

    F32 glyph_w = q.x1 - q.x0;
    F32 glyph_h = q.y1 - q.y0;
    F32 glyph_x = q.x0 - (glyph_w / 2.0f);
    F32 glyph_y = q.y0 + (2 * glyph_h);

    DrawSubImage(
        font_atlas,
        glyph_x + (glyph_w / 2.0f), glyph_y - (glyph_h / 2.0f),
        glyph_w, glyph_h,
        q.s0, q.t0, q.s1, q.t1);
  }
}

int main(void) {
  DEBUG_ASSERT(WindowInit(1920, 1080, "render font stbtt"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  DirSetCurrentToExeDir();
  Arena* arena = ArenaAllocate();

  String8 file_data;
  DEBUG_ASSERT(FileReadAll(arena, Str8Lit("c:/windows/fonts/times.ttf"), &file_data.str, &file_data.size));
  stbtt_BakeFontBitmap(file_data.str, 0, FONT_SIZE, atlas_bitmap, ATLAS_WIDTH, ATLAS_HEIGHT, ' ', '~' - ' ', cdata);
  RendererRegisterImageR(&font_atlas, atlas_bitmap, ATLAS_WIDTH, ATLAS_HEIGHT);

  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    DrawString(Str8Lit("hello world"), 16, 512);

    WindowSwapBuffers();
    WindowFlushEvents();
  }
  return 0;
}
