//===---------------------------------------------------------------------------===//
/**
 * @file Fenwick_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the FenwickTree class (Binary Indexed Tree).
 * @version 0.1
 * @date 2026-01-28
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef FENWICK_TREE_HPP
#define FENWICK_TREE_HPP

#include "Fenwick_Tree_Exception.hpp"
#include "Tree_Concepts.hpp"

#include <bit>
#include <cstddef>
#include <initializer_list>
#include <utility>
#include <vector>

namespace ads::trees {

/**
 * @brief Fenwick Tree (Binary Indexed Tree) for efficient prefix and range sums.
 *
 * @details This data structure supports point updates and prefix sum queries in
 *          O(log n) time, with linear space. The public API is 0-based, while the
 *          internal tree uses 1-based indexing. A copy of the original values is
 *          stored to enable value_at() and set() operations.
 *
 * @tparam T Value type satisfying FenwickElement concept.
 */
template <FenwickElement T>
class FenwickTree {
public:
  using value_type      = T;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = T&;
  using const_reference = const T&;
  using pointer         = T*;
  using const_pointer   = const T*;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty Fenwick tree.
   * @complexity Time O(1), Space O(1)
   */
  FenwickTree();

  /**
   * @brief Constructs a Fenwick tree with the given size, zero-initialized.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
   * @note Requires T to be default constructible.
   */
  explicit FenwickTree(size_t size);

  /**
   * @brief Constructs a Fenwick tree with all elements set to a value.
   * @param size Number of elements.
   * @param value Initial value for each element.
   * @complexity Time O(n), Space O(n)
   */
  FenwickTree(size_t size, const T& value);

  /**
   * @brief Constructs a Fenwick tree from a vector of values.
   * @param values Input values (0-based indexing).
   * @complexity Time O(n), Space O(n)
   */
  explicit FenwickTree(const std::vector<T>& values);

  /**
   * @brief Constructs a Fenwick tree from an initializer list.
   * @param values Input values.
   * @complexity Time O(n), Space O(n)
   */
  FenwickTree(std::initializer_list<T> values);

  /**
   * @brief Move constructor.
   * @param other The tree to move from.
   * @complexity Time O(1), Space O(1)
   */
  FenwickTree(FenwickTree&& other) noexcept;

  /**
   * @brief Destructor.
   */
  ~FenwickTree() = default;

  /**
   * @brief Move assignment operator.
   * @param other The tree to move from.
   * @return Reference to this instance.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(FenwickTree&& other) noexcept -> FenwickTree&;

  // Copy constructor and assignment are disabled (move-only type).
  FenwickTree(const FenwickTree&)                    = delete;
  auto operator=(const FenwickTree&) -> FenwickTree& = delete;

  //===------------------------ MODIFICATION OPERATIONS ------------------------===//

  /**
   * @brief Rebuilds the tree from a vector of values.
   * @param values Input values (0-based indexing).
   * @complexity Time O(n), Space O(n)
   */
  auto build(const std::vector<T>& values) -> void;

  /**
   * @brief Rebuilds the tree from an initializer list of values.
   * @param values Input values.
   * @complexity Time O(n), Space O(n)
   */
  auto build(std::initializer_list<T> values) -> void;

  /**
   * @brief Adds a delta to the element at the given index.
   * @param index Zero-based index.
   * @param delta Value to add.
   * @throws FenwickTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   */
  auto add(size_t index, const T& delta) -> void;

  /**
   * @brief Sets the element at the given index to a new value.
   * @param index Zero-based index.
   * @param value New value.
   * @throws FenwickTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   * @note Requires operator- for T to compute the delta.
   */
  auto set(size_t index, const T& value) -> void;

  /**
   * @brief Resets the tree to a given size with zeroed values.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
   * @note Requires T to be default constructible.
   */
  auto reset(size_t size) -> void;

  /**
   * @brief Removes all elements from the tree.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Returns the prefix sum in the range [0, index].
   * @param index Zero-based index (inclusive).
   * @return The prefix sum.
   * @throws FenwickTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto prefix_sum(size_t index) const -> T;

  /**
   * @brief Returns the sum of values in the range [left, right].
   * @param left Zero-based left boundary (inclusive).
   * @param right Zero-based right boundary (inclusive).
   * @return The range sum.
   * @throws FenwickTreeException if the range is invalid or out of bounds.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto range_sum(size_t left, size_t right) const -> T;

  /**
   * @brief Returns the sum of all elements in the tree.
   * @return The total sum (zero if empty).
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto total_sum() const -> T;

  /**
   * @brief Returns the value stored at the given index.
   * @param index Zero-based index.
   * @return Const reference to the value.
   * @throws FenwickTreeException if index is out of range.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto value_at(size_t index) const -> const T&;

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

  /**
   * @brief Finds the smallest index where prefix_sum(index) >= target_sum.
   * @param target_sum The target sum to search for.
   * @return The smallest index satisfying the condition, or size() if not found.
   * @complexity Time O(log n), Space O(1)
   * @note Assumes all values are non-negative. For negative values, behavior is undefined.
   */
  [[nodiscard]] auto lower_bound(const T& target_sum) const -> size_t;

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Returns the least significant bit (LSB) of index.
   * @param index The index.
   * @return The LSB.
   */
  static constexpr auto lsb(size_t index) noexcept -> size_t;

  /**
   * @brief Builds the internal Fenwick tree from values_.
   */
  auto build_tree() -> void;

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

  std::vector<T> values_;   ///< Original values for value_at() and set().
  std::vector<T> tree_;     ///< Internal Fenwick tree (1-based indexing).
  size_t         size_ = 0; ///< Number of elements.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Fenwick_Tree.tpp"

#endif // FENWICK_TREE_HPP

//===---------------------------------------------------------------------------===//
