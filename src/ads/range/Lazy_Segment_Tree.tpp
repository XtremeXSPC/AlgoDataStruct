//===---------------------------------------------------------------------------===//
/**
 * @file Lazy_Segment_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the acted-monoid Lazy Segment Tree.
 * @version 2.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/range/Lazy_Segment_Tree.hpp"

#include <bit>
#include <limits>
#include <numeric>

namespace ads::range {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
LazySegmentTree<Value, Acted>::LazySegmentTree(Acted acted) noexcept(std::is_nothrow_move_constructible_v<Acted>) :
    acted_(std::move(acted)) {
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
LazySegmentTree<Value, Acted>::LazySegmentTree(size_type size, Acted acted) requires std::default_initializable<Value>
    : acted_(std::move(acted)) {
  reset(size);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
LazySegmentTree<Value, Acted>::LazySegmentTree(size_type size, const Value& value, Acted acted) : acted_(std::move(acted)) {
  rebuild(DynamicArray<Value>(size, value));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
LazySegmentTree<Value, Acted>::LazySegmentTree(const std::vector<Value>& values, Acted acted) : acted_(std::move(acted)) {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
LazySegmentTree<Value, Acted>::LazySegmentTree(std::vector<Value>&& values, Acted acted) : acted_(std::move(acted)) {
  rebuild(DynamicArray<Value>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
  values.clear();
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
LazySegmentTree<Value, Acted>::LazySegmentTree(std::initializer_list<Value> values, Acted acted) : acted_(std::move(acted)) {
  rebuild(DynamicArray<Value>(values));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
template <std::input_iterator InputIt>
LazySegmentTree<Value, Acted>::LazySegmentTree(InputIt first, InputIt last, Acted acted) : acted_(std::move(acted)) {
  rebuild(DynamicArray<Value>(first, last));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
LazySegmentTree<Value, Acted>::LazySegmentTree(LazySegmentTree&& other) noexcept(std::is_nothrow_move_constructible_v<Acted>) :
    acted_(std::move(other.acted_)),
    tree_(std::move(other.tree_)),
    size_(other.size_) {
  other.size_ = 0;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::operator=(LazySegmentTree&& other) noexcept(std::is_nothrow_move_assignable_v<Acted>)
    -> LazySegmentTree& {
  if (this != &other) {
    acted_      = std::move(other.acted_);
    tree_       = std::move(other.tree_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===----- MODIFICATION OPERATIONS ---------------------------------------------===//

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::build(const std::vector<Value>& values) -> void {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::build(std::vector<Value>&& values) -> void {
  rebuild(DynamicArray<Value>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
  values.clear();
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::build(std::initializer_list<Value> values) -> void {
  rebuild(DynamicArray<Value>(values));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
template <std::input_iterator InputIt>
auto LazySegmentTree<Value, Acted>::build(InputIt first, InputIt last) -> void
    requires std::constructible_from<Value, std::iter_reference_t<InputIt>>
{
  rebuild(DynamicArray<Value>(first, last));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::set(size_type index, const Value& value) -> void {
  validate_index(index);
  journal_type journal;
  journal.reserve(8 * std::bit_width(size_) + 8);
  try {
    set_node(1, 0, size_ - 1, index, value, journal);
  } catch (...) {
    rollback(journal);
    throw;
  }
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::apply(size_type index, const tag_type& action) -> void {
  validate_index(index);
  range_apply(index, index, action);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::range_apply(size_type left, size_type right, const tag_type& action) -> void {
  validate_range(left, right);
  journal_type journal;
  journal.reserve(12 * std::bit_width(size_) + 8);
  try {
    update_range(1, 0, size_ - 1, left, right, action, journal);
  } catch (...) {
    rollback(journal);
    throw;
  }
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::reset(size_type size) -> void requires std::default_initializable<Value>
{
  rebuild(DynamicArray<Value>(size, Value{}));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::clear() noexcept -> void {
  tree_.clear();
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::range_query(size_type left, size_type right) const -> value_type {
  validate_range(left, right);
  return query_range(1, 0, size_ - 1, left, right, std::nullopt);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::value_at(size_type index) const -> value_type {
  validate_index(index);
  return query_range(1, 0, size_ - 1, index, index, std::nullopt);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::total() const -> value_type {
  return is_empty() ? acted_.value_identity() : tree_[1].value;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::size() const noexcept -> size_type {
  return size_;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::get_acted_monoid() const noexcept -> const Acted& {
  return acted_;
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::rebuild(DynamicArray<Value>&& values) -> void {
  const size_type new_size = values.size();
  if (new_size > std::numeric_limits<size_type>::max() / 4) {
    throw std::length_error("LazySegmentTree capacity overflow");
  }

  DynamicArray<TreeNode> new_tree;
  if (new_size != 0) {
    new_tree.assign(4 * new_size, TreeNode(acted_.value_identity()));
    build_node(new_tree, 1, 0, new_size - 1, values);
  }

  tree_ = std::move(new_tree);
  size_ = new_size;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::build_node(
    DynamicArray<TreeNode>& tree, size_type node, size_type left, size_type right, const DynamicArray<Value>& values) const -> void {
  if (left == right) {
    tree[node] = TreeNode(values[left]);
    return;
  }

  const size_type middle = std::midpoint(left, right);
  build_node(tree, 2 * node, left, middle, values);
  build_node(tree, 2 * node + 1, middle + 1, right, values);
  tree[node] = TreeNode(acted_.combine(tree[2 * node].value, tree[2 * node + 1].value));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::replace_node(size_type node, TreeNode&& replacement, journal_type& journal) -> void {
  journal.push_back(JournalEntry{node, tree_[node]});
  using std::swap;
  swap(tree_[node], replacement);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::apply_tag(size_type node, size_type length, const tag_type& action, journal_type& journal) -> void {
  TreeNode replacement = tree_[node];
  replacement.value    = acted_.apply(replacement.value, action, length);
  replacement.lazy =
      replacement.lazy.has_value() ? std::optional<tag_type>(acted_.compose(*replacement.lazy, action)) : std::optional<tag_type>(action);
  replace_node(node, std::move(replacement), journal);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::push_down(size_type node, size_type left, size_type right, journal_type& journal) -> void {
  if (!tree_[node].lazy.has_value() || left == right) {
    return;
  }

  const tag_type  action    = *tree_[node].lazy;
  const size_type middle    = std::midpoint(left, right);
  const size_type left_len  = middle - left + 1;
  const size_type right_len = right - middle;

  apply_tag(2 * node, left_len, action, journal);
  apply_tag(2 * node + 1, right_len, action, journal);

  TreeNode replacement = tree_[node];
  replacement.lazy.reset();
  replace_node(node, std::move(replacement), journal);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::pull_up(size_type node, journal_type& journal) -> void {
  TreeNode replacement = tree_[node];
  replacement.value    = acted_.combine(tree_[2 * node].value, tree_[2 * node + 1].value);
  replace_node(node, std::move(replacement), journal);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::update_range(
    size_type       node,
    size_type       tree_left,
    size_type       tree_right,
    size_type       query_left,
    size_type       query_right,
    const tag_type& action,
    journal_type&   journal) -> void {
  if (query_right < tree_left || tree_right < query_left) {
    return;
  }
  if (query_left <= tree_left && tree_right <= query_right) {
    apply_tag(node, tree_right - tree_left + 1, action, journal);
    return;
  }

  push_down(node, tree_left, tree_right, journal);
  const size_type middle = std::midpoint(tree_left, tree_right);
  update_range(2 * node, tree_left, middle, query_left, query_right, action, journal);
  update_range(2 * node + 1, middle + 1, tree_right, query_left, query_right, action, journal);
  pull_up(node, journal);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::set_node(
    size_type node, size_type left, size_type right, size_type index, const value_type& value, journal_type& journal) -> void {
  if (left == right) {
    replace_node(node, TreeNode(value), journal);
    return;
  }

  push_down(node, left, right, journal);
  const size_type middle = std::midpoint(left, right);
  if (index <= middle) {
    set_node(2 * node, left, middle, index, value, journal);
  } else {
    set_node(2 * node + 1, middle + 1, right, index, value, journal);
  }
  pull_up(node, journal);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::query_range(
    size_type                      node,
    size_type                      tree_left,
    size_type                      tree_right,
    size_type                      query_left,
    size_type                      query_right,
    const std::optional<tag_type>& inherited) const -> value_type {
  if (query_right < tree_left || tree_right < query_left) {
    return acted_.value_identity();
  }
  if (query_left <= tree_left && tree_right <= query_right) {
    return inherited.has_value() ? acted_.apply(tree_[node].value, *inherited, tree_right - tree_left + 1) : tree_[node].value;
  }

  std::optional<tag_type> next = tree_[node].lazy;
  if (inherited.has_value()) {
    next = next.has_value() ? std::optional<tag_type>(acted_.compose(*next, *inherited)) : inherited;
  }

  const size_type middle = std::midpoint(tree_left, tree_right);
  return acted_.combine(
      query_range(2 * node, tree_left, middle, query_left, query_right, next),
      query_range(2 * node + 1, middle + 1, tree_right, query_left, query_right, next));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::rollback(journal_type& journal) noexcept -> void {
  using std::swap;
  for (size_type i = journal.size(); i > 0; --i) {
    swap(tree_[journal[i - 1].index], journal[i - 1].previous);
  }
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::validate_index(size_type index) const -> void {
  if (index >= size_) {
    throw RangeIndexException("LazySegmentTree index out of range");
  }
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto LazySegmentTree<Value, Acted>::validate_range(size_type left, size_type right) const -> void {
  if (size_ == 0 || left > right || right >= size_) {
    throw InvalidRangeException("LazySegmentTree invalid or out-of-bounds range");
  }
}

} // namespace ads::range

//===---------------------------------------------------------------------------===//
