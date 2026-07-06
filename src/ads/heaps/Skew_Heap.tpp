//===---------------------------------------------------------------------------===//
/**
 * @file Skew_Heap.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the SkewHeap class.
 * @version 0.1
 * @date 2026-07-06
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/heaps/Skew_Heap.hpp"

#include <string>

namespace ads::heaps {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <HeapValue T, typename Compare>
SkewHeap<T, Compare>::SkewHeap(Compare comp) noexcept(std::is_nothrow_move_constructible_v<Compare>) : comp_(std::move(comp)) {
}

template <HeapValue T, typename Compare>
SkewHeap<T, Compare>::SkewHeap(std::initializer_list<T> values, Compare comp) requires CopyHeapValue<T>
    : comp_(std::move(comp)) {
  for (const T& value : values) {
    insert(value);
  }
}

template <HeapValue T, typename Compare>
template <std::input_iterator InputIt>
SkewHeap<T, Compare>::SkewHeap(InputIt first, InputIt last, Compare comp) requires HeapRangeValue<InputIt, T>
    : comp_(std::move(comp)) {
  for (; first != last; ++first) {
    emplace(*first);
  }
}

template <HeapValue T, typename Compare>
SkewHeap<T, Compare>::SkewHeap(const std::vector<T>& elements, Compare comp) requires CopyHeapValue<T>
    : comp_(std::move(comp)) {
  for (const T& value : elements) {
    insert(value);
  }
}

template <HeapValue T, typename Compare>
SkewHeap<T, Compare>::SkewHeap(SkewHeap&& other) noexcept :
    root_(std::move(other.root_)),
    size_(other.size_),
    comp_(std::move(other.comp_)) {
  other.size_ = 0;
}

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::operator=(SkewHeap&& other) noexcept -> SkewHeap& {
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
SkewHeap<T, Compare>::~SkewHeap() {
  clear();
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::insert(const T& value) -> void requires CopyHeapValue<T>
{
  attach(std::make_unique<Node>(value));
}

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::insert(T&& value) -> void requires MoveHeapValue<T>
{
  attach(std::make_unique<Node>(std::move(value)));
}

template <HeapValue T, typename Compare>
template <typename... Args>
auto SkewHeap<T, Compare>::emplace(Args&&... args) -> T& requires EmplaceHeapValue<T, Args...>
{
  return attach(std::make_unique<Node>(std::forward<Args>(args)...)).value;
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::top() const -> const T& {
  validate_non_empty("top");
  return root_->value;
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::extract_top() -> T {
  validate_non_empty("extract_top");
  NodePtr old_root = std::move(root_);
  T       result   = std::move(old_root->value);
  root_            = merge_nodes(std::move(old_root->left), std::move(old_root->right), comp_);
  --size_;
  return result;
}

//===----- MELD OPERATIONS -----------------------------------------------------===//

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::merge(SkewHeap&& other) -> void {
  if (this == &other) {
    return; // self-merge is a no-op
  }
  if (!detail::compatible_comparators(comp_, other.comp_)) {
    throw HeapException("SkewHeap::merge: incompatible comparator objects");
  }
  root_ = merge_nodes(std::move(root_), std::move(other.root_), comp_);
  size_ += other.size_;
  other.size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::size() const noexcept -> size_type {
  return size_;
}

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::clear() noexcept -> void {
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

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::merge_nodes(NodePtr lhs, NodePtr rhs, const Compare& comp) -> NodePtr {
  if (!lhs) {
    return rhs;
  }
  if (!rhs) {
    return lhs;
  }

  // Phase 1 -- Merge the two right spines. Each spine is sorted by priority
  // (heap order holds along every downward path), so a standard sorted merge
  // yields one priority-ordered chain linked through 'right'. Every picked node
  // keeps its original left child untouched for phase 2. 'tail' tracks the last
  // node taken while both spines were non-empty; the remaining spine is spliced
  // on after it and must NOT be re-swapped (it is already a valid skew subtree).
  NodePtr merged;         // head of the merged right-spine chain
  Node*   tail = nullptr; // last node linked into 'merged'

  while (lhs && rhs) {
    // comp(a, b) == true means a ranks below b, so pick the higher-priority root
    // (ties keep 'lhs', giving a stable, deterministic order).
    NodePtr& source = comp(lhs->value, rhs->value) ? rhs : lhs;
    NodePtr  picked = std::move(source);
    source          = std::move(picked->right); // advance that spine
    Node* raw       = picked.get();
    if (tail == nullptr) {
      merged = std::move(picked);
    } else {
      tail->right = std::move(picked);
    }
    tail = raw;
  }

  // Splice the leftover spine (possibly null) onto the chain as the tail's link.
  tail->right = lhs ? std::move(lhs) : std::move(rhs);

  // Phase 2 -- Walk the merged chain and swap each picked node's children so the
  // chain becomes the left spine and every original left subtree moves right.
  // Stop at 'tail': its 'right' now points at the leftover spine, which stays as
  // it is. Capturing 'next' before the swap keeps the traversal on the original
  // chain even as the pointers are rearranged.
  Node* cur = merged.get();
  while (cur != nullptr) {
    Node* next = (cur == tail) ? nullptr : cur->right.get();
    std::swap(cur->left, cur->right);
    cur = next;
  }

  return merged;
}

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::attach(NodePtr node) -> Node& {
  Node& inserted = *node;
  root_          = merge_nodes(std::move(root_), std::move(node), comp_);
  ++size_;
  return inserted;
}

template <HeapValue T, typename Compare>
auto SkewHeap<T, Compare>::validate_non_empty(const char* operation) const -> void {
  if (is_empty()) {
    throw HeapException(std::string("SkewHeap::") + operation + ": heap is empty");
  }
}

} // namespace ads::heaps

//===---------------------------------------------------------------------------===//
