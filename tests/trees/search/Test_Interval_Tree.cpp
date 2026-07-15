//===---------------------------------------------------------------------------===//
/**
 * @file Test_Interval_Tree.cpp
 * @brief Google Test unit tests for IntervalTree.
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/trees/search/Interval_Tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using namespace ads::trees;

namespace {

using IntTree  = IntervalTree<int, int>;
using Interval = IntTree::Interval;

struct ThrowingEndpoint {
  int value;

  friend auto operator==(const ThrowingEndpoint&, const ThrowingEndpoint&) -> bool = default;

  friend auto operator<(const ThrowingEndpoint& lhs, const ThrowingEndpoint& rhs) noexcept(false) -> bool { return lhs.value < rhs.value; }
};

struct ThrowingValue {
  static inline bool fail_copy = false;

  int value;

  explicit ThrowingValue(int initial) : value(initial) {}

  ThrowingValue(const ThrowingValue& other) : value(other.value) {
    if (fail_copy) {
      throw std::runtime_error("copy failed");
    }
  }

  ThrowingValue(ThrowingValue&&) noexcept                    = default;
  auto operator=(const ThrowingValue&) -> ThrowingValue&     = default;
  auto operator=(ThrowingValue&&) noexcept -> ThrowingValue& = default;
};

struct ThrowingCopyEndpoint {
  static inline bool fail_copy = false;

  int value;

  explicit ThrowingCopyEndpoint(int initial) : value(initial) {}

  ThrowingCopyEndpoint(const ThrowingCopyEndpoint& other) : value(other.value) {
    if (fail_copy) {
      throw std::runtime_error("endpoint copy failed");
    }
  }

  ThrowingCopyEndpoint(ThrowingCopyEndpoint&&) noexcept                    = default;
  auto operator=(const ThrowingCopyEndpoint&) -> ThrowingCopyEndpoint&     = default;
  auto operator=(ThrowingCopyEndpoint&&) noexcept -> ThrowingCopyEndpoint& = default;

  friend auto operator==(const ThrowingCopyEndpoint&, const ThrowingCopyEndpoint&) -> bool = default;

  friend auto operator<(const ThrowingCopyEndpoint& lhs, const ThrowingCopyEndpoint& rhs) noexcept -> bool { return lhs.value < rhs.value; }
};

struct CountingEndpoint {
  static inline std::size_t comparisons = 0;

  int value;

  friend auto operator==(const CountingEndpoint&, const CountingEndpoint&) -> bool = default;

  friend auto operator<(const CountingEndpoint& lhs, const CountingEndpoint& rhs) noexcept -> bool {
    ++comparisons;
    return lhs.value < rhs.value;
  }
};

static_assert(IntervalEndpoint<int>);
static_assert(IntervalEndpoint<double>);
static_assert(IntervalEndpoint<CountingEndpoint>);
static_assert(IntervalEndpoint<ThrowingCopyEndpoint>);
static_assert(!IntervalEndpoint<ThrowingEndpoint>);

//===----- TEST UTILITIES ------------------------------------------------------===//

///@brief A reference interval-value pair used by the brute-force oracles.
struct RefEntry {
  int low;
  int high;
  int value;
};

///@brief Closed-interval overlap used by the oracle, independent of the tree code.
auto overlaps(int a_low, int a_high, int b_low, int b_high) -> bool {
  return a_low <= b_high && b_low <= a_high;
}

///@brief Collects every result of a find_overlapping / find_containing query, sorted.
auto collect(const IntTree& tree, const Interval& query) -> std::vector<std::tuple<int, int, int>> {
  std::vector<std::tuple<int, int, int>> out;
  tree.find_overlapping(query, [&](const Interval& iv, const int& v) { out.emplace_back(iv.low, iv.high, v); });
  std::sort(out.begin(), out.end());
  return out;
}

///@brief Brute-force oracle mirroring collect() over the reference set.
auto oracle(const std::vector<RefEntry>& ref, int q_low, int q_high) -> std::vector<std::tuple<int, int, int>> {
  std::vector<std::tuple<int, int, int>> out;
  for (const RefEntry& e : ref) {
    if (overlaps(e.low, e.high, q_low, q_high)) {
      out.emplace_back(e.low, e.high, e.value);
    }
  }
  std::sort(out.begin(), out.end());
  return out;
}

//===----- BASIC OPERATIONS ----------------------------------------------------===//

TEST(IntervalTreeBasic, EmptyTreeState) {
  IntTree tree;
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0U);
  EXPECT_EQ(tree.height(), 0);
  EXPECT_FALSE(tree.contains({1, 2}));
  EXPECT_EQ(tree.find({1, 2}), nullptr);
  EXPECT_FALSE(tree.overlaps({0, 100}));
  EXPECT_EQ(tree.find_any_overlap({0, 100}), nullptr);
  EXPECT_TRUE(tree.validate_properties());
}

TEST(IntervalTreeBasic, InsertGrowsAndFinds) {
  IntTree tree;
  tree.insert({10, 20}, 1);
  tree.insert({5, 8}, 2);
  tree.insert({15, 25}, 3);

  EXPECT_FALSE(tree.is_empty());
  EXPECT_EQ(tree.size(), 3U);
  EXPECT_EQ(tree.height(), 2);

  const auto* found = tree.find({10, 20});
  ASSERT_NE(found, nullptr);
  EXPECT_EQ(found->value, 1);
  EXPECT_TRUE(tree.contains({5, 8}));
  EXPECT_FALSE(tree.contains({10, 21}));
  EXPECT_TRUE(tree.validate_properties());
}

TEST(IntervalTreeBasic, DegeneratePointInterval) {
  IntTree tree;
  tree.insert({7, 7}, 42);
  EXPECT_TRUE(tree.contains({7, 7}));
  EXPECT_TRUE(tree.overlaps({7, 7}));
  EXPECT_TRUE(tree.overlaps({0, 7}));
  EXPECT_FALSE(tree.overlaps({8, 100}));
}

//===----- OVERLAP QUERIES -----------------------------------------------------===//

TEST(IntervalTreeOverlap, FindAnyOverlapReturnsOverlapping) {
  IntTree tree;
  tree.insert({15, 20}, 1);
  tree.insert({10, 30}, 2);
  tree.insert({17, 19}, 3);
  tree.insert({5, 20}, 4);
  tree.insert({12, 15}, 5);
  tree.insert({30, 40}, 6);

  const auto* hit = tree.find_any_overlap({21, 23});
  ASSERT_NE(hit, nullptr);
  EXPECT_TRUE(overlaps(hit->interval.low, hit->interval.high, 21, 23));

  EXPECT_EQ(tree.find_any_overlap({41, 50}), nullptr);
  EXPECT_FALSE(tree.overlaps({41, 50}));
  EXPECT_TRUE(tree.overlaps({0, 6}));
}

TEST(IntervalTreeOverlap, FindOverlappingMatchesOracle) {
  IntTree               tree;
  std::vector<RefEntry> ref;
  const RefEntry        data[] = {{15, 20, 1}, {10, 30, 2}, {17, 19, 3}, {5, 20, 4}, {12, 15, 5}, {30, 40, 6}};
  for (const RefEntry& e : data) {
    tree.insert({e.low, e.high}, e.value);
    ref.push_back(e);
  }

  for (int lo = 0; lo <= 45; ++lo) {
    for (int hi = lo; hi <= 45; ++hi) {
      EXPECT_EQ(collect(tree, {lo, hi}), oracle(ref, lo, hi)) << "query [" << lo << "," << hi << "]";
    }
  }
}

TEST(IntervalTreeOverlap, FindContainingIsPointStabbing) {
  IntTree tree;
  tree.insert({1, 5}, 1);
  tree.insert({3, 8}, 2);
  tree.insert({6, 10}, 3);

  std::vector<int> stab;
  tree.find_containing(4, [&](const Interval&, const int& v) { stab.push_back(v); });
  std::sort(stab.begin(), stab.end());
  EXPECT_EQ(stab, (std::vector<int>{1, 2}));

  stab.clear();
  tree.find_containing(9, [&](const Interval&, const int& v) { stab.push_back(v); });
  EXPECT_EQ(stab, (std::vector<int>{3}));
}

//===----- RANDOMIZED ORACLE ---------------------------------------------------===//

TEST(IntervalTreeRandom, BalancedAndCorrectUnderManyQueries) {
  std::mt19937                    rng(12'345);
  std::uniform_int_distribution<> low_dist(0, 500);
  std::uniform_int_distribution<> len_dist(0, 40);

  IntTree               tree;
  std::vector<RefEntry> ref;
  const int             n = 2'000;
  for (int i = 0; i < n; ++i) {
    const int low  = low_dist(rng);
    const int high = low + len_dist(rng);
    tree.insert({low, high}, i);
    ref.push_back({low, high, i});
  }

  ASSERT_EQ(tree.size(), static_cast<std::size_t>(n));
  EXPECT_TRUE(tree.validate_properties());

  // AVL height bound: h <= 1.4405 * log2(n + 2) - 0.3277.
  const double bound = 1.4405 * std::log2(static_cast<double>(n) + 2.0) - 0.3277;
  EXPECT_LE(tree.height(), static_cast<int>(bound) + 1);

  std::uniform_int_distribution<> q_low(0, 520);
  std::uniform_int_distribution<> q_len(0, 60);
  for (int q = 0; q < 400; ++q) {
    const int lo = q_low(rng);
    const int hi = lo + q_len(rng);
    EXPECT_EQ(collect(tree, {lo, hi}), oracle(ref, lo, hi));

    const auto* any          = tree.find_any_overlap({lo, hi});
    const bool  expected_any = !oracle(ref, lo, hi).empty();
    EXPECT_EQ(any != nullptr, expected_any);
    if (any != nullptr) {
      EXPECT_TRUE(overlaps(any->interval.low, any->interval.high, lo, hi));
    }
  }
}

TEST(IntervalTreeOverlap, ReportingPrunesDisjointSubtreesByMaximumEndpoint) {
  using Tree      = IntervalTree<CountingEndpoint, int>;
  constexpr int n = 4'095;

  Tree tree;
  tree.insert({CountingEndpoint{0}, CountingEndpoint{n}}, 0);
  for (int value = 1; value < n; ++value) {
    tree.insert({CountingEndpoint{value}, CountingEndpoint{value}}, value);
  }

  CountingEndpoint::comparisons = 0;
  int hits                      = 0;
  tree.find_containing(CountingEndpoint{n}, [&](const Tree::Interval&, const int&) -> void { ++hits; });

  EXPECT_EQ(hits, 1);
  EXPECT_LE(CountingEndpoint::comparisons, static_cast<std::size_t>(tree.height()) * 16U);
}

//===----- DUPLICATES (MULTISET) -----------------------------------------------===//

TEST(IntervalTreeDuplicates, IdenticalIntervalsCoexist) {
  IntTree tree;
  tree.insert({4, 9}, 100);
  tree.insert({4, 9}, 200);
  tree.insert({4, 9}, 300);
  EXPECT_EQ(tree.size(), 3U);
  EXPECT_TRUE(tree.validate_properties());

  int count = 0;
  tree.find_overlapping({4, 9}, [&](const Interval&, const int&) { ++count; });
  EXPECT_EQ(count, 3);
}

TEST(IntervalTreeDuplicates, RemoveDeletesSingleOccurrence) {
  IntTree tree;
  tree.insert({4, 9}, 100);
  tree.insert({4, 9}, 200);
  EXPECT_TRUE(tree.remove({4, 9}));
  EXPECT_EQ(tree.size(), 1U);
  EXPECT_TRUE(tree.contains({4, 9}));
  EXPECT_TRUE(tree.remove({4, 9}));
  EXPECT_FALSE(tree.contains({4, 9}));
  EXPECT_EQ(tree.size(), 0U);
  EXPECT_FALSE(tree.remove({4, 9}));
}

//===----- REMOVAL -------------------------------------------------------------===//

TEST(IntervalTreeRemoval, RemoveKeepsInvariantsUnderChurn) {
  std::mt19937                    rng(999);
  std::uniform_int_distribution<> low_dist(0, 200);
  std::uniform_int_distribution<> len_dist(0, 20);

  IntTree                          tree;
  std::vector<std::pair<int, int>> live;
  for (int i = 0; i < 1'000; ++i) {
    const int low  = low_dist(rng);
    const int high = low + len_dist(rng);
    tree.insert({low, high}, i);
    live.emplace_back(low, high);
  }
  ASSERT_TRUE(tree.validate_properties());

  std::shuffle(live.begin(), live.end(), rng);
  for (std::size_t i = 0; i < live.size(); i += 2) {
    EXPECT_TRUE(tree.remove({live[i].first, live[i].second}));
  }
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.size(), 1'000U - (live.size() + 1) / 2);
}

TEST(IntervalTreeRemoval, RemoveMissingReturnsFalse) {
  IntTree tree;
  tree.insert({1, 2}, 1);
  EXPECT_FALSE(tree.remove({3, 4}));
  EXPECT_EQ(tree.size(), 1U);
}

//===----- ITERATOR AND TRAVERSAL ----------------------------------------------===//

TEST(IntervalTreeIterator, InOrderIsSortedByKey) {
  IntTree                   tree;
  const std::pair<int, int> keys[] = {{5, 9}, {1, 2}, {5, 6}, {3, 4}, {1, 8}};
  int                       v      = 0;
  for (const auto& k : keys) {
    tree.insert({k.first, k.second}, v++);
  }

  std::vector<std::pair<int, int>> seen;
  for (const auto& entry : tree) {
    seen.emplace_back(entry.interval.low, entry.interval.high);
  }
  EXPECT_TRUE(std::is_sorted(seen.begin(), seen.end()));

  std::vector<std::pair<int, int>> via_traversal;
  tree.in_order_traversal([&](const Interval& iv, const int&) { via_traversal.emplace_back(iv.low, iv.high); });
  EXPECT_EQ(seen, via_traversal);
}

//===----- CONSTRUCTION VARIANTS -----------------------------------------------===//

TEST(IntervalTreeConstruction, FromEntryArray) {
  DynamicArray<IntTree::Entry> entries;
  entries.push_back({{3, 7}, 1});
  entries.push_back({{1, 4}, 2});
  entries.push_back({{6, 9}, 3});

  IntTree tree(entries);
  EXPECT_EQ(tree.size(), 3U);
  EXPECT_TRUE(tree.contains({1, 4}));
  EXPECT_TRUE(tree.validate_properties());
}

TEST(IntervalTreeConstruction, FromIteratorRange) {
  std::vector<IntTree::Entry> entries = {{{3, 7}, 1}, {{1, 4}, 2}, {{6, 9}, 3}};
  IntTree                     tree(entries.begin(), entries.end());
  EXPECT_EQ(tree.size(), 3U);
  EXPECT_TRUE(tree.contains({6, 9}));
}

//===----- MOVE SEMANTICS ------------------------------------------------------===//

TEST(IntervalTreeMove, MoveConstructTransfersOwnership) {
  IntTree source;
  source.insert({1, 5}, 1);
  source.insert({2, 6}, 2);

  IntTree moved(std::move(source));
  EXPECT_EQ(moved.size(), 2U);
  EXPECT_TRUE(moved.contains({1, 5}));
  EXPECT_TRUE(source.is_empty());
  EXPECT_EQ(source.size(), 0U);
}

TEST(IntervalTreeMove, MoveAssignReplacesTarget) {
  IntTree source;
  source.insert({1, 5}, 1);
  IntTree target;
  target.insert({9, 9}, 9);

  target = std::move(source);
  EXPECT_EQ(target.size(), 1U);
  EXPECT_TRUE(target.contains({1, 5}));
  EXPECT_FALSE(target.contains({9, 9}));
}

//===----- ALTERNATE TYPES -----------------------------------------------------===//

TEST(IntervalTreeTypes, StringValuesAndDoubleEndpoints) {
  IntervalTree<double, std::string> tree;
  tree.insert({1.5, 3.5}, "a");
  tree.insert({2.0, 4.0}, "b");

  const auto* found = tree.find({1.5, 3.5});
  ASSERT_NE(found, nullptr);
  EXPECT_EQ(found->value, "a");

  std::vector<std::string> hits;
  tree.find_containing(3.0, [&](const IntervalTree<double, std::string>::Interval&, const std::string& s) { hits.push_back(s); });
  std::sort(hits.begin(), hits.end());
  EXPECT_EQ(hits, (std::vector<std::string>{"a", "b"}));
}

//===----- ERROR HANDLING ------------------------------------------------------===//

TEST(IntervalTreeErrors, MalformedIntervalThrows) {
  IntTree tree;
  EXPECT_THROW(tree.insert({5, 1}, 0), BinaryTreeException);
  EXPECT_THROW(tree.remove({5, 1}), BinaryTreeException);
  EXPECT_THROW((void)tree.find({5, 1}), BinaryTreeException);
  EXPECT_THROW((void)tree.overlaps({5, 1}), BinaryTreeException);
  EXPECT_THROW(tree.find_overlapping({5, 1}, [](const Interval&, const int&) {}), BinaryTreeException);
}

TEST(IntervalTreeErrors, NaNEndpointsAndQueryPointsThrow) {
  using Tree           = IntervalTree<double, int>;
  using DoubleInterval = Tree::Interval;
  const double nan     = std::numeric_limits<double>::quiet_NaN();

  Tree tree;
  EXPECT_THROW(tree.insert({nan, nan}, 1), BinaryTreeException);
  EXPECT_TRUE(tree.is_empty());

  tree.insert({1.0, 2.0}, 7);
  EXPECT_THROW(tree.remove({nan, 2.0}), BinaryTreeException);
  EXPECT_THROW((void)tree.find({nan, 2.0}), BinaryTreeException);
  EXPECT_THROW((void)tree.overlaps({100.0, nan}), BinaryTreeException);
  EXPECT_THROW(tree.find_overlapping({nan, nan}, [](const DoubleInterval&, const int&) -> void {}), BinaryTreeException);
  EXPECT_THROW(tree.find_containing(nan, [](const DoubleInterval&, const int&) -> void {}), BinaryTreeException);
  EXPECT_EQ(tree.size(), 1U);
  EXPECT_TRUE(tree.validate_properties());
}

TEST(IntervalTreeExceptionSafety, FailedValueCopyPreservesExistingTree) {
  using Tree = IntervalTree<int, ThrowingValue>;

  Tree tree;
  tree.insert({10, 20}, ThrowingValue{1});
  tree.insert({5, 8}, ThrowingValue{2});
  ThrowingValue value{3};

  ThrowingValue::fail_copy = true;
  EXPECT_THROW(tree.insert({15, 25}, value), std::runtime_error);
  ThrowingValue::fail_copy = false;

  EXPECT_EQ(tree.size(), 2U);
  EXPECT_FALSE(tree.is_empty());
  EXPECT_TRUE(tree.contains({10, 20}));
  EXPECT_TRUE(tree.contains({5, 8}));
  EXPECT_FALSE(tree.contains({15, 25}));
  EXPECT_TRUE(tree.validate_properties());
}

TEST(IntervalTreeExceptionSafety, FailedEndpointCopyPreservesExistingTree) {
  using Tree     = IntervalTree<ThrowingCopyEndpoint, int>;
  using Endpoint = ThrowingCopyEndpoint;

  Tree tree;
  tree.insert({Endpoint{10}, Endpoint{20}}, 1);
  tree.insert({Endpoint{5}, Endpoint{8}}, 2);
  const Tree::Interval interval{Endpoint{15}, Endpoint{25}};

  Endpoint::fail_copy = true;
  EXPECT_THROW(tree.insert(interval, 3), std::runtime_error);
  Endpoint::fail_copy = false;

  EXPECT_EQ(tree.size(), 2U);
  EXPECT_TRUE(tree.contains({Endpoint{10}, Endpoint{20}}));
  EXPECT_TRUE(tree.contains({Endpoint{5}, Endpoint{8}}));
  EXPECT_FALSE(tree.contains(interval));
  EXPECT_TRUE(tree.validate_properties());
}

} // namespace

//===---------------------------------------------------------------------------===//
