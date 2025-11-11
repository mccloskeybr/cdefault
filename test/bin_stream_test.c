#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

// TODO: test error bounds
// TODO: test signed read fns

static U8  u8;
static U16 u16;
static U32 u32;
static U64 u64;

void PullBETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinStream s;
  BinStreamInit(&s, buffer, STATIC_ARRAY_SIZE(buffer));

  EXPECT_TRUE(BinStreamPullU8(&s, &u8));
  EXPECT_U8_EQ(u8, 0x11);

  EXPECT_TRUE(BinStreamPullU16BE(&s, &u16));
  EXPECT_U16_EQ(u16, 0x2233);

  EXPECT_TRUE(BinStreamPullU32BE(&s, &u32));
  EXPECT_U32_EQ(u32, 0x44556677);

  EXPECT_TRUE(BinStreamPullU64BE(&s, &u64));
  EXPECT_U64_EQ(u64, 0x8899AABBCCDDEEFF);
}

void PullLETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinStream s;
  BinStreamInit(&s, buffer, STATIC_ARRAY_SIZE(buffer));

  EXPECT_TRUE(BinStreamPullU8(&s, &u8));
  EXPECT_U8_EQ(u8, 0x11);

  EXPECT_TRUE(BinStreamPullU16LE(&s, &u16));
  EXPECT_U16_EQ(u16, 0x3322);

  EXPECT_TRUE(BinStreamPullU32LE(&s, &u32));
  EXPECT_U32_EQ(u32, 0x77665544);

  EXPECT_TRUE(BinStreamPullU64LE(&s, &u64));
  EXPECT_U64_EQ(u64, 0xFFEEDDCCBBAA9988);
}

void PeekBETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinStream s;
  BinStreamInit(&s, buffer, STATIC_ARRAY_SIZE(buffer));
  EXPECT_U8_EQ(BinStreamPeek8(&s, 0, sizeof(U8)),     0x11);
  EXPECT_U16_EQ(BinStreamPeek16BE(&s, 1, sizeof(U8)), 0x2233);
  EXPECT_U32_EQ(BinStreamPeek32BE(&s, 3, sizeof(U8)), 0x44556677);
  EXPECT_U64_EQ(BinStreamPeek64BE(&s, 7, sizeof(U8)), 0x8899AABBCCDDEEFF);
}

void PeekLETest(void) {
  U8 buffer[15] = {
    0x11,
    0x22, 0x33,
    0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
  BinStream s;
  BinStreamInit(&s, buffer, STATIC_ARRAY_SIZE(buffer));
  EXPECT_U8_EQ(BinStreamPeek8(&s, 0, sizeof(U8)),    0x11);
  EXPECT_U8_EQ(BinStreamPeek16LE(&s, 1, sizeof(U8)), 0x3322);
  EXPECT_U8_EQ(BinStreamPeek32LE(&s, 3, sizeof(U8)), 0x77665544);
  EXPECT_U8_EQ(BinStreamPeek64LE(&s, 7, sizeof(U8)), 0xFFEEDDCCBBAA9988);
}

void PushBETest(void) {
  U8 buffer[15];
  BinStream s;
  BinStreamInit(&s, buffer, STATIC_ARRAY_SIZE(buffer));
  BinStreamPush8(&s, 0x11);
  BinStreamPush16BE(&s, 0x2233);
  BinStreamPush32BE(&s, 0x44556677);
  BinStreamPush64BE(&s, 0x8899AABBCCDDEEFF);
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
  BinStream s;
  BinStreamInit(&s, buffer, STATIC_ARRAY_SIZE(buffer));
  BinStreamPush8(&s, 0x11);
  BinStreamPush16LE(&s, 0x3322);
  BinStreamPush32LE(&s, 0x77665544);
  BinStreamPush64LE(&s, 0xFFEEDDCCBBAA9988);
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
