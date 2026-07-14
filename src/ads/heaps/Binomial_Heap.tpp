//===---------------------------------------------------------------------------===//
/**
 * @file Binomial_Heap.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the BinomialHeap class.
 * @version 0.1
 * @date 2026-07-07
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/heaps/Binomial_Heap.hpp"

#include <bit>
#include <string>
#include <vector>

namespace ads::heaps {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <HeapValue T, typename Compare>
BinomialHeap<T, Compare>::BinomialHeap(Compare comp) noexcept(std::is_nothrow_move_constructible_v<Compare>) : comp_(std::move(comp)) {
}

template <HeapValue T, typename Compare>
BinomialHeap<T, Compare>::BinomialHeap(std::initializer_list<T> values, Compare comp) requires CopyHeapValue<T>
    : comp_(std::move(comp)) {
  for (const T& value : values) {
    insert(value);
  }
}

template <HeapValue T, typename Compare>
template <std::input_iterator InputIt>
BinomialHeap<T, Compare>::BinomialHeap(InputIt first, InputIt last, Compare comp) requires HeapRangeValue<InputIt, T>
    : comp_(std::move(comp)) {
  for (; first != last; ++first) {
    insert(*first);
  }
}

template <HeapValue T, typename Compare>
BinomialHeap<T, Compare>::BinomialHeap(const std::vector<T>& elements, Compare comp) requires CopyHeapValue<T>
    : comp_(std::move(comp)) {
  for (const T& value : elements) {
    insert(value);
  }
}

template <HeapValue T, typename Compare>
BinomialHeap<T, Compare>::BinomialHeap(BinomialHeap&& other) noexcept :
    head_(std::move(other.head_)),
    size_(other.size_),
    comp_(std::move(other.comp_)) {
  other.size_ = 0;
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::operator=(BinomialHeap&& other) noexcept -> BinomialHeap& {
  if (this != &other) {
    clear();
    head_       = std::move(other.head_);
    size_       = other.size_;
    comp_       = std::move(other.comp_);
    other.size_ = 0;
  }
  return *this;
}

template <HeapValue T, typename Compare>
BinomialHeap<T, Compare>::~BinomialHeap() {
  clear();
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::insert(const T& value) -> void requires CopyHeapValue<T>
{
  (void)emplace(value);
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::insert(T&& value) -> void requires MoveHeapValue<T>
{
  (void)emplace(std::move(value));
}

template <HeapValue T, typename Compare>
template <typename... Args>
auto BinomialHeap<T, Compare>::emplace(Args&&... args) -> Handle requires EmplaceHeapValue<T, Args...>
{
  NodePtr node       = std::make_unique<Node>(std::forward<Args>(args)...);
  node->handle       = std::make_unique<HandleData>();
  node->handle->node = node.get();
  HandleData* record = node->handle.get();

  union_with(std::move(node), 1); // single degree-0 tree melded into the forest
  ++size_;
  return Handle(record);
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::top() const -> const T& {
  validate_non_empty("top");
  return find_top_root()->value;
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::extract_top() -> T {
  validate_non_empty("extract_top");
  NodePtr detached = extract_root(find_top_root());
  T       result   = std::move(detached->value);
  --size_;
  return result; // 'detached' and its handle record die here
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::erase(const Handle& handle) -> void {
  if (!handle.valid()) {
    throw HeapException("BinomialHeap::erase: invalid handle");
  }
  // Bubble the target element to the root of its binomial tree, then drop it.
  Node*   root     = sift_up(handle.data_->node, comp_, /*to_root=*/true);
  NodePtr detached = extract_root(root);
  --size_; // 'detached' and its handle record die here
}

//===----- UPDATE OPERATIONS ---------------------------------------------------===//

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::decrease_key(const Handle& handle, const T& new_value) -> void requires CopyHeapValue<T>
{
  if (!handle.valid()) {
    throw HeapException("BinomialHeap::decrease_key: invalid handle");
  }
  Node* node = handle.data_->node;
  // The new value may only raise priority: reject anything ranking below current.
  if (comp_(new_value, node->value)) {
    throw HeapException("BinomialHeap::decrease_key: new value lowers priority");
  }
  node->value = new_value;
  sift_up(node, comp_, /*to_root=*/false);
}

//===----- MELD OPERATIONS -----------------------------------------------------===//

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::merge(BinomialHeap&& other) -> void {
  if (this == &other) {
    return; // self-merge is a no-op
  }
  if (!detail::compatible_comparators(comp_, other.comp_)) {
    throw HeapException("BinomialHeap::merge: incompatible comparator objects");
  }
  union_with(std::move(other.head_), other.size_);
  size_ += other.size_;
  other.size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::is_empty() const noexcept -> bool {
  return head_ == nullptr;
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::size() const noexcept -> size_type {
  return size_;
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::clear() noexcept -> void {
  // A binomial tree of degree k has height k = O(log n); the child and sibling
  // chains are each O(log n) deep, so the unique_ptr cascade tears the forest
  // down without unbounded recursion.
  head_.reset();
  size_ = 0;
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::link(Node& parent, NodePtr child) -> void {
  child->parent  = &parent;
  child->sibling = std::move(parent.child);
  parent.child   = std::move(child);
  ++parent.degree;
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::merge_root_lists(NodePtr lhs, NodePtr rhs) -> NodePtr {
  if (!lhs) {
    return rhs;
  }
  if (!rhs) {
    return lhs;
  }
  // Standard sorted merge of two degree-ascending sibling lists.
  NodePtr head;
  Node*   tail = nullptr;
  while (lhs && rhs) {
    NodePtr& source = (lhs->degree <= rhs->degree) ? lhs : rhs;
    NodePtr  picked = std::move(source);
    source          = std::move(picked->sibling);
    Node* raw       = picked.get();
    if (tail == nullptr) {
      head = std::move(picked);
    } else {
      tail->sibling = std::move(picked);
    }
    tail = raw;
  }
  tail->sibling = lhs ? std::move(lhs) : std::move(rhs);
  return head;
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::union_with(NodePtr other_head, size_type incoming_count) -> void {
  // Reserve the coalescing buffer BEFORE consuming either root list: if this
  // allocation fails, both lists are still owned and the heap is unchanged.
  // A forest holding at most n elements has trees of degree <= floor(log2 n),
  // and the carry can push one degree further.
  const auto slot_bound = static_cast<std::size_t>(std::bit_width(size_ + incoming_count + 1)) + 2U;
  std::vector<NodePtr> slots;
  slots.reserve(slot_bound);

  NodePtr merged = merge_root_lists(std::move(head_), std::move(other_head));

  // Coalesce equal degrees exactly like binary addition with carry: 'slots[d]'
  // holds the single surviving tree of degree d, if any. Feeding the roots in
  // ascending degree keeps the carry local. Every slots operation below stays
  // within the reserved capacity, so this loop cannot throw.
  NodePtr              cursor = std::move(merged);
  while (cursor) {
    NodePtr node = std::move(cursor);
    cursor       = std::move(node->sibling); // detach from the root list
    node->parent = nullptr;

    auto degree = static_cast<std::size_t>(node->degree);
    while (degree < slots.size() && slots[degree]) {
      NodePtr other = std::move(slots[degree]);
      // The higher-priority root becomes the parent (comp(a,b) => a ranks below b).
      if (comp_(node->value, other->value)) {
        link(*other, std::move(node));
        node = std::move(other);
      } else {
        link(*node, std::move(other));
      }
      degree = static_cast<std::size_t>(node->degree);
    }
    if (degree >= slots.size()) {
      slots.resize(degree + 1);
    }
    slots[degree] = std::move(node);
  }

  // Rebuild the degree-ascending root list from the occupied slots.
  NodePtr new_head;
  Node*   tail = nullptr;
  for (NodePtr& slot : slots) {
    if (!slot) {
      continue;
    }
    Node* raw = slot.get();
    if (tail == nullptr) {
      new_head = std::move(slot);
    } else {
      tail->sibling = std::move(slot);
    }
    tail = raw;
  }
  head_ = std::move(new_head);
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::find_top_root() const -> Node* {
  Node* best = head_.get();
  for (Node* cur = best ? best->sibling.get() : nullptr; cur != nullptr; cur = cur->sibling.get()) {
    // Keep the root that is not outranked by any other (comp(best,cur) => swap).
    if (comp_(best->value, cur->value)) {
      best = cur;
    }
  }
  return best;
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::extract_root(Node* root) -> NodePtr {
  // Detach 'root' from the root list, keeping ownership in 'detached'.
  NodePtr detached;
  if (head_.get() == root) {
    detached = std::move(head_);
    head_    = std::move(detached->sibling);
  } else {
    Node* prev = head_.get();
    while (prev->sibling.get() != root) {
      prev = prev->sibling.get();
    }
    detached      = std::move(prev->sibling);
    prev->sibling = std::move(detached->sibling);
  }
  detached->sibling = nullptr;

  // Reverse the detached root's children into a degree-ascending root list and
  // meld it back into the forest.
  NodePtr children = std::move(detached->child);
  NodePtr reversed;
  while (children) {
    NodePtr child  = std::move(children);
    children       = std::move(child->sibling);
    child->parent  = nullptr;
    child->sibling = std::move(reversed);
    reversed       = std::move(child);
  }
  // The children were already counted in size_, so no extra headroom is needed.
  union_with(std::move(reversed), 0);

  return detached;
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::sift_up(Node* node, const Compare& comp, bool to_root) -> Node* {
  Node* cur = node;
  while (cur->parent != nullptr && (to_root || comp(cur->parent->value, cur->value))) {
    swap_payload(*cur->parent, *cur);
    cur = cur->parent;
  }
  return cur;
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::swap_payload(Node& a, Node& b) -> void {
  using std::swap;
  swap(a.value, b.value);
  swap(a.handle, b.handle);
  // Each handle record must keep pointing at the node that now holds its element.
  if (a.handle) {
    a.handle->node = &a;
  }
  if (b.handle) {
    b.handle->node = &b;
  }
}

template <HeapValue T, typename Compare>
auto BinomialHeap<T, Compare>::validate_non_empty(const char* operation) const -> void {
  if (is_empty()) {
    throw HeapException(std::string("BinomialHeap::") + operation + ": heap is empty");
  }
}

} // namespace ads::heaps

//===---------------------------------------------------------------------------===//
