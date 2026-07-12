//===---------------------------------------------------------------------------===//
/**
 * @file Cuckoo_Filter.hpp
 * @author Costantino Lombardi
 * @brief Declaration of a Cuckoo filter probabilistic set with deletion.
 * @version 0.1
 * @date 2026-07-12
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef CUCKOO_FILTER_HPP
#define CUCKOO_FILTER_HPP

#include "../arrays/Dynamic_Array.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace ads::probabilistic {

using ads::arrays::DynamicArray;

///@brief Exception class for Cuckoo filter operations.
class CuckooFilterException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Probabilistic set membership structure that also supports deletion.
 *
 * @details A Cuckoo filter stores a short @e fingerprint (here a single non-zero
 *          byte) of every inserted key rather than the key itself. Each key maps
 *          to two candidate buckets of @c kSlotsPerBucket slots via
 *          @e partial-key cuckoo hashing: the first bucket comes from the key
 *          hash, and the second is derived from the first by XoR-ing in a hash of
 *          the fingerprint. Because that derivation is an involution, either
 *          bucket can recover the other from just the stored fingerprint, which is
 *          what lets the filter relocate entries without ever seeing the key
 *          again.
 *
 *          Membership can yield false positives (two keys may share a bucket and
 *          fingerprint) but, for keys that were inserted and not evicted, never a
 *          false negative. Unlike a Bloom filter it supports @c erase, at the cost
 *          of a subtle caveat: deleting a key that was never inserted may remove a
 *          different key that happens to collide, so only delete keys known to be
 *          present. The per-query false-positive probability is estimated by
 *          @c 2*kSlotsPerBucket/(2^kFingerprintBits-1) for small rates (see
 *          @c false_positive_rate).
 *
 *          Insertion places the fingerprint in whichever candidate bucket has a
 *          free slot; when both are full it evicts a random resident and relocates
 *          it to its own alternate bucket, repeating up to @c kMaxKicks times.
 *          Above roughly 95% load a chain can fail: @c insert then leaves the
 *          filter unchanged and returns @c false. Construction includes enough
 *          headroom to keep the requested key capacity at or below that target
 *          load. The bucket count is always a power of two (required by the XoR
 *          index trick) and the type is move-only, storing all slots in a single
 *          flat @c DynamicArray.
 *
 *          Cuckoo filters are intentionally @b not mergeable: two filters assign
 *          the same key to layout-dependent buckets, so there is no element-wise
 *          union as there is for Bloom, Count-Min, or HyperLogLog.
 *
 * @tparam Key Key type hashed into the filter.
 * @tparam Hash Hash functor used to derive the bucket index and fingerprint.
 */
template <typename Key, typename Hash = std::hash<Key>>
class CuckooFilter {
public:
  ///@brief Type aliases for convenience.
  using key_type    = Key;
  using hasher_type = Hash;
  using size_type   = std::size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs a filter sized for a target number of keys.
   * @param capacity Desired number of storable keys; the bucket count includes
   *        headroom for the target load factor and is rounded up to a power of two.
   * @param hasher Hash functor used to derive the bucket index and fingerprint.
   * @throws CuckooFilterException if the requested capacity is too large to size.
   */
  explicit CuckooFilter(size_type capacity = kDefaultCapacity, Hash hasher = Hash{});

  /**
   * @brief Move constructor.
   * @param other Filter to move from; left in an empty, queryable state.
   * @complexity Time O(1), Space O(1)
   */
  CuckooFilter(CuckooFilter&& other) noexcept(std::is_nothrow_move_constructible_v<Hash>);

  /**
   * @brief Move assignment operator.
   * @param other Filter to move from; left in an empty, queryable state.
   * @return Reference to this filter.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(CuckooFilter&& other) noexcept(std::is_nothrow_move_assignable_v<Hash>) -> CuckooFilter&;

  /**
   * @brief Destructor.
   * @complexity Time O(bucket_count * kSlotsPerBucket), Space O(1)
   */
  ~CuckooFilter() = default;

  // Copy operations are disabled (move-only type).
  CuckooFilter(const CuckooFilter&)                    = delete;
  auto operator=(const CuckooFilter&) -> CuckooFilter& = delete;

  //===----- CORE OPERATIONS ---------------------------------------------------===//

  /**
   * @brief Inserts a key into the filter.
   * @param key Key to hash into the filter.
   * @return true if the fingerprint was stored; false if the filter is too full to
   *         place it, in which case the filter is left unchanged.
   * @throws CuckooFilterException if called on a moved-from filter.
   * @complexity Time O(kMaxKicks) worst case, O(1) amortized, Space O(1)
   * @note The same key may be inserted several times (up to 2*kSlotsPerBucket
   *       copies); each @c erase removes one of them.
   */
  [[nodiscard]] auto insert(const Key& key) -> bool;

  /**
   * @brief Tests whether a key may be present.
   * @param key Key to query.
   * @return true if the key may be present, false if it is definitely absent.
   * @complexity Time O(kSlotsPerBucket), Space O(1)
   */
  [[nodiscard]] auto might_contain(const Key& key) const -> bool;

  /**
   * @brief Removes one stored copy of a key's fingerprint.
   * @param key Key whose fingerprint should be removed.
   * @return true if a matching fingerprint was found and cleared.
   * @complexity Time O(kSlotsPerBucket), Space O(1)
   * @warning Deleting a key that was never inserted may remove a colliding key and
   *          introduce a false negative for it. Only delete keys known present.
   */
  auto erase(const Key& key) -> bool;

  /**
   * @brief Clears every slot and resets the stored-fingerprint count.
   * @complexity Time O(bucket_count * kSlotsPerBucket), Space O(1)
   */
  auto clear() noexcept -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  ///@brief Returns the number of fingerprints currently stored.
  [[nodiscard]] auto size() const noexcept -> size_type;

  ///@brief Returns the maximum number of fingerprints (bucket_count * kSlotsPerBucket).
  [[nodiscard]] auto capacity() const noexcept -> size_type;

  ///@brief Returns the number of buckets (always a power of two).
  [[nodiscard]] auto bucket_count() const noexcept -> size_type;

  ///@brief Returns the fraction of slots currently filled (load factor in [0, 1]).
  [[nodiscard]] auto load_factor() const noexcept -> double;

  ///@brief Returns the estimated upper bound on the per-query false-positive rate.
  [[nodiscard]] auto false_positive_rate() const noexcept -> double;

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Avalanche mixer turning a raw hash into a well-distributed 64-bit value.
  [[nodiscard]] static auto mix_hash(std::uint64_t value) noexcept -> std::uint64_t;

  /**
   * @brief Derives the primary bucket index and non-zero fingerprint of a key.
   * @param key Key to hash.
   * @return Pair {index, fingerprint} with index in [0, bucket_count) and
   *         fingerprint in [1, 255].
   */
  [[nodiscard]] auto index_and_fingerprint(const Key& key) const -> std::pair<size_type, std::uint8_t>;

  ///@brief Returns the alternate bucket of a fingerprint given one of its buckets.
  [[nodiscard]] auto alt_index(size_type index, std::uint8_t fingerprint) const noexcept -> size_type;

  ///@brief Returns true if the bucket holds the given fingerprint in any slot.
  [[nodiscard]] auto bucket_contains(size_type index, std::uint8_t fingerprint) const -> bool;

  ///@brief Stores the fingerprint in the first free slot of the bucket, if any.
  [[nodiscard]] auto insert_into_bucket(size_type index, std::uint8_t fingerprint) -> bool;

  ///@brief Clears the first slot of the bucket holding the fingerprint, if any.
  [[nodiscard]] auto remove_from_bucket(size_type index, std::uint8_t fingerprint) -> bool;

  ///@brief Advances the internal xorshift generator and returns the next value.
  [[nodiscard]] auto next_random() noexcept -> std::uint64_t;

  ///@brief Rounds a target capacity up to a power-of-two bucket count.
  [[nodiscard]] static auto compute_bucket_count(size_type capacity) -> size_type;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  static constexpr size_type kSlotsPerBucket  = 4;     ///< Slots per bucket (fingerprints).
  static constexpr size_type kFingerprintBits = 8;     ///< Fingerprint storage width in bits.
  static constexpr size_type kMaxKicks        = 500;   ///< Eviction attempts before insert fails.
  static constexpr size_type kDefaultCapacity = 1'024; ///< Default target key capacity.

  static constexpr size_type kTargetLoadNumerator   = 95;  ///< Target load factor numerator.
  static constexpr size_type kTargetLoadDenominator = 100; ///< Target load factor denominator.

  static constexpr std::uint64_t kRngSeed = 0x9e37'79b9'7f4a'7c15ULL; ///< Fixed, non-zero PRNG seed.

  size_type     bucket_count_; ///< Number of buckets (power of two); 0 once moved from.
  size_type     mask_;         ///< bucket_count_ - 1, for fast modulo on the index.
  size_type     size_;         ///< Fingerprints currently stored.
  Hash          hasher_;       ///< Hash functor deriving index and fingerprint.
  std::uint64_t rng_state_;    ///< xorshift64 state driving random evictions.

  DynamicArray<std::uint8_t> buckets_; ///< Flat bucket_count_*kSlotsPerBucket slot array.
};

} // namespace ads::probabilistic

#include "../../../src/ads/probabilistic/Cuckoo_Filter.tpp"

#endif // CUCKOO_FILTER_HPP

//===---------------------------------------------------------------------------===//
