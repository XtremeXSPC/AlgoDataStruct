//===---------------------------------------------------------------------------===//
/**
 * @file Bloom_Filter.hpp
 * @author Costantino Lombardi
 * @brief Declaration of a Bloom filter probabilistic set.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef BLOOM_FILTER_HPP
#define BLOOM_FILTER_HPP

#include "../arrays/Dynamic_Array.hpp"

#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <numbers>
#include <stdexcept>
#include <utility>

namespace ads::probabilistic {

/**
 * @brief Exception class for Bloom filter operations.
 */
class BloomFilterException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Probabilistic set membership structure with false positives but no false negatives.
 *
 * @tparam Key Key type hashed into the filter.
 * @tparam Hash Hash functor used to seed the probe family.
 */
template <typename Key, typename Hash = std::hash<Key>>
class BloomFilter {
public:
  using key_type    = Key;
  using hasher_type = Hash;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs a filter with a fixed bit array and hash count.
   * @param bit_count Number of bits in the filter.
   * @param hash_count Number of derived hash probes per inserted key.
   * @param hasher Hash functor used to seed the probe family.
   * @throws BloomFilterException if bit_count or hash_count is zero.
   */
  explicit BloomFilter(size_t bit_count = kDefaultBitCount, size_t hash_count = kDefaultHashCount, Hash hasher = Hash{});

  /**
   * @brief Builds a filter from expected insertions and target false-positive rate.
   * @param expected_insertions Planned number of insertion operations.
   * @param false_positive_rate Desired false-positive rate in (0, 1).
   * @param hasher Hash functor used to seed the probe family.
   * @return Configured Bloom filter.
   * @throws BloomFilterException if expected_insertions is zero or the rate is invalid.
   */
  [[nodiscard]] static auto from_estimates(size_t expected_insertions, double false_positive_rate, Hash hasher = Hash{}) -> BloomFilter;

  BloomFilter(BloomFilter&& other) noexcept                    = default;
  auto operator=(BloomFilter&& other) noexcept -> BloomFilter& = default;
  ~BloomFilter()                                               = default;

  BloomFilter(const BloomFilter&)                    = delete;
  auto operator=(const BloomFilter&) -> BloomFilter& = delete;

  //===----- CORE OPERATIONS ---------------------------------------------------===//

  /**
   * @brief Inserts a key into the filter.
   * @param key Key to hash into the filter.
   * @complexity Time O(k), Space O(1)
   */
  auto insert(const Key& key) -> void;

  /**
   * @brief Tests whether a key may be present.
   * @param key Key to query.
   * @return true if the key may be present, false if it is definitely absent.
   * @complexity Time O(k), Space O(1)
   */
  [[nodiscard]] auto might_contain(const Key& key) const -> bool;

  /**
   * @brief Merges another compatible filter into this one.
   * @param other Filter with the same layout and hash count.
   * @throws BloomFilterException if the filters are incompatible.
   * @complexity Time O(m), Space O(1)
   */
  auto merge(const BloomFilter& other) -> void;

  /**
   * @brief Clears all bits and resets insertion statistics.
   * @complexity Time O(m), Space O(1)
   */
  auto clear() noexcept -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Returns the number of bits in the filter.
   * @return Bit count.
   */
  [[nodiscard]] auto bit_count() const noexcept -> size_t;

  /**
   * @brief Returns the number of hash probes per key.
   * @return Hash count.
   */
  [[nodiscard]] auto hash_count() const noexcept -> size_t;

  /**
   * @brief Returns how many insertion operations have been performed.
   * @return Insert count.
   */
  [[nodiscard]] auto insert_count() const noexcept -> size_t;

  /**
   * @brief Returns the number of set bits.
   * @return Set-bit count.
   */
  [[nodiscard]] auto set_bit_count() const noexcept -> size_t;

  /**
   * @brief Returns the fraction of bits currently set.
   * @return Load factor in [0, 1].
   */
  [[nodiscard]] auto load_factor() const noexcept -> double;

  /**
   * @brief Estimates the current false-positive rate from the current insert count.
   * @return Estimated false-positive rate.
   */
  [[nodiscard]] auto estimated_false_positive_rate() const noexcept -> double;

private:
  //===----- INTERNAL STORAGE --------------------------------------------------===//

  static constexpr size_t kDefaultBitCount  = 1'024;
  static constexpr size_t kDefaultHashCount = 3;
  static constexpr double kLn2              = std::numbers::ln2;

  ads::arrays::DynamicArray<bool> bits_;
  size_t                          hash_count_;
  size_t                          insert_count_;
  size_t                          set_bit_count_;
  Hash                            hasher_;

  //===----- HELPER API --------------------------------------------------------===//

  [[nodiscard]] static auto mix_hash(std::uint64_t value) noexcept -> std::uint64_t;
  [[nodiscard]] auto        probe_index(const Key& key, size_t probe_number) const -> size_t;
};

} // namespace ads::probabilistic

#include "../../../src/ads/probabilistic/Bloom_Filter.tpp"

#endif // BLOOM_FILTER_HPP

//===---------------------------------------------------------------------------===//
