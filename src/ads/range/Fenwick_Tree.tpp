//===---------------------------------------------------------------------------===//
/**
 * @file Fenwick_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the commutative-group Fenwick Tree.
 * @version 1.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/range/Fenwick_Tree.hpp"

#include <bit>

namespace ads::range {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTree<Value, Group>::FenwickTree(Group group) noexcept(std::is_nothrow_move_constructible_v<Group>) : group_(std::move(group)) {
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTree<Value, Group>::FenwickTree(size_type size, Group group) : group_(std::move(group)) {
  reset(size);
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTree<Value, Group>::FenwickTree(size_type size, const Value& value, Group group) : group_(std::move(group)) {
  rebuild(DynamicArray<Value>(size, value));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTree<Value, Group>::FenwickTree(const std::vector<Value>& values, Group group) : group_(std::move(group)) {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTree<Value, Group>::FenwickTree(std::vector<Value>&& values, Group group) : group_(std::move(group)) {
  rebuild(DynamicArray<Value>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
  values.clear();
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
template <std::input_iterator InputIt>
FenwickTree<Value, Group>::FenwickTree(InputIt first, InputIt last, Group group)
    requires std::constructible_from<Value, std::iter_reference_t<InputIt>>
    : group_(std::move(group)) {
  rebuild(DynamicArray<Value>(first, last));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTree<Value, Group>::FenwickTree(std::initializer_list<Value> values, Group group) : group_(std::move(group)) {
  rebuild(DynamicArray<Value>(values));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
FenwickTree<Value, Group>::FenwickTree(FenwickTree&& other) noexcept(std::is_nothrow_move_constructible_v<Group>) :
    group_(std::move(other.group_)),
    values_(std::move(other.values_)),
    tree_(std::move(other.tree_)),
    size_(other.size_) {
  other.size_ = 0;
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::operator=(FenwickTree&& other) noexcept(std::is_nothrow_move_assignable_v<Group>) -> FenwickTree& {
  if (this != &other) {
    group_      = std::move(other.group_);
    values_     = std::move(other.values_);
    tree_       = std::move(other.tree_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===----- MODIFICATION OPERATIONS ---------------------------------------------===//

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::build(const std::vector<Value>& values) -> void {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::build(std::vector<Value>&& values) -> void {
  rebuild(DynamicArray<Value>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
  values.clear();
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
template <std::input_iterator InputIt>
auto FenwickTree<Value, Group>::build(InputIt first, InputIt last) -> void
    requires std::constructible_from<Value, std::iter_reference_t<InputIt>>
{
  rebuild(DynamicArray<Value>(first, last));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::build(std::initializer_list<Value> values) -> void {
  rebuild(DynamicArray<Value>(values));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::apply(size_type index, const Value& delta) -> void {
  validate_index(index);
  update_transactional(index, group_.combine(values_[index], delta), delta);
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::set(size_type index, const Value& value) -> void {
  validate_index(index);
  const Value delta = group_.combine(group_.inverse(values_[index]), value);
  update_transactional(index, value, delta);
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::reset(size_type size) -> void {
  rebuild(DynamicArray<Value>(size, group_.identity()));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::clear() noexcept -> void {
  values_.clear();
  tree_.clear();
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::prefix_query(size_type index) const -> Value {
  validate_index(index);
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
auto FenwickTree<Value, Group>::range_query(size_type left, size_type right) const -> Value {
  validate_range(left, right);
  if (left == 0) {
    return prefix_query(right);
  }
  return group_.combine(group_.inverse(prefix_query(left - 1)), prefix_query(right));
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::total() const -> Value {
  return is_empty() ? group_.identity() : prefix_query(size_ - 1);
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::value_at(size_type index) const -> const_reference {
  validate_index(index);
  return values_[index];
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::size() const noexcept -> size_type {
  return size_;
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::get_group() const noexcept -> const Group& {
  return group_;
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
template <typename Compare>
requires std::predicate<Compare, const Value&, const Value&>
auto FenwickTree<Value, Group>::lower_bound(const Value& target, Compare compare) const -> size_type {
  Value     prefix   = group_.identity();
  size_type position = 0;

  for (size_type step = std::bit_floor(size_); step > 0; step >>= 1) {
    const size_type next = position + step;
    if (next <= size_) {
      Value candidate = group_.combine(prefix, tree_[next]);
      if (compare(candidate, target)) {
        prefix   = std::move(candidate);
        position = next;
      }
    }
  }
  return position;
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
constexpr auto FenwickTree<Value, Group>::lsb(size_type index) noexcept -> size_type {
  return index & (~index + 1);
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::rebuild(DynamicArray<Value>&& values) -> void {
  const size_type     new_size = values.size();
  DynamicArray<Value> new_tree(new_size + 1, group_.identity());

  for (size_type index = 0; index < new_size; ++index) {
    const size_type tree_index = index + 1;
    new_tree[tree_index]       = group_.combine(new_tree[tree_index], values[index]);
    const size_type parent     = tree_index + lsb(tree_index);
    if (parent <= new_size) {
      new_tree[parent] = group_.combine(new_tree[parent], new_tree[tree_index]);
    }
  }

  values_ = std::move(values);
  tree_   = std::move(new_tree);
  size_   = new_size;
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::update_tree(size_type index, const Value& delta, journal_type& journal) -> void {
  for (size_type cursor = index + 1; cursor <= size_; cursor += lsb(cursor)) {
    Value replacement = group_.combine(tree_[cursor], delta);
    journal.push_back(JournalEntry{cursor, tree_[cursor]});
    using std::swap;
    swap(tree_[cursor], replacement);
  }
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::rollback(journal_type& journal) noexcept -> void {
  using std::swap;
  for (size_type index = journal.size(); index > 0; --index) {
    swap(tree_[journal[index - 1].index], journal[index - 1].previous);
  }
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::update_transactional(size_type index, Value replacement, const Value& delta) -> void {
  journal_type journal;
  journal.reserve(std::bit_width(size_) + 1);
  try {
    update_tree(index, delta, journal);
  } catch (...) {
    rollback(journal);
    throw;
  }
  using std::swap;
  swap(values_[index], replacement);
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::validate_index(size_type index) const -> void {
  if (index >= size_) {
    throw RangeIndexException("FenwickTree index out of range");
  }
}

template <typename Value, typename Group>
requires FenwickPolicy<Value, Group>
auto FenwickTree<Value, Group>::validate_range(size_type left, size_type right) const -> void {
  if (size_ == 0 || left > right || right >= size_) {
    throw InvalidRangeException("FenwickTree invalid or out-of-bounds range");
  }
}

} // namespace ads::range

//===---------------------------------------------------------------------------===//
