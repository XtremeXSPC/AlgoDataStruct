//===---------------------------------------------------------------------------===//
/**
 * @file Test_Static_Array.cpp
 * @brief Google Test unit tests for StaticArray.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/arrays/Static_Array.hpp"

using namespace ads::arrays;

// Test fixture for StaticArray.
class StaticArrayTest : public ::testing::Test {
protected:
  StaticArray<int, 5> array{1, 2, 3, 4, 5};
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(StaticArrayTest, SizeIsFixed) {
  EXPECT_EQ(array.size(), 5u);
  EXPECT_EQ(decltype(array)::size(), 5u);
  EXPECT_FALSE(array.is_empty());
}

TEST_F(StaticArrayTest, InitializerListConstruction) {
  StaticArray<int, 3> arr{10, 20, 30};
  EXPECT_EQ(arr.at(0), 10);
  EXPECT_EQ(arr.at(1), 20);
  EXPECT_EQ(arr.at(2), 30);
}

TEST_F(StaticArrayTest, FillConstruction) {
  StaticArray<int, 4> arr(42);
  for (const auto& val : arr) {
    EXPECT_EQ(val, 42);
  }
}

TEST_F(StaticArrayTest, DefaultConstruction) {
  StaticArray<int, 3> arr;
  for (const auto& val : arr) {
    EXPECT_EQ(val, 0);
  }
}

TEST_F(StaticArrayTest, FrontAndBack) {
  EXPECT_EQ(array.front(), 1);
  EXPECT_EQ(array.back(), 5);

  array.front() = 100;
  array.back()  = 500;
  EXPECT_EQ(array[0], 100);
  EXPECT_EQ(array[4], 500);
}

TEST_F(StaticArrayTest, AtWithBoundsChecking) {
  EXPECT_EQ(array.at(2), 3);
  EXPECT_THROW(array.at(5), ArrayOutOfRangeException);
  EXPECT_THROW(array.at(100), ArrayOutOfRangeException);
}

TEST_F(StaticArrayTest, SubscriptOperator) {
  EXPECT_EQ(array[0], 1);
  array[0] = 99;
  EXPECT_EQ(array[0], 99);
}

TEST_F(StaticArrayTest, DataPointer) {
  int* ptr = array.data();
  EXPECT_EQ(*ptr, 1);
  EXPECT_EQ(*(ptr + 4), 5);
}

TEST_F(StaticArrayTest, FillMethod) {
  array.fill(7);
  for (size_t i = 0; i < array.size(); ++i) {
    EXPECT_EQ(array[i], 7);
  }
}

TEST_F(StaticArrayTest, SwapArrays) {
  StaticArray<int, 5> other{10, 20, 30, 40, 50};
  array.swap(other);

  EXPECT_EQ(array[0], 10);
  EXPECT_EQ(other[0], 1);
}

TEST_F(StaticArrayTest, CopyConstruction) {
  StaticArray<int, 5> copy(array);
  EXPECT_EQ(copy[0], array[0]);
  EXPECT_EQ(copy[4], array[4]);

  copy[0] = 999;
  EXPECT_NE(copy[0], array[0]);
}

TEST_F(StaticArrayTest, MoveConstruction) {
  StaticArray<int, 5> source{1, 2, 3, 4, 5};
  StaticArray<int, 5> moved(std::move(source));
  EXPECT_EQ(moved[0], 1);
  EXPECT_EQ(moved[4], 5);
}

TEST_F(StaticArrayTest, CopyAssignment) {
  StaticArray<int, 5> other{10, 20, 30, 40, 50};
  array = other;
  EXPECT_EQ(array[0], 10);
  EXPECT_EQ(array[4], 50);
}

TEST_F(StaticArrayTest, MoveAssignment) {
  StaticArray<int, 5> other{10, 20, 30, 40, 50};
  array = std::move(other);
  EXPECT_EQ(array[0], 10);
  EXPECT_EQ(array[4], 50);
}

TEST_F(StaticArrayTest, EqualityComparison) {
  StaticArray<int, 5> same{1, 2, 3, 4, 5};
  StaticArray<int, 5> different{1, 2, 3, 4, 6};

  EXPECT_TRUE(array == same);
  EXPECT_FALSE(array == different);
}

TEST_F(StaticArrayTest, ThreeWayComparison) {
  StaticArray<int, 5> same{1, 2, 3, 4, 5};
  StaticArray<int, 5> less{1, 2, 3, 4, 4};
  StaticArray<int, 5> greater{1, 2, 3, 4, 6};

  EXPECT_TRUE((array <=> same) == 0);
  EXPECT_TRUE((array <=> less) > 0);
  EXPECT_TRUE((array <=> greater) < 0);
}

TEST_F(StaticArrayTest, RangeBasedIteration) {
  std::vector<int> values;
  for (const auto& val : array) {
    values.push_back(val);
  }

  std::vector<int> expected{1, 2, 3, 4, 5};
  EXPECT_EQ(values, expected);
}

TEST_F(StaticArrayTest, ReverseIteration) {
  std::vector<int> values;
  for (auto it = array.rbegin(); it != array.rend(); ++it) {
    values.push_back(*it);
  }

  std::vector<int> expected{5, 4, 3, 2, 1};
  EXPECT_EQ(values, expected);
}

TEST_F(StaticArrayTest, ConstIteration) {
  const StaticArray<int, 5>& const_ref = array;
  std::vector<int>           values;
  for (auto it = const_ref.cbegin(); it != const_ref.cend(); ++it) {
    values.push_back(*it);
  }

  std::vector<int> expected{1, 2, 3, 4, 5};
  EXPECT_EQ(values, expected);
}

TEST_F(StaticArrayTest, StringType) {
  StaticArray<std::string, 3> strings{"hello", "world", "test"};
  EXPECT_EQ(strings[0], "hello");
  EXPECT_EQ(strings[1], "world");
  EXPECT_EQ(strings[2], "test");
}

TEST_F(StaticArrayTest, InitializerListSizeMismatchThrows) {
  EXPECT_THROW((StaticArray<int, 5>{1, 2, 3}), ArrayOutOfRangeException);
  EXPECT_THROW((StaticArray<int, 3>{1, 2, 3, 4, 5}), ArrayOutOfRangeException);
}

//===---------------------------------------------------------------------------===//
