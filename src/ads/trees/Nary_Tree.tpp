//===---------------------------------------------------------------------------===//
/**
 * @file Nary_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for NaryTree.
 * @version 0.1
 * @date 2026-06-19
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/trees/Nary_Tree.hpp"

namespace ads::trees {

//===----- NODE IMPLEMENTATION -------------------------------------------------===//

template <EqualityComparableTreeElement T>
auto NaryTree<T>::Node::value() noexcept -> T& {
  return data;
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::Node::value() const noexcept -> const T& {
  return data;
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::Node::parent() noexcept -> Node* {
  return parent_;
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::Node::parent() const noexcept -> const Node* {
  return parent_;
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::Node::child_count() const noexcept -> size_t {
  return children_.size();
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::Node::child(size_t index) -> Node* {
  if (index >= children_.size()) {
    throw InvalidOperationException("NaryTree child index out of range");
  }
  return children_[index].get();
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::Node::child(size_t index) const -> const Node* {
  if (index >= children_.size()) {
    throw InvalidOperationException("NaryTree child index out of range");
  }
  return children_[index].get();
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::Node::is_leaf() const noexcept -> bool {
  return children_.is_empty();
}

//===----- CONSTRUCTORS AND ASSIGNMENT -----------------------------------------===//

template <EqualityComparableTreeElement T>
NaryTree<T>::NaryTree() : root_(nullptr), size_(0) {
}

template <EqualityComparableTreeElement T>
NaryTree<T>::NaryTree(NaryTree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) {
  other.size_ = 0;
}

template <EqualityComparableTreeElement T>
NaryTree<T>::~NaryTree() {
  clear();
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::operator=(NaryTree&& other) noexcept -> NaryTree<T>& {
  if (this != &other) {
    clear();
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <EqualityComparableTreeElement T>
auto NaryTree<T>::set_root(const T& value) -> Node* requires std::copy_constructible<T>
{
  return set_root_impl(value);
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::set_root(T&& value) -> Node* {
  return set_root_impl(std::move(value));
}

template <EqualityComparableTreeElement T>
template <typename... Args>
auto NaryTree<T>::emplace_root(Args&&... args) -> Node* {
  return set_root_impl(T(std::forward<Args>(args)...));
}

template <EqualityComparableTreeElement T>
template <typename U>
auto NaryTree<T>::set_root_impl(U&& value) -> Node* {
  if (root_) {
    throw InvalidOperationException("set_root called on a non-empty tree");
  }
  root_ = std::unique_ptr<Node>(new Node(std::forward<U>(value)));
  size_ = 1;
  return root_.get();
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::add_child(Node* parent, const T& value) -> Node* requires std::copy_constructible<T>
{
  return add_child_impl(parent, value);
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::add_child(Node* parent, T&& value) -> Node* {
  return add_child_impl(parent, std::move(value));
}

template <EqualityComparableTreeElement T>
template <typename... Args>
auto NaryTree<T>::emplace_child(Node* parent, Args&&... args) -> Node* {
  return add_child_impl(parent, T(std::forward<Args>(args)...));
}

template <EqualityComparableTreeElement T>
template <typename U>
auto NaryTree<T>::add_child_impl(Node* parent, U&& value) -> Node* {
  if (parent == nullptr) {
    throw InvalidOperationException("add_child called with a null parent");
  }
  auto  child  = std::unique_ptr<Node>(new Node(std::forward<U>(value)));
  Node* raw    = child.get();
  raw->parent_ = parent;
  parent->children_.push_back(std::move(child));
  ++size_;
  return raw;
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <EqualityComparableTreeElement T>
void NaryTree<T>::clear() noexcept {
  if (!root_) {
    size_ = 0;
    return;
  }

  // Iterative, allocation-free teardown guided by the parent pointers: walk to
  // the deepest last child and pop it from its parent, so every node destructs
  // as a leaf. Each edge is traversed once (O(n)) and no worklist is needed,
  // so a noexcept clear() cannot abort on allocation failure.
  Node* current = root_.get();
  while (true) {
    if (!current->children_.is_empty()) {
      current = current->children_.back().get();
      continue;
    }
    Node* parent = current->parent_;
    if (parent == nullptr) {
      break; // the root itself is now a leaf
    }
    parent->children_.pop_back(); // destroys the current leaf in place
    current = parent;
  }

  root_.reset();
  size_ = 0;
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <EqualityComparableTreeElement T>
auto NaryTree<T>::root() noexcept -> Node* {
  return root_.get();
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::root() const noexcept -> const Node* {
  return root_.get();
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <EqualityComparableTreeElement T>
auto NaryTree<T>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::size() const noexcept -> size_t {
  return size_;
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::height() const noexcept -> int {
  if (!root_) {
    return -1;
  }

  struct HeightFrame {
    const Node* node;
    int         depth;
  };

  int                       max_depth = 0;
  DynamicArray<HeightFrame> stack;
  stack.push_back(HeightFrame{root_.get(), 0});

  while (!stack.is_empty()) {
    HeightFrame frame = stack.back();
    stack.pop_back();

    max_depth = frame.depth > max_depth ? frame.depth : max_depth;
    for (size_t i = 0; i < frame.node->children_.size(); ++i) {
      stack.push_back(HeightFrame{frame.node->children_[i].get(), frame.depth + 1});
    }
  }

  return max_depth;
}

template <EqualityComparableTreeElement T>
auto NaryTree<T>::contains(const T& value) const -> bool {
  if (!root_) {
    return false;
  }

  ads::queues::LinkedQueue<const Node*> queue;
  queue.enqueue(root_.get());

  while (!queue.is_empty()) {
    const Node* current = queue.front();
    queue.dequeue();

    if (current->data == value) {
      return true;
    }
    for (size_t i = 0; i < current->children_.size(); ++i) {
      queue.enqueue(current->children_[i].get());
    }
  }

  return false;
}

//===----- TRAVERSAL OPERATIONS ------------------------------------------------===//

template <EqualityComparableTreeElement T>
void NaryTree<T>::pre_order_traversal(const visitor_type& visit) const {
  if (!root_) {
    return;
  }

  DynamicArray<const Node*> stack;
  stack.push_back(root_.get());

  while (!stack.is_empty()) {
    const Node* current = stack.back();
    stack.pop_back();

    visit(current->data);

    // Push children in reverse so the leftmost child is visited next.
    for (size_t i = current->children_.size(); i > 0; --i) {
      stack.push_back(current->children_[i - 1].get());
    }
  }
}

template <EqualityComparableTreeElement T>
void NaryTree<T>::post_order_traversal(const visitor_type& visit) const {
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
    for (size_t i = frame.node->children_.size(); i > 0; --i) {
      stack.push_back(PostOrderFrame{frame.node->children_[i - 1].get(), false});
    }
  }
}

template <EqualityComparableTreeElement T>
void NaryTree<T>::level_order_traversal(const visitor_type& visit) const {
  if (!root_) {
    return;
  }

  ads::queues::LinkedQueue<const Node*> queue;
  queue.enqueue(root_.get());

  while (!queue.is_empty()) {
    const Node* current = queue.front();
    queue.dequeue();

    visit(current->data);

    for (size_t i = 0; i < current->children_.size(); ++i) {
      queue.enqueue(current->children_[i].get());
    }
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
