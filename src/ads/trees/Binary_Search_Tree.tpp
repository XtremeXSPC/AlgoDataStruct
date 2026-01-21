//===--------------------------------------------------------------------------===//
/**
 * @file Binary_Search_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for BinarySearchTree.
 * @version 0.1
 * @date 2025-10-15
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once
#include "../../../include/ads/trees/Binary_Search_Tree.hpp"

namespace ads::trees {

//===------------------------ Iterator implementation -------------------------===//

template <typename T>
BinarySearchTree<T>::iterator::iterator(Node* root) : current_(nullptr) {
  push_left(root);
  if (!stack_.empty()) {
    current_ = stack_.top();
    stack_.pop();
  }
}

template <typename T>
void BinarySearchTree<T>::iterator::push_left(Node* node) {
  while (node) {
    stack_.push(node);
    node = node->left.get();
  }
}

template <typename T>
auto BinarySearchTree<T>::iterator::operator*() const -> reference {
  return current_->data;
}

template <typename T>
auto BinarySearchTree<T>::iterator::operator->() const -> pointer {
  return &current_->data;
}

template <typename T>
auto BinarySearchTree<T>::iterator::operator++() -> iterator& {
  // If current has a right child, go to it and then all the way left
  if (current_->right) {
    push_left(current_->right.get());
  }

  // Get the next node from the stack
  if (!stack_.empty()) {
    current_ = stack_.top();
    stack_.pop();
  } else {
    current_ = nullptr; // We've reached the end
  }

  return *this;
}

template <typename T>
auto BinarySearchTree<T>::iterator::operator++(int) -> iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <typename T>
auto BinarySearchTree<T>::iterator::operator==(const iterator& other) const -> bool {
  return current_ == other.current_;
}

template <typename T>
auto BinarySearchTree<T>::iterator::operator!=(const iterator& other) const -> bool {
  return current_ != other.current_;
}

//===--------------------- BinarySearchTree implementation --------------------===//

template <typename T>
BinarySearchTree<T>::BinarySearchTree() : root_(nullptr), size_(0) {
}

template <typename T>
BinarySearchTree<T>::BinarySearchTree(BinarySearchTree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) {
  other.size_ = 0;
}

template <typename T>
auto BinarySearchTree<T>::operator=(BinarySearchTree&& other) noexcept -> BinarySearchTree<T>& {
  if (this != &other) {
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===------------------------ Modification operations -------------------------===//

template <typename T>
auto BinarySearchTree<T>::insert(const T& value) -> bool {
  return insert_helper(root_, value);
}

template <typename T>
auto BinarySearchTree<T>::insert(T&& value) -> bool {
  return insert_helper(root_, std::move(value));
}

template <typename T>
template <typename... Args>
auto BinarySearchTree<T>::emplace(Args&&... args) -> bool {
  // Construct the value in place and use move semantics
  return insert_helper(root_, T(std::forward<Args>(args)...));
}

template <typename T>
template <typename U>
auto BinarySearchTree<T>::insert_helper(std::unique_ptr<Node>& node, U&& value) -> bool {
  // Base case: found the insertion point
  if (!node) {
    node = std::make_unique<Node>(std::forward<U>(value));
    size_++;
    return true;
  }

  // Navigate left or right based on comparison
  if (value < node->data) {
    return insert_helper(node->left, std::forward<U>(value));
  } else if (node->data < value) {
    return insert_helper(node->right, std::forward<U>(value));
  } else {
    // Duplicate found - do not insert
    return false;
  }
}

template <typename T>
auto BinarySearchTree<T>::remove(const T& value) -> bool {
  return remove_helper(root_, value);
}

template <typename T>
auto BinarySearchTree<T>::remove_helper(std::unique_ptr<Node>& node, const T& value) -> bool {
  // Base case: value not found
  if (!node) {
    return false;
  }

  // Navigate to the node to delete
  if (value < node->data) {
    return remove_helper(node->left, value);
  } else if (node->data < value) {
    return remove_helper(node->right, value);
  }

  // Found the node to delete
  // Case 1: Node with no children (leaf)
  if (!node->left && !node->right) {
    node.reset(); // Simply delete the node
    size_--;
    return true;
  }

  // Case 2: Node with only right child
  if (!node->left) {
    node = std::move(node->right); // Replace node with its right child
    size_--;
    return true;
  }

  // Case 3: Node with only left child
  if (!node->right) {
    node = std::move(node->left); // Replace node with its left child
    size_--;
    return true;
  }

  // Case 4: Node with two children
  // Strategy: Replace with the minimum value from the right subtree (in-order successor)
  // Then delete that minimum node from the right subtree
  auto min_node   = detach_min(node->right);
  min_node->left  = std::move(node->left);
  min_node->right = std::move(node->right);
  node            = std::move(min_node);

  size_--;
  return true;
}

template <typename T>
auto BinarySearchTree<T>::detach_min(std::unique_ptr<Node>& node) -> std::unique_ptr<Node> {
  // If no left child, this node is the minimum
  if (!node->left) {
    // Detach this node and replace it with its right child (if any)
    auto min_node   = std::move(node);
    node            = std::move(min_node->right);
    min_node->right = nullptr; // Important: clear the right pointer before returning
    return min_node;
  }

  // Recursively find the minimum in the left subtree
  return detach_min(node->left);
}

template <typename T>
void BinarySearchTree<T>::clear() noexcept {
  // Simply reset the root - the chain of unique_ptr will handle deallocation
  // For very deep trees, we might want an iterative approach to avoid stack overflow
  // but for typical use cases, the automatic cleanup is sufficient
  root_.reset();
  size_ = 0;
}

//===--------------------------- Search operations ----------------------------===//

template <typename T>
auto BinarySearchTree<T>::contains(const T& value) const -> bool {
  return find_helper(root_.get(), value) != nullptr;
}

template <typename T>
auto BinarySearchTree<T>::find_helper(Node* node, const T& value) const -> Node* {
  if (!node) {
    return nullptr;
  }

  if (value < node->data) {
    return find_helper(node->left.get(), value);
  } else if (node->data < value) {
    return find_helper(node->right.get(), value);
  } else {
    return node; // Found it
  }
}

template <typename T>
auto BinarySearchTree<T>::find_min() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("Cannot find minimum in empty tree");
  }
  return find_min_node(root_.get())->data;
}

template <typename T>
auto BinarySearchTree<T>::find_max() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("Cannot find maximum in empty tree");
  }
  return find_max_node(root_.get())->data;
}

template <typename T>
auto BinarySearchTree<T>::find_min_node(Node* node) const -> Node* {
  // The minimum is the leftmost node
  while (node->left) {
    node = node->left.get();
  }
  return node;
}

template <typename T>
auto BinarySearchTree<T>::find_max_node(Node* node) const -> Node* {
  // The maximum is the rightmost node
  while (node->right) {
    node = node->right.get();
  }
  return node;
}

template <typename T>
auto BinarySearchTree<T>::successor(const T& value) const -> const T* {
  Node* current   = root_.get();
  Node* successor = nullptr;

  while (current) {
    if (value < current->data) {
      // Current node could be the successor, but there might be a smaller one
      successor = current;
      current   = current->left.get();
    } else {
      // Go right to find larger values
      current = current->right.get();
    }
  }

  return successor ? &successor->data : nullptr;
}

template <typename T>
auto BinarySearchTree<T>::predecessor(const T& value) const -> const T* {
  Node* current     = root_.get();
  Node* predecessor = nullptr;

  while (current) {
    if (current->data < value) {
      // Current node could be the predecessor, but there might be a larger one
      predecessor = current;
      current     = current->right.get();
    } else {
      // Go left to find smaller values
      current = current->left.get();
    }
  }

  return predecessor ? &predecessor->data : nullptr;
}

//===---------------------------- Tree properties -----------------------------===//

template <typename T>
auto BinarySearchTree<T>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <typename T>
auto BinarySearchTree<T>::size() const noexcept -> size_t {
  return size_;
}

template <typename T>
auto BinarySearchTree<T>::height() const noexcept -> int {
  return height_helper(root_.get());
}

template <typename T>
auto BinarySearchTree<T>::height_helper(const Node* node) const noexcept -> int {
  if (!node) {
    return -1; // Height of empty tree is -1
  }

  int left_height  = height_helper(node->left.get());
  int right_height = height_helper(node->right.get());

  return 1 + std::max(left_height, right_height);
}

//===-------------------------- Traversal operations --------------------------===//

template <typename T>
void BinarySearchTree<T>::in_order_traversal(const std::function<void(const T&)>& visit) const {
  in_order_helper(root_.get(), visit);
}

template <typename T>
void BinarySearchTree<T>::in_order_helper(const Node* node, const std::function<void(const T&)>& visit) const {
  if (!node) {
    return;
  }

  in_order_helper(node->left.get(), visit);
  visit(node->data);
  in_order_helper(node->right.get(), visit);
}

template <typename T>
void BinarySearchTree<T>::pre_order_traversal(const std::function<void(const T&)>& visit) const {
  pre_order_helper(root_.get(), visit);
}

template <typename T>
void BinarySearchTree<T>::pre_order_helper(const Node* node, const std::function<void(const T&)>& visit) const {
  if (!node) {
    return;
  }

  visit(node->data);
  pre_order_helper(node->left.get(), visit);
  pre_order_helper(node->right.get(), visit);
}

template <typename T>
void BinarySearchTree<T>::post_order_traversal(const std::function<void(const T&)>& visit) const {
  post_order_helper(root_.get(), visit);
}

template <typename T>
void BinarySearchTree<T>::post_order_helper(const Node* node, const std::function<void(const T&)>& visit) const {
  if (!node) {
    return;
  }

  post_order_helper(node->left.get(), visit);
  post_order_helper(node->right.get(), visit);
  visit(node->data);
}

template <typename T>
void BinarySearchTree<T>::level_order_traversal(const std::function<void(const T&)>& visit) const {
  if (!root_) {
    return;
  }

  // Use a queue for breadth-first traversal
  std::queue<const Node*> queue;
  queue.push(root_.get());

  while (!queue.empty()) {
    const Node* current = queue.front();
    queue.pop();

    visit(current->data);

    if (current->left) {
      queue.push(current->left.get());
    }
    if (current->right) {
      queue.push(current->right.get());
    }
  }
}

//===---------------------------- Iterator access -----------------------------===//

template <typename T>
auto BinarySearchTree<T>::begin() -> iterator {
  return iterator(root_.get());
}

template <typename T>
auto BinarySearchTree<T>::end() -> iterator {
  return iterator(); // Default-constructed iterator represents end
}

template <typename T>
auto BinarySearchTree<T>::begin() const -> iterator {
  return iterator(root_.get());
}

template <typename T>
auto BinarySearchTree<T>::end() const -> iterator {
  return iterator();
}

template <typename T>
auto BinarySearchTree<T>::cbegin() const -> iterator {
  return begin();
}

template <typename T>
auto BinarySearchTree<T>::cend() const -> iterator {
  return end();
}

} // namespace ads::trees

//===--------------------------------------------------------------------------===//
