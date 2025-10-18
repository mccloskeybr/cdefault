#ifndef CDEFAULT_FONT_H_
#define CDEFAULT_FONT_H_

#include "cdefault_io.h"
#include "cdefault_std.h"

typedef struct Font Font;
B32 FontInit(Font* font, U8* data, U32 data_size);

#endif // CDEFAULT_FONT_H_

#ifdef CDEFAULT_FONT_IMPLEMENTATION
#undef CDEFAULT_FONT_IMPLEMENTATION

typedef struct FontTable FontTable;
struct FontTable {
  U32 checksum;
  U32 offset;
  U32 length;
  B32 initialized;
};

struct Font {
  // maybe these can be local
  FontTable cmap;
  FontTable loca;
  FontTable head;
  FontTable glyf;
  FontTable hhea;
  FontTable hmtx;
};

static void FontTableParse(FontTable* table, BinHead* h) {
  table->checksum    = BinHeadR32BE(h);
  table->offset      = BinHeadR32BE(h);
  table->length      = BinHeadR32BE(h);
  table->initialized = true;
}

// https://tchayen.github.io/posts/ttf-file-parsing
// https://github.com/nothings/stb/blob/master/stb_truetype.h
B32 FontInit(Font* font, U8* data, U32 data_size) {
  MEMORY_ZERO_STRUCT(font);

  BinHead h;
  BinHeadInit(&h, data, data_size);
  BinHeadR32BE(&h); // version
  U16 num_tables = BinHeadR16BE(&h);
  BinHeadSkip(&h, 6, sizeof(U8));
  for (U32 i = 0; i < num_tables; i++) {
    String8 tag = String8Create(&h.bytes[h.pos], 4);
    BinHeadSkip(&h, 4, sizeof(U8));
    if      (String8Equals(&tag, &String8CreateCString("cmap"))) { FontTableParse(&font->cmap, &h); }
    if      (String8Equals(&tag, &String8CreateCString("loca"))) { FontTableParse(&font->loca, &h); }
    else if (String8Equals(&tag, &String8CreateCString("head"))) { FontTableParse(&font->head, &h); }
    else if (String8Equals(&tag, &String8CreateCString("glyf"))) { FontTableParse(&font->glyf, &h); }
    else if (String8Equals(&tag, &String8CreateCString("hhea"))) { FontTableParse(&font->hhea, &h); }
    else if (String8Equals(&tag, &String8CreateCString("hmtx"))) { FontTableParse(&font->hmtx, &h); }
    else                                                         { BinHeadSkip(&h, 12, sizeof(U8);  }
  }
  if (!font->cmap.initialized || !font->head.initialized || !font->hhea.initialized || !font->hmtx.initialized) {
    LOG_ERROR("[FONT] Failed to parse font, ttf data is missing a required table.");
    return false;
  }
}

#endif // CDEFAULT_FONT_IMPLEMENTATION
