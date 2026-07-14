//===---------------------------------------------------------------------------===//
/**
 * @file BPlus_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the BPlusTree class.
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../../include/ads/trees/search/BPlus_Tree.hpp"

#include <cmath>

namespace ads::trees {

//===----- NODE IMPLEMENTATION -------------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
BPlusTree<Key, Value, MinDegree>::Node::Node(bool leaf) :
    is_leaf(leaf),
    keys(leaf ? 0U : static_cast<size_t>(MAX_KEYS)),
    children(leaf ? 0U : static_cast<size_t>(MAX_KEYS) + 1U),
    entries(leaf ? static_cast<size_t>(MAX_KEYS) : 0U) {
}

//===----- ITERATOR IMPLEMENTATION ---------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::iterator::operator*() const -> reference {
  return leaf_->entries[index_];
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::iterator::operator->() const -> pointer {
  return &leaf_->entries[index_];
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::iterator::operator++() -> iterator& {
  ++index_;
  if (index_ >= leaf_->entries.size()) {
    leaf_  = leaf_->next;
    index_ = 0;
  }
  return *this;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::iterator::operator++(int) -> iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::iterator::operator==(const iterator& other) const -> bool {
  return leaf_ == other.leaf_ && index_ == other.index_;
}

//===----- CONSTRUCTORS AND ASSIGNMENT -----------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
BPlusTree<Key, Value, MinDegree>::BPlusTree() noexcept : root_(nullptr), size_(0) {
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
BPlusTree<Key, Value, MinDegree>::BPlusTree(const DynamicArray<Entry>& entries) requires std::copy_constructible<Value>
    : root_(nullptr), size_(0) {
  try {
    for (const Entry& entry : entries) {
      insert(entry.key, entry.value);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
template <std::input_iterator InputIt>
requires std::constructible_from<typename BPlusTree<Key, Value, MinDegree>::Entry, std::iter_reference_t<InputIt>>
BPlusTree<Key, Value, MinDegree>::BPlusTree(InputIt first, InputIt last) : root_(nullptr), size_(0) {
  try {
    for (; first != last; ++first) {
      Entry entry(*first);
      insert(entry.key, std::move(entry.value));
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
BPlusTree<Key, Value, MinDegree>::BPlusTree(BPlusTree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) {
  other.size_ = 0;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::operator=(BPlusTree&& other) noexcept -> BPlusTree& {
  if (this != &other) {
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===----- MUTATION OPERATIONS -------------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::insert(const Key& key, const Value& value) -> bool requires std::copy_constructible<Value>
{
  return insert_impl(key, value);
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::insert(const Key& key, Value&& value) -> bool requires std::move_constructible<Value>
{
  return insert_impl(key, std::move(value));
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
template <typename V>
auto BPlusTree<Key, Value, MinDegree>::insert_impl(const Key& key, V&& value) -> bool {
  validate_key(key);

  if (!root_) {
    auto new_root = std::make_unique<Node>(true);
    new_root->entries.push_back(Entry{key, std::forward<V>(value)});
    root_ = std::move(new_root);
    ++size_;
    return true;
  }

  // Split a full root first, growing the tree by one level.
  const bool root_full = root_->is_leaf ? key_count(root_.get()) == MAX_KEYS : root_->keys.size() == static_cast<size_t>(MAX_KEYS);
  if (root_full) {
    auto        new_root = std::make_unique<Node>(false);
    SplitResult split    = split_node(root_.get());
    new_root->children.push_back(std::move(root_));
    new_root->children.push_back(std::move(split.right));
    new_root->keys.push_back(std::move(split.separator));
    root_ = std::move(new_root);
  }

  const bool inserted = insert_non_full(root_.get(), key, std::forward<V>(value));
  if (inserted) {
    ++size_;
  }
  return inserted;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::remove(const Key& key) -> bool {
  validate_key(key);
  if (!root_) {
    return false;
  }

  const bool removed = remove_from_node(root_.get(), key);

  // Collapse an internal root left with no separators, or drop an emptied leaf root.
  if (!root_->is_leaf && root_->keys.size() == 0) {
    auto old_root = std::move(root_);
    root_         = std::move(old_root->children[0]);
  } else if (root_->is_leaf && root_->entries.size() == 0) {
    root_.reset();
  }

  if (removed) {
    --size_;
  }
  return removed;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::clear() noexcept -> void {
  root_.reset();
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::size() const noexcept -> size_type {
  return size_;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::height() const noexcept -> int {
  int         levels = 0;
  const Node* node   = root_.get();
  if (!node) {
    return 0;
  }
  while (!node->is_leaf) {
    node = node->children[0].get();
    ++levels;
  }
  return levels;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::contains(const Key& key) const -> bool {
  return find(key) != nullptr;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::find(const Key& key) const -> const Value* {
  validate_key(key);
  const Node* node = root_.get();
  if (!node) {
    return nullptr;
  }
  while (!node->is_leaf) {
    node = node->children[child_index(node, key)].get();
  }
  for (size_t i = 0; i < node->entries.size(); ++i) {
    if (keys_equal(node->entries[i].key, key)) {
      return &node->entries[i].value;
    }
    if (key < node->entries[i].key) {
      break;
    }
  }
  return nullptr;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::find_min() const -> const Entry& {
  if (!root_) {
    throw EmptyTreeException("B+ Tree is empty");
  }
  const Node* node = root_.get();
  while (!node->is_leaf) {
    node = node->children.front().get();
  }
  return node->entries.front();
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::find_max() const -> const Entry& {
  if (!root_) {
    throw EmptyTreeException("B+ Tree is empty");
  }
  const Node* node = root_.get();
  while (!node->is_leaf) {
    node = node->children.back().get();
  }
  return node->entries.back();
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::range(const Key& low, const Key& high, const visitor_type& visit) const -> void {
  validate_key(low);
  validate_key(high);
  const Node* node = root_.get();
  if (!node) {
    return;
  }
  while (!node->is_leaf) {
    node = node->children[child_index(node, low)].get();
  }

  while (node) {
    for (size_t i = 0; i < node->entries.size(); ++i) {
      const Entry& entry = node->entries[i];
      if (entry.key < low) {
        continue;
      }
      if (high < entry.key) {
        return;
      }
      visit(entry.key, entry.value);
    }
    node = node->next;
  }
}

//===----- TRAVERSAL OPERATIONS ------------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::in_order_traversal(const visitor_type& visit) const -> void {
  const Node* leaf = leftmost_leaf();
  while (leaf) {
    for (size_t i = 0; i < leaf->entries.size(); ++i) {
      visit(leaf->entries[i].key, leaf->entries[i].value);
    }
    leaf = leaf->next;
  }
}

//===----- VALIDATION ----------------------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::validate_properties() const -> bool {
  if (!root_) {
    return size_ == 0;
  }

  int leaf_level = -1;
  if (!validate_helper(root_.get(), true, 0, nullptr, nullptr, leaf_level)) {
    return false;
  }

  // The leaf chain must enumerate every record in strictly ascending order.
  const Node* leaf     = leftmost_leaf();
  const Key*  previous = nullptr;
  size_type   count    = 0;
  while (leaf) {
    for (size_t i = 0; i < leaf->entries.size(); ++i) {
      if (previous != nullptr && !(*previous < leaf->entries[i].key)) {
        return false;
      }
      previous = &leaf->entries[i].key;
      ++count;
    }
    leaf = leaf->next;
  }
  return count == size_;
}

//===----- B+ TREE SPECIFIC OPERATIONS -----------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
constexpr auto BPlusTree<Key, Value, MinDegree>::get_min_degree() -> int {
  return t;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
constexpr auto BPlusTree<Key, Value, MinDegree>::get_max_keys() -> int {
  return MAX_KEYS;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
constexpr auto BPlusTree<Key, Value, MinDegree>::get_min_keys() -> int {
  return MIN_KEYS;
}

//===----- ITERATOR OPERATIONS -------------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::begin() const -> iterator {
  const Node* leaf = leftmost_leaf();
  if (!leaf) {
    return iterator();
  }
  return iterator(leaf, 0);
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::end() const -> iterator {
  return iterator();
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::cbegin() const -> iterator {
  return begin();
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::cend() const -> iterator {
  return iterator();
}

//=================================================================================//
//===----- NAVIGATION HELPERS --------------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::key_count(const Node* node) noexcept -> int {
  return static_cast<int>(node->is_leaf ? node->entries.size() : node->keys.size());
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::child_index(const Node* node, const Key& key) noexcept -> int {
  // Route to the first child whose separator is strictly greater than key.
  int i = 0;
  while (i < static_cast<int>(node->keys.size()) && !(key < node->keys[static_cast<size_t>(i)])) {
    ++i;
  }
  return i;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::keys_equal(const Key& lhs, const Key& rhs) noexcept -> bool {
  return !(lhs < rhs) && !(rhs < lhs);
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::is_valid_key(const Key& key) noexcept -> bool {
  if constexpr (std::floating_point<Key>) {
    return !std::isnan(key);
  }
  return true;
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::validate_key(const Key& key) -> void {
  if (!is_valid_key(key)) {
    throw BinaryTreeException("B+ Tree: keys must be ordered values, not NaN");
  }
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::leftmost_leaf() const noexcept -> const Node* {
  const Node* node = root_.get();
  if (!node) {
    return nullptr;
  }
  while (!node->is_leaf) {
    node = node->children.front().get();
  }
  return node;
}

//===----- INSERTION HELPERS ---------------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
void BPlusTree<Key, Value, MinDegree>::split_child(Node* parent, int index) {
  // Deletion may have shrunk either DynamicArray. Complete every allocation
  // before split_node mutates the child or its leaf-chain link.
  parent->children.reserve(parent->children.size() + 1U);
  parent->keys.reserve(parent->keys.size() + 1U);

  Node*       full_child = parent->children[static_cast<size_t>(index)].get();
  SplitResult split      = split_node(full_child);
  parent->children.insert(static_cast<size_t>(index) + 1U, std::move(split.right));
  parent->keys.insert(static_cast<size_t>(index), std::move(split.separator));
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::split_node(Node* node) -> SplitResult {
  auto right = std::make_unique<Node>(node->is_leaf);

  if (node->is_leaf) {
    Key separator = node->entries[static_cast<size_t>(t)].key;
    // Move the upper t-1 records to the new leaf; the left leaf keeps the lower t.
    for (int j = 0; j < t - 1; ++j) {
      right->entries.push_back(std::move(node->entries[static_cast<size_t>(j + t)]));
    }
    while (node->entries.size() > static_cast<size_t>(t)) {
      node->entries.pop_back();
    }

    // Splice the new leaf into the chain.
    right->next = node->next;
    node->next  = right.get();
    return SplitResult{std::move(separator), std::move(right)};
  }

  // Internal split: the median key moves up (classic B-Tree behaviour).
  Key separator = std::move(node->keys[static_cast<size_t>(t - 1)]);
  for (int j = 0; j < t - 1; ++j) {
    right->keys.push_back(std::move(node->keys[static_cast<size_t>(j + t)]));
  }
  for (int j = 0; j < t; ++j) {
    right->children.push_back(std::move(node->children[static_cast<size_t>(j + t)]));
  }

  // Pop from the right so shrinking never requires Key to be default-initializable.
  while (node->keys.size() > static_cast<size_t>(t - 1)) {
    node->keys.pop_back();
  }
  while (node->children.size() > static_cast<size_t>(t)) {
    node->children.pop_back();
  }
  return SplitResult{std::move(separator), std::move(right)};
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
template <typename V>
auto BPlusTree<Key, Value, MinDegree>::insert_non_full(Node* node, const Key& key, V&& value) -> bool {
  if (node->is_leaf) {
    size_t pos = 0;
    while (pos < node->entries.size() && node->entries[pos].key < key) {
      ++pos;
    }
    if (pos < node->entries.size() && keys_equal(node->entries[pos].key, key)) {
      return false; // Duplicate key.
    }
    node->entries.insert(pos, Entry{key, std::forward<V>(value)});
    return true;
  }

  int ci = child_index(node, key);
  if (key_count(node->children[static_cast<size_t>(ci)].get()) == MAX_KEYS) {
    split_child(node, ci);
    // After the split, node->keys[ci] is the new separator; descend on its right
    // side when the key is not smaller than it.
    if (!(key < node->keys[static_cast<size_t>(ci)])) {
      ++ci;
    }
  }
  return insert_non_full(node->children[static_cast<size_t>(ci)].get(), key, std::forward<V>(value));
}

//===----- REMOVAL HELPERS -----------------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::remove_from_node(Node* node, const Key& key) -> bool {
  if (node->is_leaf) {
    for (size_t i = 0; i < node->entries.size(); ++i) {
      if (keys_equal(node->entries[i].key, key)) {
        node->entries.erase(i);
        return true;
      }
      if (key < node->entries[i].key) {
        return false;
      }
    }
    return false;
  }

  int ci = child_index(node, key);
  if (key_count(node->children[static_cast<size_t>(ci)].get()) == MIN_KEYS) {
    fill_child(node, ci);
    // A merge may have shifted separators; re-resolve which child now owns the key.
    ci = child_index(node, key);
    if (ci >= static_cast<int>(node->children.size())) {
      ci = static_cast<int>(node->children.size()) - 1;
    }
  }
  return remove_from_node(node->children[static_cast<size_t>(ci)].get(), key);
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
void BPlusTree<Key, Value, MinDegree>::fill_child(Node* node, int index) {
  if (index > 0 && key_count(node->children[static_cast<size_t>(index - 1)].get()) >= t) {
    borrow_from_previous(node, index);
    return;
  }
  if (index < static_cast<int>(node->keys.size()) && key_count(node->children[static_cast<size_t>(index + 1)].get()) >= t) {
    borrow_from_next(node, index);
    return;
  }
  if (index < static_cast<int>(node->keys.size())) {
    merge_children(node, index);
  } else {
    merge_children(node, index - 1);
  }
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
void BPlusTree<Key, Value, MinDegree>::borrow_from_previous(Node* node, int index) {
  Node* child   = node->children[static_cast<size_t>(index)].get();
  Node* sibling = node->children[static_cast<size_t>(index - 1)].get();

  if (child->is_leaf) {
    Key separator = sibling->entries.back().key;
    child->entries.insert(0, std::move(sibling->entries[sibling->entries.size() - 1]));
    sibling->entries.pop_back();
    node->keys[static_cast<size_t>(index - 1)] = std::move(separator);
    return;
  }

  child->keys.insert(0, std::move(node->keys[static_cast<size_t>(index - 1)]));
  child->children.insert(0, std::move(sibling->children[sibling->children.size() - 1]));
  sibling->children.pop_back();
  node->keys[static_cast<size_t>(index - 1)] = std::move(sibling->keys[sibling->keys.size() - 1]);
  sibling->keys.pop_back();
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
void BPlusTree<Key, Value, MinDegree>::borrow_from_next(Node* node, int index) {
  Node* child   = node->children[static_cast<size_t>(index)].get();
  Node* sibling = node->children[static_cast<size_t>(index + 1)].get();

  if (child->is_leaf) {
    Key separator = sibling->entries[1].key;
    child->entries.push_back(std::move(sibling->entries[0]));
    sibling->entries.erase(0);
    node->keys[static_cast<size_t>(index)] = std::move(separator);
    return;
  }

  child->keys.push_back(std::move(node->keys[static_cast<size_t>(index)]));
  child->children.push_back(std::move(sibling->children[0]));
  sibling->children.erase(0);
  node->keys[static_cast<size_t>(index)] = std::move(sibling->keys[0]);
  sibling->keys.erase(0);
}

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
void BPlusTree<Key, Value, MinDegree>::merge_children(Node* node, int index) {
  Node* child   = node->children[static_cast<size_t>(index)].get();
  Node* sibling = node->children[static_cast<size_t>(index + 1)].get();

  if (child->is_leaf) {
    for (size_t i = 0; i < sibling->entries.size(); ++i) {
      child->entries.push_back(std::move(sibling->entries[i]));
    }
    child->next = sibling->next; // Preserve the leaf chain across the merge.
  } else {
    child->keys.push_back(std::move(node->keys[static_cast<size_t>(index)]));
    for (size_t i = 0; i < sibling->keys.size(); ++i) {
      child->keys.push_back(std::move(sibling->keys[i]));
    }
    for (size_t i = 0; i < sibling->children.size(); ++i) {
      child->children.push_back(std::move(sibling->children[i]));
    }
  }

  node->keys.erase(static_cast<size_t>(index));
  node->children.erase(static_cast<size_t>(index) + 1U);
}

//===----- VALIDATION HELPER ---------------------------------------------------===//

template <BPlusKey Key, BPlusValue Value, int MinDegree>
requires ValidBTreeDegree<MinDegree>
auto BPlusTree<Key, Value, MinDegree>::validate_helper(
    const Node* node, bool is_root, int level, const Key* lower_bound, const Key* upper_bound, int& leaf_level) const -> bool {
  const int count = key_count(node);

  // Key-count bounds: the root is exempt from the lower bound.
  if (is_root) {
    if (!node->is_leaf && count < 1) {
      return false;
    }
    if (count > MAX_KEYS) {
      return false;
    }
  } else if (count < MIN_KEYS || count > MAX_KEYS) {
    return false;
  }

  if (node->is_leaf) {
    for (size_t i = 0; i < node->entries.size(); ++i) {
      if (!is_valid_key(node->entries[i].key)) {
        return false;
      }
      if (i > 0 && !(node->entries[i - 1].key < node->entries[i].key)) {
        return false;
      }
      if (lower_bound != nullptr && node->entries[i].key < *lower_bound) {
        return false;
      }
      if (upper_bound != nullptr && !(node->entries[i].key < *upper_bound)) {
        return false;
      }
    }
    if (leaf_level == -1) {
      leaf_level = level;
    }
    return leaf_level == level;
  }

  // Separators must be sorted and lie within the node's inherited bounds.
  for (size_t i = 0; i < node->keys.size(); ++i) {
    if (!is_valid_key(node->keys[i])) {
      return false;
    }
    if (i > 0 && !(node->keys[i - 1] < node->keys[i])) {
      return false;
    }
    if (lower_bound != nullptr && node->keys[i] < *lower_bound) {
      return false;
    }
    if (upper_bound != nullptr && !(node->keys[i] < *upper_bound)) {
      return false;
    }
  }

  if (node->children.size() != node->keys.size() + 1U) {
    return false;
  }

  for (size_t i = 0; i < node->children.size(); ++i) {
    const Key* child_lower = (i == 0) ? lower_bound : &node->keys[i - 1];
    const Key* child_upper = (i == node->keys.size()) ? upper_bound : &node->keys[i];
    if (node->children[i] == nullptr || !validate_helper(node->children[i].get(), false, level + 1, child_lower, child_upper, leaf_level)) {
      return false;
    }
  }
  return true;
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
