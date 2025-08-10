#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

struct TestStruct { U32 data; };

B32 ArenaAllocateTest(void) {
  Arena* arena = ArenaAllocate();
  TEST_EXPECT(arena->pos == 0);
  TEST_EXPECT(arena->capacity == KB(4));
  ArenaRelease(arena);
  return true;
}

B32 ArenaAllocateOverflowTest(void) {
  Arena* arena = ArenaAllocate();
  U64 whole_arena = KB(4) - sizeof(Arena);
  // NOTE: fill first arena to end.
  ARENA_PUSH_ARRAY(arena, U8, whole_arena);
  // NOTE: push second arena, fill to end.
  ARENA_PUSH_ARRAY(arena, U8, whole_arena);

  TEST_EXPECT(arena->capacity == KB(4));
  TEST_EXPECT(arena->base_pos == 0);
  TEST_EXPECT(arena->pos == whole_arena);

  TEST_EXPECT(arena->current->capacity == KB(4));
  TEST_EXPECT(arena->current->base_pos == whole_arena);
  TEST_EXPECT(arena->current->pos == whole_arena * 2);
  TEST_EXPECT(arena->current->current = arena);

  ArenaRelease(arena);
  return true;
}

B32 ArenaAllocateLargeSizeTest(void) {
  Arena* arena = ArenaAllocate();
  TEST_EXPECT(arena->capacity == KB(4));
  ARENA_PUSH_ARRAY(arena, U8, KB(5));

  TEST_EXPECT(arena->capacity == KB(4));
  TEST_EXPECT(arena->base_pos == 0);
  TEST_EXPECT(arena->pos == 0);

  TEST_EXPECT(arena->current->capacity == KB(5) + sizeof(Arena));
  TEST_EXPECT(arena->current->base_pos == 0);
  TEST_EXPECT(arena->current->pos == KB(5));
  TEST_EXPECT(arena->current->current = arena);

  ArenaRelease(arena);
  return true;
}

B32 ArenaAllocatePopToTest(void) {
  Arena* arena = ArenaAllocate();
  U64 whole_arena = KB(4) - sizeof(Arena);
  ARENA_PUSH_ARRAY(arena, U8, whole_arena);
  ARENA_PUSH_ARRAY(arena, U8, whole_arena);
  TEST_EXPECT(arena->base_pos == 0);
  TEST_EXPECT(arena->pos == whole_arena);
  TEST_EXPECT(arena->current->base_pos == whole_arena);
  TEST_EXPECT(arena->current->pos == 2 * whole_arena);

  ArenaPopTo(arena, whole_arena);
  TEST_EXPECT(arena->base_pos == 0);
  TEST_EXPECT(arena->pos == whole_arena);
  TEST_EXPECT(arena->current->base_pos == whole_arena);
  TEST_EXPECT(arena->current->pos == whole_arena);

  ArenaPopTo(arena, whole_arena / 2);
  TEST_EXPECT(arena->base_pos == 0);
  TEST_EXPECT(arena->pos == whole_arena / 2);
  TEST_EXPECT(arena->current == arena);

  ArenaRelease(arena);
  return true;
}

B32 ArenaAlignTest(void) {
  Arena* arena = ArenaAllocate();
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
  Arena* arena = ArenaAllocate();
  struct TestStruct* test_struct = ARENA_PUSH_STRUCT(arena, struct TestStruct);
  TEST_EXPECT((U8*) test_struct == ((U8*) arena) + sizeof(Arena));
  TEST_EXPECT(arena->pos = sizeof(struct TestStruct));
  ArenaRelease(arena);
  return true;
}

B32 ArenaPushArrayTest(void) {
  Arena* arena = ArenaAllocate();
  U8* test_arr = ARENA_PUSH_ARRAY(arena, U8, 32);
  TEST_EXPECT(test_arr == ((U8*) arena) + sizeof(Arena));
  TEST_EXPECT(arena->pos = sizeof(U8) * 32);
  ArenaRelease(arena);
  return true;
}

B32 TempArenaTest(void) {
  Arena* arena = ArenaAllocate();
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
  TEST(ArenaAllocateTest());
  TEST(ArenaAllocateOverflowTest());
  TEST(ArenaAllocateLargeSizeTest());
  TEST(ArenaAllocatePopToTest());
  TEST(ArenaAlignTest());
  TEST(ArenaPushStructTest());
  TEST(ArenaPushArrayTest());
  TEST(TempArenaTest());
  return 0;
}
