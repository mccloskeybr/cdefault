#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

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

B32 DllPushFrontTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_FRONT(chain.head, chain.tail, &node_1, prev, next);
  TEST_EXPECT(chain.head->prev == NULL);
  TEST_EXPECT(chain.tail->next == NULL);
  TEST_EXPECT(chain.head == &node_1);
  TEST_EXPECT(chain.tail == &node_1);
  test = chain.head;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == NULL);

  DLL_PUSH_FRONT(chain.head, chain.tail, &node_2, prev, next);
  TEST_EXPECT(chain.head->next == NULL);
  TEST_EXPECT(chain.tail->prev == NULL);
  TEST_EXPECT(chain.head == &node_2);
  TEST_EXPECT(chain.tail == &node_1);
  test = chain.head;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == &node_1);
  TEST_EXPECT(test->next == NULL);
  test = test->prev;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);

  DLL_PUSH_FRONT(chain.head, chain.tail, &node_3, prev, next);
  TEST_EXPECT(chain.head->next == NULL);
  TEST_EXPECT(chain.tail->prev == NULL);
  TEST_EXPECT(chain.head == &node_3);
  TEST_EXPECT(chain.tail == &node_1);
  test = chain.head;
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->next == NULL);
  test = test->prev;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == &node_1);
  TEST_EXPECT(test->next == &node_3);
  test = test->prev;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);

  return true;
}

B32 DllPushBackTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.head, chain.tail, &node_1, prev, next);
  TEST_EXPECT(chain.head->next == NULL);
  TEST_EXPECT(chain.tail->prev == NULL);
  TEST_EXPECT(chain.head == &node_1);
  TEST_EXPECT(chain.tail == &node_1);
  test = chain.head;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == NULL);

  DLL_PUSH_BACK(chain.head, chain.tail, &node_2, prev, next);
  TEST_EXPECT(chain.head->next == NULL);
  TEST_EXPECT(chain.tail->prev == NULL);
  TEST_EXPECT(chain.head == &node_1);
  TEST_EXPECT(chain.tail == &node_2);
  test = chain.head;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->next == NULL);
  test = test->prev;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_1);

  DLL_PUSH_BACK(chain.head, chain.tail, &node_3, prev, next);
  TEST_EXPECT(chain.head->next == NULL);
  TEST_EXPECT(chain.tail->prev == NULL);
  TEST_EXPECT(chain.tail == &node_3);
  TEST_EXPECT(chain.head == &node_1);
  test = chain.head;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->next == NULL);
  test = test->prev;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == &node_3);
  TEST_EXPECT(test->next == &node_1);
  test = test->prev;
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);

  return true;
}

B32 DllPopFrontTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.head, chain.tail, &node_1, prev, next);
  DLL_PUSH_BACK(chain.head, chain.tail, &node_2, prev, next);
  DLL_PUSH_BACK(chain.head, chain.tail, &node_3, prev, next);

  TEST_EXPECT(chain.head == &node_1);
  TEST_EXPECT(chain.tail == &node_3);
  test = chain.head;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->next == NULL);

  DLL_POP_FRONT(chain.head, chain.tail, prev, next);
  TEST_EXPECT(chain.head == &node_2);
  TEST_EXPECT(chain.tail == &node_3);
  test = chain.head;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == &node_3);
  TEST_EXPECT(test->next == NULL);

  DLL_POP_FRONT(chain.head, chain.tail, prev, next);
  TEST_EXPECT(chain.head == &node_3);
  TEST_EXPECT(chain.tail == &node_3);
  test = chain.head;
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == NULL);

  DLL_POP_FRONT(chain.head, chain.tail, prev, next);
  TEST_EXPECT(chain.head == NULL);
  TEST_EXPECT(chain.tail == NULL);

  return true;
}

B32 DllPopBackTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.head, chain.tail, &node_1, prev, next);
  DLL_PUSH_BACK(chain.head, chain.tail, &node_2, prev, next);
  DLL_PUSH_BACK(chain.head, chain.tail, &node_3, prev, next);

  TEST_EXPECT(chain.head == &node_1);
  TEST_EXPECT(chain.tail == &node_3);
  test = chain.head;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->prev->prev == &node_3);
  TEST_EXPECT(test->next == NULL);

  DLL_POP_BACK(chain.head, chain.tail, prev, next);
  TEST_EXPECT(chain.head == &node_1);
  TEST_EXPECT(chain.tail == &node_2);
  test = chain.head;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->prev->prev == NULL);
  TEST_EXPECT(test->next == NULL);

  DLL_POP_BACK(chain.head, chain.tail, prev, next);
  TEST_EXPECT(chain.head == &node_1);
  TEST_EXPECT(chain.tail == &node_1);
  test = chain.head;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == NULL);

  DLL_POP_BACK(chain.head, chain.tail, prev, next);
  TEST_EXPECT(chain.head == NULL);
  TEST_EXPECT(chain.tail == NULL);

  return true;
}

B32 DllInsertRemoveMiddleTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.head, chain.tail, &node_1, prev, next);
  DLL_PUSH_BACK(chain.head, chain.tail, &node_3, prev, next);
  TEST_EXPECT(chain.head == &node_1);
  TEST_EXPECT(chain.tail == &node_3);

  DLL_INSERT(chain.head, chain.tail, chain.tail, &node_2, prev, next);
  TEST_EXPECT(chain.head == &node_1);
  TEST_EXPECT(chain.tail == &node_3);
  test = chain.head;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->next == NULL);
  test = test->prev;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == &node_3);
  TEST_EXPECT(test->next == &node_1);
  test = test->prev;
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);

  DLL_REMOVE(chain.head, chain.tail, &node_2, prev, next);
  TEST_EXPECT(chain.head == &node_1);
  TEST_EXPECT(chain.tail == &node_3);
  TEST_EXPECT(chain.head->prev == &node_3);
  TEST_EXPECT(chain.head->prev->next == &node_1);

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
