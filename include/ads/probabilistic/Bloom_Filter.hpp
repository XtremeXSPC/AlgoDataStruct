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

using ads::arrays::DynamicArray;

///@brief Exception class for Bloom filter operations.
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
  ///@brief Type aliases for convenience.
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
   * @brief Move constructor.
   * @param other Filter to move from.
   * @complexity Time O(1), Space O(1)
   */
  BloomFilter(BloomFilter&& other) noexcept = default;

  /**
   * @brief Move assignment operator.
   * @param other Filter to move from.
   * @return Reference to this filter.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(BloomFilter&& other) noexcept -> BloomFilter& = default;

  /**
   * @brief Destructor.
   * @complexity Time O(m), Space O(1)
   */
  ~BloomFilter() = default;

  // Copy operations are disabled (move-only type).
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

  /**
   * @brief Tests whether a key may be present.
   * @param key Key to query.
   * @return true if the key may be present, false if it is definitely absent.
   * @complexity Time O(k), Space O(1)
   */
  [[nodiscard]] auto might_contain(const Key& key) const -> bool;

  /**
   * @brief Builds a filter from expected insertions and target false-positive rate.
   * @param expected_insertions Planned number of insertion operations.
   * @param false_positive_rate Desired false-positive rate in (0, 1).
   * @param hasher Hash functor used to seed the probe family.
   * @return Configured Bloom filter.
   * @throws BloomFilterException if expected_insertions is zero or the rate is invalid.
   */
  [[nodiscard]] static auto from_estimates(size_t expected_insertions, double false_positive_rate, Hash hasher = Hash{}) -> BloomFilter;


  //===----- QUERY OPERATIONS --------------------------------------------------===//

  ///@brief Returns the number of bits in the filter.
  [[nodiscard]] auto bit_count() const noexcept -> size_t;

  ///@brief Returns the number of hash probes per key.
  [[nodiscard]] auto hash_count() const noexcept -> size_t;

  ///@brief Returns how many insertion operations have been performed.
  [[nodiscard]] auto insert_count() const noexcept -> size_t;

  ///@brief Returns the number of set bits.
  [[nodiscard]] auto set_bit_count() const noexcept -> size_t;

  ///@brief Returns the fraction of bits currently set (load factor in [0, 1]).
  [[nodiscard]] auto load_factor() const noexcept -> double;

  ///@brief Estimates the current false-positive rate from the current insert count.
  [[nodiscard]] auto estimated_false_positive_rate() const noexcept -> double;

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Avalanche mixer for a raw 64-bit hash value; reduces clustering between probes.
  [[nodiscard]] static auto mix_hash(std::uint64_t value) noexcept -> std::uint64_t;

  ///@brief Returns the bit index for the given key and probe number using double hashing.
  [[nodiscard]] auto probe_index(const Key& key, size_t probe_number) const -> size_t;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  static constexpr size_t kDefaultBitCount  = 1'024; ///< Default bit count (direct constructor).
  static constexpr size_t kDefaultHashCount = 3;     ///< Default hash probe count per key.

  static constexpr double kLn2 = std::numbers::ln2; ///< ln(2), used in capacity and hash-count formulae.

  size_t hash_count_;    ///< Hash probes per key.
  size_t insert_count_;  ///< Total insertions performed.
  size_t set_bit_count_; ///< Bits currently set to true.
  Hash   hasher_;        ///< Hash functor seeding the probe family.

  DynamicArray<bool> bits_; ///< Bit array of length bit_count_.
};

} // namespace ads::probabilistic

#include "../../../src/ads/probabilistic/Bloom_Filter.tpp"

#endif // BLOOM_FILTER_HPP

//===---------------------------------------------------------------------------===//
