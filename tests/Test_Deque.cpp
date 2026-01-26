//===---------------------------------------------------------------------------===//
/**
 * @file Test_Deque.cpp
 * @brief Google Test unit tests for CircularArrayDeque.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 */
//===--------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/queues/Circular_Array_Deque.hpp"

using namespace ads::queues;

// Test fixture for CircularArrayDeque.
class CircularArrayDequeTest : public ::testing::Test {
protected:
  CircularArrayDeque<int> deque;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(CircularArrayDequeTest, IsEmptyOnConstruction) {
  EXPECT_EQ(deque.size(), 0);
  EXPECT_TRUE(deque.is_empty());
}

TEST_F(CircularArrayDequeTest, PushFrontAndBack) {
  deque.push_front(10);
  deque.push_back(20);

  EXPECT_EQ(deque.size(), 2);
  EXPECT_EQ(deque.front(), 10);
  EXPECT_EQ(deque.back(), 20);
}

TEST_F(CircularArrayDequeTest, PopFrontAndBack) {
  deque.push_back(10);
  deque.push_back(20);
  deque.push_back(30);

  deque.pop_front();
  EXPECT_EQ(deque.front(), 20);

  deque.pop_back();
  EXPECT_EQ(deque.back(), 20);

  deque.pop_front();
  EXPECT_TRUE(deque.is_empty());
}

TEST_F(CircularArrayDequeTest, PopOnEmptyThrows) {
  EXPECT_THROW(deque.pop_front(), QueueUnderflowException);
  EXPECT_THROW(deque.pop_back(), QueueUnderflowException);
}

TEST_F(CircularArrayDequeTest, AccessOnEmptyThrows) {
  EXPECT_THROW(deque.front(), QueueUnderflowException);
  EXPECT_THROW(deque.back(), QueueUnderflowException);
}

TEST_F(CircularArrayDequeTest, EmplaceFrontBack) {
  CircularArrayDeque<std::string> strings;
  strings.emplace_front("Hello");
  strings.emplace_back("World");

  EXPECT_EQ(strings.front(), "Hello");
  EXPECT_EQ(strings.back(), "World");
}

TEST_F(CircularArrayDequeTest, IndexAccess) {
  for (int i = 0; i < 5; ++i) {
    deque.push_back(i * 10);
  }

  EXPECT_EQ(deque[0], 0);
  EXPECT_EQ(deque[3], 30);
  EXPECT_THROW(deque.at(5), QueueException);
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(CircularArrayDequeTest, MoveSemantics) {
  deque.push_back(10);
  deque.push_back(20);

  CircularArrayDeque<int> moved = std::move(deque);
  EXPECT_TRUE(deque.is_empty());
  EXPECT_EQ(moved.size(), 2);

  deque = std::move(moved);
  EXPECT_TRUE(moved.is_empty());
  EXPECT_EQ(deque.size(), 2);
}

//===----------------------------- ITERATOR TESTS ------------------------------===//

TEST_F(CircularArrayDequeTest, IteratorTraversal) {
  for (int i = 1; i <= 4; ++i) {
    deque.push_back(i);
  }

  std::vector<int> values;
  for (const auto& value : deque) {
    values.push_back(value);
  }

  std::vector<int> expected = {1, 2, 3, 4};
  EXPECT_EQ(values, expected);
}

TEST_F(CircularArrayDequeTest, WrapAroundBehavior) {
  CircularArrayDeque<int> small_deque(8);

  for (int i = 0; i < 6; ++i) {
    small_deque.push_back(i);
  }
  small_deque.pop_front();
  small_deque.pop_front();

  small_deque.push_back(6);
  small_deque.push_front(-1);

  EXPECT_EQ(small_deque.front(), -1);
  EXPECT_EQ(small_deque.back(), 6);
  EXPECT_EQ(small_deque[1], 2);
}

//===---------------------------------------------------------------------------===//
