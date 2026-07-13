//===---------------------------------------------------------------------------===//
/**
 * @file Cartesian_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for CartesianTree.
 * @version 0.1
 * @date 2026-07-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../../include/ads/trees/search/Cartesian_Tree.hpp"

#include <limits>

namespace ads::trees {

//===----- ITERATOR METHODS ----------------------------------------------------===//

template <OrderedTreeElement T, typename Compare>
CartesianTree<T, Compare>::iterator::iterator(const iterator& other) :
    stack_(other.stack_.begin(), other.stack_.end()),
    current_(other.current_) {
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::iterator::operator=(const iterator& other) -> iterator& {
  if (this != &other) {
    stack_.assign(other.stack_.begin(), other.stack_.end());
    current_ = other.current_;
  }
  return *this;
}

template <OrderedTreeElement T, typename Compare>
CartesianTree<T, Compare>::iterator::iterator(Node* root) : current_(nullptr) {
  push_left(root);
  if (!stack_.is_empty()) {
    current_ = stack_.back();
    stack_.pop_back();
  }
}

template <OrderedTreeElement T, typename Compare>
void CartesianTree<T, Compare>::iterator::push_left(Node* node) {
  while (node) {
    stack_.push_back(node);
    node = node->left.get();
  }
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::iterator::operator*() const -> reference {
  return current_->data;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::iterator::operator->() const -> pointer {
  return &current_->data;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::iterator::operator++() -> iterator& {
  if (current_->right) {
    push_left(current_->right.get());
  }

  if (!stack_.is_empty()) {
    current_ = stack_.back();
    stack_.pop_back();
  } else {
    current_ = nullptr;
  }

  return *this;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::iterator::operator++(int) -> iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::iterator::operator==(const iterator& other) const -> bool {
  return current_ == other.current_;
}

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <OrderedTreeElement T, typename Compare>
CartesianTree<T, Compare>::CartesianTree(Compare compare) : comp_(std::move(compare)), root_(nullptr), size_(0) {
}

template <OrderedTreeElement T, typename Compare>
CartesianTree<T, Compare>::CartesianTree(const DynamicArray<T>& sequence, Compare compare) requires std::copy_constructible<T>
    : comp_(std::move(compare)), root_(nullptr), size_(0) {
  try {
    spine_.reserve(sequence.size());
    for (size_type i = 0; i < sequence.size(); ++i) {
      append(sequence[i]);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <OrderedTreeElement T, typename Compare>
template <std::input_iterator InputIt>
requires std::constructible_from<T, std::iter_reference_t<InputIt>>
CartesianTree<T, Compare>::CartesianTree(InputIt first, InputIt last, Compare compare) :
    comp_(std::move(compare)),
    root_(nullptr),
    size_(0),
    spine_() {
  try {
    for (; first != last; ++first) {
      append(*first);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <OrderedTreeElement T, typename Compare>
CartesianTree<T, Compare>::CartesianTree(CartesianTree&& other) noexcept(std::is_nothrow_move_constructible_v<Compare>) :
    comp_(std::move(other.comp_)),
    root_(std::move(other.root_)),
    size_(other.size_),
    spine_(std::move(other.spine_)) {
  other.size_ = 0;
}

template <OrderedTreeElement T, typename Compare>
CartesianTree<T, Compare>::~CartesianTree() {
  clear();
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::operator=(CartesianTree&& other) noexcept
    -> CartesianTree& requires std::is_nothrow_move_assignable_v<Compare>
{
  if (this != &other) {
    comp_ = std::move(other.comp_);
    clear();
    root_       = std::move(other.root_);
    size_       = other.size_;
    spine_      = std::move(other.spine_);
    other.size_ = 0;
  }
  return *this;
}

//===----- MUTATION OPERATIONS -------------------------------------------------===//

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::push_back(const T& value) -> void requires std::copy_constructible<T>
{
  append(value);
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::push_back(T&& value) -> void {
  append(std::move(value));
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::clear() noexcept -> void {
  // Iteratively dismantle the tree to avoid deep recursive destruction paths: a
  // Cartesian tree built from a monotone sequence is a chain of length size_.
  while (root_) {
    if (root_->left) {
      auto old_root  = std::move(root_);
      root_          = std::move(old_root->left);
      old_root->left = std::move(root_->right);
      root_->right   = std::move(old_root);
    } else {
      auto next = std::move(root_->right);
      root_.reset();
      root_ = std::move(next);
    }
  }
  spine_.clear();
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::size() const noexcept -> size_type {
  return size_;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::height() const -> int {
  // Iterative depth-first walk so a linear tree cannot overflow the call stack.
  if (!root_) {
    return -1;
  }

  int                 best = 0;
  DynamicArray<Node*> nodes;
  DynamicArray<int>   depths;
  nodes.push_back(root_.get());
  depths.push_back(0);

  while (!nodes.is_empty()) {
    Node*     node  = nodes.back();
    const int depth = depths.back();
    nodes.pop_back();
    depths.pop_back();

    if (depth > best) {
      best = depth;
    }
    if (node->left) {
      nodes.push_back(node->left.get());
      depths.push_back(depth + 1);
    }
    if (node->right) {
      nodes.push_back(node->right.get());
      depths.push_back(depth + 1);
    }
  }
  return best;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::extremum() const -> const T& {
  if (!root_) {
    throw EmptyTreeException("Cannot read extremum of empty Cartesian tree");
  }
  return root_->data;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::value_at(size_type index) const -> const T& {
  validate_index(index);
  return node_at(index)->data;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::range_extremum_index(size_type left, size_type right) const -> size_type {
  validate_range(left, right);
  return range_node(left, right)->index;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::range_extremum(size_type left, size_type right) const -> const T& {
  validate_range(left, right);
  return range_node(left, right)->data;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::validate_properties() const -> bool {
  if (!root_) {
    return size_ == 0 && spine_.is_empty();
  }

  // Single iterative in-order pass verifies three things at once: the visited
  // count equals size_, positions form the strictly increasing sequence
  // 0,1,...,size_-1, and every parent outranks (or ties) each child under comp_.
  DynamicArray<Node*> stack;
  Node*               current   = root_.get();
  bool                have_prev = false;
  size_type           previous  = 0;
  size_type           count     = 0;

  while (current || !stack.is_empty()) {
    while (current) {
      stack.push_back(current);
      current = current->left.get();
    }
    current = stack.back();
    stack.pop_back();

    if (have_prev && current->index != previous + 1) {
      return false;
    }
    if (!have_prev && current->index != 0) {
      return false;
    }
    // Heap order: neither child may outrank its parent.
    if (current->left && comp_(current->left->data, current->data)) {
      return false;
    }
    if (current->right && comp_(current->right->data, current->data)) {
      return false;
    }

    previous  = current->index;
    have_prev = true;
    ++count;
    current = current->right.get();
  }
  if (count != size_) {
    return false;
  }

  size_type spine_index = 0;
  for (Node* node = root_.get(); node; node = node->right.get()) {
    if (spine_index >= spine_.size() || spine_[spine_index] != node) {
      return false;
    }
    ++spine_index;
  }
  return spine_index == spine_.size();
}

//===----- TRAVERSAL METHODS ---------------------------------------------------===//

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::in_order_traversal(const visitor_type& visit) const -> void {
  DynamicArray<Node*> stack;
  Node*               current = root_.get();

  while (current || !stack.is_empty()) {
    while (current) {
      stack.push_back(current);
      current = current->left.get();
    }
    current = stack.back();
    stack.pop_back();
    visit(current->data);
    current = current->right.get();
  }
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::pre_order_traversal(const visitor_type& visit) const -> void {
  if (!root_) {
    return;
  }

  DynamicArray<Node*> stack;
  stack.push_back(root_.get());
  while (!stack.is_empty()) {
    Node* node = stack.back();
    stack.pop_back();
    visit(node->data);
    // Push right first so the left child is visited next.
    if (node->right) {
      stack.push_back(node->right.get());
    }
    if (node->left) {
      stack.push_back(node->left.get());
    }
  }
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::post_order_traversal(const visitor_type& visit) const -> void {
  if (!root_) {
    return;
  }

  // Emit a root-right-left order into a buffer, then replay it reversed to obtain
  // left-right-root without recursion or per-node visited flags.
  DynamicArray<Node*> stack;
  DynamicArray<Node*> output;
  stack.push_back(root_.get());
  while (!stack.is_empty()) {
    Node* node = stack.back();
    stack.pop_back();
    output.push_back(node);
    if (node->left) {
      stack.push_back(node->left.get());
    }
    if (node->right) {
      stack.push_back(node->right.get());
    }
  }
  for (size_type i = output.size(); i > 0; --i) {
    visit(output[i - 1]->data);
  }
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::level_order_traversal(const visitor_type& visit) const -> void {
  if (!root_) {
    return;
  }

  ads::queues::LinkedQueue<const Node*> queue;
  queue.enqueue(root_.get());

  while (!queue.is_empty()) {
    const Node* current = queue.front();
    queue.dequeue();
    visit(current->data);

    if (current->left) {
      queue.enqueue(current->left.get());
    }
    if (current->right) {
      queue.enqueue(current->right.get());
    }
  }
}

//===----- ITERATOR ACCESS -----------------------------------------------------===//

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::begin() -> iterator {
  return iterator(root_.get());
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::begin() const -> iterator {
  return iterator(root_.get());
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::end() -> iterator {
  return iterator();
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::end() const -> iterator {
  return iterator();
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::cbegin() const -> iterator {
  return iterator(root_.get());
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::cend() const -> iterator {
  return iterator();
}

//===----- PRIVATE HELPERS -----------------------------------------------------===//

template <OrderedTreeElement T, typename Compare>
template <typename U>
auto CartesianTree<T, Compare>::append(U&& value) -> void {
  if (size_ == std::numeric_limits<size_type>::max()) {
    throw BinaryTreeException("CartesianTree size overflow");
  }

  // Finish every potentially throwing operation before changing either the tree
  // links or the cached right spine. Grow geometrically to preserve amortized O(1)
  // append even when the right spine contains every node.
  if (spine_.size() == spine_.capacity()) {
    const size_type current_capacity = spine_.capacity();
    if (current_capacity == std::numeric_limits<size_type>::max()) {
      throw BinaryTreeException("CartesianTree spine capacity overflow");
    }
    size_type new_capacity = 1;
    if (current_capacity > std::numeric_limits<size_type>::max() / 2) {
      new_capacity = std::numeric_limits<size_type>::max();
    } else if (current_capacity > 0) {
      new_capacity = current_capacity * 2;
    }
    spine_.reserve(new_capacity);
  }
  auto  new_node = std::make_unique<Node>(std::forward<U>(value), size_);
  Node* raw      = new_node.get();

  size_type parent_position = spine_.size();
  while (parent_position > 0 && comp_(raw->data, spine_[parent_position - 1]->data)) {
    --parent_position;
  }

  if (parent_position > 0) {
    Node* parent   = spine_[parent_position - 1];
    new_node->left = std::move(parent->right); // detach the popped chain (may be null)
    parent->right  = std::move(new_node);
  } else {
    new_node->left = std::move(root_); // the whole previous tree drops under the newcomer
    root_          = std::move(new_node);
  }

  spine_.resize(parent_position);
  spine_.push_back(raw);
  ++size_;
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::range_node(size_type left, size_type right) const -> Node* {
  // The extremum of [left, right] is the lowest common ancestor of positions left
  // and right: descend from the root until the current position lands inside the
  // range, steering left when the whole range is smaller and right when larger.
  Node* node = root_.get();
  while (node) {
    if (right < node->index) {
      node = node->left.get();
    } else if (left > node->index) {
      node = node->right.get();
    } else {
      return node;
    }
  }
  // Unreachable while the invariants hold and the range is in bounds.
  throw BinaryTreeException("CartesianTree range query reached a null node");
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::node_at(size_type index) const -> Node* {
  // In-order position behaves like a BST key over node indices.
  Node* node = root_.get();
  while (node) {
    if (index == node->index) {
      return node;
    }
    node = index < node->index ? node->left.get() : node->right.get();
  }
  // Unreachable after validate_index for a well-formed tree.
  throw BinaryTreeException("CartesianTree position not found");
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::validate_index(size_type index) const -> void {
  if (index >= size_) {
    throw BinaryTreeException("CartesianTree index out of range");
  }
}

template <OrderedTreeElement T, typename Compare>
auto CartesianTree<T, Compare>::validate_range(size_type left, size_type right) const -> void {
  if (left > right) {
    throw BinaryTreeException("CartesianTree invalid range: left > right");
  }
  if (right >= size_) {
    throw BinaryTreeException("CartesianTree range out of bounds");
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
