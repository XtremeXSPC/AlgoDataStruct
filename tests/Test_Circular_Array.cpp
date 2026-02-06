//===---------------------------------------------------------------------------===//
/**
 * @file Test_Circular_Array.cpp
 * @brief Google Test unit tests for CircularArray.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/arrays/Circular_Array.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ads::arrays;

// Test fixture for CircularArray.
class CircularArrayTest : public ::testing::Test {
protected:
  CircularArray<int> array;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(CircularArrayTest, IsEmptyOnConstruction) {
  EXPECT_EQ(array.size(), 0u);
  EXPECT_TRUE(array.is_empty());
}

TEST_F(CircularArrayTest, PushBackAndAccess) {
  array.push_back(10);
  array.push_back(20);
  array.push_back(30);

  EXPECT_EQ(array.size(), 3u);
  EXPECT_EQ(array.front(), 10);
  EXPECT_EQ(array.back(), 30);
  EXPECT_EQ(array.at(1), 20);
}

TEST_F(CircularArrayTest, PushFrontAndAccess) {
  array.push_front(10);
  array.push_front(20);
  array.push_front(30);

  EXPECT_EQ(array.size(), 3u);
  EXPECT_EQ(array.front(), 30);
  EXPECT_EQ(array.back(), 10);
  EXPECT_EQ(array.at(1), 20);
}

TEST_F(CircularArrayTest, MixedPushOperations) {
  array.push_back(2);
  array.push_front(1);
  array.push_back(3);
  array.push_front(0);

  EXPECT_EQ(array.size(), 4u);
  EXPECT_EQ(array.at(0), 0);
  EXPECT_EQ(array.at(1), 1);
  EXPECT_EQ(array.at(2), 2);
  EXPECT_EQ(array.at(3), 3);
}

TEST_F(CircularArrayTest, PopFrontAndBack) {
  array.push_back(1);
  array.push_back(2);
  array.push_back(3);
  array.push_back(4);

  array.pop_front();
  EXPECT_EQ(array.front(), 2);
  EXPECT_EQ(array.size(), 3u);

  array.pop_back();
  EXPECT_EQ(array.back(), 3);
  EXPECT_EQ(array.size(), 2u);
}

TEST_F(CircularArrayTest, PopOnEmptyThrows) {
  EXPECT_THROW(array.pop_front(), ArrayUnderflowException);
  EXPECT_THROW(array.pop_back(), ArrayUnderflowException);
}

TEST_F(CircularArrayTest, AtThrowsOnOutOfRange) {
  array.push_back(1);
  EXPECT_THROW(array.at(1), ArrayOutOfRangeException);
}

TEST_F(CircularArrayTest, WrapAroundBehavior) {
  for (int i = 0; i < 5; ++i) {
    array.push_back(i);
  }
  for (int i = 0; i < 3; ++i) {
    array.pop_front();
  }
  for (int i = 5; i < 10; ++i) {
    array.push_back(i);
  }

  EXPECT_EQ(array.size(), 7u);
  EXPECT_EQ(array.front(), 3);
  EXPECT_EQ(array.back(), 9);
}

TEST_F(CircularArrayTest, InitializerListConstruction) {
  CircularArray<int> arr{1, 2, 3, 4, 5};
  EXPECT_EQ(arr.size(), 5u);
  EXPECT_EQ(arr.front(), 1);
  EXPECT_EQ(arr.back(), 5);
}

TEST_F(CircularArrayTest, MoveSemantics) {
  array.push_back(10);
  array.push_back(20);

  CircularArray<int> moved = std::move(array);
  EXPECT_TRUE(array.is_empty());
  EXPECT_EQ(moved.size(), 2u);

  array = std::move(moved);
  EXPECT_TRUE(moved.is_empty());
  EXPECT_EQ(array.size(), 2u);
}

TEST_F(CircularArrayTest, RangeBasedIteration) {
  for (int i = 1; i <= 5; ++i) {
    array.push_back(i);
  }

  std::vector<int> values;
  for (const auto& val : array) {
    values.push_back(val);
  }

  std::vector<int> expected{1, 2, 3, 4, 5};
  EXPECT_EQ(values, expected);
}

TEST_F(CircularArrayTest, ReverseIteration) {
  for (int i = 1; i <= 5; ++i) {
    array.push_back(i);
  }

  std::vector<int> values;
  for (auto it = array.rbegin(); it != array.rend(); ++it) {
    values.push_back(*it);
  }

  std::vector<int> expected{5, 4, 3, 2, 1};
  EXPECT_EQ(values, expected);
}

TEST_F(CircularArrayTest, IteratorArithmeticWithNegativeOffsets) {
  for (int i = 0; i < 6; ++i) {
    array.push_back(i);
  }

  auto it = array.begin();
  it += 4;
  it += -2;
  EXPECT_EQ(*it, 2);

  auto const_it = array.cend();
  const_it -= 3;
  EXPECT_EQ(*const_it, 3);

  auto shifted = array.begin() + 5;
  shifted      = shifted - 4;
  EXPECT_EQ(*shifted, 1);
}

TEST_F(CircularArrayTest, IterationAfterWrapAround) {
  for (int i = 0; i < 10; ++i) {
    array.push_back(i);
  }
  for (int i = 0; i < 5; ++i) {
    array.pop_front();
  }

  std::vector<int> values;
  for (const auto& val : array) {
    values.push_back(val);
  }

  std::vector<int> expected{5, 6, 7, 8, 9};
  EXPECT_EQ(values, expected);
}

TEST_F(CircularArrayTest, ClearOperation) {
  for (int i = 0; i < 10; ++i) {
    array.push_back(i);
  }

  array.clear();
  EXPECT_TRUE(array.is_empty());
  EXPECT_EQ(array.size(), 0u);
}

TEST_F(CircularArrayTest, EmplaceOperations) {
  CircularArray<std::string> strings;
  strings.emplace_back("Hello");
  strings.emplace_front("World");

  EXPECT_EQ(strings.front(), "World");
  EXPECT_EQ(strings.back(), "Hello");
}

TEST_F(CircularArrayTest, CapacityGrowth) {
  size_t initial_capacity = array.capacity();

  for (size_t i = 0; i < initial_capacity + 10; ++i) {
    array.push_back(static_cast<int>(i));
  }

  EXPECT_GT(array.capacity(), initial_capacity);
  EXPECT_EQ(array.size(), initial_capacity + 10);
}

TEST_F(CircularArrayTest, ReserveCapacity) {
  array.reserve(100);
  EXPECT_GE(array.capacity(), 100u);
  EXPECT_TRUE(array.is_empty());
}

TEST_F(CircularArrayTest, ShrinkToFit) {
  for (int i = 0; i < 100; ++i) {
    array.push_back(i);
  }
  for (int i = 0; i < 90; ++i) {
    array.pop_back();
  }

  array.shrink_to_fit();
  EXPECT_LE(array.capacity(), 20u);
  EXPECT_EQ(array.size(), 10u);
}

//===---------------------------------------------------------------------------===//
