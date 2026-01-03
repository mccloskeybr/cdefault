#ifndef CDEFAULT_FONT_H_
#define CDEFAULT_FONT_H_

#include "cdefault_std.h"
#include "cdefault_io.h"
#include "cdefault_math.h"
#include "cdefault_image.h"

// TODO: better atlas fitting / packing
// TODO: separate? arenas for image and atlas, since the texture can be released after gpu upload.
// TODO: in relevant places, fall back on "no glyph present" glyph instead of erroring up.

// NOTE: this is a font atlas generation library, which can be used for rendering TTF fonts. it is a relatively simple implementation and does no e.g. gridfitting.
// it primarily supports SDF bitmap generation. raw bitmap rasterization is also supported, but no antialiasing is applied, so results look bad at low resolutions.
//
// stb_truetype was used as a reference when creating this library.
//
// e.g.
//
// String8 font_file = Str8Lit("C:/Windows/Fonts/consola.ttf");
// F32 raster_height = 100.0f;
// F32 sdf_height    = 32.0f;
//
// Arena*    atlas_arena = ArenaAllocate();
// Arena*    bmp_arena   = ArenaAllocate();
// FontAtlas sdf_atlas;
// Image     sdf_atlas_image;
// DEBUG_ASSERT(FontAtlasBakeSdfFromFile(atlas_arena, bmp_arena, &sdf_atlas, &sdf_atlas_image, raster_height, sdf_height, FontCharSetLatin(), font_file));
//
// ... now go off to write the bitmap to disk, or render using it, whatever, man.

// NOTE: point is the start point of the curve. The end point is the sequential curve in the list's start point.
typedef struct GlyphCurve GlyphCurve;
struct GlyphCurve {
  V2 point;
  V2 control;
};

// NOTE: A GlyphContour is a series of quadratic bezier curves describing a single line of the glyph.
typedef struct GlyphContour GlyphContour;
struct GlyphContour {
  GlyphCurve* curves;
  U32 curves_size;
};

// NOTE: A glyph's shape is a collection of contours / lines describing the glyph outline
// (in glyph space, straight from the ttf's glyf table).
typedef struct GlyphShape GlyphShape;
struct GlyphShape {
  GlyphContour* contours;
  U32 contours_size;
  V2  min;
  V2  max;
};

typedef struct AtlasChar AtlasChar;
struct AtlasChar {
  U32 codepoint;
  F32 advance;
  V2  offset;
  V2  size;
  V2  uv_min;
  V2  uv_max;

  AtlasChar* next;
};

typedef struct GlyphKernInfo GlyphKernInfo;
struct GlyphKernInfo {
  U32 codepoint_left;
  U32 codepoint_right;
  F32 advance;

  GlyphKernInfo* next;
};

typedef struct FontAtlas FontAtlas;
struct FontAtlas {
  // NOTE: Multiply scale_coeff by the desired render pixel height to get the relevant glyph scale factor.
  F32            scale_coeff;
  F32            ascent;
  F32            descent;
  // TODO: dynamic sizing? just use a list?
  AtlasChar*     char_map[256];
  GlyphKernInfo* kern_map[256];
};

typedef struct FontCharSet FontCharSet;
struct FontCharSet {
  U32* codepoints;
  U32  codepoints_size;
  FontCharSet* next;
};

typedef enum LocFormat LocFormat;
enum LocFormat {
  LocFormat_U16,
  LocFormat_U32,
};

typedef struct Font Font;
struct Font {
  U8* data;
  U32 data_size;
  U32 loca_offset;
  U32 head_offset;
  U32 glyf_offset;
  U32 hhea_offset;
  U32 hmtx_offset;
  U32 kern_offset;
  U32 cmap_subtable_offset;
  U16 num_glyphs;
  S16 ascent;
  S16 descent;
  LocFormat loc_format;
};

// NOTE: Can pass 0 for tuning parameters to use reasonable defaults.
//
// NOTE: Quick functions to bake atlases with less boilerplate.
B32 FontAtlasBakeBitmapFromFile(Arena* atlas_arena, Arena* bitmap_arena, FontAtlas* atlas, Image* bitmap, F32 pixel_height, FontCharSet* char_set, String8 file_path);
B32 FontAtlasBakeSdfFromFile(Arena* atlas_arena, Arena* bitmap_arena, FontAtlas* atlas, Image* bitmap, F32 bmp_pixel_height, F32 sdf_pixel_height, F32 spread_factor, FontCharSet* char_set, String8 file_path);

// NOTE: A Font must be initialized successfully before it can be used by any other function.
// A reference to the TTF data is stored, so it must outlive the Font object.
B32 FontInit(Font* font, U8* data, U32 data_size);

// NOTE: bakes a regular raster of each glyph to an atlas. does *not* perform any anti-aliasing on the edges.
// should only be used for sufficiently large fonts. using an sdf atlas will almost always be better than using this.
// pixel_height dictates the height (and, consequently, resolution) of each glyph in the bitmap.
B32 FontAtlasBakeBitmap(Arena* atlas_arena, Arena* bitmap_arena, Font* font, FontAtlas* atlas, Image* bitmap, F32 pixel_height, FontCharSet* char_set);

// NOTE: bakes an SDF atlas bitmap, which can be used with a shader to do a decent job of rendering glyphs at arbitrary resolutions.
// this is done by rasterizing each glyph to a bitmap first, then, for each pixel in the sdf bitmap, compute & save the distance to the nearest edge.
// bmp_pixel_height dictates the resolution of the intermediary glyph bitmap raster -- a higher value will be more expensive to compute, but will result in more accurate distance calculations for each sdf texel. 100 is a reasonable default.
// sdf_pixel_height dictates the resolution of the final sdf bitmap. a higher value leads to more accurate glyph renders, especially at higher resolutions. 32 is a reasonable default.
// spread_factor dictates the look-around radius when finding the closest edge for each sdf pixel. higher values will lead to a more accurate distance gradient, but is more expensive to compute and may look bad (blurry) for small sdf characters. 8, 16 are reasonable defaults.
B32 FontAtlasBakeSdf(Arena* atlas_arena, Arena* bitmap_arena, Font* font, FontAtlas* atlas, Image* bitmap, F32 bmp_pixel_height, F32 sdf_pixel_height, F32 spread_factor, FontCharSet* char_set);

// NOTE: Function to retrieve data from the atlas when placing characters. Advances the provided cursor and returns relevant information for rendering.
// NOTE: codepoint_next is used for kerning; pass 0 if you don't care.
B32 FontAtlasPlace(FontAtlas* atlas, U32 codepoint, U32 codepoint_next, F32 pixel_height,
                   V2* cursor, V2* center, V2* size, V2* uv_min, V2* uv_max);
void FontAtlasGetAttributes(FontAtlas* atlas, F32 pixel_height, F32* ascent, F32* descent);
B32 FontAtlasMeasureString(FontAtlas* atlas, F32 pixel_height, String8 str, V2* size);

FontCharSet* FontCharSetLatin();
FontCharSet* FontCharSetConcat(FontCharSet* set, FontCharSet* to_concat);

// NOTE: determines the index of the glyph into the various font tables.
B32 FontGetGlyphIndex(Font* font, U32 codepoint, U32* glyph_index);
B32 FontGetGlyphSizeInfo(Font* font, U32 glyph_index, S32* advance, S32* left_side_bearing);
// NOTE: determines the offset of the glyph from the start of the file.
// returns -1 e.g. if there is no entry in the glyf table (true for e.g. ' ').
B32 FontGetGlyphGlyfOffset(Font* font, U32 glyph_index, S32* glyf_offset);
B32 FontGetGlyphShape(Arena* arena, Font* font, S32 glyph_glyf_offset, GlyphShape* shape); // NOTE: Assumes negative glyph_glyf_offset is for chars with no contours, e.g. ' '.

// NOTE: rasterizes glyph_shape into bmp scaling into the inclusive bounds provided.
// Expects *bmp to be pre-allocated with size { bmp_width, bmp_height }.
B32 FontGlyphShapeRasterize(GlyphShape* glyph_shape, U8** bmp, U32 bmp_width, U32 bmp_height, U16 bmp_glyph_min_x, U16 bmp_glyph_min_y, U16 bmp_glyph_max_x, U16 bmp_glyph_max_y);

// NOTE: returns false if codepoint already has an atlas_char represented.
B32 FontAtlasInsertChar(FontAtlas* atlas, U32 codepoint, AtlasChar* atlas_char);
AtlasChar* FontAtlasGetChar(FontAtlas* atlas, U32 codepoint);
B32 FontAtlasInsertKern(FontAtlas* atlas, U32 codepoint_left, U32 codepoint_right, GlyphKernInfo* kern);
GlyphKernInfo* FontAtlasGetKern(FontAtlas* atlas, U32 codepoint_left, U32 codepoint_right);

#endif // CDEFAULT_FONT_H_

#ifdef CDEFAULT_FONT_IMPLEMENTATION
#undef CDEFAULT_FONT_IMPLEMENTATION
// https://tchayen.github.io/posts/ttf-file-parsing
// https://github.com/nothings/stb/blob/master/stb_truetype.h
// https://developer.apple.com/fonts/TrueType-Reference-Manual/

#define FONT_PID_UNICODE_ENCODING       0
#define FONT_PID_MICROSOFT_ENCODING     3
#define FONT_MICROSOFT_EID_UNICODE_BMP  1
#define FONT_MICROSOFT_EID_UNICODE_FULL 10

#define FLAG_ON_CURVE(flags)             ((flags) & BIT(0))
#define FLAG_X_U8(flags)                 ((flags) & BIT(1))    // NOTE: if false, is U16
#define FLAG_Y_U8(flags)                 ((flags) & BIT(2))    // NOTE: if false, is U16
#define FLAG_REPEAT(flags)               ((flags) & BIT(3))
#define FLAG_X_U8_SIGN_POS(flags)        ((flags) & BIT(4))    // NOTE: only use if x format is U8
#define FLAG_X_U16_DELTA(flags)          (!((flags) & BIT(4))) // NOTE: only use if x format is U16
#define FLAG_Y_U8_SIGN_POS(flags)        ((flags) & BIT(5))    // NOTE: only use if y format is U8
#define FLAG_Y_U16_DELTA(flags)          (!((flags) & BIT(5))) // NOTE: only use if y format is U16

#define COMPOUND_FLAG_ARE_U16(flags)     ((flags) & BIT(0))
#define COMPOUND_FLAG_ARE_XY_VALS(flags) ((flags) & BIT(1))
#define COMPOUND_FLAG_SCALE(flags)       ((flags) & BIT(3))
#define COMPOUND_FLAG_MORE(flags)        ((flags) & BIT(5))
#define COMPOUND_FLAG_SEP_SCALES(flags)  ((flags) & BIT(6))
#define COMPOUND_FLAG_2X2MAT(flags)      ((flags) & BIT(7))

#define FONT_LOG_OUT_OF_CHARS() LOG_ERROR("[FONT] Ran out of characters in font TTF file.")

typedef struct FontIntersectPoint FontIntersectPoint;
struct FontIntersectPoint {
  F32 x;
  S32 winding; // NOTE: +1 for CW, -1 for CCW.
};

static S32 SortCompareFontIntersectPointAsc(void* a, void* b) {
  return ((FontIntersectPoint*) a)->x - ((FontIntersectPoint*) b)->x;
}

static F32 FontCurveEvaluateBezier(F32 start, F32 end, F32 control, F32 t) {
  F32 start_to_control = F32Lerp(start, control, t);
  F32 control_to_end   = F32Lerp(control, end, t);
  F32 result           = F32Lerp(start_to_control, control_to_end, t);
  return result;
}

static F32 FontCurveEvaluateBezierDerivative(F32 start, F32 end, F32 control, F32 t) {
  return 2.0f * ((1.0f - t) * (control - start) + (t * (end - control)));
}

static void FontBboxScale(V2* min, V2* max, V2* scaled_min, V2* scaled_max, F32 scale, V2 pad) {
  scaled_min->x = (min->x * scale) - pad.x;
  scaled_min->y = (min->y * scale) - pad.y;
  scaled_max->x = (max->x * scale) + pad.x;
  scaled_max->y = (max->y * scale) + pad.y;
}

// NOTE: this very simply determines the maximum glyph bbox and generates a width / height that fits all characters assuming that bbox.
// this will be very bad if there is a lot of variance in glyph size, but seemed more desirable than e.g. having a dynamic bitmap size.
#define FONT_TRY_PARSE(eval) if (!eval) { FONT_LOG_OUT_OF_CHARS(); return false; }
static B32 FontSuggestAtlasSize(Font* font, FontCharSet* char_set, F32 scale, U32* width, U32* height, F32 pad_x, F32 pad_y) {
  F32 num_glyphs = 0;
  F32 max_glyph_width = 0;
  F32 max_glyph_height = 0;

  BinStream s;
  BinStreamInit(&s, font->data, font->data_size);

  for (FontCharSet* curr_char_set = char_set; curr_char_set != NULL; curr_char_set = curr_char_set->next) {
    for (U32 i = 0; i < curr_char_set->codepoints_size; i++) {
      U32 codepoint = curr_char_set->codepoints[i];

      U32 glyph_index;
      if (!FontGetGlyphIndex(font, codepoint, &glyph_index)) {
        LOG_ERROR("[FONT] Failed to retrieve glyph index for codepoint: %d", codepoint);
        return false;
      }

      S32 glyph_glyf_offset;
      if (!FontGetGlyphGlyfOffset(font, glyph_index, &glyph_glyf_offset)) {
        LOG_ERROR("[FONT] Failed to get glyf offset for codepoint: %d", codepoint);
        return false;
      }
      if (glyph_glyf_offset < 0) { continue; }

      S16 min_x, min_y, max_x, max_y;
      FONT_TRY_PARSE(BinStreamSeek(&s, glyph_glyf_offset));
      FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U16)));
      FONT_TRY_PARSE(BinStreamPullS16BE(&s, &min_x));
      FONT_TRY_PARSE(BinStreamPullS16BE(&s, &min_y));
      FONT_TRY_PARSE(BinStreamPullS16BE(&s, &max_x));
      FONT_TRY_PARSE(BinStreamPullS16BE(&s, &max_y));

      F32 scaled_min_x  = min_x * scale;
      F32 scaled_min_y  = min_y * scale;
      F32 scaled_max_x  = max_x * scale;
      F32 scaled_max_y  = max_y * scale;
      F32 scaled_width  = scaled_max_x - scaled_min_x;
      F32 scaled_height = scaled_max_y - scaled_min_y;
      max_glyph_width   = MAX(scaled_width, max_glyph_width);
      max_glyph_height  = MAX(scaled_height, max_glyph_height);
      num_glyphs++;
    }
  }

  F32 num_cols = F32Ceil(F32Sqrt(num_glyphs));
  F32 num_rows = F32Ceil(num_glyphs / num_cols);
  *width       = F32Ceil(num_cols * (max_glyph_width + pad_x));
  *height      = F32Ceil(num_rows * (max_glyph_height + pad_y));
  return true;
}
#undef FONT_TRY_PARSE

// NOTE: returns false on bad parse. may return true e.g. if there is no kern table, that kind of thing.
#define FONT_TRY_PARSE(eval) if (!eval) { FONT_LOG_OUT_OF_CHARS(); return false; }
static B32 FontAtlasBuildKernTable(Arena* arena, Font* font, FontAtlas* atlas, F32 scale, FontCharSet* char_set) {
  if (font->kern_offset == 0) { return true; }

  BinStream s;
  BinStreamInit(&s, font->data, font->data_size);
  FONT_TRY_PARSE(BinStreamSeek(&s, font->kern_offset));

  // NOTE: apple's documentation insists that, after coverage, there is a tupleIndex U16.
  // it fails to mention that this is only present for apple's super special tables. not everyone else's (microsoft / OTF).
  U16 num_tables, coverage, num_pairs;
  FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U16)));
  FONT_TRY_PARSE(BinStreamPullU16BE(&s, &num_tables));
  FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U32)));
  FONT_TRY_PARSE(BinStreamPullU16BE(&s, &coverage));
  FONT_TRY_PARSE(BinStreamPullU16BE(&s, &num_pairs));
  FONT_TRY_PARSE(BinStreamSkip(&s, 3, sizeof(U16)));

  if (num_tables == 0) { return true; }
  U16 flags  = coverage & ~0xff;
  U16 format = coverage &  0xff;
  if (format != 1) {
    LOG_WARN("[FONT] Unsupported kern table format observed (only format 1 is supported): %d", format);
    return true;
  }
  if (flags != 0) {
    LOG_WARN("[FONT] Unsupported kern table flags observed: %x", flags);
    return true;
  }

  for (FontCharSet* left_char_set = char_set; left_char_set != NULL; left_char_set = left_char_set->next) {
    for (U32 i = 0; i < left_char_set->codepoints_size; i++) {
      U32 codepoint_left = left_char_set->codepoints[i];
      for (FontCharSet* right_char_set = char_set; right_char_set != NULL; right_char_set = right_char_set->next) {
        for (U32 j = 0; j < right_char_set->codepoints_size; j++) {
          U32 codepoint_right = right_char_set->codepoints[j];
          if (FontAtlasGetKern(atlas, codepoint_left, codepoint_right) != NULL) { continue; }

          S32 left    = 0;
          S32 right   = num_pairs - 1;
          U32 needle  = codepoint_left << 16 | codepoint_right;
          S16 advance = 0;
          while (left <= right) {
            S32 middle = (left + right) / 2;
            // NOTE: (middle * 3) + 0 = test_left, (middle * 3) + 1 = test_right (as U16s). so test = test_left << 16 | test_right (as U32).
            U32 test;
            FONT_TRY_PARSE(BinStreamPeekU32BE(&s, middle * 3, sizeof(U16), &test));
            if      (needle < test) { right = middle - 1; }
            else if (needle > test) { left  = middle + 1; }
            else {
              FONT_TRY_PARSE(BinStreamPeekS16BE(&s, (middle * 3) + 2, sizeof(U16), &advance));
              break;
            }
          }

          if (advance != 0) {
            GlyphKernInfo* kern   = ARENA_PUSH_STRUCT(arena, GlyphKernInfo);
            MEMORY_ZERO_STRUCT(kern);
            kern->codepoint_left  = codepoint_left;
            kern->codepoint_right = codepoint_right;
            kern->advance         = ((F32) advance) * scale;
            DEBUG_ASSERT(FontAtlasInsertKern(atlas, codepoint_left, codepoint_right, kern));
          }
        }
      }
    }
  }
  return true;
}
#undef FONT_TRY_PARSE

B32 FontAtlasBakeBitmapFromFile(Arena* atlas_arena, Arena* bitmap_arena, FontAtlas* atlas, Image* bitmap, F32 pixel_height, FontCharSet* char_set, String8 file_path) {
  B32 success = false;
  Arena* file_arena = ArenaAllocate();
  String8 ttf_data;
  if (!FileReadAll(file_arena, file_path, &ttf_data.str, &ttf_data.size)) { goto font_atlas_bake_bitmap_from_file_exit; }
  Font font;
  if (!FontInit(&font, ttf_data.str, ttf_data.size)) { goto font_atlas_bake_bitmap_from_file_exit; }
  if (!FontAtlasBakeBitmap(atlas_arena, bitmap_arena, &font, atlas, bitmap, pixel_height, char_set)) { goto font_atlas_bake_bitmap_from_file_exit; }
  success = true;
font_atlas_bake_bitmap_from_file_exit:
  ArenaRelease(file_arena);
  return success;
}

B32 FontAtlasBakeSdfFromFile(Arena* atlas_arena, Arena* bitmap_arena, FontAtlas* atlas, Image* bitmap, F32 bmp_pixel_height, F32 sdf_pixel_height, F32 spread_factor, FontCharSet* char_set, String8 file_path) {
  B32 success = false;
  Arena* file_arena = ArenaAllocate();
  String8 ttf_data;
  if (!FileReadAll(file_arena, file_path, &ttf_data.str, &ttf_data.size)) { goto font_atlas_bake_sdf_from_file_exit; }
  Font font;
  if (!FontInit(&font, ttf_data.str, ttf_data.size)) { goto font_atlas_bake_sdf_from_file_exit; }
  if (!FontAtlasBakeSdf(atlas_arena, bitmap_arena, &font, atlas, bitmap, bmp_pixel_height, sdf_pixel_height, spread_factor, char_set)) { goto font_atlas_bake_sdf_from_file_exit; }
  success = true;
font_atlas_bake_sdf_from_file_exit:
  ArenaRelease(file_arena);
  return success;
}

#define FONT_TRY_PARSE(eval) if (!eval) { FONT_LOG_OUT_OF_CHARS(); return false; }
B32 FontInit(Font* font, U8* data, U32 data_size) {
  MEMORY_ZERO_STRUCT(font);
  font->data = data;
  font->data_size = data_size;

  BinStream s;
  BinStreamInit(&s, data, data_size);

  // NOTE: parse header
  U16 num_tables;
  FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U32)));
  FONT_TRY_PARSE(BinStreamPullU16BE(&s, &num_tables));
  FONT_TRY_PARSE(BinStreamSkip(&s, 6, sizeof(U8)));

  // NOTE: parse top-level tables
  U32 cmap_offset = 0;
  U32 maxp_offset = 0;
  for (U32 i = 0; i < num_tables; i++) {
    String8 tag;
    U32 table_offset;
    FONT_TRY_PARSE(BinStreamPullStr8(&s, 4, &tag));
    FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U32)));
    FONT_TRY_PARSE(BinStreamPullU32BE(&s, &table_offset));
    FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U32)));
    if      (Str8Eq(tag, Str8Lit("cmap"))) { cmap_offset = table_offset;       }
    else if (Str8Eq(tag, Str8Lit("maxp"))) { maxp_offset = table_offset;       }
    else if (Str8Eq(tag, Str8Lit("loca"))) { font->loca_offset = table_offset; }
    else if (Str8Eq(tag, Str8Lit("head"))) { font->head_offset = table_offset; }
    else if (Str8Eq(tag, Str8Lit("glyf"))) { font->glyf_offset = table_offset; }
    else if (Str8Eq(tag, Str8Lit("hhea"))) { font->hhea_offset = table_offset; }
    else if (Str8Eq(tag, Str8Lit("hmtx"))) { font->hmtx_offset = table_offset; }
    else if (Str8Eq(tag, Str8Lit("kern"))) { font->kern_offset = table_offset; }
  }
  if (cmap_offset == 0) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'cmap'.");
    return false;
  }
  if (maxp_offset == 0) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'maxp'.");
    return false;
  }
  if (font->loca_offset == 0) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'loca'.");
    return false;
  }
  if (font->head_offset == 0) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'head'.");
    return false;
  }
  if (font->glyf_offset == 0) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'glyf'.");
    return false;
  }
  if (font->hhea_offset == 0) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'hhea'.");
    return false;
  }
  if (font->hmtx_offset == 0) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'hmtx'.");
    return false;
  }

  FONT_TRY_PARSE(BinStreamSeek(&s, cmap_offset));
  FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U16)));
  U16 num_cmap_subtables;
  FONT_TRY_PARSE(BinStreamPullU16BE(&s, &num_cmap_subtables));
  for (U16 i = 0; i < num_cmap_subtables; i++) {
    U16 plat_id, plat_spec_id;
    U32 index_offset;
    FONT_TRY_PARSE(BinStreamPullU16BE(&s, &plat_id));
    FONT_TRY_PARSE(BinStreamPullU16BE(&s, &plat_spec_id));
    FONT_TRY_PARSE(BinStreamPullU32BE(&s, &index_offset));
    if (plat_id == FONT_PID_MICROSOFT_ENCODING) {
      if (plat_spec_id == FONT_MICROSOFT_EID_UNICODE_BMP ||
          plat_spec_id == FONT_MICROSOFT_EID_UNICODE_FULL) {
        font->cmap_subtable_offset = cmap_offset + index_offset;
      }
    } else if (plat_id == FONT_PID_UNICODE_ENCODING) {
      font->cmap_subtable_offset = cmap_offset + index_offset;
    }
  }
  if (font->cmap_subtable_offset == 0) {
    LOG_ERROR("[FONT] Failed to find supported cmap encoding table.");
    return false;
  }

  FONT_TRY_PARSE(BinStreamSeek(&s, font->head_offset));
  FONT_TRY_PARSE(BinStreamSkip(&s, 50, sizeof(U8)));
  U16 index_to_loc_format;
  FONT_TRY_PARSE(BinStreamPullU16BE(&s, &index_to_loc_format));
  switch (index_to_loc_format) {
    case 0: { font->loc_format = LocFormat_U16; } break;
    case 1: { font->loc_format = LocFormat_U32; } break;
    default: {
      LOG_ERROR("[FONT] Invalid or unsupported indexToLocFormat provided: %d", index_to_loc_format);
      return false;
    } break;
  }

  FONT_TRY_PARSE(BinStreamSeek(&s, maxp_offset));
  FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U32)));
  FONT_TRY_PARSE(BinStreamPullU16BE(&s, &font->num_glyphs));

  FONT_TRY_PARSE(BinStreamSeek(&s, font->hhea_offset));
  FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U32)));
  FONT_TRY_PARSE(BinStreamPullS16BE(&s, &font->ascent));
  FONT_TRY_PARSE(BinStreamPullS16BE(&s, &font->descent));
  font->descent = F32Abs(font->descent);

  return true;
}
#undef FONT_TRY_PARSE

#define FONT_TRY_PARSE(eval) if (!eval) { FONT_LOG_OUT_OF_CHARS(); return false; }
B32 FontGetGlyphIndex(Font* font, U32 codepoint, U32* glyph_index) {
  *glyph_index = 0;
  BinStream s;
  BinStreamInit(&s, font->data, font->data_size);
  FONT_TRY_PARSE(BinStreamSeek(&s, font->cmap_subtable_offset));

  U16 cmap_format;
  FONT_TRY_PARSE(BinStreamPullU16BE(&s, &cmap_format));
  switch (cmap_format) {
    case 0: {
      U16 length;
      FONT_TRY_PARSE(BinStreamPullU16BE(&s, &length));
      if (codepoint >= length) {
        LOG_ERROR("[FONT] Failed to determine codepoint glyph index, codepoint exceeds table length: %d, %d", codepoint, length);
        return false;
      }
      FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U16)));
      FONT_TRY_PARSE(BinStreamSkip(&s, codepoint, sizeof(U8)));
      FONT_TRY_PARSE(BinStreamPullU8(&s, (U8*) glyph_index));
      return true;
    } break;

    case 4: {
      if (codepoint > 0xffff) {
        LOG_ERROR("[FONT] Codepoint exceeds bounds for subtable 4: %d", codepoint);
        return false;
      }

      U16 seg_count, search_range, entry_selector, range_shift;
      FONT_TRY_PARSE(BinStreamSkip(&s, 2, sizeof(U16)));
      FONT_TRY_PARSE(BinStreamPullU16BE(&s, &seg_count));
      FONT_TRY_PARSE(BinStreamPullU16BE(&s, &search_range));
      FONT_TRY_PARSE(BinStreamPullU16BE(&s, &entry_selector));
      FONT_TRY_PARSE(BinStreamPullU16BE(&s, &range_shift));
      seg_count    = seg_count / 2;
      search_range = search_range / 2;

      BinStream end_codes   = s;
      FONT_TRY_PARSE(BinStreamSkip(&s, seg_count, sizeof(U16)));
      FONT_TRY_PARSE(BinStreamSkip(&s, 1, sizeof(U16)));
      BinStream start_codes = s;
      FONT_TRY_PARSE(BinStreamSkip(&s, seg_count, sizeof(U16)));
      BinStream id_deltas   = s;
      FONT_TRY_PARSE(BinStreamSkip(&s, seg_count, sizeof(U16)));
      BinStream offsets     = s;

      U32 search = 0;
      U16 test_codepoint;
      FONT_TRY_PARSE(BinStreamPeekU16BE(&end_codes, range_shift, sizeof(U8), &test_codepoint));
      if (codepoint > test_codepoint) { search += range_shift; }

      while (entry_selector > 0) {
        search_range /= 2;
        U16 end;
        FONT_TRY_PARSE(BinStreamPeekU16BE(&end_codes, search + (search_range * 2) - 2, sizeof(U8), &end));
        if (codepoint > end) { search += search_range * 2; }
        entry_selector -= 1;
      }

      U16 item = search / 2;
      U16 start_code, end_code;
      FONT_TRY_PARSE(BinStreamPeekU16BE(&start_codes, item, sizeof(U16), &start_code));
      FONT_TRY_PARSE(BinStreamPeekU16BE(&end_codes, item, sizeof(U16), &end_code));
      if (codepoint < start_code || end_code < codepoint) {
        LOG_ERROR("[FONT] Failed to find valid character segment / glyph index for codepoint: %d", codepoint);
        return false;
      }

      U16 offset;
      FONT_TRY_PARSE(BinStreamPeekU16BE(&offsets, item, sizeof(U16), &offset));
      if (offset == 0) {
        S16 delta;
        FONT_TRY_PARSE(BinStreamPeekS16BE(&id_deltas, item, sizeof(U16), &delta));
        *glyph_index = codepoint + delta;
      } else {
        *glyph_index = 0;
        FONT_TRY_PARSE(BinStreamPeekU16BE(&offsets, offset + (2 * (codepoint - start_code)) + (item * 2), sizeof(U8), (U16*) glyph_index));
      }
      return true;
    } break;

    case 12:
    case 13: {
      FONT_TRY_PARSE(BinStreamSkip(&s, 10, sizeof(U8)));
      U32 number_groups;
      FONT_TRY_PARSE(BinStreamPullU32BE(&s, &number_groups));
      BinStream groups = s;
      U32 low  = 0;
      U32 high = number_groups;
      while (low < high ) {
        U32 mid = low + ((high - low) / 2);
        U32 mid_start_char, mid_end_char;
        FONT_TRY_PARSE(BinStreamPeekU32BE(&groups, (mid * 3) + 0, sizeof(U32), &mid_start_char));
        FONT_TRY_PARSE(BinStreamPeekU32BE(&groups, (mid * 3) + 1, sizeof(U32), &mid_end_char));
        if      (codepoint < mid_start_char) { high = mid;    }
        else if (codepoint > mid_end_char)   { low = mid + 1; }
        else {
          U32 start_glyph;
          FONT_TRY_PARSE(BinStreamPeekU32BE(&groups, (mid * 12) + 8, sizeof(U8), &start_glyph));
          if (cmap_format == 12) {
            *glyph_index = start_glyph + codepoint - mid_start_char;
            break;
          } else {
            DEBUG_ASSERT(cmap_format == 13);
            *glyph_index = start_glyph;
            break;
          }
        }
      }
      if (*glyph_index == 0) {
        LOG_ERROR("[FONT] Failed to find character group / glyph index for codepoint: %d", codepoint);
        return false;
      }
      return true;
    } break;
  }

  LOG_ERROR("[FONT] Unimplemented cmap subtable format encountered: %d", cmap_format);
  return false;
}
#undef FONT_TRY_PARSE

#define FONT_TRY_PARSE(eval) if (!eval) { FONT_LOG_OUT_OF_CHARS(); return false; }
B32 FontGetGlyphGlyfOffset(Font* font, U32 glyph_index, S32* glyf_offset) {
  *glyf_offset = 0;
  if (glyph_index > font->num_glyphs) {
    LOG_ERROR("[FONT] Provided glyph index exceeds the number of glyphs: %d, %d", glyph_index, font->num_glyphs);
    return false;
  }

  U32 next_glyf_offset = 0;
  BinStream s;
  BinStreamInit(&s, font->data, font->data_size);
  FONT_TRY_PARSE(BinStreamSeek(&s, font->loca_offset));
  switch (font->loc_format) {
    case LocFormat_U16: {
      U16 curr_offset, next_offset;
      FONT_TRY_PARSE(BinStreamSkip(&s, glyph_index, sizeof(U16)));
      FONT_TRY_PARSE(BinStreamPullU16BE(&s, &curr_offset));
      FONT_TRY_PARSE(BinStreamPullU16BE(&s, &next_offset));
      *glyf_offset     = font->glyf_offset + (curr_offset * 2);
      next_glyf_offset = font->glyf_offset + (next_offset * 2);
    } break;
    case LocFormat_U32: {
      U32 curr_offset, next_offset;
      FONT_TRY_PARSE(BinStreamSkip(&s, glyph_index, sizeof(U32)));
      FONT_TRY_PARSE(BinStreamPullU32BE(&s, &curr_offset));
      FONT_TRY_PARSE(BinStreamPullU32BE(&s, &next_offset));
      *glyf_offset     = font->glyf_offset + curr_offset;
      next_glyf_offset = font->glyf_offset + next_offset;
    } break;
    default: {
      LOG_ERROR("[FONT] Unrecognized TTF loca format: %d", font->loc_format);
      return false;
    } break;
  }
  // NOTE: length is implied from offset_next - offset_curr.
  U32 glyf_length = next_glyf_offset - *glyf_offset;
  if (glyf_length == 0) { *glyf_offset = -1; }
  return true;
}
#undef FONT_TRY_PARSE

#define FONT_TRY_PARSE(eval) if (!eval) { FONT_LOG_OUT_OF_CHARS(); return false; }
B32 FontGetGlyphSizeInfo(Font* font, U32 glyph_index, S32* advance, S32* left_side_bearing) {
  *advance = 0;
  *left_side_bearing = 0;
  if (glyph_index > font->num_glyphs) {
    LOG_ERROR("[FONT] Provided glyph index exceeds the number of glyphs: %d, %d", glyph_index, font->num_glyphs);
    return false;
  }

  BinStream s;
  BinStreamInit(&s, font->data, font->data_size);
  U16 num_long_hor_metrics;
  FONT_TRY_PARSE(BinStreamSeek(&s, font->hhea_offset));
  FONT_TRY_PARSE(BinStreamSkip(&s, 34, sizeof(U8)));
  FONT_TRY_PARSE(BinStreamPullU16BE(&s, &num_long_hor_metrics));
  if (glyph_index < num_long_hor_metrics) {
    FONT_TRY_PARSE(BinStreamSeek(&s, font->hmtx_offset));
    FONT_TRY_PARSE(BinStreamSkip(&s, glyph_index * 2, sizeof(U16)));
    FONT_TRY_PARSE(BinStreamPullS16BE(&s, (S16*) advance));
    FONT_TRY_PARSE(BinStreamPullS16BE(&s, (S16*) left_side_bearing));

  } else {
    // NOTE: e.g. for monospaced fonts
    FONT_TRY_PARSE(BinStreamSeek(&s, font->hmtx_offset));
    FONT_TRY_PARSE(BinStreamSkip(&s, 2 * (num_long_hor_metrics - 1), sizeof(U16)));
    FONT_TRY_PARSE(BinStreamPullS16BE(&s, (S16*) advance));

    FONT_TRY_PARSE(BinStreamSeek(&s, font->hmtx_offset));
    FONT_TRY_PARSE(BinStreamSkip(&s, 2 * num_long_hor_metrics, sizeof(U16)));
    FONT_TRY_PARSE(BinStreamSkip(&s, glyph_index - num_long_hor_metrics, sizeof(U16)));
    FONT_TRY_PARSE(BinStreamPullS16BE(&s, (S16*) left_side_bearing));
  }
  return true;
}
#undef FONT_TRY_PARSE

#define FONT_TRY_PARSE(eval) if (!eval) { FONT_LOG_OUT_OF_CHARS(); goto font_get_glyph_shape_end; }
B32 FontGetGlyphShape(Arena* arena, Font* font, S32 glyph_glyf_offset, GlyphShape* shape) {
  MEMORY_ZERO_STRUCT(shape);
  if (glyph_glyf_offset < 0) { return true; }
  B32 success = false;
  U64 base_pos = ArenaPos(arena);

  Arena* temp_arena = ArenaAllocate();

  BinStream s;
  BinStreamInit(&s, font->data, font->data_size);
  FONT_TRY_PARSE(BinStreamSeek(&s, glyph_glyf_offset));

  S16 contours_size, min_x, min_y, max_x, max_y;
  FONT_TRY_PARSE(BinStreamPullS16BE(&s, &contours_size));
  FONT_TRY_PARSE(BinStreamPullS16BE(&s, &min_x));
  FONT_TRY_PARSE(BinStreamPullS16BE(&s, &min_y));
  FONT_TRY_PARSE(BinStreamPullS16BE(&s, &max_x));
  FONT_TRY_PARSE(BinStreamPullS16BE(&s, &max_y));
  shape->min.x = min_x;
  shape->min.y = min_y;
  shape->max.x = max_x;
  shape->max.y = max_y;

  if (contours_size > 0) {
    // NOTE: parse simple shape
    shape->contours      = ARENA_PUSH_ARRAY(arena, GlyphContour, contours_size);
    shape->contours_size = contours_size;

    BinStream end_pts_of_contours = s;
    FONT_TRY_PARSE(BinStreamSkip(&s, contours_size, sizeof(U16)));
    U16 instructions_length;
    FONT_TRY_PARSE(BinStreamPullU16BE(&s, &instructions_length));
    FONT_TRY_PARSE(BinStreamSkip(&s, instructions_length, sizeof(U8)));

    U16 num_vertices;
    FONT_TRY_PARSE(BinStreamPeekU16BE(&end_pts_of_contours, contours_size - 1, sizeof(U16), &num_vertices));
    num_vertices += 1;
    U8*  pt_flags = ARENA_PUSH_ARRAY(temp_arena, U8, num_vertices);
    S16* pt_xs    = ARENA_PUSH_ARRAY(temp_arena, S16, num_vertices);
    S16* pt_ys    = ARENA_PUSH_ARRAY(temp_arena, S16, num_vertices);

    U8 flags = 0;
    U8 flag_num_repeat = 0;
    for (U16 i = 0; i < num_vertices; i++) {
      if (flag_num_repeat == 0) {
        FONT_TRY_PARSE(BinStreamPullU8(&s, &flags));
        if (FLAG_REPEAT(flags)) {
          FONT_TRY_PARSE(BinStreamPullU8(&s, &flag_num_repeat));
        }
      } else {
        flag_num_repeat--;
      }
      pt_flags[i] = flags;
    }
    if (flag_num_repeat != 0) {
      LOG_ERROR("[FONT] Invalid TTF glyph data present. Flag repeat counter at end of parse should be 0, got: %d", flag_num_repeat);
      goto font_get_glyph_shape_end;
    }

    S16 x = 0;
    for (U16 i = 0; i < num_vertices; i++) {
      U8 flags = pt_flags[i];
      if (FLAG_X_U8(flags)) {
        U8 dx;
        FONT_TRY_PARSE(BinStreamPullU8(&s, &dx));
        if (FLAG_X_U8_SIGN_POS(flags)) { x += dx; }
        else                           { x -= dx; }
      } else {
        if (FLAG_X_U16_DELTA(flags)) {
          S16 dx;
          FONT_TRY_PARSE(BinStreamPullS16BE(&s, &dx));
          x += dx;
        }
      }
      pt_xs[i] = x;
    }

    S16 y = 0;
    for (U16 i = 0; i < num_vertices; i++) {
      U8 flags = pt_flags[i];
      if (FLAG_Y_U8(flags)) {
        U8 dy;
        FONT_TRY_PARSE(BinStreamPullU8(&s, &dy));
        if (FLAG_Y_U8_SIGN_POS(flags)) { y += dy; }
        else                           { y -= dy; }
      } else {
        if (FLAG_Y_U16_DELTA(flags)) {
          S16 dy;
          FONT_TRY_PARSE(BinStreamPullS16BE(&s, &dy));
          y += dy;
        }
      }
      pt_ys[i] = y;
    }

    // NOTE: decompress / build final glyph contour
    U16 contours_idx     = 0;
    U16 start_of_contour = 0;
    U16 end_of_contour;
    FONT_TRY_PARSE(BinStreamPeekU16BE(&end_pts_of_contours, contours_idx, sizeof(U16), &end_of_contour));
    while (true) {
      GlyphContour contour;
      MEMORY_ZERO_STRUCT(&contour);
      U32 curves_capacity = 0;

      for (U16 i = start_of_contour; i <= end_of_contour; i++) {
        // NOTE: loop points around so they stay within [start_of_contour, end_of_contour].
        U32 p0 = start_of_contour + (((i + 0) - start_of_contour) % (end_of_contour + 1 - start_of_contour));
        U32 p1 = start_of_contour + (((i + 1) - start_of_contour) % (end_of_contour + 1 - start_of_contour));
        U32 p2 = start_of_contour + (((i + 2) - start_of_contour) % (end_of_contour + 1 - start_of_contour));

        // NOTE: off contour == is a control point
        B32 p0_on_contour = FLAG_ON_CURVE(pt_flags[p0]);
        B32 p1_on_contour = FLAG_ON_CURVE(pt_flags[p1]);
        B32 p2_on_contour = FLAG_ON_CURVE(pt_flags[p2]);

        S16 p0_x = pt_xs[p0];
        S16 p0_y = pt_ys[p0];
        S16 p1_x = pt_xs[p1];
        S16 p1_y = pt_ys[p1];

        // NOTE: in cases of 2 off-contour points in a row, it is implied there is an on-contour point at their midpoint.
        // NOTE: this series of checks is structured so that we always *start* at a real or implied curve start point,
        // which is why we check 3 points at a time and there are spurious index skips.
        GlyphCurve curve;
        if (p0_on_contour) {
          if (p1_on_contour) {
            // on on n/a
            // NOTE: insert a fake control point in the middle so we don't need to differentiate between curves and lines.
            curve.point.x   = p0_x;
            curve.point.y   = p0_y;
            curve.control.x = ((F32) (p0_x + p1_x)) / 2.0f;
            curve.control.y = ((F32) (p0_y + p1_y)) / 2.0f;
          } else if (p2_on_contour) {
            // on off on
            curve.point.x   = p0_x;
            curve.point.y   = p0_y;
            curve.control.x = p1_x;
            curve.control.y = p1_y;
            i++;
          } else {
            // on off off
            curve.point.x   = p0_x;
            curve.point.y   = p0_y;
            curve.control.x = p1_x;
            curve.control.y = p1_y;
          }
        } else {
          // TODO: could this maybe happen for the first point? if it does happen, maybe just ignore / passively increment in this case. should be handled at the end of the loop.
          DEBUG_ASSERT(!p1_on_contour);
          if (p2_on_contour) {
            // off off on
            curve.point.x   = ((F32) (p0_x + p1_x)) / 2.0f;
            curve.point.y   = ((F32) (p0_y + p1_y)) / 2.0f;
            curve.control.x = p1_x;
            curve.control.y = p1_y;
            i++;
          } else {
            // off off off
            curve.point.x   = ((F32) (p0_x + p1_x)) / 2.0f;
            curve.point.y   = ((F32) (p0_y + p1_y)) / 2.0f;
            curve.control.x = p1_x;
            curve.control.y = p1_y;
          }
        }
        // NOTE: contour.curves has exclusive arena access here, so we don't need to have a separate dedicated arena for these dynamic arrays.
        DA_PUSH_BACK_EX(arena, contour.curves, contour.curves_size, curves_capacity, curve);
      }
      // NOTE: pop dynamic array excess
      ARENA_POP_ARRAY(arena, GlyphCurve, curves_capacity - contour.curves_size);

      shape->contours[contours_idx++] = contour;
      if (contours_idx >= contours_size) { break; }
      start_of_contour = end_of_contour + 1;
      FONT_TRY_PARSE(BinStreamPeekU16BE(&end_pts_of_contours, contours_idx, sizeof(U16), &end_of_contour));
    }
  } else if (contours_size < 0) {
    // NOTE: parse compound shape

    // NOTE: determine the number of contours for the compound shape, for allocation.
    contours_size = 0;
    BinStream h_copy = s;
    while (true) {
      U16 flags, compound_glyph_index;
      FONT_TRY_PARSE(BinStreamPullU16BE(&h_copy, &flags));
      FONT_TRY_PARSE(BinStreamPullU16BE(&h_copy, &compound_glyph_index));
      if (COMPOUND_FLAG_ARE_XY_VALS(flags)) {
        if (COMPOUND_FLAG_ARE_U16(flags)) { FONT_TRY_PARSE(BinStreamSkip(&h_copy, 2, sizeof(U16))); }
        else                              { FONT_TRY_PARSE(BinStreamSkip(&h_copy, 2, sizeof(U8)));  }
      }
      if      (COMPOUND_FLAG_SCALE(flags))      { FONT_TRY_PARSE(BinStreamSkip(&h_copy, 1, sizeof(U16))); }
      else if (COMPOUND_FLAG_SEP_SCALES(flags)) { FONT_TRY_PARSE(BinStreamSkip(&h_copy, 2, sizeof(U16))); }
      else if (COMPOUND_FLAG_2X2MAT(flags))     { FONT_TRY_PARSE(BinStreamSkip(&h_copy, 4, sizeof(U16))); }

      S32 compound_glyph_glyf_offset;
      if (!FontGetGlyphGlyfOffset(font, compound_glyph_index, &compound_glyph_glyf_offset)) {
        LOG_ERROR("[FONT] Unable to retrieve glyf offset of compound shape.");
        goto font_get_glyph_shape_end;
      }

      BinStream compound_glyph_glyf_h;
      BinStreamInit(&compound_glyph_glyf_h, font->data, font->data_size);
      FONT_TRY_PARSE(BinStreamSeek(&compound_glyph_glyf_h, compound_glyph_glyf_offset));
      S16 compound_glyph_contours_size;
      FONT_TRY_PARSE(BinStreamPullS16BE(&compound_glyph_glyf_h, &compound_glyph_contours_size));
      if (compound_glyph_contours_size < 0) {
        LOG_ERROR("[FONT] Invalid state observed, compound glyph is made of additional compound glyphs.");
        goto font_get_glyph_shape_end;
      }
      contours_size += (U32) compound_glyph_contours_size;

      if (!COMPOUND_FLAG_MORE(flags)) { break; }
    }
    shape->contours      = ARENA_PUSH_ARRAY(arena, GlyphContour, contours_size);
    shape->contours_size = contours_size;

    // NOTE: determine and save the shapes for each contour.
    U16 contours_idx = 0;
    while (true) {
      U16 flags, compound_glyph_index;
      FONT_TRY_PARSE(BinStreamPullU16BE(&h_copy, &flags));
      FONT_TRY_PARSE(BinStreamPullU16BE(&h_copy, &compound_glyph_index));
      F32 matrix[6]            = { 1, 0, 0, 1, 0, 0 };
      if (COMPOUND_FLAG_ARE_XY_VALS(flags)) {
        if (COMPOUND_FLAG_ARE_U16(flags)) {
          S16 m1, m2;
          FONT_TRY_PARSE(BinStreamPullS16BE(&s, &m1));
          FONT_TRY_PARSE(BinStreamPullS16BE(&s, &m2));
          matrix[4] = m1;
          matrix[5] = m2;
        } else {
          S8 m1, m2;
          FONT_TRY_PARSE(BinStreamPullS8(&s, &m1));
          FONT_TRY_PARSE(BinStreamPullS8(&s, &m2));
          matrix[4] = m1;
          matrix[5] = m2;
        }
      } else {
        LOG_ERROR("[FONT] Defining compound glyphs by points is not supported.");
        goto font_get_glyph_shape_end;
      }
      if (COMPOUND_FLAG_SCALE(flags)) {
        S16 scale;
        FONT_TRY_PARSE(BinStreamPullS16BE(&s, &scale));
        matrix[0] = scale / 16384.0f;
        matrix[3] = scale / 16384.0f;
        matrix[1] = 0;
        matrix[2] = 0;
      } else if (COMPOUND_FLAG_SEP_SCALES(flags)) {
        S16 x_scale, y_scale;
        FONT_TRY_PARSE(BinStreamPullS16BE(&s, &x_scale));
        FONT_TRY_PARSE(BinStreamPullS16BE(&s, &y_scale));
        matrix[0] = x_scale / 16384.0f;
        matrix[3] = y_scale / 16384.0f;
        matrix[1] = 0;
        matrix[2] = 0;
      } else if (COMPOUND_FLAG_2X2MAT(flags)) {
        S16 m1, m2, m3, m4;
        FONT_TRY_PARSE(BinStreamPullS16BE(&s, &m1));
        FONT_TRY_PARSE(BinStreamPullS16BE(&s, &m2));
        FONT_TRY_PARSE(BinStreamPullS16BE(&s, &m3));
        FONT_TRY_PARSE(BinStreamPullS16BE(&s, &m4));
        matrix[0] = m1 / 16384.0f;
        matrix[1] = m2 / 16384.0f;
        matrix[2] = m3 / 16384.0f;
        matrix[3] = m4 / 16384.0f;
      }

      F32 m = F32Sqrt((matrix[0] * matrix[0]) + (matrix[1] * matrix[1]));
      F32 n = F32Sqrt((matrix[2] * matrix[2]) + (matrix[3] * matrix[3]));

      // NOTE: assertion should pass, we retrieved before when sizing the contours list.
      S32 compound_glyph_glyf_offset;
      DEBUG_ASSERT(FontGetGlyphGlyfOffset(font, compound_glyph_index, &compound_glyph_glyf_offset));

      GlyphShape component_shape;
      if (!FontGetGlyphShape(temp_arena, font, compound_glyph_glyf_offset, &component_shape)) {
        LOG_ERROR("[FONT] Failed to determine compound glyph shape.");
        goto font_get_glyph_shape_end;
      }

      // NOTE: apply transformation to the component contour and save it on self.
      if (component_shape.contours_size > 0) {
        for (U32 i = 0; i < component_shape.contours_size; i++) {
          GlyphContour contour;
          MEMORY_ZERO_STRUCT(&contour);

          U32 curves_capacity = 0;
          GlyphContour* component_contour = &component_shape.contours[i];
          for (U32 j = 0; j < component_contour->curves_size; j++) {
            GlyphCurve* curve = &component_contour->curves[j];
            curve->point.x   = m * ((matrix[0] * curve->point.x) + (matrix[2] * curve->point.y) + matrix[4]);
            curve->point.y   = n * ((matrix[1] * curve->point.x) + (matrix[3] * curve->point.y) + matrix[5]);
            curve->control.x = m * ((matrix[0] * curve->control.x) + (matrix[2] * curve->control.y) + matrix[4]);
            curve->control.y = n * ((matrix[1] * curve->control.x) + (matrix[3] * curve->control.y) + matrix[5]);
            DA_PUSH_BACK_EX(arena, contour.curves, contour.curves_size, curves_capacity, *curve);
          }
          ARENA_POP_ARRAY(arena, GlyphCurve, curves_capacity - contour.curves_size);
          shape->contours[contours_idx++] = contour;
        }
      }

      if (!COMPOUND_FLAG_MORE(flags)) { break; }
    }
    DEBUG_ASSERT(contours_idx == contours_size);
  }

  success = true;
font_get_glyph_shape_end:
  if (!success) { ArenaPopTo(arena, base_pos); }
  ArenaRelease(temp_arena);
  return success;
}
#undef FONT_TRY_PARSE

B32 FontGlyphShapeRasterize(GlyphShape* glyph_shape, U8** bmp, U32 bmp_width, U32 bmp_height, U16 bmp_glyph_min_x, U16 bmp_glyph_min_y, U16 bmp_glyph_max_x, U16 bmp_glyph_max_y) {
  if (!((0 <= bmp_glyph_min_x && bmp_glyph_min_x <= bmp_width - 1) &&
        (0 <= bmp_glyph_max_x && bmp_glyph_max_x <= bmp_width - 1) &&
        (0 <= bmp_glyph_min_y && bmp_glyph_min_y <= bmp_height - 1) &&
        (0 <= bmp_glyph_max_y && bmp_glyph_max_y <= bmp_height - 1) &&
        (bmp_glyph_min_x <= bmp_glyph_max_x) &&
        (bmp_glyph_min_y <= bmp_glyph_max_y))) {
    LOG_ERROR("[FONT] Provided glyph dimensions min: { %d, %d }, { %d, %d }, do not fit in the provided bitmap: { %d, %d }",
              bmp_glyph_min_x, bmp_glyph_min_y, bmp_glyph_max_x, bmp_glyph_max_y, bmp_width, bmp_height);
    return false;
  }

  Arena*              temp_arena                = ArenaAllocate();
  FontIntersectPoint* intersect_x_vals          = NULL;
  U32                 intersect_x_vals_size     = 0;
  U32                 intersect_x_vals_capacity = 0;

  S16 pen_y = bmp_glyph_min_y;
  while (pen_y <= bmp_glyph_max_y) {
    F32 scanline_y = F32MapRange(pen_y + 0.5f, bmp_glyph_min_y, bmp_glyph_max_y, glyph_shape->min.y, glyph_shape->max.y);

    // NOTE: find all points that intersect with the current scanline.
    intersect_x_vals_size = 0;
    for (U32 i = 0; i < glyph_shape->contours_size; i++) {
      GlyphContour* contour = &glyph_shape->contours[i];
      for (U32 j = 0; j < contour->curves_size; j++) {
        GlyphCurve* curr = &contour->curves[j];
        GlyphCurve* next = &contour->curves[(j + 1) % contour->curves_size];

        F32 a = curr->point.y - (2 * curr->control.y) + next->point.y;
        F32 b = 2 * (curr->control.y - curr->point.y);
        F32 c = curr->point.y - scanline_y;

        // NOTE: get y-extremes for the given curve, determine that the current scanline intersects based on the extents.
        F32 curve_min_y = MIN(curr->point.y, next->point.y);
        F32 curve_max_y = MAX(curr->point.y, next->point.y);
        F32 t_y_extreme = (curr->point.y - curr->control.y) / a;
        if (0 <= t_y_extreme && t_y_extreme <= 1) {
          F32 y_extreme = FontCurveEvaluateBezier(curr->point.y, next->point.y, curr->control.y, t_y_extreme);
          curve_min_y = MIN(curve_min_y, y_extreme);
          curve_max_y = MAX(curve_max_y, y_extreme);
        }
        if (scanline_y < curve_min_y || scanline_y > curve_max_y) { continue; }

        // NOTE: find Ts for x-intersections on given scanline y
        F32 t[2];
        U32 t_size = 0;
        if (F32Abs(a) > 0.00001) {
          // curve is a parabola
          F32 det = (b * b) - (4 * a * c);
          if (det > 0) {
            det = F32Sqrt(det);
            if (det == 0) {
              t[t_size++] = (-b) / (2 * a);
            } else {
              t[t_size++] = (-b + det) / (2 * a);
              t[t_size++] = (-b - det) / (2 * a);
            }
          }
        } else if (F32Abs(b) > 0.00001) {
          // curve is a line
          t[t_size++] = -c / b;
        } else {
          // curve is invalid
          continue;
        }

        // NOTE: save intersection points
        for (U32 k = 0; k < t_size; k++) {
          // NOTE: [0, 1), not [0, 1] because the 1 will be double counted at the next curve's t = 0.
          if (!(0 <= t[k] && t[k] < 1)) { continue; }
          F32 intersect_x = FontCurveEvaluateBezier(curr->point.x, next->point.x, curr->control.x, t[k]);
          if (intersect_x < glyph_shape->min.x || intersect_x > glyph_shape->max.x) {
            LOG_WARN("[FONT] Font defined glyph shape exceeds defined bounds! Attempting to recover, but there may be visual artifacts.");
            intersect_x = MAX(glyph_shape->min.x, intersect_x);
            intersect_x = MIN(glyph_shape->max.x, intersect_x);
          }
          F32 intersect_deriv = FontCurveEvaluateBezierDerivative(curr->point.y, next->point.y, curr->control.y, t[k]);
          if (intersect_deriv == 0) { intersect_deriv = FontCurveEvaluateBezierDerivative(curr->point.x, next->point.x, curr->control.x, t[k]); }

          FontIntersectPoint intersect_point;
          intersect_point.x = intersect_x;
          intersect_point.winding = (intersect_deriv > 0) ? +1 : -1;
          DA_PUSH_BACK_EX(temp_arena, intersect_x_vals, intersect_x_vals_size, intersect_x_vals_capacity, intersect_point);
        }
      }
    }
    SORT(FontIntersectPoint, intersect_x_vals, intersect_x_vals_size, SortCompareFontIntersectPointAsc);

    // NOTE: color in the current scanline. we follow the approach described in apple's ttf documentation,
    // deciding to color lines based on a running accumulated winding order. when != 0, in a shape, when = 0, not in a shape.
    F32 pen_start = 0;
    S32 winding   = 0;
    for (U32 i = 0; i < intersect_x_vals_size; i++) {
      FontIntersectPoint* intersect = &intersect_x_vals[i];
      B32 winding_was_0             = (winding == 0);
      winding                       = winding + intersect->winding;
      B32 winding_became_0          = (winding == 0);

      if (winding_was_0) {
        // NOTE: entering a shape
        pen_start = intersect->x;

      } else if (!winding_was_0 && winding_became_0) {
        // NOTE: exiting a shape
        // TODO: this doesn't sample from the bmp's center point.
        U16 bmp_pen_start = F32MapRange(pen_start,    glyph_shape->min.x, glyph_shape->max.x, bmp_glyph_min_x, bmp_glyph_max_x);
        U16 bmp_pen_stop  = F32MapRange(intersect->x, glyph_shape->min.x, glyph_shape->max.x, bmp_glyph_min_x, bmp_glyph_max_x);
        for (U16 j = bmp_pen_start; j <= bmp_pen_stop; j++) { (*bmp)[(pen_y * bmp_width) + j] = 255; }
      }
    }

    pen_y += 1;
  }
  ArenaRelease(temp_arena);
  return true;
}

B32 FontAtlasBakeBitmap(Arena* atlas_arena, Arena* bitmap_arena, Font* font, FontAtlas* atlas, Image* bitmap, F32 pixel_height, FontCharSet* char_set) {
  B32 success  = false;
  U64 atlas_arena_base_pos = ArenaPos(atlas_arena);
  U64 image_arena_base_pos = ArenaPos(bitmap_arena);
  Arena* temp_arena = ArenaAllocate();

  MEMORY_ZERO_STRUCT(atlas);
  MEMORY_ZERO_STRUCT(bitmap);
  bitmap->format = ImageFormat_R;

  S16 font_size = font->ascent + font->descent;
  F32 scale = pixel_height / font_size;
  if (scale <= 0) {
    LOG_ERROR("[FONT] Invalid scale: %0.5f determined from provided desired pixel height: %0.5f", scale, pixel_height);
    goto font_atlas_bake_bitmap_end;
  }

  U16 pad = 2;
  if (!FontSuggestAtlasSize(font, char_set, scale, &bitmap->width, &bitmap->height, pad * 2, pad * 2)) {
    goto font_atlas_bake_bitmap_end;
  }
  bitmap->data = ARENA_PUSH_ARRAY(bitmap_arena, U8, bitmap->width * bitmap->height);
  MEMORY_ZERO_ARRAY(bitmap->data, bitmap->width * bitmap->height);

  // NOTE: bake glyph bitmap
  U32 bitmap_x = pad;
  U32 bitmap_y = pad;
  U16 max_glyph_height_for_row = 0;
  for (FontCharSet* curr_char_set = char_set; curr_char_set != NULL; curr_char_set = curr_char_set->next) {
    for (U32 i = 0; i < curr_char_set->codepoints_size; i++) {
      U32 codepoint = curr_char_set->codepoints[i];
      if (FontAtlasGetChar(atlas, codepoint) != NULL) { continue; };
      ArenaClear(temp_arena);

      U32 glyph_index;
      if (!FontGetGlyphIndex(font, codepoint, &glyph_index)) {
        LOG_ERROR("[FONT] Failed to retrieve glyph index for codepoint: %d", codepoint);
        goto font_atlas_bake_bitmap_end;
      }

      S32 advance, left_side_bearing;
      if (!FontGetGlyphSizeInfo(font, glyph_index, &advance, &left_side_bearing)) {
        LOG_ERROR("[FONT] Failed to determine glyph size info for codepoint: %d", codepoint);
        goto font_atlas_bake_bitmap_end;
      }

      S32 glyph_glyf_offset;
      if (!FontGetGlyphGlyfOffset(font, glyph_index, &glyph_glyf_offset)) {
        LOG_ERROR("[FONT] Failed to get glyf offset for codepoint: %d", codepoint);
        goto font_atlas_bake_bitmap_end;
      }

      GlyphShape glyph_shape;
      if (!FontGetGlyphShape(temp_arena, font, glyph_glyf_offset, &glyph_shape)) {
        LOG_ERROR("[FONT] Failed to determine glyph shape for codepoint: %d.", codepoint);
        goto font_atlas_bake_bitmap_end;
      }

      // NOTE: determine bitmap box
      V2 scaled_glyph_min, scaled_glyph_max;
      FontBboxScale(&glyph_shape.min, &glyph_shape.max, &scaled_glyph_min, &scaled_glyph_max, scale, V2Assign(0, 0));
      F32 scaled_glyph_width  = scaled_glyph_max.x - scaled_glyph_min.x;
      F32 scaled_glyph_height = scaled_glyph_max.y - scaled_glyph_min.y;
      if (bitmap_x + scaled_glyph_width >= bitmap->width) {
        bitmap_x = pad;
        bitmap_y += max_glyph_height_for_row;
        max_glyph_height_for_row = pad;
      }
      if (bitmap_y + scaled_glyph_height >= bitmap->height) {
        LOG_ERROR("[FONT] Provided font SDF bitmap does not fit all glyphs.");
        goto font_atlas_bake_bitmap_end;
      }
      max_glyph_height_for_row = MAX(scaled_glyph_height + pad, max_glyph_height_for_row);

      U16 bitmap_min_x = bitmap_x;
      U16 bitmap_min_y = bitmap_y;
      U16 bitmap_max_x = bitmap_x + scaled_glyph_width;
      U16 bitmap_max_y = bitmap_y + scaled_glyph_height;
      DEBUG_ASSERT(FontGlyphShapeRasterize(&glyph_shape, &bitmap->data, bitmap->width, bitmap->height, bitmap_min_x, bitmap_min_y, bitmap_max_x, bitmap_max_y));

      AtlasChar* atlas_char = ARENA_PUSH_STRUCT(atlas_arena, AtlasChar);
      atlas_char->advance   = ((F32) advance) * scale;
      V2MultF32(&atlas_char->offset, &glyph_shape.min, scale);
      atlas_char->size.x    = bitmap_max_x - bitmap_min_x;
      atlas_char->size.y    = bitmap_max_y - bitmap_min_y;
      atlas_char->uv_min.x  = ((F32) bitmap_min_x) / bitmap->width;
      atlas_char->uv_min.y  = ((F32) bitmap_min_y) / bitmap->height;
      atlas_char->uv_max.x  = ((F32) bitmap_max_x) / bitmap->width;
      atlas_char->uv_max.y  = ((F32) bitmap_max_y) / bitmap->height;
      atlas_char->codepoint = codepoint;
      DEBUG_ASSERT(FontAtlasInsertChar(atlas, codepoint, atlas_char));

      bitmap_x += scaled_glyph_width + pad;
    }
  }

  if (!FontAtlasBuildKernTable(atlas_arena, font, atlas, scale, char_set)) { goto font_atlas_bake_bitmap_end; }
  atlas->scale_coeff = 1.0f / pixel_height;
  atlas->ascent      = font->ascent * scale;
  atlas->descent     = font->descent * scale;

  success = true;
font_atlas_bake_bitmap_end:
  ArenaRelease(temp_arena);
  if (!success) {
    ArenaPopTo(atlas_arena, atlas_arena_base_pos);
    ArenaPopTo(bitmap_arena, image_arena_base_pos);
  }
  return success;
}

// https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
B32 FontAtlasBakeSdf(Arena* atlas_arena, Arena* bitmap_arena, Font* font, FontAtlas* atlas, Image* bitmap, F32 bmp_pixel_height, F32 sdf_pixel_height, F32 spread_factor, FontCharSet* char_set) {
  B32 success = false;
  Arena* temp_arena = ArenaAllocate();
  U64 atlas_arena_base_pos = ArenaPos(atlas_arena);
  U64 image_arena_base_pos = ArenaPos(bitmap_arena);

  MEMORY_ZERO_STRUCT(atlas);
  MEMORY_ZERO_STRUCT(bitmap);
  bitmap->format = ImageFormat_R;

  DEBUG_ASSERT(bmp_pixel_height >= 0);
  if (bmp_pixel_height == 0) { bmp_pixel_height = 64.0f; }

  DEBUG_ASSERT(sdf_pixel_height >= 0);
  if (sdf_pixel_height == 0) { sdf_pixel_height = 32.0f; }

  DEBUG_ASSERT(spread_factor >= 0);
  if (spread_factor == 0) { spread_factor = 4.0f; }

  S16 font_size = font->ascent + font->descent;
  F32 bmp_scale = bmp_pixel_height / font_size;
  if (bmp_scale <= 0) {
    LOG_ERROR("[FONT] Invalid bitmap scale: %0.5f determined from provided desired pixel height: %0.5f", bmp_scale, bmp_pixel_height);
    goto font_atlas_bake_sdf_end;
  }
  F32 sdf_scale = sdf_pixel_height / font_size;
  if (sdf_scale <= 0) {
    LOG_ERROR("[FONT] Invalid SDF scale: %0.5f determined from provided desired pixel height: %0.5f", sdf_scale, sdf_pixel_height);
    goto font_atlas_bake_sdf_end;
  }

  V2 sdf_pad;
  V2 sdf_kernel = V2Assign(spread_factor, spread_factor);
  V2MultF32(&sdf_pad, &sdf_kernel, sdf_scale / bmp_scale);
  F32 max_possible_distance = F32Sqrt((sdf_kernel.x * sdf_kernel.x) + (sdf_kernel.y * sdf_kernel.y));

  if (!FontSuggestAtlasSize(font, char_set, sdf_scale, &bitmap->width, &bitmap->height, 2.0f * sdf_pad.x, 2.0f * sdf_pad.y)) {
    goto font_atlas_bake_sdf_end;
  }
  bitmap->data = ARENA_PUSH_ARRAY(bitmap_arena, U8, bitmap->width * bitmap->height);
  MEMORY_ZERO_ARRAY(bitmap->data, bitmap->width * bitmap->height);

  U32 sdf_y = sdf_pad.y;
  U32 sdf_x = sdf_pad.x;
  S16 max_glyph_height_for_row = 0;
  for (FontCharSet* curr_char_set = char_set; curr_char_set != NULL; curr_char_set = curr_char_set->next) {
    for (U32 i = 0; i < curr_char_set->codepoints_size; i++) {
      U32 codepoint = curr_char_set->codepoints[i];
      if (FontAtlasGetChar(atlas, codepoint) != NULL) { continue; };
      ArenaClear(temp_arena);

      U32 glyph_index;
      if (!FontGetGlyphIndex(font, codepoint, &glyph_index)) {
        LOG_ERROR("[FONT] Failed to retrieve glyph index for codepoint: %d", codepoint);
        goto font_atlas_bake_sdf_end;
      }

      S32 advance, left_side_bearing;
      if (!FontGetGlyphSizeInfo(font, glyph_index, &advance, &left_side_bearing)) {
        LOG_ERROR("[FONT] Failed to determine glyph size info for codepoint: %d", codepoint);
        goto font_atlas_bake_sdf_end;
      }

      S32 glyph_glyf_offset;
      if (!FontGetGlyphGlyfOffset(font, glyph_index, &glyph_glyf_offset)) {
        LOG_ERROR("[FONT] Failed to get glyf offset for codepoint: %d", codepoint);
        goto font_atlas_bake_sdf_end;
      }

      GlyphShape glyph_shape;
      if (!FontGetGlyphShape(temp_arena, font, glyph_glyf_offset, &glyph_shape)) {
        LOG_ERROR("[FONT] Failed to determine glyph shape for codepoint: %d.", codepoint);
        goto font_atlas_bake_sdf_end;
      }

      // NOTE: we generate an SDF by first rasterizing a bitmap, then for each pixel in the SDF bitmap, determine its
      // comparative distance to a pixel of the opposite state in the rasterized bitmap.
      V2 bmp_glyph_min, bmp_glyph_max;
      FontBboxScale(&glyph_shape.min, &glyph_shape.max, &bmp_glyph_min, &bmp_glyph_max, bmp_scale, sdf_kernel);
      U16 bmp_glyph_width  = bmp_glyph_max.x - bmp_glyph_min.x;
      U16 bmp_glyph_height = bmp_glyph_max.y - bmp_glyph_min.y;
      U8* bmp_glyph_bytes  = ARENA_PUSH_ARRAY(temp_arena, U8, bmp_glyph_width * bmp_glyph_height);
      MEMORY_ZERO_ARRAY(bmp_glyph_bytes, bmp_glyph_width * bmp_glyph_height);
      DEBUG_ASSERT(FontGlyphShapeRasterize(
            &glyph_shape, &bmp_glyph_bytes, bmp_glyph_width, bmp_glyph_height,
            sdf_kernel.x, sdf_kernel.y, bmp_glyph_width - sdf_kernel.x, bmp_glyph_height - sdf_kernel.y));

      V2 sdf_glyph_min, sdf_glyph_max;
      FontBboxScale(&glyph_shape.min, &glyph_shape.max, &sdf_glyph_min, &sdf_glyph_max, sdf_scale, sdf_pad);
      U16 sdf_glyph_width  = sdf_glyph_max.x - sdf_glyph_min.x;
      U16 sdf_glyph_height = sdf_glyph_max.y - sdf_glyph_min.y;
      if (sdf_x + sdf_glyph_width >= bitmap->width) {
        sdf_x = sdf_pad.x;
        sdf_y += max_glyph_height_for_row;
      }
      if (sdf_y + sdf_glyph_height >= bitmap->height) {
        LOG_ERROR("[FONT] Provided font SDF bitmap does not fit all glyphs.");
        goto font_atlas_bake_sdf_end;
      }
      max_glyph_height_for_row = MAX(sdf_glyph_height + sdf_pad.y, max_glyph_height_for_row);

      U16 sdf_bitmap_min_x = sdf_x;
      U16 sdf_bitmap_min_y = sdf_y;
      U16 sdf_bitmap_max_x = sdf_x + sdf_glyph_width;
      U16 sdf_bitmap_max_y = sdf_y + sdf_glyph_height;

      for (S16 sdf_glyph_y = 0; sdf_glyph_y < sdf_glyph_height; sdf_glyph_y++) {
        S16 bmp_glyph_y = F32MapRange(sdf_glyph_y + 0.5f, 0, sdf_glyph_height, 0, bmp_glyph_height);
        for (S16 sdf_glyph_x = 0; sdf_glyph_x < sdf_glyph_width; sdf_glyph_x++) {
          S16 bmp_glyph_x = F32MapRange(sdf_glyph_x + 0.5f, 0, sdf_glyph_width, 0, bmp_glyph_width);
          U8 value = bmp_glyph_bytes[(bmp_glyph_y * bmp_glyph_width) + bmp_glyph_x];

          // NOTE: determine distance to closest edge by looking for the closest pixel of opposite state in a neighborhood of texels.
          F32 min_distance_sq = max_possible_distance * max_possible_distance;
          S16 kernel_min_y = MAX(0, bmp_glyph_y - sdf_kernel.y);
          S16 kernel_max_y = MIN(bmp_glyph_height, bmp_glyph_y + sdf_kernel.y);
          for (S16 kernel_y = kernel_min_y; kernel_y < kernel_max_y; kernel_y++) {
            S16 kernel_min_x = MAX(0, bmp_glyph_x - sdf_kernel.x);
            S16 kernel_max_x = MIN(bmp_glyph_width, bmp_glyph_x + sdf_kernel.x);
            for (S16 kernel_x = kernel_min_x; kernel_x < kernel_max_x; kernel_x++) {
              U8 test_value = bmp_glyph_bytes[(kernel_y * bmp_glyph_width) + kernel_x];
              if (test_value == value) { continue; }
              F32 test_x_distance  = kernel_x - bmp_glyph_x;
              F32 test_y_distance  = kernel_y - bmp_glyph_y;
              F32 test_distance_sq = (test_x_distance * test_x_distance) + (test_y_distance * test_y_distance);
              if (test_distance_sq < min_distance_sq) { min_distance_sq = test_distance_sq; }
            }
          }

          // NOTE: determine final sdf value (- if inside shape, in range [0, 255] w/ edge at 127)
          F32 sdf_value = F32Sqrt(min_distance_sq);
          if (value > 0) { sdf_value *= -1; }
          sdf_value = F32MapRange(sdf_value, -max_possible_distance, max_possible_distance, 0, 255);
          // NOTE: flip it so the inside of the shape is close to 255, outside is close to 0.
          // this is so the outside of the shape is the same color as the atlas bitmap (black), which could otherwise confuse
          // linear interpolation when sampling from the sdf atlas (mostly due to uv float precision lost when picking atlas chars).
          sdf_value = 255 - sdf_value;
          bitmap->data[((sdf_y + sdf_glyph_y) * bitmap->width) + (sdf_x + sdf_glyph_x)] = (U8) sdf_value;
        }
      }

      AtlasChar* atlas_char = ARENA_PUSH_STRUCT(atlas_arena, AtlasChar);
      atlas_char->advance  = ((F32) advance) * sdf_scale;
      V2MultF32(&atlas_char->offset, &glyph_shape.min, sdf_scale);
      V2SubV2(&atlas_char->offset, &atlas_char->offset, &sdf_pad);
      atlas_char->size.x   = sdf_bitmap_max_x - sdf_bitmap_min_x;
      atlas_char->size.y   = sdf_bitmap_max_y - sdf_bitmap_min_y;
      atlas_char->uv_min.x = ((F32) sdf_bitmap_min_x) / bitmap->width;
      atlas_char->uv_min.y = ((F32) sdf_bitmap_min_y) / bitmap->height;
      atlas_char->uv_max.x = ((F32) sdf_bitmap_max_x) / bitmap->width;
      atlas_char->uv_max.y = ((F32) sdf_bitmap_max_y) / bitmap->height;

      atlas_char->codepoint = codepoint;
      DEBUG_ASSERT(FontAtlasInsertChar(atlas, atlas_char->codepoint, atlas_char));

      sdf_x += sdf_glyph_width;
    }
  }

  if (!FontAtlasBuildKernTable(atlas_arena, font, atlas, sdf_scale, char_set)) { goto font_atlas_bake_sdf_end; }
  atlas->scale_coeff = 1.0f / sdf_pixel_height;
  atlas->ascent      = font->ascent * sdf_scale;
  atlas->descent     = font->descent * sdf_scale;

  success = true;
font_atlas_bake_sdf_end:
  if (!success) {
    ArenaPopTo(atlas_arena, atlas_arena_base_pos);
    ArenaPopTo(bitmap_arena, image_arena_base_pos);
  }
  ArenaRelease(temp_arena);
  return success;
}

static U32 FontAtlasHash(U32 key) {
  // fast bit mixer
  U32 hash = key;
  hash ^= hash >> 16;
  hash *= 0x7feb352d;
  hash ^= hash >> 15;
  hash *= 0x846ca68b;
  hash ^= hash >> 16;
  return hash;
}

B32 FontAtlasInsertChar(FontAtlas* atlas, U32 codepoint, AtlasChar* atlas_char) {
  if(FontAtlasGetChar(atlas, codepoint) != NULL) { return false; }
  U32 key_index = FontAtlasHash(codepoint) % STATIC_ARRAY_SIZE(atlas->char_map);
  AtlasChar** curr = &atlas->char_map[key_index];
  while (*curr != NULL) { curr = &(*curr)->next; }
  *curr = atlas_char;
  return true;
}

AtlasChar* FontAtlasGetChar(FontAtlas* atlas, U32 codepoint) {
  U32 key_index = FontAtlasHash(codepoint) % STATIC_ARRAY_SIZE(atlas->char_map);
  AtlasChar* curr = atlas->char_map[key_index];
  while (curr != NULL) {
    if (curr->codepoint == codepoint) { return curr; }
    curr = curr->next;
  }
  return NULL;
}

B32 FontAtlasInsertKern(FontAtlas* atlas, U32 codepoint_left, U32 codepoint_right, GlyphKernInfo* kern) {
  if (FontAtlasGetKern(atlas, codepoint_left, codepoint_right) != NULL) { return false; }
  U32 key_index = FontAtlasHash(codepoint_left << 16 | codepoint_right) % STATIC_ARRAY_SIZE(atlas->kern_map);;
  GlyphKernInfo** curr = &atlas->kern_map[key_index];
  while (*curr != NULL) { curr = &(*curr)->next; }
  *curr = kern;
  return true;
}

GlyphKernInfo* FontAtlasGetKern(FontAtlas* atlas, U32 codepoint_left, U32 codepoint_right) {
  U32 key_index = FontAtlasHash(codepoint_left << 16 | codepoint_right) % STATIC_ARRAY_SIZE(atlas->kern_map);;
  GlyphKernInfo* curr = atlas->kern_map[key_index];
  while (curr != NULL) {
    if (curr->codepoint_left == codepoint_left && curr->codepoint_right == codepoint_right) { return curr; }
    curr = curr->next;
  }
  return NULL;
}

B32 FontAtlasPlace(FontAtlas* atlas, U32 codepoint, U32 codepoint_next, F32 pixel_height, V2* cursor, V2* center, V2* size, V2* uv_min, V2* uv_max) {
  AtlasChar* atlas_char = FontAtlasGetChar(atlas, codepoint);
  if (atlas_char == NULL) { return false; }
  F32 scale = pixel_height * atlas->scale_coeff;
  V2MultF32(size, &atlas_char->size, scale);
  center->x = cursor->x + (atlas_char->offset.x * scale) + (size->x / 2.0f);
  center->y = cursor->y + (atlas_char->offset.y * scale) + (size->y / 2.0f);
  *uv_min   = atlas_char->uv_min;
  *uv_max   = atlas_char->uv_max;

  F32 advance = atlas_char->advance;
  GlyphKernInfo* kern = FontAtlasGetKern(atlas, codepoint, codepoint_next);
  if (kern != NULL) { advance += kern->advance; }
  cursor->x += advance * scale;

  return true;
}

B32 FontAtlasMeasureString(FontAtlas* atlas, F32 pixel_height, String8 str, V2* size) {
  V2 cursor = V2_ZEROES;
  for (S32 i = 0; i < str.size; i++) {
    U8 curr = str.str[i];
    U8 next = i < str.size - 1 ? str.str[i + 1] : 0;
    V2 center, char_size, min_uv, max_uv;
    if (!FontAtlasPlace(atlas, curr, next, pixel_height, &cursor, &center, &char_size, &min_uv, &max_uv)) { return false; }
  }
  size->x = cursor.x;
  size->y = pixel_height;
  return true;
}

void FontAtlasGetAttributes(FontAtlas* atlas, F32 pixel_height, F32* ascent, F32* descent) {
  F32 scale = pixel_height * atlas->scale_coeff;
  if (ascent != NULL)  { *ascent   = atlas->ascent * scale;  }
  if (descent != NULL) { *descent  = atlas->descent * scale; }
}

FontCharSet* FontCharSetConcat(FontCharSet* set, FontCharSet* to_concat) {
  FontCharSet* curr = set;
  while (curr->next != NULL) { curr = curr->next; }
  curr->next = to_concat;
  return set;
}

FontCharSet* FontCharSetLatin() {
  static U32 latin_characters['~' - ' ' + 1];
  for (U32 c = ' '; c <= '~'; c++) { latin_characters[c - ' '] = c; }
  static FontCharSet latin_char_set = { latin_characters, STATIC_ARRAY_SIZE(latin_characters), NULL };
  return &latin_char_set;
}

#undef FONT_PID_UNICODE_ENCODING
#undef FONT_PID_MICROSOFT_ENCODING
#undef FONT_MICROSOFT_EID_UNICODE_BMP
#undef FONT_MICROSOFT_EID_UNICODE_FULL
#undef FLAG_ON_CURVE
#undef FLAG_X_U8
#undef FLAG_Y_U8
#undef FLAG_REPEAT
#undef FLAG_X_U8_SIGN_POS
#undef FLAG_X_U16_DELTA
#undef FLAG_Y_U8_SIGN_POS
#undef FLAG_Y_U16_DELTA
#undef COMPOUND_FLAG_ARE_U16
#undef COMPOUND_FLAG_ARE_XY_VALS
#undef COMPOUND_FLAG_SCALE
#undef COMPOUND_FLAG_MORE
#undef COMPOUND_FLAG_SEP_SCALES
#undef COMPOUND_FLAG_2X2MAT

#endif // CDEFAULT_FONT_IMPLEMENTATION
