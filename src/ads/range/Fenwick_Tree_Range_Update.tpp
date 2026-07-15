//===---------------------------------------------------------------------------===//
/**
 * @file Fenwick_Tree_Range_Update.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the range-action Fenwick Tree.
 * @version 1.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/range/Fenwick_Tree_Range_Update.hpp"

#include <bit>
#include <optional>

namespace ads::range {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTreeRangeUpdate<Value, Group>::FenwickTreeRangeUpdate(Group group) noexcept(std::is_nothrow_move_constructible_v<Group>) :
    group_(std::move(group)) {
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTreeRangeUpdate<Value, Group>::FenwickTreeRangeUpdate(size_type size, Group group) : group_(std::move(group)) {
  reset(size);
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTreeRangeUpdate<Value, Group>::FenwickTreeRangeUpdate(size_type size, const Value& value, Group group) : group_(std::move(group)) {
  rebuild(DynamicArray<Value>(size, value));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTreeRangeUpdate<Value, Group>::FenwickTreeRangeUpdate(const std::vector<Value>& values, Group group) : group_(std::move(group)) {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTreeRangeUpdate<Value, Group>::FenwickTreeRangeUpdate(std::vector<Value>&& values, Group group) : group_(std::move(group)) {
  rebuild(DynamicArray<Value>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
  values.clear();
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
template <std::input_iterator InputIt>
FenwickTreeRangeUpdate<Value, Group>::FenwickTreeRangeUpdate(InputIt first, InputIt last, Group group)
    requires std::constructible_from<Value, std::iter_reference_t<InputIt>>
    : group_(std::move(group)) {
  rebuild(DynamicArray<Value>(first, last));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTreeRangeUpdate<Value, Group>::FenwickTreeRangeUpdate(std::initializer_list<Value> values, Group group) : group_(std::move(group)) {
  rebuild(DynamicArray<Value>(values));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTreeRangeUpdate<Value, Group>::FenwickTreeRangeUpdate(FenwickTreeRangeUpdate&& other) noexcept(
    std::is_nothrow_move_constructible_v<Group>) :
    group_(std::move(other.group_)),
    tree_(std::move(other.tree_)),
    size_(other.size_) {
  other.size_ = 0;
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::operator=(FenwickTreeRangeUpdate&& other) noexcept(std::is_nothrow_move_assignable_v<Group>)
    -> FenwickTreeRangeUpdate& {
  if (this != &other) {
    group_      = std::move(other.group_);
    tree_       = std::move(other.tree_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===----- MODIFICATION OPERATIONS ---------------------------------------------===//

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::build(const std::vector<Value>& values) -> void {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::build(std::vector<Value>&& values) -> void {
  rebuild(DynamicArray<Value>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
  values.clear();
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
template <std::input_iterator InputIt>
auto FenwickTreeRangeUpdate<Value, Group>::build(InputIt first, InputIt last) -> void
    requires std::constructible_from<Value, std::iter_reference_t<InputIt>>
{
  rebuild(DynamicArray<Value>(first, last));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::build(std::initializer_list<Value> values) -> void {
  rebuild(DynamicArray<Value>(values));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::range_apply(size_type left, size_type right, const Value& delta) -> void {
  validate_range(left, right);
  std::optional<Value> inverse;
  if (right + 1 < size_) {
    inverse = group_.inverse(delta);
  }

  journal_type journal;
  journal.reserve(2 * std::bit_width(size_) + 2);
  try {
    update_tree(left, delta, journal);
    if (inverse.has_value()) {
      update_tree(right + 1, *inverse, journal);
    }
  } catch (...) {
    rollback(journal);
    throw;
  }
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::reset(size_type size) -> void {
  rebuild(DynamicArray<Value>(size, group_.identity()));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::clear() noexcept -> void {
  tree_.clear();
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::point_query(size_type index) const -> Value {
  validate_index(index);
  return prefix_query_internal(index);
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::size() const noexcept -> size_type {
  return size_;
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::get_group() const noexcept -> const Group& {
  return group_;
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
constexpr auto FenwickTreeRangeUpdate<Value, Group>::lsb(size_type index) noexcept -> size_type {
  return index & (~index + 1);
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::rebuild(DynamicArray<Value>&& values) -> void {
  const size_type     new_size = values.size();
  DynamicArray<Value> new_tree(new_size + 1, group_.identity());

  for (size_type index = 0; index < new_size; ++index) {
    const Value difference =
        (index == 0) ? values[index] : group_.combine(group_.inverse(values[index - 1]), values[index]);
    const size_type tree_index = index + 1;
    new_tree[tree_index]       = group_.combine(new_tree[tree_index], difference);
    const size_type parent     = tree_index + lsb(tree_index);
    if (parent <= new_size) {
      new_tree[parent] = group_.combine(new_tree[parent], new_tree[tree_index]);
    }
  }

  tree_ = std::move(new_tree);
  size_ = new_size;
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::update_tree(size_type index, const Value& delta, journal_type& journal) -> void {
  for (size_type cursor = index + 1; cursor <= size_; cursor += lsb(cursor)) {
    Value replacement = group_.combine(tree_[cursor], delta);
    journal.push_back(JournalEntry{cursor, tree_[cursor]});
    using std::swap;
    swap(tree_[cursor], replacement);
  }
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::rollback(journal_type& journal) noexcept -> void {
  using std::swap;
  for (size_type index = journal.size(); index > 0; --index) {
    swap(tree_[journal[index - 1].index], journal[index - 1].previous);
  }
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::prefix_query_internal(size_type index) const -> Value {
  Value     result = group_.identity();
  size_type cursor = index + 1;
  while (cursor > 0) {
    result = group_.combine(result, tree_[cursor]);
    cursor -= lsb(cursor);
  }
  return result;
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::validate_index(size_type index) const -> void {
  if (index >= size_) {
    throw RangeIndexException("FenwickTreeRangeUpdate index out of range");
  }
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTreeRangeUpdate<Value, Group>::validate_range(size_type left, size_type right) const -> void {
  if (size_ == 0 || left > right || right >= size_) {
    throw InvalidRangeException("FenwickTreeRangeUpdate invalid or out-of-bounds range");
  }
}

} // namespace ads::range

//===---------------------------------------------------------------------------===//
