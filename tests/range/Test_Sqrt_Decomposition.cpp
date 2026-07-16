//===---------------------------------------------------------------------------===//
/**
 * @file Test_Sqrt_Decomposition.cpp
 * @brief Unit tests for the acted-monoid SqrtDecomposition.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/range/Sqrt_Decomposition.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

using namespace ads::range;

namespace {

//===----- ORACLES -------------------------------------------------------------===//

// Reference model for the default configuration: range-add updates, range-sum queries.
struct SumOracle {
  std::vector<long> v;

  void range_add(size_t l, size_t r, long delta) {
    for (size_t i = l; i <= r; ++i) {
      v[i] += delta;
    }
  }

  void set(size_t index, long value) { v[index] = value; }

  [[nodiscard]] auto range_sum(size_t l, size_t r) const -> long {
    long s = 0;
    for (size_t i = l; i <= r; ++i) {
      s += v[i];
    }
    return s;
  }
};

//===----- CUSTOM CONFIGURATION: range-add / range-min -------------------------===//

using SdMinSqrt = SqrtDecomposition<long, ads::algebra::RangeAddRangeMin<long>>;

struct SdMinOracle {
  std::vector<long> v;

  void range_add(size_t l, size_t r, long delta) {
    for (size_t i = l; i <= r; ++i) {
      v[i] += delta;
    }
  }

  [[nodiscard]] auto range_min(size_t l, size_t r) const -> long {
    long m = std::numeric_limits<long>::max();
    for (size_t i = l; i <= r; ++i) {
      m = (v[i] < m) ? v[i] : m;
    }
    return m;
  }
};

struct BadActedPolicy {
  using value_monoid_type  = ads::algebra::AddMonoid<long>;
  using action_monoid_type = ads::algebra::AddMonoid<long>;
  using value_type         = long;
  using action_type        = long;
};

struct MoveOnlyTag {
  MoveOnlyTag()                                          = default;
  MoveOnlyTag(MoveOnlyTag&&) noexcept                    = default;
  auto operator=(MoveOnlyTag&&) noexcept -> MoveOnlyTag& = default;
  MoveOnlyTag(const MoveOnlyTag&)                        = delete;
  auto operator=(const MoveOnlyTag&) -> MoveOnlyTag&     = delete;
};

struct MoveOnlyActionMonoid {
  using value_type = MoveOnlyTag;

  static constexpr bool is_commutative = true;
  static constexpr bool is_idempotent  = false;

  [[nodiscard]] auto identity() const -> MoveOnlyTag { return {}; }

  [[nodiscard]] auto combine(const MoveOnlyTag&, const MoveOnlyTag&) const -> MoveOnlyTag { return {}; }
};

struct MoveOnlyApply {
  [[nodiscard]] auto apply(const long& value, const MoveOnlyTag&, std::size_t) const -> long { return value; }
};

using MoveOnlyActed = ads::algebra::ActedMonoid<ads::algebra::AddMonoid<long>, MoveOnlyActionMonoid, MoveOnlyApply>;

struct ThrowingApply {
  static inline std::size_t calls    = 0;
  static inline std::size_t throw_on = std::numeric_limits<std::size_t>::max();

  [[nodiscard]] auto apply(const long& value, const long& tag, std::size_t len) const -> long {
    if (++calls == throw_on) {
      throw std::runtime_error("injected apply failure");
    }
    return value + tag * static_cast<long>(len);
  }

  static auto reset(std::size_t next_throw = std::numeric_limits<std::size_t>::max()) -> void {
    calls    = 0;
    throw_on = next_throw;
  }
};

struct ThrowingCombineMonoid {
  using value_type = long;

  static constexpr bool is_commutative = true;
  static constexpr bool is_idempotent  = false;

  static inline std::size_t calls    = 0;
  static inline std::size_t throw_on = std::numeric_limits<std::size_t>::max();

  [[nodiscard]] auto identity() const -> long { return 0; }

  [[nodiscard]] auto combine(const long& left, const long& right) const -> long {
    if (++calls == throw_on) {
      throw std::runtime_error("injected combine failure");
    }
    return left + right;
  }

  static auto reset(std::size_t next_throw = std::numeric_limits<std::size_t>::max()) -> void {
    calls    = 0;
    throw_on = next_throw;
  }
};

struct ThrowingActionMonoid {
  using value_type = long;

  static constexpr bool is_commutative = true;
  static constexpr bool is_idempotent  = false;

  static inline bool should_throw = false;

  [[nodiscard]] auto identity() const -> long { return 0; }

  [[nodiscard]] auto combine(const long& old_tag, const long& new_tag) const -> long {
    if (should_throw) {
      throw std::runtime_error("injected compose failure");
    }
    return old_tag + new_tag;
  }
};

struct ThrowingMoveMonoid {
  using value_type = long;

  static constexpr bool is_commutative = true;
  static constexpr bool is_idempotent  = false;

  ThrowingMoveMonoid()                                             = default;
  ThrowingMoveMonoid(const ThrowingMoveMonoid&)                    = default;
  auto operator=(const ThrowingMoveMonoid&) -> ThrowingMoveMonoid& = default;

  ThrowingMoveMonoid(ThrowingMoveMonoid&&) noexcept(false) {}

  auto operator=(ThrowingMoveMonoid&&) noexcept(false) -> ThrowingMoveMonoid& { return *this; }

  [[nodiscard]] auto identity() const -> long { return 0; }

  [[nodiscard]] auto combine(const long& left, const long& right) const -> long { return left + right; }
};

using ThrowingMoveActed =
    ads::algebra::ActedMonoid<ThrowingMoveMonoid, ads::algebra::AddMonoid<long>, ads::algebra::RangeAddSumApplicator<long>>;
using ThrowingMoveSqrt = SqrtDecomposition<long, ThrowingMoveActed>;

} // namespace

//===----- CONCEPT STATIC ASSERTIONS -------------------------------------------===//

static_assert(RangeActionPolicy<long, ads::algebra::RangeAddRangeSum<long>>);
static_assert(RangeActionPolicy<long, ads::algebra::RangeAddRangeMin<long>>);
static_assert(!ads::algebra::ActedMonoidPolicy<BadActedPolicy>);
static_assert(ads::algebra::ActedMonoidPolicy<MoveOnlyActed>);
static_assert(!RangeActionPolicy<long, MoveOnlyActed>);
static_assert(!std::is_nothrow_move_constructible_v<ThrowingMoveSqrt>);
static_assert(!std::is_nothrow_move_assignable_v<ThrowingMoveSqrt>);

//===----- CONSTRUCTION TESTS --------------------------------------------------===//

TEST(SqrtDecomposition, DefaultIsEmpty) {
  SqrtDecomposition<long> sd;
  EXPECT_TRUE(sd.is_empty());
  EXPECT_EQ(sd.size(), 0u);
  EXPECT_EQ(sd.block_size(), 0u);
  EXPECT_TRUE(sd.to_vector().empty());
}

TEST(SqrtDecomposition, SingleElement) {
  SqrtDecomposition<long> sd(std::vector<long>{42});
  EXPECT_EQ(sd.size(), 1u);
  EXPECT_EQ(sd.range_query(0, 0), 42);
  EXPECT_EQ(sd.value_at(0), 42);
  sd.apply(0, 8);
  EXPECT_EQ(sd.range_query(0, 0), 50);
}

TEST(SqrtDecomposition, ConstructionVariants) {
  const std::vector<long> expected = {3, 1, 4, 1, 5, 9, 2, 6};
  SqrtDecomposition<long> from_vec(expected);
  SqrtDecomposition<long> from_init{3, 1, 4, 1, 5, 9, 2, 6};
  SqrtDecomposition<long> from_range(expected.begin(), expected.end());
  std::vector<long>       copy = expected;
  SqrtDecomposition<long> from_move(std::move(copy));

  EXPECT_EQ(from_vec.to_vector(), expected);
  EXPECT_EQ(from_init.to_vector(), expected);
  EXPECT_EQ(from_range.to_vector(), expected);
  EXPECT_EQ(from_move.to_vector(), expected);
}

TEST(SqrtDecomposition, BlockSizeIsRoughlySqrt) {
  for (size_t n : {1u, 4u, 16u, 100u, 101u, 1'000u}) {
    SqrtDecomposition<long> sd(std::vector<long>(n, 0));
    const size_t            bs = sd.block_size();
    EXPECT_GE(bs, 1u);
    EXPECT_LE(bs * bs, 4 * n);             // bs <= 2*sqrt(n)
    EXPECT_GE((bs + 1) * (bs + 1), n / 4); // not absurdly small
  }
}

//===----- INITIAL QUERY TESTS -------------------------------------------------===//

TEST(SqrtDecomposition, InitialQueriesExhaustive) {
  std::vector<long>       v = {5, 2, 9, 3, 7, 1, 8, 4, 6, 0, 10, 11, 2, 2, 3, 15, 7};
  SqrtDecomposition<long> sd(v);
  SumOracle               oracle{v};
  for (size_t l = 0; l < v.size(); ++l) {
    for (size_t r = l; r < v.size(); ++r) {
      EXPECT_EQ(sd.range_query(l, r), oracle.range_sum(l, r)) << "range [" << l << ", " << r << "]";
    }
  }
}

//===----- RANGE UPDATE TESTS --------------------------------------------------===//

TEST(SqrtDecomposition, RangeUpdateExhaustiveAfterEachOp) {
  std::vector<long>       v(30, 0);
  SqrtDecomposition<long> sd(v);
  SumOracle               oracle{v};

  struct Upd {
    size_t l, r;
    long   d;
  };

  const std::vector<Upd> updates = {{0, 29, 3}, {5, 12, -2}, {2, 2, 100}, {7, 25, 5}, {0, 0, -50}, {10, 29, 1}, {3, 9, 7}, {14, 28, -4}};

  for (const auto& u : updates) {
    sd.range_apply(u.l, u.r, u.d);
    oracle.range_add(u.l, u.r, u.d);
    for (size_t l = 0; l < v.size(); ++l) {
      for (size_t r = l; r < v.size(); ++r) {
        ASSERT_EQ(sd.range_query(l, r), oracle.range_sum(l, r)) << "after update; range [" << l << ", " << r << "]";
      }
    }
    for (size_t i = 0; i < v.size(); ++i) {
      ASSERT_EQ(sd.value_at(i), oracle.v[i]);
    }
  }
}

TEST(SqrtDecomposition, PointUpdateEquivalentToSingletonRange) {
  SqrtDecomposition<long> sd(std::vector<long>(20, 0));
  sd.apply(7, 5);
  sd.range_apply(7, 7, 3);
  EXPECT_EQ(sd.value_at(7), 8);
  EXPECT_EQ(sd.range_query(0, 19), 8);
}

TEST(SqrtDecomposition, PointSetMaterializesPendingBlockAndRefreshesAggregate) {
  std::vector<long>       values{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  SqrtDecomposition<long> sd(values);

  sd.range_apply(0, 15, 5);
  sd.set(7, 100);

  for (long& value : values) {
    value += 5;
  }
  values[7] = 100;
  EXPECT_EQ(sd.to_vector(), values);
  EXPECT_EQ(sd.total(), std::accumulate(values.begin(), values.end(), 0L));
}

//===----- CUSTOM CONFIGURATION TESTS ------------------------------------------===//

TEST(SqrtDecomposition, RangeAddRangeMin) {
  std::vector<long> v = {8, 3, 6, 1, 9, 4, 7, 2, 5, 10, 0, 12, 3, 3, 8};
  SdMinSqrt         sd(v);
  SdMinOracle       oracle{v};

  // Initial min queries.
  for (size_t l = 0; l < v.size(); ++l) {
    for (size_t r = l; r < v.size(); ++r) {
      ASSERT_EQ(sd.range_query(l, r), oracle.range_min(l, r));
    }
  }

  const std::vector<std::array<long, 3>> updates = {{0, 14, 2}, {4, 10, -3}, {0, 0, 100}, {6, 13, 5}};
  for (const auto& u : updates) {
    sd.range_apply(static_cast<size_t>(u[0]), static_cast<size_t>(u[1]), u[2]);
    oracle.range_add(static_cast<size_t>(u[0]), static_cast<size_t>(u[1]), u[2]);
    for (size_t l = 0; l < v.size(); ++l) {
      for (size_t r = l; r < v.size(); ++r) {
        ASSERT_EQ(sd.range_query(l, r), oracle.range_min(l, r)) << "min range [" << l << ", " << r << "]";
      }
    }
  }
}

//===----- MODIFICATION TESTS --------------------------------------------------===//

TEST(SqrtDecomposition, RebuildViaBuild) {
  SqrtDecomposition<long> sd;
  sd.build(std::vector<long>{1, 2, 3, 4, 5});
  EXPECT_EQ(sd.range_query(0, 4), 15);
  sd.build({10, 20, 30});
  EXPECT_EQ(sd.range_query(0, 2), 60);
  sd.clear();
  EXPECT_TRUE(sd.is_empty());
}

//===----- MOVE SEMANTICS TESTS ------------------------------------------------===//

TEST(SqrtDecomposition, MoveLeavesSourceEmpty) {
  SqrtDecomposition<long> a(std::vector<long>{5, 3, 8, 1, 9});
  SqrtDecomposition<long> b = std::move(a);
  EXPECT_TRUE(a.is_empty());
  EXPECT_EQ(b.range_query(0, 4), 26);

  SqrtDecomposition<long> c(std::vector<long>{100, 200});
  c = std::move(b);
  EXPECT_TRUE(b.is_empty());
  EXPECT_EQ(c.range_query(0, 4), 26);
}

//===----- EXCEPTION TESTS -----------------------------------------------------===//

TEST(SqrtDecomposition, Exceptions) {
  SqrtDecomposition<long> sd(std::vector<long>{1, 2, 3});
  EXPECT_THROW((void)sd.range_query(2, 1), InvalidRangeException);
  EXPECT_THROW((void)sd.range_query(0, 3), InvalidRangeException);
  EXPECT_THROW(sd.range_apply(1, 0, 5), InvalidRangeException);
  EXPECT_THROW(sd.range_apply(0, 3, 5), InvalidRangeException);
  EXPECT_THROW(sd.set(3, 1), RangeIndexException);
  EXPECT_THROW(sd.apply(3, 1), RangeIndexException);
  EXPECT_THROW((void)sd.value_at(3), RangeException); // RangeIndexException derives from RangeException

  SqrtDecomposition<long> empty;
  EXPECT_THROW((void)empty.range_query(0, 0), InvalidRangeException);
  EXPECT_THROW(empty.set(0, 1), RangeIndexException);
  EXPECT_THROW(empty.apply(0, 1), RangeIndexException);
}

TEST(SqrtDecomposition, ThrowingCombineLeavesPointSetUnchangedAndUsable) {
  using ThrowingActed =
      ads::algebra::ActedMonoid<ThrowingCombineMonoid, ads::algebra::AddMonoid<long>, ads::algebra::RangeAddSumApplicator<long>>;
  using Decomposition = SqrtDecomposition<long, ThrowingActed>;

  ThrowingCombineMonoid::reset();
  Decomposition decomposition(std::vector<long>(16, 2));
  decomposition.range_apply(0, 15, 5);
  const auto before = decomposition.to_vector();

  ThrowingCombineMonoid::reset(2);
  EXPECT_THROW(decomposition.set(4, 99), std::runtime_error);

  ThrowingCombineMonoid::reset();
  EXPECT_EQ(decomposition.to_vector(), before);
  EXPECT_EQ(decomposition.range_query(0, 15), 112);
  decomposition.set(4, 99);
  EXPECT_EQ(decomposition.value_at(4), 99);
}

TEST(SqrtDecomposition, ThrowingApplyLeavesCurrentBlockUnchangedAndUsable) {
  using ThrowingActed = ads::algebra::ActedMonoid<ads::algebra::AddMonoid<long>, ads::algebra::AddMonoid<long>, ThrowingApply>;
  using Decomposition = SqrtDecomposition<long, ThrowingActed>;

  ThrowingApply::reset();
  Decomposition decomposition(std::vector<long>(16, 0));
  decomposition.range_apply(0, 15, 5);
  const auto before = decomposition.to_vector();

  ThrowingApply::reset(2);
  EXPECT_THROW(decomposition.range_apply(4, 4, 1), std::runtime_error);

  ThrowingApply::reset();
  EXPECT_EQ(decomposition.to_vector(), before);
  EXPECT_EQ(decomposition.range_query(0, 15), 80);
  decomposition.apply(4, 1);
  EXPECT_EQ(decomposition.value_at(4), 6);
}

TEST(SqrtDecomposition, ThrowingCombineLeavesCurrentBlockUnchangedAndUsable) {
  using ThrowingActed =
      ads::algebra::ActedMonoid<ThrowingCombineMonoid, ads::algebra::AddMonoid<long>, ads::algebra::RangeAddSumApplicator<long>>;
  using Decomposition = SqrtDecomposition<long, ThrowingActed>;

  ThrowingCombineMonoid::reset();
  Decomposition decomposition(std::vector<long>(16, 2));
  const auto    before = decomposition.to_vector();

  ThrowingCombineMonoid::reset(2);
  EXPECT_THROW(decomposition.apply(4, 3), std::runtime_error);

  ThrowingCombineMonoid::reset();
  EXPECT_EQ(decomposition.to_vector(), before);
  EXPECT_EQ(decomposition.range_query(0, 15), 32);
}

TEST(SqrtDecomposition, ThrowingComposePreservesAllBlockInvariants) {
  using ThrowingActed =
      ads::algebra::ActedMonoid<ads::algebra::AddMonoid<long>, ThrowingActionMonoid, ads::algebra::RangeAddSumApplicator<long>>;
  using Decomposition = SqrtDecomposition<long, ThrowingActed>;

  ThrowingActionMonoid::should_throw = false;
  Decomposition decomposition(std::vector<long>(16, 0));
  decomposition.range_apply(0, 15, 5);

  ThrowingActionMonoid::should_throw = true;
  EXPECT_THROW(decomposition.range_apply(0, 15, 1), std::runtime_error);

  ThrowingActionMonoid::should_throw = false;
  std::vector<long> expected(16, 5);
  EXPECT_EQ(decomposition.to_vector(), expected);
  EXPECT_EQ(decomposition.range_query(0, 15), 80);
}

//===----- RANDOMIZED ORACLE TESTS ---------------------------------------------===//

TEST(SqrtDecomposition, RandomizedSumAgainstOracle) {
  std::mt19937                    rng(20'260'715);
  std::uniform_int_distribution<> size_dist(1, 400);
  std::uniform_int_distribution<> val_dist(-100, 100);

  for (int trial = 0; trial < 40; ++trial) {
    const int         n = size_dist(rng);
    std::vector<long> init(static_cast<size_t>(n));
    for (long& x : init) {
      x = val_dist(rng);
    }
    SqrtDecomposition<long> sd(init);
    SumOracle               oracle{init};

    std::uniform_int_distribution<> idx(0, n - 1);
    for (int step = 0; step < 200; ++step) {
      int l = idx(rng);
      int r = idx(rng);
      if (l > r) {
        std::swap(l, r);
      }
      const auto lo = static_cast<size_t>(l);
      const auto hi = static_cast<size_t>(r);

      const int kind = static_cast<int>(rng() % 5);
      if (kind == 0) { // range update
        const long d = val_dist(rng);
        sd.range_apply(lo, hi, d);
        oracle.range_add(lo, hi, d);
      } else if (kind == 1) { // point update
        const long d = val_dist(rng);
        sd.apply(lo, d);
        oracle.range_add(lo, lo, d);
      } else if (kind == 2) { // point replacement
        const long value = val_dist(rng);
        sd.set(lo, value);
        oracle.set(lo, value);
      } else if (kind == 3) { // range query
        ASSERT_EQ(sd.range_query(lo, hi), oracle.range_sum(lo, hi))
            << "trial " << trial << " step " << step << " range [" << lo << ", " << hi << "]";
      } else { // point read
        ASSERT_EQ(sd.value_at(lo), oracle.v[lo]) << "trial " << trial << " step " << step;
      }
    }
    // Final full consistency check.
    ASSERT_EQ(sd.to_vector(), oracle.v) << "trial " << trial << " final";
  }
}

TEST(SqrtDecomposition, RandomizedMinAgainstOracle) {
  std::mt19937                    rng(424'242);
  std::uniform_int_distribution<> size_dist(1, 250);
  std::uniform_int_distribution<> val_dist(-50, 50);

  for (int trial = 0; trial < 30; ++trial) {
    const int         n = size_dist(rng);
    std::vector<long> init(static_cast<size_t>(n));
    for (long& x : init) {
      x = val_dist(rng);
    }
    SdMinSqrt   sd(init);
    SdMinOracle oracle{init};

    std::uniform_int_distribution<> idx(0, n - 1);
    for (int step = 0; step < 150; ++step) {
      int l = idx(rng);
      int r = idx(rng);
      if (l > r) {
        std::swap(l, r);
      }
      const auto lo = static_cast<size_t>(l);
      const auto hi = static_cast<size_t>(r);

      if ((rng() & 1u) != 0) {
        const long d = val_dist(rng);
        sd.range_apply(lo, hi, d);
        oracle.range_add(lo, hi, d);
      } else {
        ASSERT_EQ(sd.range_query(lo, hi), oracle.range_min(lo, hi))
            << "min trial " << trial << " step " << step << " range [" << lo << ", " << hi << "]";
      }
    }
  }
}
