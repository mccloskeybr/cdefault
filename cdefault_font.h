#ifndef CDEFAULT_FONT_H_
#define CDEFAULT_FONT_H_

#include "cdefault_io.h"
#include "cdefault_image.h"
#include "cdefault_std.h"
#include "cdefault_math.h"

B32 FontBakeBitmap(Arena* arena, U8* data, U32 data_size, F32 pixel_height, Image* bitmap, U32 bitmap_width, U32 bitmap_height);

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

typedef struct FontCurve FontCurve;
struct FontCurve {
  S16 start_x;
  S16 start_y;
  S16 end_x;
  S16 end_y;
  S16 control_x;
  S16 control_y;
};

static void FontTableParse(FontTable* table, BinHead* h) {
  BinHeadR32BE(h);
  table->offset      = BinHeadR32BE(h);
  table->length      = BinHeadR32BE(h);
  table->initialized = true;
}

// https://tchayen.github.io/posts/ttf-file-parsing
// https://github.com/nothings/stb/blob/master/stb_truetype.h
// https://developer.apple.com/fonts/TrueType-Reference-Manual/
B32 FontBakeBitmap(Arena* arena, U8* data, U32 data_size, F32 pixel_height, Image* bitmap, U32 bitmap_width, U32 bitmap_height) {
  // TODO: initialize the bitmap
  U64 base_pos = ArenaPos(arena);
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

  // NOTE: bake glyph bitmap
  U32 bitmap_x = 1;
  U32 bitmap_y = 1;
  U32 max_glyph_height_for_row = 0;
  for (U32 i = 0; i < 1; i++) {
    U32 codepoint = 'a' + i; // TODO: configure

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
    S32 glyf_offset, g2;
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
    // TODO: validations? e.g. glyph index in range
    S32 x_min = 0;
    S32 y_min = 0;
    S32 x_max = 0;
    S32 y_max = 0;
    if (glyf_offset != g2) {
      BinHeadSetPos(&h, glyf_offset);
      BinHeadSkip(&h, 2, sizeof(U8));
      x_min = BinHeadR16BE(&h);
      y_min = BinHeadR16BE(&h);
      x_max = BinHeadR16BE(&h);
      y_max = BinHeadR16BE(&h);
      x_min = F32Floor(x_min * scale);
      y_min = F32Floor(y_min * scale);
      x_max = F32Ceil(x_max * scale);
      y_max = F32Ceil(y_max * scale);
    } else {
      // E.g. for space characters
      glyf_offset = -1;
    }

    // NOTE: determine glyph bitmap placement
    U32 glyph_width  = x_max - x_min;
    U32 glyph_height = y_max - y_min;
    if (bitmap_x + glyph_width > bitmap_width) {
      bitmap_x = 1;
      bitmap_y += max_glyph_height_for_row;
      max_glyph_height_for_row = 1;
      if (bitmap_y > bitmap_height) {
        LOG_ERROR("[FONT] Provided font bitmap does not fit all glyphs.");
        goto font_bake_bitmap_end;
      }
    }
    max_glyph_height_for_row = MAX(glyph_height, max_glyph_height_for_row);

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
        FontVertex* vertices = ARENA_PUSH_ARRAY(arena, FontVertex, num_vertices); // TODO: pop this!!!

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
        U16 start_of_contour        = 0;
        U16 end_of_contour          = ReadU16BE(end_pts_of_contours);
        U16 next_end_of_contour_idx = 1;
        FontCurve debug_curve;
        while (true) {
          for (U16 j = start_of_contour; j < end_of_contour; j++) {
            if (j > TEST) { return; }
            FontVertex* p0 = &vertices[start_of_contour + (((j + 0) - start_of_contour) % (end_of_contour - start_of_contour))];
            FontVertex* p1 = &vertices[start_of_contour + (((j + 1) - start_of_contour) % (end_of_contour - start_of_contour))];
            FontVertex* p2 = &vertices[start_of_contour + (((j + 2) - start_of_contour) % (end_of_contour - start_of_contour))];

            B32 p0_on_curve = p0->flags & BIT(0);
            B32 p1_on_curve = p1->flags & BIT(0);
            B32 p2_on_curve = p2->flags & BIT(0);

            if (WindowIsKeyJustPressed(Key_Space)) {
              U32 hook = 5;
            }

            FontCurve* curve = &debug_curve;
            if (p0_on_curve) {
              if (p1_on_curve) { // hit
                curve->start_x   = p0->x;
                curve->start_y   = p0->x;
                curve->control_x = (p0->x + p1->x) / 2;
                curve->control_y = (p0->y + p1->y) / 2;
                curve->end_x     = p1->x;
                curve->end_y     = p1->y;

              } else if (p2_on_curve) { // miss
                curve->start_x   = p0->x;
                curve->start_y   = p0->x;
                curve->control_x = p1->x;
                curve->control_y = p1->y;
                curve->end_x     = p2->x;
                curve->end_y     = p2->y;
                j++;

              } else { // hit
                curve->start_x   = p0->x;
                curve->start_y   = p0->y;
                curve->control_x = p1->x;
                curve->control_y = p1->y;
                curve->end_x     = (p1->x + p2->x) / 2;
                curve->end_y     = (p1->y + p2->y) / 2;
              }

            } else {
              DEBUG_ASSERT(!p1_on_curve);
              if (p2_on_curve) { // hit
                curve->start_x   = (p0->x + p1->x) / 2;
                curve->start_y   = (p0->y + p1->y) / 2;
                curve->control_x = p1->x;
                curve->control_y = p1->y;
                curve->end_x     = p2->x;
                curve->end_y     = p2->y;
                j++;

              } else { // miss
                curve->start_x   = (p0->x + p1->x) / 2;
                curve->start_y   = (p0->y + p1->y) / 2;
                curve->control_x = p1->x;
                curve->control_y = p1->y;
                curve->end_x     = (p1->x + p2->x) / 2;
                curve->end_y     = (p1->y + p2->y) / 2;
                j++;
              }
            }

            curve->start_x = curve->start_x / 4 + 100;
            curve->start_y = curve->start_y / 4 + 100;
            curve->end_x = curve->end_x / 4 + 100;
            curve->end_y = curve->end_y / 4 + 100;
            curve->control_x = curve->control_x / 4 + 100;
            curve->control_y = curve->control_y / 4 + 100;

            DrawQuadBezier(curve->start_x, curve->start_y, curve->control_x, curve->control_y, curve->end_x, curve->end_y, 10, j * 2, 0, 0, 1);
            DrawCircle(curve->start_x, curve->start_y, 2, 1, next_end_of_contour_idx == 2, 0);
            DrawCircle(curve->control_x, curve->control_y, 2, 0, next_end_of_contour_idx == 2, 1);
            DrawCircle(curve->end_x, curve->end_y, 2, 1, next_end_of_contour_idx == 2, 0);

          }
          return;
          if (next_end_of_contour_idx >= num_contours) { break; }
          start_of_contour = end_of_contour + 1;
          end_of_contour   = ReadU16BE(end_pts_of_contours + (next_end_of_contour_idx * 2));
          next_end_of_contour_idx++;
        }
        DEBUG_ASSERT(next_end_of_contour_idx == num_contours);

      } else if (num_contours < 0) {
        // NOTE: parse compound shape
      }
    }

    bitmap_x += glyph_width;
  }

  success = true;
font_bake_bitmap_end:
  if (!success) { ArenaPopTo(arena, base_pos); }
  return success;
}

#undef FONT_PID_UNICODE_ENCODING
#undef FONT_PID_MICROSOFT_ENCODING
#undef FONT_MICROSOFT_EID_UNICODE_BMP
#undef FONT_MICROSOFT_EID_UNICODE_FULL

#endif // CDEFAULT_FONT_IMPLEMENTATION
