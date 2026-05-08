//===---------------------------------------------------------------------------===//
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
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/trees/Binary_Search_Tree.hpp"

namespace ads::trees {

// DynamicArray keeps iterator state copyable; LinkedQueue provides BFS storage without std::queue.

//===------------------------- ITERATOR IMPLEMENTATION -------------------------===//

template <OrderedTreeElement T>
BinarySearchTree<T>::iterator::iterator(const iterator& other) :
    stack_(other.stack_.begin(), other.stack_.end()),
    current_(other.current_) {
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::iterator::operator=(const iterator& other) -> iterator& {
  if (this != &other) {
    // Assign after building replacement storage so a failed copy keeps this iterator usable.
    stack_.assign(other.stack_.begin(), other.stack_.end());
    current_ = other.current_;
  }
  return *this;
}

template <OrderedTreeElement T>
BinarySearchTree<T>::iterator::iterator(Node* root) : current_(nullptr) {
  push_left(root);
  if (!stack_.is_empty()) {
    current_ = stack_.back();
    stack_.pop_back();
  }
}

template <OrderedTreeElement T>
void BinarySearchTree<T>::iterator::push_left(Node* node) {
  while (node) {
    stack_.push_back(node);
    node = node->left.get();
  }
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::iterator::operator*() const -> reference {
  return current_->data;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::iterator::operator->() const -> pointer {
  return &current_->data;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::iterator::operator++() -> iterator& {
  // If current has a right child, go to it and then all the way left.
  if (current_->right) {
    push_left(current_->right.get());
  }

  // Get the next node from the stack.
  if (!stack_.is_empty()) {
    current_ = stack_.back();
    stack_.pop_back();
  } else {
    current_ = nullptr; // We've reached the end.
  }

  return *this;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::iterator::operator++(int) -> iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::iterator::operator==(const iterator& other) const -> bool {
  return current_ == other.current_;
}

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

template <OrderedTreeElement T>
BinarySearchTree<T>::BinarySearchTree() : root_(nullptr), size_(0) {
}

template <OrderedTreeElement T>
BinarySearchTree<T>::BinarySearchTree(BinarySearchTree&& other) noexcept :
    root_(std::move(other.root_)),
    size_(other.size_) {
  other.size_ = 0;
}

template <OrderedTreeElement T>
BinarySearchTree<T>::~BinarySearchTree() {
  clear();
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::operator=(BinarySearchTree&& other) noexcept -> BinarySearchTree<T>& {
  if (this != &other) {
    clear();
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <OrderedTreeElement T>
auto BinarySearchTree<T>::insert(const T& value) -> bool
  requires std::copy_constructible<T>
{
  return insert_helper(root_, value);
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::insert(T&& value) -> bool {
  return insert_helper(root_, std::move(value));
}

template <OrderedTreeElement T>
template <typename... Args>
auto BinarySearchTree<T>::emplace(Args&&... args) -> bool {
  // Construct the value in place and use move semantics.
  return insert_helper(root_, T(std::forward<Args>(args)...));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <OrderedTreeElement T>
auto BinarySearchTree<T>::remove(const T& value) -> bool {
  return remove_helper(root_, value);
}

template <OrderedTreeElement T>
void BinarySearchTree<T>::clear() noexcept {
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

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <OrderedTreeElement T>
auto BinarySearchTree<T>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::size() const noexcept -> size_t {
  return size_;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::height() const noexcept -> int {
  return height_helper(root_.get());
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::contains(const T& value) const -> bool {
  return find_helper(root_.get(), value) != nullptr;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::find_min() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("Cannot find minimum in empty tree");
  }
  return find_min_node(root_.get())->data;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::find_max() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("Cannot find maximum in empty tree");
  }
  return find_max_node(root_.get())->data;
}

//===-------------------------- TRAVERSAL OPERATIONS ---------------------------===//

template <OrderedTreeElement T>
void BinarySearchTree<T>::in_order_traversal(const std::function<void(const T&)>& visit) const {
  in_order_helper(root_.get(), visit);
}

template <OrderedTreeElement T>
void BinarySearchTree<T>::pre_order_traversal(const std::function<void(const T&)>& visit) const {
  pre_order_helper(root_.get(), visit);
}

template <OrderedTreeElement T>
void BinarySearchTree<T>::post_order_traversal(const std::function<void(const T&)>& visit) const {
  post_order_helper(root_.get(), visit);
}

template <OrderedTreeElement T>
void BinarySearchTree<T>::level_order_traversal(const std::function<void(const T&)>& visit) const {
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

//===------------------ ADDITIONAL BST-SPECIFIC FUNCTIONALITY ------------------===//

template <OrderedTreeElement T>
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

template <OrderedTreeElement T>
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

template <OrderedTreeElement T>
auto BinarySearchTree<T>::validate_properties() const -> bool {
  if (!root_) {
    return size_ == 0;
  }

  struct ValidationFrame {
    const Node* node;
    const T*    lower_bound;
    const T*    upper_bound;
  };

  size_t                                     counted = 0;
  ads::arrays::DynamicArray<ValidationFrame> stack;
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

    if (frame.node->right) {
      stack.push_back(ValidationFrame{frame.node->right.get(), &frame.node->data, frame.upper_bound});
    }
    if (frame.node->left) {
      stack.push_back(ValidationFrame{frame.node->left.get(), frame.lower_bound, &frame.node->data});
    }
  }

  return counted == size_;
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <OrderedTreeElement T>
auto BinarySearchTree<T>::begin() -> iterator {
  return iterator(root_.get());
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::begin() const -> iterator {
  return iterator(root_.get());
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::end() -> iterator {
  return iterator(); // Default-constructed iterator represents end
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::end() const -> iterator {
  return iterator();
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::cbegin() const -> iterator {
  return begin();
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::cend() const -> iterator {
  return end();
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <OrderedTreeElement T>
template <typename U>
auto BinarySearchTree<T>::insert_helper(std::unique_ptr<Node>& node, U&& value) -> bool {
  auto* current = &node;
  while (*current) {
    if (value < (*current)->data) {
      current = &((*current)->left);
    } else if ((*current)->data < value) {
      current = &((*current)->right);
    } else {
      return false;
    }
  }

  *current = std::make_unique<Node>(std::forward<U>(value));
  ++size_;
  return true;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::remove_helper(std::unique_ptr<Node>& node, const T& value) -> bool {
  auto* current = &node;
  while (*current) {
    if (value < (*current)->data) {
      current = &((*current)->left);
    } else if ((*current)->data < value) {
      current = &((*current)->right);
    } else {
      break;
    }
  }

  if (!*current) {
    return false;
  }

  if (!(*current)->left) {
    auto replacement = std::move((*current)->right);
    current->reset();
    *current = std::move(replacement);
    --size_;
    return true;
  }

  if (!(*current)->right) {
    auto replacement = std::move((*current)->left);
    current->reset();
    *current = std::move(replacement);
    --size_;
    return true;
  }

  auto old_node   = std::move(*current);
  auto min_node   = detach_min(old_node->right);
  min_node->left  = std::move(old_node->left);
  min_node->right = std::move(old_node->right);
  *current        = std::move(min_node);

  --size_;
  return true;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::detach_min(std::unique_ptr<Node>& node) -> std::unique_ptr<Node> {
  auto* current = &node;
  while ((*current)->left) {
    current = &((*current)->left);
  }

  auto min_node   = std::move(*current);
  *current        = std::move(min_node->right);
  min_node->right = nullptr;
  return min_node;
}

//===----------------------------- SEARCH HELPERS ------------------------------===//

template <OrderedTreeElement T>
auto BinarySearchTree<T>::find_helper(Node* node, const T& value) const -> Node* {
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

template <OrderedTreeElement T>
auto BinarySearchTree<T>::find_min_node(Node* node) const -> Node* {
  // The minimum is the leftmost node.
  while (node->left) {
    node = node->left.get();
  }
  return node;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::find_max_node(Node* node) const -> Node* {
  // The maximum is the rightmost node
  while (node->right) {
    node = node->right.get();
  }
  return node;
}

template <OrderedTreeElement T>
auto BinarySearchTree<T>::height_helper(const Node* node) const noexcept -> int {
  if (!node) {
    return -1;
  }

  struct HeightFrame {
    const Node* node;
    int         depth;
  };

  int                                    max_depth = -1;
  ads::arrays::DynamicArray<HeightFrame> stack;
  stack.push_back(HeightFrame{node, 0});

  while (!stack.is_empty()) {
    HeightFrame frame = stack.back();
    stack.pop_back();

    max_depth = std::max(max_depth, frame.depth);
    if (frame.node->left) {
      stack.push_back(HeightFrame{frame.node->left.get(), frame.depth + 1});
    }
    if (frame.node->right) {
      stack.push_back(HeightFrame{frame.node->right.get(), frame.depth + 1});
    }
  }

  return max_depth;
}

//===---------------------------- TRAVERSAL HELPERS ----------------------------===//

template <OrderedTreeElement T>
void BinarySearchTree<T>::in_order_helper(const Node* node, const std::function<void(const T&)>& visit) const {
  ads::arrays::DynamicArray<const Node*> stack;
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

template <OrderedTreeElement T>
void BinarySearchTree<T>::pre_order_helper(const Node* node, const std::function<void(const T&)>& visit) const {
  if (!node) {
    return;
  }

  ads::arrays::DynamicArray<const Node*> stack;
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

template <OrderedTreeElement T>
void BinarySearchTree<T>::post_order_helper(const Node* node, const std::function<void(const T&)>& visit) const {
  if (!node) {
    return;
  }

  struct PostOrderFrame {
    const Node* node;
    bool        visited;
  };

  ads::arrays::DynamicArray<PostOrderFrame> stack;
  stack.push_back(PostOrderFrame{node, false});

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

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
