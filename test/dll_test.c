#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

typedef struct Node Node;
struct Node {
  Node* prev;
  Node* next;
};

typedef struct NodeChain NodeChain;
struct NodeChain {
  Node* head;
  Node* tail;
};

void DllPushFrontTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_FRONT(chain.head, chain.tail, &node_1, prev, next);
  EXPECT_PTR_NULL(chain.head->prev);
  EXPECT_PTR_NULL(chain.tail->next);
  EXPECT_PTR_EQ(chain.head, &node_1);
  EXPECT_PTR_EQ(chain.tail, &node_1);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_NULL(test->prev);
  EXPECT_PTR_NULL(test->next);

  DLL_PUSH_FRONT(chain.head, chain.tail, &node_2, prev, next);
  EXPECT_PTR_NULL(chain.head->prev);
  EXPECT_PTR_NULL(chain.tail->next);
  EXPECT_PTR_EQ(chain.head, &node_2);
  EXPECT_PTR_EQ(chain.tail, &node_1);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_2);
  EXPECT_PTR_EQ(test->next, &node_1);
  EXPECT_PTR_NULL(test->prev);
  test = test->next;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_NULL(test->next);
  EXPECT_PTR_EQ(test->prev, &node_2);

  DLL_PUSH_FRONT(chain.head, chain.tail, &node_3, prev, next);
  EXPECT_PTR_NULL(chain.head->prev);
  EXPECT_PTR_NULL(chain.tail->next);
  EXPECT_PTR_EQ(chain.head, &node_3);
  EXPECT_PTR_EQ(chain.tail, &node_1);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_3);
  EXPECT_PTR_EQ(test->next, &node_2);
  EXPECT_PTR_NULL(test->prev);
  test = test->next;
  EXPECT_PTR_EQ(test, &node_2);
  EXPECT_PTR_EQ(test->next, &node_1);
  EXPECT_PTR_EQ(test->prev, &node_3);
  test = test->next;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_NULL(test->next);
  EXPECT_PTR_EQ(test->prev, &node_2);
}

void DllPushBackTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.head, chain.tail, &node_1, prev, next);
  EXPECT_PTR_NULL(chain.head->prev);
  EXPECT_PTR_NULL(chain.tail->next);
  EXPECT_PTR_EQ(chain.head, &node_1);
  EXPECT_PTR_EQ(chain.tail, &node_1);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_NULL(test->prev);
  EXPECT_PTR_NULL(test->next);

  DLL_PUSH_BACK(chain.head, chain.tail, &node_2, prev, next);
  EXPECT_PTR_NULL(chain.head->prev);
  EXPECT_PTR_NULL(chain.tail->next);
  EXPECT_PTR_EQ(chain.head, &node_1);
  EXPECT_PTR_EQ(chain.tail, &node_2);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_EQ(test->next, &node_2);
  EXPECT_PTR_NULL(test->prev);
  test = test->next;
  EXPECT_PTR_EQ(test, &node_2);
  EXPECT_PTR_NULL(test->next);
  EXPECT_PTR_EQ(test->prev, &node_1);

  DLL_PUSH_BACK(chain.head, chain.tail, &node_3, prev, next);
  EXPECT_PTR_NULL(chain.head->prev);
  EXPECT_PTR_NULL(chain.tail->next);
  EXPECT_PTR_EQ(chain.tail, &node_3);
  EXPECT_PTR_EQ(chain.head, &node_1);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_EQ(test->next, &node_2);
  EXPECT_PTR_NULL(test->prev);
  test = test->next;
  EXPECT_PTR_EQ(test, &node_2);
  EXPECT_PTR_EQ(test->next, &node_3);
  EXPECT_PTR_EQ(test->prev, &node_1);
  test = test->next;
  EXPECT_PTR_EQ(test, &node_3);
  EXPECT_PTR_NULL(test->next);
  EXPECT_PTR_EQ(test->prev, &node_2);
}

void DllPopFrontTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.head, chain.tail, &node_1, prev, next);
  DLL_PUSH_BACK(chain.head, chain.tail, &node_2, prev, next);
  DLL_PUSH_BACK(chain.head, chain.tail, &node_3, prev, next);

  EXPECT_PTR_EQ(chain.head, &node_1);
  EXPECT_PTR_EQ(chain.tail, &node_3);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_EQ(test->next, &node_2);
  EXPECT_PTR_NULL(test->prev);

  DLL_POP_FRONT(chain.head, chain.tail, prev, next);
  EXPECT_PTR_EQ(chain.head, &node_2);
  EXPECT_PTR_EQ(chain.tail, &node_3);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_2);
  EXPECT_PTR_EQ(test->next, &node_3);
  EXPECT_PTR_NULL(test->prev);

  DLL_POP_FRONT(chain.head, chain.tail, prev, next);
  EXPECT_PTR_EQ(chain.head, &node_3);
  EXPECT_PTR_EQ(chain.tail, &node_3);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_3);
  EXPECT_PTR_NULL(test->next);
  EXPECT_PTR_NULL(test->prev);

  DLL_POP_FRONT(chain.head, chain.tail, prev, next);
  EXPECT_PTR_NULL(chain.head);
  EXPECT_PTR_NULL(chain.tail);
}

void DllPopBackTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.head, chain.tail, &node_1, prev, next);
  DLL_PUSH_BACK(chain.head, chain.tail, &node_2, prev, next);
  DLL_PUSH_BACK(chain.head, chain.tail, &node_3, prev, next);

  EXPECT_PTR_EQ(chain.head, &node_1);
  EXPECT_PTR_EQ(chain.tail, &node_3);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_EQ(test->next, &node_2);
  EXPECT_PTR_EQ(test->next->next, &node_3);
  EXPECT_PTR_NULL(test->prev);

  DLL_POP_BACK(chain.head, chain.tail, prev, next);
  EXPECT_PTR_EQ(chain.head, &node_1);
  EXPECT_PTR_EQ(chain.tail, &node_2);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_EQ(test->next, &node_2);
  EXPECT_PTR_NULL(test->next->next);
  EXPECT_PTR_NULL(test->prev);

  DLL_POP_BACK(chain.head, chain.tail, prev, next);
  EXPECT_PTR_EQ(chain.head, &node_1);
  EXPECT_PTR_EQ(chain.tail, &node_1);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_NULL(test->next);
  EXPECT_PTR_NULL(test->prev);

  DLL_POP_BACK(chain.head, chain.tail, prev, next);
  EXPECT_PTR_NULL(chain.head);
  EXPECT_PTR_NULL(chain.tail);
}

void DllInsertRemoveMiddleTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.head, chain.tail, &node_1, prev, next);
  DLL_PUSH_BACK(chain.head, chain.tail, &node_3, prev, next);
  EXPECT_PTR_EQ(chain.head, &node_1);
  EXPECT_PTR_EQ(chain.tail, &node_3);

  DLL_INSERT(chain.head, chain.tail, chain.head, &node_2, prev, next);
  EXPECT_PTR_EQ(chain.head, &node_1);
  EXPECT_PTR_EQ(chain.tail, &node_3);
  test = chain.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_EQ(test->next, &node_2);
  EXPECT_PTR_NULL(test->prev);
  test = test->next;
  EXPECT_PTR_EQ(test, &node_2);
  EXPECT_PTR_EQ(test->next, &node_3);
  EXPECT_PTR_EQ(test->prev, &node_1);
  test = test->next;
  EXPECT_PTR_EQ(test, &node_3);
  EXPECT_PTR_NULL(test->next);
  EXPECT_PTR_EQ(test->prev, &node_2);

  DLL_REMOVE(chain.head, chain.tail, &node_2, prev, next);
  EXPECT_PTR_EQ(chain.head, &node_1);
  EXPECT_PTR_EQ(chain.tail, &node_3);
  EXPECT_PTR_EQ(chain.head->next, &node_3);
  EXPECT_PTR_EQ(chain.head->next->prev, &node_1);
}

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  RUN_TEST(DllPushFrontTest);
  RUN_TEST(DllPushBackTest);
  RUN_TEST(DllPopFrontTest);
  RUN_TEST(DllPopBackTest);
  RUN_TEST(DllInsertRemoveMiddleTest);
  LogTestReport();
  return 0;
}
