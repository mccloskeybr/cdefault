#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

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

B32 SllStackPushTest(void) {
  Stack stack = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_STACK_PUSH(stack.head, &node_1, next);
  TEST_EXPECT(stack.head == &node_1);
  test = stack.head;
  TEST_EXPECT(test->next == NULL);

  SLL_STACK_PUSH(stack.head, &node_2, next);
  TEST_EXPECT(stack.head == &node_2);
  test = stack.head;
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

  SLL_STACK_PUSH(stack.head, &node_1, next);
  SLL_STACK_PUSH(stack.head, &node_2, next);
  TEST_EXPECT(stack.head == &node_2);
  test = stack.head;
  TEST_EXPECT(test->next == &node_1);
  test = test->next;
  TEST_EXPECT(test->next == NULL);

  SLL_STACK_POP(stack.head, next);
  TEST_EXPECT(stack.head == &node_1);
  test = stack.head;
  TEST_EXPECT(test->next == NULL);

  SLL_STACK_POP(stack.head, next);
  TEST_EXPECT(stack.head == NULL);

  return true;
}

B32 SllQueuePushBackTest(void) {
  Queue queue = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_QUEUE_PUSH_BACK(queue.head, queue.tail, &node_1, next);
  TEST_EXPECT(queue.head == &node_1);
  TEST_EXPECT(queue.tail == &node_1);
  test = queue.head;
  TEST_EXPECT(test->next == NULL);

  SLL_QUEUE_PUSH_BACK(queue.head, queue.tail, &node_2, next);
  TEST_EXPECT(queue.head == &node_1);
  TEST_EXPECT(queue.tail == &node_2);
  test = queue.tail;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->next == &node_1);
  test = test->next;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->next == NULL);

  return true;
}

B32 SllQueuePushFrontTest(void) {
  Queue queue = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_QUEUE_PUSH_FRONT(queue.head, queue.tail, &node_1, next);
  TEST_EXPECT(queue.head == &node_1);
  TEST_EXPECT(queue.tail == &node_1);
  test = queue.head;
  TEST_EXPECT(test->next == NULL);

  SLL_QUEUE_PUSH_FRONT(queue.head, queue.tail, &node_2, next);
  test = queue.tail;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->next == &node_2);
  test = test->next;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->next == NULL);

  return true;
}

B32 SllQueuePopTest(void) {
  Queue queue = {0};
  Node node_1 = {0};
  Node node_2 = {0};
  Node* test = NULL;

  SLL_QUEUE_PUSH_BACK(queue.head, queue.tail, &node_1, next);
  SLL_QUEUE_PUSH_BACK(queue.head, queue.tail, &node_2, next);
  test = queue.tail;
  TEST_EXPECT(test == &node_2);
  TEST_EXPECT(test->next == &node_1);
  TEST_EXPECT(test->next->next == NULL);

  SLL_QUEUE_POP(queue.head, queue.tail, next);
  test = queue.tail;
  TEST_EXPECT(test == &node_1);
  TEST_EXPECT(test->next == NULL);

  SLL_QUEUE_POP(queue.head, queue.tail, next);
  test = queue.tail;
  TEST_EXPECT(test == NULL);

  return true;
}

int main(void) {
  TEST(SllStackPushTest());
  TEST(SllStackPopTest());
  TEST(SllQueuePushBackTest());
  TEST(SllQueuePushFrontTest());
  TEST(SllQueuePopTest());
  return 0;
}
