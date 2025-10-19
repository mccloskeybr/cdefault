#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_TEST_IMPLEMENTATION
#include "../cdefault_test.h"

void ReadBETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinHead h;
  BinHeadInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  EXPECT_U8_EQ(BinHeadR8(&h),     0x11);
  EXPECT_U16_EQ(BinHeadR16BE(&h), 0x2233);
  EXPECT_U32_EQ(BinHeadR32BE(&h), 0x44556677);
  EXPECT_U64_EQ(BinHeadR64BE(&h), 0x8899AABBCCDDEEFF);
}

void ReadLETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinHead h;
  BinHeadInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  EXPECT_U8_EQ(BinHeadR8(&h),     0x11);
  EXPECT_U16_EQ(BinHeadR16LE(&h), 0x3322);
  EXPECT_U32_EQ(BinHeadR32LE(&h), 0x77665544);
  EXPECT_U64_EQ(BinHeadR64LE(&h), 0xFFEEDDCCBBAA9988);
}

void WriteBETest(void) {
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
  EXPECT_TRUE(IS_MEMORY_EQUAL(buffer, expected_buffer, 15));
}

void WriteLETest(void) {
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
  EXPECT_TRUE(IS_MEMORY_EQUAL(buffer, expected_buffer, 15));
}

int main(void) {
  RUN_TEST(ReadBETest);
  RUN_TEST(ReadLETest);
  RUN_TEST(WriteBETest);
  RUN_TEST(WriteLETest);
  LogTestReport();
  return 0;
}
