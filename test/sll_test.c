#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

typedef struct Node Node;
struct Node {
  Node* next;
};

typedef struct Stack Stack;
struct Stack {
  Node* front;
};

typedef struct Queue Queue;
struct Queue {
  Node* front;
  Node* back;
};

B32 SllStackPushTest(void) {
  Stack stack = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_STACK_PUSH(stack.front, &node_1, next);
  TEST_EXPECT(stack.front == &node_1);
  test = stack.front;
  TEST_EXPECT(test->next == NULL);

  SLL_STACK_PUSH(stack.front, &node_2, next);
  TEST_EXPECT(stack.front == &node_2);
  test = stack.front;
  TEST_EXPECT(test->next == &node_1);
  test = test->next;
  TEST_EXPECT(test->next == NULL);

  return true;
}

B32 SllStackPopTest(void) {
  Stack stack = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_STACK_PUSH(stack.front, &node_1, next);
  SLL_STACK_PUSH(stack.front, &node_2, next);
  TEST_EXPECT(stack.front == &node_2);
  test = stack.front;
  TEST_EXPECT(test->next == &node_1);
  test = test->next;
  TEST_EXPECT(test->next == NULL);

  SLL_STACK_POP(stack.front, next);
  TEST_EXPECT(stack.front == &node_1);
  test = stack.front;
  TEST_EXPECT(test->next == NULL);

  SLL_STACK_POP(stack.front, next);
  TEST_EXPECT(stack.front == NULL);

  return true;
}

B32 SllQueuePushTest(void) {
  Queue queue = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_QUEUE_PUSH(queue.front, queue.back, &node_1, next);
  TEST_EXPECT(queue.front == &node_1);
  test = queue.front;
  TEST_EXPECT(test->next == NULL);

  SLL_QUEUE_PUSH(queue.front, queue.back, &node_2, next);
  TEST_EXPECT(queue.front == &node_1);
  test = queue.front;
  TEST_EXPECT(test->next == &node_2);
  test = test->next;
  TEST_EXPECT(test->next == NULL);

  return true;
}

B32 SllQueuePushFrontTest(void) {
  Queue queue = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_QUEUE_PUSH_FRONT(queue.front, queue.back, &node_1, next);
  TEST_EXPECT(queue.front == &node_1);
  test = queue.front;
  TEST_EXPECT(test->next == NULL);

  SLL_QUEUE_PUSH_FRONT(queue.front, queue.back, &node_2, next);
  TEST_EXPECT(queue.front == &node_2);
  test = queue.front;
  TEST_EXPECT(test->next == &node_1);
  test = test->next;
  TEST_EXPECT(test->next == NULL);

  return true;
}

B32 SllQueuePopTest(void) {
  Queue queue = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_QUEUE_PUSH(queue.front, queue.back, &node_1, next);
  SLL_QUEUE_PUSH(queue.front, queue.back, &node_2, next);
  TEST_EXPECT(queue.front == &node_1);
  test = queue.front;
  TEST_EXPECT(test->next == &node_2);
  test = test->next;
  TEST_EXPECT(test->next == NULL);

  SLL_QUEUE_POP(queue.front, queue.back, next);
  TEST_EXPECT(queue.front == &node_2);
  test = queue.front;
  TEST_EXPECT(test->next == NULL);

  return true;
}

int main(void) {
  TEST(SllStackPushTest());
  TEST(SllStackPopTest());
  TEST(SllQueuePushTest());
  TEST(SllQueuePushFrontTest());
  TEST(SllQueuePopTest());
  return 0;
}
