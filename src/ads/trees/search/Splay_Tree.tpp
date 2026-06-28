//===---------------------------------------------------------------------------===//
/**
 * @file Splay_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for SplayTree.
 * @version 0.1
 * @date 2026-06-19
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../../include/ads/trees/search/Splay_Tree.hpp"

namespace ads::trees {

//===----- ITERATOR IMPLEMENTATION ---------------------------------------------===//

template <OrderedTreeElement T>
SplayTree<T>::iterator::iterator(Node* current) : current_(current) {
}

template <OrderedTreeElement T>
auto SplayTree<T>::iterator::leftmost(Node* node) noexcept -> Node* {
  while (node && node->left) {
    node = node->left.get();
  }
  return node;
}

template <OrderedTreeElement T>
auto SplayTree<T>::iterator::successor(Node* node) noexcept -> Node* {
  if (!node) {
    return nullptr;
  }
  if (node->right) {
    return leftmost(node->right.get());
  }

  Node* parent = node->parent;
  while (parent && node == parent->right.get()) {
    node   = parent;
    parent = parent->parent;
  }
  return parent;
}

template <OrderedTreeElement T>
auto SplayTree<T>::iterator::operator*() const -> reference {
  return current_->data;
}

template <OrderedTreeElement T>
auto SplayTree<T>::iterator::operator->() const -> pointer {
  return &current_->data;
}

template <OrderedTreeElement T>
auto SplayTree<T>::iterator::operator++() -> iterator& {
  current_ = successor(current_);
  return *this;
}

template <OrderedTreeElement T>
auto SplayTree<T>::iterator::operator++(int) -> iterator {
  iterator previous = *this;
  ++(*this);
  return previous;
}

template <OrderedTreeElement T>
auto SplayTree<T>::iterator::operator==(const iterator& other) const -> bool {
  return current_ == other.current_;
}

//===----- CONSTRUCTORS AND ASSIGNMENT -----------------------------------------===//

template <OrderedTreeElement T>
SplayTree<T>::SplayTree() : root_(nullptr), size_(0) {
}

template <OrderedTreeElement T>
SplayTree<T>::SplayTree(SplayTree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) {
  other.size_ = 0;
  if (root_) {
    root_->parent = nullptr;
  }
}

template <OrderedTreeElement T>
SplayTree<T>::~SplayTree() {
  clear();
}

template <OrderedTreeElement T>
auto SplayTree<T>::operator=(SplayTree&& other) noexcept -> SplayTree<T>& {
  if (this != &other) {
    clear();
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
    if (root_) {
      root_->parent = nullptr;
    }
  }
  return *this;
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <OrderedTreeElement T>
auto SplayTree<T>::insert(const T& value) -> bool requires std::copy_constructible<T>
{
  return insert_impl(value);
}

template <OrderedTreeElement T>
auto SplayTree<T>::insert(T&& value) -> bool {
  return insert_impl(std::move(value));
}

template <OrderedTreeElement T>
template <typename... Args>
auto SplayTree<T>::emplace(Args&&... args) -> bool {
  return insert_impl(T(std::forward<Args>(args)...));
}

template <OrderedTreeElement T>
template <typename U>
auto SplayTree<T>::insert_impl(U&& value) -> bool {
  if (!root_) {
    root_ = std::make_unique<Node>(std::forward<U>(value));
    ++size_;
    return true;
  }

  // Descend, recording the slot the new node will occupy. The direction is
  // captured before the value is forwarded so a moved-from value is never read.
  Node* current = root_.get();
  Node* parent  = nullptr;
  bool  go_left = false;
  while (current) {
    parent = current;
    if (value < current->data) {
      go_left = true;
      current = current->left.get();
    } else if (current->data < value) {
      go_left = false;
      current = current->right.get();
    } else {
      // Duplicate: splay the existing node and report no insertion.
      splay(current);
      return false;
    }
  }

  auto  node = std::make_unique<Node>(std::forward<U>(value));
  Node* raw  = node.get();
  raw->parent = parent;
  if (go_left) {
    parent->left = std::move(node);
  } else {
    parent->right = std::move(node);
  }
  ++size_;

  splay(raw);
  return true;
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <OrderedTreeElement T>
auto SplayTree<T>::remove(const T& value) -> bool {
  Node* target = find_node(value);
  if (!target) {
    return false;
  }

  // Bring the target to the root, detach its subtrees, then rejoin them by
  // splaying the maximum of the left subtree (whose right child becomes null).
  splay(target);

  auto left  = std::move(root_->left);
  auto right = std::move(root_->right);
  if (left) {
    left->parent = nullptr;
  }
  if (right) {
    right->parent = nullptr;
  }
  root_.reset(); // Destroy the detached target node.

  if (!left) {
    root_ = std::move(right);
  } else {
    root_       = std::move(left);
    Node* max_l = subtree_max(root_.get());
    splay(max_l);
    root_->right = std::move(right);
    if (root_->right) {
      root_->right->parent = root_.get();
    }
  }

  --size_;
  return true;
}

template <OrderedTreeElement T>
void SplayTree<T>::clear() noexcept {
  // Iteratively dismantle the tree so a degenerate chain cannot overflow the
  // stack through recursive unique_ptr destruction.
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

template <OrderedTreeElement T>
auto SplayTree<T>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <OrderedTreeElement T>
auto SplayTree<T>::size() const noexcept -> size_t {
  return size_;
}

template <OrderedTreeElement T>
auto SplayTree<T>::height() const noexcept -> int {
  return height_helper(root_.get());
}

template <OrderedTreeElement T>
auto SplayTree<T>::contains(const T& value) const -> bool {
  Node* current = root_.get();
  Node* last    = nullptr;

  while (current) {
    last = current;
    if (value < current->data) {
      current = current->left.get();
    } else if (current->data < value) {
      current = current->right.get();
    } else {
      splay(current);
      return true;
    }
  }

  // Splay the last node on the access path even on a miss (standard behavior).
  if (last) {
    splay(last);
  }
  return false;
}

template <OrderedTreeElement T>
auto SplayTree<T>::find_min() const -> const T& {
  if (!root_) {
    throw EmptyTreeException("Cannot find minimum in empty tree");
  }
  Node* node = subtree_min(root_.get());
  splay(node);
  return root_->data;
}

template <OrderedTreeElement T>
auto SplayTree<T>::find_max() const -> const T& {
  if (!root_) {
    throw EmptyTreeException("Cannot find maximum in empty tree");
  }
  Node* node = subtree_max(root_.get());
  splay(node);
  return root_->data;
}

//===----- TRAVERSAL OPERATIONS ------------------------------------------------===//

template <OrderedTreeElement T>
void SplayTree<T>::in_order_traversal(const visitor_type& visit) const {
  for (Node* node = iterator::leftmost(root_.get()); node != nullptr; node = iterator::successor(node)) {
    visit(node->data);
  }
}

template <OrderedTreeElement T>
void SplayTree<T>::pre_order_traversal(const visitor_type& visit) const {
  if (!root_) {
    return;
  }

  DynamicArray<const Node*> stack;
  stack.push_back(root_.get());

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

template <OrderedTreeElement T>
void SplayTree<T>::post_order_traversal(const visitor_type& visit) const {
  if (!root_) {
    return;
  }

  struct PostOrderFrame {
    const Node* node;
    bool        visited;
  };

  DynamicArray<PostOrderFrame> stack;
  stack.push_back(PostOrderFrame{root_.get(), false});

  while (!stack.is_empty()) {
    PostOrderFrame frame = stack.back();
    stack.pop_back();

    if (frame.visited) {
      visit(frame.node->data);
      continue;
    }

    stack.push_back(PostOrderFrame{frame.node, true});
    if (frame.node->right) {
      stack.push_back(PostOrderFrame{frame.node->right.get(), false});
    }
    if (frame.node->left) {
      stack.push_back(PostOrderFrame{frame.node->left.get(), false});
    }
  }
}

template <OrderedTreeElement T>
void SplayTree<T>::level_order_traversal(const visitor_type& visit) const {
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

template <OrderedTreeElement T>
auto SplayTree<T>::validate_properties() const -> bool {
  return validate_helper();
}

//===----- ITERATOR OPERATIONS -------------------------------------------------===//

template <OrderedTreeElement T>
auto SplayTree<T>::begin() -> iterator {
  return iterator(iterator::leftmost(root_.get()));
}

template <OrderedTreeElement T>
auto SplayTree<T>::begin() const -> iterator {
  return iterator(iterator::leftmost(root_.get()));
}

template <OrderedTreeElement T>
auto SplayTree<T>::end() -> iterator {
  return iterator();
}

template <OrderedTreeElement T>
auto SplayTree<T>::end() const -> iterator {
  return iterator();
}

template <OrderedTreeElement T>
auto SplayTree<T>::cbegin() const -> iterator {
  return begin();
}

template <OrderedTreeElement T>
auto SplayTree<T>::cend() const -> iterator {
  return end();
}

//=================================================================================//
//===----- STRUCTURAL (SPLAY) HELPERS ------------------------------------------===//

template <OrderedTreeElement T>
auto SplayTree<T>::owning_link(Node* node) const -> std::unique_ptr<Node>& {
  if (node->parent == nullptr) {
    return root_;
  }
  if (node->parent->left.get() == node) {
    return node->parent->left;
  }
  return node->parent->right;
}

template <OrderedTreeElement T>
auto SplayTree<T>::rotate_left(std::unique_ptr<Node> x_ptr) const -> std::unique_ptr<Node> {
  auto  y_ptr = std::move(x_ptr->right);
  Node* x     = x_ptr.get();

  // y's left subtree becomes x's right subtree.
  x_ptr->right = std::move(y_ptr->left);
  if (x_ptr->right) {
    x_ptr->right->parent = x;
  }

  // x becomes y's left child.
  y_ptr->left         = std::move(x_ptr);
  y_ptr->left->parent = y_ptr.get();

  return y_ptr;
}

template <OrderedTreeElement T>
auto SplayTree<T>::rotate_right(std::unique_ptr<Node> y_ptr) const -> std::unique_ptr<Node> {
  auto  x_ptr = std::move(y_ptr->left);
  Node* y     = y_ptr.get();

  // x's right subtree becomes y's left subtree.
  y_ptr->left = std::move(x_ptr->right);
  if (y_ptr->left) {
    y_ptr->left->parent = y;
  }

  // y becomes x's right child.
  x_ptr->right         = std::move(y_ptr);
  x_ptr->right->parent = x_ptr.get();

  return x_ptr;
}

template <OrderedTreeElement T>
void SplayTree<T>::rotate_left_at(Node* node) const {
  Node* parent = node->parent;
  auto& link   = owning_link(node);
  link         = rotate_left(std::move(link));
  link->parent = parent;
}

template <OrderedTreeElement T>
void SplayTree<T>::rotate_right_at(Node* node) const {
  Node* parent = node->parent;
  auto& link   = owning_link(node);
  link         = rotate_right(std::move(link));
  link->parent = parent;
}

template <OrderedTreeElement T>
void SplayTree<T>::splay(Node* node) const {
  if (!node) {
    return;
  }

  while (node->parent != nullptr) {
    Node* parent      = node->parent;
    Node* grandparent = parent->parent;

    const bool node_is_left = parent->left.get() == node;

    if (grandparent == nullptr) {
      // Zig: single rotation at the parent.
      if (node_is_left) {
        rotate_right_at(parent);
      } else {
        rotate_left_at(parent);
      }
    } else {
      const bool parent_is_left = grandparent->left.get() == parent;

      if (node_is_left && parent_is_left) {
        // Zig-zig (left): rotate grandparent then parent.
        rotate_right_at(grandparent);
        rotate_right_at(parent);
      } else if (!node_is_left && !parent_is_left) {
        // Zig-zig (right).
        rotate_left_at(grandparent);
        rotate_left_at(parent);
      } else if (!node_is_left && parent_is_left) {
        // Zig-zag.
        rotate_left_at(parent);
        rotate_right_at(grandparent);
      } else {
        // Zig-zag (mirror).
        rotate_right_at(parent);
        rotate_left_at(grandparent);
      }
    }
  }
}

//===----- SEARCH HELPERS ------------------------------------------------------===//

template <OrderedTreeElement T>
auto SplayTree<T>::find_node(const T& value) const -> Node* {
  Node* current = root_.get();
  while (current) {
    if (value < current->data) {
      current = current->left.get();
    } else if (current->data < value) {
      current = current->right.get();
    } else {
      return current;
    }
  }
  return nullptr;
}

template <OrderedTreeElement T>
auto SplayTree<T>::subtree_min(Node* node) noexcept -> Node* {
  while (node && node->left) {
    node = node->left.get();
  }
  return node;
}

template <OrderedTreeElement T>
auto SplayTree<T>::subtree_max(Node* node) noexcept -> Node* {
  while (node && node->right) {
    node = node->right.get();
  }
  return node;
}

//===----- ITERATIVE TREE HELPERS ----------------------------------------------===//

template <OrderedTreeElement T>
auto SplayTree<T>::height_helper(const Node* node) const noexcept -> int {
  if (!node) {
    return -1;
  }

  struct HeightFrame {
    const Node* node;
    int         depth;
  };

  int                          max_depth = -1;
  DynamicArray<HeightFrame> stack;
  stack.push_back(HeightFrame{node, 0});

  while (!stack.is_empty()) {
    HeightFrame frame = stack.back();
    stack.pop_back();

    max_depth = frame.depth > max_depth ? frame.depth : max_depth;
    if (frame.node->left) {
      stack.push_back(HeightFrame{frame.node->left.get(), frame.depth + 1});
    }
    if (frame.node->right) {
      stack.push_back(HeightFrame{frame.node->right.get(), frame.depth + 1});
    }
  }

  return max_depth;
}

template <OrderedTreeElement T>
auto SplayTree<T>::validate_helper() const -> bool {
  if (!root_) {
    return size_ == 0;
  }

  if (root_->parent != nullptr) {
    return false;
  }

  struct ValidationFrame {
    const Node* node;
    const T*    lower_bound;
    const T*    upper_bound;
  };

  size_t                            counted = 0;
  DynamicArray<ValidationFrame> stack;
  stack.push_back(ValidationFrame{root_.get(), nullptr, nullptr});

  while (!stack.is_empty()) {
    const ValidationFrame frame = stack.back();
    stack.pop_back();

    if (frame.lower_bound != nullptr && !(*frame.lower_bound < frame.node->data)) {
      return false;
    }
    if (frame.upper_bound != nullptr && !(frame.node->data < *frame.upper_bound)) {
      return false;
    }

    ++counted;

    if (frame.node->left) {
      if (frame.node->left->parent != frame.node) {
        return false;
      }
      stack.push_back(ValidationFrame{frame.node->left.get(), frame.lower_bound, &frame.node->data});
    }
    if (frame.node->right) {
      if (frame.node->right->parent != frame.node) {
        return false;
      }
      stack.push_back(ValidationFrame{frame.node->right.get(), &frame.node->data, frame.upper_bound});
    }
  }

  return counted == size_;
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
