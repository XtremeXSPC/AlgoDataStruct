//===---------------------------------------------------------------------------===//
/**
 * @file Segment_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the SegmentTree class.
 * @version 1.0
 * @date 2026-02-05
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SEGMENT_TREE_HPP
#define SEGMENT_TREE_HPP

#include "Segment_Tree_Exception.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::trees {

namespace detail {

/**
 * @brief Default identity functor for aggregate operations.
 * @tparam Node Node type (must be default constructible).
 */
template <typename Node>
struct DefaultIdentity {
  constexpr auto operator()() const noexcept(std::is_nothrow_default_constructible_v<Node>) -> Node { return Node{}; }
};

/**
 * @brief Default leaf builder that converts a value into a node.
 * @tparam Value Input value type.
 * @tparam Node Internal node value type.
 */
template <typename Value, typename Node>
struct DefaultLeafBuilder {
  constexpr auto operator()(const Value& value) const noexcept(std::is_nothrow_constructible_v<Node, const Value&>)
      -> Node {
    return Node{value};
  }
};

/**
 * @brief Concept that validates Segment Tree functor compatibility.
 *
 * @details Requirements:
 *            - LeafBuilder must convert Value to Node
 *            - Combine must merge two Nodes into one Node
 *            - Identity must return the identity element for Combine
 *
 * @note The Combine functor MUST be associative for correct results:
 *       combine(combine(a, b), c) == combine(a, combine(b, c))
 *       Common associative operations: addition, multiplication, min, max, bitwise AND/OR/XOR.
 */
template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
concept SegmentTreeTraits = requires(Value value, Node node, Combine combine, Identity identity, LeafBuilder leaf) {
  { leaf(value) } -> std::convertible_to<Node>;
  { combine(node, node) } -> std::convertible_to<Node>;
  { identity() } -> std::convertible_to<Node>;
};

/**
 * @brief Concept for types supporting addition.
 */
template <typename T>
concept Addable = requires(T a, T b) {
  { a + b } -> std::convertible_to<T>;
};

/**
 * @brief Helper to detect noexcept for combine operation.
 */
template <typename Combine, typename Node>
inline constexpr bool is_combine_nothrow_v =
    noexcept(std::declval<Combine>()(std::declval<Node>(), std::declval<Node>()));

/**
 * @brief Helper to detect noexcept for identity operation.
 */
template <typename Identity>
inline constexpr bool is_identity_nothrow_v = noexcept(std::declval<Identity>()());

/**
 * @brief Helper to detect noexcept for leaf builder operation.
 */
template <typename LeafBuilder, typename Value>
inline constexpr bool is_leaf_builder_nothrow_v = noexcept(std::declval<LeafBuilder>()(std::declval<Value>()));

} // namespace detail

/**
 * @brief Efficient iterative Segment Tree for range queries with point updates.
 *
 * @details This implementation uses an iterative approach with exactly 2n memory
 *          (where n is the number of elements). The tree is stored implicitly in
 *          a vector using 0-based indexing for the public API.
 *
 *          **Memory Layout:**
 *            - Indices [0, n-1]: Internal nodes (index 0 unused, root at index 1)
 *            - Indices [n, 2n-1]: Leaf nodes corresponding to input values
 *
 *          **Complexity:**
 *            - Build: O(n) time, O(n) space
 *            - Point update: O(log n) time, O(1) space
 *            - Range query: O(log n) time, O(1) space
 *
 *          **Customization:**
 *              Value and Node types can differ: LeafBuilder maps Value to Node, and
 *              Combine merges Node instances during queries. This allows for complex
 *              aggregations like tracking both sum and count simultaneously.
 *
 * @note The Combine functor "MUST" be associative for correct results.
 *       Formally: combine(combine(a, b), c) == combine(a, combine(b, c))
 *
 * @tparam Value Value type stored externally (leaf values).
 * @tparam Node Internal node aggregate type (default: Value).
 * @tparam Combine Binary functor to merge two nodes (default: std::plus<Node>).
 * @tparam Identity Functor that returns the identity node for Combine.
 * @tparam LeafBuilder Functor that converts a Value into a Node.
 */
template <typename Value, typename Node = Value, typename Combine = std::plus<Node>,
          typename Identity    = detail::DefaultIdentity<Node>,
          typename LeafBuilder = detail::DefaultLeafBuilder<Value, Node>>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
class SegmentTree {
public:
  //===--------------------------- TYPE ALIASES --------------------------------===//

  using value_type      = Value;
  using node_type       = Node;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = Value&;
  using const_reference = const Value&;
  using pointer         = Value*;
  using const_pointer   = const Value*;

  //===---------------------------- ITERATOR TYPES -----------------------------===//

  /**
   * @brief Const iterator for traversing the original values.
   */
  using const_iterator         = typename std::vector<Value>::const_iterator;
  using const_reverse_iterator = typename std::vector<Value>::const_reverse_iterator;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty Segment Tree.
   * @complexity Time O(1), Space O(1)
   */
  constexpr SegmentTree() noexcept = default;

  /**
   * @brief Constructs an empty Segment Tree with custom functors.
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(1), Space O(1)
   */
  constexpr explicit SegmentTree(Combine combine, Identity identity = {}, LeafBuilder leaf_builder = {}) noexcept(
      std::is_nothrow_move_constructible_v<Combine> && std::is_nothrow_move_constructible_v<Identity>
      && std::is_nothrow_move_constructible_v<LeafBuilder>);

  /**
   * @brief Constructs a Segment Tree with the given size, default-initialized.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
   * @note Requires Value to be default constructible.
   */
  constexpr explicit SegmentTree(size_type size)
    requires std::default_initializable<Value>;

  /**
   * @brief Constructs a Segment Tree with the given size and custom functors.
   * @param size Number of elements.
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(n), Space O(n)
   * @note Requires Value to be default constructible.
   */
  constexpr SegmentTree(size_type size, Combine combine, Identity identity = {}, LeafBuilder leaf_builder = {})
    requires std::default_initializable<Value>;

  /**
   * @brief Constructs a Segment Tree with all elements set to a value.
   * @param size Number of elements.
   * @param value Initial value for each element.
   * @complexity Time O(n), Space O(n)
   */
  constexpr SegmentTree(size_type size, const Value& value);

  /**
   * @brief Constructs a Segment Tree with all elements set to a value and custom functors.
   * @param size Number of elements.
   * @param value Initial value for each element.
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(n), Space O(n)
   */
  constexpr SegmentTree(size_type size, const Value& value, Combine combine, Identity identity = {},
                        LeafBuilder leaf_builder = {});

  /**
   * @brief Constructs a Segment Tree from a vector of values (copy).
   * @param values Input values (0-based indexing).
   * @complexity Time O(n), Space O(n)
   */
  constexpr explicit SegmentTree(const std::vector<Value>& values);

  /**
   * @brief Constructs a Segment Tree from a vector of values (move).
   * @param values Input values (0-based indexing), moved from.
   * @complexity Time O(n), Space O(n)
   */
  constexpr explicit SegmentTree(std::vector<Value>&& values) noexcept(
      std::is_nothrow_move_constructible_v<std::vector<Value>>);

  /**
   * @brief Constructs a Segment Tree from a vector with custom functors (copy).
   * @param values Input values (0-based indexing).
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(n), Space O(n)
   */
  constexpr SegmentTree(const std::vector<Value>& values, Combine combine, Identity identity = {},
                        LeafBuilder leaf_builder = {});

  /**
   * @brief Constructs a Segment Tree from a vector with custom functors (move).
   * @param values Input values (0-based indexing), moved from.
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(n), Space O(n)
   */
  constexpr SegmentTree(std::vector<Value>&& values, Combine combine, Identity identity = {},
                        LeafBuilder leaf_builder = {}) noexcept(std::is_nothrow_move_constructible_v<std::vector<Value>>
                                                                && std::is_nothrow_move_constructible_v<Combine>
                                                                && std::is_nothrow_move_constructible_v<Identity>
                                                                && std::is_nothrow_move_constructible_v<LeafBuilder>);

  /**
   * @brief Constructs a Segment Tree from an initializer list.
   * @param values Input values.
   * @complexity Time O(n), Space O(n)
   */
  constexpr SegmentTree(std::initializer_list<Value> values);

  /**
   * @brief Constructs a Segment Tree from an initializer list with custom functors.
   * @param values Input values.
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(n), Space O(n)
   */
  constexpr SegmentTree(std::initializer_list<Value> values, Combine combine, Identity identity = {},
                        LeafBuilder leaf_builder = {});

  /**
   * @brief Constructs a Segment Tree from an iterator range.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  constexpr SegmentTree(InputIt first, InputIt last);

  /**
   * @brief Constructs a Segment Tree from an iterator range with custom functors.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  constexpr SegmentTree(InputIt first, InputIt last, Combine combine, Identity identity = {},
                        LeafBuilder leaf_builder = {});

  /**
   * @brief Move constructor.
   * @param other The tree to move from.
   * @complexity Time O(1), Space O(1)
   */
  constexpr SegmentTree(SegmentTree&& other) noexcept;

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
  constexpr auto operator=(SegmentTree&& other) noexcept -> SegmentTree&;

  // Copy constructor and assignment are disabled (move-only type).
  SegmentTree(const SegmentTree&)                    = delete;
  auto operator=(const SegmentTree&) -> SegmentTree& = delete;

  //===------------------------ MODIFICATION OPERATIONS ------------------------===//

  /**
   * @brief Rebuilds the tree from a vector of values (copy).
   * @param values Input values (0-based indexing).
   * @complexity Time O(n), Space O(n)
   */
  constexpr auto build(const std::vector<Value>& values) -> void;

  /**
   * @brief Rebuilds the tree from a vector of values (move).
   * @param values Input values (0-based indexing), moved from.
   * @complexity Time O(n), Space O(n)
   */
  constexpr auto build(std::vector<Value>&& values) noexcept(std::is_nothrow_move_assignable_v<std::vector<Value>>)
      -> void;

  /**
   * @brief Rebuilds the tree from an initializer list of values.
   * @param values Input values.
   * @complexity Time O(n), Space O(n)
   */
  constexpr auto build(std::initializer_list<Value> values) -> void;

  /**
   * @brief Rebuilds the tree from an iterator range.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  constexpr auto build(InputIt first, InputIt last) -> void;

  /**
   * @brief Sets the element at the given index to a new value.
   * @param index Zero-based index.
   * @param value New value.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   */
  constexpr auto set(size_type index, const Value& value) -> void;

  /**
   * @brief Sets the element at the given index to a new value (move).
   * @param index Zero-based index.
   * @param value New value, moved from.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   */
  constexpr auto set(size_type index, Value&& value) -> void;

  /**
   * @brief Adds a delta to the element at the given index.
   * @param index Zero-based index.
   * @param delta Value to add.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   * @note Requires operator+ for Value. Intended for sum trees.
   */
  constexpr auto add(size_type index, const Value& delta) -> void
    requires detail::Addable<Value>;

  /**
   * @brief Resets the tree to a given size with default values.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
   * @note Requires Value to be default constructible.
   */
  constexpr auto reset(size_type size) -> void
    requires std::default_initializable<Value>;

  /**
   * @brief Removes all elements from the tree.
   * @complexity Time O(n), Space O(1)
   */
  constexpr auto clear() noexcept -> void;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Returns the aggregate over the range [left, right].
   * @param left Zero-based left boundary (inclusive).
   * @param right Zero-based right boundary (inclusive).
   * @return The aggregated node value.
   * @throws SegmentTreeException if the range is invalid or out of bounds.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] constexpr auto range_query(size_type left, size_type right) const -> node_type;

  /**
   * @brief Returns the sum of values in the range [left, right].
   * @param left Zero-based left boundary (inclusive).
   * @param right Zero-based right boundary (inclusive).
   * @return The range sum.
   * @throws SegmentTreeException if the range is invalid or out of bounds.
   * @complexity Time O(log n), Space O(1)
   * @note Convenience alias for range_query. For non-sum aggregates, prefer range_query().
   */
  [[nodiscard]] constexpr auto range_sum(size_type left, size_type right) const -> node_type;

  /**
   * @brief Returns the aggregate of all elements in the tree.
   * @return The total aggregate (identity if empty).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto total() const noexcept(detail::is_identity_nothrow_v<Identity>) -> node_type;

  /**
   * @brief Returns the aggregate of all elements in the tree.
   * @return The total aggregate (identity if empty).
   * @complexity Time O(1), Space O(1)
   * @note Alias for total(). Kept for backwards compatibility.
   */
  [[nodiscard]] constexpr auto total_sum() const noexcept(detail::is_identity_nothrow_v<Identity>) -> node_type;

  /**
   * @brief Returns the value stored at the given index.
   * @param index Zero-based index.
   * @return Const reference to the value.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto value_at(size_type index) const -> const_reference;

  /**
   * @brief Returns the node (aggregated value) at the given leaf index.
   * @param index Zero-based index.
   * @return The node value at that leaf position.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(1), Space O(1)
   * @note Returns the Node type, not the Value type. Useful when Value != Node.
   */
  [[nodiscard]] constexpr auto node_at(size_type index) const -> node_type;

  /**
   * @brief Returns the number of elements.
   * @return The current size.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto size() const noexcept -> size_type;

  /**
   * @brief Checks if the tree is empty.
   * @return true if empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto is_empty() const noexcept -> bool;

  /**
   * @brief Alias for is_empty() for STL compatibility.
   * @return true if empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto empty() const noexcept -> bool;

  //===--------------------------- ITERATOR ACCESS -----------------------------===//

  /**
   * @brief Returns a const iterator to the first value.
   * @return Const iterator to the beginning.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator;

  /**
   * @brief Returns a const iterator past the last value.
   * @return Const iterator to the end.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto end() const noexcept -> const_iterator;

  /**
   * @brief Returns a const iterator to the first value.
   * @return Const iterator to the beginning.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator;

  /**
   * @brief Returns a const iterator past the last value.
   * @return Const iterator to the end.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator;

  /**
   * @brief Returns a const reverse iterator to the last value.
   * @return Const reverse iterator to the reverse beginning.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto rbegin() const noexcept -> const_reverse_iterator;

  /**
   * @brief Returns a const reverse iterator before the first value.
   * @return Const reverse iterator to the reverse end.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto rend() const noexcept -> const_reverse_iterator;

  /**
   * @brief Returns a const reverse iterator to the last value.
   * @return Const reverse iterator to the reverse beginning.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator;

  /**
   * @brief Returns a const reverse iterator before the first value.
   * @return Const reverse iterator to the reverse end.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator;

  //===-------------------------- FUNCTOR ACCESSORS ----------------------------===//

  /**
   * @brief Returns a const reference to the combine functor.
   * @return Const reference to the combine functor.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto get_combine() const noexcept -> const Combine&;

  /**
   * @brief Returns a const reference to the identity functor.
   * @return Const reference to the identity functor.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto get_identity() const noexcept -> const Identity&;

  /**
   * @brief Returns a const reference to the leaf builder functor.
   * @return Const reference to the leaf builder functor.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto get_leaf_builder() const noexcept -> const LeafBuilder&;

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Builds the internal segment tree from values_.
   * @complexity Time O(n), Space O(n)
   */
  constexpr auto build_tree() -> void;

  /**
   * @brief Propagates an update from a leaf to the root (iterative).
   * @param leaf_index Internal index of the leaf node (in [n, 2n-1]).
   * @complexity Time O(log n), Space O(1)
   */
  constexpr auto propagate_up(size_type leaf_index) noexcept(detail::is_combine_nothrow_v<Combine, node_type>) -> void;

  /**
   * @brief Validates that the index is within range.
   * @param index Index to validate.
   * @throws SegmentTreeException if index >= size_.
   */
  constexpr auto validate_index(size_type index) const -> void;

  /**
   * @brief Validates that the range [left, right] is valid and within bounds.
   * @param left Left boundary.
   * @param right Right boundary.
   * @throws SegmentTreeException if left > right or right >= size_.
   */
  constexpr auto validate_range(size_type left, size_type right) const -> void;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  Combine                combine_{};      ///< Functor to combine two nodes.
  Identity               identity_{};     ///< Functor to get the identity node.
  LeafBuilder            leaf_builder_{}; ///< Functor to build leaf nodes from values.
  std::vector<Value>     values_{};       ///< Original values for point access.
  std::vector<node_type> tree_{};         ///< Internal segment tree storage (size 2n).
  size_type              size_ = 0;       ///< Number of elements in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Segment_Tree.tpp"

#endif // SEGMENT_TREE_HPP

//===---------------------------------------------------------------------------===//
