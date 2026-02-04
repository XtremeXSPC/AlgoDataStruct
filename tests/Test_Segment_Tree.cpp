//===---------------------------------------------------------------------------===//
/**
 * @file Test_Segment_Tree.cpp
 * @brief Google Test unit tests for SegmentTree implementation.
 * @version 0.2
 * @date 2026-02-04
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include <algorithm>
#include <gtest/gtest.h>
#include <limits>
#include <vector>

#include "../include/ads/trees/Segment_Tree.hpp"

using namespace ads::trees;

class SegmentTreeTest : public ::testing::Test {
protected:
  SegmentTree<int> tree{std::vector<int>{1, 2, 3, 4, 5}};
};

struct MaxCombine {
  auto operator()(int left, int right) const -> int { return std::max(left, right); }
};

struct MaxIdentity {
  auto operator()() const -> int { return std::numeric_limits<int>::min(); }
};

struct SumCount {
  int sum   = 0;
  int count = 0;
};

struct SumCountCombine {
  auto operator()(const SumCount& left, const SumCount& right) const -> SumCount {
    return SumCount{left.sum + right.sum, left.count + right.count};
  }
};

struct SumCountIdentity {
  auto operator()() const -> SumCount { return SumCount{}; }
};

struct SumCountLeaf {
  auto operator()(int value) const -> SumCount { return SumCount{value, 1}; }
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST(SegmentTreeBasicTest, ConstructionFromSize) {
  SegmentTree<int> segment(5);
  EXPECT_EQ(segment.size(), 5);
  EXPECT_FALSE(segment.is_empty());
  EXPECT_EQ(segment.total_sum(), 0);
  EXPECT_EQ(segment.range_sum(0, 4), 0);
}

TEST_F(SegmentTreeTest, RangeSumQueries) {
  EXPECT_EQ(tree.range_sum(0, 4), 15);
  EXPECT_EQ(tree.range_sum(1, 3), 9);
  EXPECT_EQ(tree.range_sum(2, 2), 3);
}

TEST_F(SegmentTreeTest, ValueAtAccess) {
  EXPECT_EQ(tree.value_at(0), 1);
  EXPECT_EQ(tree.value_at(3), 4);
}

//===---------------------------- UPDATE OPERATIONS ----------------------------===//

TEST_F(SegmentTreeTest, AddUpdates) {
  tree.add(1, 5);
  EXPECT_EQ(tree.value_at(1), 7);
  EXPECT_EQ(tree.total_sum(), 20);
}

TEST_F(SegmentTreeTest, SetUpdates) {
  tree.set(2, 10);
  EXPECT_EQ(tree.value_at(2), 10);
  EXPECT_EQ(tree.range_sum(2, 2), 10);
  EXPECT_EQ(tree.total_sum(), 22);
}

//===--------------------------- RESET & CLEAR TESTS ---------------------------===//

TEST_F(SegmentTreeTest, ResetAndClear) {
  tree.clear();
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.total_sum(), 0);

  tree.reset(3);
  EXPECT_EQ(tree.size(), 3);
  EXPECT_EQ(tree.total_sum(), 0);
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(SegmentTreeTest, MoveSemantics) {
  SegmentTree<int> moved = std::move(tree);
  EXPECT_EQ(moved.size(), 5);
  EXPECT_TRUE(tree.is_empty());

  SegmentTree<int> assigned;
  assigned = std::move(moved);
  EXPECT_EQ(assigned.size(), 5);
  EXPECT_TRUE(moved.is_empty());
}

//===-------------------------- ERROR HANDLING TESTS ---------------------------===//

TEST(SegmentTreeErrorTest, OutOfRangeThrows) {
  SegmentTree<int> segment(3);
  EXPECT_THROW(segment.set(3, 1), SegmentTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.range_sum(2, 1); }, SegmentTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.range_sum(0, 3); }, SegmentTreeException);
}

//===--------------------------- FUNCTOR EXTENSION TESTS ------------------------===//

TEST(SegmentTreeFunctorTest, MaxAggregate) {
  SegmentTree<int, int, MaxCombine, MaxIdentity> segment({1, 5, 3, -2});
  EXPECT_EQ(segment.range_query(0, 3), 5);
  EXPECT_EQ(segment.range_query(2, 3), 3);

  segment.set(1, -4);
  EXPECT_EQ(segment.range_query(0, 3), 3);
}

TEST(SegmentTreeFunctorTest, CustomNodeAggregation) {
  SegmentTree<int, SumCount, SumCountCombine, SumCountIdentity, SumCountLeaf> segment({2, 4, 6, 8});
  SumCount                                                                    result = segment.range_query(1, 3);
  EXPECT_EQ(result.sum, 18);
  EXPECT_EQ(result.count, 3);
}

//===---------------------------------------------------------------------------===//
