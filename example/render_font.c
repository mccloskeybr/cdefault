#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

#define BMP_FONT_HEIGHT 100.0f
#define SDF_FONT_HEIGHT 32.0f

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  TimeInit();
  DEBUG_ASSERT(WindowInit(1920, 1080, "render font cdefault"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);
  RendererDisableDepthTest();
  DirSetCurrentToExeDir();
  Arena* font_arena = ArenaAllocate();

  String8 file_data;
  // DEBUG_ASSERT(FileReadAll(font_arena, Str8Lit("../data/firacode.ttf"), &file_data.str, &file_data.size));
  // DEBUG_ASSERT(FileReadAll(font_arena, Str8Lit("c:/windows/fonts/times.ttf"), &file_data.str, &file_data.size));
  // DEBUG_ASSERT(FileReadAll(font_arena, Str8Lit("c:/windows/fonts/consola.ttf"), &file_data.str, &file_data.size));
  DEBUG_ASSERT(FileReadAll(font_arena, Str8Lit("c:/windows/fonts/verdana.ttf"), &file_data.str, &file_data.size));
  Font font; DEBUG_ASSERT(FontInit(&font, file_data.str, file_data.size));

  FontAtlas bmp_atlas;
  Image     bmp_atlas_image;
  DEBUG_ASSERT(FontAtlasBakeBitmap(font_arena, font_arena, &font, &bmp_atlas, &bmp_atlas_image, BMP_FONT_HEIGHT, FontCharSetLatin()));
  DEBUG_ASSERT(ImageDumpBmp(&bmp_atlas_image, Str8Lit("../data/TEST_BMP.bmp")));
  U32 bmp_atlas_handle = RendererRegisterImage(&bmp_atlas_image);

  FontAtlas sdf_atlas;
  Image     sdf_atlas_image;
  DEBUG_ASSERT(FontAtlasBakeSdf(font_arena, font_arena, &font, &sdf_atlas, &sdf_atlas_image, BMP_FONT_HEIGHT, SDF_FONT_HEIGHT, 4.0f, FontCharSetLatin()));
  DEBUG_ASSERT(ImageDumpBmp(&sdf_atlas_image, Str8Lit("../data/TEST_SDF.bmp")));
  U32 sdf_atlas_handle = RendererRegisterImage(&sdf_atlas_image);

  String8 alphabet = Str8Lit("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    DrawStringSdf(alphabet, &sdf_atlas, sdf_atlas_handle, 64, 8, 264, 0, 0, 0);
    DrawStringBmp(alphabet, &bmp_atlas, bmp_atlas_handle, 64, 8, 200, 0, 0, 0);

    DrawStringSdf(alphabet, &sdf_atlas, sdf_atlas_handle, 32, 8, 150, 0, 0, 0);
    DrawStringBmp(alphabet, &bmp_atlas, bmp_atlas_handle, 32, 8, 118, 0, 0, 0);

    DrawStringSdf(alphabet, &sdf_atlas, sdf_atlas_handle, 20, 8, 85, 0, 0, 0);
    DrawStringBmp(alphabet, &bmp_atlas, bmp_atlas_handle, 20, 8, 65, 0, 0, 0);

    DrawStringSdf(alphabet, &sdf_atlas, sdf_atlas_handle, 16, 8, 40, 0, 0, 0);
    DrawStringBmp(alphabet, &bmp_atlas, bmp_atlas_handle, 16, 8, 24, 0, 0, 0);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  return 0;
}
