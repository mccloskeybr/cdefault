#ifndef CDEFAULT_FONT_H_
#define CDEFAULT_FONT_H_

#include "cdefault_io.h"
#include "cdefault_image.h"
#include "cdefault_std.h"
#include "cdefault_math.h"

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
  U32 cmap_subtable_offset;
  U16 num_glyphs;
  LocFormat loc_format;
};

B32 FontInit(Font* font, U8* data, U32 data_size);
B32 FontBakeBitmap(Arena* arena, Font* font, F32 pixel_height, Image* bitmap, U16 bitmap_width, U16 bitmap_height);

// NOTE: determines the index of the glyph into the various font tables.
B32 FontGetGlyphIndex(Font* font, U32 codepoint, U32* glyph_index);
// NOTE: determines the offset of the glyph from the glyf table.
// NOTE: returns false e.g. if there is no entry in the glyf table (true for e.g. ' ').
B32 FontGetGlyphGlyfOffset(Font* font, U32 glyph_index, S32* glyf_offset);
B32 FontGetGlyphSizeInfo(Font* font, U32 glyph_index, S32* advance_width, S32* left_side_bearing);

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

// NOTE: Each node's end point is inferred to be the next node in the contour's start point.
typedef struct FontSplineNode FontSplineNode;
struct FontSplineNode {
  S16 start_x;
  S16 start_y;
  S16 control_x;
  S16 control_y;
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

  // NOTE: determine loc format
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

  // NOTE: retrieve validation data
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
      BinHeadSkip(&h, 4, sizeof(U8));
      BinHeadSkip(&h, codepoint, sizeof(U8));
      *glyph_index = BinHeadR8(&h);
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
        if (codepoint < mid_start_char) {
          high = mid;
        } else if (codepoint > mid_end_char) {
          low = mid + 1;
        } else {
          U32 start_glyph = ReadU32BE(groups + ((mid * 12) + 8));
          if (cmap_format == 12) {
            *glyph_index = start_glyph + codepoint - mid_start_char;
            break;
          } else {
            *glyph_index = start_glyph;
            break;
          }
        }
      }
    } break;
    default: TODO();
  }
  if (*glyph_index == 0) { return false; }
  return true;
}

// TODO: validate that the provided glyph index is less than the maximum number of glyphs.
B32 FontGetGlyphGlyfOffset(Font* font, U32 glyph_index, S32* glyf_offset) {
  *glyf_offset = 0;
  if (glyph_index > font->num_glyphs) {
    LOG_ERROR("[FONT] Provided glyph index exceeds the number of glyphs: %d, %d", glyph_index, font->num_glyphs);
    return false;
  }

  U32 next_glyf_offset = 0;
  BinHead h;
  BinHeadInit(&h, font->data, font->data_size);
  BinHeadSetPos(&h, font->loca_offset);
  switch (font->loc_format) {
    case LocFormat_U16: {
      BinHeadSkip(&h, 2 * glyph_index, sizeof(U8));
      *glyf_offset     = font->glyf_offset + BinHeadR16BE(&h) * 2;
      next_glyf_offset = font->glyf_offset + BinHeadR16BE(&h) * 2;
    } break;
    case LocFormat_U32: {
      BinHeadSkip(&h, 4 * glyph_index, sizeof(U8));
      *glyf_offset     = font->glyf_offset + BinHeadR32BE(&h);
      next_glyf_offset = font->glyf_offset + BinHeadR32BE(&h);
    } break;
    default: UNREACHABLE();
  }

  // NOTE: length is implied from offset_next - offset_curr.
  U32 glyf_length = next_glyf_offset - *glyf_offset;
  if (glyf_length == 0) { return false; }
  return true;
}

B32 FontGetGlyphSizeInfo(Font* font, U32 glyph_index, S32* advance_width, S32* left_side_bearing) {
  *advance_width = 0;
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
    *advance_width     = BinHeadR16BE(&h);
    *left_side_bearing = BinHeadR16BE(&h);

  } else {
    // NOTE: e.g. for monospaced fonts
    BinHeadSetPos(&h, font->hmtx_offset);
    BinHeadSkip(&h, 4 * (num_long_hor_metrics - 1), sizeof(U8));
    *advance_width = BinHeadR16BE(&h);

    BinHeadSetPos(&h, font->hmtx_offset);
    BinHeadSkip(&h, 4 * num_long_hor_metrics, sizeof(U8));
    BinHeadSkip(&h, 2 * (glyph_index - num_long_hor_metrics), sizeof(U8));
    *left_side_bearing = BinHeadR16BE(&h);
  }
  return true;
}

B32 FontBakeBitmap(Arena* arena, Font* font, F32 pixel_height, Image* bitmap, U16 bitmap_width, U16 bitmap_height) {
  B32 success  = false;
  U64 base_pos = ArenaPos(arena);

  MEMORY_ZERO_STRUCT(bitmap);
  bitmap->format = ImageFormat_R;
  bitmap->width  = bitmap_width;
  bitmap->height = bitmap_height;
  bitmap->data   = ARENA_PUSH_ARRAY(arena, U8, bitmap_width * bitmap_height);
  MEMORY_ZERO(bitmap->data, sizeof(U8) * (bitmap_width * bitmap_height));

  Arena* temp_arena = ArenaAllocate();
  DynamicArray contours;
  DynamicArrayInit(&contours, sizeof(DynamicArray), 8);

  BinHead h;
  BinHeadInit(&h, font->data, font->data_size);

  // NOTE: determine glyph scale
  BinHeadSetPos(&h, font->hhea_offset);
  BinHeadSkip(&h, 4, sizeof(U8));
  S16 ascent  = BinHeadR16BE(&h);
  S16 descent = BinHeadR16BE(&h);
  S16 char_height = ascent - descent;
  F32 scale = (pixel_height) / ((F32) char_height);
  if (scale == 0) {
    LOG_ERROR("[FONT] Scale of the font is 0!");
    goto font_bake_bitmap_end;
  }

  // NOTE: bake glyph bitmap
  U16 bitmap_x = 1;
  U16 bitmap_y = 1;
  U16 max_glyph_height_for_row = 0;
  for (U32 i = 0; i < TEST; i++) {
    U32 codepoint = 'A' + i; // TODO: configure

    U32 glyph_index;
    if (!FontGetGlyphIndex(font, codepoint, &glyph_index)) {
      LOG_ERROR("[FONT] Unable to retrieve glyph index for codepoint: %d", codepoint);
      goto font_bake_bitmap_end;
    }

    S32 advance_width, left_side_bearing;
    if (!FontGetGlyphSizeInfo(font, glyph_index, &advance_width, &left_side_bearing)) {
      LOG_ERROR("[FONT] Unable to determine glyph size info for codepoint: %d", codepoint);
      goto font_bake_bitmap_end;
    }

    S32 glyph_glyf_offset;
    B32 has_glyf_entry = FontGetGlyphGlyfOffset(font, glyph_index, &glyph_glyf_offset);

    // NOTE: determine bitmap box
    S16 glyph_min_x = 0;
    S16 glyph_min_y = 0;
    S16 glyph_max_x = 0;
    S16 glyph_max_y = 0;
    if (has_glyf_entry) {
      BinHeadSetPos(&h, glyph_glyf_offset);
      BinHeadSkip(&h, 2, sizeof(U8));
      glyph_min_x = BinHeadR16BE(&h);
      glyph_min_y = BinHeadR16BE(&h);
      glyph_max_x = BinHeadR16BE(&h);
      glyph_max_y = BinHeadR16BE(&h);
    }
    S16 scaled_glyph_min_x = (S16) F32Floor(((F32) glyph_min_x) * scale);
    S16 scaled_glyph_min_y = (S16) F32Floor(((F32) glyph_min_y) * scale);
    S16 scaled_glyph_max_x = (S16) F32Ceil(((F32)  glyph_max_x) * scale);
    S16 scaled_glyph_max_y = (S16) F32Ceil(((F32)  glyph_max_y) * scale);

    // NOTE: determine glyph bitmap placement
    U16 scaled_glyph_width  = scaled_glyph_max_x - scaled_glyph_min_x;
    U16 scaled_glyph_height = scaled_glyph_max_y - scaled_glyph_min_y;
    if (bitmap_x + scaled_glyph_width > bitmap_width) {
      bitmap_x = 1;
      bitmap_y += max_glyph_height_for_row;
      max_glyph_height_for_row = 1;
      if (bitmap_y > bitmap_height) {
        LOG_ERROR("[FONT] Provided font bitmap does not fit all glyphs.");
        goto font_bake_bitmap_end;
      }
    }
    max_glyph_height_for_row = MAX(scaled_glyph_height + 1, max_glyph_height_for_row);
    S16 bitmap_min_x = bitmap_x;
    S16 bitmap_min_y = bitmap_y;
    S16 bitmap_max_x = bitmap_x + scaled_glyph_width;
    S16 bitmap_max_y = bitmap_y + scaled_glyph_height;

    // NOTE: determine glyph shape
    if (has_glyf_entry) {
      BinHeadSetPos(&h, glyph_glyf_offset);
      S16 num_contours = BinHeadR16BE(&h);
      if (num_contours > 0) {
        // NOTE: parse simple shape
        BinHeadSkip(&h, 8, sizeof(U8));
        U8* end_pts_of_contours = BinHeadDecay(&h);
        BinHeadSkip(&h, 2 * num_contours, sizeof(U8));
        U16 instructions_length = BinHeadR16BE(&h);
        BinHeadSkip(&h, instructions_length, sizeof(U8));
        U8* points = BinHeadDecay(&h);

        U16 num_vertices = ReadU16BE(end_pts_of_contours + (2 * (num_contours - 1))) + 1;
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
        DynamicArrayClear(&contours);
        while (true) {
          // TODO: likely a better way to store contours than creating a dynamic array for each one?
          DynamicArray contour;
          DynamicArrayInit(&contour, sizeof(FontSplineNode), 32);
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
            FontSplineNode spline_node;
            if (p0_on_contour) {
              if (p1_on_contour) {
                // on on n/a
                // NOTE: insert a fake control point in the middle so we don't need to differentiate between curves and lines.
                spline_node.start_x = p0->x;
                spline_node.start_y = p0->y;
                spline_node.control_x = (p0->x + p1->x) / 2;
                spline_node.control_y = (p0->y + p1->y) / 2;
              } else if (p2_on_contour) {
                // on off on
                spline_node.start_x   = p0->x;
                spline_node.start_y   = p0->y;
                spline_node.control_x = p1->x;
                spline_node.control_y = p1->y;
                j++;
              } else {
                // on off off
                spline_node.start_x   = p0->x;
                spline_node.start_y   = p0->y;
                spline_node.control_x = p1->x;
                spline_node.control_y = p1->y;
              }
            } else {
              DEBUG_ASSERT(!p1_on_contour);
              if (p2_on_contour) {
                // off off on
                spline_node.start_x   = (p0->x + p1->x) / 2;
                spline_node.start_y   = (p0->y + p1->y) / 2;
                spline_node.control_x = p1->x;
                spline_node.control_y = p1->y;
                j++;
              } else {
                // off off off
                spline_node.start_x   = (p0->x + p1->x) / 2;
                spline_node.start_y   = (p0->y + p1->y) / 2;
                spline_node.control_x = p1->x;
                spline_node.control_y = p1->y;
              }
            }
            DynamicArrayPushBackSafe(&contour, &spline_node);
          }
          DynamicArrayPushBackSafe(&contours, &contour);
          contour_idx++;
          if (contour_idx >= num_contours) { break; }
          start_of_contour = end_of_contour + 1;
          end_of_contour   = ReadU16BE(end_pts_of_contours + (contour_idx * 2));
        }
      } else if (num_contours < 0) {
        // NOTE: parse compound shape
        TODO();
      }
    }

    // NOTE: rasterize the glyph into the bitmap
    // TODO: should we reference the encoded winding order here?
    S16 pen_y = bitmap_min_y;
    DynamicArray intersect_x_vals;
    DynamicArrayInit(&intersect_x_vals, sizeof(F32), 32);
    while (pen_y <= bitmap_max_y) {
      F32 scanline_y = FontMapValue(pen_y + 0.5f, bitmap_min_y, bitmap_max_y, glyph_min_y, glyph_max_y);

      // NOTE: find all points that intersect with the current scanline.
      DynamicArrayClear(&intersect_x_vals);
      for (U32 i = 0; i < contours.size; i++) {
        DynamicArray* contour = (DynamicArray*) DynamicArrayGet(&contours, i);
        for (U32 j = 0; j < contour->size; j++) {
          FontSplineNode* curr = (FontSplineNode*) DynamicArrayGet(contour, j);
          FontSplineNode* next = (FontSplineNode*) DynamicArrayGet(contour, (j + 1) % contour->size);

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

          // NOTE: find Ts for x-intersections on given scanline
          F32 t[2];
          U32 t_size = 0;
          if (F32Abs(a) > 0.00001) {
            // is a parabola
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
            // is a line
            t[t_size++] = -c / b;
          } else {
            continue;
          }

          for (U32 k = 0; k < t_size; k++) {
            if (!(0 <= t[k] && t[k] <= 1)) { continue; }
            F32 x_intersect = FontCurveEvaluateBezier(curr->start_x, next->start_x, curr->control_x, t[k]);
            if (x_intersect < glyph_min_x || x_intersect > glyph_max_x) {
              LOG_WARN("[FONT] Font defined glyph shape exceeds defined bounds! Attempting to recover, but there may be visual artifacts.");
              x_intersect = MAX(glyph_min_x, x_intersect);
              x_intersect = MIN(glyph_max_x, x_intersect);
            }
            DynamicArrayPushBackSafe(&intersect_x_vals, &x_intersect);
          }
        }
      }
      SORT_ASC(F32, (F32*) intersect_x_vals.data, intersect_x_vals.size);

      F32 scanline_x = glyph_min_x;
      B32 pen_down = false;
      for (U32 i = 0; i < intersect_x_vals.size; i++) {
        F32 x_value = *(F32*) DynamicArrayGet(&intersect_x_vals, i);
        if (x_value < scanline_x) {
          pen_down = !pen_down;
          continue;
        }
        // TODO: take a second look at remapping -- only want to color pixel if the curve overlaps the center.
        S16 pen_start = FontMapValue(scanline_x, glyph_min_x, glyph_max_x, bitmap_min_x, bitmap_max_x) - 0.5f;
        S16 pen_stop  = FontMapValue(x_value, glyph_min_x, glyph_max_x, bitmap_min_x, bitmap_max_x) - 0.5f;
        if (pen_down) {
          // TODO: antialias edges
          for (S16 i = pen_start; i <= pen_stop; i++) {
            bitmap->data[((bitmap_height - pen_y) * bitmap_width) + i] = 255;
          }
        }
        scanline_x = x_value;
        pen_down = !pen_down;
      }

      pen_y += 1;
    }

    for (U32 i = 0; i < contours.size; i++) {
      DynamicArrayDeinit((DynamicArray*) DynamicArrayGet(&contours, i));
    }
    DynamicArrayDeinit(&intersect_x_vals);
    bitmap_x += scaled_glyph_width;
  }

  success = true;
font_bake_bitmap_end:
  ArenaRelease(temp_arena);
  DynamicArrayDeinit(&contours);
  if (!success) { ArenaPopTo(arena, base_pos); }
  return success;
}

#undef FONT_PID_UNICODE_ENCODING
#undef FONT_PID_MICROSOFT_ENCODING
#undef FONT_MICROSOFT_EID_UNICODE_BMP
#undef FONT_MICROSOFT_EID_UNICODE_FULL

#endif // CDEFAULT_FONT_IMPLEMENTATION
