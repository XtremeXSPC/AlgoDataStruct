//===---------------------------------------------------------------------------===//
/**
 * @file Test_Segment_Tree.cpp
 * @brief Google Test unit tests for SegmentTree and LazySegmentTree implementations.
 * @version 1.0
 * @date 2026-02-05
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/trees/Lazy_Segment_Tree.hpp"
#include "../include/ads/trees/Segment_Tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <vector>

using namespace ads::trees;

//===---------------------------------------------------------------------------===//
//=== ++++++++++++++++++++++++++ SEGMENT TREE TESTS +++++++++++++++++++++++++++ ===//
//===---------------------------------------------------------------------------===//

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

struct MinCombine {
  auto operator()(int left, int right) const -> int { return std::min(left, right); }
};

struct MinIdentity {
  auto operator()() const -> int { return std::numeric_limits<int>::max(); }
};

struct SumCount {
  int sum   = 0;
  int count = 0;

  auto operator==(const SumCount& other) const -> bool { return sum == other.sum && count == other.count; }
};

struct SumCountCombine {
  auto operator()(const SumCount& left, const SumCount& right) const -> SumCount {
    return SumCount{.sum = left.sum + right.sum, .count = left.count + right.count};
  }
};

struct SumCountIdentity {
  auto operator()() const -> SumCount { return SumCount{}; }
};

struct SumCountLeaf {
  auto operator()(int value) const -> SumCount { return SumCount{value, 1}; }
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST(SegmentTreeBasicTest, DefaultConstruction) {
  SegmentTree<int> segment;
  EXPECT_EQ(segment.size(), 0);
  EXPECT_TRUE(segment.is_empty());
  EXPECT_TRUE(segment.empty());
  EXPECT_EQ(segment.total(), 0);
  EXPECT_EQ(segment.total_sum(), 0);
}

TEST(SegmentTreeBasicTest, ConstructionFromSize) {
  SegmentTree<int> segment(5);
  EXPECT_EQ(segment.size(), 5);
  EXPECT_FALSE(segment.is_empty());
  EXPECT_EQ(segment.total(), 0);
  EXPECT_EQ(segment.range_sum(0, 4), 0);
}

TEST(SegmentTreeBasicTest, ConstructionFromSizeWithValue) {
  SegmentTree<int> segment(5, 3);
  EXPECT_EQ(segment.size(), 5);
  EXPECT_EQ(segment.total(), 15);
  EXPECT_EQ(segment.range_query(0, 4), 15);
}

TEST(SegmentTreeBasicTest, ConstructionWithSizeOne) {
  SegmentTree<int> segment(1);
  EXPECT_EQ(segment.size(), 1);
  EXPECT_EQ(segment.total(), 0);

  segment.set(0, 42);
  EXPECT_EQ(segment.total(), 42);
  EXPECT_EQ(segment.range_query(0, 0), 42);
  EXPECT_EQ(segment.value_at(0), 42);
}

TEST(SegmentTreeBasicTest, ConstructionPowerOfTwo) {
  SegmentTree<int> segment({1, 2, 3, 4, 5, 6, 7, 8}); // 8 elements
  EXPECT_EQ(segment.size(), 8);
  EXPECT_EQ(segment.total(), 36);
  EXPECT_EQ(segment.range_query(0, 3), 10);
  EXPECT_EQ(segment.range_query(4, 7), 26);
}

TEST(SegmentTreeBasicTest, ConstructionNonPowerOfTwo) {
  SegmentTree<int> segment({1, 2, 3, 4, 5, 6, 7}); // 7 elements
  EXPECT_EQ(segment.size(), 7);
  EXPECT_EQ(segment.total(), 28);
  EXPECT_EQ(segment.range_query(0, 2), 6);
  EXPECT_EQ(segment.range_query(3, 6), 22);
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

//===--------------------------- NODE_AT ACCESS TESTS ---------------------------===//

TEST_F(SegmentTreeTest, NodeAtAccess) {
  EXPECT_EQ(tree.node_at(0), 1);
  EXPECT_EQ(tree.node_at(2), 3);
  EXPECT_EQ(tree.node_at(4), 5);
}

TEST(SegmentTreeNodeAtTest, CustomNodeType) {
  SegmentTree<int, SumCount, SumCountCombine, SumCountIdentity, SumCountLeaf> segment({2, 4, 6, 8});

  SumCount node0 = segment.node_at(0);
  EXPECT_EQ(node0.sum, 2);
  EXPECT_EQ(node0.count, 1);

  SumCount node2 = segment.node_at(2);
  EXPECT_EQ(node2.sum, 6);
  EXPECT_EQ(node2.count, 1);
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

TEST_F(SegmentTreeTest, SetWithMoveSemantics) {
  std::string              value = "hello";
  SegmentTree<std::string> str_tree(3, "");
  str_tree.set(0, std::move(value));
  EXPECT_EQ(str_tree.value_at(0), "hello");
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

TEST(SegmentTreeMoveTest, MoveConstructorFromVector) {
  std::vector<int> values = {1, 2, 3, 4, 5};
  SegmentTree<int> tree(std::move(values));
  EXPECT_EQ(tree.size(), 5);
  EXPECT_EQ(tree.total(), 15);
  EXPECT_TRUE(values.empty()); // Values should be moved
}

TEST(SegmentTreeMoveTest, BuildFromMovedVector) {
  std::vector<int> values = {10, 20, 30};
  SegmentTree<int> tree;
  tree.build(std::move(values));
  EXPECT_EQ(tree.size(), 3);
  EXPECT_EQ(tree.total(), 60);
}

//===----------------------------- ITERATOR TESTS ------------------------------===//

TEST_F(SegmentTreeTest, IteratorBasic) {
  std::vector<int> expected = {1, 2, 3, 4, 5};
  std::vector<int> actual(tree.begin(), tree.end());
  EXPECT_EQ(actual, expected);
}

TEST_F(SegmentTreeTest, ConstIterators) {
  std::vector<int> actual(tree.cbegin(), tree.cend());
  EXPECT_EQ(actual.size(), 5);
  EXPECT_EQ(actual[0], 1);
  EXPECT_EQ(actual[4], 5);
}

TEST_F(SegmentTreeTest, ReverseIterators) {
  std::vector<int> expected = {5, 4, 3, 2, 1};
  std::vector<int> actual(tree.rbegin(), tree.rend());
  EXPECT_EQ(actual, expected);
}

TEST_F(SegmentTreeTest, RangeBasedFor) {
  int sum = 0;
  for (const auto& val : tree) {
    sum += val;
  }
  EXPECT_EQ(sum, 15);
}

TEST(SegmentTreeIteratorTest, EmptyTreeIterators) {
  SegmentTree<int> empty;
  EXPECT_EQ(empty.begin(), empty.end());
  EXPECT_EQ(empty.cbegin(), empty.cend());
  EXPECT_EQ(empty.rbegin(), empty.rend());
}

TEST(SegmentTreeIteratorTest, STLAlgorithms) {
  SegmentTree<int> tree({5, 3, 8, 1, 9});

  auto min_it = std::ranges::min_element(tree);
  EXPECT_EQ(*min_it, 1);

  auto max_it = std::ranges::max_element(tree);
  EXPECT_EQ(*max_it, 9);

  int stl_sum = std::accumulate(tree.begin(), tree.end(), 0);
  EXPECT_EQ(stl_sum, tree.total());
}

//===----------------------- ITERATOR CONSTRUCTION TESTS -----------------------===//

TEST(SegmentTreeConstructorTest, FromIteratorRange) {
  std::vector<int> source = {1, 2, 3, 4, 5};
  SegmentTree<int> tree(source.begin(), source.end());
  EXPECT_EQ(tree.size(), 5);
  EXPECT_EQ(tree.total(), 15);
}

TEST(SegmentTreeConstructorTest, FromArray) {
  std::array<int, 4> arr = {10, 20, 30, 40};
  SegmentTree<int>   tree(arr.begin(), arr.end());
  EXPECT_EQ(tree.size(), 4);
  EXPECT_EQ(tree.total(), 100);
}

TEST(SegmentTreeConstructorTest, BuildFromIteratorRange) {
  std::vector<int> source = {5, 10, 15};
  SegmentTree<int> tree;
  tree.build(source.begin(), source.end());
  EXPECT_EQ(tree.size(), 3);
  EXPECT_EQ(tree.total(), 30);
}

//===-------------------------- ERROR HANDLING TESTS ---------------------------===//

TEST(SegmentTreeErrorTest, OutOfRangeThrows) {
  SegmentTree<int> segment(3);
  EXPECT_THROW(segment.set(3, 1), SegmentTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.range_sum(2, 1); }, SegmentTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.range_sum(0, 3); }, SegmentTreeException);
}

TEST(SegmentTreeErrorTest, ValueAtOutOfRange) {
  SegmentTree<int> segment({1, 2, 3});
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.value_at(3); }, SegmentTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.value_at(100); }, SegmentTreeException);
}

TEST(SegmentTreeErrorTest, NodeAtOutOfRange) {
  SegmentTree<int> segment({1, 2, 3});
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.node_at(3); }, SegmentTreeException);
}

TEST(SegmentTreeErrorTest, AddOutOfRange) {
  SegmentTree<int> segment(3);
  EXPECT_THROW(segment.add(5, 10), SegmentTreeException);
}

//===------------------------- FUNCTOR EXTENSION TESTS -------------------------===//

TEST(SegmentTreeFunctorTest, MaxAggregate) {
  SegmentTree<int, int, MaxCombine, MaxIdentity> segment({1, 5, 3, -2});
  EXPECT_EQ(segment.range_query(0, 3), 5);
  EXPECT_EQ(segment.range_query(2, 3), 3);

  segment.set(1, -4);
  EXPECT_EQ(segment.range_query(0, 3), 3);
}

TEST(SegmentTreeFunctorTest, MinAggregate) {
  SegmentTree<int, int, MinCombine, MinIdentity> segment({5, 2, 8, 1, 9});
  EXPECT_EQ(segment.range_query(0, 4), 1);
  EXPECT_EQ(segment.range_query(0, 2), 2);
  EXPECT_EQ(segment.range_query(2, 4), 1);

  segment.set(3, 10);
  EXPECT_EQ(segment.range_query(0, 4), 2);
}

TEST(SegmentTreeFunctorTest, CustomNodeAggregation) {
  SegmentTree<int, SumCount, SumCountCombine, SumCountIdentity, SumCountLeaf> segment({2, 4, 6, 8});
  SumCount                                                                    result = segment.range_query(1, 3);
  EXPECT_EQ(result.sum, 18);
  EXPECT_EQ(result.count, 3);
}

TEST(SegmentTreeFunctorTest, FunctorAccessors) {
  SegmentTree<int, int, MaxCombine, MaxIdentity> segment({1, 2, 3});

  [[maybe_unused]] const auto& combine  = segment.get_combine();
  [[maybe_unused]] const auto& identity = segment.get_identity();
  [[maybe_unused]] const auto& builder  = segment.get_leaf_builder();

  // Just verify they're accessible without crashes.
  EXPECT_EQ(segment.range_query(0, 2), 3);
}

//===----------------------------- EDGE CASE TESTS -----------------------------===//

TEST(SegmentTreeEdgeCaseTest, SingleElementQueries) {
  SegmentTree<int> segment({42});
  EXPECT_EQ(segment.range_query(0, 0), 42);
  EXPECT_EQ(segment.value_at(0), 42);
  EXPECT_EQ(segment.total(), 42);

  segment.set(0, 100);
  EXPECT_EQ(segment.total(), 100);
}

TEST(SegmentTreeEdgeCaseTest, LargeTree) {
  std::vector<int> large(1'000);
  std::iota(large.begin(), large.end(), 1); // 1 to 1000
  SegmentTree<int> segment(large);

  EXPECT_EQ(segment.size(), 1'000);
  EXPECT_EQ(segment.total(), 500'500); // Sum 1 to 1000

  // Test various range queries
  EXPECT_EQ(segment.range_query(0, 9), 55);    // Sum 1 to 10
  EXPECT_EQ(segment.range_query(99, 99), 100); // Single element
}

TEST(SegmentTreeEdgeCaseTest, ConsecutiveUpdates) {
  SegmentTree<int> segment(5, 0);

  for (int i = 0; i < 5; ++i) {
    segment.set(i, i + 1);
  }

  EXPECT_EQ(segment.total(), 15);
  EXPECT_EQ(segment.range_query(0, 4), 15);
}

//===---------------------------------------------------------------------------===//
//=== ++++++++++++++++++++++++ LAZY SEGMENT TREE TESTS ++++++++++++++++++++++++ ===//
//===---------------------------------------------------------------------------===//

class LazySegmentTreeTest : public ::testing::Test {
protected:
  LazySegmentTree<int> tree{std::vector<int>{1, 2, 3, 4, 5}};
};

//===---------------------------- BASIC LAZY TESTS -----------------------------===//

TEST(LazySegmentTreeBasicTest, DefaultConstruction) {
  LazySegmentTree<int> segment;
  EXPECT_EQ(segment.size(), 0);
  EXPECT_TRUE(segment.is_empty());
  EXPECT_TRUE(segment.empty());
}

TEST(LazySegmentTreeBasicTest, ConstructionFromSize) {
  LazySegmentTree<int> segment(5);
  EXPECT_EQ(segment.size(), 5);
  EXPECT_FALSE(segment.is_empty());
  EXPECT_EQ(segment.total(), 0);
}

TEST(LazySegmentTreeBasicTest, ConstructionFromVector) {
  LazySegmentTree<int> segment({1, 2, 3, 4, 5});
  EXPECT_EQ(segment.size(), 5);
  EXPECT_EQ(segment.total(), 15);
  EXPECT_EQ(segment.range_query(0, 4), 15);
}

TEST_F(LazySegmentTreeTest, RangeQueries) {
  EXPECT_EQ(tree.range_query(0, 4), 15);
  EXPECT_EQ(tree.range_query(1, 3), 9);
  EXPECT_EQ(tree.range_query(2, 2), 3);
}

//===--------------------------- RANGE UPDATE TESTS ----------------------------===//

TEST_F(LazySegmentTreeTest, RangeUpdateSingleElement) {
  tree.range_update(2, 2, 10); // Add 10 to element at index 2
  EXPECT_EQ(tree.range_query(2, 2), 13);
  EXPECT_EQ(tree.total(), 25);
}

TEST_F(LazySegmentTreeTest, RangeUpdateFullRange) {
  tree.range_update(0, 4, 1);  // Add 1 to all elements
  EXPECT_EQ(tree.total(), 20); // 15 + 5*1 = 20
  EXPECT_EQ(tree.range_query(0, 4), 20);
}

TEST_F(LazySegmentTreeTest, RangeUpdatePartialRange) {
  tree.range_update(1, 3, 5); // Add 5 to elements 1,2,3
  // Original: [1, 2, 3, 4, 5] -> [1, 7, 8, 9, 5]
  EXPECT_EQ(tree.range_query(1, 3), 24); // 7 + 8 + 9 = 24
  EXPECT_EQ(tree.total(), 30);           // 1 + 7 + 8 + 9 + 5 = 30
}

TEST_F(LazySegmentTreeTest, MultipleRangeUpdates) {
  tree.range_update(0, 2, 10); // [11, 12, 13, 4, 5]
  tree.range_update(2, 4, 5);  // [11, 12, 18, 9, 10]

  EXPECT_EQ(tree.range_query(0, 0), 11);
  EXPECT_EQ(tree.range_query(2, 2), 18);
  EXPECT_EQ(tree.range_query(4, 4), 10);
  EXPECT_EQ(tree.total(), 60);
}

TEST(LazySegmentTreeRangeUpdateTest, OverlappingUpdates) {
  LazySegmentTree<int> segment({0, 0, 0, 0, 0});

  segment.range_update(0, 2, 1); // [1, 1, 1, 0, 0]
  segment.range_update(1, 3, 2); // [1, 3, 3, 2, 0]
  segment.range_update(2, 4, 3); // [1, 3, 6, 5, 3]

  EXPECT_EQ(segment.range_query(0, 0), 1);
  EXPECT_EQ(segment.range_query(1, 1), 3);
  EXPECT_EQ(segment.range_query(2, 2), 6);
  EXPECT_EQ(segment.range_query(3, 3), 5);
  EXPECT_EQ(segment.range_query(4, 4), 3);
  EXPECT_EQ(segment.total(), 18);
}

//===--------------------------- POINT ACCESS TESTS ----------------------------===//

TEST_F(LazySegmentTreeTest, ValueAtAfterRangeUpdate) {
  tree.range_update(1, 3, 10);

  EXPECT_EQ(tree.value_at(0), 1);
  EXPECT_EQ(tree.value_at(1), 12);
  EXPECT_EQ(tree.value_at(2), 13);
  EXPECT_EQ(tree.value_at(3), 14);
  EXPECT_EQ(tree.value_at(4), 5);
}

//===----------------------------- LAZY MOVE TESTS -----------------------------===//

TEST(LazySegmentTreeMoveTest, MoveConstructor) {
  LazySegmentTree<int> original({1, 2, 3, 4, 5});
  original.range_update(0, 4, 5);

  LazySegmentTree<int> moved = std::move(original);
  EXPECT_EQ(moved.size(), 5);
  EXPECT_EQ(moved.total(), 40);
  EXPECT_TRUE(original.is_empty());
}

TEST(LazySegmentTreeMoveTest, MoveAssignment) {
  LazySegmentTree<int> original({1, 2, 3});
  LazySegmentTree<int> target;

  target = std::move(original);
  EXPECT_EQ(target.size(), 3);
  EXPECT_EQ(target.total(), 6);
}

//===---------------------------- LAZY ERROR TESTS -----------------------------===//

TEST(LazySegmentTreeErrorTest, OutOfRangeThrows) {
  LazySegmentTree<int> segment(3);
  EXPECT_THROW(segment.range_update(0, 3, 1), SegmentTreeException);
  EXPECT_THROW(segment.range_update(2, 1, 1), SegmentTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.value_at(5); }, SegmentTreeException);
}

//===-------------------------- LAZY EDGE CASE TESTS ---------------------------===//

TEST(LazySegmentTreeEdgeCaseTest, SingleElement) {
  LazySegmentTree<int> segment({42});
  EXPECT_EQ(segment.total(), 42);

  segment.range_update(0, 0, 8);
  EXPECT_EQ(segment.total(), 50);
  EXPECT_EQ(segment.value_at(0), 50);
}

TEST(LazySegmentTreeEdgeCaseTest, LargeRangeUpdate) {
  std::vector<int>     zeros(100, 0);
  LazySegmentTree<int> segment(zeros);

  segment.range_update(0, 99, 1);
  EXPECT_EQ(segment.total(), 100);
  EXPECT_EQ(segment.range_query(50, 59), 10);
}

TEST(LazySegmentTreeEdgeCaseTest, ClearAndRebuild) {
  LazySegmentTree<int> segment({1, 2, 3});
  segment.range_update(0, 2, 10);

  segment.clear();
  EXPECT_TRUE(segment.is_empty());
  EXPECT_EQ(segment.total(), 0);

  segment.build({4, 5, 6});
  EXPECT_EQ(segment.size(), 3);
  EXPECT_EQ(segment.total(), 15);
}

//===---------------------------------------------------------------------------===//
