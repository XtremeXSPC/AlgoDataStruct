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

//===----- BASIC STATE TESTS ---------------------------------------------------===//

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

//===----- REVERSE TESTS -------------------------------------------------------===//

TEST_F(CircularLinkedListTest, Reverse) {
  for (int i = 1; i <= 4; ++i) {
    list.push_back(i); // 1 2 3 4
  }
  list.reverse(); // 4 3 2 1
  EXPECT_EQ(list.front(), 4);
  EXPECT_EQ(list.back(), 1);

  std::vector<int> actual;
  for (int v : list) {
    actual.push_back(v);
  }
  EXPECT_EQ(actual, (std::vector<int>{4, 3, 2, 1}));
}

TEST_F(CircularLinkedListTest, ReverseEdgeCases) {
  list.reverse(); // empty
  EXPECT_TRUE(list.is_empty());

  list.push_back(7);
  list.reverse(); // single element
  EXPECT_EQ(list.size(), 1u);
  EXPECT_EQ(list.front(), 7);
  EXPECT_EQ(list.back(), 7);
}

//===----- EQUALITY TESTS ------------------------------------------------------===//

TEST_F(CircularLinkedListTest, EqualityOperator) {
  CircularLinkedList<int> other;
  EXPECT_TRUE(list == other); // both empty

  for (int i = 1; i <= 3; ++i) {
    list.push_back(i);
    other.push_back(i);
  }
  EXPECT_TRUE(list == other);
  EXPECT_FALSE(list != other);

  other.push_back(4);
  EXPECT_TRUE(list != other); // different size

  list.push_back(9);
  EXPECT_TRUE(list != other); // same size, differing element
}

//===----- CONCEPT CONFORMANCE -------------------------------------------------===//

static_assert(std::forward_iterator<CircularLinkedList<int>::iterator>);
static_assert(std::forward_iterator<CircularLinkedList<int>::const_iterator>);

//===----- MOVE-ONLY PAYLOAD TESTS ---------------------------------------------===//

// Move-only, non-copyable payload to exercise the move-only push/emplace paths.
struct CllMoveOnly {
  int value     = 0;
  CllMoveOnly() = default;

  explicit CllMoveOnly(int v) : value(v) {}

  CllMoveOnly(CllMoveOnly&&) noexcept            = default;
  CllMoveOnly& operator=(CllMoveOnly&&) noexcept = default;
  CllMoveOnly(const CllMoveOnly&)                = delete;
  CllMoveOnly& operator=(const CllMoveOnly&)     = delete;

  auto operator==(const CllMoveOnly& o) const -> bool { return value == o.value; }
};

TEST(CircularLinkedListMoveOnly, HoldsMoveOnlyValues) {
  CircularLinkedList<CllMoveOnly> mo;
  mo.push_back(CllMoveOnly(1));
  mo.emplace_back(2);
  mo.push_front(CllMoveOnly(0));
  EXPECT_EQ(mo.size(), 3u);
  EXPECT_EQ(mo.front().value, 0);
  EXPECT_EQ(mo.back().value, 2);
  EXPECT_TRUE(mo.contains(CllMoveOnly(1)));
}

//===---------------------------------------------------------------------------===//
