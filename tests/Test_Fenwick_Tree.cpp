//===---------------------------------------------------------------------------===//
/**
 * @file Test_Fenwick_Tree.cpp
 * @brief Google Test unit tests for FenwickTree implementation.
 * @version 0.1
 * @date 2026-01-28
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <vector>

#include "../include/ads/trees/Fenwick_Tree.hpp"

using namespace ads::trees;

class FenwickTreeTest : public ::testing::Test {
protected:
  FenwickTree<int> tree{std::vector<int>{1, 2, 3, 4, 5}};
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST(FenwickTreeBasicTest, ConstructionFromSize) {
  FenwickTree<int> fenwick(5);
  EXPECT_EQ(fenwick.size(), 5);
  EXPECT_FALSE(fenwick.is_empty());
  EXPECT_EQ(fenwick.total_sum(), 0);
  EXPECT_EQ(fenwick.prefix_sum(4), 0);
}

TEST_F(FenwickTreeTest, PrefixSumQueries) {
  EXPECT_EQ(tree.prefix_sum(0), 1);
  EXPECT_EQ(tree.prefix_sum(2), 6);
  EXPECT_EQ(tree.prefix_sum(4), 15);
}

TEST_F(FenwickTreeTest, RangeSumQueries) {
  EXPECT_EQ(tree.range_sum(0, 4), 15);
  EXPECT_EQ(tree.range_sum(1, 3), 9);
  EXPECT_EQ(tree.range_sum(2, 2), 3);
}

TEST_F(FenwickTreeTest, ValueAtAccess) {
  EXPECT_EQ(tree.value_at(0), 1);
  EXPECT_EQ(tree.value_at(3), 4);
}

//===---------------------------- UPDATE OPERATIONS ----------------------------===//

TEST_F(FenwickTreeTest, AddUpdates) {
  tree.add(1, 5);
  EXPECT_EQ(tree.value_at(1), 7);
  EXPECT_EQ(tree.total_sum(), 20);
}

TEST_F(FenwickTreeTest, SetUpdates) {
  tree.set(2, 10);
  EXPECT_EQ(tree.value_at(2), 10);
  EXPECT_EQ(tree.range_sum(2, 2), 10);
  EXPECT_EQ(tree.total_sum(), 22);
}

//===--------------------------- RESET & CLEAR TESTS ---------------------------===//

TEST_F(FenwickTreeTest, ResetAndClear) {
  tree.clear();
  EXPECT_TRUE(tree.is_empty());

  tree.reset(3);
  EXPECT_EQ(tree.size(), 3);
  EXPECT_EQ(tree.total_sum(), 0);
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(FenwickTreeTest, MoveSemantics) {
  FenwickTree<int> moved = std::move(tree);
  EXPECT_EQ(moved.size(), 5);
  EXPECT_TRUE(tree.is_empty());

  FenwickTree<int> assigned;
  assigned = std::move(moved);
  EXPECT_EQ(assigned.size(), 5);
  EXPECT_TRUE(moved.is_empty());
}

//===-------------------------- ERROR HANDLING TESTS ---------------------------===//

TEST(FenwickTreeErrorTest, OutOfRangeThrows) {
  FenwickTree<int> fenwick(3);
  EXPECT_THROW(fenwick.add(3, 1), FenwickTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = fenwick.prefix_sum(3); }, FenwickTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = fenwick.range_sum(2, 1); }, FenwickTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = fenwick.range_sum(0, 3); }, FenwickTreeException);
}

//===---------------------------------------------------------------------------===//
