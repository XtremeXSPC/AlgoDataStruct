//===---------------------------------------------------------------------------===//
/**
 * @file Segment_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the SegmentTree class.
 * @version 0.2
 * @date 2026-02-04
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SEGMENT_TREE_HPP
#define SEGMENT_TREE_HPP

#include <concepts>
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
 * @tparam Node Node type (must be default constructible).
 */
template <typename Node>
struct DefaultIdentity {
  constexpr auto operator()() const -> Node { return Node{}; }
};

/**
 * @brief Default leaf builder that converts a value into a node.
 * @tparam Value Input value type.
 * @tparam Node Internal node value type.
 */
template <typename Value, typename Node>
struct DefaultLeafBuilder {
  constexpr auto operator()(const Value& value) const -> Node { return Node{value}; }
};

/**
 * @brief Concept that validates Segment Tree functor compatibility.
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
concept SegmentTreeAddable = requires(T a, T b) {
  { a + b } -> std::convertible_to<T>;
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
 *          Value and Node types can differ: LeafBuilder maps Value to Node, and
 *          Combine merges Node instances during queries.
 *
 * @tparam Value Value type stored externally (leaf values).
 * @tparam Node Internal node aggregate type (default: Value).
 * @tparam Combine Binary functor to merge two nodes (default: std::plus<Node>).
 * @tparam Identity Functor that returns the identity node for Combine.
 * @tparam LeafBuilder Functor that converts a Value into a Node.
 */
template <
    typename Value,
    typename Node        = Value,
    typename Combine     = std::plus<Node>,
    typename Identity    = detail::DefaultIdentity<Node>,
    typename LeafBuilder = detail::DefaultLeafBuilder<Value, Node>>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
class SegmentTree {
public:
  using value_type      = Value;
  using node_type       = Node;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = Value&;
  using const_reference = const Value&;
  using pointer         = Value*;
  using const_pointer   = const Value*;

  /**
   * @brief Node stored in the internal tree.
   * @details Keeping a dedicated node type makes it easier to extend
   *          the data structure later (e.g., adding lazy tags).
   */
  struct TreeNode {
    node_type value;

    explicit TreeNode(const node_type& node_value) : value(node_value) {}
    explicit TreeNode(node_type&& node_value) : value(std::move(node_value)) {}
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty Segment Tree.
   * @complexity Time O(1), Space O(1)
   */
  SegmentTree();

  /**
   * @brief Constructs an empty Segment Tree with custom functors.
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(1), Space O(1)
   */
  explicit SegmentTree(Combine combine, Identity identity = {}, LeafBuilder leaf_builder = {});

  /**
   * @brief Constructs a Segment Tree with the given size, default-initialized.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
   * @note Requires Value to be default constructible.
   */
  explicit SegmentTree(size_t size)
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
  SegmentTree(size_t size, Combine combine, Identity identity = {}, LeafBuilder leaf_builder = {})
    requires std::default_initializable<Value>;

  /**
   * @brief Constructs a Segment Tree with all elements set to a value.
   * @param size Number of elements.
   * @param value Initial value for each element.
   * @complexity Time O(n), Space O(n)
   */
  SegmentTree(size_t size, const Value& value);

  /**
   * @brief Constructs a Segment Tree with all elements set to a value and custom functors.
   * @param size Number of elements.
   * @param value Initial value for each element.
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(n), Space O(n)
   */
  SegmentTree(size_t size, const Value& value, Combine combine, Identity identity = {}, LeafBuilder leaf_builder = {});

  /**
   * @brief Constructs a Segment Tree from a vector of values.
   * @param values Input values (0-based indexing).
   * @complexity Time O(n), Space O(n)
   */
  explicit SegmentTree(const std::vector<Value>& values);

  /**
   * @brief Constructs a Segment Tree from a vector with custom functors.
   * @param values Input values (0-based indexing).
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(n), Space O(n)
   */
  SegmentTree(const std::vector<Value>& values, Combine combine, Identity identity = {}, LeafBuilder leaf_builder = {});

  /**
   * @brief Constructs a Segment Tree from an initializer list.
   * @param values Input values.
   * @complexity Time O(n), Space O(n)
   */
  SegmentTree(std::initializer_list<Value> values);

  /**
   * @brief Constructs a Segment Tree from an initializer list with custom functors.
   * @param values Input values.
   * @param combine Functor used to merge nodes.
   * @param identity Functor that returns the identity node.
   * @param leaf_builder Functor that converts a value into a node.
   * @complexity Time O(n), Space O(n)
   */
  SegmentTree(std::initializer_list<Value> values, Combine combine, Identity identity = {}, LeafBuilder leaf_builder = {});

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
  auto build(const std::vector<Value>& values) -> void;

  /**
   * @brief Rebuilds the tree from an initializer list of values.
   * @param values Input values.
   * @complexity Time O(n), Space O(n)
   */
  auto build(std::initializer_list<Value> values) -> void;

  /**
   * @brief Sets the element at the given index to a new value.
   * @param index Zero-based index.
   * @param value New value.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   */
  auto set(size_t index, const Value& value) -> void;

  /**
   * @brief Adds a delta to the element at the given index.
   * @param index Zero-based index.
   * @param delta Value to add.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(log n), Space O(1)
   * @note Requires operator+ for Value. Intended for sum trees.
   */
  auto add(size_t index, const Value& delta) -> void
    requires detail::SegmentTreeAddable<Value>;

  /**
   * @brief Resets the tree to a given size with default values.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
   * @note Requires Value to be default constructible.
   */
  auto reset(size_t size) -> void
    requires std::default_initializable<Value>;

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
  [[nodiscard]] auto range_query(size_t left, size_t right) const -> node_type;

  /**
   * @brief Returns the sum of values in the range [left, right].
   * @param left Zero-based left boundary (inclusive).
   * @param right Zero-based right boundary (inclusive).
   * @return The range sum.
   * @throws SegmentTreeException if the range is invalid or out of bounds.
   * @complexity Time O(log n), Space O(1)
   * @note Convenience alias for range_query. For non-sum aggregates, prefer range_query().
   */
  [[nodiscard]] auto range_sum(size_t left, size_t right) const -> node_type;

  /**
   * @brief Returns the aggregate of all elements in the tree.
   * @return The total aggregate (identity if empty).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto total_sum() const -> node_type;

  /**
   * @brief Returns the value stored at the given index.
   * @param index Zero-based index.
   * @return Const reference to the value.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto value_at(size_t index) const -> const Value&;

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
  auto update_node(size_t v, size_t tl, size_t tr, size_t index, const Value& value) -> void;

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
  [[nodiscard]] auto query_node(size_t v, size_t tl, size_t tr, size_t l, size_t r) const -> TreeNode;

  /** @brief Creates a leaf node with the given value. */
  [[nodiscard]] auto make_leaf(const Value& value) const -> TreeNode;

  /** @brief Combines two nodes using the Combine functor. */
  [[nodiscard]] auto combine_nodes(const TreeNode& left, const TreeNode& right) const -> TreeNode;

  /** @brief Returns the identity node. */
  [[nodiscard]] auto identity_node() const -> TreeNode;

  /** @brief Validates that the index is within range. */
  auto validate_index(size_t index) const -> void;

  /** @brief Validates that the range [left, right] is valid and within bounds. */
  auto validate_range(size_t left, size_t right) const -> void;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  Combine               combine_{};      ///< Functor to combine two nodes.
  Identity              identity_{};     ///< Functor to get the identity node.
  LeafBuilder           leaf_builder_{}; ///< Functor to build leaf nodes from values.
  std::vector<Value>    values_{};       ///< Original values for point updates.
  std::vector<TreeNode> tree_{};         ///< Internal segment tree storage.
  size_t                size_ = 0;       ///< Number of elements in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Segment_Tree.tpp"

#endif // SEGMENT_TREE_HPP

//===---------------------------------------------------------------------------===//
