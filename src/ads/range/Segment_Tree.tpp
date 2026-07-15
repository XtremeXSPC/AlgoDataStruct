//===---------------------------------------------------------------------------===//
/**
 * @file Segment_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the monoid-driven SegmentTree class.
 * @version 2.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/range/Segment_Tree.hpp"

#include <bit>
#include <iterator>
#include <limits>
#include <stdexcept>

namespace ads::range {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
             && std::is_nothrow_swappable_v<typename Monoid::value_type>
SegmentTree<Value, Monoid, LeafBuilder>::SegmentTree(Monoid monoid, LeafBuilder leaf_builder) :
    monoid_(std::move(monoid)),
    leaf_builder_(std::move(leaf_builder)) {
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
         SegmentTree<Value, Monoid, LeafBuilder>::SegmentTree(size_type size)
             requires std::default_initializable<Value> && std::default_initializable<Monoid> && std::default_initializable<LeafBuilder>
{
  rebuild(DynamicArray<Value>(size, Value{}));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
         SegmentTree<Value, Monoid, LeafBuilder>::SegmentTree(size_type size, Monoid monoid, LeafBuilder leaf_builder)
             requires std::default_initializable<Value>
    : monoid_(std::move(monoid)), leaf_builder_(std::move(leaf_builder)) {
  rebuild(DynamicArray<Value>(size, Value{}));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
             && std::is_nothrow_swappable_v<typename Monoid::value_type>
SegmentTree<Value, Monoid, LeafBuilder>::SegmentTree(size_type size, const Value& value, Monoid monoid, LeafBuilder leaf_builder) :
    monoid_(std::move(monoid)),
    leaf_builder_(std::move(leaf_builder)) {
  rebuild(DynamicArray<Value>(size, value));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
             && std::is_nothrow_swappable_v<typename Monoid::value_type>
SegmentTree<Value, Monoid, LeafBuilder>::SegmentTree(const std::vector<Value>& values, Monoid monoid, LeafBuilder leaf_builder) :
    monoid_(std::move(monoid)),
    leaf_builder_(std::move(leaf_builder)) {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
             && std::is_nothrow_swappable_v<typename Monoid::value_type>
SegmentTree<Value, Monoid, LeafBuilder>::SegmentTree(std::vector<Value>&& values, Monoid monoid, LeafBuilder leaf_builder) :
    monoid_(std::move(monoid)),
    leaf_builder_(std::move(leaf_builder)) {
  rebuild(DynamicArray<Value>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
  values.clear();
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
             && std::is_nothrow_swappable_v<typename Monoid::value_type>
SegmentTree<Value, Monoid, LeafBuilder>::SegmentTree(std::initializer_list<Value> values, Monoid monoid, LeafBuilder leaf_builder) :
    monoid_(std::move(monoid)),
    leaf_builder_(std::move(leaf_builder)) {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
             && std::is_nothrow_swappable_v<typename Monoid::value_type>
template <std::input_iterator InputIt>
SegmentTree<Value, Monoid, LeafBuilder>::SegmentTree(InputIt first, InputIt last, Monoid monoid, LeafBuilder leaf_builder) :
    monoid_(std::move(monoid)),
    leaf_builder_(std::move(leaf_builder)) {
  rebuild(DynamicArray<Value>(first, last));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
             && std::is_nothrow_swappable_v<typename Monoid::value_type>
SegmentTree<Value, Monoid, LeafBuilder>::SegmentTree(SegmentTree&& other) noexcept(
    std::is_nothrow_move_constructible_v<Monoid> && std::is_nothrow_move_constructible_v<LeafBuilder>) :
    monoid_(std::move(other.monoid_)),
    leaf_builder_(std::move(other.leaf_builder_)),
    values_(std::move(other.values_)),
    tree_(std::move(other.tree_)),
    leaf_count_(other.leaf_count_),
    size_(other.size_) {
  other.leaf_count_ = 0;
  other.size_       = 0;
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::operator=(SegmentTree&& other) noexcept(
    std::is_nothrow_move_assignable_v<Monoid> && std::is_nothrow_move_assignable_v<LeafBuilder>) -> SegmentTree& {
  if (this != &other) {
    monoid_           = std::move(other.monoid_);
    leaf_builder_     = std::move(other.leaf_builder_);
    values_           = std::move(other.values_);
    tree_             = std::move(other.tree_);
    leaf_count_       = other.leaf_count_;
    size_             = other.size_;
    other.leaf_count_ = 0;
    other.size_       = 0;
  }
  return *this;
}

//===----- MODIFICATION OPERATIONS ---------------------------------------------===//

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::build(const std::vector<Value>& values) -> void {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::build(std::vector<Value>&& values) -> void {
  rebuild(DynamicArray<Value>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
  values.clear();
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::build(std::initializer_list<Value> values) -> void {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
template <std::input_iterator InputIt>
auto SegmentTree<Value, Monoid, LeafBuilder>::build(InputIt first, InputIt last) -> void {
  rebuild(DynamicArray<Value>(first, last));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::set(size_type index, const Value& value) -> void {
  validate_index(index);
  Value replacement = value;
  set_transactional(index, std::move(replacement));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::set(size_type index, Value&& value) -> void {
  validate_index(index);
  Value replacement = std::move(value);
  set_transactional(index, std::move(replacement));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
         auto SegmentTree<Value, Monoid, LeafBuilder>::add(size_type index, const Value& delta) -> void requires Addable<Value>
{
  validate_index(index);
  Value replacement = values_[index] + delta;
  set_transactional(index, std::move(replacement));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
         auto SegmentTree<Value, Monoid, LeafBuilder>::reset(size_type size) -> void requires std::default_initializable<Value>
{
  rebuild(DynamicArray<Value>(size, Value{}));
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::clear() noexcept -> void {
  values_.clear();
  tree_.clear();
  leaf_count_ = 0;
  size_       = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::range_query(size_type left, size_type right) const -> node_type {
  validate_range(left, right);
  node_type left_result  = monoid_.identity();
  node_type right_result = monoid_.identity();
  left += leaf_count_;
  right += leaf_count_ + 1;

  while (left < right) {
    if ((left & 1U) != 0U) {
      left_result = monoid_.combine(left_result, tree_[left++]);
    }
    if ((right & 1U) != 0U) {
      right_result = monoid_.combine(tree_[--right], right_result);
    }
    left >>= 1;
    right >>= 1;
  }
  return monoid_.combine(left_result, right_result);
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::total() const -> node_type {
  return is_empty() ? monoid_.identity() : tree_[1];
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::value_at(size_type index) const -> const_reference {
  validate_index(index);
  return values_[index];
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::node_at(size_type index) const -> node_type {
  validate_index(index);
  return tree_[leaf_count_ + index];
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::size() const noexcept -> size_type {
  return size_;
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

//===----- ITERATOR OPERATIONS -------------------------------------------------===//

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::begin() const noexcept -> const_iterator {
  return values_.cbegin();
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::end() const noexcept -> const_iterator {
  return values_.cend();
}

//===----- POLICY ACCESSORS ----------------------------------------------------===//

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::get_monoid() const noexcept -> const Monoid& {
  return monoid_;
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::get_leaf_builder() const noexcept -> const LeafBuilder& {
  return leaf_builder_;
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::rebuild(DynamicArray<Value>&& values) -> void {
  const size_type         new_size       = values.size();
  size_type               new_leaf_count = 0;
  DynamicArray<node_type> new_tree;

  if (new_size > 0) {
    const size_type maximum_leaf_count = std::bit_floor(std::numeric_limits<size_type>::max() / 2);
    if (new_size > maximum_leaf_count) {
      throw std::length_error("SegmentTree size exceeds representable storage");
    }
    new_leaf_count = std::bit_ceil(new_size);
    new_tree.assign(2 * new_leaf_count, monoid_.identity());
    for (size_type i = 0; i < new_size; ++i) {
      new_tree[new_leaf_count + i] = leaf_builder_(values[i]);
    }
    for (size_type i = new_leaf_count - 1; i > 0; --i) {
      new_tree[i] = monoid_.combine(new_tree[2 * i], new_tree[2 * i + 1]);
    }
  }

  values_     = std::move(values);
  tree_       = std::move(new_tree);
  leaf_count_ = new_leaf_count;
  size_       = new_size;
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::set_transactional(size_type index, Value&& value) -> void {
  DynamicArray<size_type> indexes;
  DynamicArray<node_type> replacements;
  const size_type         path_length = static_cast<size_type>(std::bit_width(leaf_count_)) + 1;
  indexes.reserve(path_length);
  replacements.reserve(path_length);

  size_type internal_index = leaf_count_ + index;
  node_type replacement    = leaf_builder_(value);
  indexes.push_back(internal_index);
  replacements.push_back(replacement);

  while (internal_index > 1) {
    const size_type parent = internal_index >> 1;
    if ((internal_index & 1U) == 0U) {
      replacement = monoid_.combine(replacement, tree_[internal_index + 1]);
    } else {
      replacement = monoid_.combine(tree_[internal_index - 1], replacement);
    }
    indexes.push_back(parent);
    replacements.push_back(replacement);
    internal_index = parent;
  }

  using std::swap;
  swap(values_[index], value);
  for (size_type i = 0; i < indexes.size(); ++i) {
    swap(tree_[indexes[i]], replacements[i]);
  }
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::validate_index(size_type index) const -> void {
  if (index >= size_) {
    throw RangeIndexException("SegmentTree index out of range");
  }
}

template <typename Value, typename Monoid, typename LeafBuilder>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
auto SegmentTree<Value, Monoid, LeafBuilder>::validate_range(size_type left, size_type right) const -> void {
  if (size_ == 0 || left > right || right >= size_) {
    throw InvalidRangeException("SegmentTree range is invalid or out of bounds");
  }
}

} // namespace ads::range

//===---------------------------------------------------------------------------===//
