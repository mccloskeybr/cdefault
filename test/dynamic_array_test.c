#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_TEST_IMPLEMENTATION
#include "../cdefault_test.h"

void InsertTest(void) {
  U32 x;
  DynamicArray a;
  DynamicArrayInit(&a, sizeof(U32), 10);

  x = 10;
  EXPECT_TRUE(DynamicArrayInsert(&a, (U8*) &x, 0));
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(a.size, 1);

  x = 20;
  EXPECT_TRUE(DynamicArrayInsert(&a, (U8*) &x, 0));
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 10);
  EXPECT_U32_EQ(a.size, 2);

  x = 30;
  EXPECT_TRUE(DynamicArrayInsert(&a, (U8*) &x, 1));
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 30);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 10);
  EXPECT_U32_EQ(a.size, 3);

  x = 40;
  EXPECT_TRUE(DynamicArrayInsert(&a, (U8*) &x, 3));
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 30);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 3), 40);
  EXPECT_U32_EQ(a.size, 4);

  DynamicArrayDeinit(&a);
}

void SwapRemoveTest(void) {
  U32 x;
  DynamicArray a;
  DynamicArrayInit(&a, sizeof(U32), 10);

  x = 10;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 20;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 30;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 40;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 50;
  DynamicArrayPushBack(&a, (U8*) &x);

  EXPECT_U32_EQ(a.size, 5);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 30);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 3), 40);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 4), 50);

  EXPECT_TRUE(DynamicArraySwapRemove(&a, 0));
  EXPECT_U32_EQ(a.size, 4);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 50);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 30);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 3), 40);

  EXPECT_TRUE(DynamicArraySwapRemove(&a, 2));
  EXPECT_U32_EQ(a.size, 3);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 50);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 40);

  EXPECT_TRUE(DynamicArraySwapRemove(&a, 0));
  EXPECT_U32_EQ(a.size, 2);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 40);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 20);

  EXPECT_TRUE(DynamicArraySwapRemove(&a, 1));
  EXPECT_U32_EQ(a.size, 1);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 40);

  EXPECT_TRUE(DynamicArraySwapRemove(&a, 0));
  EXPECT_U32_EQ(a.size, 0);

  EXPECT_FALSE(DynamicArraySwapRemove(&a, 0));

  DynamicArrayDeinit(&a);
}

void RemoveTest(void) {
  U32 x;
  DynamicArray a;
  DynamicArrayInit(&a, sizeof(U32), 10);

  x = 10;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 20;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 30;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 40;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 50;
  DynamicArrayPushBack(&a, (U8*) &x);

  EXPECT_U32_EQ(a.size, 5);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 30);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 3), 40);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 4), 50);

  EXPECT_TRUE(DynamicArrayRemove(&a, 0));
  EXPECT_U32_EQ(a.size, 4);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 30);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 40);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 3), 50);

  EXPECT_TRUE(DynamicArrayRemove(&a, 2));
  EXPECT_U32_EQ(a.size, 3);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 30);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 50);

  EXPECT_TRUE(DynamicArrayRemove(&a, 0));
  EXPECT_U32_EQ(a.size, 2);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 30);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 50);

  EXPECT_TRUE(DynamicArrayRemove(&a, 1));
  EXPECT_U32_EQ(a.size, 1);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 30);

  EXPECT_TRUE(DynamicArrayRemove(&a, 0));
  EXPECT_U32_EQ(a.size, 0);

  EXPECT_FALSE(DynamicArrayRemove(&a, 0));

  DynamicArrayDeinit(&a);
}

void PushBackTest(void) {
  U32 x;
  DynamicArray a;
  DynamicArrayInit(&a, sizeof(U32), 10);

  x = 10;
  DynamicArrayPushBack(&a, (U8*) &x);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(a.size, 1);

  x = 20;
  DynamicArrayPushBack(&a, (U8*) &x);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 20);
  EXPECT_U32_EQ(a.size, 2);

  x = 30;
  DynamicArrayPushBack(&a, (U8*) &x);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 30);
  EXPECT_U32_EQ(a.size, 3);

  DynamicArrayDeinit(&a);
}

void PopBackTest(void) {
  U32 x;
  DynamicArray a;
  DynamicArrayInit(&a, sizeof(U32), 10);

  x = 10;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 20;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 30;
  DynamicArrayPushBack(&a, (U8*) &x);

  EXPECT_U32_EQ(a.size, 3);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 20);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 30);

  EXPECT_TRUE(DynamicArrayPopBack(&a));
  EXPECT_U32_EQ(a.size, 2);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 20);

  EXPECT_TRUE(DynamicArrayPopBack(&a));
  EXPECT_U32_EQ(a.size, 1);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);

  EXPECT_TRUE(DynamicArrayPopBack(&a));
  EXPECT_U32_EQ(a.size, 0);

  EXPECT_FALSE(DynamicArrayPopBack(&a));

  DynamicArrayDeinit(&a);
}

void ExpandCapacityTest(void) {
  U32 x;
  DynamicArray a;
  DynamicArrayInit(&a, sizeof(U32), 1);

  x = 10;
  DynamicArrayPushBack(&a, (U8*) &x);
  EXPECT_U32_EQ(a.size, 1);
  EXPECT_U32_EQ(a.capacity, 1);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);

  DynamicArrayPushBack(&a, (U8*) &x);
  EXPECT_U32_EQ(a.size, 2);
  EXPECT_U32_EQ(a.capacity, 2);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 10);

  DynamicArrayPushBack(&a, (U8*) &x);
  EXPECT_U32_EQ(a.size, 3);
  EXPECT_U32_EQ(a.capacity, 4);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 10);

  DynamicArrayPushBack(&a, (U8*) &x);
  DynamicArrayPushBack(&a, (U8*) &x);
  EXPECT_U32_EQ(a.size, 5);
  EXPECT_U32_EQ(a.capacity, 8);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 0), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 1), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 2), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 3), 10);
  EXPECT_U32_EQ(*(U32*) DynamicArrayGet(&a, 4), 10);

  DynamicArrayDeinit(&a);
}

int main(void) {
  RUN_TEST(InsertTest);
  RUN_TEST(SwapRemoveTest);
  RUN_TEST(RemoveTest);
  RUN_TEST(PushBackTest);
  RUN_TEST(PopBackTest);
  RUN_TEST(ExpandCapacityTest);
  LogTestReport();
  return 0;
}
