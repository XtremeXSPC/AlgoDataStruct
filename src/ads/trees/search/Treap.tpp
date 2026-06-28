//===---------------------------------------------------------------------------===//
/**
 * @file Treap.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for Treap.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../../include/ads/trees/search/Treap.hpp"

namespace ads::trees {

//===----- ITERATOR METHODS ----------------------------------------------------===//

template <OrderedTreeElement T, std::totally_ordered Priority>
Treap<T, Priority>::iterator::iterator(const iterator& other) : stack_(other.stack_.begin(), other.stack_.end()), current_(other.current_) {
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::iterator::operator=(const iterator& other) -> iterator& {
  if (this != &other) {
    stack_.assign(other.stack_.begin(), other.stack_.end());
    current_ = other.current_;
  }
  return *this;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
Treap<T, Priority>::iterator::iterator(Node* root) : current_(nullptr) {
  push_left(root);
  if (!stack_.is_empty()) {
    current_ = stack_.back();
    stack_.pop_back();
  }
}

template <OrderedTreeElement T, std::totally_ordered Priority>
void Treap<T, Priority>::iterator::push_left(Node* node) {
  while (node) {
    stack_.push_back(node);
    node = node->left.get();
  }
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::iterator::operator*() const -> reference {
  return current_->data;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::iterator::operator->() const -> pointer {
  return &current_->data;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::iterator::operator++() -> iterator& {
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

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::iterator::operator++(int) -> iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::iterator::operator==(const iterator& other) const -> bool {
  return current_ == other.current_;
}

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <OrderedTreeElement T, std::totally_ordered Priority>
Treap<T, Priority>::Treap(std::uint64_t seed) : root_(nullptr), size_(0), random_state_(seed == 0 ? kDefaultSeed : seed) {
}

template <OrderedTreeElement T, std::totally_ordered Priority>
Treap<T, Priority>::Treap(Treap&& other) noexcept : root_(std::move(other.root_)), size_(other.size_), random_state_(other.random_state_) {
  other.size_ = 0;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
Treap<T, Priority>::~Treap() {
  clear();
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::operator=(Treap&& other) noexcept -> Treap& {
  if (this != &other) {
    clear();
    root_         = std::move(other.root_);
    size_         = other.size_;
    random_state_ = other.random_state_;
    other.size_   = 0;
  }
  return *this;
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::insert(const T& value) -> bool requires std::copy_constructible<T>
{
  return insert_with_priority(value, next_priority());
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::insert(T&& value) -> bool {
  return insert_with_priority(std::move(value), next_priority());
}

template <OrderedTreeElement T, std::totally_ordered Priority>
template <typename... Args>
auto Treap<T, Priority>::emplace(Args&&... args) -> bool {
  return insert_with_priority(T(std::forward<Args>(args)...), next_priority());
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::insert_with_priority(const T& value, const Priority& priority) -> bool requires std::copy_constructible<T>
{
  return insert_with_priority_impl(root_, value, priority);
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::insert_with_priority(T&& value, const Priority& priority) -> bool {
  return insert_with_priority_impl(root_, std::move(value), priority);
}

template <OrderedTreeElement T, std::totally_ordered Priority>
template <typename... Args>
auto Treap<T, Priority>::emplace_with_priority(const Priority& priority, Args&&... args) -> bool {
  return insert_with_priority_impl(root_, T(std::forward<Args>(args)...), priority);
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::remove(const T& value) -> bool {
  return remove_impl(root_, value);
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::clear() noexcept -> void {
  // Iteratively dismantle the tree to avoid deep recursive destruction paths.
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
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::size() const noexcept -> size_t {
  return size_;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::height() const noexcept -> int {
  return height_helper(root_.get());
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::contains(const T& value) const -> bool {
  return find_node(root_.get(), value) != nullptr;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::find_min() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("Cannot find minimum in empty treap");
  }
  return find_min_node(root_.get())->data;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::find_max() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("Cannot find maximum in empty treap");
  }
  return find_max_node(root_.get())->data;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::priority_of(const T& value) const -> const Priority* {
  Node* node = find_node(root_.get(), value);
  return node ? &node->priority : nullptr;
}

//===----- TRAVERSAL METHODS ---------------------------------------------------===//

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::in_order_traversal(const visitor_type& visit) const -> void {
  in_order_helper(root_.get(), visit);
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::pre_order_traversal(const visitor_type& visit) const -> void {
  pre_order_helper(root_.get(), visit);
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::post_order_traversal(const visitor_type& visit) const -> void {
  post_order_helper(root_.get(), visit);
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::level_order_traversal(const visitor_type& visit) const -> void {
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

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::successor(const T& value) const -> const T* {
  Node* current   = root_.get();
  Node* successor = nullptr;

  while (current) {
    if (value < current->data) {
      successor = current;
      current   = current->left.get();
    } else {
      current = current->right.get();
    }
  }

  return successor ? &successor->data : nullptr;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::predecessor(const T& value) const -> const T* {
  Node* current     = root_.get();
  Node* predecessor = nullptr;

  while (current) {
    if (current->data < value) {
      predecessor = current;
      current     = current->right.get();
    } else {
      current = current->left.get();
    }
  }

  return predecessor ? &predecessor->data : nullptr;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::validate_properties() const -> bool {
  if (!root_) {
    return size_ == 0;
  }

  struct ValidationFrame {
    const Node*     node;
    const T*        lower_bound;
    const T*        upper_bound;
    const Priority* parent_priority;
  };

  size_t                                     counted = 0;
  DynamicArray<ValidationFrame> stack;
  stack.push_back({root_.get(), nullptr, nullptr, nullptr});

  while (!stack.is_empty()) {
    const ValidationFrame frame = stack.back();
    stack.pop_back();

    if (frame.lower_bound != nullptr && !(*frame.lower_bound < frame.node->data)) {
      return false;
    }
    if (frame.upper_bound != nullptr && !(frame.node->data < *frame.upper_bound)) {
      return false;
    }
    if (frame.parent_priority != nullptr && frame.node->priority > *frame.parent_priority) {
      return false;
    }

    ++counted;

    if (frame.node->right) {
      stack.push_back({frame.node->right.get(), &frame.node->data, frame.upper_bound, &frame.node->priority});
    }
    if (frame.node->left) {
      stack.push_back({frame.node->left.get(), frame.lower_bound, &frame.node->data, &frame.node->priority});
    }
  }

  return counted == size_;
}

//===----- ITERATOR ACCESS -----------------------------------------------------===//

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::begin() -> iterator {
  return iterator(root_.get());
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::begin() const -> iterator {
  return iterator(root_.get());
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::end() -> iterator {
  return iterator();
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::end() const -> iterator {
  return iterator();
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::cbegin() const -> iterator {
  return begin();
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::cend() const -> iterator {
  return end();
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <OrderedTreeElement T, std::totally_ordered Priority>
template <typename U>
auto Treap<T, Priority>::insert_with_priority_impl(std::unique_ptr<Node>& node, U&& value, const Priority& priority) -> bool {
  if (!node) {
    node = std::make_unique<Node>(std::forward<U>(value), priority);
    ++size_;
    return true;
  }

  if (value < node->data) {
    const bool inserted = insert_with_priority_impl(node->left, std::forward<U>(value), priority);
    if (inserted && node->left && node->left->priority > node->priority) {
      rotate_right(node);
    }
    return inserted;
  }

  if (node->data < value) {
    const bool inserted = insert_with_priority_impl(node->right, std::forward<U>(value), priority);
    if (inserted && node->right && node->right->priority > node->priority) {
      rotate_left(node);
    }
    return inserted;
  }

  return false;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::remove_impl(std::unique_ptr<Node>& node, const T& value) -> bool {
  if (!node) {
    return false;
  }

  if (value < node->data) {
    return remove_impl(node->left, value);
  }
  if (node->data < value) {
    return remove_impl(node->right, value);
  }

  if (node->left && node->right) {
    // Rotate the higher-priority child upward so the target value keeps moving
    // down toward a position where it can be removed with a single splice.
    if (node->left->priority > node->right->priority) {
      rotate_right(node);
      return remove_impl(node->right, value);
    }

    rotate_left(node);
    return remove_impl(node->left, value);
  }

  std::unique_ptr<Node> replacement;
  if (node->left) {
    replacement = std::move(node->left);
  } else {
    replacement = std::move(node->right);
  }

  node.reset();
  node = std::move(replacement);
  --size_;
  return true;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::rotate_left(std::unique_ptr<Node>& node) -> void {
  auto new_root  = std::move(node->right);
  node->right    = std::move(new_root->left);
  new_root->left = std::move(node);
  node           = std::move(new_root);
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::rotate_right(std::unique_ptr<Node>& node) -> void {
  auto new_root   = std::move(node->left);
  node->left      = std::move(new_root->right);
  new_root->right = std::move(node);
  node            = std::move(new_root);
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::next_priority() -> Priority {
  random_state_ ^= random_state_ << 7;
  random_state_ ^= random_state_ >> 9;
  random_state_ ^= random_state_ << 8;
  return static_cast<Priority>(random_state_);
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::find_node(Node* node, const T& value) const -> Node* {
  while (node) {
    if (value < node->data) {
      node = node->left.get();
    } else if (node->data < value) {
      node = node->right.get();
    } else {
      return node;
    }
  }
  return nullptr;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::find_min_node(Node* node) const -> Node* {
  while (node->left) {
    node = node->left.get();
  }
  return node;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::find_max_node(Node* node) const -> Node* {
  while (node->right) {
    node = node->right.get();
  }
  return node;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::height_helper(const Node* node) const noexcept -> int {
  if (!node) {
    return -1;
  }

  struct HeightFrame {
    const Node* node;
    int         depth;
  };

  int                                    max_depth = -1;
  DynamicArray<HeightFrame> stack;
  stack.push_back({node, 0});

  while (!stack.is_empty()) {
    const HeightFrame frame = stack.back();
    stack.pop_back();

    max_depth = std::max(max_depth, frame.depth);
    if (frame.node->left) {
      stack.push_back({frame.node->left.get(), frame.depth + 1});
    }
    if (frame.node->right) {
      stack.push_back({frame.node->right.get(), frame.depth + 1});
    }
  }

  return max_depth;
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::in_order_helper(const Node* node, const visitor_type& visit) const -> void {
  DynamicArray<const Node*> stack;
  const Node*                            current = node;

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

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::pre_order_helper(const Node* node, const visitor_type& visit) const -> void {
  if (!node) {
    return;
  }

  DynamicArray<const Node*> stack;
  stack.push_back(node);

  while (!stack.is_empty()) {
    const Node* current = stack.back();
    stack.pop_back();
    visit(current->data);

    if (current->right) {
      stack.push_back(current->right.get());
    }
    if (current->left) {
      stack.push_back(current->left.get());
    }
  }
}

template <OrderedTreeElement T, std::totally_ordered Priority>
auto Treap<T, Priority>::post_order_helper(const Node* node, const visitor_type& visit) const -> void {
  if (!node) {
    return;
  }

  struct PostOrderFrame {
    const Node* node;
    bool        visited;
  };

  DynamicArray<PostOrderFrame> stack;
  stack.push_back({node, false});

  while (!stack.is_empty()) {
    const PostOrderFrame frame = stack.back();
    stack.pop_back();

    if (frame.visited) {
      visit(frame.node->data);
      continue;
    }

    stack.push_back({frame.node, true});
    if (frame.node->right) {
      stack.push_back({frame.node->right.get(), false});
    }
    if (frame.node->left) {
      stack.push_back({frame.node->left.get(), false});
    }
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
