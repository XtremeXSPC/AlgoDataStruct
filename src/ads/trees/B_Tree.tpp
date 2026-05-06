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

// DynamicArray stores node keys and child ownership slots without STL containers.

//===--------------------------- NODE IMPLEMENTATION ---------------------------===//

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
B_Tree<T, MinDegree>::Node::Node(bool leaf) :
    keys(static_cast<size_t>(MAX_KEYS)),
    children(leaf ? 0U : static_cast<size_t>(MAX_KEYS) + 1U),
    is_leaf(leaf) {
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
  if (!root_) {
    return false;
  }

  bool removed = remove_from_node(root_.get(), key);
  if (!removed) {
    return false;
  }

  --size_;
  if (root_->n == 0) {
    if (root_->is_leaf) {
      root_.reset();
    } else {
      // The root may underflow; promoting its only child preserves equal leaf depth.
      auto old_root = std::move(root_);
      root_         = std::move(old_root->children[0]);
    }
  }

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

  int leaf_level = -1;
  return validate_helper(root_.get(), 1, MAX_KEYS, 0, nullptr, nullptr, leaf_level);
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
  parent->children.insert(static_cast<size_t>(index) + 1U, std::move(new_child));

  // Move median key up to parent.
  parent->keys.insert(static_cast<size_t>(index), full_child->keys[t - 1]);
  parent->n++;

  // Pop from the right so key shrinking does not require T to be default-initializable.
  while (full_child->keys.size() > static_cast<size_t>(t) - 1U) {
    full_child->keys.pop_back();
  }
  if (!full_child->is_leaf) {
    while (full_child->children.size() > static_cast<size_t>(t)) {
      full_child->children.pop_back();
    }
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
    node->keys.insert(static_cast<size_t>(i) + 1U, key);
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

//===----------------------------- REMOVAL HELPERS -----------------------------===//

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::find_key_index(const Node* node, const T& key) const -> int {
  int index = 0;
  while (index < node->n && node->keys[index] < key) {
    ++index;
  }
  return index;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::remove_from_node(Node* node, const T& key) -> bool {
  int index = find_key_index(node, key);

  if (index < node->n && key == node->keys[index]) {
    if (node->is_leaf) {
      remove_from_leaf(node, index);
    } else {
      remove_from_internal(node, index);
    }
    return true;
  }

  if (node->is_leaf) {
    return false;
  }

  bool descending_past_last_key = index == node->n;
  if (node->children[index]->n == MIN_KEYS) {
    // Deletion descends only into children that can spare or absorb one key safely.
    fill_child(node, index);
  }

  if (descending_past_last_key && index > node->n) {
    return remove_from_node(node->children[index - 1].get(), key);
  }
  return remove_from_node(node->children[index].get(), key);
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
void B_Tree<T, MinDegree>::remove_from_leaf(Node* node, int index) {
  node->keys.erase(static_cast<size_t>(index));
  --node->n;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
void B_Tree<T, MinDegree>::remove_from_internal(Node* node, int index) {
  T key = node->keys[index];

  if (node->children[index]->n >= t) {
    T predecessor     = predecessor_key(node, index);
    node->keys[index] = predecessor;
    remove_from_node(node->children[index].get(), predecessor);
    return;
  }

  if (node->children[index + 1]->n >= t) {
    T successor       = successor_key(node, index);
    node->keys[index] = successor;
    remove_from_node(node->children[index + 1].get(), successor);
    return;
  }

  merge_children(node, index);
  remove_from_node(node->children[index].get(), key);
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::predecessor_key(const Node* node, int index) const -> T {
  const Node* current = node->children[index].get();
  while (!current->is_leaf) {
    current = current->children[current->n].get();
  }
  return current->keys[current->n - 1];
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
auto B_Tree<T, MinDegree>::successor_key(const Node* node, int index) const -> T {
  const Node* current = node->children[index + 1].get();
  while (!current->is_leaf) {
    current = current->children[0].get();
  }
  return current->keys[0];
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
void B_Tree<T, MinDegree>::fill_child(Node* node, int index) {
  if (index > 0 && node->children[index - 1]->n >= t) {
    borrow_from_previous(node, index);
    return;
  }

  if (index < node->n && node->children[index + 1]->n >= t) {
    borrow_from_next(node, index);
    return;
  }

  if (index < node->n) {
    merge_children(node, index);
  } else {
    merge_children(node, index - 1);
  }
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
void B_Tree<T, MinDegree>::borrow_from_previous(Node* node, int index) {
  Node* child   = node->children[index].get();
  Node* sibling = node->children[index - 1].get();

  child->keys.insert(0, node->keys[index - 1]);
  if (!child->is_leaf) {
    child->children.insert(0, std::move(sibling->children.back()));
    sibling->children.pop_back();
  }

  node->keys[index - 1] = sibling->keys[sibling->n - 1];
  sibling->keys.pop_back();

  ++child->n;
  --sibling->n;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
void B_Tree<T, MinDegree>::borrow_from_next(Node* node, int index) {
  Node* child   = node->children[index].get();
  Node* sibling = node->children[index + 1].get();

  child->keys.push_back(node->keys[index]);
  if (!child->is_leaf) {
    child->children.push_back(std::move(sibling->children.front()));
    sibling->children.erase(0);
  }

  node->keys[index] = sibling->keys[0];
  sibling->keys.erase(0);

  ++child->n;
  --sibling->n;
}

template <OrderedTreeElement T, int MinDegree>
  requires ValidBTreeDegree<MinDegree>
void B_Tree<T, MinDegree>::merge_children(Node* node, int index) {
  Node* child   = node->children[index].get();
  Node* sibling = node->children[index + 1].get();

  child->keys.push_back(node->keys[index]);
  for (int i = 0; i < sibling->n; ++i) {
    child->keys.push_back(std::move(sibling->keys[i]));
  }

  if (!child->is_leaf) {
    for (int i = 0; i <= sibling->n; ++i) {
      child->children.push_back(std::move(sibling->children[i]));
    }
  }

  child->n += sibling->n + 1;
  node->keys.erase(static_cast<size_t>(index));
  node->children.erase(static_cast<size_t>(index) + 1U);
  --node->n;
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
auto B_Tree<T, MinDegree>::validate_helper(
    const Node* node,
    int         min_keys,
    int         max_keys,
    int         level,
    const T*    lower_bound,
    const T*    upper_bound,
    int&        leaf_level) const -> bool {
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

  for (int i = 0; i < node->n; ++i) {
    if (lower_bound != nullptr && !(*lower_bound < node->keys[i])) {
      return false;
    }
    if (upper_bound != nullptr && !(node->keys[i] < *upper_bound)) {
      return false;
    }
  }

  if (node->is_leaf) {
    if (leaf_level == -1) {
      leaf_level = level;
    }
    return leaf_level == level;
  }

  // Internal nodes must have exactly one more child than key.
  if (static_cast<int>(node->children.size()) != node->n + 1) {
    return false;
  }

  for (int i = 0; i <= node->n; ++i) {
    const T* child_lower_bound = (i == 0) ? lower_bound : &node->keys[i - 1];
    const T* child_upper_bound = (i == node->n) ? upper_bound : &node->keys[i];

    if (node->children[i] == nullptr
        || !validate_helper(
            node->children[i].get(), MIN_KEYS, MAX_KEYS, level + 1, child_lower_bound, child_upper_bound, leaf_level)) {
      return false;
    }
  }

  return true;
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
