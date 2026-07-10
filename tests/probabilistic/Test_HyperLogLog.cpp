//===---------------------------------------------------------------------------===//
/**
 * @file Test_HyperLogLog.cpp
 * @brief Google Test unit tests for the HyperLogLog cardinality estimator.
 * @version 0.1
 * @date 2026-07-10
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/probabilistic/HyperLogLog.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_set>

using ads::probabilistic::HyperLogLog;
using ads::probabilistic::HyperLogLogException;

namespace {

using HLL = HyperLogLog<int>;

struct HllSeededHash {
  std::uint64_t seed = 0;

  [[nodiscard]] auto operator()(int value) const noexcept -> std::size_t {
    const auto base = static_cast<std::uint64_t>(std::hash<int>{}(value));
    return static_cast<std::size_t>(base ^ (seed + 0x9e37'79b9'7f4a'7c15ULL + (base << 6U) + (base >> 2U)));
  }

  [[nodiscard]] auto operator==(const HllSeededHash&) const -> bool = default;
};

[[nodiscard]] constexpr auto undo_hll_xor_shift(std::uint64_t value, unsigned shift) noexcept -> std::uint64_t {
  for (unsigned offset = shift; offset < 64U; offset *= 2U) {
    value ^= value >> offset;
  }
  return value;
}

[[nodiscard]] constexpr auto unmix_hll_hash(std::uint64_t value) noexcept -> std::uint64_t {
  value = undo_hll_xor_shift(value, 31U);
  value *= 0x3196'42b2'd24d'8ec3ULL;
  value = undo_hll_xor_shift(value, 27U);
  value *= 0x96de'1b17'3f11'9089ULL;
  value = undo_hll_xor_shift(value, 30U);
  return value - 0x9e37'79b9'7f4a'7c15ULL;
}

struct HllExtremeHash {
  [[nodiscard]] auto operator()(int value) const noexcept -> std::size_t {
    const auto bucket = static_cast<std::uint64_t>(value & 0x0F);
    return static_cast<std::size_t>(unmix_hll_hash(bucket << 60U));
  }
};

// Relative deviation of an estimate from the truth.
[[nodiscard]] auto hll_rel_error(std::size_t estimate, std::size_t truth) -> double {
  if (truth == 0) {
    return estimate == 0 ? 0.0 : 1.0;
  }
  return std::fabs(static_cast<double>(estimate) - static_cast<double>(truth)) / static_cast<double>(truth);
}

} // namespace

//===----- TYPE-TRAIT CHECKS ---------------------------------------------------===//

static_assert(std::is_move_constructible_v<HLL>);
static_assert(!std::is_copy_constructible_v<HLL>);
static_assert(!std::is_copy_assignable_v<HLL>);

//===----- CONSTRUCTION & VALIDATION -------------------------------------------===//

TEST(HyperLogLogBasic, DefaultConstruction) {
  HLL sketch;
  EXPECT_EQ(sketch.precision(), 14U);
  EXPECT_EQ(sketch.register_count(), 1U << 14U);
  EXPECT_EQ(sketch.estimate(), 0U);
}

TEST(HyperLogLogBasic, CustomPrecision) {
  HLL sketch(10);
  EXPECT_EQ(sketch.precision(), 10U);
  EXPECT_EQ(sketch.register_count(), 1'024U);
}

TEST(HyperLogLogBasic, OutOfRangePrecisionThrows) {
  EXPECT_THROW(HLL(3), HyperLogLogException);
  EXPECT_THROW(HLL(19), HyperLogLogException);
}

TEST(HyperLogLogBasic, RelativeErrorMatchesRegisters) {
  HLL          sketch(14);
  const double expected = 1.04 / std::sqrt(static_cast<double>(1U << 14U));
  EXPECT_NEAR(sketch.relative_error(), expected, 1e-12);
}

TEST(HyperLogLogBasic, FromErrorSizesPrecision) {
  // precision = ceil(2*log2(1.04/error)); error 0.01 -> ceil(2*log2(104)) = ceil(13.4) = 14.
  auto sketch = HLL::from_error(0.01);
  EXPECT_EQ(sketch.precision(), 14U);
}

TEST(HyperLogLogBasic, FromErrorClampsAndRejects) {
  EXPECT_EQ(HLL::from_error(0.5).precision(), 4U);   // clamps to minimum
  EXPECT_EQ(HLL::from_error(1e-6).precision(), 18U); // clamps to maximum
  EXPECT_THROW((void)HLL::from_error(0.0), HyperLogLogException);
  EXPECT_THROW((void)HLL::from_error(1.0), HyperLogLogException);
}

//===----- COUNTING BEHAVIOUR --------------------------------------------------===//

TEST(HyperLogLogCounting, DuplicatesDoNotInflate) {
  HLL sketch(14);
  for (int i = 0; i < 10'000; ++i) {
    sketch.add(42); // one distinct key added many times
  }
  EXPECT_LE(sketch.estimate(), 3U); // essentially 1
}

TEST(HyperLogLogCounting, SmallCardinalityIsAccurate) {
  // Linear counting keeps small cardinalities sharp.
  HLL sketch(14);
  for (int i = 0; i < 50; ++i) {
    sketch.add(i);
  }
  EXPECT_LE(hll_rel_error(sketch.estimate(), 50), 0.05);
}

TEST(HyperLogLogCounting, MediumCardinalityWithinErrorBound) {
  HLL       sketch(14);
  const int distinct = 50'000;
  for (int i = 0; i < distinct; ++i) {
    sketch.add(i);
  }
  // Three standard errors gives ample slack for a single deterministic run.
  const double tolerance = 3.0 * sketch.relative_error();
  EXPECT_LE(hll_rel_error(sketch.estimate(), distinct), tolerance);
}

TEST(HyperLogLogCounting, RepeatedStreamMatchesDistinctCount) {
  HLL                     sketch(14);
  std::unordered_set<int> oracle;
  for (int pass = 0; pass < 3; ++pass) {
    for (int i = 0; i < 20'000; ++i) {
      const int k = (i * 2'654'435'761U) % 40'000; // scrambled keys, with repeats across passes
      sketch.add(k);
      oracle.insert(k);
    }
  }
  const double tolerance = 3.0 * sketch.relative_error();
  EXPECT_LE(hll_rel_error(sketch.estimate(), oracle.size()), tolerance);
}

TEST(HyperLogLogCounting, LowPrecisionHasLargerError) {
  // A coarser sketch still lands in the right ballpark for a big stream.
  HLL       sketch(8);
  const int distinct = 100'000;
  for (int i = 0; i < distinct; ++i) {
    sketch.add(i);
  }
  EXPECT_LE(hll_rel_error(sketch.estimate(), distinct), 3.0 * sketch.relative_error());
}

TEST(HyperLogLogCounting, EstimateSaturatesAtSizeTypeMax) {
  if constexpr (sizeof(std::size_t) >= sizeof(std::uint64_t)) {
    HyperLogLog<int, HllExtremeHash> sketch(4, HllExtremeHash{});
    for (int bucket = 0; bucket < 16; ++bucket) {
      sketch.add(bucket);
    }
    EXPECT_EQ(sketch.estimate(), std::numeric_limits<std::size_t>::max());
  }
}

//===----- MERGE ---------------------------------------------------------------===//

TEST(HyperLogLogMerge, UnionOfDisjointStreams) {
  HLL a(14);
  HLL b(14);
  for (int i = 0; i < 30'000; ++i) {
    a.add(i);
  }
  for (int i = 30'000; i < 50'000; ++i) {
    b.add(i);
  }
  a.merge(b);
  EXPECT_LE(hll_rel_error(a.estimate(), 50'000), 3.0 * a.relative_error());
}

TEST(HyperLogLogMerge, OverlapCountedOnce) {
  HLL a(14);
  HLL b(14);
  for (int i = 0; i < 40'000; ++i) {
    a.add(i);
  }
  for (int i = 20'000; i < 60'000; ++i) {
    b.add(i); // 20000..39999 overlap with a
  }
  a.merge(b);
  // Union is 0..59999 -> 60000 distinct.
  EXPECT_LE(hll_rel_error(a.estimate(), 60'000), 3.0 * a.relative_error());
}

TEST(HyperLogLogMerge, IncompatiblePrecisionThrows) {
  HLL a(14);
  HLL b(12);
  EXPECT_THROW(a.merge(b), HyperLogLogException);
}

TEST(HyperLogLogMerge, IncompatibleHashFamilyThrows) {
  HyperLogLog<int, HllSeededHash> a(12, HllSeededHash{1});
  HyperLogLog<int, HllSeededHash> b(12, HllSeededHash{2});
  EXPECT_THROW(a.merge(b), HyperLogLogException);
}

TEST(HyperLogLogMerge, EqualStatefulHashFamilyMerges) {
  HyperLogLog<int, HllSeededHash> a(14, HllSeededHash{7});
  HyperLogLog<int, HllSeededHash> b(14, HllSeededHash{7});
  for (int i = 0; i < 5'000; ++i) {
    a.add(i);
  }
  for (int i = 5'000; i < 10'000; ++i) {
    b.add(i);
  }
  EXPECT_NO_THROW(a.merge(b));
  EXPECT_LE(hll_rel_error(a.estimate(), 10'000), 3.0 * a.relative_error());
}

//===----- CLEAR & MOVE --------------------------------------------------------===//

TEST(HyperLogLogLifecycle, ClearResetsAndReuses) {
  HLL sketch(12);
  for (int i = 0; i < 5'000; ++i) {
    sketch.add(i);
  }
  sketch.clear();
  EXPECT_EQ(sketch.estimate(), 0U);
  for (int i = 0; i < 100; ++i) {
    sketch.add(i);
  }
  EXPECT_LE(hll_rel_error(sketch.estimate(), 100), 0.1);
}

TEST(HyperLogLogLifecycle, MoveTransfersState) {
  HLL src(12);
  for (int i = 0; i < 3'000; ++i) {
    src.add(i);
  }
  const auto expected = src.estimate();

  HLL dst(std::move(src));
  EXPECT_EQ(dst.estimate(), expected);
  EXPECT_EQ(src.precision(), 0U);
  EXPECT_EQ(src.register_count(), 0U);
  EXPECT_EQ(src.relative_error(), 0.0);
  EXPECT_EQ(src.estimate(), 0U);
  EXPECT_THROW(src.add(1), HyperLogLogException);

  HLL dst2(8);
  dst2 = std::move(dst);
  EXPECT_EQ(dst2.estimate(), expected);
  EXPECT_EQ(dst.estimate(), 0U);

  src = HLL(8);
  src.add(1);
  EXPECT_GT(src.estimate(), 0U);
}

//===----- ALTERNATE TYPES -----------------------------------------------------===//

TEST(HyperLogLogTypes, StringKeys) {
  HyperLogLog<std::string>        sketch(14);
  std::unordered_set<std::string> oracle;
  for (int i = 0; i < 10'000; ++i) {
    std::string key = "item-" + std::to_string(i);
    sketch.add(key);
    oracle.insert(key);
  }
  EXPECT_LE(hll_rel_error(sketch.estimate(), oracle.size()), 3.0 * sketch.relative_error());
}

//===---------------------------------------------------------------------------===//
