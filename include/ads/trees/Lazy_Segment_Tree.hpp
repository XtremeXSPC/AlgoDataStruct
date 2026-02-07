//===---------------------------------------------------------------------------===//
/**
 * @file Lazy_Segment_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the LazySegmentTree class with range update support.
 * @version 1.0
 * @date 2026-02-05
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef LAZY_SEGMENT_TREE_HPP
#define LAZY_SEGMENT_TREE_HPP

#include "Segment_Tree_Exception.hpp"

#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::trees {

namespace detail {

/**
 * @brief Concept for lazy segment tree operations.
 *
 * @details Requirements for a valid lazy segment tree configuration:
 * - Combine: Merges two nodes (must be associative)
 * - Apply: Applies a lazy tag to a node, given the segment length
 * - Compose: Composes two lazy tags (must be associative)
 * - Identity: Returns the identity element for Combine
 *
 * @note Both Combine and Compose MUST be associative for correct results.
 */
template <typename Node, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
concept LazySegmentTreeTraits =
    requires(Node node, Tag tag, Combine combine, Apply apply, Compose compose, Identity identity, std::size_t len) {
      { combine(node, node) } -> std::convertible_to<Node>;
      { apply(node, tag, len) } -> std::convertible_to<Node>;
      { compose(tag, tag) } -> std::convertible_to<Tag>;
      { identity() } -> std::convertible_to<Node>;
    };

/**
 * @brief Default combine functor using std::plus.
 */
template <typename Node>
struct LazyDefaultCombine {
  constexpr auto operator()(const Node& left, const Node& right) const noexcept(noexcept(left + right)) -> Node {
    return left + right;
  }
};

/**
 * @brief Default apply functor for range-add lazy propagation.
 * @details Applies tag * length to node (for sum aggregation with range-add).
 */
template <typename Node, typename Tag>
struct LazyDefaultApply {
  constexpr auto operator()(const Node& node, const Tag& tag, std::size_t len) const
      noexcept(noexcept(node + tag * static_cast<Tag>(len))) -> Node {
    return node + tag * static_cast<Tag>(len);
  }
};

/**
 * @brief Default compose functor for additive tags.
 * @details Composes two additive tags by summing them.
 */
template <typename Tag>
struct LazyDefaultCompose {
  constexpr auto operator()(const Tag& old_tag, const Tag& new_tag) const noexcept(noexcept(old_tag + new_tag)) -> Tag {
    return old_tag + new_tag;
  }
};

/**
 * @brief Default identity functor.
 */
template <typename Node>
struct LazyDefaultIdentity {
  constexpr auto operator()() const noexcept(std::is_nothrow_default_constructible_v<Node>) -> Node { return Node{}; }
};

} // namespace detail

/**
 * @brief Lazy Segment Tree supporting efficient range updates and range queries.
 *
 * @details This implementation uses lazy propagation to achieve O(log n) complexity
 *          for both range updates and range queries. The tree uses a recursive
 *          approach internally to properly handle lazy tag propagation.
 *
 *          **Memory Layout:**
 *            - Uses 4n memory to accommodate the recursive structure
 *            - Each node stores both a value and an optional lazy tag
 *
 *          **Complexity:**
 *            - Build: O(n) time, O(n) space
 *            - Point update: O(log n) time, O(log n) space (stack)
 *            - Range update: O(log n) time, O(log n) space (stack)
 *            - Range query: O(log n) time, O(log n) space (stack)
 *
 *          **Default Configuration:**
 *          The default functors implement a range-sum tree with range-add updates:
 *            - Combine: addition (sum of two segments)
 *            - Apply: node + tag * length (apply additive tag to segment)
 *            - Compose: addition (compose additive tags)
 *
 * @note Both Combine and Compose functors MUST be associative.
 *       Apply must distribute over Combine: apply(combine(a,b), tag, len) ==
 *       combine(apply(a, tag, len_a), apply(b, tag, len_b)) where len = len_a + len_b
 *
 * @tparam Value Value type stored in the tree.
 * @tparam Tag Lazy tag type for deferred updates.
 * @tparam Combine Binary functor to merge two nodes.
 * @tparam Apply Functor to apply a tag to a node given segment length.
 * @tparam Compose Functor to compose two tags.
 * @tparam Identity Functor returning the identity element for Combine.
 */
template <typename Value, typename Tag = Value, typename Combine = detail::LazyDefaultCombine<Value>,
          typename Apply = detail::LazyDefaultApply<Value, Tag>, typename Compose = detail::LazyDefaultCompose<Tag>,
          typename Identity = detail::LazyDefaultIdentity<Value>>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
class LazySegmentTree {
public:
  //===----------------------------- TYPE ALIASES ------------------------------===//

  using value_type      = Value;
  using tag_type        = Tag;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty Lazy Segment Tree.
   * @complexity Time O(1), Space O(1)
   */
  constexpr LazySegmentTree() noexcept = default;

  /**
   * @brief Constructs an empty Lazy Segment Tree with custom functors.
   * @param combine Functor to merge nodes.
   * @param apply Functor to apply tags.
   * @param compose Functor to compose tags.
   * @param identity Functor for identity element.
   * @complexity Time O(1), Space O(1)
   */
  constexpr explicit LazySegmentTree(Combine combine, Apply apply = {}, Compose compose = {}, Identity identity = {});

  /**
   * @brief Constructs a Lazy Segment Tree with the given size, default-initialized.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
   */
  constexpr explicit LazySegmentTree(size_type size)
    requires std::default_initializable<Value>;

  /**
   * @brief Constructs a Lazy Segment Tree with all elements set to a value.
   * @param size Number of elements.
   * @param value Initial value for each element.
   * @complexity Time O(n), Space O(n)
   */
  constexpr LazySegmentTree(size_type size, const Value& value);

  /**
   * @brief Constructs a Lazy Segment Tree from a vector of values.
   * @param values Input values (0-based indexing).
   * @complexity Time O(n), Space O(n)
   */
  constexpr explicit LazySegmentTree(const std::vector<Value>& values);

  /**
   * @brief Constructs a Lazy Segment Tree from a vector of values (move).
   * @param values Input values, moved from.
   * @complexity Time O(n), Space O(n)
   */
  constexpr explicit LazySegmentTree(std::vector<Value>&& values);

  /**
   * @brief Constructs a Lazy Segment Tree from an initializer list.
   * @param values Input values.
   * @complexity Time O(n), Space O(n)
   */
  constexpr LazySegmentTree(std::initializer_list<Value> values);

  /**
   * @brief Constructs a Lazy Segment Tree from an iterator range.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  constexpr LazySegmentTree(InputIt first, InputIt last);

  /**
   * @brief Move constructor.
   * @param other The tree to move from.
   * @complexity Time O(1), Space O(1)
   */
  constexpr LazySegmentTree(LazySegmentTree&& other) noexcept;

  /**
   * @brief Destructor.
   */
  ~LazySegmentTree() = default;

  /**
   * @brief Move assignment operator.
   * @param other The tree to move from.
   * @return Reference to this instance.
   * @complexity Time O(1), Space O(1)
   */
  constexpr auto operator=(LazySegmentTree&& other) noexcept -> LazySegmentTree&;

  // Copy operations are disabled (move-only type).
  LazySegmentTree(const LazySegmentTree&)                    = delete;
  auto operator=(const LazySegmentTree&) -> LazySegmentTree& = delete;

  //===------------------------ MODIFICATION OPERATIONS ------------------------===//

  /**
   * @brief Rebuilds the tree from a vector of values.
   * @param values Input values (0-based indexing).
   * @complexity Time O(n), Space O(n)
   */
  constexpr auto build(const std::vector<Value>& values) -> void;

  /**
   * @brief Rebuilds the tree from a vector of values (move).
   * @param values Input values, moved from.
   * @complexity Time O(n), Space O(n)
   */
  constexpr auto build(std::vector<Value>&& values) -> void;

  /**
   * @brief Rebuilds the tree from an initializer list.
   * @param values Input values.
   * @complexity Time O(n), Space O(n)
   */
  constexpr auto build(std::initializer_list<Value> values) -> void;

  /**
   * @brief Sets the element at the given index to a new value.
   * @param index Zero-based index.
   * @param value New value.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(log n), Space O(log n)
   */
  constexpr auto set(size_type index, const Value& value) -> void;

  /**
   * @brief Applies an update to a single element.
   * @param index Zero-based index.
   * @param tag The tag to apply.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(log n), Space O(log n)
   */
  constexpr auto update(size_type index, const Tag& tag) -> void;

  /**
   * @brief Applies an update to all elements in range [left, right].
   * @param left Zero-based left boundary (inclusive).
   * @param right Zero-based right boundary (inclusive).
   * @param tag The tag to apply to the range.
   * @throws SegmentTreeException if the range is invalid or out of bounds.
   * @complexity Time O(log n), Space O(log n)
   */
  constexpr auto range_update(size_type left, size_type right, const Tag& tag) -> void;

  /**
   * @brief Resets the tree to a given size with default values.
   * @param size Number of elements.
   * @complexity Time O(n), Space O(n)
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
   * @return The aggregated value.
   * @throws SegmentTreeException if the range is invalid or out of bounds.
   * @complexity Time O(log n), Space O(log n)
   */
  [[nodiscard]] constexpr auto range_query(size_type left, size_type right) const -> value_type;

  /**
   * @brief Returns the value at a specific index.
   * @param index Zero-based index.
   * @return The value at the index.
   * @throws SegmentTreeException if index is out of range.
   * @complexity Time O(log n), Space O(log n)
   * @note This requires traversing the tree to apply pending lazy tags.
   */
  [[nodiscard]] constexpr auto value_at(size_type index) const -> value_type;

  /**
   * @brief Returns the aggregate of all elements in the tree.
   * @return The total aggregate (identity if empty).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto total() const -> value_type;

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

private:
  //===----------------------------- INTERNAL NODE -----------------------------===//

  /**
   * @brief Internal node structure with value and optional lazy tag.
   */
  struct TreeNode {
    value_type              value;
    std::optional<tag_type> lazy = std::nullopt;

    constexpr TreeNode() : value{}, lazy{} {}

    constexpr explicit TreeNode(const value_type& v) : value(v), lazy{} {}
  };

  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Builds the tree from values_.
   */
  constexpr auto build_tree() -> void;

  /**
   * @brief Recursively builds tree nodes.
   */
  constexpr auto build_node(size_type v, size_type tl, size_type tr) -> void;

  /**
   * @brief Pushes lazy tag down to children.
   */
  constexpr auto push_down(size_type v, size_type tl, size_type tr) const -> void;

  /**
   * @brief Pulls values up from children.
   */
  constexpr auto pull_up(size_type v) -> void;

  /**
   * @brief Applies a tag to a node.
   */
  constexpr auto apply_tag(size_type v, size_type len, const tag_type& tag) const -> void;

  /**
   * @brief Recursively updates a range.
   */
  constexpr auto update_range(size_type v, size_type tl, size_type tr, size_type l, size_type r, const tag_type& tag)
      -> void;

  /**
   * @brief Recursively queries a range.
   */
  [[nodiscard]] constexpr auto query_range(size_type v, size_type tl, size_type tr, size_type l, size_type r) const
      -> value_type;

  /**
   * @brief Validates index bounds.
   */
  constexpr auto validate_index(size_type index) const -> void;

  /**
   * @brief Validates range bounds.
   */
  constexpr auto validate_range(size_type left, size_type right) const -> void;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  Combine                       combine_{};  ///< Functor to combine two nodes.
  Apply                         apply_{};    ///< Functor to apply a tag to a node.
  Compose                       compose_{};  ///< Functor to compose two tags.
  Identity                      identity_{}; ///< Functor to get the identity node.
  std::vector<Value>            values_{};   ///< Original values for point access.
  mutable std::vector<TreeNode> tree_{};     ///< Mutable for lazy propagation in const queries.
  size_type                     size_ = 0;   ///< Number of elements in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Lazy_Segment_Tree.tpp"

#endif // LAZY_SEGMENT_TREE_HPP

//===---------------------------------------------------------------------------===//
