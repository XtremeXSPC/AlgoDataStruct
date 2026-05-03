//===---------------------------------------------------------------------------===//
/**
 * @file Test_Priority_Queue.cpp
 * @brief Google Test unit tests for PriorityQueue.
 * @version 0.1
 * @date 2026-05-05
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/queues/Priority_Queue.hpp"

#include <gtest/gtest.h>

#include <array>
#include <functional>
#include <string>
#include <utility>
#include <vector>

using namespace ads::queues;

class PriorityQueueTest : public ::testing::Test {
protected:
  PriorityQueue<int> queue;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(PriorityQueueTest, IsEmptyOnConstruction) {
  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0);
}

TEST_F(PriorityQueueTest, EmptyAccessThrows) {
  EXPECT_THROW(queue.top(), QueueException);
  EXPECT_THROW(queue.pop(), QueueException);
}

//===---------------------------- HEAP ORDER TESTS -----------------------------===//

TEST_F(PriorityQueueTest, PushPopMaintainsMaxHeapOrder) {
  const std::vector<int> values = {4, 1, 9, 7, 3, 9};

  for (int value : values) {
    queue.push(value);
  }

  const std::vector<int> expected = {9, 9, 7, 4, 3, 1};
  for (int value : expected) {
    ASSERT_FALSE(queue.empty());
    EXPECT_EQ(queue.top(), value);
    queue.pop();
  }

  EXPECT_TRUE(queue.empty());
}

TEST_F(PriorityQueueTest, ConstTopReadsHighestPriorityElement) {
  queue.push(2);
  queue.push(11);
  queue.push(5);

  const auto& const_queue = queue;
  EXPECT_EQ(const_queue.top(), 11);
}

TEST_F(PriorityQueueTest, SingleElementPopLeavesQueueEmpty) {
  queue.push(42);
  queue.pop();

  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0);
}

TEST(PriorityQueueMinHeapTest, CustomComparatorBuildsMinHeap) {
  PriorityQueue<int, std::greater<int>> queue(std::greater<int>{});

  queue.push(4);
  queue.push(1);
  queue.push(9);
  queue.push(7);

  const std::vector<int> expected = {1, 4, 7, 9};
  for (int value : expected) {
    ASSERT_FALSE(queue.empty());
    EXPECT_EQ(queue.top(), value);
    queue.pop();
  }

  EXPECT_TRUE(queue.empty());
}

//===---------------------------- CONSTRUCTOR TESTS ----------------------------===//

TEST(PriorityQueueConstructorTest, VectorConstructorHeapifiesElements) {
  const std::vector<int> values = {6, 2, 8, 1, 5};
  PriorityQueue<int>     queue(values);

  EXPECT_EQ(queue.size(), values.size());
  EXPECT_EQ(queue.top(), 8);
}

TEST(PriorityQueueConstructorTest, RangeConstructorHeapifiesElements) {
  const std::array<int, 5> values = {6, 2, 8, 1, 5};
  PriorityQueue<int>       queue(values.begin(), values.end());

  EXPECT_EQ(queue.size(), values.size());
  EXPECT_EQ(queue.top(), 8);
}

TEST(PriorityQueueConstructorTest, InitializerListConstructorHeapifiesElements) {
  PriorityQueue<int> queue{6, 2, 8, 1, 5};

  EXPECT_EQ(queue.size(), 5);
  EXPECT_EQ(queue.top(), 8);
}

//===------------------------------ UTILITY TESTS ------------------------------===//

TEST(PriorityQueueUtilityTest, SortedElementsExtractsInPriorityOrder) {
  PriorityQueue<int> queue{4, 1, 9, 7, 3};

  const std::vector<int> sorted = queue.sorted_elements();

  EXPECT_EQ(sorted, (std::vector<int>{9, 7, 4, 3, 1}));
  EXPECT_TRUE(queue.empty());
}

TEST(PriorityQueueUtilityTest, ReserveSupportsBulkInsertion) {
  PriorityQueue<int> queue;
  queue.reserve(128);

  for (int value = 0; value < 100; ++value) {
    queue.push(value);
  }

  EXPECT_EQ(queue.size(), 100);
  EXPECT_EQ(queue.top(), 99);
}

TEST(PriorityQueueUtilityTest, ClearRemovesAllElements) {
  PriorityQueue<int> queue{4, 1, 9};

  queue.clear();

  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0);
}

//===-------------------------- VALUE CATEGORY TESTS ---------------------------===//

TEST(PriorityQueueValueTest, EmplaceConstructsElementInPlace) {
  PriorityQueue<std::string> queue;

  queue.emplace("beta");
  queue.emplace("alpha");
  queue.emplace("gamma");

  EXPECT_EQ(queue.top(), "gamma");
}

TEST(PriorityQueueValueTest, MoveSemanticsTransferOwnership) {
  PriorityQueue<int> source{3, 1, 2};
  PriorityQueue<int> moved(std::move(source));

  EXPECT_TRUE(source.empty());
  EXPECT_EQ(moved.top(), 3);

  PriorityQueue<int> assigned;
  assigned.push(10);
  assigned = std::move(moved);

  EXPECT_TRUE(moved.empty());
  EXPECT_EQ(assigned.size(), 3);
  EXPECT_EQ(assigned.top(), 3);
}

//===---------------------------------------------------------------------------===//
