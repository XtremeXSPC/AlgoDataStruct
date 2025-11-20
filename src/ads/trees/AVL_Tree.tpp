//===--------------------------------------------------------------------------===//
/**
 * @file AVL_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the AVL Tree class.
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/trees/AVL_Tree.hpp"

using ads::trees::AVLTree;

//============================================================================//
// CONSTRUCTORS AND ASSIGNMENT
//============================================================================//

template <typename T>
AVLTree<T>::AVLTree() : root_(nullptr), size_(0) {
}

template <typename T>
AVLTree<T>::AVLTree(AVLTree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) {
  other.size_ = 0;
}

template <typename T>
AVLTree<T>& AVLTree<T>::operator=(AVLTree&& other) noexcept {
  if (this != &other) {
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//============================================================================//
// HEIGHT MANAGEMENT
//============================================================================//

template <typename T>
auto AVLTree<T>::get_height(const Node* node) const noexcept -> int {
  return node ? node->height : 0;
}

template <typename T>
void AVLTree<T>::update_height(Node* node) noexcept {
  if (node) {
    node->height = 1 + std::max(get_height(node->left.get()), get_height(node->right.get()));
  }
}

template <typename T>
auto AVLTree<T>::get_balance_factor(const Node* node) const noexcept -> int {
  return node ? get_height(node->left.get()) - get_height(node->right.get()) : 0;
}

//============================================================================//
// ROTATION OPERATIONS
//============================================================================//

template <typename T>
auto AVLTree<T>::rotate_right(std::unique_ptr<Node> y) -> std::unique_ptr<Node> {
  // Save x (left child of y)
  auto x = std::move(y->left);

  // Move B subtree from x to y
  y->left = std::move(x->right);

  // Update heights
  update_height(y.get());
  update_height(x.get());

  // Make y the right child of x
  x->right = std::move(y);

  return x;
}

template <typename T>
auto AVLTree<T>::rotate_left(std::unique_ptr<Node> x) -> std::unique_ptr<Node> {
  // Save y (right child of x)
  auto y = std::move(x->right);

  // Move B subtree from y to x
  x->right = std::move(y->left);

  // Update heights
  update_height(x.get());
  update_height(y.get());

  // Make x the left child of y
  y->left = std::move(x);

  return y;
}

template <typename T>
auto AVLTree<T>::rotate_left_right(std::unique_ptr<Node> node) -> std::unique_ptr<Node> {
  // First: rotate left on left child
  node->left = rotate_left(std::move(node->left));
  // Second: rotate right on node
  return rotate_right(std::move(node));
}

template <typename T>
auto AVLTree<T>::rotate_right_left(std::unique_ptr<Node> node) -> std::unique_ptr<Node> {
  // First: rotate right on right child
  node->right = rotate_right(std::move(node->right));
  // Second: rotate left on node
  return rotate_left(std::move(node));
}

//============================================================================//
// BALANCING
//============================================================================//

template <typename T>
auto AVLTree<T>::balance(std::unique_ptr<Node> node) -> std::unique_ptr<Node> {
  if (!node) {
    return node;
  }

  // Update height of current node
  update_height(node.get());

  // Get balance factor
  int balance = get_balance_factor(node.get());

  // Left-Left case (LL)
  if (balance > 1 && get_balance_factor(node->left.get()) >= 0) {
    return rotate_right(std::move(node));
  }

  // Left-Right case (LR)
  if (balance > 1 && get_balance_factor(node->left.get()) < 0) {
    return rotate_left_right(std::move(node));
  }

  // Right-Right case (RR)
  if (balance < -1 && get_balance_factor(node->right.get()) <= 0) {
    return rotate_left(std::move(node));
  }

  // Right-Left case (RL)
  if (balance < -1 && get_balance_factor(node->right.get()) > 0) {
    return rotate_right_left(std::move(node));
  }

  // Node is balanced
  return node;
}

//============================================================================//
// INSERT OPERATIONS
//============================================================================//

template <typename T>
auto AVLTree<T>::insert(const T& value) -> bool {
  bool inserted = false;
  root_         = insert_helper(std::move(root_), value, inserted);
  if (inserted) {
    ++size_;
  }
  return inserted;
}

template <typename T>
auto AVLTree<T>::insert(T&& value) -> bool {
  bool inserted = false;
  root_         = insert_helper(std::move(root_), std::move(value), inserted);
  if (inserted) {
    ++size_;
  }
  return inserted;
}

template <typename T>
template <typename... Args>
auto AVLTree<T>::emplace(Args&&... args) -> bool {
  return insert(T(std::forward<Args>(args)...));
}

template <typename T>
template <typename U>
auto AVLTree<T>::insert_helper(std::unique_ptr<Node> node, U&& value, bool& inserted) -> std::unique_ptr<Node> {
  // Base case: create new node
  if (!node) {
    inserted = true;
    return std::make_unique<Node>(std::forward<U>(value));
  }

  // Recursive BST insertion
  if (value < node->data) {
    node->left = insert_helper(std::move(node->left), std::forward<U>(value), inserted);
  } else if (value > node->data) {
    node->right = insert_helper(std::move(node->right), std::forward<U>(value), inserted);
  } else {
    // Duplicate value, don't insert
    inserted = false;
    return node;
  }

  // Balance the node after insertion
  return balance(std::move(node));
}

//============================================================================//
// REMOVE OPERATIONS
//============================================================================//

template <typename T>
auto AVLTree<T>::remove(const T& value) -> bool {
  bool removed = false;
  root_        = remove_helper(std::move(root_), value, removed);
  if (removed) {
    --size_;
  }
  return removed;
}

template <typename T>
auto AVLTree<T>::remove_helper(std::unique_ptr<Node> node, const T& value, bool& removed) -> std::unique_ptr<Node> {
  // Base case: value not found
  if (!node) {
    removed = false;
    return nullptr;
  }

  // Recursive search for node to remove
  if (value < node->data) {
    node->left = remove_helper(std::move(node->left), value, removed);
  } else if (value > node->data) {
    node->right = remove_helper(std::move(node->right), value, removed);
  } else {
    // Found the node to remove
    removed = true;

    // Case 1: Node with only right child or no children
    if (!node->left) {
      return std::move(node->right);
    }

    // Case 2: Node with only left child
    if (!node->right) {
      return std::move(node->left);
    }

    // Case 3: Node with two children
    // Find the in-order successor (minimum in right subtree)
    auto successor = detach_min(node->right);

    // Replace node's data with successor's data
    successor->left  = std::move(node->left);
    successor->right = std::move(node->right);

    node = std::move(successor);
  }

  // Balance the node after removal
  return balance(std::move(node));
}

template <typename T>
auto AVLTree<T>::detach_min(std::unique_ptr<Node>& node) -> std::unique_ptr<Node> {
  if (!node->left) {
    // This is the minimum node
    auto min_node = std::move(node);
    node          = std::move(min_node->right);
    return min_node;
  }

  // Recursively find minimum in left subtree
  auto min_node = detach_min(node->left);

  // Balance after detachment
  node = balance(std::move(node));

  return min_node;
}

//============================================================================//
// SEARCH OPERATIONS
//============================================================================//

template <typename T>
auto AVLTree<T>::contains(const T& value) const -> bool {
  return find_helper(root_.get(), value) != nullptr;
}

template <typename T>
auto AVLTree<T>::find_helper(Node* node, const T& value) const -> Node* {
  if (!node) {
    return nullptr;
  }

  if (value < node->data) {
    return find_helper(node->left.get(), value);
  }

  if (value > node->data) {
    return find_helper(node->right.get(), value);
  }

  return node;
}

template <typename T>
auto AVLTree<T>::find_min() const -> const T& {
  if (!root_) {
    throw EmptyTreeException("Cannot find minimum in empty tree");
  }
  return find_min_node(root_.get())->data;
}

template <typename T>
auto AVLTree<T>::find_max() const -> const T& {
  if (!root_) {
    throw EmptyTreeException("Cannot find maximum in empty tree");
  }
  return find_max_node(root_.get())->data;
}

template <typename T>
auto AVLTree<T>::find_min_node(Node* node) const -> Node* {
  while (node->left) {
    node = node->left.get();
  }
  return node;
}

template <typename T>
auto AVLTree<T>::find_max_node(Node* node) const -> Node* {
  while (node->right) {
    node = node->right.get();
  }
  return node;
}

//============================================================================//
// TREE PROPERTIES
//============================================================================//

template <typename T>
auto AVLTree<T>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <typename T>
auto AVLTree<T>::size() const noexcept -> size_t {
  return size_;
}

template <typename T>
auto AVLTree<T>::height() const noexcept -> int {
  return get_height(root_.get());
}

template <typename T>
void AVLTree<T>::clear() noexcept {
  root_.reset();
  size_ = 0;
}

template <typename T>
auto AVLTree<T>::get_balance() const noexcept -> int {
  return get_balance_factor(root_.get());
}

template <typename T>
auto AVLTree<T>::is_balanced() const noexcept -> bool {
  return is_balanced_helper(root_.get());
}

template <typename T>
auto AVLTree<T>::is_balanced_helper(const Node* node) const noexcept -> bool {
  if (!node) {
    return true;
  }

  int balance = get_balance_factor(node);

  // Check if current node is balanced
  if (balance < -1 || balance > 1) {
    return false;
  }

  // Recursively check left and right subtrees
  return is_balanced_helper(node->left.get()) && is_balanced_helper(node->right.get());
}

//============================================================================//
// TRAVERSAL OPERATIONS
//============================================================================//

template <typename T>
void AVLTree<T>::in_order_traversal(const std::function<void(const T&)>& visit) const {
  in_order_helper(root_.get(), visit);
}

template <typename T>
void AVLTree<T>::pre_order_traversal(const std::function<void(const T&)>& visit) const {
  pre_order_helper(root_.get(), visit);
}

template <typename T>
void AVLTree<T>::post_order_traversal(const std::function<void(const T&)>& visit) const {
  post_order_helper(root_.get(), visit);
}

template <typename T>
void AVLTree<T>::level_order_traversal(const std::function<void(const T&)>& visit) const {
  if (!root_) {
    return;
  }

  std::queue<Node*> q;
  q.push(root_.get());

  while (!q.empty()) {
    Node* current = q.front();
    q.pop();

    visit(current->data);

    if (current->left) {
      q.push(current->left.get());
    }
    if (current->right) {
      q.push(current->right.get());
    }
  }
}

template <typename T>
void AVLTree<T>::in_order_helper(const Node* node, const std::function<void(const T&)>& visit) const {
  if (!node) {
    return;
  }

  in_order_helper(node->left.get(), visit);
  visit(node->data);
  in_order_helper(node->right.get(), visit);
}

template <typename T>
void AVLTree<T>::pre_order_helper(const Node* node, const std::function<void(const T&)>& visit) const {
  if (!node) {
    return;
  }

  visit(node->data);
  pre_order_helper(node->left.get(), visit);
  pre_order_helper(node->right.get(), visit);
}

template <typename T>
void AVLTree<T>::post_order_helper(const Node* node, const std::function<void(const T&)>& visit) const {
  if (!node) {
    return;
  }

  post_order_helper(node->left.get(), visit);
  post_order_helper(node->right.get(), visit);
  visit(node->data);
}

//============================================================================//
// ITERATOR IMPLEMENTATION
//============================================================================//

template <typename T>
AVLTree<T>::iterator::iterator(Node* root) : current_(nullptr) {
  push_left(root);
}

template <typename T>
void AVLTree<T>::iterator::push_left(Node* node) {
  while (node) {
    stack_.push(node);
    node = node->left.get();
  }
  if (!stack_.empty()) {
    current_ = stack_.top();
  }
}

template <typename T>
auto AVLTree<T>::iterator::operator*() const -> reference {
  return current_->data;
}

template <typename T>
auto AVLTree<T>::iterator::operator->() const -> pointer {
  return &(current_->data);
}

template <typename T>
auto AVLTree<T>::iterator::operator++() -> iterator& {
  if (stack_.empty()) {
    current_ = nullptr;
    return *this;
  }

  Node* node = stack_.top();
  stack_.pop();

  if (node->right) {
    push_left(node->right.get());
  }

  if (!stack_.empty()) {
    current_ = stack_.top();
  } else {
    current_ = nullptr;
  }

  return *this;
}

template <typename T>
auto AVLTree<T>::iterator::operator++(int) -> iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <typename T>
auto AVLTree<T>::iterator::operator==(const iterator& other) const -> bool {
  return current_ == other.current_;
}

template <typename T>
auto AVLTree<T>::iterator::operator!=(const iterator& other) const -> bool {
  return !(*this == other);
}

template <typename T>
auto AVLTree<T>::begin() -> iterator {
  return iterator(root_.get());
}

template <typename T>
auto AVLTree<T>::end() -> iterator {
  return iterator();
}

template <typename T>
auto AVLTree<T>::begin() const -> iterator {
  return iterator(root_.get());
}

template <typename T>
auto AVLTree<T>::end() const -> iterator {
  return iterator();
}

template <typename T>
auto AVLTree<T>::cbegin() const -> iterator {
  return iterator(root_.get());
}

template <typename T>
auto AVLTree<T>::cend() const -> iterator {
  return iterator();
}

//===--------------------------------------------------------------------------===//
