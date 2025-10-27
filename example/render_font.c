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

int main(void) {
  DirSetCurrentToExeDir();
  Arena* arena = ArenaAllocate();

  String8 file_data;
  DEBUG_ASSERT(FileReadAll(arena, (U8*) "../data/firacode.ttf", &file_data.str, &file_data.size));

#if 1

  Image font_image;
  DEBUG_ASSERT(FontBakeBitmap(arena, file_data.str, file_data.size, 32, &font_image, 500, 500));

#else

  stbtt_bakedchar cdata[96];
  unsigned char stb_bitmap[512 * 512];
  stbtt_BakeFontBitmap(file_data.str, 0, 32, stb_bitmap, 512, 512, 'a', 'z' - 'a', cdata);

#endif

  return 0;
}
