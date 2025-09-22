#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

B32 ReadBETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinHead h;
  BinHeadInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  TEST_EXPECT(BinHeadR8(&h)    == 0x11);
  TEST_EXPECT(BinHeadR16BE(&h) == 0x2233);
  TEST_EXPECT(BinHeadR32BE(&h) == 0x44556677);
  TEST_EXPECT(BinHeadR64BE(&h) == 0x8899AABBCCDDEEFF);
  return true;
}

B32 ReadLETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinHead h;
  BinHeadInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  TEST_EXPECT(BinHeadR8(&h)    == 0x11);
  TEST_EXPECT(BinHeadR16LE(&h) == 0x3322);
  TEST_EXPECT(BinHeadR32LE(&h) == 0x77665544);
  TEST_EXPECT(BinHeadR64LE(&h) == 0xFFEEDDCCBBAA9988);
  return true;
}

B32 WriteBETest(void) {
  U8 buffer[15];
  BinHead h;
  BinHeadInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  BinHeadW8(&h, 0x11);
  BinHeadW16BE(&h, 0x2233);
  BinHeadW32BE(&h, 0x44556677);
  BinHeadW64BE(&h, 0x8899AABBCCDDEEFF);
  U8 expected_buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  TEST_EXPECT(IS_MEMORY_EQUAL(buffer, expected_buffer, 15));
  return true;
}

B32 WriteLETest(void) {
  U8 buffer[15];
  BinHead h;
  BinHeadInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  BinHeadW8(&h, 0x11);
  BinHeadW16LE(&h, 0x3322);
  BinHeadW32LE(&h, 0x77665544);
  BinHeadW64LE(&h, 0xFFEEDDCCBBAA9988);
  U8 expected_buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  TEST_EXPECT(IS_MEMORY_EQUAL(buffer, expected_buffer, 15));
  return true;
}

int main(void) {
  TEST(ReadBETest());
  TEST(ReadLETest());
  TEST(WriteBETest());
  TEST(WriteLETest());
  return 0;
}
