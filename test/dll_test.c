#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

typedef struct Node Node;
struct Node { DLL_NODE(Node); };
typedef struct NodeChain NodeChain;
struct NodeChain { DLL_CONTROLLER(Node); };

B32 DllPushFrontTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_FRONT(&chain, &node_1);
  TEST_EXPECT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  TEST_EXPECT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_1);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == NULL);

  DLL_PUSH_FRONT(&chain, &node_2);
  TEST_EXPECT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  TEST_EXPECT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_2);
  TEST_EXPECT(DLL_BACK(&chain) == &node_1);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == &node_1);
  test = DLL_NEXT(test);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == &node_2);
  TEST_EXPECT(DLL_NEXT(test) == NULL);

  DLL_PUSH_FRONT(&chain, &node_3);
  TEST_EXPECT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  TEST_EXPECT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_3);
  TEST_EXPECT(DLL_BACK(&chain) == &node_1);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == &node_2);
  test = DLL_NEXT(test);
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(DLL_PREV(test) == &node_3);
  TEST_EXPECT(DLL_NEXT(test) == &node_1);
  test = DLL_NEXT(test);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == &node_2);
  TEST_EXPECT(DLL_NEXT(test) == NULL);

  return true;
}

B32 DllPushBackTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(&chain, &node_1);
  TEST_EXPECT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  TEST_EXPECT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_1);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == NULL);

  DLL_PUSH_BACK(&chain, &node_2);
  TEST_EXPECT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  TEST_EXPECT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_2);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == &node_2);
  test = DLL_NEXT(test);
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(DLL_PREV(test) == &node_1);
  TEST_EXPECT(DLL_NEXT(test) == NULL);

  DLL_PUSH_BACK(&chain, &node_3);
  TEST_EXPECT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  TEST_EXPECT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == &node_2);
  test = DLL_NEXT(test);
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(DLL_PREV(test) == &node_1);
  TEST_EXPECT(DLL_NEXT(test) == &node_3);
  test = DLL_NEXT(test);
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(DLL_PREV(test) == &node_2);
  TEST_EXPECT(DLL_NEXT(test) == NULL);

  return true;
}

B32 DllPopFrontTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(&chain, &node_1);
  DLL_PUSH_BACK(&chain, &node_2);
  DLL_PUSH_BACK(&chain, &node_3);

  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == &node_2);

  DLL_POP_FRONT(&chain);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_2);
  TEST_EXPECT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == &node_3);

  DLL_POP_FRONT(&chain);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_3);
  TEST_EXPECT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == NULL);

  DLL_POP_FRONT(&chain);
  TEST_EXPECT(DLL_FRONT(&chain) == NULL);
  TEST_EXPECT(DLL_BACK(&chain) == NULL);

  return true;
}

B32 DllPopBackTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(&chain, &node_1);
  DLL_PUSH_BACK(&chain, &node_2);
  DLL_PUSH_BACK(&chain, &node_3);

  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == &node_2);

  DLL_POP_BACK(&chain);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_2);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == &node_2);

  DLL_POP_BACK(&chain);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_1);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == NULL);

  DLL_POP_BACK(&chain);
  TEST_EXPECT(DLL_FRONT(&chain) == NULL);
  TEST_EXPECT(DLL_BACK(&chain) == NULL);

  return true;
}

B32 DllInsertRemoveMiddleTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(&chain, &node_1);
  DLL_PUSH_BACK(&chain, &node_3);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_3);

  DLL_INSERT(&chain, DLL_FRONT(&chain), &node_2);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(DLL_PREV(test) == NULL);
  TEST_EXPECT(DLL_NEXT(test) == &node_2);
  test = DLL_NEXT(test);
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(DLL_PREV(test) == &node_1);
  TEST_EXPECT(DLL_NEXT(test) == &node_3);
  test = DLL_NEXT(test);
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(DLL_PREV(test) == &node_2);
  TEST_EXPECT(DLL_NEXT(test) == NULL);

  DLL_REMOVE(&chain, &node_2);
  TEST_EXPECT(DLL_FRONT(&chain) == &node_1);
  TEST_EXPECT(DLL_BACK(&chain) == &node_3);
  TEST_EXPECT(DLL_NEXT(DLL_FRONT(&chain)) == &node_3);
  TEST_EXPECT(DLL_PREV(DLL_NEXT(DLL_FRONT(&chain))) == &node_1);

  return true;
}

int main(void) {
  TEST(DllPushFrontTest());
  TEST(DllPushBackTest());
  TEST(DllPopFrontTest());
  TEST(DllPopBackTest());
  TEST(DllInsertRemoveMiddleTest());
  return 0;
}
