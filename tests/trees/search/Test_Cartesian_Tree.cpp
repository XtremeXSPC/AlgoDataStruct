//===---------------------------------------------------------------------------===//
/**
 * @file Test_Cartesian_Tree.cpp
 * @brief Google Test unit tests for CartesianTree.
 * @version 0.1
 * @date 2026-07-13
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/trees/search/Cartesian_Tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace ads::trees;

namespace {

//===----- TEST UTILITIES ------------------------------------------------------===//

struct CartesianMoveOnly {
  int value;

  explicit CartesianMoveOnly(int v) : value(v) {}

  CartesianMoveOnly(const CartesianMoveOnly&)                        = delete;
  auto operator=(const CartesianMoveOnly&) -> CartesianMoveOnly&     = delete;
  CartesianMoveOnly(CartesianMoveOnly&&) noexcept                    = default;
  auto operator=(CartesianMoveOnly&&) noexcept -> CartesianMoveOnly& = default;

  auto operator<(const CartesianMoveOnly& other) const -> bool { return value < other.value; }

  auto operator==(const CartesianMoveOnly& other) const -> bool { return value == other.value; }
};

struct CartesianThrowState {
  std::size_t calls    = 0;
  std::size_t throw_on = std::numeric_limits<std::size_t>::max();
};

struct CartesianThrowingLess {
  std::shared_ptr<CartesianThrowState> state;

  auto operator()(int lhs, int rhs) const -> bool {
    ++state->calls;
    if (state->calls == state->throw_on) {
      throw std::runtime_error("Cartesian comparator failure");
    }
    return lhs < rhs;
  }
};

struct CartesianMoveState {
  bool throw_on_move = false;
};

struct CartesianThrowingMoveCompare {
  std::shared_ptr<CartesianMoveState> state;

  explicit CartesianThrowingMoveCompare(std::shared_ptr<CartesianMoveState> move_state) : state(std::move(move_state)) {}

  CartesianThrowingMoveCompare(const CartesianThrowingMoveCompare&)                    = default;
  auto operator=(const CartesianThrowingMoveCompare&) -> CartesianThrowingMoveCompare& = default;

  CartesianThrowingMoveCompare(CartesianThrowingMoveCompare&& other) : state(other.state) {
    if (state->throw_on_move) {
      throw std::runtime_error("Cartesian comparator move failure");
    }
  }

  auto operator=(CartesianThrowingMoveCompare&& other) noexcept(false) -> CartesianThrowingMoveCompare& {
    state = std::move(other.state);
    throw std::runtime_error("Cartesian comparator assignment failure");
  }

  auto operator()(int lhs, int rhs) const -> bool { return lhs < rhs; }
};

using CartesianThrowingTree = CartesianTree<int, CartesianThrowingLess>;
using CartesianMoveTree     = CartesianTree<int, CartesianThrowingMoveCompare>;

static_assert(std::is_move_assignable_v<CartesianTree<int>>);
static_assert(!std::is_move_assignable_v<CartesianMoveTree>);
static_assert(!noexcept(std::declval<const CartesianTree<int>&>().height()));

template <typename Tree>
auto ct_collect_in_order(const Tree& tree) -> std::vector<int> {
  std::vector<int> values;
  tree.in_order_traversal([&](const int& value) -> void { values.push_back(value); });
  return values;
}

auto make_dynamic(const std::vector<int>& values) -> ads::arrays::DynamicArray<int> {
  ads::arrays::DynamicArray<int> array;
  array.reserve(values.size());
  for (int value : values) {
    array.push_back(value);
  }
  return array;
}

auto build_from(const std::vector<int>& values) -> CartesianTree<int> {
  return CartesianTree<int>(values.begin(), values.end());
}

} // namespace

//===----- BASIC PROPERTIES ----------------------------------------------------===//

TEST(CartesianTreeBasic, EmptyTree) {
  CartesianTree<int> tree;
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0U);
  EXPECT_EQ(tree.height(), -1);
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.begin(), tree.end());
}

TEST(CartesianTreeBasic, SingleElement) {
  CartesianTree<int> tree;
  tree.push_back(42);
  EXPECT_FALSE(tree.is_empty());
  EXPECT_EQ(tree.size(), 1U);
  EXPECT_EQ(tree.height(), 0);
  EXPECT_EQ(tree.extremum(), 42);
  EXPECT_EQ(tree.value_at(0), 42);
  EXPECT_EQ(tree.range_extremum(0, 0), 42);
  EXPECT_EQ(tree.range_extremum_index(0, 0), 0U);
  EXPECT_TRUE(tree.validate_properties());
}

TEST(CartesianTreeBasic, ClearResets) {
  CartesianTree<int> tree = build_from({5, 3, 8, 1, 9});
  ASSERT_FALSE(tree.is_empty());
  tree.clear();
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0U);
  EXPECT_TRUE(tree.validate_properties());
  // Reusable after clear.
  tree.push_back(7);
  EXPECT_EQ(tree.extremum(), 7);
  EXPECT_TRUE(tree.validate_properties());
}

//===----- CONSTRUCTION & INVARIANTS -------------------------------------------===//

TEST(CartesianTreeBuild, InOrderReproducesSequence) {
  const std::vector<int> values = {9, 3, 7, 1, 8, 12, 10, 20, 15, 18, 5};
  CartesianTree<int>     tree   = build_from(values);

  EXPECT_EQ(tree.size(), values.size());
  EXPECT_EQ(ct_collect_in_order(tree), values);
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.extremum(), *std::min_element(values.begin(), values.end()));
}

TEST(CartesianTreeBuild, DynamicArrayConstructorMatchesIteratorConstructor) {
  const std::vector<int> values = {4, 2, 6, 1, 3, 5, 7};
  CartesianTree<int>     from_it(values.begin(), values.end());
  CartesianTree<int>     from_array(make_dynamic(values));

  EXPECT_EQ(ct_collect_in_order(from_it), ct_collect_in_order(from_array));
  EXPECT_TRUE(from_array.validate_properties());
}

TEST(CartesianTreeBuild, IncrementalPushBackMatchesBatch) {
  const std::vector<int> values = {6, 2, 9, 4, 1, 8, 3, 7, 5};
  CartesianTree<int>     batch  = build_from(values);

  CartesianTree<int> incremental;
  for (int value : values) {
    incremental.push_back(value);
  }
  EXPECT_EQ(ct_collect_in_order(incremental), ct_collect_in_order(batch));
  EXPECT_EQ(incremental.height(), batch.height());
  EXPECT_TRUE(incremental.validate_properties());
}

TEST(CartesianTreeBuild, ComparatorFailurePreservesAppendState) {
  auto                  state = std::make_shared<CartesianThrowState>();
  CartesianThrowingTree tree(CartesianThrowingLess{state});
  tree.push_back(1);
  tree.push_back(2);
  tree.push_back(3);

  state->throw_on = state->calls + 2;
  EXPECT_THROW(tree.push_back(0), std::runtime_error);
  EXPECT_EQ(tree.size(), 3U);
  EXPECT_TRUE(tree.validate_properties());

  state->throw_on = std::numeric_limits<std::size_t>::max();
  tree.push_back(4);
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(ct_collect_in_order(tree), (std::vector<int>{1, 2, 3, 4}));
  EXPECT_EQ(tree.range_extremum(2, 3), 3);
}

TEST(CartesianTreeBuild, FailedLargeConstructionUnwindsIteratively) {
  constexpr std::size_t kNodeCount = 300'000;
  std::vector<int>      values(kNodeCount);
  std::iota(values.begin(), values.end(), 0);

  auto state        = std::make_shared<CartesianThrowState>();
  state->throw_on   = kNodeCount - 1;
  auto construction = [&]() -> void { CartesianThrowingTree tree(values.begin(), values.end(), CartesianThrowingLess{state}); };

  EXPECT_THROW(construction(), std::runtime_error);
}

TEST(CartesianTreeBuild, DuplicatesArePreserved) {
  const std::vector<int> values = {5, 1, 5, 1, 5, 3, 3, 1};
  CartesianTree<int>     tree   = build_from(values);

  EXPECT_EQ(ct_collect_in_order(tree), values);
  EXPECT_EQ(tree.size(), values.size());
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.extremum(), 1);
}

TEST(CartesianTreeBuild, ValueAtCoversEveryPosition) {
  const std::vector<int> values = {8, 4, 6, 2, 7, 1, 9, 3, 5};
  CartesianTree<int>     tree   = build_from(values);
  for (std::size_t i = 0; i < values.size(); ++i) {
    EXPECT_EQ(tree.value_at(i), values[i]) << "at index " << i;
  }
}

//===----- RANGE-EXTREMUM (RMQ) ORACLE -----------------------------------------===//

TEST(CartesianTreeRMQ, MatchesBruteForceMinOverAllRanges) {
  std::mt19937                       rng(20'260'713U);
  std::uniform_int_distribution<int> dist(-50, 50);

  for (int trial = 0; trial < 20; ++trial) {
    const std::size_t n = 1U + static_cast<std::size_t>(trial) * 7U;
    std::vector<int>  values(n);
    for (int& value : values) {
      value = dist(rng);
    }
    CartesianTree<int> tree = build_from(values);
    ASSERT_TRUE(tree.validate_properties());

    for (std::size_t l = 0; l < n; ++l) {
      int expected = values[l];
      for (std::size_t r = l; r < n; ++r) {
        expected = std::min(expected, values[r]);
        EXPECT_EQ(tree.range_extremum(l, r), expected) << "range [" << l << ", " << r << "]";

        const std::size_t idx = tree.range_extremum_index(l, r);
        EXPECT_GE(idx, l);
        EXPECT_LE(idx, r);
        EXPECT_EQ(values[idx], expected);
      }
    }
  }
}

TEST(CartesianTreeMaxTree, GreaterComparatorGivesRangeMax) {
  const std::vector<int>                values = {3, 1, 4, 1, 5, 9, 2, 6};
  CartesianTree<int, std::greater<int>> tree(values.begin(), values.end());

  EXPECT_EQ(ct_collect_in_order(tree), values);
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.extremum(), 9);

  for (std::size_t l = 0; l < values.size(); ++l) {
    int expected = values[l];
    for (std::size_t r = l; r < values.size(); ++r) {
      expected = std::max(expected, values[r]);
      EXPECT_EQ(tree.range_extremum(l, r), expected) << "range [" << l << ", " << r << "]";
    }
  }
}

//===----- DEGENERATE (LINEAR) SHAPES ------------------------------------------===//

TEST(CartesianTreeDegenerate, IncreasingInputIsRightChain) {
  const std::vector<int> values = {1, 2, 3, 4, 5, 6, 7, 8};
  CartesianTree<int>     tree   = build_from(values);
  EXPECT_EQ(tree.height(), static_cast<int>(values.size()) - 1);
  EXPECT_EQ(tree.extremum(), 1);
  EXPECT_TRUE(tree.validate_properties());
}

TEST(CartesianTreeDegenerate, DecreasingInputIsLeftChain) {
  const std::vector<int> values = {8, 7, 6, 5, 4, 3, 2, 1};
  CartesianTree<int>     tree   = build_from(values);
  EXPECT_EQ(tree.height(), static_cast<int>(values.size()) - 1);
  EXPECT_EQ(tree.extremum(), 1);
  EXPECT_TRUE(tree.validate_properties());
}

TEST(CartesianTreeDegenerate, LargeMonotoneInputDoesNotOverflow) {
  const int        n = 100'000;
  std::vector<int> increasing(n);
  std::vector<int> decreasing(n);
  for (int i = 0; i < n; ++i) {
    increasing[i] = i;
    decreasing[i] = n - i;
  }

  CartesianTree<int> right_chain = build_from(increasing);
  CartesianTree<int> left_chain  = build_from(decreasing);

  // Build, validate (iterative), traverse (iterative), and destroy (iterative)
  // must all survive a chain of length n without recursing to depth n.
  EXPECT_EQ(right_chain.height(), n - 1);
  EXPECT_EQ(left_chain.height(), n - 1);
  EXPECT_TRUE(right_chain.validate_properties());
  EXPECT_TRUE(left_chain.validate_properties());
  EXPECT_EQ(right_chain.range_extremum(10, n - 1), 10);
  EXPECT_EQ(left_chain.range_extremum(0, n - 10), 10);

  std::size_t counted = 0;
  right_chain.in_order_traversal([&](const int&) -> void { ++counted; });
  EXPECT_EQ(counted, static_cast<std::size_t>(n));
}

//===----- ERROR HANDLING ------------------------------------------------------===//

TEST(CartesianTreeErrors, ExtremumOnEmptyThrows) {
  CartesianTree<int> tree;
  EXPECT_THROW((void)tree.extremum(), EmptyTreeException);
}

TEST(CartesianTreeErrors, OutOfRangeAccessThrows) {
  CartesianTree<int> tree = build_from({4, 2, 6, 1});
  EXPECT_THROW((void)tree.value_at(4), BinaryTreeException);
  EXPECT_THROW((void)tree.range_extremum(0, 4), BinaryTreeException);
  EXPECT_THROW((void)tree.range_extremum_index(2, 1), BinaryTreeException);
  EXPECT_THROW((void)tree.range_extremum(1, 0), BinaryTreeException);
}

TEST(CartesianTreeErrors, RangeQueryOnEmptyThrows) {
  CartesianTree<int> tree;
  EXPECT_THROW((void)tree.range_extremum(0, 0), BinaryTreeException);
  EXPECT_THROW((void)tree.value_at(0), BinaryTreeException);
}

//===----- TRAVERSALS ----------------------------------------------------------===//

TEST(CartesianTreeTraversal, KnownShapeOrders) {
  // Sequence {3, 1, 2} builds a min-tree with root 1, left child 3, right child 2.
  CartesianTree<int> tree = build_from({3, 1, 2});

  std::vector<int> pre;
  tree.pre_order_traversal([&](const int& v) -> void { pre.push_back(v); });
  EXPECT_EQ(pre, (std::vector<int>{1, 3, 2}));

  std::vector<int> post;
  tree.post_order_traversal([&](const int& v) -> void { post.push_back(v); });
  EXPECT_EQ(post, (std::vector<int>{3, 2, 1}));

  std::vector<int> level;
  tree.level_order_traversal([&](const int& v) -> void { level.push_back(v); });
  EXPECT_EQ(level, (std::vector<int>{1, 3, 2}));

  std::vector<int> in;
  tree.in_order_traversal([&](const int& v) -> void { in.push_back(v); });
  EXPECT_EQ(in, (std::vector<int>{3, 1, 2}));
}

//===----- ITERATOR ------------------------------------------------------------===//

TEST(CartesianTreeIterator, RangeForYieldsSequence) {
  const std::vector<int> values = {7, 2, 9, 4, 1, 8};
  CartesianTree<int>     tree   = build_from(values);

  std::vector<int> seen;
  for (const int& value : tree) {
    seen.push_back(value);
  }
  EXPECT_EQ(seen, values);
  EXPECT_EQ(static_cast<std::size_t>(std::distance(tree.begin(), tree.end())), values.size());
}

//===----- MOVE SEMANTICS ------------------------------------------------------===//

TEST(CartesianTreeMove, MoveConstructorTransfersOwnership) {
  const std::vector<int> values = {5, 1, 4, 2, 8};
  CartesianTree<int>     source = build_from(values);
  CartesianTree<int>     moved(std::move(source));

  EXPECT_EQ(ct_collect_in_order(moved), values);
  EXPECT_TRUE(moved.validate_properties());
  EXPECT_TRUE(source.is_empty()); // NOLINT(bugprone-use-after-move): checking moved-from state
  EXPECT_EQ(source.size(), 0U);
}

TEST(CartesianTreeMove, MoveAssignmentReplacesTarget) {
  CartesianTree<int> target = build_from({100, 200, 300});
  CartesianTree<int> source = build_from({5, 1, 4, 2, 8});

  target = std::move(source);
  EXPECT_EQ(ct_collect_in_order(target), (std::vector<int>{5, 1, 4, 2, 8}));
  EXPECT_TRUE(target.validate_properties());
  EXPECT_TRUE(source.is_empty()); // NOLINT(bugprone-use-after-move): checking moved-from state
}

TEST(CartesianTreeMove, ThrowingComparatorMoveConstructionPreservesSourceOwnership) {
  auto              state = std::make_shared<CartesianMoveState>();
  CartesianMoveTree source(CartesianThrowingMoveCompare{state});
  source.push_back(5);
  source.push_back(1);
  source.push_back(4);

  state->throw_on_move = true;
  EXPECT_THROW((void)CartesianMoveTree(std::move(source)), std::runtime_error);
  state->throw_on_move = false;

  EXPECT_EQ(source.size(), 3U); // NOLINT(bugprone-use-after-move): the move construction failed
  EXPECT_TRUE(source.validate_properties());
  EXPECT_EQ(source.range_extremum(0, 2), 1);
}

//===----- MOVE-ONLY VALUE SUPPORT ---------------------------------------------===//

TEST(CartesianTreeTypes, SupportsMoveOnlyValues) {
  CartesianTree<CartesianMoveOnly> tree;
  tree.push_back(CartesianMoveOnly(5));
  tree.push_back(CartesianMoveOnly(2));
  tree.push_back(CartesianMoveOnly(8));
  tree.push_back(CartesianMoveOnly(1));

  EXPECT_EQ(tree.size(), 4U);
  EXPECT_EQ(tree.extremum().value, 1);
  EXPECT_TRUE(tree.validate_properties());

  std::vector<int> seen;
  tree.in_order_traversal([&](const CartesianMoveOnly& item) -> void { seen.push_back(item.value); });
  EXPECT_EQ(seen, (std::vector<int>{5, 2, 8, 1}));
  EXPECT_EQ(tree.range_extremum(0, 2).value, 2);
}

TEST(CartesianTreeTypes, SupportsStringValues) {
  CartesianTree<std::string> tree;
  tree.push_back("delta");
  tree.push_back("alpha");
  tree.push_back("charlie");
  tree.push_back("bravo");

  EXPECT_EQ(tree.extremum(), "alpha");
  EXPECT_EQ(tree.range_extremum(2, 3), "bravo");
  EXPECT_TRUE(tree.validate_properties());
}
