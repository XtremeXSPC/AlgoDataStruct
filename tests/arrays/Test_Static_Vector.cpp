//===---------------------------------------------------------------------------===//
/**
 * @file Test_Static_Vector.cpp
 * @brief Google Test unit tests for StaticVector.
 * @version 0.1
 * @date 2026-06-08
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Static_Vector.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ads::arrays;

//===----- TEST FIXTURE --------------------------------------------------------===//

class StaticVectorTest : public ::testing::Test {
protected:
  StaticVector<int, 5> vec{1, 2, 3};
};

//===----- BASIC STATE TESTS ---------------------------------------------------===//

TEST_F(StaticVectorTest, CapacityIsFixedSizeIsDynamic) {
  EXPECT_EQ(vec.size(), 3u);
  EXPECT_EQ(vec.capacity(), 5u);
  EXPECT_EQ(decltype(vec)::max_size(), 5u);
  EXPECT_FALSE(vec.is_empty());
  EXPECT_FALSE(vec.is_full());
}

TEST_F(StaticVectorTest, DefaultConstructionIsEmpty) {
  StaticVector<int, 4> v;
  EXPECT_TRUE(v.is_empty());
  EXPECT_EQ(v.size(), 0u);
  EXPECT_EQ(v.capacity(), 4u);
}

TEST_F(StaticVectorTest, CountValueConstruction) {
  StaticVector<int, 8> v(3, 7);
  EXPECT_EQ(v.size(), 3u);
  for (const auto& x : v) {
    EXPECT_EQ(x, 7);
  }
}

TEST_F(StaticVectorTest, InitializerListOverCapacityThrows) {
  EXPECT_THROW((StaticVector<int, 2>{1, 2, 3}), ArrayOverflowException);
  EXPECT_THROW((StaticVector<int, 3>(4, 0)), ArrayOverflowException);
}

//===----- INSERTION TESTS -----------------------------------------------------===//

TEST_F(StaticVectorTest, PushBackUntilFull) {
  vec.push_back(4);
  vec.push_back(5);
  EXPECT_TRUE(vec.is_full());
  EXPECT_EQ(vec.size(), 5u);
  EXPECT_EQ(vec.back(), 5);
}

TEST_F(StaticVectorTest, PushBackWhenFullThrowsAndLeavesIntact) {
  StaticVector<int, 3> v{1, 2, 3};
  EXPECT_TRUE(v.is_full());
  EXPECT_THROW(v.push_back(4), ArrayOverflowException);
  EXPECT_EQ(v.size(), 3u);
  EXPECT_EQ(v.back(), 3);
}

TEST_F(StaticVectorTest, EmplaceBackReturnsReference) {
  StaticVector<std::string, 4> v;
  auto&                        ref = v.emplace_back(5, 'a');
  EXPECT_EQ(ref, "aaaaa");
  EXPECT_EQ(v.back(), "aaaaa");
}

TEST_F(StaticVectorTest, InsertAtIndex) {
  vec.insert(1, 99);
  EXPECT_EQ(vec.size(), 4u);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 99);
  EXPECT_EQ(vec[2], 2);
}

TEST_F(StaticVectorTest, InsertOutOfRangeThrows) {
  EXPECT_THROW(vec.insert(10, 0), ArrayOutOfRangeException);
}

TEST_F(StaticVectorTest, InsertWhenFullThrows) {
  StaticVector<int, 3> v{1, 2, 3};
  EXPECT_THROW(v.insert(1, 0), ArrayOverflowException);
}

//===----- REMOVAL TESTS -------------------------------------------------------===//

TEST_F(StaticVectorTest, PopBack) {
  vec.pop_back();
  EXPECT_EQ(vec.size(), 2u);
  EXPECT_EQ(vec.back(), 2);
}

TEST_F(StaticVectorTest, PopBackOnEmptyThrows) {
  StaticVector<int, 4> v;
  EXPECT_THROW(v.pop_back(), ArrayUnderflowException);
}

TEST_F(StaticVectorTest, EraseAtIndex) {
  vec.erase(1);
  EXPECT_EQ(vec.size(), 2u);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 3);
}

TEST_F(StaticVectorTest, EraseOutOfRangeThrows) {
  EXPECT_THROW(vec.erase(5), ArrayOutOfRangeException);
}

TEST_F(StaticVectorTest, Clear) {
  vec.clear();
  EXPECT_TRUE(vec.is_empty());
  EXPECT_EQ(vec.size(), 0u);
}

TEST_F(StaticVectorTest, ResizeGrowAndShrink) {
  vec.resize(5, 8);
  EXPECT_EQ(vec.size(), 5u);
  EXPECT_EQ(vec[4], 8);
  vec.resize(2);
  EXPECT_EQ(vec.size(), 2u);
  EXPECT_EQ(vec.back(), 2);
}

TEST_F(StaticVectorTest, ResizeOverCapacityThrows) {
  EXPECT_THROW(vec.resize(6), ArrayOverflowException);
}

//===----- ACCESS TESTS --------------------------------------------------------===//

TEST_F(StaticVectorTest, AtWithBoundsChecking) {
  EXPECT_EQ(vec.at(0), 1);
  EXPECT_THROW(vec.at(3), ArrayOutOfRangeException);
}

TEST_F(StaticVectorTest, FrontBackAndData) {
  EXPECT_EQ(vec.front(), 1);
  EXPECT_EQ(vec.back(), 3);
  int* ptr = vec.data();
  EXPECT_EQ(*ptr, 1);
  EXPECT_EQ(*(ptr + 2), 3);
}

TEST_F(StaticVectorTest, FrontOnEmptyThrows) {
  StaticVector<int, 4> v;
  EXPECT_THROW(v.front(), ArrayUnderflowException);
  EXPECT_THROW(v.back(), ArrayUnderflowException);
}

//===----- ITERATION TESTS -----------------------------------------------------===//

TEST_F(StaticVectorTest, RangeBasedIteration) {
  std::vector<int> values;
  for (const auto& v : vec) {
    values.push_back(v);
  }
  EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
}

TEST_F(StaticVectorTest, ReverseIteration) {
  std::vector<int> values(vec.rbegin(), vec.rend());
  EXPECT_EQ(values, (std::vector<int>{3, 2, 1}));
}

TEST_F(StaticVectorTest, ConstIteration) {
  const StaticVector<int, 5>& cref = vec;
  std::vector<int>            values(cref.cbegin(), cref.cend());
  EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
}

//===----- COMPARISON TESTS ----------------------------------------------------===//

TEST_F(StaticVectorTest, EqualityAndThreeWay) {
  StaticVector<int, 5> same{1, 2, 3};
  StaticVector<int, 5> greater{1, 2, 4};
  EXPECT_TRUE(vec == same);
  EXPECT_TRUE(vec != greater);
  EXPECT_TRUE((vec <=> greater) < 0);
  EXPECT_TRUE((greater <=> vec) > 0);
}

TEST_F(StaticVectorTest, ComparisonRespectsSize) {
  StaticVector<int, 5> prefix{1, 2};
  EXPECT_TRUE(prefix < vec);
  EXPECT_FALSE(prefix == vec);
}

//===----- MOVE SEMANTICS TESTS ------------------------------------------------===//

TEST_F(StaticVectorTest, MoveConstructionLeavesSourceEmpty) {
  StaticVector<std::string, 4> s;
  s.emplace_back(10, 'x');
  s.emplace_back(5, 'y');
  StaticVector<std::string, 4> moved(std::move(s));
  EXPECT_EQ(moved.size(), 2u);
  EXPECT_EQ(moved[0].size(), 10u);
  EXPECT_TRUE(s.is_empty());
}

TEST_F(StaticVectorTest, MoveAssignmentLeavesSourceEmpty) {
  StaticVector<std::string, 4> s;
  s.emplace_back("hello");
  StaticVector<std::string, 4> dst;
  dst.emplace_back("discarded");
  dst = std::move(s);
  EXPECT_EQ(dst.size(), 1u);
  EXPECT_EQ(dst[0], "hello");
  EXPECT_TRUE(s.is_empty());
}

TEST_F(StaticVectorTest, IsMoveOnly) {
  static_assert(!std::is_copy_constructible_v<StaticVector<int, 4>>);
  static_assert(!std::is_copy_assignable_v<StaticVector<int, 4>>);
  static_assert(std::is_move_constructible_v<StaticVector<int, 4>>);
  SUCCEED();
}

TEST_F(StaticVectorTest, SatisfiesRandomAccessSequence) {
  static_assert(RandomAccessSequence<StaticVector<int, 8>>);
  SUCCEED();
}

//===---------------------------------------------------------------------------===//
