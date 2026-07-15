//===---------------------------------------------------------------------------===//
/**
 * @file Fenwick_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of a commutative-group Fenwick Tree.
 * @version 1.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef FENWICK_TREE_HPP
#define FENWICK_TREE_HPP

#include "../algebra/Basic_Monoids.hpp"
#include "../arrays/Dynamic_Array.hpp"
#include "Range_Concepts.hpp"
#include "Range_Exception.hpp"

#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::range {

using ads::arrays::DynamicArray;

/**
 * @brief Binary Indexed Tree over a commutative group.
 *
 * @details The group supplies the identity, combination, and inverse operations.
 *          Commutativity is required because the Fenwick layout may reorder stored
 *          contributions. Point changes are journaled and committed with
 *          non-throwing swaps, providing the strong exception guarantee.
 *
 * @tparam Value Stored and aggregated value type.
 * @tparam Group Commutative-group policy whose @c value_type is @p Value.
 */
template <typename Value, typename Group = algebra::AddGroup<Value>>
requires FenwickPolicy<Value, Group>
class FenwickTree {
public:
  using value_type      = Value;
  using group_type      = Group;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;
  using const_reference = const Value&;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty tree with a default-constructed group.
   * @complexity Time O(1), Space O(1)
   */
  FenwickTree() noexcept(std::is_nothrow_default_constructible_v<Group>) requires std::default_initializable<Group>
  = default;

  /**
   * @brief Constructs an empty tree with an explicit group policy.
   * @param group Commutative-group policy to store.
   * @complexity Time O(1), Space O(1)
   */
  explicit FenwickTree(Group group) noexcept(std::is_nothrow_move_constructible_v<Group>);

  /**
   * @brief Constructs @p size elements initialized to the group identity.
   * @param size Number of elements.
   * @param group Commutative-group policy to store.
   * @complexity Time O(n), Space O(n)
   */
  explicit FenwickTree(size_type size, Group group = {});

  /**
   * @brief Constructs @p size copies of @p value.
   * @param size Number of elements.
   * @param value Initial value of every element.
   * @param group Commutative-group policy to store.
   * @complexity Time O(n), Space O(n)
   */
  FenwickTree(size_type size, const Value& value, Group group = {});

  /**
   * @brief Constructs a tree by copying a vector.
   * @param values Initial sequence.
   * @param group Commutative-group policy to store.
   * @complexity Time O(n), Space O(n)
   */
  explicit FenwickTree(const std::vector<Value>& values, Group group = {});

  /**
   * @brief Constructs a tree by moving a vector.
   * @param values Initial sequence; consumed by the constructor.
   * @param group Commutative-group policy to store.
   * @complexity Time O(n), Space O(n)
   */
  explicit FenwickTree(std::vector<Value>&& values, Group group = {});

  /**
   * @brief Constructs a tree from an input range.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the final element.
   * @param group Commutative-group policy to store.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  FenwickTree(InputIt first, InputIt last, Group group = {}) requires std::constructible_from<Value, std::iter_reference_t<InputIt>>;

  /**
   * @brief Constructs a tree from an initializer list.
   * @param values Initial sequence.
   * @param group Commutative-group policy to store.
   * @complexity Time O(n), Space O(n)
   */
  FenwickTree(std::initializer_list<Value> values, Group group = {});

  /**
   * @brief Move constructor; leaves @p other empty.
   * @param other Tree to move from.
   * @complexity Time O(1), Space O(1)
   */
  FenwickTree(FenwickTree&& other) noexcept(std::is_nothrow_move_constructible_v<Group>);

  /**
   * @brief Move assignment; leaves @p other empty.
   * @param other Tree to move from.
   * @return Reference to this tree.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(FenwickTree&& other) noexcept(std::is_nothrow_move_assignable_v<Group>) -> FenwickTree&;

  ///@brief Destroys the tree and its contiguous storage.
  ~FenwickTree() = default;

  FenwickTree(const FenwickTree&)                    = delete;
  auto operator=(const FenwickTree&) -> FenwickTree& = delete;

  //===----- MODIFICATION OPERATIONS -------------------------------------------===//

  ///@brief Rebuilds the tree from a copied vector in O(n) time.
  auto build(const std::vector<Value>& values) -> void;

  ///@brief Rebuilds the tree from a moved vector in O(n) time.
  auto build(std::vector<Value>&& values) -> void;

  ///@brief Rebuilds the tree from the input range [@p first, @p last) in O(n) time.
  template <std::input_iterator InputIt>
  auto build(InputIt first, InputIt last) -> void requires std::constructible_from<Value, std::iter_reference_t<InputIt>>;

  ///@brief Rebuilds the tree from @p values in O(n) time.
  auto build(std::initializer_list<Value> values) -> void;

  /**
   * @brief Combines @p delta with the value at @p index.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  auto apply(size_type index, const Value& delta) -> void;

  /**
   * @brief Replaces the value at @p index.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  auto set(size_type index, const Value& value) -> void;

  ///@brief Replaces the sequence with @p size group-identity values in O(n) time.
  auto reset(size_type size) -> void;

  ///@brief Removes every element in O(n) time.
  auto clear() noexcept -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Returns the aggregate over the inclusive prefix [0, @p index].
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto prefix_query(size_type index) const -> Value;

  /**
   * @brief Returns the aggregate over the inclusive range [@p left, @p right].
   * @throws InvalidRangeException if the range is empty, reversed, or out of bounds.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto range_query(size_type left, size_type right) const -> Value;

  ///@brief Returns the whole-sequence aggregate, or the group identity when empty.
  [[nodiscard]] auto total() const -> Value;

  ///@brief Returns a const reference to the value at @p index.
  [[nodiscard]] auto value_at(size_type index) const -> const_reference;

  ///@brief Returns the number of stored elements.
  [[nodiscard]] auto size() const noexcept -> size_type;

  ///@brief Returns whether the tree contains no elements.
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  ///@brief Returns the stored commutative-group policy.
  [[nodiscard]] auto get_group() const noexcept -> const Group&;

  /**
   * @brief Finds the first prefix for which @c compare(prefix,target) is false.
   * @return Its zero-based index, or @c size() when no prefix qualifies.
   * @note The sequence of prefix aggregates must be monotone under @p compare.
   * @complexity O(log n)
   */
  template <typename Compare = std::less<>>
  requires std::predicate<Compare, const Value&, const Value&>
  [[nodiscard]] auto lower_bound(const Value& target, Compare compare = {}) const -> size_type;

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

  ///@brief Builds fresh value and Fenwick arrays, then commits them atomically.
  auto rebuild(DynamicArray<Value>&& values) -> void;

  ///@brief Applies @p delta to the Fenwick path while recording previous nodes.
  auto update_tree(size_type index, const Value& delta, journal_type& journal) -> void;

  ///@brief Restores all nodes recorded in @p journal.
  auto rollback(journal_type& journal) noexcept -> void;

  ///@brief Commits a point replacement and its corresponding group delta.
  auto update_transactional(size_type index, Value replacement, const Value& delta) -> void;

  ///@brief Throws if @p index is outside the stored sequence.
  auto validate_index(size_type index) const -> void;

  ///@brief Throws if [@p left, @p right] is not a valid inclusive range.
  auto validate_range(size_type left, size_type right) const -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  [[no_unique_address]] Group group_;    ///< Stored commutative-group policy.
  DynamicArray<Value>         values_{}; ///< Materialized element values.
  DynamicArray<Value>         tree_{};   ///< One-based Fenwick storage.
  size_type                   size_ = 0; ///< Number of logical elements.
};

} // namespace ads::range

#include "../../../src/ads/range/Fenwick_Tree.tpp"

#endif // FENWICK_TREE_HPP

//===---------------------------------------------------------------------------===//
