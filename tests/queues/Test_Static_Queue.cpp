//===---------------------------------------------------------------------------===//
/**
 * @file Test_Static_Queue.cpp
 * @brief Google Test unit tests for StaticQueue.
 * @version 0.1
 * @date 2026-06-09
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/queues/Static_Queue.hpp"

#include <gtest/gtest.h>

#include <string>
#include <utility>

using namespace ads::queues;

//===----- TEST FIXTURE --------------------------------------------------------===//

class StaticQueueTest : public ::testing::Test {
protected:
  StaticQueue<int, 5> queue;
};

//===----- BASIC STATE TESTS ---------------------------------------------------===//

TEST_F(StaticQueueTest, EmptyOnConstruction) {
  EXPECT_TRUE(queue.is_empty());
  EXPECT_FALSE(queue.is_full());
  EXPECT_EQ(queue.size(), 0u);
  EXPECT_EQ(queue.capacity(), 5u);
  EXPECT_EQ(decltype(queue)::max_size(), 5u);
}

TEST_F(StaticQueueTest, EnqueueUntilFull) {
  for (int i = 0; i < 5; ++i) {
    queue.enqueue(i);
  }
  EXPECT_TRUE(queue.is_full());
  EXPECT_EQ(queue.front(), 0);
  EXPECT_EQ(queue.rear(), 4);
}

TEST_F(StaticQueueTest, EnqueueWhenFullThrowsAndLeavesIntact) {
  for (int i = 0; i < 5; ++i) {
    queue.enqueue(i);
  }
  EXPECT_THROW(queue.enqueue(99), QueueOverflowException);
  EXPECT_EQ(queue.size(), 5u);
  EXPECT_EQ(queue.rear(), 4);
}

TEST_F(StaticQueueTest, EmplaceReturnsReference) {
  StaticQueue<std::string, 4> q;
  auto&                       ref = q.emplace(4, 'z');
  EXPECT_EQ(ref, "zzzz");
  EXPECT_EQ(q.rear(), "zzzz");
}

//===----- FIFO TESTS ----------------------------------------------------------===//

TEST_F(StaticQueueTest, DequeueReturnsFifoOrder) {
  for (int i = 0; i < 5; ++i) {
    queue.enqueue(i);
  }
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(queue.front(), i);
    queue.dequeue();
  }
  EXPECT_TRUE(queue.is_empty());
}

TEST_F(StaticQueueTest, WrapAroundReuseOfSlots) {
  for (int i = 0; i < 5; ++i) {
    queue.enqueue(i);
  }
  queue.dequeue(); // front advances; a slot frees up
  queue.dequeue();
  queue.enqueue(10); // wraps to a reused physical slot
  queue.enqueue(20);
  EXPECT_EQ(queue.size(), 5u);
  EXPECT_EQ(queue.front(), 2);
  EXPECT_EQ(queue.rear(), 20);
}

TEST_F(StaticQueueTest, DequeueOnEmptyThrows) {
  EXPECT_THROW(queue.dequeue(), QueueUnderflowException);
}

TEST_F(StaticQueueTest, FrontRearOnEmptyThrow) {
  EXPECT_THROW(queue.front(), QueueUnderflowException);
  EXPECT_THROW(queue.rear(), QueueUnderflowException);
}

TEST_F(StaticQueueTest, Clear) {
  queue.enqueue(1);
  queue.enqueue(2);
  queue.clear();
  EXPECT_TRUE(queue.is_empty());
  EXPECT_EQ(queue.size(), 0u);
}

//===----- POLYMORPHISM TESTS --------------------------------------------------===//

TEST_F(StaticQueueTest, UsableThroughQueueInterface) {
  queue.enqueue(10);
  queue.enqueue(20);
  Queue<int>& base = queue;
  EXPECT_EQ(base.size(), 2u);
  EXPECT_EQ(base.front(), 10);
  EXPECT_EQ(base.rear(), 20);
  base.dequeue();
  EXPECT_EQ(base.front(), 20);
}

//===----- MOVE SEMANTICS TESTS ------------------------------------------------===//

TEST_F(StaticQueueTest, MoveConstructionLinearizesAndEmptiesSource) {
  for (int i = 0; i < 5; ++i) {
    queue.enqueue(i);
  }
  queue.dequeue(); // make front_ != 0 so the move must linearize
  queue.enqueue(7);
  StaticQueue<int, 5> moved(std::move(queue));
  EXPECT_EQ(moved.size(), 5u);
  EXPECT_EQ(moved.front(), 1);
  EXPECT_EQ(moved.rear(), 7);
  EXPECT_TRUE(queue.is_empty());
}

TEST_F(StaticQueueTest, IsMoveOnly) {
  static_assert(!std::is_copy_constructible_v<StaticQueue<int, 4>>);
  static_assert(!std::is_copy_assignable_v<StaticQueue<int, 4>>);
  static_assert(std::is_move_constructible_v<StaticQueue<int, 4>>);
  SUCCEED();
}

//===---------------------------------------------------------------------------===//
