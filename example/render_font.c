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

#define BMP_FONT_HEIGHT  300.0f
#define SDF_FONT_HEIGHT  24.0f

void DrawString(String8 str, FontAtlas* atlas, U32 atlas_handle, U32 atlas_width, U32 atlas_height, F32 x, F32 y) {
  V2 cursor = { x, y };
  for (S32 i = 0; i < str.size; i++) {
    V2 center, size, min_uv, max_uv;
    DEBUG_ASSERT(FontAtlasPlace(atlas, atlas_width, atlas_height, str.str[i], 1.0f, &cursor, &center, &size, &min_uv, &max_uv));
    DrawFontCharacterV(atlas_handle, center, size, min_uv, max_uv, V3_BLACK);
  }
}

void DrawStringSdf(String8 str, FontAtlas* atlas, U32 atlas_handle, U32 atlas_width, U32 atlas_height, F32 x, F32 y) {
  V2 cursor = { x, y };
  for (S32 i = 0; i < str.size; i++) {
    V2 center, size, min_uv, max_uv;
    DEBUG_ASSERT(FontAtlasPlace(atlas, atlas_width, atlas_height, str.str[i], 1.0f, &cursor, &center, &size, &min_uv, &max_uv));
    DrawFontSdfCharacterV(atlas_handle, center, size, min_uv, max_uv, V3_BLACK);
  }
}

int main(void) {
  TimeInit();
  DEBUG_ASSERT(WindowInit(1920, 1080, "render font cdefault"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  DirSetCurrentToExeDir();
  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);
  Arena* font_arena = ArenaAllocate();
  Arena* temp_arena = ArenaAllocate();

  String8 file_data;
  DEBUG_ASSERT(FileReadAll(font_arena, Str8Lit("../data/firacode.ttf"), &file_data.str, &file_data.size));

  Font font;
  DEBUG_ASSERT(FontInit(&font, file_data.str, file_data.size));

  FontAtlas bmp_atlas;
  U8* bmp_atlas_bitmap;
  U32 bmp_atlas_width, bmp_atlas_height;
  DEBUG_ASSERT(FontAtlasBakeBitmap(
        font_arena, &font,
        &bmp_atlas, &bmp_atlas_bitmap, &bmp_atlas_width, &bmp_atlas_height,
        BMP_FONT_HEIGHT, FontCharSetLatin()));
  U32 bmp_atlas_handle;
  RendererRegisterImageR(&bmp_atlas_handle, bmp_atlas_bitmap, bmp_atlas_width, bmp_atlas_height);

  Image font_bitmap_image;
  font_bitmap_image.format = ImageFormat_R;
  font_bitmap_image.width  = bmp_atlas_width;
  font_bitmap_image.height = bmp_atlas_height;
  font_bitmap_image.data   = bmp_atlas_bitmap;
  DEBUG_ASSERT(ImageDumpBmp(&font_bitmap_image, Str8Lit("../data/TEST_BMP.bmp")));

  FontAtlas sdf_atlas;
  U8* sdf_atlas_bitmap;
  U32 sdf_atlas_width, sdf_atlas_height;
  DEBUG_ASSERT(FontAtlasBakeSdf(
        font_arena, &font,
        &sdf_atlas, &sdf_atlas_bitmap, &sdf_atlas_width, &sdf_atlas_height,
        BMP_FONT_HEIGHT, SDF_FONT_HEIGHT, FontCharSetLatin()));

  U32 sdf_atlas_handle;
  RendererRegisterImageR(&sdf_atlas_handle, sdf_atlas_bitmap, sdf_atlas_width, sdf_atlas_height);

  Image font_sdf_image;
  font_sdf_image.format = ImageFormat_R;
  font_sdf_image.width  = sdf_atlas_width;
  font_sdf_image.height = sdf_atlas_height;
  font_sdf_image.data   = sdf_atlas_bitmap;
  DEBUG_ASSERT(ImageDumpBmp(&font_sdf_image, Str8Lit("../data/TEST_SDF.bmp")));

  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }
    String8 fps = Str8Format(temp_arena, "%0.2f FPS", 1.0f / dt_s);

    DrawStringSdf(Str8Lit("Hello, world!"), &sdf_atlas, sdf_atlas_handle, sdf_atlas_width, sdf_atlas_height, 128, 256);
    DrawStringSdf(Str8Lit("Text rendering is hard!"), &sdf_atlas, sdf_atlas_width, sdf_atlas_height, sdf_atlas_handle, 512, 512);
    DrawStringSdf(fps, &sdf_atlas, sdf_atlas_handle, sdf_atlas_width, sdf_atlas_height, 128, 512);

    DrawStringSdf(Str8Lit("ABCQRTabpqg"), &sdf_atlas, sdf_atlas_handle, sdf_atlas_width, sdf_atlas_height, 512, 512);

    // DrawString(Str8Lit("Hello, world!"), &bmp_atlas, bmp_atlas_handle, bmp_atlas_width, bmp_atlas_height, 128, 256);
    // DrawString(Str8Lit("Text rendering is hard!"), &bmp_atlas, bmp_atlas_handle, bmp_atlas_width, bmp_atlas_height, 512, 512);
    // DrawString(fps, &bmp_atlas, bmp_atlas_handle, bmp_atlas_width, bmp_atlas_height, 128, 512);

    dt_s = StopwatchReadSeconds(&frame_stopwatch);
    StopwatchReset(&frame_stopwatch);
    ArenaClear(temp_arena);
    WindowSwapBuffers();
    WindowFlushEvents();
  }

  return 0;
}
