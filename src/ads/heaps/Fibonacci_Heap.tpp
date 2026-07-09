//===---------------------------------------------------------------------------===//
/**
 * @file Fibonacci_Heap.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the FibonacciHeap class.
 * @version 0.1
 * @date 2026-07-09
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/heaps/Fibonacci_Heap.hpp"

#include <string>
#include <vector>

namespace ads::heaps {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <HeapValue T, typename Compare>
FibonacciHeap<T, Compare>::FibonacciHeap(Compare comp) noexcept(std::is_nothrow_move_constructible_v<Compare>) : comp_(std::move(comp)) {
}

template <HeapValue T, typename Compare>
FibonacciHeap<T, Compare>::FibonacciHeap(std::initializer_list<T> values, Compare comp) requires CopyHeapValue<T>
    : comp_(std::move(comp)) {
  for (const T& value : values) {
    insert(value);
  }
}

template <HeapValue T, typename Compare>
template <std::input_iterator InputIt>
FibonacciHeap<T, Compare>::FibonacciHeap(InputIt first, InputIt last, Compare comp) requires HeapRangeValue<InputIt, T>
    : comp_(std::move(comp)) {
  for (; first != last; ++first) {
    insert(*first);
  }
}

template <HeapValue T, typename Compare>
FibonacciHeap<T, Compare>::FibonacciHeap(const std::vector<T>& elements, Compare comp) requires CopyHeapValue<T>
    : comp_(std::move(comp)) {
  for (const T& value : elements) {
    insert(value);
  }
}

template <HeapValue T, typename Compare>
FibonacciHeap<T, Compare>::FibonacciHeap(FibonacciHeap&& other) noexcept :
    pool_(std::move(other.pool_)),
    top_(other.top_),
    size_(other.size_),
    comp_(std::move(other.comp_)) {
  other.top_  = nullptr;
  other.size_ = 0;
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::operator=(FibonacciHeap&& other) noexcept -> FibonacciHeap& {
  if (this != &other) {
    clear();
    pool_       = std::move(other.pool_);
    top_        = other.top_;
    size_       = other.size_;
    comp_       = std::move(other.comp_);
    other.top_  = nullptr;
    other.size_ = 0;
  }
  return *this;
}

template <HeapValue T, typename Compare>
FibonacciHeap<T, Compare>::~FibonacciHeap() {
  clear();
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::insert(const T& value) -> void requires CopyHeapValue<T>
{
  (void)emplace(value);
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::insert(T&& value) -> void requires MoveHeapValue<T>
{
  (void)emplace(std::move(value));
}

template <HeapValue T, typename Compare>
template <typename... Args>
auto FibonacciHeap<T, Compare>::emplace(Args&&... args) -> Handle requires EmplaceHeapValue<T, Args...>
{
  Node* node = arena_alloc(std::forward<Args>(args)...);
  make_single(node);
  add_to_root_list(node);
  ++size_;
  return Handle(node);
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::top() const -> const T& {
  validate_non_empty("top");
  return top_->value;
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::extract_top() -> T {
  validate_non_empty("extract_top");
  Node* z      = top_;
  T     result = std::move(z->value);

  // Promote z's children to roots.
  if (z->child != nullptr) {
    Node* c = z->child;
    do {
      c->parent = nullptr;
      c         = c->right;
    } while (c != z->child);
    concat_lists(z, z->child);
  }

  Node* next = z->right;
  unlink(z);
  if (z == next) {
    top_ = nullptr; // z was the only node
  } else {
    top_ = next; // provisional; consolidate finds the real top
    consolidate();
  }
  arena_free(z);
  --size_;
  return result;
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::erase(const Handle& handle) -> void {
  if (!handle.valid()) {
    throw HeapException("FibonacciHeap::erase: invalid handle");
  }
  Node* node   = handle.node_;
  Node* parent = node->parent;
  if (parent != nullptr) {
    cut(node, parent);
    cascading_cut(parent);
  }
  // Force the node to the top, then remove it with the standard extract path.
  top_ = node;
  (void)extract_top();
}

//===----- UPDATE OPERATIONS ---------------------------------------------------===//

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::decrease_key(const Handle& handle, const T& new_value) -> void requires CopyHeapValue<T>
{
  if (!handle.valid()) {
    throw HeapException("FibonacciHeap::decrease_key: invalid handle");
  }
  Node* node = handle.node_;
  // The new value may only raise priority: reject anything ranking below current.
  if (comp_(new_value, node->value)) {
    throw HeapException("FibonacciHeap::decrease_key: new value lowers priority");
  }
  node->value = new_value;

  Node* parent = node->parent;
  if (parent != nullptr && comp_(parent->value, node->value)) {
    cut(node, parent);
    cascading_cut(parent);
  }
  if (comp_(top_->value, node->value)) {
    top_ = node;
  }
}

//===----- MELD OPERATIONS -----------------------------------------------------===//

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::merge(FibonacciHeap&& other) -> void {
  if (this == &other) {
    return; // self-merge is a no-op
  }
  if (!detail::compatible_comparators(comp_, other.comp_)) {
    throw HeapException("FibonacciHeap::merge: incompatible comparator objects");
  }
  if (other.top_ != nullptr) {
    if (top_ == nullptr) {
      top_ = other.top_;
    } else {
      concat_lists(top_, other.top_);
      if (comp_(top_->value, other.top_->value)) {
        top_ = other.top_;
      }
    }
    pool_.splice(pool_.end(), other.pool_); // O(1) ownership transfer
    size_ += other.size_;
  }
  other.top_  = nullptr;
  other.size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::is_empty() const noexcept -> bool {
  return top_ == nullptr;
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::size() const noexcept -> size_type {
  return size_;
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::clear() noexcept -> void {
  // The arena owns every node, so clearing it frees the whole forest at once;
  // the circular raw links simply cease to matter.
  pool_.clear();
  top_  = nullptr;
  size_ = 0;
}

//===----- ARENA HELPERS -------------------------------------------------------===//

template <HeapValue T, typename Compare>
template <typename... Args>
auto FibonacciHeap<T, Compare>::arena_alloc(Args&&... args) -> Node* {
  pool_.push_back(std::make_unique<Node>(std::forward<Args>(args)...));
  Node* node = pool_.back().get();
  node->self = std::prev(pool_.end());
  return node;
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::arena_free(Node* node) -> void {
  pool_.erase(node->self);
}

//===----- CIRCULAR-LIST HELPERS -----------------------------------------------===//

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::make_single(Node* node) noexcept -> void {
  node->left  = node;
  node->right = node;
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::insert_after(Node* pos, Node* node) noexcept -> void {
  node->left       = pos;
  node->right      = pos->right;
  pos->right->left = node;
  pos->right       = node;
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::concat_lists(Node* list, Node* other) noexcept -> void {
  Node* list_right  = list->right;
  Node* other_left  = other->left;
  list->right       = other;
  other->left       = list;
  other_left->right = list_right;
  list_right->left  = other_left;
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::unlink(Node* node) noexcept -> void {
  node->left->right = node->right;
  node->right->left = node->left;
}

//===----- STRUCTURAL HELPERS --------------------------------------------------===//

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::add_to_root_list(Node* node) -> void {
  node->parent = nullptr;
  if (top_ == nullptr) {
    make_single(node);
    top_ = node;
  } else {
    insert_after(top_, node);
    if (comp_(top_->value, node->value)) {
      top_ = node;
    }
  }
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::link_child(Node* child, Node* parent) noexcept -> void {
  child->parent = parent;
  child->mark   = false;
  if (parent->child == nullptr) {
    make_single(child);
    parent->child = child;
  } else {
    insert_after(parent->child, child);
  }
  ++parent->degree;
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::consolidate() -> void {
  // Collect the current roots up front: linking rewrites the root list, so we
  // must not walk it live.
  std::vector<Node*> roots;
  Node*              start = top_;
  Node*              w     = start;
  do {
    roots.push_back(w);
    w = w->right;
  } while (w != start);

  std::vector<Node*> by_degree; // by_degree[d] == the single surviving root of degree d
  for (Node* root : roots) {
    Node* x = root;
    auto  d = static_cast<std::size_t>(x->degree);
    if (by_degree.size() <= d) {
      by_degree.resize(d + 1, nullptr);
    }
    while (by_degree[d] != nullptr) {
      Node* y = by_degree[d];
      if (comp_(x->value, y->value)) {
        std::swap(x, y); // keep the higher-priority root as the parent
      }
      link_child(y, x);
      by_degree[d] = nullptr;
      ++d;
      if (by_degree.size() <= d) {
        by_degree.resize(d + 1, nullptr);
      }
    }
    by_degree[d] = x;
  }

  // Rebuild the root list from the survivors and locate the new top.
  top_ = nullptr;
  for (Node* node : by_degree) {
    if (node == nullptr) {
      continue;
    }
    make_single(node);
    node->parent = nullptr;
    if (top_ == nullptr) {
      top_ = node;
    } else {
      insert_after(top_, node);
      if (comp_(top_->value, node->value)) {
        top_ = node;
      }
    }
  }
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::cut(Node* node, Node* parent) -> void {
  // Detach 'node' from parent's child circle.
  if (node->right == node) {
    parent->child = nullptr; // node was the only child
  } else {
    if (parent->child == node) {
      parent->child = node->right;
    }
    unlink(node);
  }
  --parent->degree;

  // Move 'node' to the root list, cleared of its mark.
  make_single(node);
  add_to_root_list(node);
  node->mark = false;
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::cascading_cut(Node* node) -> void {
  // Walk up the ancestor chain iteratively: the first unmarked ancestor gets
  // marked and stops the cascade; marked ancestors are themselves cut.
  Node* parent = node->parent;
  while (parent != nullptr) {
    if (!node->mark) {
      node->mark = true;
      break;
    }
    cut(node, parent);
    node   = parent;
    parent = node->parent;
  }
}

template <HeapValue T, typename Compare>
auto FibonacciHeap<T, Compare>::validate_non_empty(const char* operation) const -> void {
  if (is_empty()) {
    throw HeapException(std::string("FibonacciHeap::") + operation + ": heap is empty");
  }
}

} // namespace ads::heaps

//===---------------------------------------------------------------------------===//
