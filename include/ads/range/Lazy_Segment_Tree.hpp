//===---------------------------------------------------------------------------===//
/**
 * @file Lazy_Segment_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of an acted-monoid Lazy Segment Tree.
 * @version 2.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef LAZY_SEGMENT_TREE_HPP
#define LAZY_SEGMENT_TREE_HPP

#include "../algebra/Range_Actions.hpp"
#include "../arrays/Dynamic_Array.hpp"
#include "Range_Concepts.hpp"
#include "Range_Exception.hpp"

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::range {

using ads::arrays::DynamicArray;

/**
 * @brief Segment Tree with lazy range actions described by an acted monoid.
 *
 * @details @p Acted owns the complete algebra: a value monoid, an action monoid,
 *          chronological action composition, and a length-aware action. The default
 *          policy provides range addition and range-sum queries. Queries never push
 *          tags or mutate the tree; pending ancestor actions are composed in local
 *          query state.
 *
 *          Modifications use a rollback journal. Every node replacement is prepared
 *          before the old node is recorded and committed with a non-throwing swap.
 *          Consequently @c set, @c apply, and @c range_apply provide the strong
 *          exception guarantee even when an algebra policy throws.
 *
 *          Construction takes O(n) time and space. Point and range operations
 *          take O(log n) time with O(log n) transactional or recursive storage.
 *
 * @tparam Value Aggregate and leaf value type.
 * @tparam Acted Acted-monoid policy. Its @c value_type must be @p Value.
 */
template <typename Value, typename Acted = algebra::RangeAddRangeSum<Value>>
requires RangeActionPolicy<Value, Acted>
class LazySegmentTree {
public:
  using value_type        = Value;
  using acted_monoid_type = Acted;
  using tag_type          = typename Acted::action_type;
  using size_type         = std::size_t;
  using difference_type   = std::ptrdiff_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /// @brief Constructs an empty tree with a default-constructed policy.
  LazySegmentTree() noexcept(std::is_nothrow_default_constructible_v<Acted>) requires std::default_initializable<Acted>
  = default;

  /// @brief Constructs an empty tree with an explicit acted-monoid policy.
  explicit LazySegmentTree(Acted acted) noexcept(std::is_nothrow_move_constructible_v<Acted>);

  /// @brief Constructs @p size value-initialized leaves. @complexity O(n)
  explicit LazySegmentTree(size_type size, Acted acted = {}) requires std::default_initializable<Value>;

  /// @brief Constructs @p size leaves equal to @p value. @complexity O(n)
  LazySegmentTree(size_type size, const Value& value, Acted acted = {});

  /// @brief Constructs from a copied vector. @complexity O(n)
  explicit LazySegmentTree(const std::vector<Value>& values, Acted acted = {});

  /// @brief Constructs from a moved vector. @complexity O(n)
  explicit LazySegmentTree(std::vector<Value>&& values, Acted acted = {});

  /// @brief Constructs from an initializer list. @complexity O(n)
  LazySegmentTree(std::initializer_list<Value> values, Acted acted = {});

  /// @brief Constructs from an input range. @complexity O(n)
  template <std::input_iterator InputIt>
  LazySegmentTree(InputIt first, InputIt last, Acted acted = {});

  /// @brief Move constructor; leaves @p other empty.
  LazySegmentTree(LazySegmentTree&& other) noexcept(std::is_nothrow_move_constructible_v<Acted>);

  /// @brief Move assignment; leaves @p other empty.
  auto operator=(LazySegmentTree&& other) noexcept(std::is_nothrow_move_assignable_v<Acted>) -> LazySegmentTree&;

  ///@brief Destroys the tree and all pending actions.
  ~LazySegmentTree() = default;

  LazySegmentTree(const LazySegmentTree&)                    = delete;
  auto operator=(const LazySegmentTree&) -> LazySegmentTree& = delete;

  //===----- MODIFICATION OPERATIONS -------------------------------------------===//

  /// @brief Rebuilds from a copied vector. @complexity O(n)
  auto build(const std::vector<Value>& values) -> void;

  /// @brief Rebuilds from a moved vector. @complexity O(n)
  auto build(std::vector<Value>&& values) -> void;

  /// @brief Rebuilds from an initializer list. @complexity O(n)
  auto build(std::initializer_list<Value> values) -> void;

  /// @brief Rebuilds from an input range. @complexity O(n)
  template <std::input_iterator InputIt>
  auto build(InputIt first, InputIt last) -> void requires std::constructible_from<Value, std::iter_reference_t<InputIt>>;

  /**
   * @brief Replaces one leaf value.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  auto set(size_type index, const Value& value) -> void;

  /**
   * @brief Applies an action to one position.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  auto apply(size_type index, const tag_type& action) -> void;

  /**
   * @brief Applies an action to the inclusive range [left, right].
   * @throws InvalidRangeException if the range is empty, reversed, or out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  auto range_apply(size_type left, size_type right, const tag_type& action) -> void;

  /// @brief Replaces the sequence with @p size value-initialized leaves. @complexity O(n)
  auto reset(size_type size) -> void requires std::default_initializable<Value>;

  /// @brief Removes every element. @complexity O(n)
  auto clear() noexcept -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Returns the aggregate over the inclusive range [@p left, @p right].
   * @throws InvalidRangeException if the range is empty, reversed, or out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  [[nodiscard]] auto range_query(size_type left, size_type right) const -> value_type;

  /**
   * @brief Returns the materialized value at @p index.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  [[nodiscard]] auto value_at(size_type index) const -> value_type;

  /// @brief Returns the whole-tree aggregate, or the value identity when empty. @complexity O(1)
  [[nodiscard]] auto total() const -> value_type;

  /// @brief Returns the number of leaves.
  [[nodiscard]] auto size() const noexcept -> size_type;

  /// @brief Returns whether the tree is empty.
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /// @brief Returns the acted-monoid policy.
  [[nodiscard]] auto get_acted_monoid() const noexcept -> const Acted&;

private:
  //===----- NODE AND JOURNAL TYPES -------------------------------------------===//

  ///@brief Aggregate and optional deferred action stored at one tree node.
  struct TreeNode {
    value_type              value;  ///< Aggregate including this node's pending action.
    std::optional<tag_type> lazy{}; ///< Action still to be propagated to the children.

    ///@brief Constructs a node with @p initial aggregate and no pending action.
    explicit TreeNode(value_type initial) : value(std::move(initial)) {}

    /**
     * @brief Member-wise swap used by transactional commits and rollbacks.
     * @details Swapping the members directly keeps the operation non-throwing
     *          under @ref RangeActionPolicy, which guarantees nothrow swaps for
     *          the value and action types but not nothrow moves for the value.
     */
    friend auto swap(TreeNode& left, TreeNode& right) noexcept -> void {
      using std::swap;
      swap(left.value, right.value);
      swap(left.lazy, right.lazy);
    }
  };

  ///@brief Previous node state recorded for transactional rollback.
  struct JournalEntry {
    size_type index;    ///< Tree-array index.
    TreeNode  previous; ///< Node state to restore.
  };

  using journal_type = DynamicArray<JournalEntry>;

  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Builds a fresh tree, then commits it atomically.
  auto rebuild(DynamicArray<Value>&& values) -> void;

  ///@brief Recursively initializes one node in a fresh tree array.
  auto build_node(DynamicArray<TreeNode>& tree, size_type node, size_type left, size_type right, const DynamicArray<Value>& values) const
      -> void;

  ///@brief Journals and replaces one node with @p replacement.
  auto replace_node(size_type node, TreeNode&& replacement, journal_type& journal) -> void;

  ///@brief Applies and composes @p action at one node.
  auto apply_tag(size_type node, size_type length, const tag_type& action, journal_type& journal) -> void;

  ///@brief Propagates a pending action to both children.
  auto push_down(size_type node, size_type left, size_type right, journal_type& journal) -> void;

  ///@brief Recomputes a node aggregate from its children.
  auto pull_up(size_type node, journal_type& journal) -> void;

  ///@brief Recursively applies an action to an inclusive query range.
  auto update_range(
      size_type       node,
      size_type       tree_left,
      size_type       tree_right,
      size_type       query_left,
      size_type       query_right,
      const tag_type& action,
      journal_type&   journal) -> void;

  ///@brief Recursively replaces one leaf and refreshes its ancestor path.
  auto set_node(size_type node, size_type left, size_type right, size_type index, const value_type& value, journal_type& journal) -> void;

  ///@brief Evaluates a range while carrying inherited actions without mutation.
  [[nodiscard]] auto query_range(
      size_type                      node,
      size_type                      tree_left,
      size_type                      tree_right,
      size_type                      query_left,
      size_type                      query_right,
      const std::optional<tag_type>& inherited) const -> value_type;

  ///@brief Restores all nodes recorded in @p journal.
  auto rollback(journal_type& journal) noexcept -> void;

  ///@brief Throws if @p index is outside the stored sequence.
  auto validate_index(size_type index) const -> void;

  ///@brief Throws if [@p left, @p right] is not a valid inclusive range.
  auto validate_range(size_type left, size_type right) const -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  [[no_unique_address]] Acted acted_;    ///< Stored acted-monoid policy.
  DynamicArray<TreeNode>      tree_{};   ///< One-based recursive tree storage.
  size_type                   size_ = 0; ///< Number of logical leaves.
};

} // namespace ads::range

#include "../../../src/ads/range/Lazy_Segment_Tree.tpp"

#endif // LAZY_SEGMENT_TREE_HPP

//===---------------------------------------------------------------------------===//
