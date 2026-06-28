//===---------------------------------------------------------------------------===//
/**
 * @file Test_Bloom_Filter.cpp
 * @brief Google Test unit tests for BloomFilter.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/probabilistic/Bloom_Filter.hpp"

#include <gtest/gtest.h>

#include <string>

using namespace ads::probabilistic;

namespace {

//===----- TEST UTILITIES ------------------------------------------------------===//

struct IdentityHash {
  auto operator()(int value) const noexcept -> size_t { return static_cast<size_t>(value); }
};

} // namespace

//===----- BASIC BEHAVIOR ------------------------------------------------------===//

TEST(BloomFilterTest, InsertedElementsAreAlwaysReportedAsPresent) {
  BloomFilter<int, IdentityHash> filter(128, 1, IdentityHash{});

  filter.insert(3);
  filter.insert(7);
  filter.insert(11);

  EXPECT_TRUE(filter.might_contain(3));
  EXPECT_TRUE(filter.might_contain(7));
  EXPECT_TRUE(filter.might_contain(11));
  EXPECT_FALSE(filter.might_contain(42));
}

TEST(BloomFilterTest, ClearResetsBitsAndStatistics) {
  BloomFilter<int> filter(256, 3);

  filter.insert(1);
  filter.insert(2);
  ASSERT_GT(filter.set_bit_count(), 0U);
  ASSERT_GT(filter.insert_count(), 0U);

  filter.clear();

  EXPECT_EQ(filter.set_bit_count(), 0U);
  EXPECT_EQ(filter.insert_count(), 0U);
  EXPECT_EQ(filter.load_factor(), 0.0);
  EXPECT_FALSE(filter.might_contain(1));
}

//===----- COMPOSITION TESTS ---------------------------------------------------===//

TEST(BloomFilterTest, MergeCombinesCompatibleFilters) {
  BloomFilter<std::string> left(256, 3);
  BloomFilter<std::string> right(256, 3);

  left.insert("apple");
  right.insert("banana");

  left.merge(right);

  EXPECT_TRUE(left.might_contain("apple"));
  EXPECT_TRUE(left.might_contain("banana"));
  EXPECT_EQ(left.insert_count(), 2U);
}

TEST(BloomFilterTest, MergeRejectsIncompatibleLayouts) {
  BloomFilter<int> left(128, 3);
  BloomFilter<int> right(256, 3);

  EXPECT_THROW(left.merge(right), BloomFilterException);
}

//===----- CONFIGURATION / ERROR TESTS -----------------------------------------===//

TEST(BloomFilterFactoryTest, BuildsFromCapacityEstimates) {
  auto filter = BloomFilter<std::string>::from_estimates(200, 0.01);

  EXPECT_GT(filter.bit_count(), 0U);
  EXPECT_GT(filter.hash_count(), 0U);

  filter.insert("graph");
  filter.insert("algorithm");

  EXPECT_TRUE(filter.might_contain("graph"));
  EXPECT_TRUE(filter.might_contain("algorithm"));
  EXPECT_GT(filter.estimated_false_positive_rate(), 0.0);
}

TEST(BloomFilterErrorTest, RejectsInvalidConfiguration) {
  EXPECT_THROW(static_cast<void>(BloomFilter<int>(0, 3)), BloomFilterException);
  EXPECT_THROW(static_cast<void>(BloomFilter<int>(64, 0)), BloomFilterException);
  EXPECT_THROW(static_cast<void>(BloomFilter<int>::from_estimates(0, 0.01)), BloomFilterException);
  EXPECT_THROW(static_cast<void>(BloomFilter<int>::from_estimates(10, 1.0)), BloomFilterException);
}

//===---------------------------------------------------------------------------===//
