#ifndef CDEFAULT_IMAGE_H_
#define CDEFAULT_IMAGE_H_

#include "cdefault_io.h"
#include "cdefault_std.h"

// API for loading image files, e.g. off of disk.
// The following file formats are supported:
// - BMP: versions v2 - v5, bpp in (1, 4, 8, 16, 24, 32), uncompressed or bitfield compression.

// TODO: support RLE BMP?
// TODO: better error messages?
// TODO: support other input / file formats?
// TODO: different output formats?
// TODO: SIMD

// NOTE: The format to load the image data in as.
typedef enum ImageFormat ImageFormat;
enum ImageFormat {
  ImageFormat_RGBA,
  ImageFormat_RGB,
};

B32 ImageLoadFile(Arena* arena, U8** image, U32* width, U32* height, ImageFormat format, U8* file_path);
B32 ImageLoad(Arena* arena, U8** image, U32* width, U32* height, ImageFormat format, U8* file_data, U32 image_data_size);
B32 ImageConvert(Arena* arena, ImageFormat out_format, ImageFormat in_format, U8** out_image, U8* image, U32 width, U32 height); // NOTE: Converts from e.g. RGB -> RGBA on a pre-loaded image.

B32 ImageLoadBmp(Arena* arena, U8** image, U32* width, U32* height, ImageFormat format, U8* file_data, U32 image_data_size);

#endif // CDEFAULT_IMAGE_H_

#ifdef CDEFAULT_IMAGE_IMPLEMENTATION
#undef CDEFAULT_IMAGE_IMPLEMENTATION

typedef enum ImageBmpVersion ImageBmpVersion;
enum ImageBmpVersion {
  ImageBmpVersion_WinV2,
  ImageBmpVersion_WinV3,
  ImageBmpVersion_WinV3_Adobe,
  ImageBmpVersion_WinV4,
  ImageBmpVersion_WinV5,
};

typedef enum ImageBmpCompression ImageBmpCompression;
enum ImageBmpCompression {
  ImageBmpCompression_Uncompressed,
  ImageBmpCompression_Bitfields,
};

B32 ImageLoadBmp(Arena* arena, U8** image, U32* width, U32* height, ImageFormat format, U8* file_data, U32 file_data_size) {
  // NOTE: Read shared header
  BinReader r;
  BinReaderInit(&r, file_data, file_data_size);
  if (BinReader8(&r) != 'B' || BinReader8(&r) != 'M') { return false; }
  BinReaderSkip(&r, 8, sizeof(U8));
  U32 palette_offset = 14; // NOTE: +14 for the data we just processed above.

  U32 data_offset = BinReader32LE(&r);

  // NOTE: Read version-specific BMP header
  U32 header_size = BinReader32LE(&r);
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
      return false;
    } break;
  }

  if (version == ImageBmpVersion_WinV2) {
    *width  = BinReader16LE(&r);
    *height = BinReader16LE(&r);
  } else {
    *width  = BinReader32LE(&r);
    *height = BinReader32LE(&r);
  }
  U16 color_plane = BinReader16LE(&r);
  if (color_plane != 1) {
    LOG_ERROR("[IMAGE] Bad BMP file - expected a color plane of 1, got %d.", color_plane);
    return false;
  }
  B8 y_flipped = false;
  if (*height < 0) {
    *height *= -1;
    y_flipped = true;
  }

  U16 bits_per_pixel = BinReader16LE(&r);

  U32 r_mask = 0;
  U32 g_mask = 0;
  U32 b_mask = 0;
  U32 a_mask = 0;
  if (bits_per_pixel == 16) {
    a_mask = 0;
    r_mask = 0x1F << 10;
    g_mask = 0x1F << 5;
    b_mask = 0x1F << 0;
  } else if (bits_per_pixel == 32) {
    a_mask = 0xFF << 24;
    r_mask = 0xFF << 16;
    g_mask = 0xFF << 8;
    b_mask = 0xFF << 0;
  }

  if (version != ImageBmpVersion_WinV2) {
    ImageBmpCompression compression;
    U32 c = BinReader32LE(&r);
    switch (c) {
      case 0: {
        compression = ImageBmpCompression_Uncompressed;
      } break;
      case 1:
      case 2: {
        LOG_ERROR("[IMAGE] BMP RLE compression is not supported: %d", c);
        return false;
      } break;
      case 3: {
        compression = ImageBmpCompression_Bitfields;
      } break;
      default: {
        LOG_ERROR("[IMAGE] Unrecognized BMP compression format: %d", c);
        return false;
      } break;
    }
    BinReaderSkip(&r, 5, sizeof(U32));

    if (version == ImageBmpVersion_WinV3 || version == ImageBmpVersion_WinV3_Adobe) {
      if (version == ImageBmpVersion_WinV3_Adobe) { BinReaderSkip(&r, 4, sizeof(U32)); }
      if ((bits_per_pixel == 16 || bits_per_pixel == 32) && compression == ImageBmpCompression_Bitfields) {
        r_mask = BinReader32LE(&r);
        g_mask = BinReader32LE(&r);
        b_mask = BinReader32LE(&r);
        palette_offset += 12;

        if (r_mask == g_mask && g_mask == b_mask) {
          LOG_ERROR("[IMAGE] Bad BMP bitfields provided.");
          return false;
        }
      }
    } else if (version == ImageBmpVersion_WinV4 || version == ImageBmpVersion_WinV5) {
      if (compression == ImageBmpCompression_Bitfields) {
        r_mask = BinReader32LE(&r);
        g_mask = BinReader32LE(&r);
        b_mask = BinReader32LE(&r);
        a_mask = BinReader32LE(&r);
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
    return false;
  }
  BinReaderSetPos(&r, palette_offset);
  for (S32 i = 0; i < palette_num_entries; i++) {
    palette[i][2] = BinReader8(&r); // G
    palette[i][1] = BinReader8(&r); // B
    palette[i][0] = BinReader8(&r); // R
    // NOTE: Skip extra reserved byte.
    if (palette_bytes_per_entry == 4) { BinReaderSkip(&r, 1, sizeof(U8)); }
  }

  // NOTE: load image into temp buffer first to simplify format logic.
  // ingest as RGBA and convert to the requested format later.
  Arena* temp_arena = ArenaAllocate();
  U8* temp_image = ARENA_PUSH_ARRAY(arena, U8, *width * *height * 4);
  B32 success = false;

  // NOTE: Read color data
  BinReaderSetPos(&r, data_offset);
  switch (bits_per_pixel) {
    case 1: {
      S32 stride  = (*width + 7) >> 3;
      S32 pad     = (-stride) & 3;
      U32 out_idx = 0;
      for (U32 i = 0; i < *height; i++) {
        S8 bit_offset = 7;
        U8 current_byte = BinReader8(&r);
        for (U32 j = 0; j < *width; j++) {
          U8 color = (current_byte >> bit_offset) & 1;
          temp_image[out_idx++] = palette[color][0];
          temp_image[out_idx++] = palette[color][1];
          temp_image[out_idx++] = palette[color][2];
          temp_image[out_idx++] = 255;
          if (j + 1 == *width) { break; }
          bit_offset -= 1;
          if (bit_offset < 0) {
            bit_offset = 7;
            current_byte = BinReader8(&r);
          }
        }
        BinReaderSkip(&r, pad, sizeof(U8));
      }
    } break;

    case 4: {
      S32 stride  = (*width + 1) >> 1;
      S32 pad     = (-stride) & 3;
      U32 out_idx = 0;
      for (U32 i = 0; i < *height; i++) {
        B8 first = true;
        U8 current_byte = BinReader8(&r);;
        for (U32 j = 0; j < *width; j++) {
          U8 color = (current_byte >> (4 * first)) & 0xF;
          temp_image[out_idx++] = palette[color][0];
          temp_image[out_idx++] = palette[color][1];
          temp_image[out_idx++] = palette[color][2];
          temp_image[out_idx++] = 255;
          if (j + 1 == *width) { break; }
          first = !first;
          if (first) { current_byte = BinReader8(&r); }
        }
        BinReaderSkip(&r, pad, sizeof(U8));
      }
    } break;

    case 8: {
      S32 pad = (-((S32)*width)) & 3;
      U32 out_idx = 0;
      for (U32 i = 0; i < *height; i++) {
        for (U32 j = 0; j < *width; j++) {
          U8 color = BinReader8(&r);
          temp_image[out_idx++] = palette[color][0];
          temp_image[out_idx++] = palette[color][1];
          temp_image[out_idx++] = palette[color][2];
          temp_image[out_idx++] = 255;
        }
        BinReaderSkip(&r, pad, sizeof(U8));
      }
    } break;

    case 16: {
      S32 stride = *width * 2;
      S32 pad    = -stride & 3;
      if (r_mask == 0 || g_mask == 0 || b_mask == 0) {
        LOG_ERROR("[IMAGE] Bad color masks detected for BMP image.");
        goto image_load_bmp_exit;
      }
      U32 out_idx = 0;
      for (U32 i = 0; i < *height; i++) {
        for (U32 j = 0; j < *width; j++) {
          U16 color = BinReader16LE(&r);
          U8 red   = (color & r_mask) >> r_shift;
          U8 green = (color & g_mask) >> g_shift;
          U8 blue  = (color & b_mask) >> b_shift;
          // NOTE: Map from e.g. 5 bits to 8 bits.
          temp_image[out_idx++] = red   * (((F32) 0xff) / (r_mask >> r_shift));
          temp_image[out_idx++] = green * (((F32) 0xff) / (g_mask >> g_shift));
          temp_image[out_idx++] = blue  * (((F32) 0xff) / (b_mask >> b_shift));
          temp_image[out_idx++] = 255;
        }
        BinReaderSkip(&r, pad, sizeof(U8));
      }
    } break;

    case 24: {
      S32 stride  = *width * 3;
      S32 pad     = -stride & 3;
      U32 out_idx = 0;
      for (U32 i = 0; i < *height; i++) {
        for (U32 j = 0; j < *width; j++) {
          U8 green = BinReader8(&r);
          U8 blue  = BinReader8(&r);
          U8 red   = BinReader8(&r);
          temp_image[out_idx++] = red;
          temp_image[out_idx++] = blue;
          temp_image[out_idx++] = green;
          temp_image[out_idx++] = 255;
        }
        BinReaderSkip(&r, pad, sizeof(U8));
      }
    } break;

    case 32: {
      S32 stride  = *width * 4;
      S32 pad     = -stride & 3;
      U32 out_idx = 0;
      for (U32 i = 0; i < *height; i++) {
        for (U32 j = 0; j < *width; j++) {
          U32 color = BinReader32LE(&r);
          U32 red   = (color & r_mask) >> r_shift;
          U32 green = (color & g_mask) >> g_shift;
          U32 blue  = (color & b_mask) >> b_shift;
          U32 alpha = (color & a_mask) >> a_shift;
          // NOTE: Map from e.g. 10 bits to 8 bits.
          temp_image[out_idx++] = red   * (((F32) 0xff) / (r_mask >> r_shift));
          temp_image[out_idx++] = green * (((F32) 0xff) / (g_mask >> g_shift));
          temp_image[out_idx++] = blue  * (((F32) 0xff) / (b_mask >> b_shift));
          temp_image[out_idx++] = alpha * (((F32) 0xff) / (a_mask >> a_shift));
        }
        BinReaderSkip(&r, pad, sizeof(U8));
      }
    } break;

    default: {
      LOG_ERROR("[IMAGE] Unsupported bits per pixel identified: %d", bits_per_pixel);
      goto image_load_bmp_exit;
    } break;
  }

  // NOTE: BMP is normally ordered from bottom to top.
  // Flip it by default to order from top to bottom, unless it's already in that format.
  y_flipped = !y_flipped;
  if (y_flipped) {
    for (U32 i = 0; i < *height / 2; i++) {
      U8* a = temp_image + (i * *width * 4);
      U8* b = temp_image + ((*height - 1 - i) * *width * 4);
      for (U32 j = 0; j < *width * 4; j++) { SWAP(U8, a[j], b[j]); }
    }
  }

  DEBUG_ASSERT(ImageConvert(arena, format, ImageFormat_RGBA, image, temp_image, *width, *height));
  success = true;
image_load_bmp_exit:
  ArenaRelease(temp_arena);
  return success;
}

B32 ImageLoadFile(Arena* arena, U8** image, U32* width, U32* height, ImageFormat format, U8* file_path) {
  B32 success = false;
  U8* file_data;
  U32 file_data_size;
  Arena* file_arena = ArenaAllocate();
  if (!FileReadAll(file_arena, file_path, &file_data, &file_data_size)) { goto image_load_file_exit; }
  success = ImageLoad(arena, image, width, height, format, file_data, file_data_size);
image_load_file_exit:
  ArenaRelease(file_arena);
  return success;
}

B32 ImageLoad(Arena* arena, U8** image, U32* width, U32* height, ImageFormat format, U8* file_data, U32 image_data_size) {
  if (ImageLoadBmp(arena, image, width, height, format, file_data, image_data_size)) { return true; }
  return false;
}

B32 ImageConvert(Arena* arena, ImageFormat out_format, ImageFormat in_format, U8** out_image, U8* image, U32 width, U32 height) {
  U32 bytes_per_pixel = 0;
  switch (out_format) {
    case ImageFormat_RGB:  { bytes_per_pixel = 3; } break;
    case ImageFormat_RGBA: { bytes_per_pixel = 4; } break;
    default: {
      LOG_ERROR("[IMAGE] Unrecognized image format provided: %d", out_format);
      return false;
    }
  }

  *out_image = ARENA_PUSH_ARRAY(arena, U8, width * height * bytes_per_pixel);
  for (U32 i = 0; i < height; i++) {
    for (U32 j = 0; j < width; j++) {
      U8 r, g, b, a;
      switch (in_format) {
        case ImageFormat_RGB: {
          r = image[(((i * width) + j) * 3) + 0];
          g = image[(((i * width) + j) * 3) + 1];
          b = image[(((i * width) + j) * 3) + 2];
        } break;
        case ImageFormat_RGBA: {
          r = image[(((i * width) + j) * 4) + 0];
          g = image[(((i * width) + j) * 4) + 1];
          b = image[(((i * width) + j) * 4) + 2];
          a = image[(((i * width) + j) * 4) + 3];
        } break;
      }

      switch (out_format) {
        case ImageFormat_RGB: {
          (*out_image)[(((i * width) + j) * 3) + 0] = r;
          (*out_image)[(((i * width) + j) * 3) + 1] = g;
          (*out_image)[(((i * width) + j) * 3) + 2] = b;
        } break;
        case ImageFormat_RGBA: {
          (*out_image)[(((i * width) + j) * 4) + 0] = r;
          (*out_image)[(((i * width) + j) * 4) + 1] = g;
          (*out_image)[(((i * width) + j) * 4) + 2] = b;
          (*out_image)[(((i * width) + j) * 4) + 3] = a;
        } break;
      }
    }
  }
  return true;
}

#endif // CDEFAULT_IMAGE_IMPLEMENTATION
