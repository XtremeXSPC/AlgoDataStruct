//===--------------------------------------------------------------------------===//
/**
 * @file Test_SinglyLinkedList.cpp
 * @brief Google Test unit tests for SinglyLinkedList
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/lists/Singly_Linked_List.hpp"

using namespace ads::lists;

// Test fixture for SinglyLinkedList
class SinglyLinkedListTest : public ::testing::Test {
protected:
  SinglyLinkedList<int> list;
};

// ----- Basic State Tests ----- //

TEST_F(SinglyLinkedListTest, IsEmptyOnConstruction) {
  EXPECT_EQ(list.size(), 0);
  EXPECT_TRUE(list.is_empty());
}

TEST_F(SinglyLinkedListTest, Clear) {
  list.push_back(10);
  list.push_back(20);
  EXPECT_FALSE(list.is_empty());

  list.clear();
  EXPECT_EQ(list.size(), 0);
  EXPECT_TRUE(list.is_empty());
  EXPECT_THROW(list.front(), ListException);
}

// ----- Modifier Tests (push/pop/emplace) ----- //

TEST_F(SinglyLinkedListTest, PushFront) {
  list.push_front(10);
  EXPECT_EQ(list.size(), 1);
  EXPECT_FALSE(list.is_empty());
  EXPECT_EQ(list.front(), 10);
  EXPECT_EQ(list.back(), 10);

  list.push_front(20);
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.front(), 20);
  EXPECT_EQ(list.back(), 10);
}

TEST_F(SinglyLinkedListTest, PushBack) {
  list.push_back(10);
  EXPECT_EQ(list.size(), 1);
  EXPECT_FALSE(list.is_empty());
  EXPECT_EQ(list.front(), 10);
  EXPECT_EQ(list.back(), 10);

  list.push_back(20);
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.front(), 10);
  EXPECT_EQ(list.back(), 20);
}

TEST_F(SinglyLinkedListTest, PopFront) {
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);

  list.pop_front();
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.front(), 20);

  list.pop_front();
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.front(), 30);

  list.pop_front();
  EXPECT_TRUE(list.is_empty());
  EXPECT_THROW(list.pop_front(), ListException);
}

TEST_F(SinglyLinkedListTest, PopBack) {
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);

  list.pop_back();
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.back(), 20);

  list.pop_back();
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.back(), 10);

  list.pop_back();
  EXPECT_TRUE(list.is_empty());
  EXPECT_THROW(list.pop_back(), ListException);
}

TEST(SinglyLinkedListStringTest, Emplace) {
  SinglyLinkedList<std::string> str_list;
  str_list.emplace_front("world");
  str_list.emplace_back("!");
  str_list.emplace_front("Hello");

  EXPECT_EQ(str_list.size(), 3);
  EXPECT_EQ(str_list.front(), "Hello");
  EXPECT_EQ(str_list.back(), "!");
}

// ----- Access Tests ----- //

TEST_F(SinglyLinkedListTest, AccessOnEmptyThrows) {
  EXPECT_THROW(list.front(), ListException);
  EXPECT_THROW(list.back(), ListException);

  const auto& const_list = list;
  EXPECT_THROW(const_list.front(), ListException);
  EXPECT_THROW(const_list.back(), ListException);
}

TEST_F(SinglyLinkedListTest, ConstCorrectness) {
  list.push_back(10);
  list.push_back(20);

  const auto& const_list = list;

  auto iterator = const_list.cbegin();
  ASSERT_NE(iterator, const_list.cend());
  EXPECT_EQ(*iterator, 10);
  ++iterator;
  ASSERT_NE(iterator, const_list.cend());
  EXPECT_EQ(*iterator, 20);
  ++iterator;
  EXPECT_EQ(iterator, const_list.cend());

  std::vector<int> actual;
  for (const int& val : const_list) {
    actual.push_back(val);
  }
  std::vector<int> expected = {10, 20};
  EXPECT_EQ(actual, expected);
}

// ----- Algorithm Tests (reverse) ----- //

TEST_F(SinglyLinkedListTest, Reverse) {
  list.push_back(1);
  list.push_back(2);
  list.push_back(3);

  list.reverse();
  EXPECT_EQ(list.front(), 3);
  EXPECT_EQ(list.back(), 1);

  std::vector<int> expected = {3, 2, 1};
  std::vector<int> actual;
  for (int val : list) {
    actual.push_back(val);
  }
  EXPECT_EQ(actual, expected);
}

TEST_F(SinglyLinkedListTest, ReverseEdgeCases) {
  // Empty list
  list.reverse();
  EXPECT_TRUE(list.is_empty());

  // Single element
  list.push_back(42);
  list.reverse();
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.front(), 42);
  EXPECT_EQ(list.back(), 42);
}

// ----- Move Semantics Tests ----- //

TEST_F(SinglyLinkedListTest, MoveSemantics) {
  list.push_back(10);
  list.push_back(20);

  // Move constructor
  SinglyLinkedList<int> moved_list_ctor = std::move(list);
  EXPECT_TRUE(list.is_empty());
  EXPECT_EQ(list.size(), 0);
  EXPECT_EQ(moved_list_ctor.size(), 2);
  EXPECT_EQ(moved_list_ctor.front(), 10);

  // Move assignment
  list = std::move(moved_list_ctor);
  EXPECT_TRUE(moved_list_ctor.is_empty());
  EXPECT_EQ(moved_list_ctor.size(), 0);
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.back(), 20);
}

// ----- Iterator Tests ----- //

TEST_F(SinglyLinkedListTest, IteratorTraversal) {
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i);
  }

  std::vector<int> actual;
  for (auto it = list.begin(); it != list.end(); ++it) {
    actual.push_back(*it);
  }

  std::vector<int> expected = {1, 2, 3, 4, 5};
  EXPECT_EQ(actual, expected);
}

TEST_F(SinglyLinkedListTest, RangeBasedFor) {
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);

  int sum = 0;
  for (const auto& val : list) {
    sum += val;
  }
  EXPECT_EQ(sum, 60);
}

// ----- Large List Test ----- //

TEST_F(SinglyLinkedListTest, LargeListOperations) {
  const int N = 1000;

  for (int i = 0; i < N; ++i) {
    list.push_back(i);
  }

  EXPECT_EQ(list.size(), N);
  EXPECT_EQ(list.front(), 0);
  EXPECT_EQ(list.back(), N - 1);

  list.reverse();
  EXPECT_EQ(list.front(), N - 1);
  EXPECT_EQ(list.back(), 0);

  list.clear();
  EXPECT_TRUE(list.is_empty());
}
