//===---------------------------------------------------------------------------===//
/**
 * @file Test_Sparse_Table.cpp
 * @brief Unit tests for the idempotent-semigroup SparseTable.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/range/Sparse_Table.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <numeric>
#include <random>
#include <string>
#include <vector>

using namespace ads::range;

namespace {

// Idempotent, associative custom operation (bitwise OR) for policy testing.
struct BitOrPolicy {
  using value_type = int;

  static constexpr bool is_commutative = true;
  static constexpr bool is_idempotent  = true;

  [[nodiscard]] auto combine(const int& a, const int& b) const -> int { return a | b; }
};

// Missing the required algebraic metadata and combine operation.
struct NotAPolicy {
  using value_type = int;
};

// Brute-force aggregate over the inclusive range [l, r].
template <typename T, typename Policy>
auto brute(const std::vector<T>& v, size_t l, size_t r, const Policy& policy) -> T {
  T acc = v[l];
  for (size_t i = l + 1; i <= r; ++i) {
    acc = policy.combine(acc, v[i]);
  }
  return acc;
}

// Exhaustively checks every [l, r] against the brute-force oracle.
template <typename T, typename Policy>
auto check_all_ranges(const SparseTable<T, Policy>& table, const std::vector<T>& v, const Policy& policy) -> void {
  ASSERT_EQ(table.size(), v.size());
  for (size_t l = 0; l < v.size(); ++l) {
    for (size_t r = l; r < v.size(); ++r) {
      EXPECT_EQ(table.range_query(l, r), brute(v, l, r, policy)) << "range [" << l << ", " << r << "]";
    }
  }
}

} // namespace

//===----- CONCEPT STATIC ASSERTIONS -------------------------------------------===//

static_assert(ads::algebra::IdempotentSemigroupPolicy<ads::algebra::MinSemigroup<int>>);
static_assert(ads::algebra::IdempotentSemigroupPolicy<ads::algebra::MaxSemigroup<int>>);
static_assert(ads::algebra::IdempotentSemigroupPolicy<ads::algebra::GcdMonoid<int>>);
static_assert(ads::algebra::IdempotentSemigroupPolicy<BitOrPolicy>);
static_assert(ads::algebra::IdempotentSemigroupPolicy<ads::algebra::MinSemigroup<std::string>>);
static_assert(!ads::algebra::IdempotentSemigroupPolicy<NotAPolicy>);

//===----- CONSTRUCTION TESTS --------------------------------------------------===//

TEST(SparseTable, DefaultIsEmpty) {
  SparseTable<int> table;
  EXPECT_TRUE(table.is_empty());
  EXPECT_EQ(table.size(), 0u);
  EXPECT_EQ(table.begin(), table.end());
}

TEST(SparseTable, SingleElement) {
  SparseTable<int> table(std::vector<int>{42});
  EXPECT_EQ(table.size(), 1u);
  EXPECT_FALSE(table.is_empty());
  EXPECT_EQ(table.range_query(0, 0), 42);
  EXPECT_EQ(table.value_at(0), 42);
}

TEST(SparseTable, InitializerListConstruction) {
  SparseTable<int> table{5, 3, 8, 1, 9, 2};
  EXPECT_EQ(table.size(), 6u);
  EXPECT_EQ(table.range_query(0, 5), 1); // min of all
  EXPECT_EQ(table.range_query(2, 4), 1); // {8, 1, 9}
  EXPECT_EQ(table.range_query(0, 1), 3); // {5, 3}
  EXPECT_EQ(table.range_query(4, 5), 2); // {9, 2}
}

TEST(SparseTable, IteratorRangeConstruction) {
  std::vector<int> v = {7, 2, 5, 1, 8};
  SparseTable<int> table(v.begin(), v.end());
  check_all_ranges(table, v, ads::algebra::MinSemigroup<int>{});
}

TEST(SparseTable, MoveVectorConstruction) {
  std::vector<int> v    = {4, 4, 4, 2, 6};
  std::vector<int> copy = v;
  SparseTable<int> table(std::move(v));
  check_all_ranges(table, copy, ads::algebra::MinSemigroup<int>{});
}

TEST(SparseTable, IterationYieldsOriginalValues) {
  std::vector<int> v = {9, 1, 4, 1, 5, 9, 2, 6};
  SparseTable<int> table(v);
  std::vector<int> seen(table.begin(), table.end());
  EXPECT_EQ(seen, v);
}

//===----- QUERY CORRECTNESS TESTS ---------------------------------------------===//

TEST(SparseTable, MinExhaustive) {
  std::vector<int> v = {5, 2, 9, 3, 7, 1, 8, 4, 6, 0, 10, 11, 2, 2, 3, 15, 7};
  SparseTable<int> table(v);
  check_all_ranges(table, v, ads::algebra::MinSemigroup<int>{});
}

TEST(SparseTable, MaxExhaustive) {
  std::vector<int>                                  v = {5, 2, 9, 3, 7, 1, 8, 4, 6, 0, 10, 11, 2, 2, 3};
  SparseTable<int, ads::algebra::MaxSemigroup<int>> table(v);
  check_all_ranges(table, v, ads::algebra::MaxSemigroup<int>{});
}

TEST(SparseTable, GcdExhaustive) {
  std::vector<int>                               v = {12, 18, 24, 6, 9, 15, 100, 40, 8, 16};
  SparseTable<int, ads::algebra::GcdMonoid<int>> table(v);
  check_all_ranges(table, v, ads::algebra::GcdMonoid<int>{});
}

TEST(SparseTable, CustomOperationExhaustive) {
  std::vector<int>              v = {1, 2, 4, 8, 16, 3, 5, 6, 7};
  SparseTable<int, BitOrPolicy> table(v);
  check_all_ranges(table, v, BitOrPolicy{});
}

TEST(SparseTable, StringMinLexicographic) {
  std::vector<std::string>                                          v = {"pear", "apple", "orange", "banana", "kiwi"};
  SparseTable<std::string, ads::algebra::MinSemigroup<std::string>> table(v);
  EXPECT_EQ(table.range_query(0, 4), "apple");
  EXPECT_EQ(table.range_query(2, 4), "banana");
  EXPECT_EQ(table.range_query(0, 0), "pear");
  check_all_ranges(table, v, ads::algebra::MinSemigroup<std::string>{});
}

TEST(SparseTable, FullRangeAndSingletons) {
  std::vector<int> v = {8, 3, 6, 1, 9, 4};
  SparseTable<int> table(v);
  EXPECT_EQ(table.range_query(0, v.size() - 1), 1);
  for (size_t i = 0; i < v.size(); ++i) {
    EXPECT_EQ(table.range_query(i, i), v[i]);
    EXPECT_EQ(table.value_at(i), v[i]);
  }
}

//===----- EXCEPTION TESTS -----------------------------------------------------===//

TEST(SparseTable, QueryExceptions) {
  SparseTable<int> table(std::vector<int>{1, 2, 3});
  EXPECT_THROW((void)table.range_query(2, 1), InvalidRangeException); // left > right
  EXPECT_THROW((void)table.range_query(0, 3), InvalidRangeException); // right out of bounds
  EXPECT_THROW((void)table.range_query(3, 3), InvalidRangeException); // left out of bounds
  EXPECT_NO_THROW((void)table.range_query(0, 2));

  SparseTable<int> empty;
  EXPECT_THROW((void)empty.range_query(0, 0), InvalidRangeException);
}

TEST(SparseTable, ValueAtException) {
  SparseTable<int> table(std::vector<int>{1, 2, 3});
  EXPECT_NO_THROW((void)table.value_at(2));
  EXPECT_THROW((void)table.value_at(3), RangeIndexException);
  // RangeIndexException derives from RangeException.
  EXPECT_THROW((void)table.value_at(99), RangeException);
}

//===----- MODIFICATION TESTS --------------------------------------------------===//

TEST(SparseTable, RebuildViaBuild) {
  SparseTable<int> table;
  table.build(std::vector<int>{3, 1, 4, 1, 5, 9, 2, 6});
  EXPECT_EQ(table.size(), 8u);
  EXPECT_EQ(table.range_query(0, 7), 1);

  table.build({10, 20, 5, 30}); // initializer_list
  EXPECT_EQ(table.size(), 4u);
  EXPECT_EQ(table.range_query(0, 3), 5);

  std::vector<int> more = {2, 2, 2};
  table.build(more.begin(), more.end()); // iterator range
  EXPECT_EQ(table.range_query(0, 2), 2);
}

TEST(SparseTable, Clear) {
  SparseTable<int> table(std::vector<int>{1, 2, 3});
  table.clear();
  EXPECT_TRUE(table.is_empty());
  EXPECT_EQ(table.size(), 0u);
  EXPECT_EQ(table.begin(), table.end());
}

//===----- MOVE SEMANTICS TESTS ------------------------------------------------===//

TEST(SparseTable, MoveConstructorLeavesSourceEmpty) {
  std::vector<int> v = {5, 3, 8, 1};
  SparseTable<int> source(v);
  SparseTable<int> moved = std::move(source);
  EXPECT_TRUE(source.is_empty());
  EXPECT_EQ(moved.size(), 4u);
  EXPECT_EQ(moved.range_query(0, 3), 1);
}

TEST(SparseTable, MoveAssignmentLeavesSourceEmpty) {
  SparseTable<int> a(std::vector<int>{5, 3, 8, 1, 9});
  SparseTable<int> b(std::vector<int>{100, 200});
  b = std::move(a);
  EXPECT_TRUE(a.is_empty());
  EXPECT_EQ(b.size(), 5u);
  EXPECT_EQ(b.range_query(0, 4), 1);
}

//===----- RANDOMIZED ORACLE TESTS ---------------------------------------------===//

TEST(SparseTable, RandomizedMinAgainstOracle) {
  std::mt19937                    rng(2'024);
  std::uniform_int_distribution<> size_dist(1, 200);
  std::uniform_int_distribution<> val_dist(-1'000, 1'000);

  for (int trial = 0; trial < 60; ++trial) {
    const int        n = size_dist(rng);
    std::vector<int> v(static_cast<size_t>(n));
    for (int& x : v) {
      x = val_dist(rng);
    }
    SparseTable<int> table(v);

    std::uniform_int_distribution<> idx(0, n - 1);
    for (int q = 0; q < 50; ++q) {
      int l = idx(rng);
      int r = idx(rng);
      if (l > r) {
        std::swap(l, r);
      }
      ASSERT_EQ(
          table.range_query(static_cast<size_t>(l), static_cast<size_t>(r)),
          brute(v, static_cast<size_t>(l), static_cast<size_t>(r), ads::algebra::MinSemigroup<int>{}))
          << "trial " << trial << " range [" << l << ", " << r << "]";
    }
  }
}

TEST(SparseTable, RandomizedMaxAndGcdAgainstOracle) {
  std::mt19937                    rng(777);
  std::uniform_int_distribution<> size_dist(1, 150);
  std::uniform_int_distribution<> val_dist(1, 500);

  for (int trial = 0; trial < 40; ++trial) {
    const int        n = size_dist(rng);
    std::vector<int> v(static_cast<size_t>(n));
    for (int& x : v) {
      x = val_dist(rng);
    }
    SparseTable<int, ads::algebra::MaxSemigroup<int>> tmax(v);
    SparseTable<int, ads::algebra::GcdMonoid<int>>    tgcd(v);

    std::uniform_int_distribution<> idx(0, n - 1);
    for (int q = 0; q < 40; ++q) {
      int l = idx(rng);
      int r = idx(rng);
      if (l > r) {
        std::swap(l, r);
      }
      const auto lo = static_cast<size_t>(l);
      const auto hi = static_cast<size_t>(r);
      ASSERT_EQ(tmax.range_query(lo, hi), brute(v, lo, hi, ads::algebra::MaxSemigroup<int>{})) << "max trial " << trial;
      ASSERT_EQ(tgcd.range_query(lo, hi), brute(v, lo, hi, ads::algebra::GcdMonoid<int>{})) << "gcd trial " << trial;
    }
  }
}
