//===---------------------------------------------------------------------------===//
/**
 * @file Test_Segment_Tree.cpp
 * @brief Unit tests for the monoid-driven SegmentTree.
 * @version 1.1
 * @date 2026-07-16
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/algebra/Basic_Monoids.hpp"
#include "ads/range/Segment_Tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

using namespace ads::range;

//===---------------------------------------------------------------------------===//
//=== ++++++++++++++++++++++++++ SEGMENT TREE TESTS +++++++++++++++++++++++++++ ===//
//===---------------------------------------------------------------------------===//

class SegmentTreeTest : public ::testing::Test {
protected:
  SegmentTree<int> tree{std::vector<int>{1, 2, 3, 4, 5}};
};

struct SumCount {
  int sum   = 0;
  int count = 0;

  auto operator==(const SumCount& other) const -> bool { return sum == other.sum && count == other.count; }
};

struct SumCountMonoid {
  using value_type = SumCount;

  static constexpr bool is_commutative = true;

  [[nodiscard]] auto identity() const -> SumCount { return {}; }

  [[nodiscard]] auto combine(const SumCount& left, const SumCount& right) const -> SumCount {
    return SumCount{.sum = left.sum + right.sum, .count = left.count + right.count};
  }
};

struct SumCountLeaf {
  auto operator()(int value) const -> SumCount { return SumCount{.sum = value, .count = 1}; }
};

static_assert(ads::algebra::MonoidPolicy<SumCountMonoid>);

//===----- BASIC STATE TESTS ---------------------------------------------------===//

TEST(SegmentTreeBasicTest, DefaultConstruction) {
  SegmentTree<int> segment;
  EXPECT_EQ(segment.size(), 0);
  EXPECT_TRUE(segment.is_empty());
  EXPECT_TRUE(segment.is_empty());
  EXPECT_EQ(segment.total(), 0);
}

TEST(SegmentTreeBasicTest, ConstructionFromSize) {
  SegmentTree<int> segment(5);
  EXPECT_EQ(segment.size(), 5);
  EXPECT_FALSE(segment.is_empty());
  EXPECT_EQ(segment.total(), 0);
  EXPECT_EQ(segment.range_query(0, 4), 0);
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
  EXPECT_EQ(tree.range_query(0, 4), 15);
  EXPECT_EQ(tree.range_query(1, 3), 9);
  EXPECT_EQ(tree.range_query(2, 2), 3);
}

TEST_F(SegmentTreeTest, ValueAtAccess) {
  EXPECT_EQ(tree.value_at(0), 1);
  EXPECT_EQ(tree.value_at(3), 4);
}

//===----- NODE_AT ACCESS TESTS ------------------------------------------------===//

TEST_F(SegmentTreeTest, NodeAtAccess) {
  EXPECT_EQ(tree.node_at(0), 1);
  EXPECT_EQ(tree.node_at(2), 3);
  EXPECT_EQ(tree.node_at(4), 5);
}

TEST(SegmentTreeNodeAtTest, CustomNodeType) {
  SegmentTree<int, SumCountMonoid, SumCountLeaf> segment({2, 4, 6, 8});

  SumCount node0 = segment.node_at(0);
  EXPECT_EQ(node0.sum, 2);
  EXPECT_EQ(node0.count, 1);

  SumCount node2 = segment.node_at(2);
  EXPECT_EQ(node2.sum, 6);
  EXPECT_EQ(node2.count, 1);
}

//===----- UPDATE OPERATIONS ---------------------------------------------------===//

TEST_F(SegmentTreeTest, AddUpdates) {
  tree.add(1, 5);
  EXPECT_EQ(tree.value_at(1), 7);
  EXPECT_EQ(tree.total(), 20);
}

TEST_F(SegmentTreeTest, SetUpdates) {
  tree.set(2, 10);
  EXPECT_EQ(tree.value_at(2), 10);
  EXPECT_EQ(tree.range_query(2, 2), 10);
  EXPECT_EQ(tree.total(), 22);
}

TEST_F(SegmentTreeTest, SetWithMoveSemantics) {
  std::string              value = "hello";
  SegmentTree<std::string> str_tree(3, "");
  str_tree.set(0, std::move(value));
  EXPECT_EQ(str_tree.value_at(0), "hello");
}

//===----- RESET & CLEAR TESTS -------------------------------------------------===//

TEST_F(SegmentTreeTest, ResetAndClear) {
  tree.clear();
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.total(), 0);

  tree.reset(3);
  EXPECT_EQ(tree.size(), 3);
  EXPECT_EQ(tree.total(), 0);
}

//===----- MOVE SEMANTICS TESTS ------------------------------------------------===//

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
  EXPECT_EQ(tree.value_at(0), 1);
  EXPECT_EQ(tree.value_at(4), 5);
}

TEST(SegmentTreeMoveTest, BuildFromMovedVector) {
  std::vector<int> values = {10, 20, 30};
  SegmentTree<int> tree;
  tree.build(std::move(values));
  EXPECT_EQ(tree.size(), 3);
  EXPECT_EQ(tree.total(), 60);
}

//===----- ITERATOR TESTS ------------------------------------------------------===//

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

//===----- ITERATOR CONSTRUCTION TESTS -----------------------------------------===//

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

//===----- ERROR HANDLING TESTS ------------------------------------------------===//

TEST(SegmentTreeErrorTest, OutOfRangeThrows) {
  SegmentTree<int> segment(3);
  EXPECT_THROW(segment.set(3, 1), RangeIndexException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.range_query(2, 1); }, InvalidRangeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.range_query(0, 3); }, InvalidRangeException);
}

TEST(SegmentTreeErrorTest, ValueAtOutOfRange) {
  SegmentTree<int> segment({1, 2, 3});
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.value_at(3); }, RangeIndexException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.value_at(100); }, RangeIndexException);
}

TEST(SegmentTreeErrorTest, NodeAtOutOfRange) {
  SegmentTree<int> segment({1, 2, 3});
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.node_at(3); }, RangeIndexException);
}

TEST(SegmentTreeErrorTest, AddOutOfRange) {
  SegmentTree<int> segment(3);
  EXPECT_THROW(segment.add(5, 10), RangeIndexException);
}

//===----- POLICY EXTENSION TESTS ---------------------------------------------===//

TEST(SegmentTreePolicyTest, MaxAggregate) {
  SegmentTree<int, ads::algebra::MaxMonoid<int>> segment({1, 5, 3, -2});
  EXPECT_EQ(segment.range_query(0, 3), 5);
  EXPECT_EQ(segment.range_query(2, 3), 3);

  segment.set(1, -4);
  EXPECT_EQ(segment.range_query(0, 3), 3);
}

TEST(SegmentTreePolicyTest, MinAggregate) {
  SegmentTree<int, ads::algebra::MinMonoid<int>> segment({5, 2, 8, 1, 9});
  EXPECT_EQ(segment.range_query(0, 4), 1);
  EXPECT_EQ(segment.range_query(0, 2), 2);
  EXPECT_EQ(segment.range_query(2, 4), 1);

  segment.set(3, 10);
  EXPECT_EQ(segment.range_query(0, 4), 2);
}

TEST(SegmentTreePolicyTest, CustomNodeAggregation) {
  SegmentTree<int, SumCountMonoid, SumCountLeaf> segment({2, 4, 6, 8});
  SumCount                                       result = segment.range_query(1, 3);
  EXPECT_EQ(result.sum, 18);
  EXPECT_EQ(result.count, 3);
}

TEST(SegmentTreePolicyTest, PolicyAccessors) {
  SegmentTree<int, ads::algebra::MaxMonoid<int>> segment({1, 2, 3});

  [[maybe_unused]] const auto& monoid  = segment.get_monoid();
  [[maybe_unused]] const auto& builder = segment.get_leaf_builder();

  // Just verify they're accessible without crashes.
  EXPECT_EQ(segment.range_query(0, 2), 3);
}

//===----- EDGE CASE TESTS -----------------------------------------------------===//

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
