#ifndef CDEFAULT_FONT_H_
#define CDEFAULT_FONT_H_

#include "cdefault_io.h"
#include "cdefault_image.h"
#include "cdefault_std.h"
#include "cdefault_math.h"

B32 FontBakeBitmap(Arena* arena, U8* data, U32 data_size, F32 pixel_height, Image* bitmap, U16 bitmap_width, U16 bitmap_height);

#endif // CDEFAULT_FONT_H_

#ifdef CDEFAULT_FONT_IMPLEMENTATION
#undef CDEFAULT_FONT_IMPLEMENTATION

#define FONT_PID_UNICODE_ENCODING       0
#define FONT_PID_MICROSOFT_ENCODING     3
#define FONT_MICROSOFT_EID_UNICODE_BMP  1
#define FONT_MICROSOFT_EID_UNICODE_FULL 10

typedef struct FontTable FontTable;
struct FontTable {
  U32 offset;
  U32 length;
  B32 initialized;
};

typedef struct Font Font;
struct Font {
  // maybe these can be local?
  FontTable cmap;
  FontTable loca;
  FontTable head;
  FontTable glyf;
  FontTable hhea;
  FontTable hmtx;
};

typedef struct FontVertex FontVertex;
struct FontVertex {
  U8 flags;
  S16 x;
  S16 y;
};

// NOTE: Each node's end point is inferred to be the next node's start point.
typedef struct FontSplineNode FontSplineNode;
struct FontSplineNode {
  S16 start_x;
  S16 start_y;
  S16 control_x;
  S16 control_y;
};

static void FontTableParse(FontTable* table, BinHead* h) {
  BinHeadR32BE(h);
  table->offset      = BinHeadR32BE(h);
  table->length      = BinHeadR32BE(h);
  table->initialized = true;
}

// NOTE: maps a point in rectangle A into rectangle B.
// TODO: SPEEDUP: some obvious speedups here.
static void FontScalePoint(
    F32 a_x, F32 a_y, F32* b_x, F32* b_y,
    F32 a_min_x, F32 a_min_y, F32 a_max_x, F32 a_max_y,
    F32 b_min_x, F32 b_min_y, F32 b_max_x, F32 b_max_y) {
  DEBUG_ASSERT(a_min_x <= a_x && a_x <= a_max_x);
  DEBUG_ASSERT(a_min_y <= a_y && a_y <= a_max_y);
  F32 u = (a_x - a_min_x) / (a_max_x - a_min_x);
  F32 v = (a_y - a_min_y) / (a_max_y - a_min_y);
  *b_x = b_min_x + u * (b_max_x - b_min_x);
  *b_y = b_min_y + v * (b_max_y - b_min_y);
}

static void FontCurveGetDimBounds(F32 start, F32 end, F32 control, F32* min, F32* max) {
  *min = MIN(start, end);
  *max = MAX(start, end);
  F32 t_extreme = (start - end) / (start - (2 * end) + control);
  if (0 <= t_extreme && t_extreme <= 1) {
    F32 start_control = F32Lerp(start, control, t_extreme);
    F32 control_end   = F32Lerp(control, end, t_extreme);
    F32 y_extreme     = F32Lerp(start_control, control_end, t_extreme);
    *min = MIN(*min, y_extreme);
    *max = MAX(*max, y_extreme);
  }
}

// https://tchayen.github.io/posts/ttf-file-parsing
// https://github.com/nothings/stb/blob/master/stb_truetype.h
// https://developer.apple.com/fonts/TrueType-Reference-Manual/
B32 FontBakeBitmap(Arena* arena, U8* data, U32 data_size, F32 pixel_height, Image* bitmap, U16 bitmap_width, U16 bitmap_height) {
  U64 base_pos   = ArenaPos(arena);

  MEMORY_ZERO_STRUCT(bitmap);
  bitmap->format = ImageFormat_R;
  bitmap->width  = bitmap_width;
  bitmap->height = bitmap_height;
  bitmap->data   = ARENA_PUSH_ARRAY(arena, U8, bitmap_width * bitmap_height);
  MEMORY_ZERO(bitmap->data, sizeof(U8) * (bitmap_width * bitmap_height));

  Arena* temp_arena = ArenaAllocate();
  DynamicArray contours;
  DynamicArrayInit(&contours, sizeof(DynamicArray), 8);
  B32 success = false;

  Font font;
  MEMORY_ZERO_STRUCT(&font);

  BinHead h;
  BinHeadInit(&h, data, data_size);

  // NOTE: parse header
  BinHeadR32BE(&h);
  U16 num_tables = BinHeadR16BE(&h);
  BinHeadSkip(&h, 6, sizeof(U8));

  // NOTE: parse top-level tables
  for (U32 i = 0; i < num_tables; i++) {
    String8 tag = Str8(BinHeadDecay(&h), 4);
    BinHeadSkip(&h, 4, sizeof(U8));
    if      (Str8Eq(tag, Str8Lit("cmap"))) { FontTableParse(&font.cmap, &h); }
    else if (Str8Eq(tag, Str8Lit("loca"))) { FontTableParse(&font.loca, &h); }
    else if (Str8Eq(tag, Str8Lit("head"))) { FontTableParse(&font.head, &h); }
    else if (Str8Eq(tag, Str8Lit("glyf"))) { FontTableParse(&font.glyf, &h); }
    else if (Str8Eq(tag, Str8Lit("hhea"))) { FontTableParse(&font.hhea, &h); }
    else if (Str8Eq(tag, Str8Lit("hmtx"))) { FontTableParse(&font.hmtx, &h); }
    else                                   { BinHeadSkip(&h, 12, sizeof(U8)); }
  }
  if (!font.cmap.initialized) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'cmap'.");
    goto font_bake_bitmap_end;
  }
  if (!font.loca.initialized) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'loca'.");
    goto font_bake_bitmap_end;
  }
  if (!font.head.initialized) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'head'.");
    goto font_bake_bitmap_end;
  }
  if (!font.glyf.initialized) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'glyf'.");
    goto font_bake_bitmap_end;
  }
  if (!font.hhea.initialized) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'hhea'.");
    goto font_bake_bitmap_end;
  }
  if (!font.hmtx.initialized) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing required table 'hmtx'.");
    goto font_bake_bitmap_end;
  }

  // NOTE: find cmap encoding index
  U32 cmap_subtable_offset = 0;
  BinHeadSetPos(&h, font.cmap.offset);
  BinHeadSkip(&h, 2, sizeof(U8));
  U16 num_cmap_subtables = BinHeadR16BE(&h);
  for (U16 i = 0; i < num_cmap_subtables; i++) {
    U16 plat_id      = BinHeadR16BE(&h);
    U16 plat_spec_id = BinHeadR16BE(&h);
    U32 index_offset = BinHeadR32BE(&h);
    if (plat_id == FONT_PID_MICROSOFT_ENCODING) {
      if (plat_spec_id == FONT_MICROSOFT_EID_UNICODE_BMP ||
          plat_spec_id == FONT_MICROSOFT_EID_UNICODE_FULL) {
        cmap_subtable_offset = font.cmap.offset + index_offset;
      }
    } else if (plat_id == FONT_PID_UNICODE_ENCODING) {
      cmap_subtable_offset = font.cmap.offset + index_offset;
    }
  }
  if (cmap_subtable_offset == 0) {
    LOG_ERROR("[FONT] Failed to find supported cmap encoding table.");
    goto font_bake_bitmap_end;
  }

  // NOTE: determine loc format
  BinHeadSetPos(&h, font.head.offset);
  BinHeadSkip(&h, 50, sizeof(U8));
  U16 index_to_loc_format = BinHeadR16BE(&h);
  if (index_to_loc_format > 1) {
    LOG_ERROR("[FONT] Invalid or unsupported indexToLocFormat provided: %d", index_to_loc_format);
    goto font_bake_bitmap_end;
  }

  // NOTE: determine glyph scale
  BinHeadSetPos(&h, font.hhea.offset);
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

    // NOTE: find glyph index
    U32 glyph_index = 0;
    BinHeadSetPos(&h, cmap_subtable_offset);
    U16 cmap_format = BinHeadR16BE(&h);
    switch (cmap_format) {
      case 0: {
        U16 length = BinHeadR16BE(&h);
        BinHeadSkip(&h, 2, sizeof(U8));
        BinHeadSkip(&h, codepoint, sizeof(U8));
        glyph_index = BinHeadR8(&h);
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
              glyph_index = start_glyph + codepoint - mid_start_char;
              break;
            } else {
              glyph_index = start_glyph;
              break;
            }
          }
        }
      } break;
      default: TODO();
    }
    DEBUG_ASSERT(glyph_index != 0);

    // NOTE: find glyph size metrics
    S32 advance_width, left_side_bearing;
    BinHeadSetPos(&h, font.hhea.offset);
    BinHeadSkip(&h, 34, sizeof(U8));
    U16 num_long_hor_metrics = BinHeadR16BE(&h);
    if (glyph_index < num_long_hor_metrics) {
      BinHeadSetPos(&h, font.hmtx.offset);
      BinHeadSkip(&h, 4 * glyph_index, sizeof(U8));
      advance_width     = BinHeadR16BE(&h);
      left_side_bearing = BinHeadR16BE(&h);
    } else {
      // NOTE: e.g. for monospaced fonts
      BinHeadSetPos(&h, font.hmtx.offset);
      BinHeadSkip(&h, 4 * (num_long_hor_metrics - 1), sizeof(U8));
      advance_width = BinHeadR16BE(&h);

      BinHeadSetPos(&h, font.hmtx.offset);
      BinHeadSkip(&h, 4 * num_long_hor_metrics, sizeof(U8));
      BinHeadSkip(&h, 2 * (glyph_index - num_long_hor_metrics), sizeof(U8));
      left_side_bearing = BinHeadR16BE(&h);
    }

    // NOTE: get glyf offset
    S32 glyf_offset, g2; // TODO: understand what is g2
    BinHeadSetPos(&h, font.loca.offset);
    switch (index_to_loc_format) {
      case 0: { // NOTE: stored as U16s
        BinHeadSkip(&h, 2 * glyph_index, sizeof(U8));
        glyf_offset = font.glyf.offset + BinHeadR16BE(&h) * 2;
        g2          = font.glyf.offset + BinHeadR16BE(&h) * 2;
      } break;
      case 1: { // NOTE: stored as U32s
        BinHeadSkip(&h, 4 * glyph_index, sizeof(U8));
        glyf_offset = font.glyf.offset + BinHeadR32BE(&h);
        g2          = font.glyf.offset + BinHeadR32BE(&h);
      } break;
      default: UNREACHABLE();
    }

    // NOTE: determine bitmap box
    S16 min_x = 0;
    S16 min_y = 0;
    S16 max_x = 0;
    S16 max_y = 0;
    if (glyf_offset != g2) {
      BinHeadSetPos(&h, glyf_offset);
      BinHeadSkip(&h, 2, sizeof(U8));
      min_x = BinHeadR16BE(&h);
      min_y = BinHeadR16BE(&h);
      max_x = BinHeadR16BE(&h);
      max_y = BinHeadR16BE(&h);
    } else {
      // E.g. for space characters
      glyf_offset = -1;
    }
    S16 scaled_min_x = (S16) F32Floor(((F32) min_x) * scale);
    S16 scaled_min_y = (S16) F32Floor(((F32) min_y) * scale);
    S16 scaled_max_x = (S16) F32Ceil(((F32)  max_x) * scale);
    S16 scaled_max_y = (S16) F32Ceil(((F32)  max_y) * scale);

    // NOTE: determine glyph bitmap placement
    U16 scaled_glyph_width  = scaled_max_x - scaled_min_x;
    U16 scaled_glyph_height = scaled_max_y - scaled_min_y;
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
    S16 bitmap_max_x = scaled_min_x + scaled_glyph_width;
    S16 bitmap_max_y = scaled_min_y + scaled_glyph_height;

    // NOTE: determine glyph shape
    if (glyf_offset > 0) {
      BinHeadSetPos(&h, glyf_offset);
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

        for (U32 j = 0; j < contours.size; j++) {
          DynamicArray* contour = (DynamicArray*) DynamicArrayGet(&contours, j);
          for (U32 k = 0; k < contour->size; k++) {
            FontSplineNode* curr = (FontSplineNode*) DynamicArrayGet(contour, k);
            FontSplineNode* next = (FontSplineNode*) DynamicArrayGet(contour, (k + 1) % contour->size);

            F32 start_x = curr->start_x / 4 + 100;
            F32 start_y = curr->start_y / 4 + 100;
            F32 end_x = next->start_x / 4 + 100;
            F32 end_y = next->start_y / 4 + 100;
            F32 control_x = curr->control_x / 4 + 100;
            F32 control_y = curr->control_y / 4 + 100;

            DrawQuadBezier(start_x, start_y, control_x, control_y, end_x, end_y, 10, 2, 0, 0, 1);
          }
        }

      } else if (num_contours < 0) {
        // NOTE: parse compound shape
        TODO();
      }
    }

    // NOTE: rasterize the glyph into the bitmap
    B32 pen_down;
    S16 pen_x;
    S16 pen_y = bitmap_min_y;
    while (pen_y <= bitmap_max_y) {
      pen_down = false;
      pen_x    = bitmap_min_x;
      while (pen_x <= bitmap_max_x) {
        // TODO: scaling like this is cringe
        F32 pen_glyph_x, pen_glyph_y;
        FontScalePoint(
            pen_x, pen_y, &pen_glyph_x, &pen_glyph_y,
            bitmap_min_x, bitmap_min_y, bitmap_max_x, bitmap_max_y,
            min_x, min_y, max_x, max_y);

        // NOTE: find the closest contour to the right.
        // TODO: can sort the contours to speed this up
        S32 closest_contour = -1;
        S32 closest_curve   = -1;
        F32 min_distance    = F32_MAX;
        for (U32 i = 0; i < contours.size; i++) {
          DynamicArray* contour = (DynamicArray*) DynamicArrayGet(&contours, i);
          for (U32 j = 0; j < contour->size; j++) {
            FontSplineNode* curr = (FontSplineNode*) DynamicArrayGet(contour, j);
            FontSplineNode* next = (FontSplineNode*) DynamicArrayGet(contour, (j + 1) % contour->size);

            F32 min_y, max_y, min_x, max_x;
            FontCurveGetDimBounds(curr->start_y, next->start_y, curr->control_y, &min_y, &max_y);
            if (pen_glyph_y < min_y || pen_glyph_y > max_y) { continue; } // NOTE: missed in vertical direction.
            FontCurveGetDimBounds(curr->start_x, next->start_x, curr->control_x, &min_x, &max_x);
            if (pen_glyph_x > min_x && pen_glyph_x > max_x) { continue; } // NOTE: already passed this curve.

            F32 distance = MAX(min_x - pen_glyph_x, max_x - pen_glyph_x);
            if (distance < min_distance) {
              closest_contour = i;
              closest_curve   = j;
              min_distance    = distance;
            }
          }
        }
        // NOTE: if no contour was found, we're done drawing this line.
        if(closest_contour == -1) {
          DEBUG_ASSERT(!pen_down);
          break;
        }

        F32 draw_to_x, draw_to_y;
        FontScalePoint(
            pen_glyph_x + min_distance, pen_glyph_y, &draw_to_x, &draw_to_y,
            min_x, min_y, max_x, max_y,
            bitmap_min_x, bitmap_min_y, bitmap_max_x, bitmap_max_y);

        S16 pixel_dist = draw_to_x - pen_x;
        if (pen_down) {
          for (S16 i = pen_x; i <= pen_x + pixel_dist; i++) {
            bitmap->data[(pen_y * bitmap_height) + i] = 255;
          }
        }

        pen_x = draw_to_x + 1;
        pen_down = !pen_down;
      }

      pen_y += 1;
    }

    for (U32 i = 0; i < contours.size; i++) {
      DynamicArray* contour = (DynamicArray*) DynamicArrayGet(&contours, i);
      DynamicArrayDeinit(contour);
    }
    DynamicArrayClear(&contours);
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
