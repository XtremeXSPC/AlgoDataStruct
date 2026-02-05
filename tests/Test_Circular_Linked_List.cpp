//===---------------------------------------------------------------------------===//
/**
 * @file Test_Circular_Linked_List.cpp
 * @brief Google Test unit tests for CircularLinkedList.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/lists/Circular_Linked_List.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ads::lists;

// Test fixture for CircularLinkedList.
class CircularLinkedListTest : public ::testing::Test {
protected:
  CircularLinkedList<int> list;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(CircularLinkedListTest, IsEmptyOnConstruction) {
  EXPECT_EQ(list.size(), 0u);
  EXPECT_TRUE(list.is_empty());
}

TEST_F(CircularLinkedListTest, PushBackAndAccess) {
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);

  EXPECT_EQ(list.size(), 3u);
  EXPECT_EQ(list.front(), 10);
  EXPECT_EQ(list.back(), 30);
}

TEST_F(CircularLinkedListTest, PushFrontAndAccess) {
  list.push_front(10);
  list.push_front(20);
  list.push_front(30);

  EXPECT_EQ(list.size(), 3u);
  EXPECT_EQ(list.front(), 30);
  EXPECT_EQ(list.back(), 10);
}

TEST_F(CircularLinkedListTest, MixedPushOperations) {
  list.push_back(2);
  list.push_front(1);
  list.push_back(3);
  list.push_front(0);

  EXPECT_EQ(list.size(), 4u);
  EXPECT_EQ(list.front(), 0);
  EXPECT_EQ(list.back(), 3);
}

TEST_F(CircularLinkedListTest, PopFront) {
  list.push_back(1);
  list.push_back(2);
  list.push_back(3);

  list.pop_front();
  EXPECT_EQ(list.front(), 2);
  EXPECT_EQ(list.size(), 2u);

  list.pop_front();
  EXPECT_EQ(list.front(), 3);
  EXPECT_EQ(list.size(), 1u);

  list.pop_front();
  EXPECT_TRUE(list.is_empty());
}

TEST_F(CircularLinkedListTest, PopBack) {
  list.push_back(1);
  list.push_back(2);
  list.push_back(3);

  list.pop_back();
  EXPECT_EQ(list.back(), 2);
  EXPECT_EQ(list.size(), 2u);

  list.pop_back();
  EXPECT_EQ(list.back(), 1);
  EXPECT_EQ(list.size(), 1u);

  list.pop_back();
  EXPECT_TRUE(list.is_empty());
}

TEST_F(CircularLinkedListTest, PopOnEmptyThrows) {
  EXPECT_THROW(list.pop_front(), ListException);
  EXPECT_THROW(list.pop_back(), ListException);
}

TEST_F(CircularLinkedListTest, AccessOnEmptyThrows) {
  EXPECT_THROW(list.front(), ListException);
  EXPECT_THROW(list.back(), ListException);
}

TEST_F(CircularLinkedListTest, RotateOperation) {
  list.push_back(1);
  list.push_back(2);
  list.push_back(3);
  list.push_back(4);

  EXPECT_EQ(list.front(), 1);
  EXPECT_EQ(list.back(), 4);

  list.rotate();
  EXPECT_EQ(list.front(), 2);
  EXPECT_EQ(list.back(), 1);

  list.rotate();
  EXPECT_EQ(list.front(), 3);
  EXPECT_EQ(list.back(), 2);
}

TEST_F(CircularLinkedListTest, ContainsOperation) {
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);

  EXPECT_TRUE(list.contains(10));
  EXPECT_TRUE(list.contains(20));
  EXPECT_TRUE(list.contains(30));
  EXPECT_FALSE(list.contains(40));
  EXPECT_FALSE(list.contains(0));
}

TEST_F(CircularLinkedListTest, ContainsOnEmpty) {
  EXPECT_FALSE(list.contains(42));
}

TEST_F(CircularLinkedListTest, MoveSemantics) {
  list.push_back(10);
  list.push_back(20);

  CircularLinkedList<int> moved = std::move(list);
  EXPECT_TRUE(list.is_empty());
  EXPECT_EQ(moved.size(), 2u);

  list = std::move(moved);
  EXPECT_TRUE(moved.is_empty());
  EXPECT_EQ(list.size(), 2u);
}

TEST_F(CircularLinkedListTest, RangeBasedIteration) {
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i);
  }

  std::vector<int> values;
  for (const auto& val : list) {
    values.push_back(val);
  }

  std::vector<int> expected{1, 2, 3, 4, 5};
  EXPECT_EQ(values, expected);
}

TEST_F(CircularLinkedListTest, IterationAfterRotate) {
  for (int i = 1; i <= 4; ++i) {
    list.push_back(i);
  }

  list.rotate();
  list.rotate();

  std::vector<int> values;
  for (const auto& val : list) {
    values.push_back(val);
  }

  std::vector<int> expected{3, 4, 1, 2};
  EXPECT_EQ(values, expected);
}

TEST_F(CircularLinkedListTest, ClearOperation) {
  for (int i = 0; i < 10; ++i) {
    list.push_back(i);
  }

  list.clear();
  EXPECT_TRUE(list.is_empty());
  EXPECT_EQ(list.size(), 0u);
}

TEST_F(CircularLinkedListTest, EmplaceOperations) {
  CircularLinkedList<std::string> strings;
  strings.emplace_back("World");
  strings.emplace_front("Hello");
  strings.emplace_back("!");

  EXPECT_EQ(strings.front(), "Hello");
  EXPECT_EQ(strings.back(), "!");
  EXPECT_EQ(strings.size(), 3u);
}

TEST_F(CircularLinkedListTest, SingleElementOperations) {
  list.push_back(42);
  EXPECT_EQ(list.front(), 42);
  EXPECT_EQ(list.back(), 42);
  EXPECT_EQ(list.size(), 1u);

  list.rotate();
  EXPECT_EQ(list.front(), 42);
  EXPECT_EQ(list.back(), 42);

  EXPECT_TRUE(list.contains(42));
  EXPECT_FALSE(list.contains(0));
}

//===---------------------------------------------------------------------------===//
