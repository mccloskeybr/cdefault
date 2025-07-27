#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

typedef struct Node Node;
struct Node { DLL_NODE(Node); };
typedef struct NodeChain NodeChain;
struct NodeChain { DLL_CONTROLLER(Node); };

void DllPushFrontTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_FRONT(&chain, &node_1);
  ASSERT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  ASSERT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_1);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == NULL);

  DLL_PUSH_FRONT(&chain, &node_2);
  ASSERT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  ASSERT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  ASSERT(DLL_FRONT(&chain) == &node_2);
  ASSERT(DLL_BACK(&chain) == &node_1);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_2);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == &node_1);
  test = DLL_NEXT(test);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == &node_2);
  ASSERT(DLL_NEXT(test) == NULL);

  DLL_PUSH_FRONT(&chain, &node_3);
  ASSERT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  ASSERT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  ASSERT(DLL_FRONT(&chain) == &node_3);
  ASSERT(DLL_BACK(&chain) == &node_1);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_3);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == &node_2);
  test = DLL_NEXT(test);
  ASSERT(test == &node_2);
  ASSERT(DLL_PREV(test) == &node_3);
  ASSERT(DLL_NEXT(test) == &node_1);
  test = DLL_NEXT(test);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == &node_2);
  ASSERT(DLL_NEXT(test) == NULL);

  LOG_INFO("[SUCCESS] DllPushFrontTest.");
}

void DllPushBackTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(&chain, &node_1);
  ASSERT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  ASSERT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_1);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == NULL);

  DLL_PUSH_BACK(&chain, &node_2);
  ASSERT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  ASSERT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_2);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == &node_2);
  test = DLL_NEXT(test);
  ASSERT(test == &node_2);
  ASSERT(DLL_PREV(test) == &node_1);
  ASSERT(DLL_NEXT(test) == NULL);

  DLL_PUSH_BACK(&chain, &node_3);
  ASSERT(DLL_PREV(DLL_FRONT(&chain)) == NULL);
  ASSERT(DLL_NEXT(DLL_BACK(&chain)) == NULL);
  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == &node_2);
  test = DLL_NEXT(test);
  ASSERT(test == &node_2);
  ASSERT(DLL_PREV(test) == &node_1);
  ASSERT(DLL_NEXT(test) == &node_3);
  test = DLL_NEXT(test);
  ASSERT(test == &node_3);
  ASSERT(DLL_PREV(test) == &node_2);
  ASSERT(DLL_NEXT(test) == NULL);

  LOG_INFO("[SUCCESS] DllPushBackTest.");
}

void DllPopFrontTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(&chain, &node_1);
  DLL_PUSH_BACK(&chain, &node_2);
  DLL_PUSH_BACK(&chain, &node_3);

  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == &node_2);

  DLL_POP_FRONT(&chain);
  ASSERT(DLL_FRONT(&chain) == &node_2);
  ASSERT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_2);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == &node_3);

  DLL_POP_FRONT(&chain);
  ASSERT(DLL_FRONT(&chain) == &node_3);
  ASSERT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_3);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == NULL);

  DLL_POP_FRONT(&chain);
  ASSERT(DLL_FRONT(&chain) == NULL);
  ASSERT(DLL_BACK(&chain) == NULL);

  LOG_INFO("[SUCCESS] DllPopFrontTest.");
}

void DllPopBackTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(&chain, &node_1);
  DLL_PUSH_BACK(&chain, &node_2);
  DLL_PUSH_BACK(&chain, &node_3);

  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == &node_2);

  DLL_POP_BACK(&chain);
  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_2);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == &node_2);

  DLL_POP_BACK(&chain);
  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_1);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == NULL);

  DLL_POP_BACK(&chain);
  ASSERT(DLL_FRONT(&chain) == NULL);
  ASSERT(DLL_BACK(&chain) == NULL);

  LOG_INFO("[SUCCESS] DllPopBackTest.");
}

void DllInsertRemoveMiddleTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(&chain, &node_1);
  DLL_PUSH_BACK(&chain, &node_3);
  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_3);

  DLL_INSERT(&chain, DLL_FRONT(&chain), &node_2);
  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_3);
  test = DLL_FRONT(&chain);
  ASSERT(test == &node_1);
  ASSERT(DLL_PREV(test) == NULL);
  ASSERT(DLL_NEXT(test) == &node_2);
  test = DLL_NEXT(test);
  ASSERT(test == &node_2);
  ASSERT(DLL_PREV(test) == &node_1);
  ASSERT(DLL_NEXT(test) == &node_3);
  test = DLL_NEXT(test);
  ASSERT(test == &node_3);
  ASSERT(DLL_PREV(test) == &node_2);
  ASSERT(DLL_NEXT(test) == NULL);

  DLL_REMOVE(&chain, &node_2);
  ASSERT(DLL_FRONT(&chain) == &node_1);
  ASSERT(DLL_BACK(&chain) == &node_3);
  ASSERT(DLL_NEXT(DLL_FRONT(&chain)) == &node_3);
  ASSERT(DLL_PREV(DLL_NEXT(DLL_FRONT(&chain))) == &node_1);

  LOG_INFO("[SUCCESS] DllInsertRemoveMiddleTest.");
}

int main(void) {
  DllPushFrontTest();
  DllPushBackTest();
  DllPopFrontTest();
  DllPopBackTest();
  DllInsertRemoveMiddleTest();
  return 0;
}
