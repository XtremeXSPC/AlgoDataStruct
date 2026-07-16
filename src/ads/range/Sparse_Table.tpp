//===---------------------------------------------------------------------------===//
/**
 * @file Sparse_Table.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the idempotent-semigroup-driven SparseTable class.
 * @version 1.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/range/Sparse_Table.hpp"

#include <bit>
#include <iterator>

namespace ads::range {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
SparseTable<T, Policy>::SparseTable(Policy policy) : policy_(std::move(policy)) {
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
SparseTable<T, Policy>::SparseTable(const std::vector<T>& values, Policy policy) : policy_(std::move(policy)) {
  rebuild(DynamicArray<T>(values.begin(), values.end()));
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
SparseTable<T, Policy>::SparseTable(std::vector<T>&& values, Policy policy) : policy_(std::move(policy)) {
  rebuild(DynamicArray<T>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
SparseTable<T, Policy>::SparseTable(std::initializer_list<T> values, Policy policy) : policy_(std::move(policy)) {
  rebuild(DynamicArray<T>(values.begin(), values.end()));
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
template <std::input_iterator InputIt>
SparseTable<T, Policy>::SparseTable(InputIt first, InputIt last, Policy policy) : policy_(std::move(policy)) {
  rebuild(DynamicArray<T>(first, last));
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
SparseTable<T, Policy>::SparseTable(SparseTable&& other) noexcept(std::is_nothrow_move_constructible_v<Policy>) :
    policy_(std::move(other.policy_)),
    table_(std::move(other.table_)),
    size_(other.size_) {
  other.size_ = 0;
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::operator=(SparseTable&& other) noexcept(std::is_nothrow_move_assignable_v<Policy>) -> SparseTable& {
  if (this != &other) {
    policy_     = std::move(other.policy_);
    table_      = std::move(other.table_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===----- MODIFICATION OPERATIONS ---------------------------------------------===//

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::build(const std::vector<T>& values) -> void {
  rebuild(DynamicArray<T>(values.begin(), values.end()));
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::build(std::vector<T>&& values) -> void {
  rebuild(DynamicArray<T>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::build(std::initializer_list<T> values) -> void {
  rebuild(DynamicArray<T>(values.begin(), values.end()));
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
template <std::input_iterator InputIt>
auto SparseTable<T, Policy>::build(InputIt first, InputIt last) -> void {
  rebuild(DynamicArray<T>(first, last));
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::clear() noexcept -> void {
  table_.clear();
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::range_query(size_type left, size_type right) const -> T {
  validate_range(left, right);
  const size_type length = right - left + 1;
  const size_type level  = static_cast<size_type>(std::bit_width(length)) - 1;
  const size_type block  = size_type{1} << level;
  return policy_.combine(table_[level][left], table_[level][right - block + 1]);
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::value_at(size_type index) const -> const_reference {
  validate_index(index);
  return table_[0][index];
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::size() const noexcept -> size_type {
  return size_;
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::get_policy() const noexcept -> const Policy& {
  return policy_;
}

//===----- ITERATOR OPERATIONS -------------------------------------------------===//

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::begin() const noexcept -> const_iterator {
  return is_empty() ? const_iterator{} : table_[0].begin();
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::end() const noexcept -> const_iterator {
  return is_empty() ? const_iterator{} : table_[0].end();
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::rebuild(DynamicArray<T>&& level0) -> void {
  const size_type n = level0.size();

  DynamicArray<DynamicArray<T>> new_table;
  if (n > 0) {
    const size_type total_levels = static_cast<size_type>(std::bit_width(n));
    new_table.reserve(total_levels);
    new_table.push_back(std::move(level0));

    for (size_type k = 1; k < total_levels; ++k) {
      const size_type half  = size_type{1} << (k - 1);
      const size_type span  = size_type{1} << k;
      const size_type count = n - span + 1;

      DynamicArray<T> row;
      row.reserve(count);
      const DynamicArray<T>& previous = new_table[k - 1];
      for (size_type i = 0; i < count; ++i) {
        row.push_back(policy_.combine(previous[i], previous[i + half]));
      }
      new_table.push_back(std::move(row));
    }
  }

  table_ = std::move(new_table);
  size_  = n;
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::validate_index(size_type index) const -> void {
  if (index >= size_) {
    throw RangeIndexException();
  }
}

template <typename T, typename Policy>
requires std::copyable<T> && algebra::IdempotentSemigroupPolicy<Policy> && std::same_as<typename Policy::value_type, T>
auto SparseTable<T, Policy>::validate_range(size_type left, size_type right) const -> void {
  if (size_ == 0 || left > right || right >= size_) {
    throw InvalidRangeException();
  }
}

} // namespace ads::range

//===---------------------------------------------------------------------------===//
