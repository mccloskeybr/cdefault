#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

struct TestStruct { U32 data; };

B32 ArenaCreateTest(void) {
  Arena* arena = ArenaCreate(KB(1));
  TEST_EXPECT(arena->pos == 0);
  TEST_EXPECT(arena->capacity == KB(1));
  ArenaRelease(arena);
  return true;
}

B32 ArenaAlignTest(void) {
  Arena* arena = ArenaCreate(KB(1));
  U8* test_arr = ARENA_PUSH_ARRAY(arena, U8, 15);
  TEST_EXPECT(test_arr == ((U8*) arena) + sizeof(Arena));
  TEST_EXPECT(arena->pos = sizeof(U8) * 15);
  U8* test_arr_2 = ARENA_PUSH_ARRAY(arena, U8, 16);
  // NOTE: Aligned to boundary, 16 not 15.
  TEST_EXPECT(test_arr_2 == ((U8*) arena) + sizeof(Arena) + 16);
  TEST_EXPECT(arena->pos = sizeof(U8) * 32);
  ArenaRelease(arena);
  return true;
}

B32 ArenaPushStructTest(void) {
  Arena* arena = ArenaCreate(KB(1));
  struct TestStruct* test_struct = ARENA_PUSH_STRUCT(arena, struct TestStruct);
  TEST_EXPECT((U8*) test_struct == ((U8*) arena) + sizeof(Arena));
  TEST_EXPECT(arena->pos = sizeof(struct TestStruct));
  ArenaRelease(arena);
  return true;
}

B32 ArenaPushArrayTest(void) {
  Arena* arena = ArenaCreate(KB(1));
  U8* test_arr = ARENA_PUSH_ARRAY(arena, U8, 32);
  TEST_EXPECT(test_arr == ((U8*) arena) + sizeof(Arena));
  TEST_EXPECT(arena->pos = sizeof(U8) * 32);
  ArenaRelease(arena);
  return true;
}

B32 TempArenaTest(void) {
  TEST_EXPECT(true == false);
  Arena* arena = ArenaCreate(KB(1));
  U8* buffer = ARENA_PUSH_ARRAY(arena, U8, 32);
  ArenaTemp temp = ArenaTempBegin(arena);
  U8* temp_buffer = ARENA_PUSH_ARRAY(temp.arena, U8, 32);
  TEST_EXPECT(temp_buffer == ((U8*) arena) + sizeof(Arena) + 32);
  TEST_EXPECT(arena->pos == 64);
  ArenaTempEnd(&temp);
  TEST_EXPECT(arena->pos == 32);
  ArenaRelease(arena);
  return true;
}

int main(void) {
  TEST(ArenaCreateTest());
  TEST(ArenaAlignTest());
  TEST(ArenaPushStructTest());
  TEST(ArenaPushArrayTest());
  TEST(TempArenaTest());
  return 0;
}
