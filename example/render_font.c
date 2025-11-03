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
#define CDEFAULT_FONT_IMPLEMENTATION
#include "../cdefault_font.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "third_party/stb_truetype.h"

#define ATLAS_WIDTH  2048.0f
#define ATLAS_HEIGHT 2048.0f
#define FONT_SIZE    24.0f

void DrawString(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 x, F32 y) {
  V2 cursor = { x, y };
  for (S32 i = 0; i < str.size; i++) {
    V2 center, size, min_uv, max_uv;
    DEBUG_ASSERT(FontAtlasPlace(atlas, str.str[i], &cursor, &center, &size, &min_uv, &max_uv));
    DrawFontCharacterV(atlas_handle, center, size, min_uv, max_uv, V3_BLACK);
  }
}

int main(void) {
  TimeInit();
  DEBUG_ASSERT(WindowInit(1920, 1080, "render font cdefault"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  DirSetCurrentToExeDir();
  Arena* font_arena = ArenaAllocate();
  Arena* temp_arena = ArenaAllocate();

  String8 file_data;
  DEBUG_ASSERT(FileReadAll(font_arena, Str8Lit("../data/firacode.ttf"), &file_data.str, &file_data.size));

  Font font;
  DEBUG_ASSERT(FontInit(&font, file_data.str, file_data.size));
  FontAtlas atlas;
  DEBUG_ASSERT(FontAtlasBake(font_arena, &font, &atlas, FontCharSetLatin(), FONT_SIZE, ATLAS_WIDTH, ATLAS_HEIGHT));
  U32 atlas_handle;
  RendererRegisterImageR(&atlas_handle, atlas.bitmap, atlas.bitmap_width, atlas.bitmap_height);

  Image font_image;
  font_image.format = ImageFormat_R;
  font_image.width  = atlas.bitmap_width;
  font_image.height = atlas.bitmap_height;
  font_image.data   = atlas.bitmap;
  DEBUG_ASSERT(ImageDumpBmp(&font_image, Str8Lit("../data/TEST_2.bmp")));

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    DrawString(Str8Lit("Hello, world!"), &atlas, atlas_handle, 128, 256);
    DrawString(Str8Lit("Text rendering is hard!"), &atlas, atlas_handle, 512, 512);

    String8 fps = Str8Format(temp_arena, "%0.2f FPS", 1.0f / dt_s);
    DrawString(fps, &atlas, atlas_handle, 128, 512);
    dt_s = StopwatchReadSeconds(&frame_stopwatch);
    StopwatchReset(&frame_stopwatch);

    ArenaClear(temp_arena);
    WindowSwapBuffers();
    WindowFlushEvents();
  }

  return 0;
}
