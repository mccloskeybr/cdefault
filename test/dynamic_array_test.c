#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_TEST_IMPLEMENTATION
#include "../cdefault_test.h"

typedef struct List List;
struct List {
  U16* data;
  U32 size;
  U32 capacity;
};

Arena* arena;

void InsertTest(void) {
  List list;
  MEMORY_ZERO_STRUCT(&list);

  DA_INSERT(arena, &list, 10, 0);
  EXPECT_U32_EQ(list.size, 1);
  EXPECT_U32_EQ(list.data[0], 10);

  DA_INSERT(arena, &list, 20, 0);
  EXPECT_U32_EQ(list.size, 2);
  EXPECT_U32_EQ(list.data[0], 20);
  EXPECT_U32_EQ(list.data[1], 10);

  DA_INSERT(arena, &list, 30, 1);
  EXPECT_U32_EQ(list.size, 3);
  EXPECT_U32_EQ(list.data[0], 20);
  EXPECT_U32_EQ(list.data[1], 30);
  EXPECT_U32_EQ(list.data[2], 10);

  DA_INSERT(arena, &list, 40, 3);
  EXPECT_U32_EQ(list.size, 4);
  EXPECT_U32_EQ(list.data[0], 20);
  EXPECT_U32_EQ(list.data[1], 30);
  EXPECT_U32_EQ(list.data[2], 10);
  EXPECT_U32_EQ(list.data[3], 40);
}

void SwapRemoveTest(void) {
  List list;
  MEMORY_ZERO_STRUCT(&list);

  DA_PUSH_BACK(arena, &list, 10);
  DA_PUSH_BACK(arena, &list, 20);
  DA_PUSH_BACK(arena, &list, 30);
  DA_PUSH_BACK(arena, &list, 40);
  DA_PUSH_BACK(arena, &list, 50);

  EXPECT_U32_EQ(list.size, 5);
  EXPECT_U32_EQ(list.data[0], 10);
  EXPECT_U32_EQ(list.data[1], 20);
  EXPECT_U32_EQ(list.data[2], 30);
  EXPECT_U32_EQ(list.data[3], 40);
  EXPECT_U32_EQ(list.data[4], 50);

  DA_SWAP_REMOVE(&list, 0);
  EXPECT_U32_EQ(list.size, 4);
  EXPECT_U32_EQ(list.data[0], 50);
  EXPECT_U32_EQ(list.data[1], 20);
  EXPECT_U32_EQ(list.data[2], 30);
  EXPECT_U32_EQ(list.data[3], 40);

  DA_SWAP_REMOVE(&list, 2);
  EXPECT_U32_EQ(list.size, 3);
  EXPECT_U32_EQ(list.data[0], 50);
  EXPECT_U32_EQ(list.data[1], 20);
  EXPECT_U32_EQ(list.data[2], 40);

  DA_SWAP_REMOVE(&list, 0);
  EXPECT_U32_EQ(list.size, 2);
  EXPECT_U32_EQ(list.data[0], 40);
  EXPECT_U32_EQ(list.data[1], 20);

  DA_SWAP_REMOVE(&list, 1);
  EXPECT_U32_EQ(list.size, 1);
  EXPECT_U32_EQ(list.data[0], 40);

  DA_SWAP_REMOVE(&list, 0);
  EXPECT_U32_EQ(list.size, 0);
}

void RemoveTest(void) {
  List list;
  MEMORY_ZERO_STRUCT(&list);

  DA_PUSH_BACK(arena, &list, 10);
  DA_PUSH_BACK(arena, &list, 20);
  DA_PUSH_BACK(arena, &list, 30);
  DA_PUSH_BACK(arena, &list, 40);
  DA_PUSH_BACK(arena, &list, 50);

  EXPECT_U32_EQ(list.size, 5);
  EXPECT_U32_EQ(list.data[0], 10);
  EXPECT_U32_EQ(list.data[1], 20);
  EXPECT_U32_EQ(list.data[2], 30);
  EXPECT_U32_EQ(list.data[3], 40);
  EXPECT_U32_EQ(list.data[4], 50);

  DA_SHIFT_REMOVE(&list, 0);
  EXPECT_U32_EQ(list.size, 4);
  EXPECT_U32_EQ(list.data[0], 20);
  EXPECT_U32_EQ(list.data[1], 30);
  EXPECT_U32_EQ(list.data[2], 40);
  EXPECT_U32_EQ(list.data[3], 50);

  DA_SHIFT_REMOVE(&list, 2);
  EXPECT_U32_EQ(list.size, 3);
  EXPECT_U32_EQ(list.data[0], 20);
  EXPECT_U32_EQ(list.data[1], 30);
  EXPECT_U32_EQ(list.data[2], 50);

  DA_SHIFT_REMOVE(&list, 0);
  EXPECT_U32_EQ(list.size, 2);
  EXPECT_U32_EQ(list.data[0], 30);
  EXPECT_U32_EQ(list.data[1], 50);

  DA_SHIFT_REMOVE(&list, 1);
  EXPECT_U32_EQ(list.size, 1);
  EXPECT_U32_EQ(list.data[0], 30);

  DA_SHIFT_REMOVE(&list, 0);
  EXPECT_U32_EQ(list.size, 0);
}

void PushBackTest(void) {
  List list;
  MEMORY_ZERO_STRUCT(&list);

  DA_PUSH_BACK(arena, &list, 10);
  EXPECT_U32_EQ(list.size, 1);
  EXPECT_U32_EQ(list.data[0], 10);

  DA_PUSH_BACK(arena, &list, 20);
  EXPECT_U32_EQ(list.size, 2);
  EXPECT_U32_EQ(list.data[0], 10);
  EXPECT_U32_EQ(list.data[1], 20);

  DA_PUSH_BACK(arena, &list, 30);
  EXPECT_U32_EQ(list.size, 3);
  EXPECT_U32_EQ(list.data[0], 10);
  EXPECT_U32_EQ(list.data[1], 20);
  EXPECT_U32_EQ(list.data[2], 30);
}

void PopBackTest(void) {
  List list;
  MEMORY_ZERO_STRUCT(&list);

  DA_PUSH_BACK(arena, &list, 10);
  DA_PUSH_BACK(arena, &list, 20);
  DA_PUSH_BACK(arena, &list, 30);

  EXPECT_U32_EQ(list.size, 3);
  EXPECT_U32_EQ(list.data[0], 10);
  EXPECT_U32_EQ(list.data[1], 20);
  EXPECT_U32_EQ(list.data[2], 30);

  DA_POP_BACK(&list);
  EXPECT_U32_EQ(list.size, 2);
  EXPECT_U32_EQ(list.data[0], 10);
  EXPECT_U32_EQ(list.data[1], 20);

  DA_POP_BACK(&list);
  EXPECT_U32_EQ(list.size, 1);
  EXPECT_U32_EQ(list.data[0], 10);

  DA_POP_BACK(&list);
  EXPECT_U32_EQ(list.size, 0);
}

void ExpandCapacityTest(void) {
  List list;
  MEMORY_ZERO_STRUCT(&list);

  for (U32 i = 0; i < DA_INITIAL_CAPACITY; i++) {
    DA_PUSH_BACK(arena, &list, i * 10);
    EXPECT_U32_EQ(list.capacity, DA_INITIAL_CAPACITY);
    EXPECT_U32_EQ(list.size, i + 1);
    EXPECT_U16_EQ(list.data[i], i * 10);
  }

  for (U32 i = 0; i < DA_INITIAL_CAPACITY; i++) {
    DA_PUSH_BACK(arena, &list, i * 20);
    EXPECT_U32_EQ(list.capacity, DA_INITIAL_CAPACITY * 2);
    EXPECT_U32_EQ(list.size, DA_INITIAL_CAPACITY + i + 1);
    EXPECT_U16_EQ(list.data[DA_INITIAL_CAPACITY + i], i * 20);
  }
}

int main(void) {
  arena = ArenaAllocate();
  RUN_TEST(InsertTest);
  RUN_TEST(SwapRemoveTest);
  RUN_TEST(RemoveTest);
  RUN_TEST(PushBackTest);
  RUN_TEST(PopBackTest);
  RUN_TEST(ExpandCapacityTest);
  LogTestReport();
  return 0;
}
