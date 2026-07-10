//===---------------------------------------------------------------------------===//
/**
 * @file Test_Count_Min_Sketch.cpp
 * @brief Google Test unit tests for the CountMinSketch frequency estimator.
 * @version 0.1
 * @date 2026-07-10
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/probabilistic/Count_Min_Sketch.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <random>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

using ads::probabilistic::CountMinSketch;
using ads::probabilistic::CountMinSketchException;

namespace {

using Sketch = CountMinSketch<int>;

struct SeededHash {
  std::uint64_t seed = 0;

  [[nodiscard]] auto operator()(int value) const noexcept -> std::size_t {
    const auto base = static_cast<std::uint64_t>(std::hash<int>{}(value));
    return static_cast<std::size_t>(base ^ (seed + 0x9e37'79b9'7f4a'7c15ULL + (base << 6U) + (base >> 2U)));
  }

  [[nodiscard]] auto operator==(const SeededHash&) const -> bool = default;
};

} // namespace

//===----- TYPE-TRAIT CHECKS ---------------------------------------------------===//

static_assert(std::is_move_constructible_v<Sketch>);
static_assert(!std::is_copy_constructible_v<Sketch>);
static_assert(!std::is_copy_assignable_v<Sketch>);

//===----- CONSTRUCTION & VALIDATION -------------------------------------------===//

TEST(CountMinSketchBasic, EmptyEstimatesAreZero) {
  Sketch sketch(256, 4);
  EXPECT_EQ(sketch.width(), 256U);
  EXPECT_EQ(sketch.depth(), 4U);
  EXPECT_EQ(sketch.total_count(), 0U);
  EXPECT_EQ(sketch.estimate(42), 0U);
}

TEST(CountMinSketchBasic, ZeroWidthOrDepthThrows) {
  EXPECT_THROW(Sketch(0, 4), CountMinSketchException);
  EXPECT_THROW(Sketch(256, 0), CountMinSketchException);
}

TEST(CountMinSketchBasic, OversizedTableThrowsBeforeAllocation) {
  constexpr auto kTooWide = std::numeric_limits<std::size_t>::max() / 2U + 1U;
  EXPECT_THROW(Sketch(kTooWide, 2), CountMinSketchException);
}

TEST(CountMinSketchBasic, FromEstimatesSizesTable) {
  // width = ceil(e/epsilon), depth = ceil(ln(1/delta)).
  auto sketch = Sketch::from_estimates(/*epsilon=*/0.01, /*delta=*/0.01);
  EXPECT_EQ(sketch.width(), 272U); // ceil(2.71828.../0.01)
  EXPECT_EQ(sketch.depth(), 5U);   // ceil(ln(100)) = ceil(4.60...) = 5
}

TEST(CountMinSketchBasic, FromEstimatesRejectsOutOfRange) {
  EXPECT_THROW((void)Sketch::from_estimates(0.0, 0.5), CountMinSketchException);
  EXPECT_THROW((void)Sketch::from_estimates(1.0, 0.5), CountMinSketchException);
  EXPECT_THROW((void)Sketch::from_estimates(0.5, 0.0), CountMinSketchException);
  EXPECT_THROW((void)Sketch::from_estimates(0.5, 1.0), CountMinSketchException);
}

TEST(CountMinSketchBasic, FromEstimatesRejectsOversizedTable) {
  EXPECT_THROW((void)Sketch::from_estimates(std::numeric_limits<double>::min(), 0.5), CountMinSketchException);
}

//===----- EXACT / BASIC COUNTING ----------------------------------------------===//

TEST(CountMinSketchCounting, SingleKeyIsExact) {
  Sketch sketch(256, 4);
  for (int i = 0; i < 1'000; ++i) {
    sketch.add(7);
  }
  // With only one distinct key nothing can inflate its counters.
  EXPECT_EQ(sketch.estimate(7), 1'000U);
  EXPECT_EQ(sketch.total_count(), 1'000U);
}

TEST(CountMinSketchCounting, WeightedAddsAccumulate) {
  Sketch sketch(256, 4);
  sketch.add(3, 5);
  sketch.add(3, 8);
  EXPECT_EQ(sketch.estimate(3), 13U);
  EXPECT_EQ(sketch.total_count(), 13U);
}

TEST(CountMinSketchCounting, CounterOverflowThrowsWithoutMutation) {
  CountMinSketch<int, std::hash<int>, std::uint8_t> sketch(1, 2);
  sketch.add(7, std::numeric_limits<std::uint8_t>::max());

  EXPECT_THROW(sketch.add(7), CountMinSketchException);
  EXPECT_EQ(sketch.estimate(7), std::numeric_limits<std::uint8_t>::max());
  EXPECT_EQ(sketch.total_count(), std::numeric_limits<std::uint8_t>::max());
}

//===----- PROBABILISTIC GUARANTEES --------------------------------------------===//

TEST(CountMinSketchGuarantee, NeverUnderestimates) {
  Sketch                             sketch(512, 5);
  std::unordered_map<int, uint64_t>  oracle;
  std::mt19937                       rng(2'024);
  std::uniform_int_distribution<int> key(0, 2'000);

  for (int i = 0; i < 20'000; ++i) {
    int k = key(rng);
    sketch.add(k);
    ++oracle[k];
  }

  // The core guarantee: the estimate is never below the true frequency.
  for (const auto& [k, true_count] : oracle) {
    EXPECT_GE(sketch.estimate(k), true_count) << "underestimate for key " << k;
  }
}

TEST(CountMinSketchGuarantee, OverestimateWithinErrorBound) {
  // With epsilon/delta chosen up front, the additive error is <= epsilon*N.
  const double epsilon = 0.01;
  auto         sketch  = Sketch::from_estimates(epsilon, /*delta=*/0.01);

  std::unordered_map<int, uint64_t>  oracle;
  std::mt19937                       rng(777);
  std::uniform_int_distribution<int> key(0, 5'000);

  constexpr int kOps = 20'000;
  for (int i = 0; i < kOps; ++i) {
    int k = key(rng);
    sketch.add(k);
    ++oracle[k];
  }

  const auto bound = static_cast<uint64_t>(epsilon * static_cast<double>(kOps));
  for (const auto& [k, true_count] : oracle) {
    const uint64_t est = sketch.estimate(k);
    EXPECT_GE(est, true_count);
    EXPECT_LE(est - true_count, bound) << "error bound exceeded for key " << k;
  }
}

//===----- MERGE ---------------------------------------------------------------===//

TEST(CountMinSketchMerge, ElementWiseSum) {
  Sketch a(256, 4);
  Sketch b(256, 4);
  a.add(1, 3);
  a.add(2, 4);
  b.add(2, 5);
  b.add(3, 6);

  a.merge(b);
  EXPECT_EQ(a.total_count(), 18U);
  EXPECT_EQ(a.estimate(1), 3U);
  EXPECT_EQ(a.estimate(2), 9U);
  EXPECT_EQ(a.estimate(3), 6U);
}

TEST(CountMinSketchMerge, IncompatibleLayoutThrows) {
  Sketch a(256, 4);
  Sketch b(128, 4);
  Sketch c(256, 3);
  EXPECT_THROW(a.merge(b), CountMinSketchException);
  EXPECT_THROW(a.merge(c), CountMinSketchException);
}

TEST(CountMinSketchMerge, IncompatibleHashFamilyThrows) {
  CountMinSketch<int, SeededHash> a(256, 4, SeededHash{1});
  CountMinSketch<int, SeededHash> b(256, 4, SeededHash{2});
  EXPECT_THROW(a.merge(b), CountMinSketchException);
}

TEST(CountMinSketchMerge, EqualStatefulHashFamilyMerges) {
  CountMinSketch<int, SeededHash> a(256, 4, SeededHash{7});
  CountMinSketch<int, SeededHash> b(256, 4, SeededHash{7});
  a.add(5, 3);
  b.add(5, 4);

  a.merge(b);
  EXPECT_EQ(a.estimate(5), 7U);
}

TEST(CountMinSketchMerge, CounterOverflowThrowsWithoutMutation) {
  using NarrowSketch = CountMinSketch<int, std::hash<int>, std::uint8_t>;
  NarrowSketch a(1, 2);
  NarrowSketch b(1, 2);
  a.add(7, 250);
  b.add(7, 10);

  EXPECT_THROW(a.merge(b), CountMinSketchException);
  EXPECT_EQ(a.estimate(7), 250U);
  EXPECT_EQ(a.total_count(), 250U);
}

TEST(CountMinSketchMerge, MatchesSingleSketchOverStream) {
  // Splitting a stream across two sketches and merging equals one sketch fed the
  // whole stream (both never underestimate; the merged one must agree).
  Sketch whole(512, 5);
  Sketch part1(512, 5);
  Sketch part2(512, 5);

  std::mt19937                       rng(99);
  std::uniform_int_distribution<int> key(0, 1'000);
  for (int i = 0; i < 8'000; ++i) {
    int k = key(rng);
    whole.add(k);
    (i % 2 == 0 ? part1 : part2).add(k);
  }
  part1.merge(part2);

  std::mt19937                       qrng(1'234);
  std::uniform_int_distribution<int> qkey(0, 1'000);
  for (int i = 0; i < 200; ++i) {
    int k = qkey(qrng);
    EXPECT_EQ(part1.estimate(k), whole.estimate(k));
  }
  EXPECT_EQ(part1.total_count(), whole.total_count());
}

//===----- CLEAR & MOVE --------------------------------------------------------===//

TEST(CountMinSketchLifecycle, ClearResetsAndReuses) {
  Sketch sketch(128, 3);
  sketch.add(5, 10);
  sketch.clear();
  EXPECT_EQ(sketch.total_count(), 0U);
  EXPECT_EQ(sketch.estimate(5), 0U);
  sketch.add(5, 2);
  EXPECT_EQ(sketch.estimate(5), 2U);
}

TEST(CountMinSketchLifecycle, MoveTransfersState) {
  Sketch src(128, 3);
  src.add(9, 7);
  Sketch dst(std::move(src));
  EXPECT_EQ(dst.estimate(9), 7U);
  EXPECT_EQ(dst.total_count(), 7U);
  EXPECT_EQ(src.width(), 0U);
  EXPECT_EQ(src.depth(), 0U);
  EXPECT_EQ(src.total_count(), 0U);
  EXPECT_EQ(src.estimate(9), 0U);
  EXPECT_THROW(src.add(9), CountMinSketchException);

  Sketch dst2(64, 2);
  dst2 = std::move(dst);
  EXPECT_EQ(dst2.estimate(9), 7U);
  EXPECT_EQ(dst.estimate(9), 0U);

  src = Sketch(32, 2);
  src.add(9, 3);
  EXPECT_EQ(src.estimate(9), 3U);
}

//===----- ALTERNATE TYPES -----------------------------------------------------===//

TEST(CountMinSketchTypes, StringKeys) {
  CountMinSketch<std::string> sketch(256, 4);
  sketch.add("apple", 3);
  sketch.add("banana");
  sketch.add("apple");
  EXPECT_EQ(sketch.estimate("apple"), 4U);
  EXPECT_EQ(sketch.estimate("banana"), 1U);
  EXPECT_EQ(sketch.estimate("cherry"), 0U);
}

TEST(CountMinSketchTypes, NarrowCounterType) {
  CountMinSketch<int, std::hash<int>, std::uint32_t> sketch(256, 4);
  sketch.add(1, 100);
  sketch.add(1, 50);
  EXPECT_EQ(sketch.estimate(1), 150U);
  static_assert(std::is_same_v<decltype(sketch.estimate(1)), std::uint32_t>);
}

//===---------------------------------------------------------------------------===//
