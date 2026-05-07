//===---------------------------------------------------------------------------===//
/**
 * @file Lazy_Segment_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the LazySegmentTree class.
 * @version 1.0
 * @date 2026-02-05
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/trees/Lazy_Segment_Tree.hpp"

#include <numeric>

namespace ads::trees {

// Storage choice: DynamicArray keeps the value table and lazy tree nodes
// contiguous for recursive lazy propagation.

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::LazySegmentTree(
    Combine combine, Apply apply, Compose compose, Identity identity) :
    combine_(std::move(combine)),
    apply_(std::move(apply)),
    compose_(std::move(compose)),
    identity_(std::move(identity)),
    values_(),
    tree_(),
    size_(0) {
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::LazySegmentTree(size_type size)
  requires std::default_initializable<Value>
    : combine_(), apply_(), compose_(), identity_(), values_(), tree_(), size_(0) {
  reset(size);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::LazySegmentTree(
    size_type size, const Value& value) :
    combine_(),
    apply_(),
    compose_(),
    identity_(),
    values_(size, value),
    tree_(),
    size_(size) {
  build_tree();
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::LazySegmentTree(
    const std::vector<Value>& values) :
    combine_(),
    apply_(),
    compose_(),
    identity_(),
    values_(values.begin(), values.end()),
    tree_(),
    size_(values.size()) {
  build_tree();
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::LazySegmentTree(std::vector<Value>&& values) :
    combine_(),
    apply_(),
    compose_(),
    identity_(),
    values_(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())),
    tree_(),
    size_(0) {
  // Preserve the previous vector-backed move behavior that callers may observe.
  values.clear();
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::LazySegmentTree(
    std::initializer_list<Value> values) :
    combine_(),
    apply_(),
    compose_(),
    identity_(),
    values_(values),
    tree_(),
    size_(values.size()) {
  build_tree();
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
template <std::input_iterator InputIt>
constexpr LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::LazySegmentTree(InputIt first, InputIt last) :
    combine_(),
    apply_(),
    compose_(),
    identity_(),
    values_(first, last),
    tree_(),
    size_(0) {
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::LazySegmentTree(
    LazySegmentTree&& other) noexcept :
    combine_(std::move(other.combine_)),
    apply_(std::move(other.apply_)),
    compose_(std::move(other.compose_)),
    identity_(std::move(other.identity_)),
    values_(std::move(other.values_)),
    tree_(std::move(other.tree_)),
    size_(other.size_) {
  other.size_ = 0;
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto
LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::operator=(LazySegmentTree&& other) noexcept
    -> LazySegmentTree& {
  if (this != &other) {
    combine_    = std::move(other.combine_);
    apply_      = std::move(other.apply_);
    compose_    = std::move(other.compose_);
    identity_   = std::move(other.identity_);
    values_     = std::move(other.values_);
    tree_       = std::move(other.tree_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===------------------------- MODIFICATION OPERATIONS -------------------------===//

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::build(const std::vector<Value>& values)
    -> void {
  values_.assign(values.begin(), values.end());
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::build(std::vector<Value>&& values)
    -> void {
  values_.assign(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()));
  // Preserve the previous vector-backed move behavior that callers may observe.
  values.clear();
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto
LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::build(std::initializer_list<Value> values) -> void {
  values_.assign(values);
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
template <std::input_iterator InputIt>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::build(InputIt first, InputIt last)
    -> void
  requires std::constructible_from<Value, std::iter_reference_t<InputIt>>
{
  values_.assign(first, last);
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::set(size_type index, const Value& value)
    -> void {
  validate_index(index);
  // Preserve pending range updates by materializing only the path to the leaf.
  set_node(1, 0, size_ - 1, index, value);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::update(size_type index, const Tag& tag)
    -> void {
  validate_index(index);
  update_range(1, 0, size_ - 1, index, index, tag);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::range_update(
    size_type left, size_type right, const Tag& tag) -> void {
  validate_range(left, right);
  update_range(1, 0, size_ - 1, left, right, tag);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::reset(size_type size) -> void
  requires std::default_initializable<Value>
{
  values_.assign(size, Value{});
  size_ = size;
  build_tree();
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::clear() noexcept -> void {
  values_.clear();
  tree_.clear();
  size_ = 0;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto
LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::range_query(size_type left, size_type right) const
    -> value_type {
  validate_range(left, right);
  return query_range(1, 0, size_ - 1, left, right);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::value_at(size_type index) const
    -> value_type {
  validate_index(index);
  return query_range(1, 0, size_ - 1, index, index);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::total() const -> value_type {
  if (is_empty()) {
    return identity_();
  }
  return tree_[1].value;
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::size() const noexcept -> size_type {
  return size_;
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::empty() const noexcept -> bool {
  return is_empty();
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::build_tree() -> void {
  if (size_ == 0) {
    tree_.clear();
    return;
  }

  tree_.assign(4 * size_, TreeNode{identity_()});
  build_node(1, 0, size_ - 1);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto
LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::build_node(size_type v, size_type tl, size_type tr)
    -> void {
  tree_[v].lazy = std::nullopt;

  if (tl == tr) {
    tree_[v].value = values_[tl];
    return;
  }

  const size_type mid = std::midpoint(tl, tr);
  build_node(2 * v, tl, mid);
  build_node(2 * v + 1, mid + 1, tr);
  pull_up(v);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto
LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::push_down(size_type v, size_type tl, size_type tr) const
    -> void {
  if (!tree_[v].lazy.has_value()) {
    return;
  }

  const size_type mid       = std::midpoint(tl, tr);
  const size_type left_len  = mid - tl + 1;
  const size_type right_len = tr - mid;

  apply_tag(2 * v, left_len, tree_[v].lazy.value());
  apply_tag(2 * v + 1, right_len, tree_[v].lazy.value());
  tree_[v].lazy = std::nullopt;
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::pull_up(size_type v) -> void {
  tree_[v].value = combine_(tree_[2 * v].value, tree_[2 * v + 1].value);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::apply_tag(
    size_type v, size_type len, const tag_type& tag) const -> void {
  tree_[v].value = apply_(tree_[v].value, tag, len);
  if (tree_[v].lazy.has_value()) {
    tree_[v].lazy = compose_(tree_[v].lazy.value(), tag);
  } else {
    tree_[v].lazy = tag;
  }
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::update_range(
    size_type v, size_type tl, size_type tr, size_type l, size_type r, const tag_type& tag) -> void {
  if (l > tr || r < tl) {
    return;
  }

  if (l <= tl && tr <= r) {
    apply_tag(v, tr - tl + 1, tag);
    return;
  }

  push_down(v, tl, tr);
  const size_type mid = std::midpoint(tl, tr);
  update_range(2 * v, tl, mid, l, r, tag);
  update_range(2 * v + 1, mid + 1, tr, l, r, tag);
  pull_up(v);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::set_node(
    size_type v, size_type tl, size_type tr, size_type index, const value_type& value) -> void {
  if (tl == tr) {
    values_[index] = value;
    tree_[v].value = value;
    tree_[v].lazy  = std::nullopt;
    return;
  }

  // Pushing first keeps sibling ranges current before this assignment rewrites one leaf.
  push_down(v, tl, tr);

  const size_type mid = std::midpoint(tl, tr);
  if (index <= mid) {
    set_node(2 * v, tl, mid, index, value);
  } else {
    set_node(2 * v + 1, mid + 1, tr, index, value);
  }
  pull_up(v);
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::query_range(
    size_type v, size_type tl, size_type tr, size_type l, size_type r) const -> value_type {
  if (l > tr || r < tl) {
    return identity_();
  }

  if (l <= tl && tr <= r) {
    return tree_[v].value;
  }

  // Push down lazy tags (logical const operation over mutable lazy state).
  push_down(v, tl, tr);

  const size_type mid = std::midpoint(tl, tr);
  return combine_(query_range(2 * v, tl, mid, l, r), query_range(2 * v + 1, mid + 1, tr, l, r));
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::validate_index(size_type index) const
    -> void {
  if (index >= size_) {
    throw SegmentTreeException("LazySegmentTree index out of range");
  }
}

template <typename Value, typename Tag, typename Combine, typename Apply, typename Compose, typename Identity>
  requires detail::LazySegmentTreeTraits<Value, Tag, Combine, Apply, Compose, Identity>
constexpr auto
LazySegmentTree<Value, Tag, Combine, Apply, Compose, Identity>::validate_range(size_type left, size_type right) const
    -> void {
  if (left > right) {
    throw SegmentTreeException("LazySegmentTree invalid range: left > right");
  }
  if (right >= size_) {
    throw SegmentTreeException("LazySegmentTree range out of bounds");
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
