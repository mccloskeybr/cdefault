#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

typedef struct Node Node;
struct Node {
  Node* prev;
  Node* next;
};

typedef struct NodeChain NodeChain;
struct NodeChain {
  Node* front;
  Node* back;
};

B32 DllPushFrontTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_FRONT(chain.front, chain.back, &node_1, prev, next);
  TEST_EXPECT(chain.front->prev == NULL);
  TEST_EXPECT(chain.back->next == NULL);
  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_1);
  test = chain.front;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == NULL);

  DLL_PUSH_FRONT(chain.front, chain.back, &node_2, prev, next);
  TEST_EXPECT(chain.front->prev == NULL);
  TEST_EXPECT(chain.back->next == NULL);
  TEST_EXPECT(chain.front == &node_2);
  TEST_EXPECT(chain.back == &node_1);
  test = chain.front;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_1);
  test = test->next;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->next == NULL);

  DLL_PUSH_FRONT(chain.front, chain.back, &node_3, prev, next);
  TEST_EXPECT(chain.front->prev == NULL);
  TEST_EXPECT(chain.back->next == NULL);
  TEST_EXPECT(chain.front == &node_3);
  TEST_EXPECT(chain.back == &node_1);
  test = chain.front;
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);
  test = test->next;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == &node_3);
  TEST_EXPECT(test->next == &node_1);
  test = test->next;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->next == NULL);

  return true;
}

B32 DllPushBackTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.front, chain.back, &node_1, prev, next);
  TEST_EXPECT(chain.front->prev == NULL);
  TEST_EXPECT(chain.back->next == NULL);
  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_1);
  test = chain.front;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == NULL);

  DLL_PUSH_BACK(chain.front, chain.back, &node_2, prev, next);
  TEST_EXPECT(chain.front->prev == NULL);
  TEST_EXPECT(chain.back->next == NULL);
  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_2);
  test = chain.front;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);
  test = test->next;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == &node_1);
  TEST_EXPECT(test->next == NULL);

  DLL_PUSH_BACK(chain.front, chain.back, &node_3, prev, next);
  TEST_EXPECT(chain.front->prev == NULL);
  TEST_EXPECT(chain.back->next == NULL);
  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_3);
  test = chain.front;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);
  test = test->next;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == &node_1);
  TEST_EXPECT(test->next == &node_3);
  test = test->next;
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->next == NULL);

  return true;
}

B32 DllPopFrontTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.front, chain.back, &node_1, prev, next);
  DLL_PUSH_BACK(chain.front, chain.back, &node_2, prev, next);
  DLL_PUSH_BACK(chain.front, chain.back, &node_3, prev, next);

  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_3);
  test = chain.front;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);

  DLL_POP_FRONT(chain.front, chain.back, prev, next);
  TEST_EXPECT(chain.front == &node_2);
  TEST_EXPECT(chain.back == &node_3);
  test = chain.front;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_3);

  DLL_POP_FRONT(chain.front, chain.back, prev, next);
  TEST_EXPECT(chain.front == &node_3);
  TEST_EXPECT(chain.back == &node_3);
  test = chain.front;
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == NULL);

  DLL_POP_FRONT(chain.front, chain.back, prev, next);
  TEST_EXPECT(chain.front == NULL);
  TEST_EXPECT(chain.back == NULL);

  return true;
}

B32 DllPopBackTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.front, chain.back, &node_1, prev, next);
  DLL_PUSH_BACK(chain.front, chain.back, &node_2, prev, next);
  DLL_PUSH_BACK(chain.front, chain.back, &node_3, prev, next);

  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_3);
  test = chain.front;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);

  DLL_POP_BACK(chain.front, chain.back, prev, next);
  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_2);
  test = chain.front;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);

  DLL_POP_BACK(chain.front, chain.back, prev, next);
  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_1);
  test = chain.front;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == NULL);

  DLL_POP_BACK(chain.front, chain.back, prev, next);
  TEST_EXPECT(chain.front == NULL);
  TEST_EXPECT(chain.back == NULL);

  return true;
}

B32 DllInsertRemoveMiddleTest(void) {
  NodeChain chain = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node node_3 = {0};
  Node* test = NULL;

  DLL_PUSH_BACK(chain.front, chain.back, &node_1, prev, next);
  DLL_PUSH_BACK(chain.front, chain.back, &node_3, prev, next);
  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_3);

  DLL_INSERT(chain.front, chain.back, chain.front, &node_2, prev, next);
  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_3);
  test = chain.front;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->prev == NULL);
  TEST_EXPECT(test->next == &node_2);
  test = test->next;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->prev == &node_1);
  TEST_EXPECT(test->next == &node_3);
  test = test->next;
  TEST_EXPECT(test == &node_3);
  TEST_EXPECT(test->prev == &node_2);
  TEST_EXPECT(test->next == NULL);

  DLL_REMOVE(chain.front, chain.back, &node_2, prev, next);
  TEST_EXPECT(chain.front == &node_1);
  TEST_EXPECT(chain.back == &node_3);
  TEST_EXPECT(chain.front->next == &node_3);
  TEST_EXPECT(chain.front->next->prev == &node_1);

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
