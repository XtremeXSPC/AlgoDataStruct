//===---------------------------------------------------------------------------===//
/**
 * @file Segment_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the SegmentTree class.
 * @version 0.1
 * @date 2026-02-03
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/trees/Segment_Tree.hpp"

namespace ads::trees {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T, typename Combine, typename Identity>
SegmentTree<T, Combine, Identity>::SegmentTree() : combine_(), identity_(), values_(), tree_(), size_(0) {
}

template <typename T, typename Combine, typename Identity>
SegmentTree<T, Combine, Identity>::SegmentTree(size_t size) : combine_(), identity_(), values_(), tree_(), size_(0) {
  reset(size);
}

template <typename T, typename Combine, typename Identity>
SegmentTree<T, Combine, Identity>::SegmentTree(size_t size, const T& value) :
    combine_(), identity_(), values_(size, value), tree_(), size_(size) {
  build_tree();
}

template <typename T, typename Combine, typename Identity>
SegmentTree<T, Combine, Identity>::SegmentTree(const std::vector<T>& values) : combine_(), identity_(), values_(), tree_(), size_(0) {
  build(values);
}

template <typename T, typename Combine, typename Identity>
SegmentTree<T, Combine, Identity>::SegmentTree(std::initializer_list<T> values) : combine_(), identity_(), values_(), tree_(), size_(0) {
  build(values);
}

template <typename T, typename Combine, typename Identity>
SegmentTree<T, Combine, Identity>::SegmentTree(SegmentTree&& other) noexcept :
    combine_(std::move(other.combine_)), identity_(std::move(other.identity_)), values_(std::move(other.values_)),
    tree_(std::move(other.tree_)), size_(other.size_) {
  other.size_ = 0;
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::operator=(SegmentTree&& other) noexcept -> SegmentTree<T, Combine, Identity>& {
  if (this != &other) {
    combine_    = std::move(other.combine_);
    identity_   = std::move(other.identity_);
    values_     = std::move(other.values_);
    tree_       = std::move(other.tree_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===------------------------- MODIFICATION OPERATIONS -------------------------===//

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::build(const std::vector<T>& values) -> void {
  values_ = values;
  size_   = values_.size();
  build_tree();
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::build(std::initializer_list<T> values) -> void {
  values_.assign(values.begin(), values.end());
  size_ = values_.size();
  build_tree();
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::set(size_t index, const T& value) -> void {
  validate_index(index);
  values_[index] = value;
  update_node(1, 0, size_ - 1, index, value);
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::add(size_t index, const T& delta) -> void {
  validate_index(index);
  values_[index] = values_[index] + delta;
  update_node(1, 0, size_ - 1, index, values_[index]);
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::reset(size_t size) -> void {
  values_.assign(size, identity_());
  size_ = size;
  build_tree();
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::clear() noexcept -> void {
  values_.clear();
  tree_.clear();
  size_ = 0;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::range_query(size_t left, size_t right) const -> T {
  validate_range(left, right);
  return query_node(1, 0, size_ - 1, left, right).value;
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::range_sum(size_t left, size_t right) const -> T {
  return range_query(left, right);
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::total_sum() const -> T {
  if (is_empty()) {
    return identity_();
  }
  return tree_[1].value;
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::value_at(size_t index) const -> const T& {
  validate_index(index);
  return values_[index];
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::size() const noexcept -> size_t {
  return size_;
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::build_tree() -> void {
  if (size_ == 0) {
    tree_.clear();
    return;
  }

  tree_.assign(4 * size_, identity_node());
  build_node(1, 0, size_ - 1);
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::build_node(size_t v, size_t tl, size_t tr) -> void {
  if (tl == tr) {
    tree_[v] = make_leaf(values_[tl]);
    return;
  }

  const size_t mid = tl + (tr - tl) / 2;
  build_node(v * 2, tl, mid);
  build_node(v * 2 + 1, mid + 1, tr);
  tree_[v] = combine_nodes(tree_[v * 2], tree_[v * 2 + 1]);
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::update_node(size_t v, size_t tl, size_t tr, size_t index, const T& value) -> void {
  if (tl == tr) {
    tree_[v] = make_leaf(value);
    return;
  }

  const size_t mid = tl + (tr - tl) / 2;
  if (index <= mid) {
    update_node(v * 2, tl, mid, index, value);
  } else {
    update_node(v * 2 + 1, mid + 1, tr, index, value);
  }
  tree_[v] = combine_nodes(tree_[v * 2], tree_[v * 2 + 1]);
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::query_node(size_t v, size_t tl, size_t tr, size_t l, size_t r) const -> Node {
  if (l == tl && r == tr) {
    return tree_[v];
  }

  const size_t mid = tl + (tr - tl) / 2;
  if (r <= mid) {
    return query_node(v * 2, tl, mid, l, r);
  }
  if (l > mid) {
    return query_node(v * 2 + 1, mid + 1, tr, l, r);
  }

  Node left  = query_node(v * 2, tl, mid, l, mid);
  Node right = query_node(v * 2 + 1, mid + 1, tr, mid + 1, r);
  return combine_nodes(left, right);
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::make_leaf(const T& value) const -> Node {
  return Node{value};
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::combine_nodes(const Node& left, const Node& right) const -> Node {
  return Node{combine_(left.value, right.value)};
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::identity_node() const -> Node {
  return Node{identity_()};
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::validate_index(size_t index) const -> void {
  if (index >= size_) {
    throw SegmentTreeException("SegmentTree index out of range");
  }
}

template <typename T, typename Combine, typename Identity>
auto SegmentTree<T, Combine, Identity>::validate_range(size_t left, size_t right) const -> void {
  if (left > right) {
    throw SegmentTreeException("SegmentTree invalid range: left > right");
  }
  if (right >= size_) {
    throw SegmentTreeException("SegmentTree range out of bounds");
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
