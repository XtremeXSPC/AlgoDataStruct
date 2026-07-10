//===---------------------------------------------------------------------------===//
/**
 * @file HyperLogLog.hpp
 * @author Costantino Lombardi
 * @brief Declaration of a HyperLogLog cardinality-estimation structure.
 * @version 0.1
 * @date 2026-07-10
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef HYPERLOGLOG_HPP
#define HYPERLOGLOG_HPP

#include "../arrays/Dynamic_Array.hpp"

#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace ads::probabilistic {

using ads::arrays::DynamicArray;

///@brief Exception class for HyperLogLog operations.
class HyperLogLogException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Sub-linear cardinality estimator for a stream of keys.
 *
 * @details A HyperLogLog counts the number of "distinct" keys in a stream using
 *          only 'm = 2^precision' small registers. Each key is hashed to 64 bits;
 *          the top 'precision' bits select a register and the remaining bits give
 *          'rho', the position of the leftmost set bit (plus one). Every register
 *          keeps the maximum 'rho' it has seen, and the cardinality is recovered
 *          from the harmonic mean of '2^register' across all registers, scaled by
 *          a bias constant 'alpha_m'. The standard error is about '1.04 / sqrt(m)'.
 *
 *          This implementation uses a 64-bit hash, so the classic large-range
 *          correction (for a 32-bit hash space) is unnecessary and omitted; only
 *          the small-range "linear counting" correction is applied when many
 *          registers are still zero. The sketch is linearly "mergeable" (the
 *          union of two sketches is the register-wise maximum), move-only, and
 *          stores its registers in a single flat 'DynamicArray'.
 *
 * @tparam Key Key type hashed into the sketch.
 * @tparam Hash Hash functor used to seed the 64-bit hash.
 */
template <typename Key, typename Hash = std::hash<Key>>
class HyperLogLog {
public:
  ///@brief Type aliases for convenience.
  using key_type    = Key;
  using hasher_type = Hash;
  using size_type   = std::size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs a sketch with 2^precision registers.
   * @param precision Number of index bits; register count is 2^precision.
   * @param hasher Hash functor used to seed the 64-bit hash.
   * @throws HyperLogLogException if precision is outside [kMinPrecision, kMaxPrecision].
   */
  explicit HyperLogLog(size_type precision = kDefaultPrecision, Hash hasher = Hash{});

  /**
   * @brief Move constructor.
   * @param other Sketch to move from; left in an empty, queryable state.
   * @complexity Time O(1), Space O(1)
   */
  HyperLogLog(HyperLogLog&& other) noexcept(std::is_nothrow_move_constructible_v<Hash>);

  /**
   * @brief Move assignment operator.
   * @param other Sketch to move from; left in an empty, queryable state.
   * @return Reference to this sketch.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(HyperLogLog&& other) noexcept(std::is_nothrow_move_assignable_v<Hash>) -> HyperLogLog&;

  /**
   * @brief Destructor.
   * @complexity Time O(m), Space O(1)
   */
  ~HyperLogLog() = default;

  // Copy operations are disabled (move-only type).
  HyperLogLog(const HyperLogLog&)                    = delete;
  auto operator=(const HyperLogLog&) -> HyperLogLog& = delete;

  //===----- CORE OPERATIONS ---------------------------------------------------===//

  /**
   * @brief Observes a key, updating the register for its bucket.
   * @param key Key to hash into the sketch.
   * @throws HyperLogLogException if the sketch was moved from.
   * @complexity Time O(1), Space O(1)
   */
  auto add(const Key& key) -> void;

  /**
   * @brief Estimates the number of distinct keys observed so far.
   * @return The estimated cardinality, saturated at the largest representable size.
   * @complexity Time O(m), Space O(1)
   */
  [[nodiscard]] auto estimate() const -> size_type;

  /**
   * @brief Merges another compatible sketch into this one (register-wise maximum).
   * @param other Sketch with the same precision and equivalent hash functor.
   * @throws HyperLogLogException if the precisions or hash families differ.
   * @complexity Time O(m), Space O(1)
   * @note Stateless hashers are compatible by type. Stateful hashers must be
   *       equality-comparable and compare equal.
   */
  auto merge(const HyperLogLog& other) -> void;

  /**
   * @brief Resets every register to zero.
   * @complexity Time O(m), Space O(1)
   */
  auto clear() noexcept -> void;

  /**
   * @brief Builds a sketch sized for a target relative error.
   * @param relative_error Desired standard error in (0, 1): about 1.04/sqrt(m).
   * @param hasher Hash functor used to seed the 64-bit hash.
   * @return Configured sketch with precision = ceil(2*log2(1.04/relative_error)),
   *         clamped to [kMinPrecision, kMaxPrecision].
   * @throws HyperLogLogException if relative_error is outside (0, 1).
   */
  [[nodiscard]] static auto from_error(double relative_error, Hash hasher = Hash{}) -> HyperLogLog;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  ///@brief Returns the precision (number of index bits).
  [[nodiscard]] auto precision() const noexcept -> size_type;

  ///@brief Returns the number of registers (m = 2^precision).
  [[nodiscard]] auto register_count() const noexcept -> size_type;

  ///@brief Returns the theoretical standard error 1.04/sqrt(m).
  [[nodiscard]] auto relative_error() const noexcept -> double;

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Avalanche mixer turning a raw hash into a well-distributed 64-bit value.
  [[nodiscard]] static auto mix_hash(std::uint64_t value) noexcept -> std::uint64_t;

  /**
   * @brief Splits a hashed key into its register index and rho value.
   * @param key Key to hash.
   * @return Pair {index, rho}: index in [0, m), rho = leftmost-set-bit position + 1.
   */
  [[nodiscard]] auto bucket_and_rho(const Key& key) const -> std::pair<size_type, std::uint8_t>;

  ///@brief Returns the raw HyperLogLog estimate before the small-range correction.
  [[nodiscard]] auto raw_estimate() const -> double;

  ///@brief Returns the bias-correction constant alpha_m for the current register count.
  [[nodiscard]] auto alpha() const noexcept -> double;

  ///@brief Validates a precision value and returns it unchanged.
  [[nodiscard]] static auto validate_precision(size_type precision) -> size_type;

  ///@brief Returns true when two sketches use the same hash family.
  [[nodiscard]] auto has_compatible_hasher(const HyperLogLog& other) const -> bool;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  static constexpr size_type kMinPrecision     = 4;  ///< Smallest supported precision.
  static constexpr size_type kMaxPrecision     = 18; ///< Largest supported precision.
  static constexpr size_type kDefaultPrecision = 14; ///< Default precision (m=16384, ~0.81% error).

  static constexpr double kErrorConstant = 1.04; ///< Standard-error constant 1.04/sqrt(m).

  size_type precision_;  ///< Number of index bits.
  size_type register_m_; ///< Register count m = 2^precision_.
  Hash      hasher_;     ///< Hash functor seeding the 64-bit hash.

  DynamicArray<std::uint8_t> registers_; ///< Flat array of m registers, each a max rho.
};

} // namespace ads::probabilistic

#include "../../../src/ads/probabilistic/HyperLogLog.tpp"

#endif // HYPERLOGLOG_HPP

//===---------------------------------------------------------------------------===//
