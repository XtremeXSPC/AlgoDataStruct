//===---------------------------------------------------------------------------===//
/**
 * @file Cuckoo_Filter.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the CuckooFilter class.
 * @version 0.1
 * @date 2026-07-12
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/probabilistic/Cuckoo_Filter.hpp"

#include <bit>
#include <limits>
#include <utility>

namespace ads::probabilistic {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <typename Key, typename Hash>
CuckooFilter<Key, Hash>::CuckooFilter(size_type capacity, Hash hasher) :
    bucket_count_(compute_bucket_count(capacity)),
    mask_(bucket_count_ - 1),
    size_(0),
    hasher_(std::move(hasher)),
    rng_state_(kRngSeed),
    buckets_(bucket_count_ * kSlotsPerBucket, std::uint8_t{0}) {
}

template <typename Key, typename Hash>
CuckooFilter<Key, Hash>::CuckooFilter(CuckooFilter&& other) noexcept(std::is_nothrow_move_constructible_v<Hash>) :
    bucket_count_(other.bucket_count_),
    mask_(other.mask_),
    size_(other.size_),
    hasher_(std::move(other.hasher_)),
    rng_state_(other.rng_state_),
    buckets_(std::move(other.buckets_)) {
  other.bucket_count_ = 0;
  other.mask_         = 0;
  other.size_         = 0;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::operator=(CuckooFilter&& other) noexcept(std::is_nothrow_move_assignable_v<Hash>) -> CuckooFilter& {
  if (this != &other) {
    // Assign the only potentially throwing member before changing the layout
    // metadata. If it throws, bucket_count_/mask_/buckets_ still agree.
    hasher_             = std::move(other.hasher_);
    bucket_count_       = other.bucket_count_;
    mask_               = other.mask_;
    size_               = other.size_;
    rng_state_          = other.rng_state_;
    buckets_            = std::move(other.buckets_);
    other.bucket_count_ = 0;
    other.mask_         = 0;
    other.size_         = 0;
  }
  return *this;
}

//===----- CORE OPERATIONS -----------------------------------------------------===//

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::insert(const Key& key) -> bool {
  if (bucket_count_ == 0) {
    throw CuckooFilterException("CuckooFilter cannot insert into a moved-from filter");
  }

  const auto [index_one, fingerprint] = index_and_fingerprint(key);
  const size_type index_two           = alt_index(index_one, fingerprint);

  // Fast path: either candidate bucket has a free slot.
  if (insert_into_bucket(index_one, fingerprint) || insert_into_bucket(index_two, fingerprint)) {
    ++size_;
    return true;
  }

  // Both buckets are full: evict a random resident and relocate it, recording each
  // swap so the whole chain can be rolled back if it fails to settle.
  size_type    index   = (next_random() & 1U) != 0U ? index_one : index_two;
  std::uint8_t carried = fingerprint;

  DynamicArray<size_type> trail;
  trail.reserve(kMaxKicks);

  for (size_type kick = 0; kick < kMaxKicks; ++kick) {
    const size_type slot = static_cast<size_type>(next_random() % kSlotsPerBucket);
    const size_type cell = index * kSlotsPerBucket + slot;

    std::swap(carried, buckets_[cell]); // carried now holds the evicted fingerprint
    trail.push_back(cell);

    index = alt_index(index, carried);
    if (insert_into_bucket(index, carried)) {
      ++size_;
      return true;
    }
  }

  // No stable placement within kMaxKicks: undo every swap to restore the filter,
  // then report that the key was not inserted.
  for (size_type step = trail.size(); step > 0; --step) {
    std::swap(carried, buckets_[trail[step - 1]]);
  }
  return false;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::might_contain(const Key& key) const -> bool {
  if (bucket_count_ == 0) {
    return false;
  }

  const auto [index_one, fingerprint] = index_and_fingerprint(key);
  if (bucket_contains(index_one, fingerprint)) {
    return true;
  }
  return bucket_contains(alt_index(index_one, fingerprint), fingerprint);
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::erase(const Key& key) -> bool {
  if (bucket_count_ == 0) {
    return false;
  }

  const auto [index_one, fingerprint] = index_and_fingerprint(key);
  const size_type index_two           = alt_index(index_one, fingerprint);

  if (remove_from_bucket(index_one, fingerprint) || remove_from_bucket(index_two, fingerprint)) {
    --size_;
    return true;
  }
  return false;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::clear() noexcept -> void {
  for (size_type slot = 0; slot < buckets_.size(); ++slot) {
    buckets_[slot] = std::uint8_t{0};
  }
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::size() const noexcept -> size_type {
  return size_;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::capacity() const noexcept -> size_type {
  return bucket_count_ * kSlotsPerBucket;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::bucket_count() const noexcept -> size_type {
  return bucket_count_;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::load_factor() const noexcept -> double {
  const size_type slots = capacity();
  return slots == 0 ? 0.0 : static_cast<double>(size_) / static_cast<double>(slots);
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::false_positive_rate() const noexcept -> double {
  // Per-query collision estimate: 1 - (1 - 1/(2^f-1))^(2b), with f fingerprint
  // storage bits, one reserved empty value, and b slots per bucket.
  const double per_slot = 1.0 / static_cast<double>((size_type{1} << kFingerprintBits) - 1U);
  return 1.0 - std::pow(1.0 - per_slot, 2.0 * static_cast<double>(kSlotsPerBucket));
}

//===----- PRIVATE HELPERS -----------------------------------------------------===//

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::mix_hash(std::uint64_t value) noexcept -> std::uint64_t {
  value += 0x9e37'79b9'7f4a'7c15ULL;
  value = (value ^ (value >> 30U)) * 0xbf58'476d'1ce4'e5b9ULL;
  value = (value ^ (value >> 27U)) * 0x94d0'49bb'1331'11ebULL;
  return value ^ (value >> 31U);
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::index_and_fingerprint(const Key& key) const -> std::pair<size_type, std::uint8_t> {
  const std::uint64_t hash = mix_hash(static_cast<std::uint64_t>(hasher_(key)));

  // Use the high 16 bits to distribute fingerprints across all 255 non-zero byte
  // values. Zero remains reserved as the empty-slot sentinel.
  const auto fingerprint_source = static_cast<std::uint16_t>(hash >> 48U);
  const auto fingerprint        = static_cast<std::uint8_t>(fingerprint_source % 255U + 1U);

  // Primary bucket from the low bits (disjoint from the fingerprint byte).
  const auto index = static_cast<size_type>(hash & mask_);
  return {index, fingerprint};
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::alt_index(size_type index, std::uint8_t fingerprint) const noexcept -> size_type {
  // XoR with a hash of the fingerprint. Masking keeps the result in range, and the
  // operation is its own inverse, so alt_index(alt_index(i, f), f) == i.
  if (mask_ == 0) {
    return 0; // A one-bucket filter cannot provide a distinct alternate bucket.
  }
  const std::uint64_t fingerprint_hash = mix_hash(static_cast<std::uint64_t>(fingerprint));
  size_type           delta            = static_cast<size_type>(fingerprint_hash & mask_);
  if (delta == 0) {
    delta = 1;
  }
  return index ^ delta;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::bucket_contains(size_type index, std::uint8_t fingerprint) const -> bool {
  const size_type base = index * kSlotsPerBucket;
  for (size_type slot = 0; slot < kSlotsPerBucket; ++slot) {
    if (buckets_[base + slot] == fingerprint) {
      return true;
    }
  }
  return false;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::insert_into_bucket(size_type index, std::uint8_t fingerprint) -> bool {
  const size_type base = index * kSlotsPerBucket;
  for (size_type slot = 0; slot < kSlotsPerBucket; ++slot) {
    if (buckets_[base + slot] == 0) {
      buckets_[base + slot] = fingerprint;
      return true;
    }
  }
  return false;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::remove_from_bucket(size_type index, std::uint8_t fingerprint) -> bool {
  const size_type base = index * kSlotsPerBucket;
  for (size_type slot = 0; slot < kSlotsPerBucket; ++slot) {
    if (buckets_[base + slot] == fingerprint) {
      buckets_[base + slot] = std::uint8_t{0};
      return true;
    }
  }
  return false;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::next_random() noexcept -> std::uint64_t {
  std::uint64_t state = rng_state_;
  state ^= state << 13U;
  state ^= state >> 7U;
  state ^= state << 17U;
  rng_state_ = state;
  return state;
}

template <typename Key, typename Hash>
auto CuckooFilter<Key, Hash>::compute_bucket_count(size_type capacity) -> size_type {
  // ceil(capacity / (slots_per_bucket * 0.95)). Split quotient and remainder
  // before scaling so even size_type::max() can be validated without overflow.
  constexpr size_type kScaleDivisor = kSlotsPerBucket * kTargetLoadNumerator;

  const size_type quotient       = capacity / kScaleDivisor;
  const size_type remainder      = capacity % kScaleDivisor;
  size_type       buckets_needed = quotient * kTargetLoadDenominator;
  buckets_needed += (remainder * kTargetLoadDenominator + kScaleDivisor - 1U) / kScaleDivisor;
  if (buckets_needed == 0) {
    buckets_needed = 1;
  }

  // Bound both std::bit_ceil and the final flat-slot multiplication.
  constexpr size_type max_bucket_count = std::bit_floor(std::numeric_limits<size_type>::max() / kSlotsPerBucket);
  if (buckets_needed > max_bucket_count) {
    throw CuckooFilterException("CuckooFilter capacity is too large to size");
  }
  return std::bit_ceil(buckets_needed);
}

} // namespace ads::probabilistic

//===---------------------------------------------------------------------------===//
