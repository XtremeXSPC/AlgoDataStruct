//===---------------------------------------------------------------------------===//
/**
 * @file Complete_Binary_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for CompleteBinaryTree.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/trees/Complete_Binary_Tree.hpp"

namespace ads::trees {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
CompleteBinaryTree<T>::CompleteBinaryTree() : root_(nullptr), size_(0) {
}

template <typename T>
CompleteBinaryTree<T>::CompleteBinaryTree(std::initializer_list<T> values) : root_(nullptr), size_(0) {
  for (const auto& value : values) {
    insert(value);
  }
}

template <typename T>
CompleteBinaryTree<T>::CompleteBinaryTree(CompleteBinaryTree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) {
  other.size_ = 0;
}

template <typename T>
auto CompleteBinaryTree<T>::operator=(CompleteBinaryTree&& other) noexcept -> CompleteBinaryTree& {
  if (this != &other) {
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename T>
auto CompleteBinaryTree<T>::insert(const T& value) -> void {
  emplace(value);
}

template <typename T>
auto CompleteBinaryTree<T>::insert(T&& value) -> void {
  emplace(std::move(value));
}

template <typename T>
template <typename... Args>
auto CompleteBinaryTree<T>::emplace(Args&&... args) -> T& {
  auto new_node = std::make_unique<Node>(std::forward<Args>(args)...);
  T&   ref      = new_node->data;

  if (is_empty()) {
    root_ = std::move(new_node);
    ++size_;
    return ref;
  }

  // Use BFS to find the first node with an empty child slot.
  std::queue<Node*> queue;
  queue.push(root_.get());

  while (!queue.empty()) {
    Node* current = queue.front();
    queue.pop();

    if (!current->left) {
      current->left = std::move(new_node);
      ++size_;
      return ref;
    }
    queue.push(current->left.get());

    if (!current->right) {
      current->right = std::move(new_node);
      ++size_;
      return ref;
    }
    queue.push(current->right.get());
  }

  ++size_;
  return ref;
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
auto CompleteBinaryTree<T>::clear() noexcept -> void {
  root_.reset();
  size_ = 0;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto CompleteBinaryTree<T>::root() -> T& {
  if (is_empty()) {
    throw EmptyTreeException("root() called on empty tree");
  }
  return root_->data;
}

template <typename T>
auto CompleteBinaryTree<T>::root() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("root() called on empty tree");
  }
  return root_->data;
}

template <typename T>
auto CompleteBinaryTree<T>::root_node() -> Node* {
  return root_.get();
}

template <typename T>
auto CompleteBinaryTree<T>::root_node() const -> const Node* {
  return root_.get();
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto CompleteBinaryTree<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T>
auto CompleteBinaryTree<T>::size() const noexcept -> size_t {
  return size_;
}

template <typename T>
auto CompleteBinaryTree<T>::height() const noexcept -> int {
  return compute_height(root_.get());
}

template <typename T>
auto CompleteBinaryTree<T>::contains(const T& value) const -> bool {
  if (is_empty()) {
    return false;
  }

  std::queue<const Node*> queue;
  queue.push(root_.get());

  while (!queue.empty()) {
    const Node* current = queue.front();
    queue.pop();

    if (current->data == value) {
      return true;
    }

    if (current->left) {
      queue.push(current->left.get());
    }
    if (current->right) {
      queue.push(current->right.get());
    }
  }

  return false;
}

//===-------------------------- TRAVERSAL OPERATIONS ---------------------------===//

template <typename T>
auto CompleteBinaryTree<T>::in_order_traversal(const std::function<void(const T&)>& visit) const -> void {
  in_order_impl(root_.get(), visit);
}

template <typename T>
auto CompleteBinaryTree<T>::pre_order_traversal(const std::function<void(const T&)>& visit) const -> void {
  pre_order_impl(root_.get(), visit);
}

template <typename T>
auto CompleteBinaryTree<T>::post_order_traversal(const std::function<void(const T&)>& visit) const -> void {
  post_order_impl(root_.get(), visit);
}

template <typename T>
auto CompleteBinaryTree<T>::level_order_traversal(const std::function<void(const T&)>& visit) const -> void {
  if (is_empty()) {
    return;
  }

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

template <typename T>
auto CompleteBinaryTree<T>::to_vector() const -> std::vector<T> {
  std::vector<T> result;
  result.reserve(size_);
  level_order_traversal([&result](const T& value) { result.push_back(value); });
  return result;
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T>
auto CompleteBinaryTree<T>::compute_height(const Node* node) const noexcept -> int {
  if (node == nullptr) {
    return -1;
  }
  int left_height  = compute_height(node->left.get());
  int right_height = compute_height(node->right.get());
  return 1 + std::max(left_height, right_height);
}

template <typename T>
auto CompleteBinaryTree<T>::in_order_impl(const Node* node, const std::function<void(const T&)>& visit) const -> void {
  if (node == nullptr) {
    return;
  }
  in_order_impl(node->left.get(), visit);
  visit(node->data);
  in_order_impl(node->right.get(), visit);
}

template <typename T>
auto CompleteBinaryTree<T>::pre_order_impl(const Node* node, const std::function<void(const T&)>& visit) const -> void {
  if (node == nullptr) {
    return;
  }
  visit(node->data);
  pre_order_impl(node->left.get(), visit);
  pre_order_impl(node->right.get(), visit);
}

template <typename T>
auto CompleteBinaryTree<T>::post_order_impl(const Node* node, const std::function<void(const T&)>& visit) const -> void {
  if (node == nullptr) {
    return;
  }
  post_order_impl(node->left.get(), visit);
  post_order_impl(node->right.get(), visit);
  visit(node->data);
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
