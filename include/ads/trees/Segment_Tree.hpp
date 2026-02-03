//===---------------------------------------------------------------------------===//
/**
 * @file Segment_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the SegmentTree class.
 * @version 0.1
 * @date 2026-02-03
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SEGMENT_TREE_HPP
#define SEGMENT_TREE_HPP

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <utility>
#include <vector>

#include "Segment_Tree_Exception.hpp"

namespace ads::trees {

namespace detail {

/**
 * @brief Default identity functor for aggregate operations.
 * @tparam T Value type (must be default constructible).
 */
template <typename T>
struct DefaultIdentity {
  constexpr auto operator()() const -> T { return T{}; }
};

} // namespace detail

/**
 * @brief Segment Tree for range queries with point updates.
 *
 * @details This implementation stores the tree implicitly in a vector and
 *          uses 0-based indices for the public API. The default behavior
 *          aggregates values using std::plus (range sum). The internal Node
 *          and combine helpers are intentionally factored to make future
 *          extensions (e.g., lazy propagation, custom aggregates) straightforward.
 *
 * @tparam T Value type. Must be default constructible for the identity value.
 * @tparam Combine Binary functor to merge two values (default: std::plus<T>).
 * @tparam Identity Functor that returns the identity value for Combine.
 */
template <typename T, typename Combine = std::plus<T>, typename Identity = detail::DefaultIdentity<T>>
class SegmentTree {
public:
  using value_type      = T;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = T&;
  using const_reference = const T&;
  using pointer         = T*;
  using const_pointer   = const T*;

  /**
   * @brief Node stored in the internal tree.
   * @details Keeping a dedicated node type makes it easier to extend
   *          the data structure later (e.g., adding lazy tags).
   */
  struct Node {
    T value{};
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty Segment Tree.
   * @complexity Time O(1), Space O(1)
   */
  SegmentTree();

  /**
   * @brief Constructs a Segment Tree with the given size, identity-initialized.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
   */
  explicit SegmentTree(size_t size);

  /**
   * @brief Constructs a Segment Tree with all elements set to a value.
   * @param size Number of elements.
   * @param value Initial value for each element.
   * @complexity Time O(n), Space O(n)
   */
  SegmentTree(size_t size, const T& value);

  /**
   * @brief Constructs a Segment Tree from a vector of values.
   * @param values Input values (0-based indexing).
   * @complexity Time O(n), Space O(n)
   */
  explicit SegmentTree(const std::vector<T>& values);

  /**
   * @brief Constructs a Segment Tree from an initializer list.
   * @param values Input values.
   * @complexity Time O(n), Space O(n)
   */
  SegmentTree(std::initializer_list<T> values);

  /**
   * @brief Move constructor.
   * @param other The tree to move from.
   * @complexity Time O(1), Space O(1)
   */
  SegmentTree(SegmentTree&& other) noexcept;

  /**
   * @brief Destructor.
   */
  ~SegmentTree() = default;

  /**
   * @brief Move assignment operator.
   * @param other The tree to move from.
   * @return Reference to this instance.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(SegmentTree&& other) noexcept -> SegmentTree&;

  // Copy constructor and assignment are disabled (move-only type).
  SegmentTree(const SegmentTree&)                    = delete;
  auto operator=(const SegmentTree&) -> SegmentTree& = delete;

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
   * @brief Sets the element at the given index to a new value.
   * @param index Zero-based index.
   * @param value New value.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   */
  auto set(size_t index, const T& value) -> void;

  /**
   * @brief Adds a delta to the element at the given index.
   * @param index Zero-based index.
   * @param delta Value to add.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   * @note Requires operator+ for T. Intended for sum trees.
   */
  auto add(size_t index, const T& delta) -> void;

  /**
   * @brief Resets the tree to a given size with identity values.
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
   * @brief Returns the aggregate over the range [left, right].
   * @param left Zero-based left boundary (inclusive).
   * @param right Zero-based right boundary (inclusive).
   * @return The aggregated value.
   * @throws SegmentTreeException if the range is invalid or out of bounds.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto range_query(size_t left, size_t right) const -> T;

  /**
   * @brief Returns the sum of values in the range [left, right].
   * @param left Zero-based left boundary (inclusive).
   * @param right Zero-based right boundary (inclusive).
   * @return The range sum.
   * @throws SegmentTreeException if the range is invalid or out of bounds.
   * @complexity Time O(log n), Space O(1)
   * @note Convenience alias for range_query with std::plus.
   */
  [[nodiscard]] auto range_sum(size_t left, size_t right) const -> T;

  /**
   * @brief Returns the aggregate of all elements in the tree.
   * @return The total aggregate (identity if empty).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto total_sum() const -> T;

  /**
   * @brief Returns the value stored at the given index.
   * @param index Zero-based index.
   * @return Const reference to the value.
   * @throws SegmentTreeException if index is out of range.
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

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Builds the internal segment tree from values_.
   * @complexity Time O(n), Space O(n)
   */
  auto build_tree() -> void;

  /**
   * @brief Recursively builds the tree nodes.
   * @param v Current tree node index.
   * @param tl Left boundary of the segment.
   * @param tr Right boundary of the segment.
   * @complexity Time O(n), Space O(log n) due to recursion stack.
   */
  auto build_node(size_t v, size_t tl, size_t tr) -> void;

  /**
   * @brief Recursively updates a node at the given index.
   * @param v Current tree node index.
   * @param tl Left boundary of the segment.
   * @param tr Right boundary of the segment.
   * @param index Index to update.
   * @param value New value.
   * @complexity Time O(log n), Space O(log n) due to recursion stack.
   */
  auto update_node(size_t v, size_t tl, size_t tr, size_t index, const T& value) -> void;

  /**
   * @brief Recursively queries the aggregate over a range.
   * @param v Current tree node index.
   * @param tl Left boundary of the segment.
   * @param tr Right boundary of the segment.
   * @param l Query left boundary.
   * @param r Query right boundary.
   * @return The aggregated Node over [l, r].
   * @complexity Time O(log n), Space O(log n) due to recursion stack.
   */
  [[nodiscard]] auto query_node(size_t v, size_t tl, size_t tr, size_t l, size_t r) const -> Node;

  /** @brief Creates a leaf node with the given value. */
  [[nodiscard]] auto make_leaf(const T& value) const -> Node;

  /** @brief Combines two nodes using the Combine functor. */
  [[nodiscard]] auto combine_nodes(const Node& left, const Node& right) const -> Node;

  /** @brief Returns the identity node. */
  [[nodiscard]] auto identity_node() const -> Node;

  /** @brief Validates that the index is within range. */
  auto validate_index(size_t index) const -> void;

  /** @brief Validates that the range [left, right] is valid and within bounds. */
  auto validate_range(size_t left, size_t right) const -> void;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  Combine           combine_{};  ///< Functor to combine two values.
  Identity          identity_{}; ///< Functor to get the identity value.
  std::vector<T>    values_{};   ///< Original values for point updates.
  std::vector<Node> tree_{};     ///< Internal segment tree storage.
  size_t            size_ = 0;   ///< Number of elements in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Segment_Tree.tpp"

#endif // SEGMENT_TREE_HPP

//===---------------------------------------------------------------------------===//
