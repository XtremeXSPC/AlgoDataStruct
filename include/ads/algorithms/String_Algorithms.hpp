//===---------------------------------------------------------------------------===//
/**
 * @file String_Algorithms.hpp
 * @author Costantino Lombardi
 * @brief Reusable string and pattern-matching algorithms.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef STRING_ALGORITHMS_HPP
#define STRING_ALGORITHMS_HPP

#include "../arrays/Dynamic_Array.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>

namespace ads::algorithms {

/**
 * @brief Exception class for string algorithm configuration errors.
 */
class StringAlgorithmException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

//===---------------------------- SEARCH PRIMITIVES ----------------------------===//

/**
 * @brief Computes the prefix-function table used by KMP.
 * @param pattern Pattern to preprocess.
 * @return Prefix-function values for each pattern position.
 * @complexity Time O(m), Space O(m)
 */
[[nodiscard]] auto compute_kmp_prefix_function(std::string_view pattern) -> ads::arrays::DynamicArray<size_t>;

/**
 * @brief Finds every occurrence of pattern in text with Knuth-Morris-Pratt.
 * @param text Input text.
 * @param pattern Pattern to search for.
 * @return Start indices of every match.
 * @complexity Time O(n + m), Space O(m)
 * @note An empty pattern matches at every position, including text.size().
 */
[[nodiscard]] auto kmp_search(std::string_view text, std::string_view pattern) -> ads::arrays::DynamicArray<size_t>;

/**
 * @brief Finds every occurrence of pattern in text with Rabin-Karp.
 * @param text Input text.
 * @param pattern Pattern to search for.
 * @param base Polynomial rolling-hash base.
 * @param modulus Prime modulus used for the rolling hash.
 * @return Start indices of every match.
 * @throws StringAlgorithmException if base or modulus is zero.
 * @complexity Time O(n + m) average, O(nm) worst case, Space O(1) besides matches.
 * @note An empty pattern matches at every position, including text.size().
 */
[[nodiscard]] auto
rabin_karp_search(std::string_view text, std::string_view pattern, std::uint64_t base = 257ULL, std::uint64_t modulus = 1'000'000'007ULL)
    -> ads::arrays::DynamicArray<size_t>;

} // namespace ads::algorithms

#include "../../../src/ads/algorithms/String_Algorithms.tpp"

#endif // STRING_ALGORITHMS_HPP

//===---------------------------------------------------------------------------===//
