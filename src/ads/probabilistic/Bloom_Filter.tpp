//===---------------------------------------------------------------------------===//
/**
 * @file Bloom_Filter.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the BloomFilter class.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/probabilistic/Bloom_Filter.hpp"

#include <algorithm>

namespace ads::probabilistic {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <typename Key, typename Hash>
BloomFilter<Key, Hash>::BloomFilter(size_t bit_count, size_t hash_count, Hash hasher) :
    hash_count_(hash_count),
    insert_count_(0),
    set_bit_count_(0),
    hasher_(std::move(hasher)),
    bits_(bit_count, false) {
  if (bit_count == 0) {
    throw BloomFilterException("Bloom filter bit_count must be greater than zero");
  }
  if (hash_count == 0) {
    throw BloomFilterException("Bloom filter hash_count must be greater than zero");
  }
}

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::from_estimates(size_t expected_insertions, double false_positive_rate, Hash hasher) -> BloomFilter<Key, Hash> {
  if (expected_insertions == 0) {
    throw BloomFilterException("Bloom filter expected_insertions must be greater than zero");
  }
  if (!(false_positive_rate > 0.0 && false_positive_rate < 1.0)) {
    throw BloomFilterException("Bloom filter false_positive_rate must be in (0, 1)");
  }

  const double bit_count_estimate  = -static_cast<double>(expected_insertions) * std::log(false_positive_rate) / (kLn2 * kLn2);
  const double hash_count_estimate = (bit_count_estimate / static_cast<double>(expected_insertions)) * kLn2;

  const size_t bit_count  = std::max<size_t>(1, static_cast<size_t>(std::ceil(bit_count_estimate)));
  const size_t hash_count = std::max<size_t>(1, static_cast<size_t>(std::ceil(hash_count_estimate)));
  return BloomFilter(bit_count, hash_count, std::move(hasher));
}

//===----- CORE OPERATIONS -----------------------------------------------------===//

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::insert(const Key& key) -> void {
  if (bits_.is_empty()) {
    throw BloomFilterException("BloomFilter cannot insert into a moved-from filter");
  }

  for (size_t probe_number = 0; probe_number < hash_count_; ++probe_number) {
    const size_t index = probe_index(key, probe_number);
    if (!bits_[index]) {
      bits_[index] = true;
      ++set_bit_count_;
    }
  }
  ++insert_count_;
}

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::might_contain(const Key& key) const -> bool {
  if (bits_.is_empty()) {
    return false; // moved-from filter: no bits to probe
  }

  for (size_t probe_number = 0; probe_number < hash_count_; ++probe_number) {
    if (!bits_[probe_index(key, probe_number)]) {
      return false;
    }
  }
  return true;
}

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::merge(const BloomFilter& other) -> void {
  if (bits_.size() != other.bits_.size() || hash_count_ != other.hash_count_) {
    throw BloomFilterException("Bloom filters must have the same layout to be merged");
  }

  for (size_t index = 0; index < bits_.size(); ++index) {
    if (!bits_[index] && other.bits_[index]) {
      bits_[index] = true;
      ++set_bit_count_;
    }
  }

  insert_count_ += other.insert_count_;
}

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::clear() noexcept -> void {
  for (size_t index = 0; index < bits_.size(); ++index) {
    bits_[index] = false;
  }
  insert_count_  = 0;
  set_bit_count_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::bit_count() const noexcept -> size_t {
  return bits_.size();
}

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::hash_count() const noexcept -> size_t {
  return hash_count_;
}

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::insert_count() const noexcept -> size_t {
  return insert_count_;
}

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::set_bit_count() const noexcept -> size_t {
  return set_bit_count_;
}

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::load_factor() const noexcept -> double {
  return bits_.is_empty() ? 0.0 : static_cast<double>(set_bit_count_) / static_cast<double>(bits_.size());
}

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::estimated_false_positive_rate() const noexcept -> double {
  if (insert_count_ == 0) {
    return 0.0;
  }

  const auto m = static_cast<double>(bits_.size());
  const auto k = static_cast<double>(hash_count_);
  const auto n = static_cast<double>(insert_count_);
  return std::pow(1.0 - std::exp((-k * n) / m), k);
}

//===----- PRIVATE HELPERS -----------------------------------------------------===//

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::mix_hash(std::uint64_t value) noexcept -> std::uint64_t {
  value += 0x9e37'79b9'7f4a'7c15ULL;
  value = (value ^ (value >> 30U)) * 0xbf58'476d'1ce4'e5b9ULL;
  value = (value ^ (value >> 27U)) * 0x94d0'49bb'1331'11ebULL;
  return value ^ (value >> 31U);
}

template <typename Key, typename Hash>
auto BloomFilter<Key, Hash>::probe_index(const Key& key, size_t probe_number) const -> size_t {
  const auto seed  = static_cast<std::uint64_t>(hasher_(key));
  const auto mixed = mix_hash(seed + 0x9e37'79b9'7f4a'7c15ULL * static_cast<std::uint64_t>(probe_number + 1));
  return static_cast<size_t>(mixed % bits_.size());
}

} // namespace ads::probabilistic

//===---------------------------------------------------------------------------===//
