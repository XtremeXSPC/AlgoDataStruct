//===---------------------------------------------------------------------------===//
/**
 * @file Leftist_Heap.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the LeftistHeap class.
 * @version 0.1
 * @date 2026-06-28
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/heaps/Leftist_Heap.hpp"

#include <string>

namespace ads::heaps {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <HeapValue T, typename Compare>
LeftistHeap<T, Compare>::LeftistHeap(Compare comp) noexcept(std::is_nothrow_move_constructible_v<Compare>) : comp_(std::move(comp)) {
}

template <HeapValue T, typename Compare>
LeftistHeap<T, Compare>::LeftistHeap(std::initializer_list<T> values, Compare comp) requires CopyHeapValue<T>
    : comp_(std::move(comp)) {
  for (const T& value : values) {
    insert(value);
  }
}

template <HeapValue T, typename Compare>
template <std::input_iterator InputIt>
LeftistHeap<T, Compare>::LeftistHeap(InputIt first, InputIt last, Compare comp) requires HeapRangeValue<InputIt, T>
    : comp_(std::move(comp)) {
  for (; first != last; ++first) {
    emplace(*first);
  }
}

template <HeapValue T, typename Compare>
LeftistHeap<T, Compare>::LeftistHeap(const std::vector<T>& elements, Compare comp) requires CopyHeapValue<T>
    : comp_(std::move(comp)) {
  for (const T& value : elements) {
    insert(value);
  }
}

template <HeapValue T, typename Compare>
LeftistHeap<T, Compare>::LeftistHeap(LeftistHeap&& other) noexcept :
    root_(std::move(other.root_)),
    size_(other.size_),
    comp_(std::move(other.comp_)) {
  other.size_ = 0;
}

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::operator=(LeftistHeap&& other) noexcept -> LeftistHeap& {
  if (this != &other) {
    clear();
    root_       = std::move(other.root_);
    size_       = other.size_;
    comp_       = std::move(other.comp_);
    other.size_ = 0;
  }
  return *this;
}

template <HeapValue T, typename Compare>
LeftistHeap<T, Compare>::~LeftistHeap() {
  clear();
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::insert(const T& value) -> void requires CopyHeapValue<T>
{
  attach(std::make_unique<Node>(value));
}

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::insert(T&& value) -> void requires MoveHeapValue<T>
{
  attach(std::make_unique<Node>(std::move(value)));
}

template <HeapValue T, typename Compare>
template <typename... Args>
auto LeftistHeap<T, Compare>::emplace(Args&&... args) -> T& requires EmplaceHeapValue<T, Args...>
{
  return attach(std::make_unique<Node>(std::forward<Args>(args)...)).value;
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::top() const -> const T& {
  validate_non_empty("top");
  return root_->value;
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::extract_top() -> T {
  validate_non_empty("extract_top");
  NodePtr old_root = std::move(root_);
  T       result   = std::move(old_root->value);
  root_            = merge_nodes(std::move(old_root->left), std::move(old_root->right), comp_);
  --size_;
  return result;
}

//===----- MELD OPERATIONS -----------------------------------------------------===//

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::merge(LeftistHeap&& other) -> void {
  if (this == &other) {
    return; // self-merge is a no-op
  }
  if (!detail::compatible_comparators(comp_, other.comp_)) {
    throw HeapException("LeftistHeap::merge: incompatible comparator objects");
  }
  root_ = merge_nodes(std::move(root_), std::move(other.root_), comp_);
  size_ += other.size_;
  other.size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::size() const noexcept -> size_type {
  return size_;
}

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::clear() noexcept -> void {
  // Iterative, allocation-free teardown: repeatedly right-rotate so the node we
  // are about to drop has no left child. This keeps ~Node from recursing down a
  // potentially long left spine.
  NodePtr node = std::move(root_);
  while (node) {
    if (node->left) {
      NodePtr left = std::move(node->left);
      node->left   = std::move(left->right);
      left->right  = std::move(node);
      node         = std::move(left);
    } else {
      node = std::move(node->right);
    }
  }
  size_ = 0;
}

//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::npl_of(const NodePtr& node) noexcept -> int {
  return node ? node->npl : -1;
}

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::merge_nodes(NodePtr lhs, NodePtr rhs, const Compare& comp) -> NodePtr {
  if (!lhs) {
    return rhs;
  }
  if (!rhs) {
    return lhs;
  }
  // Keep the higher-priority root on top: if lhs ranks below rhs, swap them so
  // the survivor is the element that should sit closer to the top.
  if (comp(lhs->value, rhs->value)) {
    std::swap(lhs, rhs);
  }
  lhs->right = merge_nodes(std::move(lhs->right), std::move(rhs), comp);
  if (npl_of(lhs->left) < npl_of(lhs->right)) {
    std::swap(lhs->left, lhs->right);
  }
  lhs->npl = npl_of(lhs->right) + 1;
  return lhs;
}

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::attach(NodePtr node) -> Node& {
  Node& inserted = *node;
  root_          = merge_nodes(std::move(root_), std::move(node), comp_);
  ++size_;
  return inserted;
}

template <HeapValue T, typename Compare>
auto LeftistHeap<T, Compare>::validate_non_empty(const char* operation) const -> void {
  if (is_empty()) {
    throw HeapException(std::string("LeftistHeap::") + operation + ": heap is empty");
  }
}

} // namespace ads::heaps

//===---------------------------------------------------------------------------===//
