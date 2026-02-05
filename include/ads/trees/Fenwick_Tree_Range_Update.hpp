//===---------------------------------------------------------------------------===//
/**
 * @file Fenwick_Tree_Range_Update.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the FenwickTreeRangeUpdate class.
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef FENWICK_TREE_RANGE_UPDATE_HPP
#define FENWICK_TREE_RANGE_UPDATE_HPP

#include "Fenwick_Tree.hpp"
#include "Fenwick_Tree_Exception.hpp"

#include <cstddef>
#include <vector>

namespace ads::trees {

/**
 * @brief Fenwick Tree variant supporting range updates and point queries.
 *
 * @details This data structure supports:
 *          - Range updates: add a delta to all elements in [left, right] in O(log n)
 *          - Point queries: retrieve the value at a single index in O(log n)
 *
 *          Internally uses a difference array technique with a standard Fenwick tree.
 *          The prefix sum of the difference array gives the actual value at each position.
 *
 * @tparam T Value type satisfying FenwickElement concept.
 */
template <FenwickElement T>
class FenwickTreeRangeUpdate {
public:
  using value_type      = T;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty range-update Fenwick tree.
   * @complexity Time O(1), Space O(1)
   */
  FenwickTreeRangeUpdate();

  /**
   * @brief Constructs a range-update Fenwick tree with the given size, zero-initialized.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
   */
  explicit FenwickTreeRangeUpdate(size_t size);

  /**
   * @brief Move constructor.
   * @param other The tree to move from.
   * @complexity Time O(1), Space O(1)
   */
  FenwickTreeRangeUpdate(FenwickTreeRangeUpdate&& other) noexcept;

  /**
   * @brief Destructor.
   */
  ~FenwickTreeRangeUpdate() = default;

  /**
   * @brief Move assignment operator.
   * @param other The tree to move from.
   * @return Reference to this instance.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(FenwickTreeRangeUpdate&& other) noexcept -> FenwickTreeRangeUpdate&;

  // Copy constructor and assignment are disabled (move-only type).
  FenwickTreeRangeUpdate(const FenwickTreeRangeUpdate&)                    = delete;
  auto operator=(const FenwickTreeRangeUpdate&) -> FenwickTreeRangeUpdate& = delete;

  //===------------------------ MODIFICATION OPERATIONS ------------------------===//

  /**
   * @brief Adds a delta to all elements in the range [left, right].
   * @param left Zero-based left boundary (inclusive).
   * @param right Zero-based right boundary (inclusive).
   * @param delta Value to add to each element in the range.
   * @throws FenwickTreeException if the range is invalid or out of bounds.
   * @complexity Time O(log n), Space O(1)
   */
  auto range_add(size_t left, size_t right, const T& delta) -> void;

  /**
   * @brief Resets the tree to a given size with zeroed values.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
   */
  auto reset(size_t size) -> void;

  /**
   * @brief Removes all elements from the tree.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Returns the value at the given index.
   * @param index Zero-based index.
   * @return The value at the index (including all range updates).
   * @throws FenwickTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto point_query(size_t index) const -> T;

  /**
   * @brief Returns the number of elements.
   * @return The current size.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Checks if the tree is empty.
   * @return true if empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Returns the least significant bit (LSB) of index.
   * @param index The index.
   * @return The LSB.
   */
  static constexpr auto lsb(size_t index) noexcept -> size_t;

  /**
   * @brief Adds delta at a single position in the internal BIT.
   * @param index Zero-based index.
   * @param delta Value to add.
   */
  auto add_internal(size_t index, const T& delta) -> void;

  /**
   * @brief Computes prefix sum up to index in the internal BIT.
   * @param index Zero-based index (inclusive).
   * @return The prefix sum.
   */
  [[nodiscard]] auto prefix_sum_internal(size_t index) const -> T;

  /**
   * @brief Validates the given index.
   * @param index The index to validate.
   */
  auto validate_index(size_t index) const -> void;

  /**
   * @brief Validates the range [left, right].
   * @param left Zero-based left boundary (inclusive).
   * @param right Zero-based right boundary (inclusive).
   */
  auto validate_range(size_t left, size_t right) const -> void;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::vector<T> tree_;     ///< Internal Fenwick tree for difference array (1-based).
  size_t         size_ = 0; ///< Number of elements.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Fenwick_Tree_Range_Update.tpp"

#endif // FENWICK_TREE_RANGE_UPDATE_HPP

//===---------------------------------------------------------------------------===//
