//===--------------------------------------------------------------------------===//
/**
 * @file Test_DoublyLinkedList.cpp
 * @author Costantino Lombardi
 * @brief Test cases for DoublyLinkedList
 * @version 0.1
 * @date 2025-06-30
 *
 * @copyright Copyright (c) 2025
 *
 */
//===--------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/lists/Doubly_Linked_List.hpp"

using namespace ads::list;

// Test fixture to initeratorialize a clean list for each test
class DoublyLinkedListTest : public ::testing::Test {
protected:
  DoublyLinkedList<int> list;
};

// ----- Basic tests and state ----- //

TEST_F(DoublyLinkedListTest, IsEmptyOnConstruction) {
  EXPECT_EQ(list.size(), 0);
  EXPECT_TRUE(list.is_empty());
}

TEST_F(DoublyLinkedListTest, Clear) {
  list.push_back(10);
  list.push_back(20);
  EXPECT_FALSE(list.is_empty());

  list.clear();
  EXPECT_EQ(list.size(), 0);
  EXPECT_TRUE(list.is_empty());
  // We verify that access fails after clear()
  EXPECT_THROW(list.front(), ListException);
}

// ----- Tests on modifiers (push/pop/emplace) ----- //

TEST_F(DoublyLinkedListTest, PushFront) {
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

TEST_F(DoublyLinkedListTest, PushBack) {
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

TEST_F(DoublyLinkedListTest, PopFront) {
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
  // Now we expect our custom exception
  EXPECT_THROW(list.pop_front(), ListException);
}

TEST_F(DoublyLinkedListTest, PopBack) {
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
  // Here we expect the defined exception
  EXPECT_THROW(list.pop_back(), ListException);
}

// Test for emplace witeratorh a non-trivial type like std::string
TEST(DoublyLinkedListStringTest, Emplace) {
  DoublyLinkedList<std::string> str_list;
  str_list.emplace_front("world");
  str_list.emplace_back("!");
  str_list.emplace_front("Hello");

  EXPECT_EQ(str_list.size(), 3);
  EXPECT_EQ(str_list.front(), "Hello");
  EXPECT_EQ(str_list.back(), "!");
}

// ----- Tests for access and iteratorerators ----- //

TEST_F(DoublyLinkedListTest, AccessOnEmptyThrows) {
  EXPECT_THROW(list.front(), ListException);
  EXPECT_THROW(list.back(), ListException);

  const auto& const_list = list;
  EXPECT_THROW(const_list.front(), ListException);
  EXPECT_THROW(const_list.back(), ListException);
}

TEST_F(DoublyLinkedListTest, ConstCorrectness) {
  list.push_back(10);
  list.push_back(20);

  const auto& const_list = list;

  // Verify that iteratoreration over a const list works correctly
  auto iterator = const_list.cbegin();
  ASSERT_NE(iterator, const_list.cend());
  EXPECT_EQ(*iterator, 10);
  ++iterator;
  ASSERT_NE(iterator, const_list.cend());
  EXPECT_EQ(*iterator, 20);
  ++iterator;
  EXPECT_EQ(iterator, const_list.cend());

  // The following code should NOT compile, which proves const_iteratorerator correctness:
  // *const_list.cbegin() = 5;

  // Verify that range-based for loop works witeratorh a const list
  std::vector<int> actual;
  for (const int& val : const_list) {
    actual.push_back(val);
  }
  std::vector<int> expected = {10, 20};
  EXPECT_EQ(actual, expected);
}

// ----- Tests for algoriteratorhms (insert, erase, reverse) ----- //

TEST_F(DoublyLinkedListTest, InsertComprehensive) {
  // 1. Insertion into an empty list (equivalent to push_front)
  list.insert(list.begin(), 10);
  EXPECT_EQ(list.front(), 10);
  EXPECT_EQ(list.size(), 1);

  // 2. Insertion at the end
  list.insert(list.end(), 30);
  EXPECT_EQ(list.back(), 30);
  EXPECT_EQ(list.size(), 2);

  // 3. Insertion in the middle
  auto iterator = list.begin();
  ++iterator; // iterator points to 30
  list.insert(iterator, 20);
  EXPECT_EQ(list.size(), 3);

  std::vector<int> expected = {10, 20, 30};
  std::vector<int> actual;
  for (int val : list) {
    actual.push_back(val);
  }
  EXPECT_EQ(actual, expected);
}

TEST_F(DoublyLinkedListTest, EraseComprehensive) {
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);
  list.push_back(40);

  // 1. Remove from the middle
  auto iterator = list.begin();
  ++iterator; // iterator points to 20
  iterator = list.erase(iterator);
  EXPECT_EQ(*iterator, 30);
  EXPECT_EQ(list.size(), 3);

  // 2. Remove from the head
  iterator = list.erase(list.begin());
  EXPECT_EQ(*iterator, 30);
  EXPECT_EQ(list.size(), 2);

  // 3. Remove from the tail
  auto iterator_to_tail = list.begin();
  ++iterator_to_tail; // Now points to 40 (the last element)
  iterator = list.erase(iterator_to_tail);

  EXPECT_EQ(iterator, list.end()); // The returned iteratorerator is end()
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.back(), 30);
}

TEST_F(DoublyLinkedListTest, Reverse) {
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

TEST_F(DoublyLinkedListTest, ReverseEdgeCases) {
  // 1. Reversing an empty list
  list.reverse();
  EXPECT_TRUE(list.is_empty());

  // 2. Reversing a list witeratorh a single element
  list.push_back(42);
  list.reverse();
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.front(), 42);
  EXPECT_EQ(list.back(), 42);
}

// ----- Tests for special semantics ----- //

TEST_F(DoublyLinkedListTest, MoveSemantics) {
  list.push_back(10);
  list.push_back(20);

  // Test move constructor
  DoublyLinkedList<int> moved_list_ctor = std::move(list);
  EXPECT_TRUE(list.is_empty()); // The original list is empty
  EXPECT_EQ(list.size(), 0);
  EXPECT_EQ(moved_list_ctor.size(), 2);
  EXPECT_EQ(moved_list_ctor.front(), 10);

  // Test move assignment
  list = std::move(moved_list_ctor);
  EXPECT_TRUE(moved_list_ctor.is_empty());
  EXPECT_EQ(moved_list_ctor.size(), 0);
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.back(), 20);
}
