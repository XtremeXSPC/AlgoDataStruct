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

#include "../include/ads/trees/Fenwick_Tree.hpp"

#include <gtest/gtest.h>

#include <vector>

using namespace ads::trees;

class FenwickTreeTest : public ::testing::Test {
protected:
  FenwickTree<int> tree{std::vector<int>{1, 2, 3, 4, 5}};
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST(FenwickTreeBasicTest, DefaultConstruction) {
  FenwickTree<int> fenwick;
  EXPECT_EQ(fenwick.size(), 0);
  EXPECT_TRUE(fenwick.is_empty());
  EXPECT_EQ(fenwick.total_sum(), 0);
}

TEST(FenwickTreeBasicTest, ConstructionFromSize) {
  FenwickTree<int> fenwick(5);
  EXPECT_EQ(fenwick.size(), 5);
  EXPECT_FALSE(fenwick.is_empty());
  EXPECT_EQ(fenwick.total_sum(), 0);
  EXPECT_EQ(fenwick.prefix_sum(4), 0);
}

TEST(FenwickTreeBasicTest, ConstructionFromSizeAndValue) {
  FenwickTree<int> fenwick(4, 10);
  EXPECT_EQ(fenwick.size(), 4);
  EXPECT_EQ(fenwick.total_sum(), 40);
  EXPECT_EQ(fenwick.value_at(0), 10);
  EXPECT_EQ(fenwick.value_at(3), 10);
}

TEST(FenwickTreeBasicTest, ConstructionFromInitializerList) {
  FenwickTree<int> fenwick{10, 20, 30};
  EXPECT_EQ(fenwick.size(), 3);
  EXPECT_EQ(fenwick.total_sum(), 60);
  EXPECT_EQ(fenwick.prefix_sum(1), 30);
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

TEST_F(FenwickTreeTest, BuildOnNonEmptyTree) {
  tree.build({100, 200, 300});
  EXPECT_EQ(tree.size(), 3);
  EXPECT_EQ(tree.total_sum(), 600);
  EXPECT_EQ(tree.value_at(0), 100);
  EXPECT_EQ(tree.value_at(2), 300);
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

//===----------------------------- EDGE CASE TESTS -----------------------------===//

TEST(FenwickTreeEdgeCaseTest, SingleElement) {
  FenwickTree<int> fenwick{42};
  EXPECT_EQ(fenwick.size(), 1);
  EXPECT_EQ(fenwick.total_sum(), 42);
  EXPECT_EQ(fenwick.prefix_sum(0), 42);
  EXPECT_EQ(fenwick.range_sum(0, 0), 42);
  EXPECT_EQ(fenwick.value_at(0), 42);

  fenwick.add(0, 8);
  EXPECT_EQ(fenwick.value_at(0), 50);
  EXPECT_EQ(fenwick.total_sum(), 50);
}

TEST(FenwickTreeEdgeCaseTest, AllZeros) {
  FenwickTree<int> fenwick(5);
  EXPECT_EQ(fenwick.total_sum(), 0);
  EXPECT_EQ(fenwick.prefix_sum(4), 0);
  EXPECT_EQ(fenwick.range_sum(1, 3), 0);

  for (size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(fenwick.value_at(i), 0);
  }
}

TEST(FenwickTreeEdgeCaseTest, NegativeValues) {
  FenwickTree<int> fenwick{-5, 10, -3, 8, -2};
  EXPECT_EQ(fenwick.total_sum(), 8);
  EXPECT_EQ(fenwick.prefix_sum(0), -5);
  EXPECT_EQ(fenwick.prefix_sum(1), 5);
  EXPECT_EQ(fenwick.range_sum(0, 2), 2);
  EXPECT_EQ(fenwick.range_sum(2, 4), 3);

  fenwick.add(0, -10);
  EXPECT_EQ(fenwick.value_at(0), -15);
  EXPECT_EQ(fenwick.total_sum(), -2);
}

TEST(FenwickTreeEdgeCaseTest, LargeValues) {
  FenwickTree<long long> fenwick{1'000'000'000LL, 2'000'000'000LL, 3'000'000'000LL};
  EXPECT_EQ(fenwick.total_sum(), 6'000'000'000LL);
  EXPECT_EQ(fenwick.prefix_sum(1), 3'000'000'000LL);
}

//===---------------------------- LOWER_BOUND TESTS ----------------------------===//

TEST(FenwickTreeLowerBoundTest, BasicLowerBound) {
  FenwickTree<int> fenwick{1, 2, 3, 4, 5}; // prefix sums: 1, 3, 6, 10, 15

  EXPECT_EQ(fenwick.lower_bound(1), 0);  // prefix_sum(0) = 1 >= 1
  EXPECT_EQ(fenwick.lower_bound(3), 1);  // prefix_sum(1) = 3 >= 3
  EXPECT_EQ(fenwick.lower_bound(4), 2);  // prefix_sum(2) = 6 >= 4
  EXPECT_EQ(fenwick.lower_bound(6), 2);  // prefix_sum(2) = 6 >= 6
  EXPECT_EQ(fenwick.lower_bound(7), 3);  // prefix_sum(3) = 10 >= 7
  EXPECT_EQ(fenwick.lower_bound(15), 4); // prefix_sum(4) = 15 >= 15
}

TEST(FenwickTreeLowerBoundTest, LowerBoundNotFound) {
  FenwickTree<int> fenwick{1, 2, 3, 4, 5}; // total = 15

  EXPECT_EQ(fenwick.lower_bound(16), 5); // Not found, returns size()
  EXPECT_EQ(fenwick.lower_bound(100), 5);
}

TEST(FenwickTreeLowerBoundTest, LowerBoundEmptyTree) {
  FenwickTree<int> fenwick;
  EXPECT_EQ(fenwick.lower_bound(1), 0);
}

TEST(FenwickTreeLowerBoundTest, LowerBoundSingleElement) {
  FenwickTree<int> fenwick{10};

  EXPECT_EQ(fenwick.lower_bound(5), 0);
  EXPECT_EQ(fenwick.lower_bound(10), 0);
  EXPECT_EQ(fenwick.lower_bound(11), 1);
}

TEST(FenwickTreeLowerBoundTest, LowerBoundAllSameValues) {
  FenwickTree<int> fenwick{5, 5, 5, 5}; // prefix sums: 5, 10, 15, 20

  EXPECT_EQ(fenwick.lower_bound(5), 0);
  EXPECT_EQ(fenwick.lower_bound(6), 1);
  EXPECT_EQ(fenwick.lower_bound(10), 1);
  EXPECT_EQ(fenwick.lower_bound(11), 2);
}

//===---------------------------- CUSTOM TYPE TESTS ----------------------------===//

TEST(FenwickTreeCustomTypeTest, DoubleType) {
  FenwickTree<double> fenwick{1.5, 2.5, 3.0};
  EXPECT_DOUBLE_EQ(fenwick.total_sum(), 7.0);
  EXPECT_DOUBLE_EQ(fenwick.prefix_sum(1), 4.0);
  EXPECT_DOUBLE_EQ(fenwick.range_sum(0, 1), 4.0);

  fenwick.add(0, 0.5);
  EXPECT_DOUBLE_EQ(fenwick.value_at(0), 2.0);
  EXPECT_DOUBLE_EQ(fenwick.total_sum(), 7.5);
}

TEST(FenwickTreeCustomTypeTest, LongLongType) {
  FenwickTree<long long> fenwick{1LL, 2LL, 3LL, 4LL, 5LL};
  EXPECT_EQ(fenwick.total_sum(), 15LL);
  EXPECT_EQ(fenwick.prefix_sum(2), 6LL);

  fenwick.set(0, 100LL);
  EXPECT_EQ(fenwick.value_at(0), 100LL);
  EXPECT_EQ(fenwick.total_sum(), 114LL);
}

//===-------------------------- ERROR HANDLING TESTS ---------------------------===//

TEST(FenwickTreeErrorTest, OutOfRangeThrows) {
  FenwickTree<int> fenwick(3);
  EXPECT_THROW(fenwick.add(3, 1), FenwickTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = fenwick.prefix_sum(3); }, FenwickTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = fenwick.range_sum(2, 1); }, FenwickTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = fenwick.range_sum(0, 3); }, FenwickTreeException);
}

TEST(FenwickTreeErrorTest, ValueAtOutOfRange) {
  FenwickTree<int> fenwick{1, 2, 3};
  EXPECT_THROW({ [[maybe_unused]] auto _ = fenwick.value_at(3); }, FenwickTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = fenwick.value_at(100); }, FenwickTreeException);
}

TEST(FenwickTreeErrorTest, SetOutOfRange) {
  FenwickTree<int> fenwick{1, 2, 3};
  EXPECT_THROW(fenwick.set(3, 10), FenwickTreeException);
}

//===---------------------------------------------------------------------------===//
