//===---------------------------------------------------------------------------===//
/**
 * @file Test_Lazy_Segment_Tree.cpp
 * @brief Unit tests for the acted-monoid LazySegmentTree.
 * @version 1.0
 * @date 2026-07-16
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/algebra/Algebra.hpp"
#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/range/Lazy_Segment_Tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

using namespace ads::range;

//===---------------------------------------------------------------------------===//
//=== ++++++++++++++++++++++++ LAZY SEGMENT TREE TESTS ++++++++++++++++++++++++ ===//
//===---------------------------------------------------------------------------===//

class LazySegmentTreeTest : public ::testing::Test {
protected:
  LazySegmentTree<int> tree{std::vector<int>{1, 2, 3, 4, 5}};
};

//===----- LAZY TEST SUPPORT ---------------------------------------------------===//

struct LazySumOracle {
  std::vector<long> values;

  auto range_add(size_t left, size_t right, long delta) -> void {
    for (size_t index = left; index <= right; ++index) {
      values[index] += delta;
    }
  }

  [[nodiscard]] auto range_sum(size_t left, size_t right) const -> long {
    long result = 0;
    for (size_t index = left; index <= right; ++index) {
      result += values[index];
    }
    return result;
  }
};

using LazySumTree = LazySegmentTree<long>;

int g_lazy_algebra_ops_until_throw = -1;

auto lazy_algebra_tick() -> void {
  if (g_lazy_algebra_ops_until_throw >= 0 && g_lazy_algebra_ops_until_throw-- == 0) {
    throw std::runtime_error("lazy algebra fuse blown");
  }
}

struct FragileLazySum {
  using value_monoid_type  = ads::algebra::AddMonoid<long>;
  using action_monoid_type = ads::algebra::AddMonoid<long>;
  using value_type         = long;
  using action_type        = long;

  [[nodiscard]] auto value_identity() const -> long { return 0; }

  [[nodiscard]] auto action_identity() const -> long { return 0; }

  [[nodiscard]] auto combine(const long& left, const long& right) const -> long {
    lazy_algebra_tick();
    return left + right;
  }

  [[nodiscard]] auto compose(const long& existing, const long& fresh) const -> long {
    lazy_algebra_tick();
    return existing + fresh;
  }

  [[nodiscard]] auto apply(const long& value, const long& action, std::size_t length) const -> long {
    lazy_algebra_tick();
    return value + action * static_cast<long>(length);
  }
};

//===----- BASIC LAZY TESTS ----------------------------------------------------===//

TEST(LazySegmentTreeBasicTest, DefaultConstruction) {
  LazySegmentTree<int> segment;
  EXPECT_EQ(segment.size(), 0);
  EXPECT_TRUE(segment.is_empty());
  EXPECT_EQ(segment.total(), 0);
  EXPECT_THROW(static_cast<void>(segment.range_query(0, 0)), InvalidRangeException);
  EXPECT_THROW(segment.range_apply(0, 0, 1), InvalidRangeException);
  EXPECT_THROW(segment.set(0, 1), RangeIndexException);
  EXPECT_THROW(segment.apply(0, 1), RangeIndexException);
  EXPECT_THROW(static_cast<void>(segment.value_at(0)), RangeIndexException);
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

TEST(LazySegmentTreeBasicTest, ConstructionFromDynamicArrayRange) {
  ads::arrays::DynamicArray<int> values{2, 4, 6, 8};
  LazySegmentTree<int>           segment(values.begin(), values.end());

  EXPECT_EQ(segment.size(), 4);
  EXPECT_EQ(segment.total(), 20);
  EXPECT_EQ(segment.range_query(1, 3), 18);
}

TEST_F(LazySegmentTreeTest, RangeQueries) {
  EXPECT_EQ(tree.range_query(0, 4), 15);
  EXPECT_EQ(tree.range_query(1, 3), 9);
  EXPECT_EQ(tree.range_query(2, 2), 3);
}

//===----- RANGE UPDATE TESTS --------------------------------------------------===//

TEST_F(LazySegmentTreeTest, RangeUpdateSingleElement) {
  tree.range_apply(2, 2, 10); // Add 10 to element at index 2
  EXPECT_EQ(tree.range_query(2, 2), 13);
  EXPECT_EQ(tree.total(), 25);
}

TEST_F(LazySegmentTreeTest, RangeUpdateFullRange) {
  tree.range_apply(0, 4, 1);   // Add 1 to all elements
  EXPECT_EQ(tree.total(), 20); // 15 + 5*1 = 20
  EXPECT_EQ(tree.range_query(0, 4), 20);
}

TEST_F(LazySegmentTreeTest, RangeUpdatePartialRange) {
  tree.range_apply(1, 3, 5); // Add 5 to elements 1,2,3
  // Original: [1, 2, 3, 4, 5] -> [1, 7, 8, 9, 5]
  EXPECT_EQ(tree.range_query(1, 3), 24); // 7 + 8 + 9 = 24
  EXPECT_EQ(tree.total(), 30);           // 1 + 7 + 8 + 9 + 5 = 30
}

TEST_F(LazySegmentTreeTest, MultipleRangeUpdates) {
  tree.range_apply(0, 2, 10); // [11, 12, 13, 4, 5]
  tree.range_apply(2, 4, 5);  // [11, 12, 18, 9, 10]

  EXPECT_EQ(tree.range_query(0, 0), 11);
  EXPECT_EQ(tree.range_query(2, 2), 18);
  EXPECT_EQ(tree.range_query(4, 4), 10);
  EXPECT_EQ(tree.total(), 60);
}

TEST(LazySegmentTreeRangeUpdateTest, OverlappingUpdates) {
  LazySegmentTree<int> segment({0, 0, 0, 0, 0});

  segment.range_apply(0, 2, 1); // [1, 1, 1, 0, 0]
  segment.range_apply(1, 3, 2); // [1, 3, 3, 2, 0]
  segment.range_apply(2, 4, 3); // [1, 3, 6, 5, 3]

  EXPECT_EQ(segment.range_query(0, 0), 1);
  EXPECT_EQ(segment.range_query(1, 1), 3);
  EXPECT_EQ(segment.range_query(2, 2), 6);
  EXPECT_EQ(segment.range_query(3, 3), 5);
  EXPECT_EQ(segment.range_query(4, 4), 3);
  EXPECT_EQ(segment.total(), 18);
}

//===----- POINT ACCESS TESTS --------------------------------------------------===//

TEST_F(LazySegmentTreeTest, ValueAtAfterRangeUpdate) {
  tree.range_apply(1, 3, 10);

  EXPECT_EQ(tree.value_at(0), 1);
  EXPECT_EQ(tree.value_at(1), 12);
  EXPECT_EQ(tree.value_at(2), 13);
  EXPECT_EQ(tree.value_at(3), 14);
  EXPECT_EQ(tree.value_at(4), 5);
}

TEST_F(LazySegmentTreeTest, SetAfterRangeUpdatePreservesExistingUpdates) {
  tree.range_apply(1, 3, 10); // [1, 12, 13, 14, 5]
  tree.set(2, 100);           // [1, 12, 100, 14, 5]

  EXPECT_EQ(tree.value_at(0), 1);
  EXPECT_EQ(tree.value_at(1), 12);
  EXPECT_EQ(tree.value_at(2), 100);
  EXPECT_EQ(tree.value_at(3), 14);
  EXPECT_EQ(tree.value_at(4), 5);
  EXPECT_EQ(tree.range_query(1, 3), 126);
  EXPECT_EQ(tree.total(), 132);

  tree.range_apply(0, 4, 1); // [2, 13, 101, 15, 6]
  EXPECT_EQ(tree.value_at(2), 101);
  EXPECT_EQ(tree.total(), 137);
}

//===----- LAZY MOVE TESTS -----------------------------------------------------===//

TEST(LazySegmentTreeMoveTest, MoveConstructor) {
  LazySegmentTree<int> original({1, 2, 3, 4, 5});
  original.range_apply(0, 4, 5);

  LazySegmentTree<int> moved = std::move(original);
  EXPECT_EQ(moved.size(), 5);
  EXPECT_EQ(moved.total(), 40);
  EXPECT_TRUE(original.is_empty());
  EXPECT_EQ(original.total(), 0);

  original.build({5, 5});
  EXPECT_EQ(original.total(), 10);
}

TEST(LazySegmentTreeMoveTest, MoveAssignment) {
  LazySegmentTree<int> original({1, 2, 3});
  LazySegmentTree<int> target;

  target = std::move(original);
  EXPECT_EQ(target.size(), 3);
  EXPECT_EQ(target.total(), 6);
  EXPECT_TRUE(original.is_empty());
  EXPECT_EQ(original.total(), 0);
}

//===----- LAZY ERROR TESTS ----------------------------------------------------===//

TEST(LazySegmentTreeErrorTest, OutOfRangeThrows) {
  LazySegmentTree<int> segment(3);
  EXPECT_THROW(segment.range_apply(0, 3, 1), InvalidRangeException);
  EXPECT_THROW(segment.range_apply(2, 1, 1), InvalidRangeException);
  EXPECT_THROW(static_cast<void>(segment.range_query(2, 1)), InvalidRangeException);
  EXPECT_THROW(static_cast<void>(segment.range_query(0, 3)), InvalidRangeException);
  EXPECT_THROW(segment.set(3, 1), RangeIndexException);
  EXPECT_THROW(segment.apply(3, 1), RangeIndexException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = segment.value_at(5); }, RangeIndexException);
}

//===----- LAZY EDGE CASE TESTS ------------------------------------------------===//

TEST(LazySegmentTreeEdgeCaseTest, SingleElement) {
  LazySegmentTree<int> segment({42});
  EXPECT_EQ(segment.total(), 42);

  segment.range_apply(0, 0, 8);
  EXPECT_EQ(segment.total(), 50);
  EXPECT_EQ(segment.value_at(0), 50);
}

TEST(LazySegmentTreeEdgeCaseTest, LargeRangeUpdate) {
  std::vector<int>     zeros(100, 0);
  LazySegmentTree<int> segment(zeros);

  segment.range_apply(0, 99, 1);
  EXPECT_EQ(segment.total(), 100);
  EXPECT_EQ(segment.range_query(50, 59), 10);
}

TEST(LazySegmentTreeEdgeCaseTest, ClearAndRebuild) {
  LazySegmentTree<int> segment({1, 2, 3});
  segment.range_apply(0, 2, 10);

  segment.clear();
  EXPECT_TRUE(segment.is_empty());
  EXPECT_EQ(segment.total(), 0);

  segment.build({4, 5, 6});
  EXPECT_EQ(segment.size(), 3);
  EXPECT_EQ(segment.total(), 15);
}

TEST(LazySegmentTreeEdgeCaseTest, BuildFromDynamicArrayRangeClearsLazyState) {
  LazySegmentTree<int> segment({1, 1, 1});
  segment.range_apply(0, 2, 5);

  ads::arrays::DynamicArray<int> values{10, 20, 30, 40};
  segment.build(values.begin(), values.end());

  EXPECT_EQ(segment.size(), 4);
  EXPECT_EQ(segment.total(), 100);
  EXPECT_EQ(segment.value_at(0), 10);
  EXPECT_EQ(segment.range_query(1, 2), 50);
}

//===----- CONST QUERY AND POLICY TESTS ----------------------------------------===//

TEST(LazySegmentTreeAdvancedTest, ConstQueriesObservePendingTagsWithoutMutation) {
  const std::vector<long> values{2, 4, 6, 8, 10, 12, 14};
  LazySumTree             tree(values);
  tree.range_apply(1, 5, 7);

  const LazySumTree& view = tree;
  for (int repetition = 0; repetition < 3; ++repetition) {
    EXPECT_EQ(view.range_query(0, 6), 56 + 5 * 7);
    EXPECT_EQ(view.value_at(3), 15);
    EXPECT_EQ(view.value_at(0), 2);
  }

  tree.range_apply(0, 6, 1);
  EXPECT_EQ(tree.range_query(0, 6), 56 + 5 * 7 + 7);
}

TEST(LazySegmentTreeAdvancedTest, AssignmentActionsComposeChronologically) {
  using AssignmentTree = LazySegmentTree<int, ads::algebra::RangeAssignRangeSum<int>>;
  AssignmentTree tree(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8});

  tree.range_apply(1, 4, std::optional<int>(7));
  tree.range_apply(3, 6, std::optional<int>(2));

  const std::vector<int> expected{1, 7, 7, 2, 2, 2, 2, 8};
  int                    expected_total = 0;
  for (size_t index = 0; index < expected.size(); ++index) {
    EXPECT_EQ(tree.value_at(index), expected[index]);
    expected_total += expected[index];
  }
  EXPECT_EQ(tree.total(), expected_total);
}

TEST(LazySegmentTreeAdvancedTest, AffineActionsComposeChronologically) {
  using AffineTree = LazySegmentTree<long, ads::algebra::RangeAffineRangeSum<long>>;
  using Transform  = ads::algebra::AffineTransform<long>;

  std::vector<long> oracle{1, 2, 3, 4, 5, 6};
  AffineTree        tree(oracle);

  tree.range_apply(0, 3, Transform{.scale = 2, .shift = 1});
  for (size_t index = 0; index <= 3; ++index) {
    oracle[index] = 2 * oracle[index] + 1;
  }
  tree.range_apply(2, 5, Transform{.scale = 3, .shift = -2});
  for (size_t index = 2; index <= 5; ++index) {
    oracle[index] = 3 * oracle[index] - 2;
  }

  for (size_t left = 0; left < oracle.size(); ++left) {
    for (size_t right = left; right < oracle.size(); ++right) {
      const long expected =
          std::accumulate(oracle.begin() + static_cast<std::ptrdiff_t>(left), oracle.begin() + static_cast<std::ptrdiff_t>(right + 1), 0L);
      EXPECT_EQ(tree.range_query(left, right), expected);
    }
  }
}

TEST(LazySegmentTreeAdvancedTest, RangeAddRangeMinTracksMinimum) {
  using MinimumTree = LazySegmentTree<long, ads::algebra::RangeAddRangeMin<long>>;
  MinimumTree tree(std::vector<long>{5, 3, 8, 1, 9, 2, 7});

  EXPECT_EQ(tree.range_query(0, 6), 1);
  tree.range_apply(2, 4, 10);
  EXPECT_EQ(tree.range_query(0, 6), 2);
  EXPECT_EQ(tree.range_query(2, 4), 11);
  tree.range_apply(0, 6, -2);
  EXPECT_EQ(tree.range_query(0, 6), 0);
  EXPECT_EQ(tree.value_at(3), 9);
}

//===----- TRANSACTIONAL ROLLBACK TESTS ---------------------------------------===//

TEST(LazySegmentTreeAdvancedTest, ThrowingPolicyRollsBackRangeApplyAtEveryDepth) {
  const std::vector<long> initial{4, 8, 15, 16, 23, 42, 7, 3, 11, 9, 5, 1, 2, 6, 13, 10};

  for (int fuse = 0; fuse < 48; ++fuse) {
    LazySegmentTree<long, FragileLazySum> tree(initial);
    tree.range_apply(2, 12, 5);

    g_lazy_algebra_ops_until_throw = -1;
    std::vector<long> expected;
    expected.reserve(initial.size());
    for (size_t index = 0; index < initial.size(); ++index) {
      expected.push_back(tree.value_at(index));
    }

    g_lazy_algebra_ops_until_throw = fuse;
    bool threw                     = false;
    try {
      tree.range_apply(1, 14, 3);
    } catch (const std::runtime_error&) {
      threw = true;
    }
    g_lazy_algebra_ops_until_throw = -1;

    if (!threw) {
      for (size_t index = 1; index <= 14; ++index) {
        expected[index] += 3;
      }
    }
    for (size_t index = 0; index < expected.size(); ++index) {
      EXPECT_EQ(tree.value_at(index), expected[index]) << "fuse=" << fuse << " index=" << index;
    }
  }
}

TEST(LazySegmentTreeAdvancedTest, ThrowingPolicyRollsBackPointSetAtEveryDepth) {
  const std::vector<long> initial{9, 7, 5, 3, 1, 2, 4, 6, 8, 10, 12, 14};

  for (int fuse = 0; fuse < 32; ++fuse) {
    LazySegmentTree<long, FragileLazySum> tree(initial);
    tree.range_apply(0, 11, 2);

    g_lazy_algebra_ops_until_throw = -1;
    std::vector<long> expected;
    expected.reserve(initial.size());
    for (size_t index = 0; index < initial.size(); ++index) {
      expected.push_back(tree.value_at(index));
    }

    g_lazy_algebra_ops_until_throw = fuse;
    bool threw                     = false;
    try {
      tree.set(7, 99);
    } catch (const std::runtime_error&) {
      threw = true;
    }
    g_lazy_algebra_ops_until_throw = -1;

    if (!threw) {
      expected[7] = 99;
    }
    for (size_t index = 0; index < expected.size(); ++index) {
      EXPECT_EQ(tree.value_at(index), expected[index]) << "fuse=" << fuse << " index=" << index;
    }
  }
}

//===----- RANDOMIZED ORACLE TEST ---------------------------------------------===//

TEST(LazySegmentTreeAdvancedTest, InterleavedOperationsMatchOracle) {
  constexpr size_t kSize = 37;
  LazySumOracle    oracle{std::vector<long>(kSize, 0)};
  for (size_t index = 0; index < kSize; ++index) {
    oracle.values[index] = static_cast<long>((index * 7) % 13) - 6;
  }
  LazySumTree tree(oracle.values);

  unsigned state = 12'345;
  auto     next  = [&state]() -> unsigned int {
    state = state * 1'664'525U + 1'013'904'223U;
    return state >> 8;
  };

  for (int step = 0; step < 200; ++step) {
    const size_t first  = next() % kSize;
    const size_t second = next() % kSize;
    const size_t left   = std::min(first, second);
    const size_t right  = std::max(first, second);

    switch (step % 4) {
    case 0: {
      const long delta = static_cast<long>(next() % 21) - 10;
      tree.range_apply(left, right, delta);
      oracle.range_add(left, right, delta);
      break;
    }
    case 1: {
      const long value = static_cast<long>(next() % 101) - 50;
      tree.set(left, value);
      oracle.values[left] = value;
      break;
    }
    case 2:
      ASSERT_EQ(tree.range_query(left, right), oracle.range_sum(left, right)) << "step=" << step;
      break;
    default:
      ASSERT_EQ(tree.value_at(right), oracle.values[right]) << "step=" << step;
      break;
    }
  }

  EXPECT_EQ(tree.total(), oracle.range_sum(0, kSize - 1));
  for (size_t index = 0; index < kSize; ++index) {
    EXPECT_EQ(tree.value_at(index), oracle.values[index]);
  }
}

//===---------------------------------------------------------------------------===//
