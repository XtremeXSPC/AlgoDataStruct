//===---------------------------------------------------------------------------===//
/**
 * @file Test_Cuckoo_Filter.cpp
 * @brief Google Test unit tests for the CuckooFilter probabilistic set.
 * @version 0.1
 * @date 2026-07-12
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/probabilistic/Cuckoo_Filter.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

using ads::probabilistic::CuckooFilter;
using ads::probabilistic::CuckooFilterException;

namespace {

using CF = CuckooFilter<int>;

// Stateful hash used to exercise the non-empty hash-functor path. Named uniquely
// to stay clear of the other probabilistic tests under the unity build.
struct CuckooSeededHash {
  std::uint64_t seed = 0;

  [[nodiscard]] auto operator()(int value) const noexcept -> std::size_t {
    const auto base = static_cast<std::uint64_t>(std::hash<int>{}(value));
    return static_cast<std::size_t>(base ^ (seed + 0x9e37'79b9'7f4a'7c15ULL + (base << 6U) + (base >> 2U)));
  }

  [[nodiscard]] auto operator==(const CuckooSeededHash&) const -> bool = default;
};

struct CuckooRawHash {
  std::uint64_t raw = 0;

  [[nodiscard]] auto operator()(int) const noexcept -> std::size_t { return static_cast<std::size_t>(raw); }
};

struct CuckooThrowingMoveHash {
  static inline bool throw_on_move_assignment = false;

  std::uint64_t seed = 0;

  CuckooThrowingMoveHash() = default;

  explicit CuckooThrowingMoveHash(std::uint64_t value) : seed(value) {}

  CuckooThrowingMoveHash(CuckooThrowingMoveHash&&) noexcept = default;

  auto operator=(CuckooThrowingMoveHash&& other) -> CuckooThrowingMoveHash& {
    if (throw_on_move_assignment) {
      throw std::runtime_error("move assignment failed");
    }
    seed = other.seed;
    return *this;
  }

  [[nodiscard]] auto operator()(int value) const noexcept -> std::size_t {
    return static_cast<std::size_t>(static_cast<std::uint64_t>(value) ^ seed);
  }
};

[[nodiscard]] constexpr auto cuckoo_test_mix(std::uint64_t value) noexcept -> std::uint64_t {
  value += 0x9e37'79b9'7f4a'7c15ULL;
  value = (value ^ (value >> 30U)) * 0xbf58'476d'1ce4'e5b9ULL;
  value = (value ^ (value >> 27U)) * 0x94d0'49bb'1331'11ebULL;
  return value ^ (value >> 31U);
}

[[nodiscard]] constexpr auto undo_cuckoo_xor_shift(std::uint64_t value, unsigned shift) noexcept -> std::uint64_t {
  for (unsigned offset = shift; offset < 64U; offset *= 2U) {
    value ^= value >> offset;
  }
  return value;
}

[[nodiscard]] constexpr auto unmix_cuckoo_hash(std::uint64_t value) noexcept -> std::uint64_t {
  value = undo_cuckoo_xor_shift(value, 31U);
  value *= 0x3196'42b2'd24d'8ec3ULL;
  value = undo_cuckoo_xor_shift(value, 27U);
  value *= 0x96de'1b17'3f11'9089ULL;
  value = undo_cuckoo_xor_shift(value, 30U);
  return value - 0x9e37'79b9'7f4a'7c15ULL;
}

} // namespace

//===----- TYPE-TRAIT CHECKS ---------------------------------------------------===//

static_assert(std::is_move_constructible_v<CF>);
static_assert(!std::is_copy_constructible_v<CF>);
static_assert(!std::is_copy_assignable_v<CF>);

//===----- CONSTRUCTION & SIZING -----------------------------------------------===//

TEST(CuckooFilterBasic, IncludesLoadHeadroomAndRoundsToPowerOfTwoBuckets) {
  CF filter(1'000); // ceil(1000 / (4 * 0.95)) = 264 -> bit_ceil = 512 buckets
  EXPECT_EQ(filter.bucket_count(), 512U);
  EXPECT_EQ(filter.capacity(), 512U * 4U);
  EXPECT_EQ(filter.size(), 0U);
  EXPECT_DOUBLE_EQ(filter.load_factor(), 0.0);
}

TEST(CuckooFilterBasic, RequestedCapacityCanBeInserted) {
  constexpr int kRequested = 1'000;
  CF            filter(kRequested);
  for (int key = 0; key < kRequested; ++key) {
    ASSERT_TRUE(filter.insert(key)) << "failed before requested capacity at key " << key;
  }
  EXPECT_EQ(filter.size(), static_cast<std::size_t>(kRequested));
}

TEST(CuckooFilterBasic, OversizedCapacityThrowsBeforeAllocation) {
  EXPECT_THROW((void)CF(std::numeric_limits<std::size_t>::max()), CuckooFilterException);
}

TEST(CuckooFilterBasic, TinyCapacityStillHasOneBucket) {
  CF filter(1);
  EXPECT_EQ(filter.bucket_count(), 1U);
  EXPECT_EQ(filter.capacity(), 4U);
}

TEST(CuckooFilterBasic, EmptyFilterContainsNothing) {
  CF filter(64);
  EXPECT_FALSE(filter.might_contain(7));
  EXPECT_FALSE(filter.erase(7));
}

TEST(CuckooFilterBasic, FalsePositiveRateIsBounded) {
  CF           filter(64);
  const double expected = 1.0 - std::pow(1.0 - 1.0 / 255.0, 8.0);
  EXPECT_NEAR(filter.false_positive_rate(), expected, 1e-12);
}

//===----- INSERT / CONTAINS / ERASE -------------------------------------------===//

TEST(CuckooFilterCore, InsertedKeysAreFound) {
  CF filter(1'024);
  for (int i = 0; i < 500; ++i) {
    EXPECT_TRUE(filter.insert(i));
  }
  EXPECT_EQ(filter.size(), 500U);
  for (int i = 0; i < 500; ++i) {
    EXPECT_TRUE(filter.might_contain(i)) << "missing key " << i;
  }
}

TEST(CuckooFilterCore, EraseRemovesMembership) {
  CF filter(1'024);
  ASSERT_TRUE(filter.insert(42));
  EXPECT_TRUE(filter.might_contain(42));
  EXPECT_TRUE(filter.erase(42));
  EXPECT_FALSE(filter.might_contain(42));
  EXPECT_EQ(filter.size(), 0U);
  EXPECT_FALSE(filter.erase(42)); // already gone
}

TEST(CuckooFilterCore, DuplicateInsertsAreCountedAndErasedOneByOne) {
  CF filter(1'024);
  ASSERT_TRUE(filter.insert(5));
  ASSERT_TRUE(filter.insert(5));
  ASSERT_TRUE(filter.insert(5));
  EXPECT_EQ(filter.size(), 3U);

  EXPECT_TRUE(filter.erase(5));
  EXPECT_TRUE(filter.might_contain(5)); // two copies remain
  EXPECT_EQ(filter.size(), 2U);
  EXPECT_TRUE(filter.erase(5));
  EXPECT_TRUE(filter.erase(5));
  EXPECT_FALSE(filter.might_contain(5));
  EXPECT_EQ(filter.size(), 0U);
}

TEST(CuckooFilterCore, AlternateBucketIsDistinctWhenMultipleBucketsExist) {
  // Select a fingerprint whose original masked delta was zero for 256 buckets,
  // then invert the filter's mixer to feed that exact mixed hash through RawHash.
  std::uint8_t fingerprint = 1;
  while ((cuckoo_test_mix(fingerprint) & 255U) != 0U) {
    ++fingerprint;
  }
  const std::uint64_t fingerprint_source = static_cast<std::uint64_t>(fingerprint - 1U);
  const std::uint64_t mixed_hash         = fingerprint_source << 48U;
  const std::uint64_t raw_hash           = unmix_cuckoo_hash(mixed_hash);

  CuckooFilter<int, CuckooRawHash> filter(900, CuckooRawHash{raw_hash});
  ASSERT_EQ(filter.bucket_count(), 256U);
  for (int copy = 0; copy < 8; ++copy) {
    EXPECT_TRUE(filter.insert(7)) << "copy " << copy << " could not use the alternate bucket";
  }
  EXPECT_EQ(filter.size(), 8U);
  EXPECT_FALSE(filter.insert(7));
}

TEST(CuckooFilterCore, NoFalseNegativesOverLargeBatch) {
  CF                      filter(8'192);
  std::unordered_set<int> inserted;
  for (int i = 0; i < 4'000; ++i) {
    const int key = i * 7 + 1;
    ASSERT_TRUE(filter.insert(key));
    inserted.insert(key);
  }
  for (int key : inserted) {
    EXPECT_TRUE(filter.might_contain(key)) << "false negative for " << key;
  }
}

//===----- SATURATION & ROLLBACK -----------------------------------------------===//

TEST(CuckooFilterSaturation, FailedInsertLeavesFilterUnchanged) {
  // A small filter is driven to saturation; once an insert fails, every key that
  // was successfully inserted must still be present and the count must match,
  // proving the eviction chain was rolled back cleanly.
  CF               filter(256);
  std::vector<int> inserted;
  bool             saw_failure = false;

  for (int i = 0; i < 100'000; ++i) {
    if (filter.insert(i)) {
      inserted.push_back(i);
    } else {
      saw_failure = true;
      break;
    }
  }

  EXPECT_TRUE(saw_failure) << "filter never saturated";
  EXPECT_GT(filter.load_factor(), 0.90); // cuckoo filters fill densely before failing
  EXPECT_EQ(filter.size(), inserted.size());
  for (int key : inserted) {
    EXPECT_TRUE(filter.might_contain(key)) << "lost key " << key << " after a failed insert";
  }
}

//===----- FALSE-POSITIVE BEHAVIOUR --------------------------------------------===//

TEST(CuckooFilterFalsePositive, AbsentKeysRarelyReportPresent) {
  CF filter(8'192);
  for (int i = 0; i < 2'000; ++i) {
    ASSERT_TRUE(filter.insert(i));
  }

  int positives = 0;
  int trials    = 0;
  for (int key = 1'000'000; key < 1'010'000; ++key) {
    ++trials;
    if (filter.might_contain(key)) {
      ++positives;
    }
  }
  const double rate = static_cast<double>(positives) / static_cast<double>(trials);
  EXPECT_LT(rate, 0.05) << "observed false-positive rate " << rate;
}

//===----- CLEAR & MOVE --------------------------------------------------------===//

TEST(CuckooFilterLifecycle, ClearResetsAndReuses) {
  CF filter(1'024);
  for (int i = 0; i < 100; ++i) {
    ASSERT_TRUE(filter.insert(i));
  }
  filter.clear();
  EXPECT_EQ(filter.size(), 0U);
  EXPECT_FALSE(filter.might_contain(50));
  ASSERT_TRUE(filter.insert(50));
  EXPECT_TRUE(filter.might_contain(50));
}

TEST(CuckooFilterLifecycle, MoveTransfersState) {
  CF src(1'024);
  ASSERT_TRUE(src.insert(9));
  ASSERT_TRUE(src.insert(11));

  CF dst(std::move(src));
  EXPECT_TRUE(dst.might_contain(9));
  EXPECT_TRUE(dst.might_contain(11));
  EXPECT_EQ(dst.size(), 2U);

  // Moved-from filter is empty and queryable; insert on it is rejected.
  EXPECT_EQ(src.bucket_count(), 0U);
  EXPECT_FALSE(src.might_contain(9));
  EXPECT_FALSE(src.erase(9));
  EXPECT_THROW((void)src.insert(9), CuckooFilterException);

  CF dst2(64);
  dst2 = std::move(dst);
  EXPECT_TRUE(dst2.might_contain(9));
  EXPECT_FALSE(dst.might_contain(9));
}

TEST(CuckooFilterLifecycle, ThrowingHashMoveAssignmentPreservesTargetLayout) {
  using ThrowingFilter = CuckooFilter<int, CuckooThrowingMoveHash>;

  ThrowingFilter source(1'024, CuckooThrowingMoveHash{11});
  ThrowingFilter target(64, CuckooThrowingMoveHash{7});
  ASSERT_TRUE(source.insert(100));
  ASSERT_TRUE(target.insert(42));
  const auto original_bucket_count = target.bucket_count();
  const auto original_capacity     = target.capacity();

  CuckooThrowingMoveHash::throw_on_move_assignment = true;
  EXPECT_THROW(target = std::move(source), std::runtime_error);
  CuckooThrowingMoveHash::throw_on_move_assignment = false;

  EXPECT_EQ(target.bucket_count(), original_bucket_count);
  EXPECT_EQ(target.capacity(), original_capacity);
  EXPECT_EQ(target.size(), 1U);
  EXPECT_TRUE(target.might_contain(42));
}

//===----- HASH FAMILY & ALTERNATE TYPES ---------------------------------------===//

TEST(CuckooFilterTypes, StatefulHashRoundTrips) {
  CuckooFilter<int, CuckooSeededHash> filter(1'024, CuckooSeededHash{7});
  for (int i = 0; i < 300; ++i) {
    ASSERT_TRUE(filter.insert(i));
  }
  for (int i = 0; i < 300; ++i) {
    EXPECT_TRUE(filter.might_contain(i));
  }
}

TEST(CuckooFilterTypes, StringKeys) {
  CuckooFilter<std::string>       filter(1'024);
  std::unordered_set<std::string> inserted;
  for (int i = 0; i < 1'000; ++i) {
    std::string key = "item-" + std::to_string(i);
    ASSERT_TRUE(filter.insert(key));
    inserted.insert(key);
  }
  for (const std::string& key : inserted) {
    EXPECT_TRUE(filter.might_contain(key));
  }
  EXPECT_TRUE(filter.erase("item-0"));
  EXPECT_EQ(filter.size(), inserted.size() - 1U);
  EXPECT_TRUE(filter.might_contain("item-1"));
}

//===---------------------------------------------------------------------------===//
