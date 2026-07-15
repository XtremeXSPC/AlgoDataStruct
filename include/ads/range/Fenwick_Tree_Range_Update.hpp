//===---------------------------------------------------------------------------===//
/**
 * @file Fenwick_Tree_Range_Update.hpp
 * @author Costantino Lombardi
 * @brief Declaration of a range-action Fenwick Tree over a commutative group.
 * @version 1.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef FENWICK_TREE_RANGE_UPDATE_HPP
#define FENWICK_TREE_RANGE_UPDATE_HPP

#include "Fenwick_Tree.hpp"

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::range {

/**
 * @brief Fenwick Tree supporting range group actions and point queries.
 *
 * @details The tree stores the group-valued difference sequence. Applying a value
 *          to [left,right] combines it at @p left and combines its inverse at
 *          @c right+1. Both internal changes share a rollback journal, so a failed
 *          update leaves the complete structure unchanged.
 *
 *          Construction and rebuild take O(n) time and space. Range actions and
 *          point queries take O(log n) time.
 *
 * @tparam Value Stored and action value type.
 * @tparam Group Commutative-group policy whose @c value_type is @p Value.
 */
template <typename Value, typename Group = algebra::AddGroup<Value>>
requires FenwickPolicy<Value, Group>
class FenwickTreeRangeUpdate {
public:
  using value_type      = Value;
  using group_type      = Group;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  ///@brief Constructs an empty tree with a default-constructed group.
  FenwickTreeRangeUpdate() noexcept(std::is_nothrow_default_constructible_v<Group>) requires std::default_initializable<Group>
  = default;

  ///@brief Constructs an empty tree with explicit @p group policy.
  explicit FenwickTreeRangeUpdate(Group group) noexcept(std::is_nothrow_move_constructible_v<Group>);

  ///@brief Constructs @p size elements initialized to the group identity.
  explicit FenwickTreeRangeUpdate(size_type size, Group group = {});

  ///@brief Constructs @p size copies of @p value.
  FenwickTreeRangeUpdate(size_type size, const Value& value, Group group = {});

  ///@brief Constructs from a copied vector in O(n) time.
  explicit FenwickTreeRangeUpdate(const std::vector<Value>& values, Group group = {});

  ///@brief Constructs from a moved vector in O(n) time.
  explicit FenwickTreeRangeUpdate(std::vector<Value>&& values, Group group = {});

  ///@brief Constructs from the input range [@p first, @p last) in O(n) time.
  template <std::input_iterator InputIt>
  FenwickTreeRangeUpdate(InputIt first, InputIt last, Group group = {})
      requires std::constructible_from<Value, std::iter_reference_t<InputIt>>;

  ///@brief Constructs from an initializer list in O(n) time.
  FenwickTreeRangeUpdate(std::initializer_list<Value> values, Group group = {});

  ///@brief Move constructor; leaves @p other empty.
  FenwickTreeRangeUpdate(FenwickTreeRangeUpdate&& other) noexcept(std::is_nothrow_move_constructible_v<Group>);

  ///@brief Move assignment; leaves @p other empty.
  auto operator=(FenwickTreeRangeUpdate&& other) noexcept(std::is_nothrow_move_assignable_v<Group>) -> FenwickTreeRangeUpdate&;

  ///@brief Destroys the tree and its contiguous storage.
  ~FenwickTreeRangeUpdate() = default;

  FenwickTreeRangeUpdate(const FenwickTreeRangeUpdate&)                    = delete;
  auto operator=(const FenwickTreeRangeUpdate&) -> FenwickTreeRangeUpdate& = delete;

  //===----- MODIFICATION OPERATIONS -------------------------------------------===//

  ///@brief Rebuilds the tree from a copied vector in O(n) time.
  auto build(const std::vector<Value>& values) -> void;

  ///@brief Rebuilds the tree from a moved vector in O(n) time.
  auto build(std::vector<Value>&& values) -> void;

  ///@brief Rebuilds the tree from the input range [@p first, @p last) in O(n) time.
  template <std::input_iterator InputIt>
  auto build(InputIt first, InputIt last) -> void requires std::constructible_from<Value, std::iter_reference_t<InputIt>>;

  ///@brief Rebuilds the tree from an initializer list in O(n) time.
  auto build(std::initializer_list<Value> values) -> void;

  /**
   * @brief Combines @p delta with every element in [left, right].
   * @throws InvalidRangeException if the range is empty, reversed, or out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  auto range_apply(size_type left, size_type right, const Value& delta) -> void;

  ///@brief Replaces the sequence with @p size group-identity values in O(n) time.
  auto reset(size_type size) -> void;

  ///@brief Removes every element in O(n) time.
  auto clear() noexcept -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Returns the materialized value at @p index.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto point_query(size_type index) const -> Value;

  ///@brief Returns the number of stored elements.
  [[nodiscard]] auto size() const noexcept -> size_type;

  ///@brief Returns whether the tree contains no elements.
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  ///@brief Returns the stored commutative-group policy.
  [[nodiscard]] auto get_group() const noexcept -> const Group&;

private:
  //===----- TRANSACTION JOURNAL ----------------------------------------------===//

  ///@brief Previous value of one internal node modified by a transaction.
  struct JournalEntry {
    size_type index;    ///< Internal Fenwick index.
    Value     previous; ///< Value to restore during rollback.
  };

  using journal_type = DynamicArray<JournalEntry>;

  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Returns the least-significant set bit of @p index.
  static constexpr auto lsb(size_type index) noexcept -> size_type;

  ///@brief Builds a fresh difference tree, then commits it atomically.
  auto rebuild(DynamicArray<Value>&& values) -> void;

  ///@brief Applies @p delta to the Fenwick path while recording previous nodes.
  auto update_tree(size_type index, const Value& delta, journal_type& journal) -> void;

  ///@brief Restores all nodes recorded in @p journal.
  auto rollback(journal_type& journal) noexcept -> void;

  ///@brief Aggregates the difference prefix ending at @p index.
  [[nodiscard]] auto prefix_query_internal(size_type index) const -> Value;

  ///@brief Throws if @p index is outside the stored sequence.
  auto validate_index(size_type index) const -> void;

  ///@brief Throws if [@p left, @p right] is not a valid inclusive range.
  auto validate_range(size_type left, size_type right) const -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  [[no_unique_address]] Group group_;    ///< Stored commutative-group policy.
  DynamicArray<Value>         tree_{};   ///< One-based Fenwick tree over differences.
  size_type                   size_ = 0; ///< Number of logical elements.
};

} // namespace ads::range

#include "../../../src/ads/range/Fenwick_Tree_Range_Update.tpp"

#endif // FENWICK_TREE_RANGE_UPDATE_HPP

//===---------------------------------------------------------------------------===//
