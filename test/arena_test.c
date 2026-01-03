#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

typedef struct TestStruct TestStruct;
struct TestStruct { U32 data; };

void ArenaAllocateTest(void) {
  Arena* arena = ArenaAllocate();

  EXPECT_U64_EQ(arena->pos, sizeof(Arena));
  EXPECT_U64_EQ(arena->reserve_size, CDEFAULT_ARENA_RESERVE_SIZE);
  EXPECT_U64_EQ(arena->commit_size, CDEFAULT_ARENA_COMMIT_SIZE);
  EXPECT_U64_EQ(arena->commit, CDEFAULT_ARENA_COMMIT_SIZE);

  ArenaRelease(arena);
}

void ArenaAllocateOverflowTest(void) {
  Arena* arena = _ArenaAllocate(PAGE_SIZE * 2, PAGE_SIZE);

  // NOTE: fill first arena to end.
  ARENA_PUSH_ARRAY(arena, U8, PAGE_SIZE - sizeof(Arena));
  EXPECT_U64_EQ(arena->pos, PAGE_SIZE);
  EXPECT_U64_EQ(arena->reserve_size, PAGE_SIZE * 2);
  EXPECT_U64_EQ(arena->commit_size, PAGE_SIZE);
  EXPECT_U64_EQ(arena->commit, PAGE_SIZE);

  // NOTE: push second arena, fill to end.
  ARENA_PUSH_ARRAY(arena, U8, PAGE_SIZE);
  EXPECT_U64_EQ(arena->pos, PAGE_SIZE * 2);
  EXPECT_U64_EQ(arena->reserve_size, PAGE_SIZE * 2);
  EXPECT_U64_EQ(arena->commit_size, PAGE_SIZE);
  EXPECT_U64_EQ(arena->commit, PAGE_SIZE * 2);

  ArenaRelease(arena);
}

void ArenaAllocateLargeSizeTest(void) {
  Arena* arena = _ArenaAllocate(PAGE_SIZE * 3, PAGE_SIZE);

  // NOTE: fill double the commit size
  ARENA_PUSH_ARRAY(arena, U8, PAGE_SIZE * 2);
  EXPECT_U64_EQ(arena->pos, PAGE_SIZE * 2 + sizeof(Arena));
  EXPECT_U64_EQ(arena->reserve_size, PAGE_SIZE * 3);
  EXPECT_U64_EQ(arena->commit_size, PAGE_SIZE);
  EXPECT_U64_EQ(arena->commit, PAGE_SIZE * 3);

  ArenaRelease(arena);
}

void ArenaAlignTest(void) {
  Arena* arena = ArenaAllocate();

  U8* test_arr = ARENA_PUSH_ARRAY(arena, U8, 15);
  EXPECT_U64_EQ(test_arr, ((U8*) arena) + sizeof(Arena));
  EXPECT_U64_EQ(arena->pos, sizeof(Arena) + sizeof(U8) * 15);
  U8* test_arr_2 = ARENA_PUSH_ARRAY(arena, U8, 16);
  // NOTE: Aligned to boundary, 16 not 15.
  EXPECT_U64_EQ(test_arr_2, ((U8*) arena) + sizeof(Arena) + 16);
  EXPECT_U64_EQ(arena->pos, sizeof(Arena) + sizeof(U8) * 32);

  ArenaRelease(arena);
}

void ArenaPushStructTest(void) {
  Arena* arena = ArenaAllocate();

  TestStruct* test_struct = ARENA_PUSH_STRUCT(arena, TestStruct);
  EXPECT_U64_EQ((U8*) test_struct, ((U8*) arena) + sizeof(Arena));
  EXPECT_U64_EQ(arena->pos, sizeof(Arena) + sizeof(TestStruct));

  ArenaRelease(arena);
}

void ArenaPushArrayTest(void) {
  Arena* arena = ArenaAllocate();

  U8* test_arr = ARENA_PUSH_ARRAY(arena, U8, 32);
  EXPECT_U64_EQ(test_arr, ((U8*) arena) + sizeof(Arena));
  EXPECT_U64_EQ(arena->pos, sizeof(Arena) + sizeof(U8) * 32);

  ArenaRelease(arena);
}

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  RUN_TEST(ArenaAllocateTest);
  RUN_TEST(ArenaAllocateOverflowTest);
  RUN_TEST(ArenaAllocateLargeSizeTest);
  RUN_TEST(ArenaAlignTest);
  RUN_TEST(ArenaPushStructTest);
  RUN_TEST(ArenaPushArrayTest);
  LogTestReport();
  return 0;
}
