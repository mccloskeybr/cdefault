#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

typedef struct TestStruct TestStruct;
struct TestStruct { U32 data; };

B32 ArenaAllocateTest(void) {
  Arena* arena = ArenaAllocate();

  TEST_EXPECT(arena->pos == sizeof(Arena));
  TEST_EXPECT(arena->reserve_size == CDEFAULT_ARENA_RESERVE_SIZE);
  TEST_EXPECT(arena->commit_size == CDEFAULT_ARENA_COMMIT_SIZE);
  TEST_EXPECT(arena->commit == CDEFAULT_ARENA_COMMIT_SIZE);

  ArenaRelease(arena);
  return true;
}

B32 ArenaAllocateOverflowTest(void) {
  Arena* arena = _ArenaAllocate(PAGE_SIZE * 2, PAGE_SIZE);

  // NOTE: fill first arena to end.
  ARENA_PUSH_ARRAY(arena, U8, PAGE_SIZE - sizeof(Arena));
  TEST_EXPECT(arena->pos == PAGE_SIZE);
  TEST_EXPECT(arena->reserve_size == PAGE_SIZE * 2);
  TEST_EXPECT(arena->commit_size == PAGE_SIZE);
  TEST_EXPECT(arena->commit == PAGE_SIZE);

  // NOTE: push second arena, fill to end.
  ARENA_PUSH_ARRAY(arena, U8, PAGE_SIZE);
  TEST_EXPECT(arena->pos == PAGE_SIZE * 2);
  TEST_EXPECT(arena->reserve_size == PAGE_SIZE * 2);
  TEST_EXPECT(arena->commit_size == PAGE_SIZE);
  TEST_EXPECT(arena->commit == PAGE_SIZE * 2);

  ArenaRelease(arena);
  return true;
}

B32 ArenaAllocateLargeSizeTest(void) {
  Arena* arena = _ArenaAllocate(PAGE_SIZE * 3, PAGE_SIZE);

  // NOTE: fill double the commit size
  ARENA_PUSH_ARRAY(arena, U8, PAGE_SIZE * 2);
  TEST_EXPECT(arena->pos == PAGE_SIZE * 2 + sizeof(Arena));
  TEST_EXPECT(arena->reserve_size == PAGE_SIZE * 3);
  TEST_EXPECT(arena->commit_size == PAGE_SIZE);
  TEST_EXPECT(arena->commit == PAGE_SIZE * 3);

  ArenaRelease(arena);
  return true;
}

B32 ArenaAlignTest(void) {
  Arena* arena = ArenaAllocate();

  U8* test_arr = ARENA_PUSH_ARRAY(arena, U8, 15);
  TEST_EXPECT(test_arr == ((U8*) arena) + sizeof(Arena));
  TEST_EXPECT(arena->pos == sizeof(Arena) + sizeof(U8) * 15);
  U8* test_arr_2 = ARENA_PUSH_ARRAY(arena, U8, 16);
  // NOTE: Aligned to boundary, 16 not 15.
  TEST_EXPECT(test_arr_2 == ((U8*) arena) + sizeof(Arena) + 16);
  TEST_EXPECT(arena->pos == sizeof(Arena) + sizeof(U8) * 32);

  ArenaRelease(arena);
  return true;
}

B32 ArenaPushStructTest(void) {
  Arena* arena = ArenaAllocate();

  TestStruct* test_struct = ARENA_PUSH_STRUCT(arena, TestStruct);
  TEST_EXPECT((U8*) test_struct == ((U8*) arena) + sizeof(Arena));
  TEST_EXPECT(arena->pos == sizeof(Arena) + sizeof(TestStruct));

  ArenaRelease(arena);
  return true;
}

B32 ArenaPushArrayTest(void) {
  Arena* arena = ArenaAllocate();

  U8* test_arr = ARENA_PUSH_ARRAY(arena, U8, 32);
  TEST_EXPECT(test_arr == ((U8*) arena) + sizeof(Arena));
  TEST_EXPECT(arena->pos == sizeof(Arena) + sizeof(U8) * 32);

  ArenaRelease(arena);
  return true;
}

B32 TempArenaTest(void) {
  Arena* arena = ArenaAllocate();

  ARENA_PUSH_ARRAY(arena, U8, 32);
  ArenaTemp temp = ArenaTempBegin(arena);
  U8* temp_buffer = ARENA_PUSH_ARRAY(temp.arena, U8, 32);
  TEST_EXPECT(temp_buffer == ((U8*) arena) + sizeof(Arena) + 32);
  TEST_EXPECT(arena->pos == sizeof(Arena) + 64);
  ArenaTempEnd(&temp);
  TEST_EXPECT(arena->pos == sizeof(Arena) + 32);

  ArenaRelease(arena);
  return true;
}

int main(void) {
  TEST(ArenaAllocateTest());
  TEST(ArenaAllocateOverflowTest());
  TEST(ArenaAllocateLargeSizeTest());
  TEST(ArenaAlignTest());
  TEST(ArenaPushStructTest());
  TEST(ArenaPushArrayTest());
  TEST(TempArenaTest());
  return 0;
}
