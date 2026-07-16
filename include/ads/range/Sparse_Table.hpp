//===---------------------------------------------------------------------------===//
/**
 * @file Sparse_Table.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the idempotent-semigroup-driven SparseTable class.
 * @version 1.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SPARSE_TABLE_HPP
#define SPARSE_TABLE_HPP

#include "../../support/Container_Facade.hpp"
#include "../algebra/Algebra_Concepts.hpp"
#include "../algebra/Basic_Monoids.hpp"
#include "../arrays/Dynamic_Array.hpp"
#include "Range_Exception.hpp"

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::range {

using ads::arrays::DynamicArray;
using ads::support::ContainerFacade;

/**
 * @brief Static Sparse Table driven by an idempotent semigroup policy.
 *
 * @details Queries combine two overlapping power-of-two windows, which requires
 *          idempotence in addition to associativity. No identity is required because
 *          the public API accepts only non-empty inclusive ranges. Rebuilds are
 *          transactional and provide the strong exception guarantee.
 *
 *          Complexity:
 *            - Build: O(n log n) time and space.
 *            - Query: O(1) time and O(1) auxiliary space.
 *
 * @tparam T Stored value type.
 * @tparam Policy Idempotent semigroup policy (default: minimum).
 */
template <typename T, typename Policy = algebra::MinSemigroup<T>>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
class SparseTable : public ContainerFacade<SparseTable<T, Policy>> {
public:
  using value_type      = T;
  using policy_type     = Policy;
  using size_type       = std::size_t;
  using reference       = T&;
  using const_reference = const T&;
  using pointer         = T*;
  using const_pointer   = const T*;

  using const_iterator         = typename DynamicArray<T>::const_iterator;
  using const_reverse_iterator = typename DynamicArray<T>::const_reverse_iterator;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  ///@brief Constructs an empty table with a default-constructed policy.
  SparseTable() requires std::default_initializable<Policy>
  = default;

  ///@brief Constructs an empty table with explicit @p policy.
  explicit SparseTable(Policy policy);

  ///@brief Constructs from a copied vector in O(n log n) time and space.
  explicit SparseTable(const std::vector<T>& values, Policy policy = {});

  ///@brief Constructs from a moved vector in O(n log n) time and space.
  explicit SparseTable(std::vector<T>&& values, Policy policy = {});

  ///@brief Constructs from an initializer list in O(n log n) time and space.
  SparseTable(std::initializer_list<T> values, Policy policy = {});

  ///@brief Constructs from the input range [@p first, @p last).
  template <std::input_iterator InputIt>
  SparseTable(InputIt first, InputIt last, Policy policy = {});

  ///@brief Move constructor; leaves @p other empty.
  SparseTable(SparseTable&& other) noexcept(std::is_nothrow_move_constructible_v<Policy>);

  ///@brief Move assignment; leaves @p other empty.
  auto operator=(SparseTable&& other) noexcept(std::is_nothrow_move_assignable_v<Policy>) -> SparseTable&;

  ///@brief Destroys all precomputed levels.
  ~SparseTable() = default;

  SparseTable(const SparseTable&)                    = delete;
  auto operator=(const SparseTable&) -> SparseTable& = delete;

  //===----- MODIFICATION OPERATIONS -------------------------------------------===//

  ///@brief Rebuilds from a copied vector in O(n log n) time and space.
  auto build(const std::vector<T>& values) -> void;

  ///@brief Rebuilds from a moved vector in O(n log n) time and space.
  auto build(std::vector<T>&& values) -> void;

  ///@brief Rebuilds from an initializer list in O(n log n) time and space.
  auto build(std::initializer_list<T> values) -> void;

  ///@brief Rebuilds from the input range [@p first, @p last).
  template <std::input_iterator InputIt>
  auto build(InputIt first, InputIt last) -> void;

  ///@brief Removes every value and precomputed level in O(n log n) time.
  auto clear() noexcept -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Returns the aggregate over inclusive range [@p left, @p right].
   * @throws InvalidRangeException if the range is empty, reversed, or out of bounds.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto range_query(size_type left, size_type right) const -> T;

  ///@brief Returns a const reference to the original value at @p index.
  [[nodiscard]] auto value_at(size_type index) const -> const_reference;

  ///@brief Returns the number of original values.
  [[nodiscard]] auto size() const noexcept -> size_type;

  ///@brief Returns whether the table contains no values.
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  ///@brief Returns the stored idempotent-semigroup policy.
  [[nodiscard]] auto get_policy() const noexcept -> const Policy&;

  //===----- ITERATOR ACCESS ---------------------------------------------------===//

  ///@brief Returns a const iterator to the first original value.
  [[nodiscard]] auto begin() const noexcept -> const_iterator;

  ///@brief Returns a const iterator past the final original value.
  [[nodiscard]] auto end() const noexcept -> const_iterator;

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Builds all levels off-structure, then commits them atomically.
  auto rebuild(DynamicArray<T>&& level0) -> void;

  ///@brief Throws if @p index is outside the original sequence.
  auto validate_index(size_type index) const -> void;

  ///@brief Throws if [@p left, @p right] is not a valid inclusive range.
  auto validate_range(size_type left, size_type right) const -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  [[no_unique_address]] Policy  policy_;   ///< Stored idempotent-semigroup policy.
  DynamicArray<DynamicArray<T>> table_{};  ///< Level k stores aggregates of length 2^k.
  size_type                     size_ = 0; ///< Number of original values.
};

} // namespace ads::range

#include "../../../src/ads/range/Sparse_Table.tpp"

#endif // SPARSE_TABLE_HPP

//===---------------------------------------------------------------------------===//
