//===---------------------------------------------------------------------------===//
/**
 * @file Test_Algebra.cpp
 * @brief Algebra-law and cross-structure integration tests for shared policies.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/algebra/Algebra.hpp"
#include "ads/range/Fenwick_Tree.hpp"
#include "ads/range/Lazy_Segment_Tree.hpp"
#include "ads/range/Segment_Tree.hpp"
#include "ads/range/Sqrt_Decomposition.hpp"

#include <gtest/gtest.h>

#include <numeric>
#include <random>
#include <string>
#include <vector>

namespace {

using namespace ads;

struct ConcatMonoid {
  using value_type = std::string;

  [[nodiscard]] auto identity() const -> std::string { return {}; }

  [[nodiscard]] auto combine(const std::string& left, const std::string& right) const -> std::string { return left + right; }
};

struct ShiftedAddGroup {
  using value_type = int;

  static constexpr bool is_commutative = true;

  explicit ShiftedAddGroup(int origin) : origin_(origin) {}

  [[nodiscard]] auto identity() const -> int { return origin_; }

  [[nodiscard]] auto combine(int left, int right) const -> int { return left + right - origin_; }

  [[nodiscard]] auto inverse(int value) const -> int { return 2 * origin_ - value; }

private:
  int origin_;
};

struct ScaledRangeAddApplicator {
  explicit ScaledRangeAddApplicator(int factor) : factor_(factor) {}

  [[nodiscard]] auto apply(int value, int action, std::size_t length) const -> int {
    return value + factor_ * action * static_cast<int>(length);
  }

private:
  int factor_;
};

using StatefulActed = algebra::ActedMonoid<algebra::AddMonoid<int>, algebra::AddMonoid<int>, ScaledRangeAddApplicator>;

static_assert(algebra::MonoidPolicy<algebra::AddMonoid<int>>);
static_assert(algebra::CommutativeGroupPolicy<algebra::AddGroup<int>>);
static_assert(algebra::IdempotentSemigroupPolicy<algebra::MinSemigroup<int>>);
static_assert(algebra::IdempotentSemigroupPolicy<algebra::GcdMonoid<int>>);
static_assert(algebra::ActedMonoidPolicy<algebra::RangeAffineRangeSum<long long>>);
static_assert(algebra::CommutativeGroupPolicy<ShiftedAddGroup>);
static_assert(algebra::ActedMonoidPolicy<StatefulActed>);

TEST(AlgebraLaws, AdditiveMonoidAndGroup) {
  const algebra::AddMonoid<int> monoid;
  const algebra::AddGroup<int>  group;

  for (int a : {-8, 0, 3}) {
    EXPECT_EQ(monoid.combine(monoid.identity(), a), a);
    EXPECT_EQ(monoid.combine(a, monoid.identity()), a);
    EXPECT_EQ(group.combine(a, group.inverse(a)), group.identity());
    EXPECT_EQ(group.combine(group.inverse(a), a), group.identity());
    for (int b : {-5, 0, 7}) {
      EXPECT_EQ(monoid.combine(a, b), monoid.combine(b, a));
      for (int c : {-4, 0, 9}) {
        EXPECT_EQ(monoid.combine(monoid.combine(a, b), c), monoid.combine(a, monoid.combine(b, c)));
      }
    }
  }
}

TEST(AlgebraLaws, IdempotentPolicies) {
  const algebra::MinSemigroup<int> minimum;
  const algebra::MaxSemigroup<int> maximum;
  const algebra::GcdMonoid<int>    gcd;

  for (int value : {0, 1, 7, 42}) {
    EXPECT_EQ(minimum.combine(value, value), value);
    EXPECT_EQ(maximum.combine(value, value), value);
    EXPECT_EQ(gcd.combine(value, value), value);
  }
  EXPECT_EQ(gcd.combine(gcd.identity(), 18), 18);
  EXPECT_EQ(gcd.combine(18, gcd.identity()), 18);
}

TEST(AlgebraLaws, AssignmentCompositionIsChronological) {
  const algebra::RangeAssignRangeSum<int> acted;
  const std::optional<int>                assign_four = 4;
  const std::optional<int>                assign_nine = 9;

  EXPECT_EQ(acted.compose(assign_four, assign_nine), assign_nine);
  EXPECT_EQ(acted.compose(assign_four, acted.action_identity()), assign_four);
  EXPECT_EQ(acted.apply(21, assign_four, 3), 12);
  EXPECT_EQ(acted.apply(acted.apply(21, assign_four, 3), assign_nine, 3), acted.apply(21, acted.compose(assign_four, assign_nine), 3));
}

TEST(AlgebraLaws, AffineCompositionIsChronological) {
  const algebra::RangeAffineRangeSum<long long> acted;
  const algebra::AffineTransform<long long>     existing{.scale = 2, .shift = 3};
  const algebra::AffineTransform<long long>     fresh{.scale = 5, .shift = 7};
  const auto                                    composed = acted.compose(existing, fresh);

  EXPECT_EQ(composed, (algebra::AffineTransform<long long>{10, 22}));
  EXPECT_EQ(acted.apply(acted.apply(6, existing, 3), fresh, 3), acted.apply(6, composed, 3));
  EXPECT_EQ(acted.apply(acted.combine(4, 9), fresh, 5), acted.combine(acted.apply(4, fresh, 2), acted.apply(9, fresh, 3)));
}

TEST(AlgebraIntegration, SegmentTreePreservesNonCommutativeOrder) {
  range::SegmentTree<std::string, ConcatMonoid> tree({"a", "b", "c", "d", "e"});

  EXPECT_EQ(tree.total(), "abcde");
  EXPECT_EQ(tree.range_query(1, 3), "bcd");
  tree.set(2, "X");
  EXPECT_EQ(tree.total(), "abXde");
  EXPECT_EQ(tree.range_query(1, 4), "bXde");
}

TEST(AlgebraIntegration, StatefulGroupDrivesFenwickTree) {
  const ShiftedAddGroup                    group(10);
  range::FenwickTree<int, ShiftedAddGroup> tree({11, 12, 13}, group);

  EXPECT_EQ(tree.prefix_query(0), 11);
  EXPECT_EQ(tree.range_query(1, 2), 15);
  EXPECT_EQ(tree.total(), 16);

  tree.apply(1, 12); // Group delta 12 is an ordinary +2 shift around identity 10.
  EXPECT_EQ(tree.value_at(1), 14);
  EXPECT_EQ(tree.total(), 18);
  tree.set(0, 20);
  EXPECT_EQ(tree.total(), 27);
}

TEST(AlgebraIntegration, StatefulActedMonoidIsSharedByLazyStructures) {
  StatefulActed          policy(algebra::AddMonoid<int>{}, algebra::AddMonoid<int>{}, ScaledRangeAddApplicator(2));
  const std::vector<int> values = {1, 2, 3, 4};

  range::LazySegmentTree<int, StatefulActed>   lazy(values, policy);
  range::SqrtDecomposition<int, StatefulActed> sqrt(values, policy);

  lazy.range_apply(1, 3, 5);
  sqrt.range_apply(1, 3, 5);
  EXPECT_EQ(lazy.total(), 40);
  EXPECT_EQ(sqrt.total(), 40);
  for (std::size_t index = 0; index < values.size(); ++index) {
    EXPECT_EQ(lazy.value_at(index), sqrt.value_at(index));
  }
}

TEST(AlgebraIntegration, AffinePolicyProducesIdenticalLazyStructures) {
  using Value  = long long;
  using Policy = algebra::RangeAffineRangeSum<Value>;

  std::mt19937                  random(20'260'715); // NOLINT(bugprone-random-generator-seed): deterministic oracle test.
  std::uniform_int_distribution value_distribution(-20, 20);
  std::uniform_int_distribution index_distribution(0, 63);
  std::uniform_int_distribution scale_distribution(-1, 1);

  std::vector<Value> oracle(64);
  for (Value& value : oracle) {
    value = value_distribution(random);
  }

  range::LazySegmentTree<Value, Policy>   lazy(oracle);
  range::SqrtDecomposition<Value, Policy> sqrt(oracle);

  for (int step = 0; step < 300; ++step) {
    auto left  = static_cast<std::size_t>(index_distribution(random));
    auto right = static_cast<std::size_t>(index_distribution(random));
    if (left > right) {
      std::swap(left, right);
    }

    if ((random() & 1U) == 0U) {
      const algebra::AffineTransform<Value> action{
          .scale = scale_distribution(random),
          .shift = value_distribution(random),
      };
      lazy.range_apply(left, right, action);
      sqrt.range_apply(left, right, action);
      for (std::size_t index = left; index <= right; ++index) {
        oracle[index] = oracle[index] * action.scale + action.shift;
      }
    } else {
      const Value expected = std::accumulate(
          oracle.begin() + static_cast<std::ptrdiff_t>(left), oracle.begin() + static_cast<std::ptrdiff_t>(right + 1), Value{});
      EXPECT_EQ(lazy.range_query(left, right), expected);
      EXPECT_EQ(sqrt.range_query(left, right), expected);
    }
  }

  EXPECT_EQ(lazy.total(), std::accumulate(oracle.begin(), oracle.end(), Value{}));
  EXPECT_EQ(sqrt.to_vector(), oracle);
}

} // namespace

//===---------------------------------------------------------------------------===//
