//===---------------------------------------------------------------------------===//
/**
 * @file Pairing_Heap.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the PairingHeap class.
 * @version 0.1
 * @date 2026-07-08
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/heaps/Pairing_Heap.hpp"

#include <string>
#include <vector>

namespace ads::heaps {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <HeapValue T, typename Compare>
PairingHeap<T, Compare>::PairingHeap(Compare comp) noexcept(std::is_nothrow_move_constructible_v<Compare>) : comp_(std::move(comp)) {
}

template <HeapValue T, typename Compare>
PairingHeap<T, Compare>::PairingHeap(std::initializer_list<T> values, Compare comp) requires CopyHeapValue<T>
    : comp_(std::move(comp)) {
  for (const T& value : values) {
    insert(value);
  }
}

template <HeapValue T, typename Compare>
template <std::input_iterator InputIt>
PairingHeap<T, Compare>::PairingHeap(InputIt first, InputIt last, Compare comp) requires HeapRangeValue<InputIt, T>
    : comp_(std::move(comp)) {
  for (; first != last; ++first) {
    insert(*first);
  }
}

template <HeapValue T, typename Compare>
PairingHeap<T, Compare>::PairingHeap(const std::vector<T>& elements, Compare comp) requires CopyHeapValue<T>
    : comp_(std::move(comp)) {
  for (const T& value : elements) {
    insert(value);
  }
}

template <HeapValue T, typename Compare>
PairingHeap<T, Compare>::PairingHeap(PairingHeap&& other) noexcept :
    root_(std::move(other.root_)),
    size_(other.size_),
    comp_(std::move(other.comp_)) {
  other.size_ = 0;
}

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::operator=(PairingHeap&& other) noexcept -> PairingHeap& {
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
PairingHeap<T, Compare>::~PairingHeap() {
  clear();
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::insert(const T& value) -> void requires CopyHeapValue<T>
{
  (void)emplace(value);
}

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::insert(T&& value) -> void requires MoveHeapValue<T>
{
  (void)emplace(std::move(value));
}

template <HeapValue T, typename Compare>
template <typename... Args>
auto PairingHeap<T, Compare>::emplace(Args&&... args) -> Handle requires EmplaceHeapValue<T, Args...>
{
  NodePtr node = std::make_unique<Node>(std::forward<Args>(args)...);
  Node*   raw  = node.get();

  root_       = meld(std::move(root_), std::move(node), comp_);
  root_->prev = nullptr;
  ++size_;
  return Handle(raw);
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::top() const -> const T& {
  validate_non_empty("top");
  return root_->value;
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::extract_top() -> T {
  validate_non_empty("extract_top");
  NodePtr old    = std::move(root_);
  T       result = std::move(old->value);
  root_          = combine_siblings(std::move(old->child), comp_);
  --size_;
  return result; // 'old' dies here (its child was moved out, a root has no sibling)
}

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::erase(const Handle& handle) -> void {
  if (!handle.valid()) {
    throw HeapException("PairingHeap::erase: invalid handle");
  }
  remove_node(handle.node_);
}

//===----- UPDATE OPERATIONS ---------------------------------------------------===//

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::decrease_key(const Handle& handle, const T& new_value) -> void requires CopyHeapValue<T>
{
  if (!handle.valid()) {
    throw HeapException("PairingHeap::decrease_key: invalid handle");
  }
  Node* node = handle.node_;
  // The new value may only raise priority: reject anything ranking below current.
  if (comp_(new_value, node->value)) {
    throw HeapException("PairingHeap::decrease_key: new value lowers priority");
  }
  node->value = new_value;

  // The root already sits at the top; otherwise cut the (still valid) subtree and
  // meld it back at the root. Cutting unconditionally is correct because the
  // subtree is a self-contained heap regardless of whether order was violated.
  if (node != root_.get()) {
    NodePtr detached = detach(node);
    root_            = meld(std::move(root_), std::move(detached), comp_);
    root_->prev      = nullptr;
  }
}

//===----- MELD OPERATIONS -----------------------------------------------------===//

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::merge(PairingHeap&& other) -> void {
  if (this == &other) {
    return; // self-merge is a no-op
  }
  if (!detail::compatible_comparators(comp_, other.comp_)) {
    throw HeapException("PairingHeap::merge: incompatible comparator objects");
  }
  root_ = meld(std::move(root_), std::move(other.root_), comp_);
  if (root_) {
    root_->prev = nullptr;
  }
  size_ += other.size_;
  other.size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::size() const noexcept -> size_type {
  return size_;
}

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::clear() noexcept -> void {
  // Iterative teardown: a pairing tree can have O(n)-long child or sibling
  // chains (e.g. after n inserts the root has n children), so a recursive
  // unique_ptr cascade could overflow the stack. Move every node onto an
  // explicit work list instead.
  std::vector<NodePtr> pending;
  if (root_) {
    pending.push_back(std::move(root_));
  }
  while (!pending.empty()) {
    NodePtr node = std::move(pending.back());
    pending.pop_back();
    if (node->child) {
      pending.push_back(std::move(node->child));
    }
    if (node->sibling) {
      pending.push_back(std::move(node->sibling));
    }
    // 'node' dies here with both owning links already moved out.
  }
  size_ = 0;
}

//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::meld(NodePtr lhs, NodePtr rhs, const Compare& comp) -> NodePtr {
  if (!lhs) {
    return rhs;
  }
  if (!rhs) {
    return lhs;
  }
  // The higher-priority root becomes the parent (comp(a,b) => a ranks below b).
  if (comp(lhs->value, rhs->value)) {
    std::swap(lhs, rhs);
  }
  // Push rhs onto the front of lhs's child list.
  rhs->prev    = lhs.get();
  rhs->sibling = std::move(lhs->child);
  if (rhs->sibling) {
    rhs->sibling->prev = rhs.get();
  }
  lhs->child = std::move(rhs);
  return lhs;
}

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::combine_siblings(NodePtr first, const Compare& comp) -> NodePtr {
  if (!first) {
    return nullptr;
  }

  // Detach the sibling list into a buffer, clearing the transient links.
  std::vector<NodePtr> subtrees;
  for (NodePtr node = std::move(first); node;) {
    NodePtr next  = std::move(node->sibling);
    node->prev    = nullptr;
    node->sibling = nullptr;
    subtrees.push_back(std::move(node));
    node = std::move(next);
  }

  // First pass: meld neighbours left to right.
  std::vector<NodePtr> paired;
  paired.reserve((subtrees.size() + 1) / 2);
  for (std::size_t i = 0; i + 1 < subtrees.size(); i += 2) {
    paired.push_back(meld(std::move(subtrees[i]), std::move(subtrees[i + 1]), comp));
  }
  if (subtrees.size() % 2 == 1) {
    paired.push_back(std::move(subtrees.back()));
  }

  // Second pass: fold the paired results right to left into a single tree.
  NodePtr result = std::move(paired.back());
  paired.pop_back();
  while (!paired.empty()) {
    result = meld(std::move(paired.back()), std::move(result), comp);
    paired.pop_back();
  }
  result->prev = nullptr;
  return result;
}

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::detach(Node* node) -> NodePtr {
  // 'node' is not the root, so prev is non-null: it owns 'node' either as its
  // leftmost child or as the next sibling in the chain.
  Node*    prev = node->prev;
  NodePtr& slot = (prev->child.get() == node) ? prev->child : prev->sibling;

  NodePtr detached = std::move(slot);
  slot             = std::move(detached->sibling);
  if (slot) {
    slot->prev = prev; // the node that filled the gap keeps the same predecessor
  }
  detached->prev    = nullptr;
  detached->sibling = nullptr;
  return detached;
}

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::remove_node(Node* node) -> void {
  if (node == root_.get()) {
    NodePtr old = std::move(root_);
    root_       = combine_siblings(std::move(old->child), comp_);
  } else {
    NodePtr target  = detach(node);
    NodePtr subheap = combine_siblings(std::move(target->child), comp_);
    root_           = meld(std::move(root_), std::move(subheap), comp_);
    if (root_) {
      root_->prev = nullptr;
    }
    // 'target' dies here holding only the removed element.
  }
  --size_;
}

template <HeapValue T, typename Compare>
auto PairingHeap<T, Compare>::validate_non_empty(const char* operation) const -> void {
  if (is_empty()) {
    throw HeapException(std::string("PairingHeap::") + operation + ": heap is empty");
  }
}

} // namespace ads::heaps

//===---------------------------------------------------------------------------===//
