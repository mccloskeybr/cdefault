#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

B32 InsertTest(void) {
  U32 x;
  DynamicArray a;
  DynamicArrayInit(&a, sizeof(U32), 10);

  x = 10;
  TEST_EXPECT(DynamicArrayInsert(&a, (U8*) &x, 0));
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(a.size == 1);

  x = 20;
  TEST_EXPECT(DynamicArrayInsert(&a, (U8*) &x, 0));
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 10);
  TEST_EXPECT(a.size == 2);

  x = 30;
  TEST_EXPECT(DynamicArrayInsert(&a, (U8*) &x, 1));
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 30);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 10);
  TEST_EXPECT(a.size == 3);

  x = 40;
  TEST_EXPECT(DynamicArrayInsert(&a, (U8*) &x, 3));
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 30);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 3) == 40);
  TEST_EXPECT(a.size == 4);

  DynamicArrayDeinit(&a);
  return true;
}

B32 SwapRemoveTest(void) {
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

  TEST_EXPECT(a.size == 5);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 30);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 3) == 40);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 4) == 50);

  TEST_EXPECT(DynamicArraySwapRemove(&a, 0));
  TEST_EXPECT(a.size == 4);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 50);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 30);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 3) == 40);

  TEST_EXPECT(DynamicArraySwapRemove(&a, 2));
  TEST_EXPECT(a.size == 3);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 50);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 40);

  TEST_EXPECT(DynamicArraySwapRemove(&a, 0));
  TEST_EXPECT(a.size == 2);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 40);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 20);

  TEST_EXPECT(DynamicArraySwapRemove(&a, 1));
  TEST_EXPECT(a.size == 1);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 40);

  TEST_EXPECT(DynamicArraySwapRemove(&a, 0));
  TEST_EXPECT(a.size == 0);

  TEST_EXPECT(DynamicArraySwapRemove(&a, 0) == false);

  DynamicArrayDeinit(&a);
  return true;
}

B32 RemoveTest(void) {
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

  TEST_EXPECT(a.size == 5);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 30);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 3) == 40);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 4) == 50);

  TEST_EXPECT(DynamicArrayRemove(&a, 0));
  TEST_EXPECT(a.size == 4);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 30);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 40);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 3) == 50);

  TEST_EXPECT(DynamicArrayRemove(&a, 2));
  TEST_EXPECT(a.size == 3);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 30);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 50);

  TEST_EXPECT(DynamicArrayRemove(&a, 0));
  TEST_EXPECT(a.size == 2);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 30);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 50);

  TEST_EXPECT(DynamicArrayRemove(&a, 1));
  TEST_EXPECT(a.size == 1);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 30);

  TEST_EXPECT(DynamicArrayRemove(&a, 0));
  TEST_EXPECT(a.size == 0);

  TEST_EXPECT(DynamicArrayRemove(&a, 0) == false);

  DynamicArrayDeinit(&a);
  return true;
}

B32 PushBackTest(void) {
  U32 x;
  DynamicArray a;
  DynamicArrayInit(&a, sizeof(U32), 10);

  x = 10;
  DynamicArrayPushBack(&a, (U8*) &x);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(a.size == 1);

  x = 20;
  DynamicArrayPushBack(&a, (U8*) &x);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 20);
  TEST_EXPECT(a.size == 2);

  x = 30;
  DynamicArrayPushBack(&a, (U8*) &x);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 30);
  TEST_EXPECT(a.size == 3);

  DynamicArrayDeinit(&a);
  return true;
}

B32 PopBackTest(void) {
  U32 x;
  DynamicArray a;
  DynamicArrayInit(&a, sizeof(U32), 10);

  x = 10;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 20;
  DynamicArrayPushBack(&a, (U8*) &x);
  x = 30;
  DynamicArrayPushBack(&a, (U8*) &x);

  TEST_EXPECT(a.size == 3);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 20);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 30);

  TEST_EXPECT(DynamicArrayPopBack(&a));
  TEST_EXPECT(a.size == 2);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 20);

  TEST_EXPECT(DynamicArrayPopBack(&a));
  TEST_EXPECT(a.size == 1);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);

  TEST_EXPECT(DynamicArrayPopBack(&a));
  TEST_EXPECT(a.size == 0);

  TEST_EXPECT(DynamicArrayPopBack(&a) == false);

  DynamicArrayDeinit(&a);
  return true;
}

B32 ExpandCapacityTest(void) {
  U32 x;
  DynamicArray a;
  DynamicArrayInit(&a, sizeof(U32), 1);

  x = 10;
  DynamicArrayPushBack(&a, (U8*) &x);
  TEST_EXPECT(a.size == 1);
  TEST_EXPECT(a.capacity == 1);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);

  DynamicArrayPushBack(&a, (U8*) &x);
  TEST_EXPECT(a.size == 2);
  TEST_EXPECT(a.capacity == 2);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 10);

  DynamicArrayPushBack(&a, (U8*) &x);
  TEST_EXPECT(a.size == 3);
  TEST_EXPECT(a.capacity == 4);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 10);

  DynamicArrayPushBack(&a, (U8*) &x);
  DynamicArrayPushBack(&a, (U8*) &x);
  TEST_EXPECT(a.size == 5);
  TEST_EXPECT(a.capacity == 8);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 0) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 1) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 2) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 3) == 10);
  TEST_EXPECT(*(U32*) DynamicArrayGet(&a, 4) == 10);

  DynamicArrayDeinit(&a);
  return true;
}

int main(void) {
  TEST(InsertTest());
  TEST(SwapRemoveTest());
  TEST(RemoveTest());
  TEST(PushBackTest());
  TEST(PopBackTest());
  TEST(ExpandCapacityTest());
  return 0;
}
