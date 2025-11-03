#ifndef CDEFAULT_FONT_H_
#define CDEFAULT_FONT_H_

#include "cdefault_std.h"
#include "cdefault_math.h"

// TODO: automatic atlas sizing
// TODO: compound glyphs
// TODO: iron out bugs with rasterizing
// TODO: antialias edges

typedef struct GlyphCurve GlyphCurve;
struct GlyphCurve {
  S16 start_x;
  S16 start_y;
  S16 control_x;
  S16 control_y;
};

// NOTE: A glyph contour is a series of quadratic bezier curves describing a single line of the glyph.
// NOTE: As GlyphCurves are defined, each curve's end point is inferred to be the sequential curve's start point.
typedef struct GlyphContour GlyphContour;
struct GlyphContour {
  GlyphCurve* curves;
  U32 curves_size;
};

// NOTE: A glyph's shape is a collection of contours / lines describing the glyph outline (in glyph space, straight from the ttf file).
typedef struct GlyphShape GlyphShape;
struct GlyphShape {
  GlyphContour* contours;
  U32 contours_size;
  S16 min_x;
  S16 min_y;
  S16 max_x;
  S16 max_y;
};

typedef struct BakedChar BakedChar;
struct BakedChar {
  S16 min_x;
  S16 min_y;
  S16 max_x;
  S16 max_y;
  F32 advance;
  F32 offset_x;
  F32 offset_y;
  U32 codepoint;
  BakedChar* next;
};

// TODO: do we really need to store the bitmap here?
// can free after registering with the renderer, so it's wasted space.
typedef struct FontAtlas FontAtlas;
struct FontAtlas {
  U32 bitmap_width;
  U32 bitmap_height;
  U8* bitmap;
  BakedChar* character_map[256];
};

typedef enum LocFormat LocFormat;
enum LocFormat {
  LocFormat_U16,
  LocFormat_U32,
};

typedef struct FontCharSet FontCharSet;
struct FontCharSet {
  U32* codepoints;
  U32  codepoints_size;
  FontCharSet* next;
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
  U32 cmap_subtable_offset;
  U16 num_glyphs;
  LocFormat loc_format;
};

B32 FontInit(Font* font, U8* data, U32 data_size);
B32 FontAtlasBake(Arena* arena, Font* font, FontAtlas* atlas, FontCharSet* char_set, F32 pixel_height, U32 atlas_width, U32 atlas_height);
B32 FontAtlasPlace(FontAtlas* atlas, U32 codepoint, V2* cursor, V2* center, V2* size, V2* min_uv, V2* max_uv);

void FontCharSetConcat(FontCharSet* set, FontCharSet* to_concat);
FontCharSet* FontCharSetLatin();

// NOTE: determines the index of the glyph into the various font tables.
B32 FontGetGlyphIndex(Font* font, U32 codepoint, U32* glyph_index);
B32 FontGetGlyphSizeInfo(Font* font, U32 glyph_index, S32* advance, S32* left_side_bearing);
// NOTE: determines the offset of the glyph from the glyf table.
// returns -1 e.g. if there is no entry in the glyf table (true for e.g. ' ').
S32 FontGetGlyphGlyfOffset(Font* font, U32 glyph_index);
B32 FontGetGlyphShape(Arena* arena, Font* font, S32 glyph_glyf_offset, GlyphShape* shape);

// NOTE: returns false if codepoint already has a baked_char represented.
B32 FontAtlasInsert(FontAtlas* atlas, U32 codepoint, BakedChar* baked_char);
BakedChar* FontAtlasGet(FontAtlas* atlas, U32 codepoint);

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

typedef struct FontVertex FontVertex;
struct FontVertex {
  U8 flags;
  S16 x;
  S16 y;
};

// NOTE: evaluates quadratic bezier lerps for a single dimension.
static F32 FontCurveEvaluateBezier(F32 start, F32 end, F32 control, F32 t) {
  F32 start_to_control = F32Lerp(start, control, t);
  F32 control_to_end   = F32Lerp(control, end, t);
  return F32Lerp(start_to_control, control_to_end, t);
}

// NOTE: maps a_value from a_range to b_range.
static F32 FontMapValue(F32 a_value, F32 a_min, F32 a_max, F32 b_min, F32 b_max) {
  return b_min + ((a_value - a_min) / (a_max - a_min)) * (b_max - b_min);
}

static void FontTableParse(U32* table_offset, BinHead* h) {
  BinHeadR32BE(h);
  *table_offset = BinHeadR32BE(h);
  BinHeadR32BE(h);
}

B32 FontInit(Font* font, U8* data, U32 data_size) {
  MEMORY_ZERO_STRUCT(font);
  font->data = data;
  font->data_size = data_size;

  BinHead h;
  BinHeadInit(&h, data, data_size);

  // NOTE: parse header
  BinHeadR32BE(&h);
  U16 num_tables = BinHeadR16BE(&h);
  BinHeadSkip(&h, 6, sizeof(U8));

  // NOTE: parse top-level tables
  U32 cmap_offset = 0;
  U32 maxp_offset = 0;
  for (U32 i = 0; i < num_tables; i++) {
    String8 tag = Str8(BinHeadDecay(&h), 4);
    BinHeadSkip(&h, 4, sizeof(U8));
    if      (Str8Eq(tag, Str8Lit("cmap"))) { FontTableParse(&cmap_offset, &h);       }
    else if (Str8Eq(tag, Str8Lit("maxp"))) { FontTableParse(&maxp_offset, &h);       }
    else if (Str8Eq(tag, Str8Lit("loca"))) { FontTableParse(&font->loca_offset, &h); }
    else if (Str8Eq(tag, Str8Lit("head"))) { FontTableParse(&font->head_offset, &h); }
    else if (Str8Eq(tag, Str8Lit("glyf"))) { FontTableParse(&font->glyf_offset, &h); }
    else if (Str8Eq(tag, Str8Lit("hhea"))) { FontTableParse(&font->hhea_offset, &h); }
    else if (Str8Eq(tag, Str8Lit("hmtx"))) { FontTableParse(&font->hmtx_offset, &h); }
    else                                   { BinHeadSkip(&h, 12, sizeof(U8));        }
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

  // NOTE: find cmap encoding index
  BinHeadSetPos(&h, cmap_offset);
  BinHeadSkip(&h, 2, sizeof(U8));
  U16 num_cmap_subtables = BinHeadR16BE(&h);
  for (U16 i = 0; i < num_cmap_subtables; i++) {
    U16 plat_id      = BinHeadR16BE(&h);
    U16 plat_spec_id = BinHeadR16BE(&h);
    U32 index_offset = BinHeadR32BE(&h);
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

  BinHeadSetPos(&h, font->head_offset);
  BinHeadSkip(&h, 50, sizeof(U8));
  U16 index_to_loc_format = BinHeadR16BE(&h);
  switch (index_to_loc_format) {
    case 0: { font->loc_format = LocFormat_U16; } break;
    case 1: { font->loc_format = LocFormat_U32;  } break;
    default: {
      LOG_ERROR("[FONT] Invalid or unsupported indexToLocFormat provided: %d", index_to_loc_format);
      return false;
    } break;
  }

  BinHeadSetPos(&h, maxp_offset);
  BinHeadSkip(&h, 4, sizeof(U8));
  font->num_glyphs = BinHeadR16BE(&h);

  return true;
}

B32 FontGetGlyphIndex(Font* font, U32 codepoint, U32* glyph_index) {
  *glyph_index = 0;
  BinHead h;
  BinHeadInit(&h, font->data, font->data_size);
  BinHeadSetPos(&h, font->cmap_subtable_offset);
  U16 cmap_format = BinHeadR16BE(&h);
  switch (cmap_format) {
    case 0: {
      U16 length = BinHeadR16BE(&h);
      if (codepoint >= length) {
        LOG_ERROR("[FONT] Unable to determine codepoint glyph index, codepoint exceeds table length: %d, %d", codepoint, length);
        return false;
      }
      BinHeadSkip(&h, 2, sizeof(U8));
      BinHeadSkip(&h, codepoint, sizeof(U8));
      *glyph_index = BinHeadR8(&h);
      return true;
    } break;
    case 12:
    case 13: {
      BinHeadSkip(&h, 10, sizeof(U8));
      U32 number_groups = BinHeadR32BE(&h);
      U8* groups = BinHeadDecay(&h);
      S32 low  = 0;
      S32 high = number_groups;
      while (low < high ) {
        S32 mid            = low + ((high - low) / 2);
        U8* mid_group      = groups + (mid * 12);
        U32 mid_start_char = ReadU32BE(mid_group);
        U32 mid_end_char   = ReadU32BE(mid_group + 4);
        if      (codepoint < mid_start_char) { high = mid;    }
        else if (codepoint > mid_end_char)   { low = mid + 1; }
        else {
          U32 start_glyph = ReadU32BE(groups + ((mid * 12) + 8));
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
        LOG_ERROR("[FONT] Unable to find character group / glyph index for codepoint: %d", codepoint);
        return false;
      }
      return true;
    } break;
  }
  TODO();
  LOG_ERROR("[FONT] Unimplemented cmap subtable format encountered: %d", cmap_format);
  return false;
}

S32 FontGetGlyphGlyfOffset(Font* font, U32 glyph_index) {
  if (glyph_index > font->num_glyphs) {
    LOG_ERROR("[FONT] Provided glyph index exceeds the number of glyphs: %d, %d", glyph_index, font->num_glyphs);
    return -1;
  }

  U32 glyf_offset      = 0;
  U32 next_glyf_offset = 0;
  BinHead h;
  BinHeadInit(&h, font->data, font->data_size);
  BinHeadSetPos(&h, font->loca_offset);
  switch (font->loc_format) {
    case LocFormat_U16: {
      BinHeadSkip(&h, 2 * glyph_index, sizeof(U8));
      glyf_offset      = font->glyf_offset + BinHeadR16BE(&h) * 2;
      next_glyf_offset = font->glyf_offset + BinHeadR16BE(&h) * 2;
    } break;
    case LocFormat_U32: {
      BinHeadSkip(&h, 4 * glyph_index, sizeof(U8));
      glyf_offset      = font->glyf_offset + BinHeadR32BE(&h);
      next_glyf_offset = font->glyf_offset + BinHeadR32BE(&h);
    } break;
    default: UNREACHABLE();
  }

  // NOTE: length is implied from offset_next - offset_curr.
  U32 glyf_length = next_glyf_offset - glyf_offset;
  if (glyf_length == 0) { return -1; }
  return glyf_offset;
}

B32 FontGetGlyphSizeInfo(Font* font, U32 glyph_index, S32* advance, S32* left_side_bearing) {
  *advance = 0;
  *left_side_bearing = 0;
  if (glyph_index > font->num_glyphs) {
    LOG_ERROR("[FONT] Provided glyph index exceeds the number of glyphs: %d, %d", glyph_index, font->num_glyphs);
    return false;
  }

  BinHead h;
  BinHeadInit(&h, font->data, font->data_size);
  BinHeadSetPos(&h, font->hhea_offset);
  BinHeadSkip(&h, 34, sizeof(U8));
  U16 num_long_hor_metrics = BinHeadR16BE(&h);
  if (glyph_index < num_long_hor_metrics) {
    BinHeadSetPos(&h, font->hmtx_offset);
    BinHeadSkip(&h, 4 * glyph_index, sizeof(U8));
    *advance           = BinHeadR16BE(&h);
    *left_side_bearing = BinHeadR16BE(&h);

  } else {
    // NOTE: e.g. for monospaced fonts
    BinHeadSetPos(&h, font->hmtx_offset);
    BinHeadSkip(&h, 4 * (num_long_hor_metrics - 1), sizeof(U8));
    *advance = BinHeadR16BE(&h);

    BinHeadSetPos(&h, font->hmtx_offset);
    BinHeadSkip(&h, 4 * num_long_hor_metrics, sizeof(U8));
    BinHeadSkip(&h, 2 * (glyph_index - num_long_hor_metrics), sizeof(U8));
    *left_side_bearing = BinHeadR16BE(&h);
  }
  return true;
}

// TODO: validations or return void.
B32 FontGetGlyphShape(Arena* arena, Font* font, S32 glyph_glyf_offset, GlyphShape* shape) {
  MEMORY_ZERO_STRUCT(shape);
  if (glyph_glyf_offset < 0) { return true; }

  Arena* temp_arena = ArenaAllocate();

  BinHead h;
  BinHeadInit(&h, font->data, font->data_size);
  BinHeadSetPos(&h, glyph_glyf_offset);
  S16 contours_size = BinHeadR16BE(&h);
  shape->min_x = BinHeadR16BE(&h);
  shape->min_y = BinHeadR16BE(&h);
  shape->max_x = BinHeadR16BE(&h);
  shape->max_y = BinHeadR16BE(&h);

  if (contours_size > 0) {
    // NOTE: parse simple shape
    shape->contours      = ARENA_PUSH_ARRAY(arena, GlyphContour, contours_size);
    shape->contours_size = contours_size;

    U8* end_pts_of_contours = BinHeadDecay(&h);
    BinHeadSkip(&h, 2 * contours_size, sizeof(U8));
    U16 instructions_length = BinHeadR16BE(&h);
    BinHeadSkip(&h, instructions_length, sizeof(U8));
    U8* points = BinHeadDecay(&h);

    U16 num_vertices = ReadU16BE(end_pts_of_contours + (2 * (contours_size - 1))) + 1;
    FontVertex* vertices = ARENA_PUSH_ARRAY(temp_arena, FontVertex, num_vertices);

    U8 flags = 0;
    U16 flag_num_repeat = 0;
    for (U16 j = 0; j < num_vertices; j++) {
      if (flag_num_repeat == 0) {
        flags = ReadU8(points);
        points++;
        if (flags & BIT(3)) {
          flag_num_repeat = ReadU8(points);
          points++;
        }
      } else {
        flag_num_repeat--;
      }
      vertices[j].flags = flags;
    }
    DEBUG_ASSERT(flag_num_repeat == 0);

    S16 x = 0;
    for (U16 j = 0; j < num_vertices; j++) {
      U8 flags = vertices[j].flags;
      if (flags & BIT(1)) {
        S16 dx = ReadU8(points);
        points++;
        if (flags & BIT(4)) { x += dx; }
        else                { x -= dx; }
      } else {
        if (!(flags & BIT(4))) {
          x += ReadU16BE(points);
          points += 2;
        }
      }
      vertices[j].x = x;
    }

    S16 y = 0;
    for (U16 j = 0; j < num_vertices; j++) {
      U8 flags = vertices[j].flags;
      if (flags & BIT(2)) {
        S16 dx = *points;
        points++;
        if (flags & BIT(5)) { y += dx; }
        else                { y -= dx; }
      } else {
        if (!(flags & BIT(5))) {
          y += (((S16) *points) << 8) + ((S16) *(points + 1));
          points += 2;
        }
      }
      vertices[j].y = y;
    }

    // NOTE: decompress / build final glyph contour
    U16 contour_idx      = 0;
    U16 start_of_contour = 0;
    U16 end_of_contour   = ReadU16BE(end_pts_of_contours + (contour_idx * 2));
    while (true) {
      GlyphContour contour;
      MEMORY_ZERO_STRUCT(&contour);
      U32 curves_capacity = 0;

      for (U16 j = start_of_contour; j <= end_of_contour; j++) {
        // NOTE: loop points around so they stay within [start_of_contour, end_of_contour].
        FontVertex* p0 = &vertices[start_of_contour + (((j + 0) - start_of_contour) % (end_of_contour + 1 - start_of_contour))];
        FontVertex* p1 = &vertices[start_of_contour + (((j + 1) - start_of_contour) % (end_of_contour + 1 - start_of_contour))];
        FontVertex* p2 = &vertices[start_of_contour + (((j + 2) - start_of_contour) % (end_of_contour + 1 - start_of_contour))];

        // NOTE: off contour == is a control point
        B32 p0_on_contour = p0->flags & BIT(0);
        B32 p1_on_contour = p1->flags & BIT(0);
        B32 p2_on_contour = p2->flags & BIT(0);

        // NOTE: in cases of 2 off-contour points in a row, it is implied there is an on-contour point at their midpoint.
        // NOTE: this series of checks is structured so that we always *start* at a real or implied curve start point,
        // which is why we check 3 points at a time and there are spurious index skips.
        GlyphCurve curve;
        if (p0_on_contour) {
          if (p1_on_contour) {
            // on on n/a
            // NOTE: insert a fake control point in the middle so we don't need to differentiate between curves and lines.
            curve.start_x = p0->x;
            curve.start_y = p0->y;
            curve.control_x = (p0->x + p1->x) / 2;
            curve.control_y = (p0->y + p1->y) / 2;
          } else if (p2_on_contour) {
            // on off on
            curve.start_x   = p0->x;
            curve.start_y   = p0->y;
            curve.control_x = p1->x;
            curve.control_y = p1->y;
            j++;
          } else {
            // on off off
            curve.start_x   = p0->x;
            curve.start_y   = p0->y;
            curve.control_x = p1->x;
            curve.control_y = p1->y;
          }
        } else {
          DEBUG_ASSERT(!p1_on_contour);
          if (p2_on_contour) {
            // off off on
            curve.start_x   = (p0->x + p1->x) / 2;
            curve.start_y   = (p0->y + p1->y) / 2;
            curve.control_x = p1->x;
            curve.control_y = p1->y;
            j++;
          } else {
            // off off off
            curve.start_x   = (p0->x + p1->x) / 2;
            curve.start_y   = (p0->y + p1->y) / 2;
            curve.control_x = p1->x;
            curve.control_y = p1->y;
          }
        }
        // NOTE: contour.curves has exclusive arena access here, so we don't need to have a separate dedicated arena for these dynamic arrays.
        DA_PUSH_BACK_EX(arena, contour.curves, contour.curves_size, curves_capacity, curve);
      }
      // NOTE: pop dynamic array excess
      ARENA_POP_ARRAY(arena, GlyphCurve, curves_capacity - contour.curves_size);
      shape->contours[contour_idx++] = contour;
      if (contour_idx >= contours_size) { break; }
      start_of_contour = end_of_contour + 1;
      end_of_contour   = ReadU16BE(end_pts_of_contours + (contour_idx * 2));
    }
  } else if (contours_size < 0) {
    // NOTE: parse compound shape
    TODO();
  }

  ArenaRelease(temp_arena);
  return true;
}

B32 FontAtlasBake(Arena* arena, Font* font, FontAtlas* atlas, FontCharSet* char_set, F32 pixel_height, U32 atlas_width, U32 atlas_height) {
  B32 success  = false;
  U64 base_pos = ArenaPos(arena);
  Arena* temp_arena = ArenaAllocate();

  MEMORY_ZERO_STRUCT(atlas);
  atlas->bitmap_width  = atlas_width;
  atlas->bitmap_height = atlas_height;
  atlas->bitmap        = ARENA_PUSH_ARRAY(arena, U8, atlas_width * atlas_height);
  MEMORY_ZERO(atlas->bitmap, sizeof(U8) * (atlas_width * atlas_height));

  GlyphShape glyph_shape;
  MEMORY_ZERO_STRUCT(&glyph_shape);

  BinHead h;
  BinHeadInit(&h, font->data, font->data_size);

  // NOTE: determine glyph scale
  BinHeadSetPos(&h, font->hhea_offset);
  BinHeadSkip(&h, 4, sizeof(U8));
  S16 ascent  = BinHeadR16BE(&h);
  S16 descent = BinHeadR16BE(&h);
  S16 char_height = ascent - descent;
  F32 scale = (pixel_height) / ((F32) char_height);
  if (scale <= 0) {
    LOG_ERROR("[FONT] Invalid font scale: %0.5f, given pixel height: %0.2f", scale, pixel_height);
    goto font_bake_bitmap_end;
  }

  // NOTE: bake glyph bitmap
  U16 bitmap_x = 1;
  U16 bitmap_y = 1;
  U16 max_glyph_height_for_row = 0;
  for (FontCharSet* curr_char_set = char_set; curr_char_set != NULL; curr_char_set = curr_char_set->next) {
    for (U32 i = 0; i < curr_char_set->codepoints_size; i++) {
      U32 codepoint = curr_char_set->codepoints[i];
      if (FontAtlasGet(atlas, codepoint) != NULL) { continue; };
      ArenaClear(temp_arena);

      U32 glyph_index;
      if (!FontGetGlyphIndex(font, codepoint, &glyph_index)) {
        LOG_ERROR("[FONT] Unable to retrieve glyph index for codepoint: %d", codepoint);
        goto font_bake_bitmap_end;
      }

      S32 advance, left_side_bearing;
      if (!FontGetGlyphSizeInfo(font, glyph_index, &advance, &left_side_bearing)) {
        LOG_ERROR("[FONT] Unable to determine glyph size info for codepoint: %d", codepoint);
        goto font_bake_bitmap_end;
      }

      S32 glyph_glyf_offset = FontGetGlyphGlyfOffset(font, glyph_index);
      if (!FontGetGlyphShape(temp_arena, font, glyph_glyf_offset, &glyph_shape)) {
        LOG_ERROR("[FONT] Unable to determine glyph shape.");
        goto font_bake_bitmap_end;
      }

      // NOTE: determine bitmap box
      S16 scaled_glyph_min_x = (S16) F32Floor((F32) glyph_shape.min_x * scale);
      S16 scaled_glyph_min_y = (S16) F32Floor((F32) glyph_shape.min_y * scale);
      S16 scaled_glyph_max_x = (S16) F32Ceil((F32)  glyph_shape.max_x * scale);
      S16 scaled_glyph_max_y = (S16) F32Ceil((F32)  glyph_shape.max_y * scale);
      U32 scaled_glyph_width  = scaled_glyph_max_x - scaled_glyph_min_x;
      U32 scaled_glyph_height = scaled_glyph_max_y - scaled_glyph_min_y;
      if (bitmap_x + scaled_glyph_width > atlas_width) {
        bitmap_x = 1;
        bitmap_y += max_glyph_height_for_row;
        max_glyph_height_for_row = 1;
        if (bitmap_y > atlas_height ) {
          LOG_ERROR("[FONT] Provided font bitmap does not fit all glyphs.");
          goto font_bake_bitmap_end;
        }
      }
      max_glyph_height_for_row = MAX(scaled_glyph_height + 1, max_glyph_height_for_row);
      S16 bitmap_min_x = bitmap_x;
      S16 bitmap_min_y = bitmap_y;
      S16 bitmap_max_x = bitmap_x + scaled_glyph_width;
      S16 bitmap_max_y = bitmap_y + scaled_glyph_height;

      // NOTE: rasterize the glyph into the bitmap
      S16 pen_y = bitmap_min_y;
      U32 intersect_x_vals_size     = 0;
      U32 intersect_x_vals_capacity = 0;
      F32* intersect_x_vals         = NULL;
      while (pen_y <= bitmap_max_y) {
        F32 scanline_y = FontMapValue(pen_y + 0.5f, bitmap_min_y, bitmap_max_y, glyph_shape.min_y, glyph_shape.max_y);

        // NOTE: find all points that intersect with the current scanline.
        intersect_x_vals_size = 0;
        for (U32 i = 0; i < glyph_shape.contours_size; i++) {
          GlyphContour* contour = &glyph_shape.contours[i];
          for (U32 j = 0; j < contour->curves_size; j++) {
            GlyphCurve* curr = &contour->curves[j];
            GlyphCurve* next = &contour->curves[(j + 1) % contour->curves_size];

            F32 a = curr->start_y - (2 * curr->control_y) + next->start_y;
            F32 b = 2 * (curr->control_y - curr->start_y);
            F32 c = curr->start_y - scanline_y;

            // NOTE: get y-extremes for the given curve
            F32 curve_min_y = MIN(curr->start_y, next->start_y);
            F32 curve_max_y = MAX(curr->start_y, next->start_y);
            F32 t_y_extreme = (curr->start_y - curr->control_y) / a;
            if (0 <= t_y_extreme && t_y_extreme <= 1) {
              F32 y_extreme = FontCurveEvaluateBezier(curr->start_y, next->start_y, curr->control_y, t_y_extreme);
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
              continue;
            }

            for (U32 k = 0; k < t_size; k++) {
              if (!(0 <= t[k] && t[k] <= 1)) { continue; }
              F32 x_intersect = FontCurveEvaluateBezier(curr->start_x, next->start_x, curr->control_x, t[k]);
              if (x_intersect < glyph_shape.min_x || x_intersect > glyph_shape.max_x) {
                LOG_WARN("[FONT] Font defined glyph shape exceeds defined bounds! Attempting to recover, but there may be visual artifacts.");
                x_intersect = MAX(glyph_shape.min_x, x_intersect);
                x_intersect = MIN(glyph_shape.max_x, x_intersect);
              }
              DA_PUSH_BACK_EX(temp_arena, intersect_x_vals, intersect_x_vals_size, intersect_x_vals_capacity, x_intersect);
            }
          }
        }
        SORT_ASC(F32, intersect_x_vals, intersect_x_vals_size);

        // NOTE: color in the current scanline. this approach assumes that the pen starts up for each
        // given line and transitions from down <> up at every line boundary.
        // TODO: should we reference the encoded winding order here?
        F32 scanline_x = glyph_shape.min_x;
        B32 pen_down = false;
        for (U32 i = 0; i < intersect_x_vals_size; i++) {
          F32 x_value = intersect_x_vals[i];
          if (x_value < scanline_x) {
            pen_down = !pen_down;
            continue;
          }
          // TODO: take a second look at remapping -- only want to color pixel if the curve overlaps the center.
          S16 pen_start = FontMapValue(scanline_x, glyph_shape.min_x, glyph_shape.max_x, bitmap_min_x, bitmap_max_x) - 0.5f;
          S16 pen_stop  = FontMapValue(x_value, glyph_shape.min_x, glyph_shape.max_x, bitmap_min_x, bitmap_max_x) - 0.5f;
          if (pen_down) {
            // TODO: antialias curve edges
            for (S16 i = pen_start; i <= pen_stop; i++) {
              if (i == pen_start || i == pen_stop) {
                atlas->bitmap[(pen_y * atlas_width) + i] = 190;
              } else {
                atlas->bitmap[(pen_y * atlas_width) + i] = 255;
              }
            }
          }
          scanline_x = x_value;
          pen_down = !pen_down;
        }

        pen_y += 1;
      }

      BakedChar* baked_char = ARENA_PUSH_STRUCT(arena, BakedChar);
      baked_char->min_x = bitmap_min_x;
      baked_char->min_y = bitmap_min_y;
      baked_char->max_x = bitmap_max_x;
      baked_char->max_y = bitmap_max_y;
      baked_char->advance = ((F32) advance) * scale;
      baked_char->offset_x = F32Floor(((F32) glyph_shape.min_x) * scale);
      baked_char->offset_y = F32Floor(((F32) glyph_shape.min_y) * scale);
      baked_char->codepoint = codepoint;
      DEBUG_ASSERT(FontAtlasInsert(atlas, codepoint, baked_char));

      bitmap_x += scaled_glyph_width + 1;
    }
  }

  success = true;
font_bake_bitmap_end:
  ArenaRelease(temp_arena);
  if (!success) { ArenaPopTo(arena, base_pos); }
  return success;
}

static U32 FontAtlasHash(U32 codepoint) {
  // fast bit mixer
  U32 hash = codepoint;
  hash ^= hash >> 16;
  hash *= 0x7feb352d;
  hash ^= hash >> 15;
  hash *= 0x846ca68b;
  hash ^= hash >> 16;
  return hash;
}

B32 FontAtlasInsert(FontAtlas* atlas, U32 codepoint, BakedChar* baked_char) {
  if(FontAtlasGet(atlas, codepoint) != NULL) { return false; }
  U32 key_index = FontAtlasHash(codepoint) % STATIC_ARRAY_SIZE(atlas->character_map);
  BakedChar** curr = &atlas->character_map[key_index];
  while (*curr != NULL) { curr = &(*curr)->next; }
  *curr = baked_char;
  return true;
}

BakedChar* FontAtlasGet(FontAtlas* atlas, U32 codepoint) {
  U32 key_index = FontAtlasHash(codepoint) % STATIC_ARRAY_SIZE(atlas->character_map);
  BakedChar* curr = atlas->character_map[key_index];
  while (curr != NULL) {
    if (curr->codepoint == codepoint) { return curr; }
    curr = curr->next;
  }
  return NULL;
}

B32 FontAtlasPlace(FontAtlas* atlas, U32 codepoint, V2* cursor, V2* center, V2* size, V2* min_uv, V2* max_uv) {
  BakedChar* baked_char = FontAtlasGet(atlas, codepoint);
  if (baked_char == NULL) { return false; }
  size->x   = baked_char->max_x - baked_char->min_x;
  size->y   = baked_char->max_y - baked_char->min_y;
  center->x = F32Floor(cursor->x + baked_char->offset_x) + (size->x / 2.0f);
  center->y = F32Floor(cursor->y + baked_char->offset_y) + (size->y / 2.0f);
  min_uv->x = ((F32) baked_char->min_x) / atlas->bitmap_width;
  min_uv->y = ((F32) baked_char->min_y) / atlas->bitmap_width;
  max_uv->x = ((F32) baked_char->max_x) / atlas->bitmap_height;
  max_uv->y = ((F32) baked_char->max_y) / atlas->bitmap_height;
  cursor->x += baked_char->advance;
  return true;
}

void FontCharSetConcat(FontCharSet* set, FontCharSet* to_concat) {
  FontCharSet* curr = set;
  while (curr->next != NULL) { curr = curr->next; }
  curr->next = to_concat;
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

#endif // CDEFAULT_FONT_IMPLEMENTATION
