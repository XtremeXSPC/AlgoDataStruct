//===---------------------------------------------------------------------------===//
/**
 * @file Test_Queues.cpp
 * @brief Google Test unit tests for Queue implementations.
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/queues/Circular_Array_Queue.hpp"
#include "../include/ads/queues/Linked_Queue.hpp"

using namespace ads::queues;

// Test fixture for CircularArrayQueue.
class CircularArrayQueueTest : public ::testing::Test {
protected:
  CircularArrayQueue<int> queue;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(CircularArrayQueueTest, IsEmptyOnConstruction) {
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.is_empty());
}

TEST_F(CircularArrayQueueTest, EnqueueAndFront) {
  queue.enqueue(10);
  EXPECT_EQ(queue.size(), 1);
  EXPECT_EQ(queue.front(), 10);
  EXPECT_EQ(queue.rear(), 10);

  queue.enqueue(20);
  EXPECT_EQ(queue.size(), 2);
  EXPECT_EQ(queue.front(), 10);
  EXPECT_EQ(queue.rear(), 20);
}

TEST_F(CircularArrayQueueTest, Dequeue) {
  queue.enqueue(10);
  queue.enqueue(20);
  queue.enqueue(30);

  queue.dequeue();
  EXPECT_EQ(queue.front(), 20);

  queue.dequeue();
  EXPECT_EQ(queue.front(), 30);

  queue.dequeue();
  EXPECT_TRUE(queue.is_empty());
}

TEST_F(CircularArrayQueueTest, DequeueOnEmptyThrows) {
  EXPECT_THROW(queue.dequeue(), QueueUnderflowException);
}

TEST_F(CircularArrayQueueTest, FrontOnEmptyThrows) {
  EXPECT_THROW(queue.front(), QueueUnderflowException);
}

TEST_F(CircularArrayQueueTest, RearOnEmptyThrows) {
  EXPECT_THROW(queue.rear(), QueueUnderflowException);
}

TEST_F(CircularArrayQueueTest, Clear) {
  queue.enqueue(10);
  queue.enqueue(20);

  queue.clear();
  EXPECT_TRUE(queue.is_empty());
  EXPECT_EQ(queue.size(), 0);
}

TEST_F(CircularArrayQueueTest, Emplace) {
  CircularArrayQueue<std::string> str_queue;
  str_queue.emplace("Hello");
  str_queue.emplace("World");

  EXPECT_EQ(str_queue.front(), "Hello");
  EXPECT_EQ(str_queue.rear(), "World");
}

TEST_F(CircularArrayQueueTest, MoveSemantics) {
  queue.enqueue(10);
  queue.enqueue(20);

  // Move constructor
  CircularArrayQueue<int> moved_queue = std::move(queue);
  EXPECT_TRUE(queue.is_empty());
  EXPECT_EQ(moved_queue.size(), 2);
  EXPECT_EQ(moved_queue.front(), 10);

  // Move assignment
  queue = std::move(moved_queue);
  EXPECT_TRUE(moved_queue.is_empty());
  EXPECT_EQ(queue.size(), 2);
}

TEST_F(CircularArrayQueueTest, FIFOOrder) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  for (int val : input) {
    queue.enqueue(val);
  }

  std::vector<int> output;
  while (!queue.is_empty()) {
    output.push_back(queue.front());
    queue.dequeue();
  }

  EXPECT_EQ(output, input); // FIFO: output should match input order.
}

TEST_F(CircularArrayQueueTest, CircularBehavior) {
  // Fill and partially empty to test circular wrapping.
  for (int i = 0; i < 5; ++i) {
    queue.enqueue(i);
  }

  for (int i = 0; i < 3; ++i) {
    queue.dequeue();
  }

  // Now add more elements (should wrap around).
  for (int i = 10; i < 15; ++i) {
    queue.enqueue(i);
  }

  EXPECT_EQ(queue.front(), 3);
  EXPECT_EQ(queue.rear(), 14);
}

TEST_F(CircularArrayQueueTest, LargeQueueOperations) {
  const int N = 10000;
  for (int i = 0; i < N; ++i) {
    queue.enqueue(i);
  }

  EXPECT_EQ(queue.size(), N);
  EXPECT_EQ(queue.front(), 0);
  EXPECT_EQ(queue.rear(), N - 1);

  for (int i = 0; i < N; ++i) {
    queue.dequeue();
  }

  EXPECT_TRUE(queue.is_empty());
}

//===--------------------------- LINKED QUEUE TESTS ----------------------------===//

class LinkedQueueTest : public ::testing::Test {
protected:
  LinkedQueue<int> queue;
};

TEST_F(LinkedQueueTest, IsEmptyOnConstruction) {
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.is_empty());
}

TEST_F(LinkedQueueTest, EnqueueAndFront) {
  queue.enqueue(10);
  EXPECT_EQ(queue.size(), 1);
  EXPECT_EQ(queue.front(), 10);
  EXPECT_EQ(queue.rear(), 10);

  queue.enqueue(20);
  EXPECT_EQ(queue.size(), 2);
  EXPECT_EQ(queue.front(), 10);
  EXPECT_EQ(queue.rear(), 20);
}

TEST_F(LinkedQueueTest, Dequeue) {
  queue.enqueue(10);
  queue.enqueue(20);
  queue.enqueue(30);

  queue.dequeue();
  EXPECT_EQ(queue.front(), 20);

  queue.dequeue();
  EXPECT_EQ(queue.front(), 30);

  queue.dequeue();
  EXPECT_TRUE(queue.is_empty());
}

TEST_F(LinkedQueueTest, DequeueOnEmptyThrows) {
  EXPECT_THROW(queue.dequeue(), QueueUnderflowException);
}

TEST_F(LinkedQueueTest, FrontOnEmptyThrows) {
  EXPECT_THROW(queue.front(), QueueUnderflowException);
}

TEST_F(LinkedQueueTest, RearOnEmptyThrows) {
  EXPECT_THROW(queue.rear(), QueueUnderflowException);
}

TEST_F(LinkedQueueTest, Clear) {
  queue.enqueue(10);
  queue.enqueue(20);

  queue.clear();
  EXPECT_TRUE(queue.is_empty());
  EXPECT_EQ(queue.size(), 0);
}

TEST_F(LinkedQueueTest, Emplace) {
  LinkedQueue<std::string> str_queue;
  str_queue.emplace("Hello");
  str_queue.emplace("World");

  EXPECT_EQ(str_queue.front(), "Hello");
  EXPECT_EQ(str_queue.rear(), "World");
}

TEST_F(LinkedQueueTest, MoveSemantics) {
  queue.enqueue(10);
  queue.enqueue(20);

  // Move constructor.
  LinkedQueue<int> moved_queue = std::move(queue);
  EXPECT_TRUE(queue.is_empty());
  EXPECT_EQ(moved_queue.size(), 2);
  EXPECT_EQ(moved_queue.front(), 10);

  // Move assignment.
  queue = std::move(moved_queue);
  EXPECT_TRUE(moved_queue.is_empty());
  EXPECT_EQ(queue.size(), 2);
}

TEST_F(LinkedQueueTest, FIFOOrder) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  for (int val : input) {
    queue.enqueue(val);
  }

  std::vector<int> output;
  while (!queue.is_empty()) {
    output.push_back(queue.front());
    queue.dequeue();
  }

  EXPECT_EQ(output, input);
}

TEST_F(LinkedQueueTest, LargeQueueOperations) {
  const int N = 10000;
  for (int i = 0; i < N; ++i) {
    queue.enqueue(i);
  }

  EXPECT_EQ(queue.size(), N);
  EXPECT_EQ(queue.front(), 0);
  EXPECT_EQ(queue.rear(), N - 1);

  for (int i = 0; i < N; ++i) {
    queue.dequeue();
  }

  EXPECT_TRUE(queue.is_empty());
}

//===--------------------------- POLYMORPHISM TESTS ----------------------------===//

TEST(QueuePolymorphismTest, CircularArrayQueueThroughInterface) {
  CircularArrayQueue<int> concrete_queue;
  Queue<int>*             q = &concrete_queue;

  q->enqueue(10);
  q->enqueue(20);

  EXPECT_EQ(q->front(), 10);
  EXPECT_EQ(q->rear(), 20);
  EXPECT_EQ(q->size(), 2);
}

TEST(QueuePolymorphismTest, LinkedQueueThroughInterface) {
  LinkedQueue<int> concrete_queue;
  Queue<int>*      q = &concrete_queue;

  q->enqueue(10);
  q->enqueue(20);

  EXPECT_EQ(q->front(), 10);
  EXPECT_EQ(q->rear(), 20);
  EXPECT_EQ(q->size(), 2);
}

//===---------------------------------------------------------------------------===//
