//===---------------------------------------------------------------------------===//
/**
 * @file String_Algorithms.tpp
 * @author Costantino Lombardi
 * @brief Implementation of reusable string algorithms.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/algorithms/String_Algorithms.hpp"

namespace ads::algorithms {

//===--------------------------- INTERNAL UTILITIES ----------------------------===//

namespace string_algorithm_detail {

inline auto all_match_positions(size_t text_length) -> ads::arrays::DynamicArray<size_t> {
  ads::arrays::DynamicArray<size_t> matches;
  matches.reserve(text_length + 1);
  for (size_t index = 0; index <= text_length; ++index) {
    matches.push_back(index);
  }
  return matches;
}

} // namespace string_algorithm_detail

//===----------------------- PATTERN MATCHING ALGORITHMS -----------------------===//

auto compute_kmp_prefix_function(std::string_view pattern) -> ads::arrays::DynamicArray<size_t> {
  ads::arrays::DynamicArray<size_t> prefix(pattern.size(), 0U);

  for (size_t index = 1, matched = 0; index < pattern.size(); ++index) {
    while (matched > 0 && pattern[index] != pattern[matched]) {
      matched = prefix[matched - 1];
    }

    if (pattern[index] == pattern[matched]) {
      ++matched;
    }
    prefix[index] = matched;
  }

  return prefix;
}

auto kmp_search(std::string_view text, std::string_view pattern) -> ads::arrays::DynamicArray<size_t> {
  if (pattern.empty()) {
    return string_algorithm_detail::all_match_positions(text.size());
  }
  if (pattern.size() > text.size()) {
    return ads::arrays::DynamicArray<size_t>{};
  }

  const auto                        prefix = compute_kmp_prefix_function(pattern);
  ads::arrays::DynamicArray<size_t> matches;

  for (size_t text_index = 0, matched = 0; text_index < text.size(); ++text_index) {
    while (matched > 0 && text[text_index] != pattern[matched]) {
      matched = prefix[matched - 1];
    }

    if (text[text_index] == pattern[matched]) {
      ++matched;
    }

    if (matched == pattern.size()) {
      matches.push_back(text_index + 1 - pattern.size());
      matched = prefix[matched - 1];
    }
  }

  return matches;
}

auto rabin_karp_search(std::string_view text, std::string_view pattern, std::uint64_t base, std::uint64_t modulus)
    -> ads::arrays::DynamicArray<size_t> {
  if (base == 0 || modulus == 0) {
    throw StringAlgorithmException("Rabin-Karp base and modulus must be greater than zero");
  }
  if (pattern.empty()) {
    return string_algorithm_detail::all_match_positions(text.size());
  }
  if (pattern.size() > text.size()) {
    return ads::arrays::DynamicArray<size_t>{};
  }

  std::uint64_t highest_power = 1;
  std::uint64_t pattern_hash  = 0;
  std::uint64_t window_hash   = 0;

  for (size_t index = 0; index < pattern.size(); ++index) {
    if (index + 1 < pattern.size()) {
      highest_power = (highest_power * base) % modulus;
    }

    pattern_hash = (pattern_hash * base + static_cast<unsigned char>(pattern[index])) % modulus;
    window_hash  = (window_hash * base + static_cast<unsigned char>(text[index])) % modulus;
  }

  ads::arrays::DynamicArray<size_t> matches;

  for (size_t start = 0; start + pattern.size() <= text.size(); ++start) {
    if (pattern_hash == window_hash && text.substr(start, pattern.size()) == pattern) {
      matches.push_back(start);
    }

    if (start + pattern.size() == text.size()) {
      continue;
    }

    const auto leading = (static_cast<unsigned char>(text[start]) * highest_power) % modulus;
    window_hash        = (window_hash + modulus - leading) % modulus;
    window_hash        = (window_hash * base + static_cast<unsigned char>(text[start + pattern.size()])) % modulus;
  }

  return matches;
}

} // namespace ads::algorithms

//===---------------------------------------------------------------------------===//
