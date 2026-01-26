//===---------------------------------------------------------------------------===//
/**
 * @file Test_Dynamic_Array.cpp
 * @brief Google Test unit tests for DynamicArray.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/arrays/Dynamic_Array.hpp"

using namespace ads::arrays;

// Test fixture for DynamicArray.
class DynamicArrayTest : public ::testing::Test {
protected:
  DynamicArray<int> array;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(DynamicArrayTest, IsEmptyOnConstruction) {
  EXPECT_EQ(array.size(), 0);
  EXPECT_TRUE(array.is_empty());
}

TEST_F(DynamicArrayTest, PushBackAndAccess) {
  array.push_back(10);
  array.push_back(20);

  EXPECT_EQ(array.size(), 2);
  EXPECT_EQ(array.front(), 10);
  EXPECT_EQ(array.back(), 20);
  EXPECT_EQ(array[1], 20);
}

TEST_F(DynamicArrayTest, PopBack) {
  array.push_back(5);
  array.push_back(15);

  array.pop_back();
  EXPECT_EQ(array.size(), 1);
  EXPECT_EQ(array.back(), 5);
}

TEST_F(DynamicArrayTest, InsertAndErase) {
  array.push_back(10);
  array.push_back(30);

  array.insert(1, 20);
  EXPECT_EQ(array.size(), 3);
  EXPECT_EQ(array[1], 20);

  array.erase(1);
  EXPECT_EQ(array.size(), 2);
  EXPECT_EQ(array[1], 30);
}

TEST_F(DynamicArrayTest, EmplaceBack) {
  DynamicArray<std::string> strings;
  strings.emplace_back("Hello");
  strings.emplace_back("World");

  EXPECT_EQ(strings.size(), 2);
  EXPECT_EQ(strings.back(), "World");
}

//===-------------------------- ERROR HANDLING TESTS ---------------------------===//

TEST_F(DynamicArrayTest, PopBackOnEmptyThrows) {
  EXPECT_THROW(array.pop_back(), ArrayUnderflowException);
}

TEST_F(DynamicArrayTest, AtThrowsOnOutOfRange) {
  array.push_back(1);
  EXPECT_THROW(array.at(1), ArrayOutOfRangeException);
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(DynamicArrayTest, MoveSemantics) {
  array.push_back(10);
  array.push_back(20);

  DynamicArray<int> moved = std::move(array);
  EXPECT_TRUE(array.is_empty());
  EXPECT_EQ(moved.size(), 2);

  array = std::move(moved);
  EXPECT_TRUE(moved.is_empty());
  EXPECT_EQ(array.size(), 2);
}

TEST_F(DynamicArrayTest, RangeBasedIteration) {
  for (int i = 1; i <= 4; ++i) {
    array.push_back(i);
  }

  std::vector<int> values;
  for (const auto& value : array) {
    values.push_back(value);
  }

  std::vector<int> expected = {1, 2, 3, 4};
  EXPECT_EQ(values, expected);
}

//===---------------------------------------------------------------------------===//
