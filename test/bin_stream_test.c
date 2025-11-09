#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

void PullBETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinStream h;
  BinStreamInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  EXPECT_U8_EQ(BinStreamPull8(&h),     0x11);
  EXPECT_U16_EQ(BinStreamPull16BE(&h), 0x2233);
  EXPECT_U32_EQ(BinStreamPull32BE(&h), 0x44556677);
  EXPECT_U64_EQ(BinStreamPull64BE(&h), 0x8899AABBCCDDEEFF);
}

void PullLETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinStream h;
  BinStreamInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  EXPECT_U8_EQ(BinStreamPull8(&h),     0x11);
  EXPECT_U16_EQ(BinStreamPull16LE(&h), 0x3322);
  EXPECT_U32_EQ(BinStreamPull32LE(&h), 0x77665544);
  EXPECT_U64_EQ(BinStreamPull64LE(&h), 0xFFEEDDCCBBAA9988);
}

void PeekBETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinStream h;
  BinStreamInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  EXPECT_U8_EQ(BinStreamPeek8(&h, 0, sizeof(U8)),     0x11);
  EXPECT_U16_EQ(BinStreamPeek16BE(&h, 1, sizeof(U8)), 0x2233);
  EXPECT_U32_EQ(BinStreamPeek32BE(&h, 3, sizeof(U8)), 0x44556677);
  EXPECT_U64_EQ(BinStreamPeek64BE(&h, 7, sizeof(U8)), 0x8899AABBCCDDEEFF);
}

void PeekLETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinStream h;
  BinStreamInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  EXPECT_U8_EQ(BinStreamPeek8(&h, 0, sizeof(U8)),    0x11);
  EXPECT_U8_EQ(BinStreamPeek16LE(&h, 1, sizeof(U8)), 0x3322);
  EXPECT_U8_EQ(BinStreamPeek32LE(&h, 3, sizeof(U8)), 0x77665544);
  EXPECT_U8_EQ(BinStreamPeek64LE(&h, 7, sizeof(U8)), 0xFFEEDDCCBBAA9988);
}

void PushBETest(void) {
  U8 buffer[15];
  BinStream h;
  BinStreamInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  BinStreamPush8(&h, 0x11);
  BinStreamPush16BE(&h, 0x2233);
  BinStreamPush32BE(&h, 0x44556677);
  BinStreamPush64BE(&h, 0x8899AABBCCDDEEFF);
  U8 expected_buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  EXPECT_TRUE(IS_MEMORY_EQUAL(buffer, expected_buffer, 15));
}

void PushLETest(void) {
  U8 buffer[15];
  BinStream h;
  BinStreamInit(&h, buffer, STATIC_ARRAY_SIZE(buffer));
  BinStreamPush8(&h, 0x11);
  BinStreamPush16LE(&h, 0x3322);
  BinStreamPush32LE(&h, 0x77665544);
  BinStreamPush64LE(&h, 0xFFEEDDCCBBAA9988);
  U8 expected_buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  EXPECT_TRUE(IS_MEMORY_EQUAL(buffer, expected_buffer, 15));
}

int main(void) {
  RUN_TEST(PullBETest);
  RUN_TEST(PullLETest);
  RUN_TEST(PeekBETest);
  RUN_TEST(PeekLETest);
  RUN_TEST(PushBETest);
  RUN_TEST(PushLETest);
  LogTestReport();
  return 0;
}
