#ifndef CDEFAULT_IMAGE_H_
#define CDEFAULT_IMAGE_H_

#include "cdefault_io.h"
#include "cdefault_std.h"

// API for loading image files, e.g. off of disk.
//
// The following file formats are supported:
// -   BMP: versions v2 - v5, bpp in (1, 4, 8, 16, 24, 32), uncompressed or bitfield compression (NOT RLE compression).
// -   PNG: 8BPP / color type 6, NOT interlace

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
// TODO: wider PNG support

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
B32  ImageLoad(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 file_data_size);
void ImageConvert(Arena* arena, Image* to, Image* from, ImageFormat to_format);
B32  ImageDumpBmp(Image* image, String8 file_path); // NOTE: Preserves alpha channel, if present.

U32  ImageBytesPerPixel(ImageFormat format);
void ImageFlipY(Image* image);

B32  ImageLoadBmp(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 file_data_size);
B32  ImageLoadPng(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 file_data_size);

#endif // CDEFAULT_IMAGE_H_

#ifdef CDEFAULT_IMAGE_IMPLEMENTATION
#undef CDEFAULT_IMAGE_IMPLEMENTATION

#define IMAGE_LOG_OUT_OF_CHARS() LOG_ERROR("[FONT] Ran out of characters in image file.")

typedef enum BmpVersion BmpVersion;
enum BmpVersion {
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
  BinStream s = BinStreamAssign(file_data, file_data_size);
  String8 tag;
  IMAGE_TRY_PARSE(BinStreamPullStr8(&s, 2, &tag));
  if (!Str8Eq(tag, Str8Lit("BM"))) { goto image_load_bmp_exit; }
  IMAGE_TRY_PARSE(BinStreamSkip(&s, 8, sizeof(U8)));
  U32 palette_offset = 14; // NOTE: +14 for the data we just processed above.

  // NOTE: Simplify loading by assuming RGBA, then converting to the desired step separately.
  Image temp_image;
  temp_image.format = ImageFormat_RGBA;

  U32 data_offset, header_size;
  IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &data_offset));

  // NOTE: Read version-specific BMP header
  IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &header_size));
  palette_offset += header_size;
  BmpVersion version;
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
    IMAGE_TRY_PARSE(BinStreamPullU16LE(&s, (U16*) &temp_image.width));
    IMAGE_TRY_PARSE(BinStreamPullU16LE(&s, (U16*) &temp_image.height));
  } else {
    IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &temp_image.width));
    IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &temp_image.height));
  }
  U16 color_plane;
  IMAGE_TRY_PARSE(BinStreamPullU16LE(&s, &color_plane));
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
  IMAGE_TRY_PARSE(BinStreamPullU16LE(&s, &bits_per_pixel));

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
    IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &compression));
    // 0    -> uncompressed
    // 1, 2 -> RLE
    // 3    -> bitfields
    if (compression != 0 && compression != 3) {
      LOG_ERROR("[IMAGE] BMP unknown or unsupported compression format detected: %d", compression);
      goto image_load_bmp_exit;
    }
    IMAGE_TRY_PARSE(BinStreamSkip(&s, 5, sizeof(U32)));

    if (version == ImageBmpVersion_WinV3 || version == ImageBmpVersion_WinV3_Adobe) {
      if (version == ImageBmpVersion_WinV3_Adobe) { IMAGE_TRY_PARSE(BinStreamSkip(&s, 4, sizeof(U32))); }
      if ((bits_per_pixel == 16 || bits_per_pixel == 32) && compression == 3) {
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &r_mask));
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &g_mask));
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &b_mask));
        palette_offset += 12;

        if (r_mask == g_mask && g_mask == b_mask) {
          LOG_ERROR("[IMAGE] Bad BMP bitfields provided.");
          goto image_load_bmp_exit;
        }
      }
    } else if (version == ImageBmpVersion_WinV4 || version == ImageBmpVersion_WinV5) {
      if (compression == 3) {
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &r_mask));
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &g_mask));
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &b_mask));
        IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &a_mask));
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
  IMAGE_TRY_PARSE(BinStreamSeek(&s, palette_offset));
  for (S32 i = 0; i < palette_num_entries; i++) {
    IMAGE_TRY_PARSE(BinStreamPullU8(&s, &palette[i][2])); // G
    IMAGE_TRY_PARSE(BinStreamPullU8(&s, &palette[i][1])); // B
    IMAGE_TRY_PARSE(BinStreamPullU8(&s, &palette[i][0])); // R
    // NOTE: Skip extra reserved byte.
    if (palette_bytes_per_entry == 4) { IMAGE_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U8))); }
  }

  // NOTE: load image into temp buffer first to simplify format logic.
  // ingest as RGBA and convert to the requested format later.
  temp_image.data = ARENA_PUSH_ARRAY(arena, U8, temp_image.width * temp_image.height * 4);

  // NOTE: Read color data
  U32 a_zero = 0;
  IMAGE_TRY_PARSE(BinStreamSeek(&s, data_offset));
  switch (bits_per_pixel) {
    case 1: {
      S32 stride  = (temp_image.width + 7) >> 3;
      S32 pad     = (-stride) & 3;
      U32 out_idx = 0;
      a_zero      = 0xff;
      for (U32 i = 0; i < temp_image.height; i++) {
        S8 bit_offset = 7;
        U8 current_byte;
        IMAGE_TRY_PARSE(BinStreamPullU8(&s, &current_byte));
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
            IMAGE_TRY_PARSE(BinStreamPullU8(&s, &current_byte));
          }
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&s, pad, sizeof(U8)));
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
        IMAGE_TRY_PARSE(BinStreamPullU8(&s, &current_byte));
        for (U32 j = 0; j < temp_image.width; j++) {
          U8 color = (current_byte >> (4 * first)) & 0xf;
          temp_image.data[out_idx++] = palette[color][0];
          temp_image.data[out_idx++] = palette[color][1];
          temp_image.data[out_idx++] = palette[color][2];
          temp_image.data[out_idx++] = 255;
          if (j + 1 == temp_image.width) { break; }
          first = !first;
          if (first) { IMAGE_TRY_PARSE(BinStreamPullU8(&s, &current_byte)); }
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&s, pad, sizeof(U8)));
      }
    } break;

    case 8: {
      S32 pad     = (-((S32)temp_image.width)) & 3;
      U32 out_idx = 0;
      a_zero      = 0xff;
      for (U32 i = 0; i < temp_image.height; i++) {
        for (U32 j = 0; j < temp_image.width; j++) {
          U8 color;
          IMAGE_TRY_PARSE(BinStreamPullU8(&s, &color));
          temp_image.data[out_idx++] = palette[color][0];
          temp_image.data[out_idx++] = palette[color][1];
          temp_image.data[out_idx++] = palette[color][2];
          temp_image.data[out_idx++] = 255;
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&s, pad, sizeof(U8)));
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
          IMAGE_TRY_PARSE(BinStreamPullU16LE(&s, &color));
          // NOTE: grab color data using mask, shift into place, and remap to 8 bits by mult. by ratio of 8 bit max to mask max.
          // TODO: is there a way to do this without divides? does it matter?
          temp_image.data[out_idx++] = ((color & r_mask) >> r_shift) * (((F32) 0xff) / (r_mask >> r_shift));
          temp_image.data[out_idx++] = ((color & g_mask) >> g_shift) * (((F32) 0xff) / (g_mask >> g_shift));
          temp_image.data[out_idx++] = ((color & b_mask) >> b_shift) * (((F32) 0xff) / (b_mask >> b_shift));
          temp_image.data[out_idx++] = ((color & a_mask) >> a_shift) * (((F32) 0xff) / (a_mask >> a_shift));
          a_zero |= temp_image.data[out_idx - 1];
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&s, pad, sizeof(U8)));
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
          IMAGE_TRY_PARSE(BinStreamPullU8(&s, &green));
          IMAGE_TRY_PARSE(BinStreamPullU8(&s, &blue));
          IMAGE_TRY_PARSE(BinStreamPullU8(&s, &red));
          temp_image.data[out_idx++] = red;
          temp_image.data[out_idx++] = blue;
          temp_image.data[out_idx++] = green;
          temp_image.data[out_idx++] = 255;
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&s, pad, sizeof(U8)));
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
          IMAGE_TRY_PARSE(BinStreamPullU32LE(&s, &color));
          // NOTE: grab color data using mask, shift into place, and remap to 8 bits by mult. by ratio of 8 bit max to mask max.
          temp_image.data[out_idx++] = ((color & r_mask) >> r_shift) * (((F32) 0xff) / (r_mask >> r_shift));
          temp_image.data[out_idx++] = ((color & g_mask) >> g_shift) * (((F32) 0xff) / (g_mask >> g_shift));
          temp_image.data[out_idx++] = ((color & b_mask) >> b_shift) * (((F32) 0xff) / (b_mask >> b_shift));
          temp_image.data[out_idx++] = ((color & a_mask) >> a_shift) * (((F32) 0xff) / (a_mask >> a_shift));
          a_zero |= temp_image.data[out_idx - 1];
        }
        IMAGE_TRY_PARSE(BinStreamSkip(&s, pad, sizeof(U8)));
      }
    } break;

    default: {
      LOG_ERROR("[IMAGE] Unsupported bits per pixel identified: %d", bits_per_pixel);
      goto image_load_bmp_exit;
    } break;
  }

  // NOTE: If there wasn't any alpha, go back and make everything opaque.
  if (a_zero == 0) {
    for (U32 i = 3; i < temp_image.width * temp_image.height * 4; i += 4) {
      temp_image.data[i] = 255;
    }
  }

  if (y_flipped) { ImageFlipY(&temp_image); }
  ImageConvert(arena, image, &temp_image, format);
  success = true;
image_load_bmp_exit:
  ArenaRelease(temp_arena);
  return success;
}
#undef IMAGE_TRY_PARSE

#define PNG_HUFFMAN_MAX_BIT_COUNT 16
static U32 _cdef_png_hclen_swizzle[]         = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
// NOTE: { last value, num bits }
static U32 _cdef_png_fixed_hclens[][2]       = { {143, 8}, {255, 9}, {279, 7}, {287, 8}, {319, 5} };
// NOTE: { length, extra bits }
static U32 _cdef_png_extra_length_codes[][2] = { {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 1}, {13, 1}, {15, 1}, {17, 1}, {19, 2}, {23, 2}, {27, 2}, {31, 2}, {35, 3}, {43, 3}, {51, 3}, {59, 3}, {67, 4}, {83, 4}, {99, 4}, {115, 4}, {131, 5}, {163, 5}, {195, 5}, {227, 5}, {258, 0} };
// NOTE: { dist, extra bits }
static U32 _cdef_png_extra_dist_codes[][2]   = { {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 1}, {7, 1}, {9, 2}, {13, 2}, {17, 3}, {25, 3}, {33, 4}, {49, 4}, {65, 5}, {97, 5}, {129, 6}, {193, 6}, {257, 7}, {385, 7}, {513, 8}, {769, 8}, {1025, 9}, {1537, 9}, {2049, 10}, {3073, 10}, {4097, 11}, {6145, 11}, {8193, 12}, {12289, 12}, {16385, 13}, {24577, 13} };

typedef struct PngChunk PngChunk;
struct PngChunk {
  BinStream s;
  PngChunk* next;
};

typedef struct PngIdatStream PngIdatStream;
struct PngIdatStream {
  PngChunk* chunks;
  U64 buffer;
  U8  buffer_size;
};

typedef struct PngHuffmanNode PngHuffmanNode;
struct PngHuffmanNode {
  S32 symbol;
  PngHuffmanNode* left;
  PngHuffmanNode* right;
};

static B32 PngIdatStreamPullBits(PngIdatStream* s, U32 num_bits, U32* result) {
  DEBUG_ASSERT(num_bits <= 32);
  while (s->buffer_size < num_bits) {
    U8 byte;
    while (!BinStreamPullU8(&s->chunks->s, &byte)) {
      UNTESTED();
      if (s->chunks->next == NULL) { return false; }
      s->chunks = s->chunks->next;
    }
    s->buffer |= ((U64) byte) << s->buffer_size;
    s->buffer_size += 8;
  }
  *result = s->buffer & ((1 << num_bits) - 1);
  s->buffer >>= num_bits;
  s->buffer_size -= num_bits;
  return true;
}

static void PngIdatStreamFlush(PngIdatStream* s) {
  U32 x = s->buffer_size % 8;
  s->buffer >>= x;
  s->buffer_size -= x;
}

static PngHuffmanNode* PngHuffmanNodeCreate(Arena* arena) {
  PngHuffmanNode* result = ARENA_PUSH_STRUCT(arena, PngHuffmanNode);
  MEMORY_ZERO_STRUCT(result);
  result->symbol = -1;
  return result;
}

static PngHuffmanNode* PngHuffmanBuild(Arena* arena, U32* code_lengths, U32 num_symbols) {
  // NOTE: count number of codes
  S32 code_length_histogram[PNG_HUFFMAN_MAX_BIT_COUNT];
  MEMORY_ZERO_STATIC_ARRAY(code_length_histogram);
  for (U32 i = 0; i < num_symbols; i++) {
    U32 code_length = code_lengths[i];
    if (code_length == 0) { continue; }
    DEBUG_ASSERT(code_length < STATIC_ARRAY_SIZE(code_length_histogram));
    code_length_histogram[code_length] += 1;
  }

  // NOTE: assign starting codes
  S32 next_code[PNG_HUFFMAN_MAX_BIT_COUNT];
  MEMORY_ZERO_STATIC_ARRAY(next_code);
  for (U32 i = 1; i < STATIC_ARRAY_SIZE(next_code); i++) {
    next_code[i] = (next_code[i - 1] + code_length_histogram[i - 1]) << 1;
  }

  // NOTE: build the tree
  PngHuffmanNode* root = PngHuffmanNodeCreate(arena);
  for (S32 symbol = 0; symbol < (S32) num_symbols; symbol++) {
    U32 code_length = code_lengths[symbol];
    if (code_length == 0) { continue; }
    S32 curr_code = next_code[code_length]++;
    PngHuffmanNode* node = root;
    for (S32 i = code_length - 1; i >= 0; i--) {
      S32 bit = (curr_code >> i) & 1;
      if (bit == 0) {
        if (node->left == NULL)  { node->left = PngHuffmanNodeCreate(arena);  }
        node = node->left;
      } else {
        if (node->right == NULL) { node->right = PngHuffmanNodeCreate(arena); }
        node = node->right;
      }
    }
    node->symbol = symbol;
  }
  return root;
}

#define IMAGE_TRY_PARSE(eval) if (!eval) { IMAGE_LOG_OUT_OF_CHARS(); return false; }
static B32 PngHuffmanDecode(PngHuffmanNode* root, PngIdatStream* s, U32* result) {
  PngHuffmanNode* curr = root;
  while (curr->symbol < 0) {
    U32 bit;
    IMAGE_TRY_PARSE(PngIdatStreamPullBits(s, 1, &bit));
    if (bit == 0) { curr = curr->left;  }
    else          { curr = curr->right; }
    if (curr == NULL) {
      LOG_ERROR("[IMAGE] PNG huffman tree unable to decode symbol.");
      return false;
    }
  }
  *result = (U32) curr->symbol;
  return true;
}
#undef IMAGE_TRY_PARSE

static U8 PngFilterSum(U8* a, U8* b, U32 channel) {
  return (U8)a[channel] + (U8)b[channel];
}

static U8 PngFilterAvg(U8* x, U8* a, U8* b, U32 channel) {
  U32 avg = ((U32)a[channel] + (U32)b[channel]) / 2;
  return x[channel] + avg;
}

static U8 PngFilterPaeth(U8* x, U8* a_arr, U8* b_arr, U8* c_arr, U32 channel) {
  S32 a = (S32) a_arr[channel];
  S32 b = (S32) b_arr[channel];
  S32 c = (S32) c_arr[channel];
  S32 p = a + b - c;

  S32 pa = p - a;
  if (pa < 0) { pa = -pa; }
  S32 pb = p - b;
  if (pb < 0) { pb = -pb; }
  S32 pc = p - c;
  if (pc < 0) { pc = -pc; }

  S32 paeth;
  if ((pa <= pb) && (pa <= pc)) { paeth = a; }
  else if (pb <= pc)            { paeth = b; }
  else                          { paeth = c; }
  return x[channel] + (U8) paeth;
}

// https://www.w3.org/TR/2003/REC-PNG-20031110
// https://github.com/HandmadeHero/cpp/blob/master/code/handmade_png.cpp
// https://github.com/nothings/stb/blob/master/stb_image.h
#define IMAGE_TRY_PARSE(eval) if (!eval) { IMAGE_LOG_OUT_OF_CHARS(); goto image_load_png_exit; }
B32 ImageLoadPng(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 file_data_size) {
  MEMORY_ZERO_STRUCT(image);
  B32 success = false;
  U64 arena_base = ArenaPos(arena);
  Arena* temp_arena = ArenaAllocate();
  BinStream s = BinStreamAssign(file_data, file_data_size);

  U64 magic_number;
  IMAGE_TRY_PARSE(BinStreamPullU64BE(&s, &magic_number));
  if (magic_number != 0x89504E470D0A1A0A) { return false; }

  // NOTE: pull out interesting chunks
  PngIdatStream idat;
  MEMORY_ZERO_STRUCT(&idat);
  BinStream* ihdr            = NULL;
  PngChunk* idat_chunks_tail = NULL;
  while (BinStreamRemaining(&s) > 0) {
    U32 chunk_length, chunk_crc;
    String8 chunk_type;
    IMAGE_TRY_PARSE(BinStreamPullU32BE(&s, &chunk_length));
    IMAGE_TRY_PARSE(BinStreamPullStr8(&s, 4, &chunk_type));
    if (BinStreamRemaining(&s) < chunk_length) { IMAGE_LOG_OUT_OF_CHARS(); goto image_load_png_exit; }
    BinStream chunk_stream = BinStreamAssign(BinStreamDecay(&s), chunk_length);
    BinStreamSkip(&s, chunk_length, sizeof(U8));
    IMAGE_TRY_PARSE(BinStreamPullU32BE(&s, &chunk_crc));
    // LOG_INFO("Identified chunk: %.*s", chunk_type.size, chunk_type.str);
    if (Str8Eq(chunk_type, Str8Lit("IHDR"))) {
      if (ihdr != NULL) {
        LOG_ERROR("[IMAGE] PNG multiple IHDR chunks detected!");
        goto image_load_png_exit;
      }
      ihdr = ARENA_PUSH_STRUCT(temp_arena, BinStream);
      MEMORY_ZERO_STRUCT(ihdr);
      *ihdr = chunk_stream;
    } else if (Str8Eq(chunk_type, Str8Lit("IDAT"))) {
      PngChunk* chunk = ARENA_PUSH_STRUCT(temp_arena, PngChunk);
      MEMORY_ZERO_STRUCT(chunk);
      chunk->s = chunk_stream;
      SLL_QUEUE_PUSH_BACK(idat.chunks, idat_chunks_tail, chunk, next);
    }
  }
  if (ihdr == NULL) {
    LOG_ERROR("[IMAGE] PNG missing required chunk 'IHDR'!");
    goto image_load_png_exit;
  }
  if (idat.chunks == NULL) {
    LOG_ERROR("[IMAGE] PNG missing required chunk 'IDAT'!");
    goto image_load_png_exit;
  }

  // NOTE: investigate header chunk
  U32 width, height;
  U8 bit_depth, color_type, compression_method, filter_method, interlace_method;
  IMAGE_TRY_PARSE(BinStreamPullU32BE(ihdr, &width));
  IMAGE_TRY_PARSE(BinStreamPullU32BE(ihdr, &height));
  IMAGE_TRY_PARSE(BinStreamPullU8(ihdr, &bit_depth));
  IMAGE_TRY_PARSE(BinStreamPullU8(ihdr, &color_type));
  IMAGE_TRY_PARSE(BinStreamPullU8(ihdr, &compression_method));
  IMAGE_TRY_PARSE(BinStreamPullU8(ihdr, &filter_method));
  IMAGE_TRY_PARSE(BinStreamPullU8(ihdr, &interlace_method));
  // TODO: support other bit depths.
  if (bit_depth != 8) {
    LOG_ERROR("[IMAGE] PNG unsupported bit depth detected: %d", bit_depth);
    goto image_load_png_exit;
  }
  // TODO: support other color types.
  if (color_type != 6) {
    LOG_ERROR("[IMAGE] PNG unsupported color type detected: %d", color_type);
    goto image_load_png_exit;
  }
  if (compression_method != 0) {
    LOG_ERROR("[IMAGE] PNG unsupported compression method detected: %d", compression_method);
    goto image_load_png_exit;
  }
  if (filter_method != 0) {
    LOG_ERROR("[IMAGE] PNG unsupported filter method detected: %d", filter_method);
    goto image_load_png_exit;
  }
  if (interlace_method != 0) {
    LOG_ERROR("[IMAGE] PNG unsupported interlace method detected: %d", interlace_method);
    goto image_load_png_exit;
  }

  // NOTE: investigate zlib chunks
  U32 zlib_compression_method, flags;
  IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 8, &zlib_compression_method));
  IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 8, &flags));
  U8 zlib_cm    = (zlib_compression_method >> 0) & 0xF;
  U8 zlib_fdict = (flags >> 5) & 0x1;
  if (zlib_cm != 8) {
    LOG_ERROR("[IMAGE] PNG invalid zlib compression method detected: %d", zlib_cm);
    goto image_load_png_exit;
  }
  if (zlib_fdict != 0) {
    LOG_ERROR("[IMAGE] PNG invalid zlib preset dictionary method detected: %d", zlib_fdict);
    goto image_load_png_exit;
  }

  // NOTE: decompress
  // NOTE: addl. height for png filtering information (+1 byte per-row)
  U32 decompressed_pixels_size = ((width * height * 4) + height) * sizeof(U8);
  U8* decompressed_pixels = ARENA_PUSH_ARRAY(temp_arena, U8, decompressed_pixels_size);
  MEMORY_ZERO(decompressed_pixels, decompressed_pixels_size);
  U8* dest = decompressed_pixels;
  U32 bfinal = 0;
  while (bfinal != 1) {
    U32 btype;
    IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 1, &bfinal));
    IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 2, &btype));

      // NOTE: no compression
    if (btype == 0) {
      PngIdatStreamFlush(&idat);
      U32 len = 0, nlen = 0;
      IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 16, &len));
      IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 16, &nlen));
      if ((U16)len != (U16)~nlen) {
        LOG_ERROR("[IMAGE] PNG zlib chunk len / nlen mismatch");
        goto image_load_png_exit;
      }
      for (U32 i = 0; i < len; i++) {
        U32 byte;
        IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 8, &byte));
        *dest++ = (U8) byte;
      }
    } else {
      U32 hlit;
      U32 hdist;
      U32 lit_len_dist_table[512];

      if (btype == 1) {
        // NOTE: fixed huffman codes
        hlit = 288;
        hdist = 32;
        U32 bit_count_idx = 0;
        for (U32 i = 0; i < STATIC_ARRAY_SIZE(_cdef_png_fixed_hclens); i++) {
          U32 last_value = _cdef_png_fixed_hclens[i][0];
          U32 bit_count  = _cdef_png_fixed_hclens[i][1];
          while (bit_count_idx <= last_value) {
            lit_len_dist_table[bit_count_idx++] = bit_count;
          }
        }
      } else if (btype == 2) {
        // NOTE: dynamic huffman codes
        U32 hclen;
        IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 5, &hlit));
        IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 5, &hdist));
        IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 4, &hclen));
        hlit  += 257;
        hdist += 1;
        hclen += 4;

        DEBUG_ASSERT(hclen <= STATIC_ARRAY_SIZE(_cdef_png_hclen_swizzle));
        U32 hclen_table[STATIC_ARRAY_SIZE(_cdef_png_hclen_swizzle)];
        MEMORY_ZERO_STATIC_ARRAY(hclen_table);
        for (U32 i = 0; i < hclen; i++) {
          IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 3, &hclen_table[_cdef_png_hclen_swizzle[i]]));
        }
        PngHuffmanNode* dict_huffman = PngHuffmanBuild(temp_arena, hclen_table, STATIC_ARRAY_SIZE(hclen_table));

        U32 lit_len_count = 0;
        U32 len_count = hlit + hdist;
        while (lit_len_count < len_count) {
          U32 encoded_len;
          if (!PngHuffmanDecode(dict_huffman, &idat, &encoded_len)) { goto image_load_png_exit; }

          U32 rep_count;
          U32 rep_val;
          if (encoded_len <= 15) {
            rep_val = encoded_len;
            rep_count = 1;
          } else if (encoded_len == 16) {
            DEBUG_ASSERT(lit_len_count > 0);
            rep_val = lit_len_dist_table[lit_len_count - 1];
            IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 2, &rep_count));
            rep_count += 3;
          } else if (encoded_len == 17) {
            rep_val = 0;
            IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 3, &rep_count));
            rep_count += 3;
          } else if (encoded_len == 18) {
            rep_val = 0;
            IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, 7, &rep_count));
            rep_count += 11;
          } else {
            LOG_ERROR("[IMAGE] PNG unrecognized encoded length: %d", encoded_len);
            goto image_load_png_exit;
          }
          while (rep_count > 0) {
            rep_count--;
            lit_len_dist_table[lit_len_count++] = rep_val;
          }
        }
        DEBUG_ASSERT(lit_len_count == len_count);

      } else {
        // NOTE: btype is reserved or invalid
        LOG_ERROR("[IMAGE] PNG invalid zlib compression btype provided: %d", btype);
        goto image_load_png_exit;
      }

      PngHuffmanNode* lit_len_huffman = PngHuffmanBuild(temp_arena, lit_len_dist_table, hlit);
      PngHuffmanNode* dist_huffman    = PngHuffmanBuild(temp_arena, lit_len_dist_table + hlit, hdist);
      while (true) {
        // NOTE: LZ compression
        U32 lit_len;
        if (!PngHuffmanDecode(lit_len_huffman, &idat, &lit_len)) { goto image_load_png_exit; }
        if (lit_len < 256) {
          *dest++ = (U8) (lit_len & 0xFF);
        } else if (lit_len > 256) {
          U32 len_idx = lit_len - 257;
          DEBUG_ASSERT(len_idx < STATIC_ARRAY_SIZE(_cdef_png_extra_length_codes));
          U32 length = _cdef_png_extra_length_codes[len_idx][0];
          U32 extra_length_bits_used  = _cdef_png_extra_length_codes[len_idx][1];
          if (extra_length_bits_used > 0) {
            U32 extra_bits;
            IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, extra_length_bits_used, &extra_bits));
            length += extra_bits;
          }

          U32 dist_idx;
          if (!PngHuffmanDecode(dist_huffman, &idat, &dist_idx)) { goto image_load_png_exit; }
          DEBUG_ASSERT(dist_idx < STATIC_ARRAY_SIZE(_cdef_png_extra_dist_codes));
          U32 distance = _cdef_png_extra_dist_codes[dist_idx][0];
          U32 extra_distance_bits_used = _cdef_png_extra_dist_codes[dist_idx][1];
          if (extra_distance_bits_used > 0) {
            U32 extra_bits;
            IMAGE_TRY_PARSE(PngIdatStreamPullBits(&idat, extra_distance_bits_used, &extra_bits));
            distance += extra_bits;
          }

          U8* src = dest - distance;
          DEBUG_ASSERT(src + length <= decompressed_pixels + decompressed_pixels_size);
          DEBUG_ASSERT(dest + length <= decompressed_pixels + decompressed_pixels_size);
          DEBUG_ASSERT(src >= decompressed_pixels);
          while (length > 0) {
            *dest++ = *src++;
            length -= 1;
          }
        } else {
          DEBUG_ASSERT(lit_len == 256);
          break;
        }
      }
    }
  }
  DEBUG_ASSERT(dest == decompressed_pixels + decompressed_pixels_size);

  // NOTE: render to temp image first and convert to final format separately
  Image temp_image;
  temp_image.format = ImageFormat_RGBA;
  temp_image.width  = width;
  temp_image.height = height;
  temp_image.data   = ARENA_PUSH_ARRAY(temp_arena, U8, width * height * 4 * sizeof(U8));
  MEMORY_ZERO(temp_image.data, width * height * 4 * sizeof(U8));

  // NOTE: apply png filters
  U32 zeroes = 0;
  U8* prior_row = (U8*) &zeroes;
  U32 prior_row_advance = 0;
  U8* src = decompressed_pixels;
  dest    = temp_image.data;
  for (U32 y = 0; y < height; y++) {
    U8 filter = *src++;
    U8* current_row = dest;

    switch (filter) {
      case 0: {
        for (U32 x = 0; x < width; x++) {
          dest[0] = src[0];
          dest[1] = src[1];
          dest[2] = src[2];
          dest[3] = src[3];

          dest += 4;
          src  += 4;
        }
      } break;

      case 1: {
        U32 a_px = 0;
        for (U32 x = 0; x < width; x++) {
          dest[0] = PngFilterSum(src, (U8*) &a_px, 0);
          dest[1] = PngFilterSum(src, (U8*) &a_px, 1);
          dest[2] = PngFilterSum(src, (U8*) &a_px, 2);
          dest[3] = PngFilterSum(src, (U8*) &a_px, 3);

          a_px = *(U32*)dest;
          dest += 4;
          src  += 4;
        }
      } break;

      case 2: {
        U8* b_px = prior_row;
        for (U32 x = 0; x < width; x++) {
          dest[0] = PngFilterSum(src, b_px, 0);
          dest[1] = PngFilterSum(src, b_px, 1);
          dest[2] = PngFilterSum(src, b_px, 2);
          dest[3] = PngFilterSum(src, b_px, 3);

          b_px += prior_row_advance;
          dest += 4;
          src  += 4;
        }
      } break;

      case 3: {
        U32 a_px = 0;
        U8* b_px = prior_row;
        for (U32 x = 0; x < width; x++) {
          dest[0] = PngFilterAvg(src, (U8*) &a_px, b_px, 0);
          dest[1] = PngFilterAvg(src, (U8*) &a_px, b_px, 1);
          dest[2] = PngFilterAvg(src, (U8*) &a_px, b_px, 2);
          dest[3] = PngFilterAvg(src, (U8*) &a_px, b_px, 3);

          a_px = *(U32*) dest;
          b_px += prior_row_advance;
          dest += 4;
          src  += 4;
        }
      } break;

      case 4: {
        U32 a_px = 0;
        U8* b_px = prior_row;
        U32 c_px = 0;
        for (U32 x = 0; x < width; x++) {
          dest[0] = PngFilterPaeth(src, (U8*) &a_px, b_px, (U8*) &c_px, 0);
          dest[1] = PngFilterPaeth(src, (U8*) &a_px, b_px, (U8*) &c_px, 1);
          dest[2] = PngFilterPaeth(src, (U8*) &a_px, b_px, (U8*) &c_px, 2);
          dest[3] = PngFilterPaeth(src, (U8*) &a_px, b_px, (U8*) &c_px, 3);

          c_px = *(U32*) b_px;
          a_px = *(U32*) dest;
          b_px += prior_row_advance;
          dest += 4;
          src  += 4;
        }
      } break;

      default: {
        LOG_ERROR("[IMAGE] PNG invalid format scheme: %d", filter);
        goto image_load_png_exit;
      } break;
    }

    prior_row = current_row;
    prior_row_advance = 4;
  }
  // NOTE: PNG stores opposite to how cdefault handles pixels, so flip it
  ImageFlipY(&temp_image);

  ImageConvert(arena, image, &temp_image, format);
  success = true;
image_load_png_exit:
  ArenaRelease(temp_arena);
  if (!success) { ArenaPopTo(arena, arena_base); }
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

B32 ImageLoad(Arena* arena, Image* image, ImageFormat format, U8* file_data, U32 file_data_size) {
  if (ImageLoadBmp(arena, image, format, file_data, file_data_size)) { return true; }
  if (ImageLoadPng(arena, image, format, file_data, file_data_size)) { return true; }
  return false;
}

void ImageConvert(Arena* arena, Image* to, Image* from, ImageFormat to_format) {
  to->format = to_format;
  to->width = from->width;
  to->height = from->height;

  U32 bytes_per_pixel = ImageBytesPerPixel(to->format);
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
  BinStream s = BinStreamAssign(bmp, bmp_file_size);

  // NOTE: file header
  DEBUG_ASSERT(BinStreamPushStr8(&s, Str8Lit("BM")));
  DEBUG_ASSERT(BinStreamPush32LE(&s, bmp_file_size)); // file size
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // reserved 1 & 2
  DEBUG_ASSERT(BinStreamPush32LE(&s, bmp_offset));

  // NOTE: winv4 header
  DEBUG_ASSERT(BinStreamPush32LE(&s, header_size));
  DEBUG_ASSERT(BinStreamPush32LE(&s, temp_image.width));
  // NOTE: internal raw format matches BMP bottom->up, so don't flip.
  DEBUG_ASSERT(BinStreamPush32LE(&s, (S32) temp_image.height));
  DEBUG_ASSERT(BinStreamPush16LE(&s, 1));  // color planes
  DEBUG_ASSERT(BinStreamPush16LE(&s, 32)); // bits per pixel
  DEBUG_ASSERT(BinStreamPush32LE(&s, 3));  // bitfield compression
  DEBUG_ASSERT(BinStreamPush32LE(&s, bmp_size)); // size of bitmap
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // horizontal resolution
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // vertical resolution
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // colors used
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // colors important
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0x00ff0000)); // red mask
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0x0000ff00)); // green mask
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0x000000ff)); // blue mask
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0xff000000)); // alpha mask
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // cs type
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // redx
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // redy
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // redz
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // greenx
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // greeny
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // greenz
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // bluex
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // bluey
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // bluez
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // gamma red
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // gamma green
  DEBUG_ASSERT(BinStreamPush32LE(&s, 0)); // gamma blue
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
      DEBUG_ASSERT(BinStreamPush32LE(&s, color));
    }
  }

  B32 success = FileDump(file_path, bmp, bmp_file_size);
  ArenaRelease(temp_arena);
  return success;
}

U32 ImageBytesPerPixel(ImageFormat format) {
  switch (format) {
    case ImageFormat_RGBA:  return 4;
    case ImageFormat_RGB:   return 3;
    case ImageFormat_R:     return 1;
    default: UNREACHABLE(); return 0;
  }
}

void ImageFlipY(Image* image) {
  U32 bytes_per_pixel = ImageBytesPerPixel(image->format);
  for (U32 i = 0; i < image->height / 2; i++) {
    U8* a = image->data + (i * image->width * bytes_per_pixel);
    U8* b = image->data + ((image->height - 1 - i) * image->width * bytes_per_pixel);
    for (U32 j = 0; j < image->width * bytes_per_pixel; j++) { SWAP(U8, a[j], b[j]); }
  }
}

#undef IMAGE_LOG_OUT_OF_CHARS
#undef PNG_HUFFMAN_MAX_BIT_COUNT

#endif // CDEFAULT_IMAGE_IMPLEMENTATION
