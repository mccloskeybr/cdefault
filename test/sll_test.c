#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

typedef struct Node Node;
struct Node {
  Node* next;
};

typedef struct Stack Stack;
struct Stack {
  Node* head;
};

typedef struct Queue Queue;
struct Queue {
  Node* head;
  Node* tail;
};

void SllStackPushTest(void) {
  Stack stack = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_STACK_PUSH(stack.head, &node_1, next);
  EXPECT_PTR_EQ(stack.head, &node_1);
  test = stack.head;
  EXPECT_PTR_NULL(test->next);

  SLL_STACK_PUSH(stack.head, &node_2, next);
  EXPECT_PTR_EQ(stack.head, &node_2);
  test = stack.head;
  EXPECT_PTR_EQ(test->next, &node_1);
  test = test->next;
  EXPECT_PTR_NULL(test->next);
}

void SllStackPopTest(void) {
  Stack stack = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_STACK_PUSH(stack.head, &node_1, next);
  SLL_STACK_PUSH(stack.head, &node_2, next);
  EXPECT_PTR_EQ(stack.head, &node_2);
  test = stack.head;
  EXPECT_PTR_EQ(test->next, &node_1);
  test = test->next;
  EXPECT_PTR_NULL(test->next);

  SLL_STACK_POP(stack.head, next);
  EXPECT_PTR_EQ(stack.head, &node_1);
  test = stack.head;
  EXPECT_PTR_NULL(test->next);

  SLL_STACK_POP(stack.head, next);
  EXPECT_PTR_NULL(stack.head);
}

void SllQueuePushBackTest(void) {
  Queue queue = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_QUEUE_PUSH_BACK(queue.head, queue.tail, &node_1, next);
  EXPECT_PTR_EQ(queue.head, &node_1);
  EXPECT_PTR_EQ(queue.tail, &node_1);
  test = queue.head;
  EXPECT_PTR_NULL(test->next);

  SLL_QUEUE_PUSH_BACK(queue.head, queue.tail, &node_2, next);
  EXPECT_PTR_EQ(queue.head, &node_1);
  EXPECT_PTR_EQ(queue.tail, &node_2);
  test = queue.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_EQ(test->next, &node_2);
  test = test->next;
  EXPECT_PTR_EQ(test, &node_2);
  EXPECT_PTR_NULL(test->next);
}

void SllQueuePushFrontTest(void) {
  Queue queue = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_QUEUE_PUSH_FRONT(queue.head, queue.tail, &node_1, next);
  EXPECT_PTR_EQ(queue.head, &node_1);
  EXPECT_PTR_EQ(queue.tail, &node_1);
  test = queue.head;
  EXPECT_PTR_NULL(test->next);

  SLL_QUEUE_PUSH_FRONT(queue.head, queue.tail, &node_2, next);
  test = queue.head;
  EXPECT_PTR_EQ(test, &node_2);
  EXPECT_PTR_EQ(test->next, &node_1);
  test = test->next;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_NULL(test->next);
}

void SllQueuePopTest(void) {
  Queue queue = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_QUEUE_PUSH_BACK(queue.head, queue.tail, &node_1, next);
  SLL_QUEUE_PUSH_BACK(queue.head, queue.tail, &node_2, next);
  test = queue.head;
  EXPECT_PTR_EQ(test, &node_1);
  EXPECT_PTR_EQ(test->next, &node_2);
  EXPECT_PTR_NULL(test->next->next);

  SLL_QUEUE_POP(queue.head, queue.tail, next);
  test = queue.head;
  EXPECT_PTR_EQ(test, &node_2);
  EXPECT_PTR_NULL(test->next);

  SLL_QUEUE_POP(queue.head, queue.tail, next);
  test = queue.tail;
  EXPECT_PTR_NULL(test);
}

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  RUN_TEST(SllStackPushTest);
  RUN_TEST(SllStackPopTest);
  RUN_TEST(SllQueuePushBackTest);
  RUN_TEST(SllQueuePushFrontTest);
  RUN_TEST(SllQueuePopTest);
  LogTestReport();
  return 0;
}
