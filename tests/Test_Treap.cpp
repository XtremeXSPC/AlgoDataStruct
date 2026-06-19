//===---------------------------------------------------------------------------===//
/**
 * @file Test_Treap.cpp
 * @brief Google Test unit tests for Treap.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/trees/search/Treap.hpp"

#include <gtest/gtest.h>

#include <random>
#include <set>
#include <string>
#include <vector>

using namespace ads::trees;

namespace {

//===----------------------------- TEST UTILITIES ------------------------------===//

struct TreapMoveOnlyOrdered {
  int value;

  explicit TreapMoveOnlyOrdered(int v) : value(v) {}

  TreapMoveOnlyOrdered(const TreapMoveOnlyOrdered&)                        = delete;
  auto operator=(const TreapMoveOnlyOrdered&) -> TreapMoveOnlyOrdered&     = delete;
  TreapMoveOnlyOrdered(TreapMoveOnlyOrdered&&) noexcept                    = default;
  auto operator=(TreapMoveOnlyOrdered&&) noexcept -> TreapMoveOnlyOrdered& = default;

  auto operator<(const TreapMoveOnlyOrdered& other) const -> bool { return value < other.value; }

  auto operator==(const TreapMoveOnlyOrdered& other) const -> bool { return value == other.value; }
};

auto collect_in_order(const Treap<int>& tree) -> std::vector<int> {
  std::vector<int> values;
  tree.in_order_traversal([&](const int& value) { values.push_back(value); });
  return values;
}

auto collect_pre_order(const Treap<int>& tree) -> std::vector<int> {
  std::vector<int> values;
  tree.pre_order_traversal([&](const int& value) { values.push_back(value); });
  return values;
}

auto expect_matches_set(const Treap<int>& tree, const std::set<int>& oracle) -> void {
  EXPECT_EQ(tree.size(), oracle.size());
  EXPECT_EQ(tree.is_empty(), oracle.empty());
  EXPECT_TRUE(tree.validate_properties());

  const std::vector<int> expected(oracle.begin(), oracle.end());
  EXPECT_EQ(collect_in_order(tree), expected);
}

} // namespace

// Test fixture for deterministic Treap scenarios.
class TreapTest : public ::testing::Test {
protected:
  Treap<int> tree;

  auto build_reference_tree() -> void {
    ASSERT_TRUE(tree.insert_with_priority(50, 80));
    ASSERT_TRUE(tree.insert_with_priority(30, 60));
    ASSERT_TRUE(tree.insert_with_priority(70, 55));
    ASSERT_TRUE(tree.insert_with_priority(20, 40));
    ASSERT_TRUE(tree.insert_with_priority(40, 45));
    ASSERT_TRUE(tree.insert_with_priority(60, 35));
    ASSERT_TRUE(tree.insert_with_priority(80, 30));
  }
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(TreapTest, IsEmptyOnConstruction) {
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0U);
  EXPECT_EQ(tree.height(), -1);
}

TEST_F(TreapTest, InsertSingleElement) {
  EXPECT_TRUE(tree.insert_with_priority(42, 99));
  EXPECT_EQ(tree.size(), 1U);
  EXPECT_TRUE(tree.contains(42));
  EXPECT_EQ(tree.find_min(), 42);
  EXPECT_EQ(tree.find_max(), 42);
  ASSERT_NE(tree.priority_of(42), nullptr);
  EXPECT_EQ(*tree.priority_of(42), 99);
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(TreapTest, DuplicateInsertionsAreRejected) {
  EXPECT_TRUE(tree.insert_with_priority(42, 10));
  EXPECT_FALSE(tree.insert_with_priority(42, 999));
  EXPECT_EQ(tree.size(), 1U);
  ASSERT_NE(tree.priority_of(42), nullptr);
  EXPECT_EQ(*tree.priority_of(42), 10);
}

//===----------------------- STRUCTURE AND TRAVERSAL TESTS ---------------------===//

TEST_F(TreapTest, ExplicitPrioritiesProduceDeterministicShape) {
  ASSERT_TRUE(tree.insert_with_priority(30, 10));
  ASSERT_TRUE(tree.insert_with_priority(10, 40));
  ASSERT_TRUE(tree.insert_with_priority(20, 50));

  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(collect_in_order(tree), (std::vector<int>{10, 20, 30}));
  EXPECT_EQ(collect_pre_order(tree), (std::vector<int>{20, 10, 30}));
  ASSERT_NE(tree.priority_of(20), nullptr);
  EXPECT_EQ(*tree.priority_of(20), 50);
}

TEST_F(TreapTest, InsertAndRemoveReferenceTree) {
  build_reference_tree();

  EXPECT_EQ(tree.size(), 7U);
  EXPECT_EQ(tree.height(), 2);
  EXPECT_EQ(collect_in_order(tree), (std::vector<int>{20, 30, 40, 50, 60, 70, 80}));

  EXPECT_TRUE(tree.remove(50));
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_FALSE(tree.contains(50));
  EXPECT_EQ(tree.size(), 6U);
  EXPECT_EQ(collect_in_order(tree), (std::vector<int>{20, 30, 40, 60, 70, 80}));
}

TEST_F(TreapTest, RemoveLeafAndMissingValueBehaveCorrectly) {
  build_reference_tree();

  EXPECT_TRUE(tree.remove(20));
  EXPECT_FALSE(tree.remove(999));
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(collect_in_order(tree), (std::vector<int>{30, 40, 50, 60, 70, 80}));
}

TEST_F(TreapTest, TraversalsRemainConsistent) {
  build_reference_tree();

  std::vector<int> level_order;
  tree.level_order_traversal([&](const int& value) { level_order.push_back(value); });

  EXPECT_EQ(collect_in_order(tree), (std::vector<int>{20, 30, 40, 50, 60, 70, 80}));
  EXPECT_EQ(collect_pre_order(tree), (std::vector<int>{50, 30, 20, 40, 70, 60, 80}));
  EXPECT_EQ(level_order, (std::vector<int>{50, 30, 70, 20, 40, 60, 80}));
}

TEST_F(TreapTest, SuccessorAndPredecessorFollowBstOrder) {
  build_reference_tree();

  const int* successor = tree.successor(40);
  ASSERT_NE(successor, nullptr);
  EXPECT_EQ(*successor, 50);

  const int* predecessor = tree.predecessor(40);
  ASSERT_NE(predecessor, nullptr);
  EXPECT_EQ(*predecessor, 30);

  EXPECT_EQ(tree.successor(80), nullptr);
  EXPECT_EQ(tree.predecessor(20), nullptr);
}

TEST_F(TreapTest, IteratorTraversalIsSorted) {
  build_reference_tree();

  std::vector<int> values;
  for (const int value : tree) {
    values.push_back(value);
  }

  EXPECT_EQ(values, (std::vector<int>{20, 30, 40, 50, 60, 70, 80}));
}

//===------------------------ RANDOMIZED AND MODEL TESTS -----------------------===//

TEST_F(TreapTest, InternalPriorityGeneratorStillMaintainsInvariants) {
  for (int value = 1; value <= 200; ++value) {
    ASSERT_TRUE(tree.insert(value));
    ASSERT_TRUE(tree.validate_properties()) << "after inserting " << value;
  }

  EXPECT_EQ(tree.size(), 200U);
  EXPECT_LE(tree.height(), 40);
}

TEST_F(TreapTest, RandomizedOperationsMatchStdSet) {
  std::mt19937                    rng(12'345);
  std::uniform_int_distribution<> value_dist(0, 200);
  std::bernoulli_distribution     insert_op(0.6);
  std::set<int>                   oracle;

  for (int step = 0; step < 1'000; ++step) {
    const int value = value_dist(rng);

    if (insert_op(rng)) {
      const bool expected = oracle.insert(value).second;
      EXPECT_EQ(tree.insert(value), expected);
    } else {
      const bool expected = oracle.erase(value) != 0;
      EXPECT_EQ(tree.remove(value), expected);
    }

    ASSERT_TRUE(tree.validate_properties()) << "after randomized step " << step;
    expect_matches_set(tree, oracle);
  }
}

//===-------------------------- MOVE AND UTILITY TESTS -------------------------===//

TEST_F(TreapTest, MoveSemanticsTransferContents) {
  build_reference_tree();

  Treap<int> moved_tree = std::move(tree);
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(moved_tree.size(), 7U);
  EXPECT_TRUE(moved_tree.validate_properties());

  Treap<int> assigned_tree;
  assigned_tree = std::move(moved_tree);
  EXPECT_TRUE(moved_tree.is_empty());
  EXPECT_EQ(assigned_tree.size(), 7U);
  EXPECT_TRUE(assigned_tree.contains(70));
}

TEST_F(TreapTest, ClearResetsTree) {
  build_reference_tree();
  tree.clear();

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0U);
  EXPECT_EQ(tree.height(), -1);
  EXPECT_TRUE(tree.validate_properties());
}

//===-------------------------- VALUE-TYPE SUPPORT TESTS -----------------------===//

TEST(TreapValueTest, SupportsMoveOnlyValuesWithExplicitPriority) {
  Treap<TreapMoveOnlyOrdered> tree;

  EXPECT_TRUE(tree.insert_with_priority(TreapMoveOnlyOrdered(20), 10));
  EXPECT_TRUE(tree.emplace_with_priority(30, 10));
  EXPECT_TRUE(tree.emplace_with_priority(40, 30));

  EXPECT_TRUE(tree.contains(TreapMoveOnlyOrdered(10)));
  EXPECT_TRUE(tree.contains(TreapMoveOnlyOrdered(20)));
  EXPECT_TRUE(tree.contains(TreapMoveOnlyOrdered(30)));
  EXPECT_TRUE(tree.validate_properties());
}

//===----------------------------- CONCEPT TESTS -------------------------------===//

TEST(TreapApiTest, SatisfiesOrderedSearchTreeConcept) {
  static_assert(OrderedSearchTree<Treap<int>, int>);
  static_assert(CopyInsertableOrderedSearchTree<Treap<int>, int>);
  static_assert(ValidatableOrderedSearchTree<Treap<int>, int>);
}

//===---------------------------------------------------------------------------===//
