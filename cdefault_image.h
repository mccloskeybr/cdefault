#ifndef CDEFAULT_IMAGE_H_
#define CDEFAULT_IMAGE_H_

#include "cdefault_io.h"
#include "cdefault_std.h"

// API for loading image files, e.g. off of disk.
//
// The following file formats are supported:
// -   BMP: versions v2 - v5, bpp in (1, 4, 8, 16, 24, 32), uncompressed or bitfield compression (NOT RLE compression).
//
// E.g.
// Arena arena = ArenaAllocate();
// Image image;
// DEBUG_ASSERT(ImageLoadFile(arena, &image, ImageFormat_RGBA, "data/my_img.bmp"));
// RenderMyBeautifulImage(image.data, image.width, image.height);
// ArenaRelease(arena);

// TODO: support RLE BMP?
// TODO: better error messages?
// TODO: support other input / file formats?
// TODO: different output formats?
// TODO: SIMD
// TODO: if end format is RGBA, don't require a separate copy (or, don't require format conversion altogether? depends on list of reasonable output formats)
// TODO: write BMPs

// NOTE: Uncompressed image data format.
typedef enum ImageFormat ImageFormat;
enum ImageFormat {
  ImageFormat_RGBA,
  ImageFormat_RGB,
};

typedef struct Image Image;
struct Image {
  ImageFormat format;
  U32 width;
  U32 height;
  U8* data;
};

B32  ImageLoadFile(Arena* arena, Image* image, ImageFormat format, U8* file_path);
B32  ImageLoad(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 image_data_size);
void ImageConvert(Arena* arena, Image* to, Image* from, ImageFormat to_format);

B32  ImageLoadBmp(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 image_data_size);

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

// https://en.wikipedia.org/wiki/BMP_file_format
// https://www.fileformat.info/format/bmp/egff.htm
B32 ImageLoadBmp(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 file_data_size) {
  // NOTE: Read shared header
  BinReader r;
  BinReaderInit(&r, file_data, file_data_size);
  if (BinReader8(&r) != 'B' || BinReader8(&r) != 'M') { return false; }
  BinReaderSkip(&r, 8, sizeof(U8));
  U32 palette_offset = 14; // NOTE: +14 for the data we just processed above.

  // NOTE: Simplify loading by assuming RGBA, then converting to the desired step separately.
  Image temp_image;
  temp_image.format = ImageFormat_RGBA;

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
    temp_image.width  = BinReader16LE(&r);
    temp_image.height = BinReader16LE(&r);
  } else {
    temp_image.width  = BinReader32LE(&r);
    temp_image.height = BinReader32LE(&r);
  }
  U16 color_plane = BinReader16LE(&r);
  if (color_plane != 1) {
    LOG_ERROR("[IMAGE] Bad BMP file - expected a color plane of 1, got %d.", color_plane);
    return false;
  }
  B8 y_flipped = false;
  if (temp_image.height < 0) {
    temp_image.height *= -1;
    y_flipped = true;
  }

  U16 bits_per_pixel = BinReader16LE(&r);

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
    U32 compression = BinReader32LE(&r);
    // 0    -> uncompressed
    // 1, 2 -> RLE
    // 3    -> bitfields
    if (compression != 0 && compression != 3) {
      LOG_ERROR("[IMAGE] BMP unknown or unsupported compression format detected: %d", compression);
      return false;
    }
    BinReaderSkip(&r, 5, sizeof(U32));

    if (version == ImageBmpVersion_WinV3 || version == ImageBmpVersion_WinV3_Adobe) {
      if (version == ImageBmpVersion_WinV3_Adobe) { BinReaderSkip(&r, 4, sizeof(U32)); }
      if ((bits_per_pixel == 16 || bits_per_pixel == 32) && compression == 3) {
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
      if (compression == 3) {
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
  temp_image.data = ARENA_PUSH_ARRAY(arena, U8, temp_image.width * temp_image.height * 4);
  B32 success = false;

  // NOTE: Read color data
  U32 a_zero = 0;
  BinReaderSetPos(&r, data_offset);
  switch (bits_per_pixel) {
    case 1: {
      S32 stride  = (temp_image.width + 7) >> 3;
      S32 pad     = (-stride) & 3;
      U32 out_idx = 0;
      a_zero      = 0xff;
      for (U32 i = 0; i < temp_image.height; i++) {
        S8 bit_offset = 7;
        U8 current_byte = BinReader8(&r);
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
            current_byte = BinReader8(&r);
          }
        }
        BinReaderSkip(&r, pad, sizeof(U8));
      }
    } break;

    case 4: {
      S32 stride  = (temp_image.width + 1) >> 1;
      S32 pad     = (-stride) & 3;
      U32 out_idx = 0;
      a_zero      = 0xff;
      for (U32 i = 0; i < temp_image.height; i++) {
        B8 first = true;
        U8 current_byte = BinReader8(&r);;
        for (U32 j = 0; j < temp_image.width; j++) {
          U8 color = (current_byte >> (4 * first)) & 0xf;
          temp_image.data[out_idx++] = palette[color][0];
          temp_image.data[out_idx++] = palette[color][1];
          temp_image.data[out_idx++] = palette[color][2];
          temp_image.data[out_idx++] = 255;
          if (j + 1 == temp_image.width) { break; }
          first = !first;
          if (first) { current_byte = BinReader8(&r); }
        }
        BinReaderSkip(&r, pad, sizeof(U8));
      }
    } break;

    case 8: {
      S32 pad     = (-((S32)temp_image.width)) & 3;
      U32 out_idx = 0;
      a_zero      = 0xff;
      for (U32 i = 0; i < temp_image.height; i++) {
        for (U32 j = 0; j < temp_image.width; j++) {
          U8 color = BinReader8(&r);
          temp_image.data[out_idx++] = palette[color][0];
          temp_image.data[out_idx++] = palette[color][1];
          temp_image.data[out_idx++] = palette[color][2];
          temp_image.data[out_idx++] = 255;
        }
        BinReaderSkip(&r, pad, sizeof(U8));
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
          U16 color = BinReader16LE(&r);
          // NOTE: grab color data using mask, shift into place, and remap to 8 bits by mult. by ratio of 8 bit max to mask max.
          // TODO: is there a way to do this without divides? does it matter?
          temp_image.data[out_idx++] = ((color & r_mask) >> r_shift) * (((F32) 0xff) / (r_mask >> r_shift));
          temp_image.data[out_idx++] = ((color & g_mask) >> g_shift) * (((F32) 0xff) / (g_mask >> g_shift));
          temp_image.data[out_idx++] = ((color & b_mask) >> b_shift) * (((F32) 0xff) / (b_mask >> b_shift));
          temp_image.data[out_idx++] = ((color & a_mask) >> a_shift) * (((F32) 0xff) / (a_mask >> a_shift));
          a_zero |= temp_image.data[out_idx - 1];
        }
        BinReaderSkip(&r, pad, sizeof(U8));
      }
    } break;

    case 24: {
      S32 stride  = temp_image.width * 3;
      S32 pad     = -stride & 3;
      U32 out_idx = 0;
      a_zero      = 0xff;
      for (U32 i = 0; i < temp_image.height; i++) {
        for (U32 j = 0; j < temp_image.width; j++) {
          U8 green = BinReader8(&r);
          U8 blue  = BinReader8(&r);
          U8 red   = BinReader8(&r);
          temp_image.data[out_idx++] = red;
          temp_image.data[out_idx++] = blue;
          temp_image.data[out_idx++] = green;
          temp_image.data[out_idx++] = 255;
        }
        BinReaderSkip(&r, pad, sizeof(U8));
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
          U32 color = BinReader32LE(&r);
          // NOTE: grab color data using mask, shift into place, and remap to 8 bits by mult. by ratio of 8 bit max to mask max.
          temp_image.data[out_idx++] = ((color & r_mask) >> r_shift) * (((F32) 0xff) / (r_mask >> r_shift));
          temp_image.data[out_idx++] = ((color & g_mask) >> g_shift) * (((F32) 0xff) / (g_mask >> g_shift));
          temp_image.data[out_idx++] = ((color & b_mask) >> b_shift) * (((F32) 0xff) / (b_mask >> b_shift));
          temp_image.data[out_idx++] = ((color & a_mask) >> a_shift) * (((F32) 0xff) / (a_mask >> a_shift));
          a_zero |= temp_image.data[out_idx - 1];
        }
        BinReaderSkip(&r, pad, sizeof(U8));
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

  // NOTE: BMP is normally ordered from bottom to top. Flip it by default to order from top to bottom.
  y_flipped = !y_flipped;
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

B32 ImageLoadFile(Arena* arena, Image* image, ImageFormat format, U8* file_path) {
  B32 success = false;
  U8* file_data;
  U32 file_data_size;
  Arena* file_arena = ArenaAllocate();
  if (!FileReadAll(file_arena, file_path, &file_data, &file_data_size)) { goto image_load_file_exit; }
  success = ImageLoad(arena, image, format, file_data, file_data_size);
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
    case ImageFormat_RGB:  { bytes_per_pixel = 3; } break;
    case ImageFormat_RGBA: { bytes_per_pixel = 4; } break;
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
        case ImageFormat_RGB: {
          r = from->data[(((i * to->width) + j) * 3) + 0];
          g = from->data[(((i * to->width) + j) * 3) + 1];
          b = from->data[(((i * to->width) + j) * 3) + 2];
          a = 255;
        } break;
        case ImageFormat_RGBA: {
          r = from->data[(((i * to->width) + j) * 4) + 0];
          g = from->data[(((i * to->width) + j) * 4) + 1];
          b = from->data[(((i * to->width) + j) * 4) + 2];
          a = from->data[(((i * to->width) + j) * 4) + 3];
        } break;
      }

      switch (to->format) {
        case ImageFormat_RGB: {
          to->data[(((i * to->width) + j) * 3) + 0] = r;
          to->data[(((i * to->width) + j) * 3) + 1] = g;
          to->data[(((i * to->width) + j) * 3) + 2] = b;
        } break;
        case ImageFormat_RGBA: {
          to->data[(((i * to->width) + j) * 4) + 0] = r;
          to->data[(((i * to->width) + j) * 4) + 1] = g;
          to->data[(((i * to->width) + j) * 4) + 2] = b;
          to->data[(((i * to->width) + j) * 4) + 3] = a;
        } break;
      }
    }
  }
}

#endif // CDEFAULT_IMAGE_IMPLEMENTATION
