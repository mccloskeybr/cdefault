#ifndef CDEFAULT_IMAGE_H_
#define CDEFAULT_IMAGE_H_

#include "cdefault_io.h"
#include "cdefault_std.h"

// API for loading image files, e.g. off of disk.
//
// The following file formats are supported:
// -   BMP: versions v2 - v5, bpp in (1, 4, 8, 16, 24, 32), uncompressed or bitfield compression (NOT RLE compression).

// Example
#if 0
Arena arena = ArenaAllocate();
Image image;
DEBUG_ASSERT(ImageLoadFile(arena, &image, ImageFormat_RGBA, "data/my_img.bmp"));
RenderMyBeautifulImage(image.data, image.width, image.height);
ArenaRelease(arena);
#endif

// TODO: better error messages?
// TODO: support other input / file formats?
// TODO: different output formats?
// TODO: SIMD

// NOTE: Uncompressed image data format.
typedef enum ImageFormat ImageFormat;
enum ImageFormat {
  ImageFormat_RGBA,
  ImageFormat_RGB,
  ImageFormat_R,
};

typedef struct Image Image;
struct Image {
  ImageFormat format;
  U32 width;
  U32 height;
  // NOTE: data is the raw pixel color data, `format` describes the layout.
  // this starts at the bottom left of the frame and increases up and to the right.
  U8* data;
};

B32  ImageLoadFile(Arena* arena, Image* image, ImageFormat format, String8 file_path);
B32  ImageLoad(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 image_data_size);
void ImageConvert(Arena* arena, Image* to, Image* from, ImageFormat to_format);
B32  ImageDumpBmp(Image* image, String8 file_path); // NOTE: Preserves alpha channel, if present.

B32  ImageLoadBmp(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 image_data_size);

#endif // CDEFAULT_IMAGE_H_

#ifdef CDEFAULT_IMAGE_IMPLEMENTATION
#undef CDEFAULT_IMAGE_IMPLEMENTATION

#define IMAGE_LOG_OUT_OF_CHARS() LOG_ERROR("[FONT] Ran out of characters in image file.")

typedef enum ImageBmpVersion ImageBmpVersion;
enum ImageBmpVersion {
  ImageBmpVersion_WinV2,
  ImageBmpVersion_WinV3,
  ImageBmpVersion_WinV3_Adobe,
  ImageBmpVersion_WinV4,
  ImageBmpVersion_WinV5,
};

// https://en.wikipedia.org/wiki/BMP_file_format
// https://www.fileformat.info/format/bmp/egff.htm
#define IMAGE_TRY_PARSE(eval) if (!eval) { IMAGE_LOG_OUT_OF_CHARS(); goto image_load_bmp_exit; }
B32 ImageLoadBmp(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 file_data_size) {
  B32 success = false;
  Arena* temp_arena = ArenaAllocate();

  // NOTE: Read shared header
  BinStream h;
  BinStreamInit(&h, file_data, file_data_size);
  String8 tag;
  IMAGE_TRY_PARSE(BinStreamPullStr8(&h, 2, &tag));
  if (!Str8Eq(tag, Str8Lit("BM"))) { goto image_load_bmp_exit; }
  IMAGE_TRY_PARSE(BinStreamSkip(&h, 8, sizeof(U8)));
  U32 palette_offset = 14; // NOTE: +14 for the data we just processed above.

  // NOTE: Simplify loading by assuming RGBA, then converting to the desired step separately.
  Image temp_image;
  temp_image.format = ImageFormat_RGBA;

  U32 data_offset, header_size;
  IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &data_offset));

  // NOTE: Read version-specific BMP header
  IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &header_size));
  palette_offset += header_size;
  ImageBmpVersion version;
  switch (header_size) {
    case 12:  { version = ImageBmpVersion_WinV2;       } break;
    case 40:  { version = ImageBmpVersion_WinV3;       } break;
    case 56:  { version = ImageBmpVersion_WinV3_Adobe; } break;
    case 108: { version = ImageBmpVersion_WinV4;       } break;
    case 124: { version = ImageBmpVersion_WinV5;       } break;
    default: {
      LOG_ERROR("[IMAGE] Unexpected BMP header size, not supported: %d", header_size);
      goto image_load_bmp_exit;
    } break;
  }

  if (version == ImageBmpVersion_WinV2) {
    IMAGE_TRY_PARSE(BinStreamPullU16LE(&h, (U16*) &temp_image.width));
    IMAGE_TRY_PARSE(BinStreamPullU16LE(&h, (U16*) &temp_image.height));
  } else {
    IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &temp_image.width));
    IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &temp_image.height));
  }
  U16 color_plane;
  IMAGE_TRY_PARSE(BinStreamPullU16LE(&h, &color_plane));
  if (color_plane != 1) {
    LOG_ERROR("[IMAGE] Bad BMP file - expected a color plane of 1, got %d.", color_plane);
    goto image_load_bmp_exit;
  }
  // NOTE: BMP is normally ordered bottom to top.
  B8 y_flipped = false;
  if (temp_image.height < 0) {
    temp_image.height *= -1;
    y_flipped = true;
  }

  U16 bits_per_pixel;
  IMAGE_TRY_PARSE(BinStreamPullU16LE(&h, &bits_per_pixel));

  U32 r_mask = 0;
  U32 g_mask = 0;
  U32 b_mask = 0;
  U32 a_mask = 0;
  if (bits_per_pixel == 16) {
    a_mask = 0;
    r_mask = 0x1f << 10;
    g_mask = 0x1f << 5;
    b_mask = 0x1f << 0;
  } else if (bits_per_pixel == 32) {
    a_mask = 0xff << 24;
    r_mask = 0xff << 16;
    g_mask = 0xff << 8;
    b_mask = 0xff << 0;
  }

  if (version != ImageBmpVersion_WinV2) {
    U32 compression;
    IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &compression));
    // 0    -> uncompressed
    // 1, 2 -> RLE
    // 3    -> bitfields
    if (compression != 0 && compression != 3) {
      LOG_ERROR("[IMAGE] BMP unknown or unsupported compression format detected: %d", compression);
      goto image_load_bmp_exit;
    }
    IMAGE_TRY_PARSE(BinStreamSkip(&h, 5, sizeof(U32)));

    if (version == ImageBmpVersion_WinV3 || version == ImageBmpVersion_WinV3_Adobe) {
      if (version == ImageBmpVersion_WinV3_Adobe) { IMAGE_TRY_PARSE(BinStreamSkip(&h, 4, sizeof(U32))); }
      if ((bits_per_pixel == 16 || bits_per_pixel == 32) && compression == 3) {
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &r_mask));
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &g_mask));
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &b_mask));
        palette_offset += 12;

        if (r_mask == g_mask && g_mask == b_mask) {
          LOG_ERROR("[IMAGE] Bad BMP bitfields provided.");
          goto image_load_bmp_exit;
        }
      }
    } else if (version == ImageBmpVersion_WinV4 || version == ImageBmpVersion_WinV5) {
      if (compression == 3) {
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &r_mask));
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &g_mask));
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &b_mask));
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &a_mask));
      }
    }
  }
  U32 r_shift = U32LsbPos(r_mask);
  U32 g_shift = U32LsbPos(g_mask);
  U32 b_shift = U32LsbPos(b_mask);
  U32 a_shift = U32LsbPos(a_mask);

  // NOTE: Read (optional) palette data
  U8  palette[256][3];
  S32 palette_byte_size       = data_offset - palette_offset;
  S32 palette_bytes_per_entry = 4;
  if (version == ImageBmpVersion_WinV2) { palette_bytes_per_entry = 3; }
  S32 palette_num_entries     = palette_byte_size / palette_bytes_per_entry;
  if (palette_num_entries > 256) {
    LOG_ERROR("[IMAGE] Unexpected palette size for BMP, max is 256, observed: %d", palette_num_entries);
    goto image_load_bmp_exit;
  }
  IMAGE_TRY_PARSE(BinStreamSeek(&h, palette_offset));
  for (S32 i = 0; i < palette_num_entries; i++) {
    IMAGE_TRY_PARSE(BinStreamPullU8(&h, &palette[i][2])); // G
    IMAGE_TRY_PARSE(BinStreamPullU8(&h, &palette[i][1])); // B
    IMAGE_TRY_PARSE(BinStreamPullU8(&h, &palette[i][0])); // R
    // NOTE: Skip extra reserved byte.
    if (palette_bytes_per_entry == 4) { IMAGE_TRY_PARSE(BinStreamSkip(&h, 1, sizeof(U8))); }
  }

  // NOTE: load image into temp buffer first to simplify format logic.
  // ingest as RGBA and convert to the requested format later.
  temp_image.data = ARENA_PUSH_ARRAY(arena, U8, temp_image.width * temp_image.height * 4);

  // NOTE: Read color data
  U32 a_zero = 0;
  IMAGE_TRY_PARSE(BinStreamSeek(&h, data_offset));
  switch (bits_per_pixel) {
    case 1: {
      S32 stride  = (temp_image.width + 7) >> 3;
      S32 pad     = (-stride) & 3;
      U32 out_idx = 0;
      a_zero      = 0xff;
      for (U32 i = 0; i < temp_image.height; i++) {
        S8 bit_offset = 7;
        U8 current_byte;
        IMAGE_TRY_PARSE(BinStreamPullU8(&h, &current_byte));
        for (U32 j = 0; j < temp_image.width; j++) {
          U8 color = (current_byte >> bit_offset) & 1;
          temp_image.data[out_idx++] = palette[color][0];
          temp_image.data[out_idx++] = palette[color][1];
          temp_image.data[out_idx++] = palette[color][2];
          temp_image.data[out_idx++] = 255;
          if (j + 1 == temp_image.width) { break; }
          bit_offset -= 1;
          if (bit_offset < 0) {
            bit_offset = 7;
            IMAGE_TRY_PARSE(BinStreamPullU8(&h, &current_byte));
          }
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&h, pad, sizeof(U8)));
      }
    } break;

    case 4: {
      S32 stride  = (temp_image.width + 1) >> 1;
      S32 pad     = (-stride) & 3;
      U32 out_idx = 0;
      a_zero      = 0xff;
      for (U32 i = 0; i < temp_image.height; i++) {
        B8 first = true;
        U8 current_byte;
        IMAGE_TRY_PARSE(BinStreamPullU8(&h, &current_byte));
        for (U32 j = 0; j < temp_image.width; j++) {
          U8 color = (current_byte >> (4 * first)) & 0xf;
          temp_image.data[out_idx++] = palette[color][0];
          temp_image.data[out_idx++] = palette[color][1];
          temp_image.data[out_idx++] = palette[color][2];
          temp_image.data[out_idx++] = 255;
          if (j + 1 == temp_image.width) { break; }
          first = !first;
          if (first) { IMAGE_TRY_PARSE(BinStreamPullU8(&h, &current_byte)); }
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&h, pad, sizeof(U8)));
      }
    } break;

    case 8: {
      S32 pad     = (-((S32)temp_image.width)) & 3;
      U32 out_idx = 0;
      a_zero      = 0xff;
      for (U32 i = 0; i < temp_image.height; i++) {
        for (U32 j = 0; j < temp_image.width; j++) {
          U8 color;
          IMAGE_TRY_PARSE(BinStreamPullU8(&h, &color));
          temp_image.data[out_idx++] = palette[color][0];
          temp_image.data[out_idx++] = palette[color][1];
          temp_image.data[out_idx++] = palette[color][2];
          temp_image.data[out_idx++] = 255;
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&h, pad, sizeof(U8)));
      }
    } break;

    case 16: {
      S32 stride = temp_image.width * 2;
      S32 pad    = -stride & 3;
      if (r_mask == 0 || g_mask == 0 || b_mask == 0) {
        LOG_ERROR("[IMAGE] Bad color masks detected for BMP image.");
        goto image_load_bmp_exit;
      }
      U32 out_idx = 0;
      for (U32 i = 0; i < temp_image.height; i++) {
        for (U32 j = 0; j < temp_image.width; j++) {
          U16 color;
          IMAGE_TRY_PARSE(BinStreamPullU16LE(&h, &color));
          // NOTE: grab color data using mask, shift into place, and remap to 8 bits by mult. by ratio of 8 bit max to mask max.
          // TODO: is there a way to do this without divides? does it matter?
          temp_image.data[out_idx++] = ((color & r_mask) >> r_shift) * (((F32) 0xff) / (r_mask >> r_shift));
          temp_image.data[out_idx++] = ((color & g_mask) >> g_shift) * (((F32) 0xff) / (g_mask >> g_shift));
          temp_image.data[out_idx++] = ((color & b_mask) >> b_shift) * (((F32) 0xff) / (b_mask >> b_shift));
          temp_image.data[out_idx++] = ((color & a_mask) >> a_shift) * (((F32) 0xff) / (a_mask >> a_shift));
          a_zero |= temp_image.data[out_idx - 1];
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&h, pad, sizeof(U8)));
      }
    } break;

    case 24: {
      S32 stride  = temp_image.width * 3;
      S32 pad     = -stride & 3;
      U32 out_idx = 0;
      a_zero      = 0xff;
      for (U32 i = 0; i < temp_image.height; i++) {
        for (U32 j = 0; j < temp_image.width; j++) {
          U8 green, blue, red;
          IMAGE_TRY_PARSE(BinStreamPullU8(&h, &green));
          IMAGE_TRY_PARSE(BinStreamPullU8(&h, &blue));
          IMAGE_TRY_PARSE(BinStreamPullU8(&h, &red));
          temp_image.data[out_idx++] = red;
          temp_image.data[out_idx++] = blue;
          temp_image.data[out_idx++] = green;
          temp_image.data[out_idx++] = 255;
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&h, pad, sizeof(U8)));
      }
    } break;

    case 32: {
      S32 stride  = temp_image.width * 4;
      S32 pad     = -stride & 3;
      if (r_mask == 0 || g_mask == 0 || b_mask == 0) {
        LOG_ERROR("[IMAGE] Bad color masks detected for BMP image.");
        goto image_load_bmp_exit;
      }
      U32 out_idx = 0;
      for (U32 i = 0; i < temp_image.height; i++) {
        for (U32 j = 0; j < temp_image.width; j++) {
          U32 color;
          IMAGE_TRY_PARSE(BinStreamPullU32LE(&h, &color));
          // NOTE: grab color data using mask, shift into place, and remap to 8 bits by mult. by ratio of 8 bit max to mask max.
          temp_image.data[out_idx++] = ((color & r_mask) >> r_shift) * (((F32) 0xff) / (r_mask >> r_shift));
          temp_image.data[out_idx++] = ((color & g_mask) >> g_shift) * (((F32) 0xff) / (g_mask >> g_shift));
          temp_image.data[out_idx++] = ((color & b_mask) >> b_shift) * (((F32) 0xff) / (b_mask >> b_shift));
          temp_image.data[out_idx++] = ((color & a_mask) >> a_shift) * (((F32) 0xff) / (a_mask >> a_shift));
          a_zero |= temp_image.data[out_idx - 1];
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&h, pad, sizeof(U8)));
      }
    } break;

    default: {
      LOG_ERROR("[IMAGE] Unsupported bits per pixel identified: %d", bits_per_pixel);
      goto image_load_bmp_exit;
    } break;
  }

  // NOTE: If there wasn't any alpha, go back and make everything opaque.
  if (a_zero == 0) {
    for (U32 i = 3; i < image->width * image->height * 4; i += 4) {
      image->data[i] = 255;
    }
  }

  if (y_flipped) {
    for (U32 i = 0; i < temp_image.height / 2; i++) {
      U8* a = temp_image.data + (i * temp_image.width * 4);
      U8* b = temp_image.data + ((temp_image.height - 1 - i) * temp_image.width * 4);
      for (U32 j = 0; j < temp_image.width * 4; j++) { SWAP(U8, a[j], b[j]); }
    }
  }
  ImageConvert(arena, image, &temp_image, format);
  success = true;
image_load_bmp_exit:
  ArenaRelease(temp_arena);
  return success;
}
#undef IMAGE_TRY_PARSE

B32 ImageLoadFile(Arena* arena, Image* image, ImageFormat format, String8 file_path) {
  B32 success = false;
  String8 file_data;
  Arena* file_arena = ArenaAllocate();
  if (!FileReadAll(file_arena, file_path, &file_data.str, &file_data.size)) { goto image_load_file_exit; }
  success = ImageLoad(arena, image, format, file_data.str, file_data.size);
image_load_file_exit:
  ArenaRelease(file_arena);
  return success;
}

B32 ImageLoad(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 image_data_size) {
  if (ImageLoadBmp(arena, image, format, file_data, image_data_size)) { return true; }
  return false;
}

void ImageConvert(Arena* arena, Image* to, Image* from, ImageFormat to_format) {
  to->format = to_format;
  to->width = from->width;
  to->height = from->height;

  U32 bytes_per_pixel = 0;
  switch (to->format) {
    case ImageFormat_RGBA: { bytes_per_pixel = 4; } break;
    case ImageFormat_RGB:  { bytes_per_pixel = 3; } break;
    case ImageFormat_R:    { bytes_per_pixel = 1; } break;
    default: { UNREACHABLE(); }
  }

  to->data = ARENA_PUSH_ARRAY(arena, U8, to->width * to->height * bytes_per_pixel);
  if (to->format == from->format) {
    MEMORY_COPY(to->data, from->data, to->width * to->height * bytes_per_pixel);
    return;
  }
  for (U32 i = 0; i < to->height; i++) {
    for (U32 j = 0; j < to->width; j++) {
      U8 r, g, b, a;
      switch (from->format) {
        case ImageFormat_RGBA: {
          r = from->data[(((i * to->width) + j) * 4) + 0];
          g = from->data[(((i * to->width) + j) * 4) + 1];
          b = from->data[(((i * to->width) + j) * 4) + 2];
          a = from->data[(((i * to->width) + j) * 4) + 3];
        } break;
        case ImageFormat_RGB: {
          r = from->data[(((i * to->width) + j) * 3) + 0];
          g = from->data[(((i * to->width) + j) * 3) + 1];
          b = from->data[(((i * to->width) + j) * 3) + 2];
          a = 255;
        } break;
        case ImageFormat_R: {
          r = from->data[(((i * to->width) + j) * 1) + 0];
          g = r;
          b = r;
          a = 255;
        } break;
      }

      switch (to->format) {
        case ImageFormat_RGBA: {
          to->data[(((i * to->width) + j) * 4) + 0] = r;
          to->data[(((i * to->width) + j) * 4) + 1] = g;
          to->data[(((i * to->width) + j) * 4) + 2] = b;
          to->data[(((i * to->width) + j) * 4) + 3] = a;
        } break;
        case ImageFormat_RGB: {
          to->data[(((i * to->width) + j) * 3) + 0] = r;
          to->data[(((i * to->width) + j) * 3) + 1] = g;
          to->data[(((i * to->width) + j) * 3) + 2] = b;
        } break;
        case ImageFormat_R: {
          to->data[(((i * to->width) + j) * 1) + 0] = (r + g + b) / 3;
        } break;
      }
    }
  }
}

B32 ImageDumpBmp(Image* image, String8 file_path) {
  // NOTE: convert to RGBA for convenience.
  Arena* temp_arena = ArenaAllocate();
  Image temp_image;
  ImageConvert(temp_arena, &temp_image, image, ImageFormat_RGBA);

  U32 file_header_size = 14;
  U32 header_size = 108; // v4 header size
  U32 bmp_offset = file_header_size + header_size;
  U32 bmp_size = temp_image.width * temp_image.height * 4;
  U32 bmp_file_size = bmp_offset + bmp_size;

  U8* bmp = ARENA_PUSH_ARRAY(temp_arena, U8, bmp_file_size);
  BinStream h;
  BinStreamInit(&h, bmp, bmp_file_size);

  // NOTE: file header
  DEBUG_ASSERT(BinStreamPushStr8(&h, Str8Lit("BM")));
  DEBUG_ASSERT(BinStreamPush32LE(&h, bmp_file_size)); // file size
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // reserved 1 & 2
  DEBUG_ASSERT(BinStreamPush32LE(&h, bmp_offset));

  // NOTE: winv4 header
  DEBUG_ASSERT(BinStreamPush32LE(&h, header_size));
  DEBUG_ASSERT(BinStreamPush32LE(&h, temp_image.width));
  // NOTE: internal raw format matches BMP bottom->up, so don't flip.
  DEBUG_ASSERT(BinStreamPush32LE(&h, (S32) temp_image.height));
  DEBUG_ASSERT(BinStreamPush16LE(&h, 1));  // color planes
  DEBUG_ASSERT(BinStreamPush16LE(&h, 32)); // bits per pixel
  DEBUG_ASSERT(BinStreamPush32LE(&h, 3));  // bitfield compression
  DEBUG_ASSERT(BinStreamPush32LE(&h, bmp_size)); // size of bitmap
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // horizontal resolution
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // vertical resolution
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // colors used
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // colors important
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0x00ff0000)); // red mask
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0x0000ff00)); // green mask
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0x000000ff)); // blue mask
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0xff000000)); // alpha mask
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // cs type
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // redx
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // redy
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // redz
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // greenx
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // greeny
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // greenz
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // bluex
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // bluey
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // bluez
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // gamma red
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // gamma green
  DEBUG_ASSERT(BinStreamPush32LE(&h, 0)); // gamma blue
  for (U32 i = 0; i < temp_image.height; i++) {
    for (U32 j = 0; j < temp_image.width; j++) {
      U32 r = temp_image.data[(((i * temp_image.width) + j) * 4) + 0];
      U32 g = temp_image.data[(((i * temp_image.width) + j) * 4) + 1];
      U32 b = temp_image.data[(((i * temp_image.width) + j) * 4) + 2];
      U32 a = temp_image.data[(((i * temp_image.width) + j) * 4) + 3];
      U32 color = 0;
      color |= a << 24;
      color |= r << 16;
      color |= g << 8;
      color |= b << 0;
      DEBUG_ASSERT(BinStreamPush32LE(&h, color));
    }
  }

  B32 success = FileDump(file_path, bmp, bmp_file_size);
  ArenaRelease(temp_arena);
  return success;
}

#undef IMAGE_LOG_OUT_OF_CHARS

#endif // CDEFAULT_IMAGE_IMPLEMENTATION
