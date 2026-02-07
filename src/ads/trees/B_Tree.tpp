//===---------------------------------------------------------------------------===//
/**
 * @file B_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the B-Tree class.
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/trees/B_Tree.hpp"

namespace ads::trees {

//===--------------------------- NODE IMPLEMENTATION ---------------------------===//

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
B_Tree<T, MinDegree>::Node::Node(bool leaf) : is_leaf(leaf) {
  keys.reserve(MAX_KEYS);
  if (!leaf) {
    children.reserve(MAX_KEYS + 1);
  }
}

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
B_Tree<T, MinDegree>::B_Tree() : root_(nullptr), size_(0) {
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
B_Tree<T, MinDegree>::B_Tree(B_Tree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) {
  other.size_ = 0;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::operator=(B_Tree&& other) noexcept -> B_Tree& {
  if (this != &other) {
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::insert(const T& key) -> bool {
  if (!root_) {
    root_ = std::make_unique<Node>(true);
    root_->keys.push_back(key);
    root_->n = 1;
    size_++;
    return true;
  }

  // If root is full, split it.
  if (root_->n == MAX_KEYS) {
    auto new_root = std::make_unique<Node>(false);
    new_root->children.push_back(std::move(root_));
    split_child(new_root.get(), 0);
    root_ = std::move(new_root);
  }

  bool inserted = insert_non_full(root_.get(), key);
  if (inserted) {
    size_++;
  }
  return inserted;
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::remove(const T& key) -> bool {
  if (is_empty()) {
    return false;
  }

  std::vector<T> retained_keys;
  retained_keys.reserve(size_ > 0 ? size_ - 1 : 0);

  bool removed = false;
  in_order_traversal([&](const T& current_key) {
    if (!removed && current_key == key) {
      removed = true;
      return;
    }
    retained_keys.push_back(current_key);
  });

  if (!removed) {
    return false;
  }

  B_Tree rebuilt_tree;
  for (const auto& current_key : retained_keys) {
    rebuilt_tree.insert(current_key);
  }

  *this = std::move(rebuilt_tree);
  return true;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
void B_Tree<T, MinDegree>::clear() {
  root_.reset();
  size_ = 0;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::is_empty() const -> bool {
  return size_ == 0;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::size() const -> size_t {
  return size_;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::height() const -> int {
  return height_helper(root_.get());
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::contains(const T& key) const -> bool {
  return search(key);
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::find_min() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("B-Tree is empty");
  }

  const Node* node = root_.get();
  while (!node->is_leaf) {
    node = node->children.front().get();
  }
  return node->keys.front();
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::find_max() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("B-Tree is empty");
  }

  const Node* node = root_.get();
  while (!node->is_leaf) {
    node = node->children.back().get();
  }
  return node->keys.back();
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::search(const T& key) const -> bool {
  return search_helper(root_.get(), key);
}

//===----------------------- B-TREE SPECIFIC OPERATIONS ------------------------===//

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
constexpr auto B_Tree<T, MinDegree>::get_min_degree() -> int {
  return t;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
constexpr auto B_Tree<T, MinDegree>::get_max_keys() -> int {
  return MAX_KEYS;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
constexpr auto B_Tree<T, MinDegree>::get_min_keys() -> int {
  return MIN_KEYS;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::count_nodes() const -> size_t {
  return count_nodes_helper(root_.get());
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::validate_properties() const -> bool {
  if (!root_) {
    return true;
  }

  // Root can have 1 to 2t-1 keys.
  return validate_helper(root_.get(), 1, MAX_KEYS, 0);
}

//===-------------------------- TRAVERSAL OPERATIONS ---------------------------===//

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
void B_Tree<T, MinDegree>::in_order_traversal(std::function<void(const T&)> visit) const {
  in_order_helper(root_.get(), visit);
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
void B_Tree<T, MinDegree>::split_child(Node* parent, int index) {
  Node* full_child = parent->children[index].get();
  auto  new_child  = std::make_unique<Node>(full_child->is_leaf);

  new_child->n = t - 1;

  // Copy upper half of keys to new child.
  for (int j = 0; j < t - 1; j++) {
    new_child->keys.push_back(full_child->keys[j + t]);
  }

  // If not leaf, copy upper half of children.
  if (!full_child->is_leaf) {
    for (int j = 0; j < t; j++) {
      new_child->children.push_back(std::move(full_child->children[j + t]));
    }
  }

  full_child->n = t - 1;

  // Insert new child into parent.
  parent->children.insert(parent->children.begin() + index + 1, std::move(new_child));

  // Move median key up to parent.
  parent->keys.insert(parent->keys.begin() + index, full_child->keys[t - 1]);
  parent->n++;

  // Trim vectors of full_child.
  full_child->keys.resize(t - 1);
  if (!full_child->is_leaf) {
    full_child->children.resize(t);
  }
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::insert_non_full(Node* node, const T& key) -> bool {
  int i = node->n - 1;

  if (node->is_leaf) {
    // Find position and insert.
    while (i >= 0 && key < node->keys[i]) {
      i--;
    }

    // Check for duplicate.
    if (i >= 0 && key == node->keys[i]) {
      return false;
    }

    // Insert key.
    node->keys.insert(node->keys.begin() + i + 1, key);
    node->n++;
    return true;

  } else {
    // Find child to insert into.
    while (i >= 0 && key < node->keys[i]) {
      i--;
    }
    i++;

    // Check for duplicate at this level.
    if (i > 0 && key == node->keys[i - 1]) {
      return false;
    }

    // If child is full, split it.
    if (node->children[i]->n == MAX_KEYS) {
      split_child(node, i);

      // After split, determine which child to insert into.
      if (node->keys[i] < key) {
        i++;
      } else if (key == node->keys[i]) {
        return false; // Duplicate.
      }
    }

    return insert_non_full(node->children[i].get(), key);
  }
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::search_helper(const Node* node, const T& key) const -> bool {
  if (!node) {
    return false;
  }

  // Find first key >= search key.
  int i = 0;
  while (i < node->n && node->keys[i] < key) {
    i++;
  }

  // Check if key found.
  if (i < node->n && key == node->keys[i]) {
    return true;
  }

  // If leaf, key not in tree.
  if (node->is_leaf) {
    return false;
  }

  // Recurse to appropriate child.
  return search_helper(node->children[i].get(), key);
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::height_helper(const Node* node) const -> int {
  if (!node) {
    return -1;
  }
  if (node->is_leaf) {
    return 0;
  }
  return 1 + height_helper(node->children[0].get());
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::count_nodes_helper(const Node* node) const -> size_t {
  if (!node) {
    return 0;
  }

  size_t count = 1; // This node.
  if (!node->is_leaf) {
    for (int i = 0; i <= node->n; i++) {
      count += count_nodes_helper(node->children[i].get());
    }
  }
  return count;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
void B_Tree<T, MinDegree>::in_order_helper(const Node* node, std::function<void(const T&)> visit) const {
  if (!node) {
    return;
  }

  for (int i = 0; i < node->n; i++) {
    // Visit left child.
    if (!node->is_leaf) {
      in_order_helper(node->children[i].get(), visit);
    }
    // Visit key.
    visit(node->keys[i]);
  }

  // Visit rightmost child.
  if (!node->is_leaf) {
    in_order_helper(node->children[node->n].get(), visit);
  }
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::validate_helper(const Node* node, int min_keys, int max_keys, int level) const -> bool {
  if (!node) {
    return true;
  }

  // Check number of keys (root is exception for min).
  if (level > 0 && (node->n < min_keys || node->n > max_keys)) {
    return false;
  }

  // Check keys are sorted.
  for (int i = 1; i < node->n; i++) {
    if (!(node->keys[i - 1] < node->keys[i])) {
      return false;
    }
  }

  // If not leaf, check children.
  if (!node->is_leaf) {
    // Should have n+1 children.
    if (static_cast<int>(node->children.size()) != node->n + 1) {
      return false;
    }

    // Recursively validate children.
    for (int i = 0; i <= node->n; i++) {
      if (!validate_helper(node->children[i].get(), MIN_KEYS, MAX_KEYS, level + 1)) {
        return false;
      }
    }
  }

  return true;
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
