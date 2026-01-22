//===--------------------------------------------------------------------------===//
/**
 * @file test_Singly_Linked_List.cpp
 * @brief Unit tests for SinglyLinkedList (GoogleTest)
 */
//===--------------------------------------------------------------------------===//

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "../include/ads/lists/Singly_Linked_List.hpp"

using namespace ads::lists;

template <typename T>
auto collect_values(const SinglyLinkedList<T>& list) -> std::vector<T> {
  std::vector<T> values;
  for (const auto& v : list) {
    values.push_back(v);
  }
  return values;
}

TEST(SinglyLinkedListTest, BasicOperations) {
  SinglyLinkedList<int> list;

  EXPECT_TRUE(list.is_empty());
  EXPECT_EQ(list.size(), 0);

  list.push_front(3);
  list.push_front(2);
  list.push_front(1);
  EXPECT_EQ(collect_values(list), (std::vector<int>{1, 2, 3}));
  EXPECT_EQ(list.front(), 1);
  EXPECT_EQ(list.back(), 3);

  list.push_back(4);
  list.push_back(5);
  list.push_back(6);
  EXPECT_EQ(collect_values(list), (std::vector<int>{1, 2, 3, 4, 5, 6}));
  EXPECT_EQ(list.size(), 6);

  list.pop_front();
  list.pop_front();
  EXPECT_EQ(collect_values(list), (std::vector<int>{3, 4, 5, 6}));
  EXPECT_EQ(list.front(), 3);

  list.pop_back();
  EXPECT_EQ(collect_values(list), (std::vector<int>{3, 4, 5}));
  EXPECT_EQ(list.back(), 5);
}

TEST(SinglyLinkedListTest, EmplaceOperations) {
  SinglyLinkedList<std::string> list;
  list.emplace_front("Hello");
  list.emplace_back("World");
  list.emplace_front("Say");

  EXPECT_EQ(collect_values(list), (std::vector<std::string>{"Say", "Hello", "World"}));
  EXPECT_EQ(list.size(), 3);
}

TEST(SinglyLinkedListTest, Reverse) {
  SinglyLinkedList<int> list;
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i);
  }

  list.reverse();
  EXPECT_EQ(collect_values(list), (std::vector<int>{5, 4, 3, 2, 1}));
  EXPECT_EQ(list.front(), 5);
  EXPECT_EQ(list.back(), 1);

  SinglyLinkedList<int> single;
  single.push_back(42);
  single.reverse();
  EXPECT_EQ(single.front(), 42);
  EXPECT_EQ(single.back(), 42);

  SinglyLinkedList<int> empty;
  empty.reverse();
  EXPECT_TRUE(empty.is_empty());
}

TEST(SinglyLinkedListTest, ClearOperation) {
  SinglyLinkedList<int> list;
  for (int i = 0; i < 10; ++i) {
    list.push_back(i);
  }
  EXPECT_EQ(list.size(), 10);

  list.clear();
  EXPECT_TRUE(list.is_empty());
  EXPECT_EQ(list.size(), 0);
}

TEST(SinglyLinkedListTest, MoveSemantics) {
  SinglyLinkedList<int> list1;
  for (int i = 1; i <= 5; ++i) {
    list1.push_back(i);
  }

  SinglyLinkedList<int> list2(std::move(list1));
  EXPECT_TRUE(list1.is_empty());
  EXPECT_EQ(list2.size(), 5);
  EXPECT_EQ(collect_values(list2), (std::vector<int>{1, 2, 3, 4, 5}));

  SinglyLinkedList<int> list3;
  list3.push_back(99);
  list3 = std::move(list2);
  EXPECT_TRUE(list2.is_empty());
  EXPECT_EQ(list3.size(), 5);
  EXPECT_EQ(list3.front(), 1);
  EXPECT_EQ(list3.back(), 5);
}

TEST(SinglyLinkedListTest, ExceptionHandling) {
  SinglyLinkedList<int> list;

  EXPECT_THROW({ list.front(); }, ListException);
  EXPECT_THROW({ list.back(); }, ListException);
  EXPECT_THROW({ list.pop_front(); }, ListException);
  EXPECT_THROW({ list.pop_back(); }, ListException);
}

TEST(SinglyLinkedListTest, LargeList) {
  SinglyLinkedList<int> list;
  const int             N = 10000;

  for (int i = 0; i < N; ++i) {
    list.push_back(i);
  }

  EXPECT_EQ(list.size(), static_cast<size_t>(N));
  EXPECT_EQ(list.front(), 0);
  EXPECT_EQ(list.back(), N - 1);

  list.reverse();
  EXPECT_EQ(list.front(), N - 1);
  EXPECT_EQ(list.back(), 0);

  list.clear();
  EXPECT_TRUE(list.is_empty());
}

TEST(SinglyLinkedListTest, ConstIterator) {
  SinglyLinkedList<int> list;
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i);
  }

  const SinglyLinkedList<int>& const_list = list;
  std::vector<int>             collected;
  for (auto it = const_list.cbegin(); it != const_list.cend(); ++it) {
    collected.push_back(*it);
  }

  EXPECT_EQ(collected, (std::vector<int>{1, 2, 3, 4, 5}));
}

TEST(SinglyLinkedListTest, RValueInsertion) {
  SinglyLinkedList<std::string> list;
  std::string                   s1 = "Move1";
  std::string                   s2 = "Move2";

  list.push_front(std::move(s1));
  list.push_back(std::move(s2));

  EXPECT_EQ(collect_values(list), (std::vector<std::string>{"Move1", "Move2"}));
}
