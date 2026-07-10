//===---------------------------------------------------------------------------===//
/**
 * @file Count_Min_Sketch.hpp
 * @author Costantino Lombardi
 * @brief Declaration of a Count-Min Sketch frequency-estimation structure.
 * @version 0.1
 * @date 2026-07-10
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef COUNT_MIN_SKETCH_HPP
#define COUNT_MIN_SKETCH_HPP

#include "../arrays/Dynamic_Array.hpp"

#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <numbers>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace ads::probabilistic {

using ads::arrays::DynamicArray;

///@brief Exception class for Count-Min Sketch operations.
class CountMinSketchException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Sub-linear frequency estimator for a stream of keys.
 *
 * @details A Count-Min Sketch is a 'depth' x 'width' table of counters. Each row
 *          owns an independent hash function; 'add(key, c)' adds 'c' to one
 *          counter per row (column 'h_row(key) % width'), and 'estimate(key)'
 *          returns the **minimum** of those counters. Because every collision can
 *          only inflate a counter, the estimate never underestimates the true
 *          frequency; with 'width = ceil(e / epsilon)' and
 *          'depth = ceil(ln(1 / delta))' the overestimate is at most
 *          'epsilon * N' (N = total added weight) with probability at least
 *          '1 - delta'. See 'from_estimates'.
 *
 *          This is the standard, linearly **mergeable** sketch (cash-register
 *          model, non-negative additions): two sketches with the same layout and
 *          compatible hash family add element-wise, so partial streams can be
 *          summed. The type is move-only and stores its counters in a single flat
 *          'DynamicArray'.
 *
 * @tparam Key Key type hashed into the sketch.
 * @tparam Hash Hash functor used to seed the per-row hash family.
 * @tparam Count Unsigned counter type (default 'std::uint64_t').
 */
template <typename Key, typename Hash = std::hash<Key>, typename Count = std::uint64_t>
class CountMinSketch {
  static_assert(
      std::unsigned_integral<Count> && !std::same_as<Count, bool>, "CountMinSketch Count type must be a non-bool unsigned integer");

public:
  ///@brief Type aliases for convenience.
  using key_type    = Key;
  using hasher_type = Hash;
  using count_type  = Count;
  using size_type   = std::size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs a sketch with a fixed counter table.
   * @param width Number of counters per row (columns).
   * @param depth Number of rows (independent hash functions).
   * @param hasher Hash functor used to seed the per-row hash family.
   * @throws CountMinSketchException if width or depth is zero.
   */
  explicit CountMinSketch(size_type width = kDefaultWidth, size_type depth = kDefaultDepth, Hash hasher = Hash{});

  /**
   * @brief Move constructor.
   * @param other Sketch to move from; left in an empty, queryable state.
   * @complexity Time O(1), Space O(1)
   */
  CountMinSketch(CountMinSketch&& other) noexcept(std::is_nothrow_move_constructible_v<Hash>);

  /**
   * @brief Move assignment operator.
   * @param other Sketch to move from; left in an empty, queryable state.
   * @return Reference to this sketch.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(CountMinSketch&& other) noexcept(std::is_nothrow_move_assignable_v<Hash>) -> CountMinSketch&;

  /**
   * @brief Destructor.
   * @complexity Time O(w * d), Space O(1)
   */
  ~CountMinSketch() = default;

  // Copy operations are disabled (move-only type).
  CountMinSketch(const CountMinSketch&)                    = delete;
  auto operator=(const CountMinSketch&) -> CountMinSketch& = delete;

  //===----- CORE OPERATIONS ---------------------------------------------------===//

  /**
   * @brief Adds weight to a key's frequency.
   * @param key Key to hash into the sketch.
   * @param count Non-negative weight to add (default 1).
   * @throws CountMinSketchException if the sketch was moved from or a counter
   *         would overflow.
   * @complexity Time O(depth), Space O(1)
   */
  auto add(const Key& key, Count count = 1) -> void;

  /**
   * @brief Estimates a key's accumulated frequency.
   * @param key Key to query.
   * @return The minimum counter across rows; never less than the true frequency.
   * @complexity Time O(depth), Space O(1)
   */
  [[nodiscard]] auto estimate(const Key& key) const -> Count;

  /**
   * @brief Merges another compatible sketch into this one (element-wise sum).
   * @param other Sketch with the same width, depth, and equivalent hash functor.
   * @throws CountMinSketchException if the layouts or hash families differ, or
   *         if any resulting counter would overflow.
   * @complexity Time O(w * d), Space O(1)
   * @note Stateless hashers are compatible by type. Stateful hashers must be
   *       equality-comparable and compare equal.
   */
  auto merge(const CountMinSketch& other) -> void;

  /**
   * @brief Resets every counter and the total weight to zero.
   * @complexity Time O(w * d), Space O(1)
   */
  auto clear() noexcept -> void;

  /**
   * @brief Builds a sketch sized for a target error and confidence.
   * @param epsilon Relative error factor in (0, 1): estimate <= true + epsilon*N.
   * @param delta Failure probability in (0, 1): the bound holds w.p. >= 1 - delta.
   * @param hasher Hash functor used to seed the per-row hash family.
   * @return Configured sketch with width = ceil(e/epsilon), depth = ceil(ln(1/delta)).
   * @throws CountMinSketchException if epsilon or delta is outside (0, 1), or if
   *         the requested bounds require an unsupported table size.
   */
  [[nodiscard]] static auto from_estimates(double epsilon, double delta, Hash hasher = Hash{}) -> CountMinSketch;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  ///@brief Returns the number of counters per row.
  [[nodiscard]] auto width() const noexcept -> size_type;

  ///@brief Returns the number of rows (hash functions).
  [[nodiscard]] auto depth() const noexcept -> size_type;

  ///@brief Returns the total weight added so far (N).
  [[nodiscard]] auto total_count() const noexcept -> Count;

  ///@brief Returns the additive error bound e*N/width for the current total weight.
  [[nodiscard]] auto error_bound() const noexcept -> double;

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Avalanche mixer for a raw 64-bit hash value; decorrelates the rows.
  [[nodiscard]] static auto mix_hash(std::uint64_t value) noexcept -> std::uint64_t;

  ///@brief Returns the column index for the given key in the given row.
  [[nodiscard]] auto column_for(const Key& key, size_type row) const -> size_type;

  ///@brief Validates the table dimensions and returns width * depth.
  [[nodiscard]] static auto table_size(size_type width, size_type depth) -> size_type;

  ///@brief Returns true when two sketches use the same hash family.
  [[nodiscard]] auto has_compatible_hasher(const CountMinSketch& other) const -> bool;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  static constexpr size_type kDefaultWidth = 2'048; ///< Default counters per row.
  static constexpr size_type kDefaultDepth = 5;     ///< Default number of rows.

  static constexpr double kEuler = std::numbers::e; ///< e, used in the sizing formulae.

  size_type width_;       ///< Counters per row.
  size_type depth_;       ///< Number of rows.
  Count     total_count_; ///< Total weight added (N).
  Hash      hasher_;      ///< Hash functor seeding the per-row family.

  DynamicArray<Count> counters_; ///< Flat width_*depth_ table, row-major.
};

} // namespace ads::probabilistic

#include "../../../src/ads/probabilistic/Count_Min_Sketch.tpp"

#endif // COUNT_MIN_SKETCH_HPP

//===---------------------------------------------------------------------------===//
