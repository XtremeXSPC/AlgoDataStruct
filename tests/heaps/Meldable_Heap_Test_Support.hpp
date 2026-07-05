//===---------------------------------------------------------------------------===//
/**
 * @file Meldable_Heap_Test_Support.hpp
 * @brief Shared test helpers for the meldable heap family (Leftist, Skew,
 *        Binomial, Pairing, Fibonacci).
 *
 * @details Every meldable heap exposes the same public surface, so its unit tests
 *          share the same payload types and behavioural drivers. They live here
 *          once instead of being copied into each Test_*_Heap.cpp. Each test file
 *          aliases this namespace (e.g. 'namespace skew_test = ads::heaps::meldable_test;')
 *          so call sites stay short. The white-box inspectors stay per-structure,
 *          since each heap's internal layout differs.
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef ADS_TESTS_HEAPS_MELDABLE_HEAP_TEST_SUPPORT_HPP
#define ADS_TESTS_HEAPS_MELDABLE_HEAP_TEST_SUPPORT_HPP

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <queue>
#include <random>
#include <vector>

namespace ads::heaps::meldable_test {

// Move-only, totally-ordered payload: exercises the move-only insert/emplace and
// extract paths of any meldable heap.
struct MoveOnlyOrdered {
  int value = 0;

  MoveOnlyOrdered() = default;

  explicit MoveOnlyOrdered(int v) : value(v) {}

  MoveOnlyOrdered(MoveOnlyOrdered&&) noexcept                    = default;
  auto operator=(MoveOnlyOrdered&&) noexcept -> MoveOnlyOrdered& = default;
  MoveOnlyOrdered(const MoveOnlyOrdered&)                        = delete;
  auto operator=(const MoveOnlyOrdered&) -> MoveOnlyOrdered&     = delete;

  auto operator<=>(const MoveOnlyOrdered&) const = default;
};

// Orders by absolute value; used to prove the comparator is fully generic.
struct AbsLess {
  auto operator()(int a, int b) const -> bool { return std::abs(a) < std::abs(b); }
};

// Same comparator type with runtime-selected min/max semantics; used to verify
// that destructive melds reject heaps whose internal orderings differ.
struct StatefulOrder {
  bool min_heap = false;

  auto operator()(int a, int b) const -> bool { return min_heap ? a > b : a < b; }

  auto operator==(const StatefulOrder&) const -> bool = default;
};

// Drains 'heap' and asserts it yields 'input' in the order produced by sorting
// with 'pop_order' (the comparator describing extraction order).
template <typename Heap, typename PopOrder>
void expect_drains_in_order(Heap heap, std::vector<int> input, PopOrder pop_order) {
  std::sort(input.begin(), input.end(), pop_order);
  ASSERT_EQ(heap.size(), input.size());
  for (int expected : input) {
    ASSERT_FALSE(heap.is_empty());
    EXPECT_EQ(heap.top(), expected);
    EXPECT_EQ(heap.extract_top(), expected);
  }
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
}

template <typename Heap>
void expect_equal_stateful_comparators_merge() {
  Heap lhs(StatefulOrder{false});
  Heap rhs(StatefulOrder{false});
  lhs.insert(4);
  rhs.insert(9);

  EXPECT_NO_THROW(lhs.merge(std::move(rhs)));
  expect_drains_in_order(std::move(lhs), {4, 9}, std::greater<int>{});
}

// Interleaved random push/pop workload validated step-by-step against a
// std::priority_queue oracle. Heap and Oracle must agree on priority order.
template <typename Heap, typename Oracle>
void expect_matches_oracle(unsigned seed, int ops) {
  Heap                               heap;
  Oracle                             oracle;
  std::mt19937                       rng(seed);
  std::uniform_int_distribution<int> value(-10'000, 10'000);
  std::bernoulli_distribution        do_pop(0.45);

  for (int i = 0; i < ops; ++i) {
    if (!oracle.empty() && do_pop(rng)) {
      ASSERT_EQ(heap.top(), oracle.top());
      EXPECT_EQ(heap.extract_top(), oracle.top());
      oracle.pop();
    } else {
      int v = value(rng);
      heap.insert(v);
      oracle.push(v);
    }
    ASSERT_EQ(heap.size(), oracle.size());
  }
  while (!oracle.empty()) {
    EXPECT_EQ(heap.extract_top(), oracle.top());
    oracle.pop();
  }
  EXPECT_TRUE(heap.is_empty());
}

using MaxOracle = std::priority_queue<int>;
using MinOracle = std::priority_queue<int, std::vector<int>, std::greater<>>;

} // namespace ads::heaps::meldable_test

#endif // ADS_TESTS_HEAPS_MELDABLE_HEAP_TEST_SUPPORT_HPP

//===---------------------------------------------------------------------------===//
