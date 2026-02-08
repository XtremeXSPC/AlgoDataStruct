//===---------------------------------------------------------------------------===//
/**
 * @file Segment_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the SegmentTree class (iterative approach).
 * @version 1.0
 * @date 2026-02-05
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/trees/Segment_Tree.hpp"

namespace ads::trees {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::
    SegmentTree(Combine combine, Identity identity, LeafBuilder leaf_builder) noexcept(
        std::is_nothrow_move_constructible_v<Combine> && std::is_nothrow_move_constructible_v<Identity>
        && std::is_nothrow_move_constructible_v<LeafBuilder>) :
    combine_(std::move(combine)),
    identity_(std::move(identity)),
    leaf_builder_(std::move(leaf_builder)),
    values_(),
    tree_(),
    size_(0) {
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(size_type size)
  requires std::default_initializable<Value>
    : combine_(), identity_(), leaf_builder_(), values_(), tree_(), size_(0) {
  reset(size);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(
    size_type size, Combine combine, Identity identity, LeafBuilder leaf_builder)
  requires std::default_initializable<Value>
    :
    combine_(std::move(combine)),
    identity_(std::move(identity)),
    leaf_builder_(std::move(leaf_builder)),
    values_(),
    tree_(),
    size_(0) {
  reset(size);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(size_type size, const Value& value) :
    combine_(),
    identity_(),
    leaf_builder_(),
    values_(size, value),
    tree_(),
    size_(size) {
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(
    size_type size, const Value& value, Combine combine, Identity identity, LeafBuilder leaf_builder) :
    combine_(std::move(combine)),
    identity_(std::move(identity)),
    leaf_builder_(std::move(leaf_builder)),
    values_(size, value),
    tree_(),
    size_(size) {
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(const std::vector<Value>& values) :
    combine_(),
    identity_(),
    leaf_builder_(),
    values_(values),
    tree_(),
    size_(values.size()) {
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(std::vector<Value>&& values) noexcept(
    std::is_nothrow_move_constructible_v<std::vector<Value>>) :
    combine_(),
    identity_(),
    leaf_builder_(),
    values_(std::move(values)),
    tree_(),
    size_(0) {
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(
    const std::vector<Value>& values, Combine combine, Identity identity, LeafBuilder leaf_builder) :
    combine_(std::move(combine)),
    identity_(std::move(identity)),
    leaf_builder_(std::move(leaf_builder)),
    values_(values),
    tree_(),
    size_(values.size()) {
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::
    SegmentTree(std::vector<Value>&& values, Combine combine, Identity identity, LeafBuilder leaf_builder) noexcept(
        std::is_nothrow_move_constructible_v<std::vector<Value>> && std::is_nothrow_move_constructible_v<Combine>
        && std::is_nothrow_move_constructible_v<Identity> && std::is_nothrow_move_constructible_v<LeafBuilder>) :
    combine_(std::move(combine)),
    identity_(std::move(identity)),
    leaf_builder_(std::move(leaf_builder)),
    values_(std::move(values)),
    tree_(),
    size_(0) {
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(std::initializer_list<Value> values) :
    combine_(),
    identity_(),
    leaf_builder_(),
    values_(values),
    tree_(),
    size_(values.size()) {
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(
    std::initializer_list<Value> values, Combine combine, Identity identity, LeafBuilder leaf_builder) :
    combine_(std::move(combine)),
    identity_(std::move(identity)),
    leaf_builder_(std::move(leaf_builder)),
    values_(values),
    tree_(),
    size_(values.size()) {
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
template <std::input_iterator InputIt>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(InputIt first, InputIt last) :
    combine_(),
    identity_(),
    leaf_builder_(),
    values_(first, last),
    tree_(),
    size_(0) {
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
template <std::input_iterator InputIt>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(
    InputIt first, InputIt last, Combine combine, Identity identity, LeafBuilder leaf_builder) :
    combine_(std::move(combine)),
    identity_(std::move(identity)),
    leaf_builder_(std::move(leaf_builder)),
    values_(first, last),
    tree_(),
    size_(0) {
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::SegmentTree(SegmentTree&& other) noexcept :
    combine_(std::move(other.combine_)),
    identity_(std::move(other.identity_)),
    leaf_builder_(std::move(other.leaf_builder_)),
    values_(std::move(other.values_)),
    tree_(std::move(other.tree_)),
    size_(other.size_) {
  other.size_ = 0;
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::operator=(SegmentTree&& other) noexcept
    -> SegmentTree& {
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
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::build(const std::vector<Value>& values)
    -> void {
  values_ = values;
  size_   = values_.size();
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::build(std::vector<Value>&& values) noexcept(
    std::is_nothrow_move_assignable_v<std::vector<Value>>) -> void {
  values_ = std::move(values);
  size_   = values_.size();
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::build(std::initializer_list<Value> values)
    -> void {
  values_.assign(values.begin(), values.end());
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
template <std::input_iterator InputIt>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::build(InputIt first, InputIt last) -> void {
  values_.assign(first, last);
  size_ = values_.size();
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::set(size_type index, const Value& value)
    -> void {
  validate_index(index);
  values_[index]       = value;
  tree_[size_ + index] = leaf_builder_(value);
  propagate_up(size_ + index);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::set(size_type index, Value&& value) -> void {
  validate_index(index);
  values_[index]       = std::move(value);
  tree_[size_ + index] = leaf_builder_(values_[index]);
  propagate_up(size_ + index);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::add(size_type index, const Value& delta)
    -> void
  requires detail::Addable<Value>
{
  validate_index(index);
  values_[index]       = values_[index] + delta;
  tree_[size_ + index] = leaf_builder_(values_[index]);
  propagate_up(size_ + index);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::reset(size_type size) -> void
  requires std::default_initializable<Value>
{
  values_.assign(size, Value{});
  size_ = size;
  build_tree();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::clear() noexcept -> void {
  values_.clear();
  tree_.clear();
  size_ = 0;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::range_query(size_type left, size_type right) const
    -> node_type {
  validate_range(left, right);

  // Convert to internal indices: leaves are at [size_, 2*size_).
  // We use half-open interval internally: [left, right+1).
  node_type result_left  = identity_();
  node_type result_right = identity_();

  // Start from the leaf positions.
  for (size_type l = size_ + left, r = size_ + right + 1; l < r; l >>= 1, r >>= 1) {
    // If l is a right child, include it and move to the next subtree.
    if (l & 1) {
      result_left = combine_(result_left, tree_[l]);
      ++l;
    }
    // If r is a right child, include the left sibling.
    if (r & 1) {
      --r;
      result_right = combine_(tree_[r], result_right);
    }
  }

  return combine_(result_left, result_right);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::range_sum(size_type left, size_type right) const
    -> node_type {
  return range_query(left, right);
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::total() const
    noexcept(detail::is_identity_nothrow_v<Identity>) -> node_type {
  if (is_empty()) {
    return identity_();
  }
  return tree_[1];
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::total_sum() const
    noexcept(detail::is_identity_nothrow_v<Identity>) -> node_type {
  return total();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::value_at(size_type index) const
    -> const_reference {
  validate_index(index);
  return values_[index];
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::node_at(size_type index) const -> node_type {
  validate_index(index);
  return tree_[size_ + index];
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::size() const noexcept -> size_type {
  return size_;
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::empty() const noexcept -> bool {
  return is_empty();
}

//===---------------------------- ITERATOR ACCESS ------------------------------===//

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::begin() const noexcept -> const_iterator {
  return values_.cbegin();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::end() const noexcept -> const_iterator {
  return values_.cend();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::cbegin() const noexcept -> const_iterator {
  return values_.cbegin();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::cend() const noexcept -> const_iterator {
  return values_.cend();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::rbegin() const noexcept
    -> const_reverse_iterator {
  return values_.crbegin();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::rend() const noexcept
    -> const_reverse_iterator {
  return values_.crend();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::crbegin() const noexcept
    -> const_reverse_iterator {
  return values_.crbegin();
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::crend() const noexcept
    -> const_reverse_iterator {
  return values_.crend();
}

//===--------------------------- FUNCTOR ACCESSORS -----------------------------===//

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::get_combine() const noexcept
    -> const Combine& {
  return combine_;
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::get_identity() const noexcept
    -> const Identity& {
  return identity_;
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::get_leaf_builder() const noexcept
    -> const LeafBuilder& {
  return leaf_builder_;
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::build_tree() -> void {
  if (size_ == 0) {
    tree_.clear();
    return;
  }

  // Allocate exactly 2n nodes: indices [0, n-1] for internal nodes, [n, 2n-1] for leaves.
  // Index 0 is unused in the standard formulation, index 1 is the root.
  tree_.assign(2 * size_, identity_());

  // Build leaves at positions [n, 2n-1].
  for (size_type i = 0; i < size_; ++i) {
    tree_[size_ + i] = leaf_builder_(values_[i]);
  }

  // Build internal nodes from bottom to top.
  // Node i has children at 2i and 2i+1.
  for (size_type i = size_ - 1; i >= 1; --i) {
    tree_[i] = combine_(tree_[2 * i], tree_[2 * i + 1]);
  }
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::propagate_up(size_type leaf_index) noexcept(
    detail::is_combine_nothrow_v<Combine, node_type>) -> void {
  // Move up from the leaf to the root, updating each parent.
  for (size_type i = leaf_index >> 1; i >= 1; i >>= 1) {
    tree_[i] = combine_(tree_[2 * i], tree_[2 * i + 1]);
  }
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::validate_index(size_type index) const -> void {
  if (index >= size_) {
    throw SegmentTreeException("SegmentTree index out of range");
  }
}

template <typename Value, typename Node, typename Combine, typename Identity, typename LeafBuilder>
  requires detail::SegmentTreeTraits<Value, Node, Combine, Identity, LeafBuilder>
constexpr auto
SegmentTree<Value, Node, Combine, Identity, LeafBuilder>::validate_range(size_type left, size_type right) const
    -> void {
  if (left > right) {
    throw SegmentTreeException("SegmentTree invalid range: left > right");
  }
  if (right >= size_) {
    throw SegmentTreeException("SegmentTree range out of bounds");
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
