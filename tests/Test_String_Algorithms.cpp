//===---------------------------------------------------------------------------===//
/**
 * @file Test_String_Algorithms.cpp
 * @brief Google Test unit tests for reusable string algorithms.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/algorithms/String_Algorithms.hpp"

#include <gtest/gtest.h>

#include <initializer_list>
#include <string_view>

using namespace ads::algorithms;

namespace {

//===----- TEST UTILITIES ------------------------------------------------------===//

auto expect_indices_eq(const ads::arrays::DynamicArray<size_t>& actual, std::initializer_list<size_t> expected) -> void {
  ASSERT_EQ(actual.size(), expected.size());
  size_t index = 0;
  for (size_t value : expected) {
    EXPECT_EQ(actual[index], value);
    ++index;
  }
}

} // namespace

//===----- PREFIX / KMP TESTS --------------------------------------------------===//

TEST(StringAlgorithmsKmpTest, ComputesPrefixFunction) {
  const auto prefix = compute_kmp_prefix_function("ababaca");
  expect_indices_eq(prefix, {0, 0, 1, 2, 3, 0, 1});
}

TEST(StringAlgorithmsKmpTest, FindsOverlappingMatches) {
  const auto matches = kmp_search("ababa", "aba");
  expect_indices_eq(matches, {0, 2});
}

TEST(StringAlgorithmsKmpTest, EmptyPatternMatchesEveryPosition) {
  const auto matches = kmp_search("abc", "");
  expect_indices_eq(matches, {0, 1, 2, 3});
}

TEST(StringAlgorithmsKmpTest, HandlesPatternLongerThanText) {
  const auto matches = kmp_search("abc", "abcd");
  EXPECT_TRUE(matches.is_empty());
}

//===----- RABIN-KARP TESTS ----------------------------------------------------===//

TEST(StringAlgorithmsRabinKarpTest, FindsSameMatchesAsKmp) {
  constexpr std::string_view text    = "bananabanaba";
  constexpr std::string_view pattern = "ana";

  const auto kmp_matches   = kmp_search(text, pattern);
  const auto rabin_matches = rabin_karp_search(text, pattern);

  ASSERT_EQ(kmp_matches.size(), rabin_matches.size());
  for (size_t index = 0; index < kmp_matches.size(); ++index) {
    EXPECT_EQ(kmp_matches[index], rabin_matches[index]);
  }
}

TEST(StringAlgorithmsRabinKarpTest, HandlesRepeatedPatterns) {
  const auto matches = rabin_karp_search("aaaaa", "aa");
  expect_indices_eq(matches, {0, 1, 2, 3});
}

TEST(StringAlgorithmsRabinKarpTest, EmptyPatternMatchesEveryPosition) {
  const auto matches = rabin_karp_search("xyz", "");
  expect_indices_eq(matches, {0, 1, 2, 3});
}

TEST(StringAlgorithmsRabinKarpTest, RejectsInvalidHashConfiguration) {
  EXPECT_THROW(static_cast<void>(rabin_karp_search("abc", "a", 0, 101)), StringAlgorithmException);
  EXPECT_THROW(static_cast<void>(rabin_karp_search("abc", "a", 257, 0)), StringAlgorithmException);
}

//===---------------------------------------------------------------------------===//
