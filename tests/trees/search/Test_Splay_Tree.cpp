//===---------------------------------------------------------------------------===//
/**
 * @file Test_Splay_Tree.cpp
 * @brief Google Test unit tests for SplayTree.
 * @version 0.1
 * @date 2026-06-19
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/trees/Tree_Concepts.hpp"
#include "ads/trees/search/Splay_Tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace ads::trees;

namespace {

// Minimal move-only ordered element for move-only conformance checks.
struct MoveOnlyOrdered {
  int value = 0;

  MoveOnlyOrdered() = default;

  explicit MoveOnlyOrdered(int v) : value(v) {}

  MoveOnlyOrdered(MoveOnlyOrdered&&) noexcept                    = default;
  auto operator=(MoveOnlyOrdered&&) noexcept -> MoveOnlyOrdered& = default;
  MoveOnlyOrdered(const MoveOnlyOrdered&)                        = delete;
  auto operator=(const MoveOnlyOrdered&) -> MoveOnlyOrdered&     = delete;

  auto operator<(const MoveOnlyOrdered& other) const -> bool { return value < other.value; }

  auto operator==(const MoveOnlyOrdered& other) const -> bool { return value == other.value; }
};

} // namespace

//===----- CONCEPT CONFORMANCE -------------------------------------------------===//

static_assert(OrderedSearchTree<SplayTree<int>, int>);
static_assert(ValidatableOrderedSearchTree<SplayTree<int>, int>);
static_assert(CopyInsertableOrderedSearchTree<SplayTree<int>, int>);
static_assert(std::forward_iterator<SplayTree<int>::iterator>);

static_assert(OrderedSearchTree<SplayTree<MoveOnlyOrdered>, MoveOnlyOrdered>);
static_assert(!CopyInsertableOrderedSearchTree<SplayTree<MoveOnlyOrdered>, MoveOnlyOrdered>);

//===----- TEST FIXTURE --------------------------------------------------------===//

class SplayTreeTest : public ::testing::Test {
protected:
  SplayTree<int> tree;
};

//===----- BASIC STATE TESTS ---------------------------------------------------===//

TEST_F(SplayTreeTest, EmptyOnConstruction) {
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0u);
  EXPECT_EQ(tree.height(), -1);
  EXPECT_FALSE(tree.contains(0));
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(SplayTreeTest, InsertAndContains) {
  EXPECT_TRUE(tree.insert(10));
  EXPECT_TRUE(tree.insert(5));
  EXPECT_TRUE(tree.insert(15));
  EXPECT_EQ(tree.size(), 3u);
  EXPECT_TRUE(tree.contains(10));
  EXPECT_TRUE(tree.contains(5));
  EXPECT_TRUE(tree.contains(15));
  EXPECT_FALSE(tree.contains(7));
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(SplayTreeTest, DuplicateInsertReturnsFalse) {
  EXPECT_TRUE(tree.insert(42));
  EXPECT_FALSE(tree.insert(42));
  EXPECT_EQ(tree.size(), 1u);
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(SplayTreeTest, EmplaceConstructsInPlace) {
  SplayTree<std::string> strings;
  EXPECT_TRUE(strings.emplace(3, 'a')); // "aaa"
  EXPECT_TRUE(strings.contains(std::string("aaa")));
  EXPECT_EQ(strings.size(), 1u);
}

//===----- ORDER TESTS ---------------------------------------------------------===//

TEST_F(SplayTreeTest, InOrderIsSorted) {
  const std::vector<int> input = {50, 30, 70, 20, 40, 60, 80, 10};
  for (int v : input) {
    tree.insert(v);
  }

  std::vector<int> collected;
  tree.in_order_traversal([&collected](const int& v) -> void { collected.push_back(v); });

  std::vector<int> expected = input;
  std::ranges::sort(expected);
  EXPECT_EQ(collected, expected);
}

TEST_F(SplayTreeTest, IteratorYieldsSortedOrder) {
  for (int v : {5, 2, 8, 1, 9, 3, 7}) {
    tree.insert(v);
  }

  std::vector<int> collected(tree.begin(), tree.end());
  EXPECT_TRUE(std::ranges::is_sorted(collected));
  EXPECT_EQ(static_cast<size_t>(std::distance(tree.begin(), tree.end())), tree.size());
}

TEST_F(SplayTreeTest, PreOrderPostOrderLevelOrderVisitAllNodes) {
  for (int v : {50, 30, 70, 20, 40, 60, 80}) {
    tree.insert(v);
  }

  size_t pre   = 0;
  size_t post  = 0;
  size_t level = 0;
  tree.pre_order_traversal([&pre](const int&) -> void { ++pre; });
  tree.post_order_traversal([&post](const int&) -> void { ++post; });
  tree.level_order_traversal([&level](const int&) -> void { ++level; });
  EXPECT_EQ(pre, tree.size());
  EXPECT_EQ(post, tree.size());
  EXPECT_EQ(level, tree.size());
}

//===----- MIN / MAX TESTS -----------------------------------------------------===//

TEST_F(SplayTreeTest, FindMinMax) {
  for (int v : {50, 30, 70, 20, 40, 60, 80}) {
    tree.insert(v);
  }
  EXPECT_EQ(tree.find_min(), 20);
  EXPECT_EQ(tree.find_max(), 80);
  // Splaying the extremes must preserve the ordering invariant.
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.size(), 7u);
}

TEST_F(SplayTreeTest, FindMinMaxOnEmptyThrows) {
  EXPECT_THROW((void)tree.find_min(), EmptyTreeException);
  EXPECT_THROW((void)tree.find_max(), EmptyTreeException);
}

//===----- REMOVE TESTS --------------------------------------------------------===//

TEST_F(SplayTreeTest, RemoveLeafSingleChildAndTwoChildren) {
  for (int v : {50, 30, 70, 20, 40, 60, 80}) {
    tree.insert(v);
  }

  EXPECT_TRUE(tree.remove(20)); // leaf
  EXPECT_FALSE(tree.contains(20));
  EXPECT_TRUE(tree.validate_properties());

  EXPECT_TRUE(tree.remove(70)); // two children
  EXPECT_FALSE(tree.contains(70));
  EXPECT_TRUE(tree.validate_properties());

  EXPECT_TRUE(tree.remove(50)); // root
  EXPECT_FALSE(tree.contains(50));
  EXPECT_TRUE(tree.validate_properties());

  EXPECT_EQ(tree.size(), 4u);
}

TEST_F(SplayTreeTest, RemoveMissingReturnsFalse) {
  tree.insert(1);
  tree.insert(2);
  EXPECT_FALSE(tree.remove(99));
  EXPECT_EQ(tree.size(), 2u);
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(SplayTreeTest, RemoveDownToEmpty) {
  for (int v : {3, 1, 2}) {
    tree.insert(v);
  }
  EXPECT_TRUE(tree.remove(1));
  EXPECT_TRUE(tree.remove(2));
  EXPECT_TRUE(tree.remove(3));
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0u);
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(SplayTreeTest, Clear) {
  for (int v : {1, 2, 3, 4, 5}) {
    tree.insert(v);
  }
  tree.clear();
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0u);
  EXPECT_TRUE(tree.validate_properties());
}

//===----- MOVE SEMANTICS TESTS ------------------------------------------------===//

TEST_F(SplayTreeTest, MoveConstructionTransfersAndEmptiesSource) {
  for (int v : {1, 2, 3, 4, 5}) {
    tree.insert(v);
  }
  SplayTree<int> moved(std::move(tree));
  EXPECT_EQ(moved.size(), 5u);
  EXPECT_TRUE(moved.contains(3));
  EXPECT_TRUE(moved.validate_properties());
  EXPECT_TRUE(tree.is_empty());
}

TEST_F(SplayTreeTest, MoveAssignmentTransfers) {
  for (int v : {1, 2, 3}) {
    tree.insert(v);
  }
  SplayTree<int> other;
  other.insert(99);
  other = std::move(tree);
  EXPECT_EQ(other.size(), 3u);
  EXPECT_TRUE(other.contains(2));
  EXPECT_FALSE(other.contains(99));
  EXPECT_TRUE(other.validate_properties());
}

TEST_F(SplayTreeTest, IsMoveOnly) {
  static_assert(!std::is_copy_constructible_v<SplayTree<int>>);
  static_assert(!std::is_copy_assignable_v<SplayTree<int>>);
  static_assert(std::is_move_constructible_v<SplayTree<int>>);
  static_assert(std::is_move_assignable_v<SplayTree<int>>);
  SUCCEED();
}

TEST(SplayTreeMoveOnly, HoldsMoveOnlyValues) {
  SplayTree<MoveOnlyOrdered> tree;
  EXPECT_TRUE(tree.insert(MoveOnlyOrdered(5)));
  EXPECT_TRUE(tree.emplace(3));
  EXPECT_TRUE(tree.insert(MoveOnlyOrdered(7)));
  EXPECT_TRUE(tree.contains(MoveOnlyOrdered(3)));
  EXPECT_EQ(tree.find_min().value, 3);
  EXPECT_EQ(tree.find_max().value, 7);
  EXPECT_TRUE(tree.validate_properties());
}

//===----- STRESS TEST ---------------------------------------------------------===//

TEST_F(SplayTreeTest, AscendingInsertStressKeepsInvariants) {
  constexpr int kCount = 1'000;
  for (int i = 0; i < kCount; ++i) {
    EXPECT_TRUE(tree.insert(i));
  }
  EXPECT_EQ(tree.size(), static_cast<size_t>(kCount));
  EXPECT_TRUE(tree.validate_properties());

  // Splaying lookups across the whole range must keep ordering intact.
  for (int i = 0; i < kCount; ++i) {
    EXPECT_TRUE(tree.contains(i));
  }
  EXPECT_TRUE(tree.validate_properties());

  std::vector<int> collected(tree.begin(), tree.end());
  EXPECT_EQ(collected.size(), static_cast<size_t>(kCount));
  EXPECT_TRUE(std::ranges::is_sorted(collected));
}

TEST_F(SplayTreeTest, RandomizedInsertRemoveMatchesReference) {
  std::mt19937                       rng(20'260'619);
  std::uniform_int_distribution<int> dist(0, 199);
  std::vector<int>                   reference;

  for (int i = 0; i < 4'000; ++i) {
    int v = dist(rng);
    if (i % 3 == 0) {
      bool present = std::ranges::find(reference, v) != reference.end();
      EXPECT_EQ(tree.remove(v), present);
      reference.erase(std::ranges::remove(reference, v).begin(), reference.end());
    } else {
      bool present = std::ranges::find(reference, v) != reference.end();
      EXPECT_EQ(tree.insert(v), !present);
      if (!present) {
        reference.push_back(v);
      }
    }
  }

  EXPECT_EQ(tree.size(), reference.size());
  EXPECT_TRUE(tree.validate_properties());

  std::ranges::sort(reference);
  std::vector<int> collected(tree.begin(), tree.end());
  EXPECT_EQ(collected, reference);
}

//===---------------------------------------------------------------------------===//
