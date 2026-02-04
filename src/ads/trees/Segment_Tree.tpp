//===---------------------------------------------------------------------------===//
/**
 * @file Segment_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the SegmentTree class.
 * @version 0.2
 * @date 2026-02-04
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/trees/Segment_Tree.hpp"
#include <numeric>

namespace ads::trees {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree() :
    combine_(), identity_(), leaf_builder_(), values_(), tree_(), size_(0) {
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(Combine combine, Identity identity, LeafBuilder leaf_builder) :
    combine_(std::move(combine)), identity_(std::move(identity)), leaf_builder_(std::move(leaf_builder)), values_(), tree_(), size_(0) {
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(size_t size)
  requires std::default_initializable<Value>
    : combine_(), identity_(), leaf_builder_(), values_(), tree_(), size_(0) {
  reset(size);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(
    size_t size, Combine combine, Identity identity, LeafBuilder leaf_builder)
  requires std::default_initializable<Value>
    : combine_(std::move(combine)), identity_(std::move(identity)), leaf_builder_(std::move(leaf_builder)), values_(), tree_(), size_(0) {
  reset(size);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(size_t size, const Value& value) :
    combine_(), identity_(), leaf_builder_(), values_(size, value), tree_(), size_(size) {
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(
    size_t size, const Value& value, Combine combine, Identity identity, LeafBuilder leaf_builder) :
    combine_(std::move(combine)), identity_(std::move(identity)), leaf_builder_(std::move(leaf_builder)), values_(size, value), tree_(),
    size_(size) {
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(const std::vector<Value>& values) :
    combine_(), identity_(), leaf_builder_(), values_(), tree_(), size_(0) {
  build(values);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(
    const std::vector<Value>& values, Combine combine, Identity identity, LeafBuilder leaf_builder) :
    combine_(std::move(combine)), identity_(std::move(identity)), leaf_builder_(std::move(leaf_builder)), values_(), tree_(), size_(0) {
  build(values);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(std::initializer_list<Value> values) :
    combine_(), identity_(), leaf_builder_(), values_(), tree_(), size_(0) {
  build(values);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(
    std::initializer_list<Value> values, Combine combine, Identity identity, LeafBuilder leaf_builder) :
    combine_(std::move(combine)), identity_(std::move(identity)), leaf_builder_(std::move(leaf_builder)), values_(), tree_(), size_(0) {
  build(values);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(SegmentTree&& other) noexcept :
    combine_(std::move(other.combine_)), identity_(std::move(other.identity_)), leaf_builder_(std::move(other.leaf_builder_)),
    values_(std::move(other.values_)), tree_(std::move(other.tree_)), size_(other.size_) {
  other.size_ = 0;
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::operator=(SegmentTree&& other) noexcept
    -> SegmentTree<Value, Node, Combine, Identity, LeafBuilder>& {
  if (this != &other) {
    combine_      = std::move(other.combine_);
    identity_     = std::move(other.identity_);
    leaf_builder_ = std::move(other.leaf_builder_);
    values_       = std::move(other.values_);
    tree_         = std::move(other.tree_);
    size_         = other.size_;
    other.size_   = 0;
  }
  return *this;
}

//===------------------------- MODIFICATION OPERATIONS -------------------------===//

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::build(const std::vector<Value>& values) -> void {
  values_ = values;
  size_   = values_.size();
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::build(std::initializer_list<Value> values) -> void {
  values_.assign(values.begin(), values.end());
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::set(size_t index, const Value& value) -> void {
  validate_index(index);
  values_[index] = value;
  update_node(1, 0, size_ - 1, index, value);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::add(size_t index, const Value& delta) -> void
  requires detail::SegmentTreeAddable<Value>
{
  validate_index(index);
  values_[index] = values_[index] + delta;
  update_node(1, 0, size_ - 1, index, values_[index]);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::reset(size_t size) -> void
  requires std::default_initializable<Value>
{
  values_.assign(size, Value{});
  size_ = size;
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::clear() noexcept -> void {
  values_.clear();
  tree_.clear();
  size_ = 0;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::range_query(size_t left, size_t right) const -> node_type {
  validate_range(left, right);
  return query_node(1, 0, size_ - 1, left, right).value;
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::range_sum(size_t left, size_t right) const -> node_type {
  return range_query(left, right);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::total_sum() const -> node_type {
  if (is_empty()) {
    return identity_();
  }
  return tree_[1].value;
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::value_at(size_t index) const -> const Value& {
  validate_index(index);
  return values_[index];
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::size() const noexcept -> size_t {
  return size_;
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::build_tree() -> void {
  if (size_ == 0) {
    tree_.clear();
    return;
  }

  tree_.assign(4 * size_, identity_node());
  build_node(1, 0, size_ - 1);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::build_node(size_t v, size_t tl, size_t tr) -> void {
  if (tl == tr) {
    tree_[v] = make_leaf(values_[tl]);
    return;
  }

  const size_t mid = std::midpoint(tl, tr);
  build_node(v * 2, tl, mid);
  build_node(v * 2 + 1, mid + 1, tr);
  tree_[v] = combine_nodes(tree_[v * 2], tree_[v * 2 + 1]);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::update_node(size_t v, size_t tl, size_t tr, size_t index, const Value& value)
    -> void {
  if (tl == tr) {
    tree_[v] = make_leaf(value);
    return;
  }

  const size_t mid = std::midpoint(tl, tr);
  if (index <= mid) {
    update_node(v * 2, tl, mid, index, value);
  } else {
    update_node(v * 2 + 1, mid + 1, tr, index, value);
  }
  tree_[v] = combine_nodes(tree_[v * 2], tree_[v * 2 + 1]);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::query_node(size_t v, size_t tl, size_t tr, size_t l, size_t r) const
    -> TreeNode {
  if (l == tl && r == tr) {
    return tree_[v];
  }

  const size_t mid = std::midpoint(tl, tr);
  if (r <= mid) {
    return query_node(v * 2, tl, mid, l, r);
  }
  if (l > mid) {
    return query_node(v * 2 + 1, mid + 1, tr, l, r);
  }

  TreeNode left  = query_node(v * 2, tl, mid, l, mid);
  TreeNode right = query_node(v * 2 + 1, mid + 1, tr, mid + 1, r);
  return combine_nodes(left, right);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::make_leaf(const Value& value) const -> TreeNode {
  return TreeNode{leaf_builder_(value)};
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::combine_nodes(const TreeNode& left, const TreeNode& right) const
    -> TreeNode {
  return TreeNode{combine_(left.value, right.value)};
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::identity_node() const -> TreeNode {
  return TreeNode{identity_()};
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::validate_index(size_t index) const -> void {
  if (index >= size_) {
    throw SegmentTreeException("SegmentTree index out of range");
  }
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::validate_range(size_t left, size_t right) const -> void {
  if (left > right) {
    throw SegmentTreeException("SegmentTree invalid range: left > right");
  }
  if (right >= size_) {
    throw SegmentTreeException("SegmentTree range out of bounds");
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
