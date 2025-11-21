//===--------------------------------------------------------------------------===//
/**
 * @file B_Tree.hpp
 * @author Costantino Lombardi
 * @brief B-Tree - Multi-way balanced search tree optimized for disk I/O
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * A B-Tree is a self-balancing search tree in which nodes can have multiple keys
 * and more than two children. It's optimized for systems that read/write large
 * blocks of data (databases, file systems).
 *
 * Properties (with minimum degree t):
 * 1. Every node has at most 2t-1 keys
 * 2. Every non-root node has at least t-1 keys
 * 3. Root has at least 1 key (if not empty)
 * 4. All leaves are at the same level
 * 5. A non-leaf node with k keys has k+1 children
 *
 * Advantages:
 * - Optimized for disk I/O (reduces seeks)
 * - Very low height: O(log_t n)
 * - Cache-friendly for large datasets
 * - Used in databases (B+ Tree variant) and file systems
 *
 * Performance:
 * - Search: O(t log_t n)
 * - Insert: O(t log_t n) with at most O(log_t n) splits
 * - Height: O(log_t n) where t is minimum degree
 *
 * Common Values:
 * - t=2: 2-3-4 Tree (each node has 1-3 keys, 2-4 children)
 * - t=3: Each node has 2-5 keys, 3-6 children
 * - t=128: Database block size optimization
 */
//===--------------------------------------------------------------------------===//
#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

namespace ads::trees {

/**
 * @brief B-Tree implementation
 *
 * @tparam T Type of elements stored (must be comparable with <)
 * @tparam t Minimum degree (t ≥ 2)
 *
 * @complexity
 * - Insert: O(t log_t n)
 * - Search: O(t log_t n)
 * - Height: O(log_t n)
 * - Space:  O(n)
 */
template <typename T, int MinDegree = 3>
class B_Tree {
private:
  static_assert(MinDegree >= 2, "Minimum degree must be at least 2");

  static constexpr int t = MinDegree;
  static constexpr int MAX_KEYS = 2 * t - 1;
  static constexpr int MIN_KEYS = t - 1;

  /**
   * @brief Internal node structure
   */
  struct Node {
    std::vector<T> keys;                               // Sorted keys
    std::vector<std::unique_ptr<Node>> children;       // Child pointers
    bool is_leaf;
    int n;  // Current number of keys

    Node(bool leaf) : is_leaf(leaf), n(0) {
      keys.reserve(MAX_KEYS);
      if (!leaf) {
        children.reserve(MAX_KEYS + 1);
      }
    }
  };

  std::unique_ptr<Node> root_;
  size_t size_;

  /**
   * @brief Search for key in subtree
   */
  bool search_helper(const Node* node, const T& key) const {
    if (!node) {
      return false;
    }

    // Find first key >= search key
    int i = 0;
    while (i < node->n && key > node->keys[i]) {
      i++;
    }

    // Check if key found
    if (i < node->n && key == node->keys[i]) {
      return true;
    }

    // If leaf, key not in tree
    if (node->is_leaf) {
      return false;
    }

    // Recurse to appropriate child
    return search_helper(node->children[i].get(), key);
  }

  /**
   * @brief Split full child of parent at given index
   *
   * Splits a full child (2t-1 keys) into two nodes with t-1 keys each,
   * moving the median key up to parent.
   *
   * @param parent Parent node (not full)
   * @param index Index of full child in parent
   */
  void split_child(Node* parent, int index) {
    Node* full_child = parent->children[index].get();
    auto new_child = std::make_unique<Node>(full_child->is_leaf);

    new_child->n = t - 1;

    // Copy upper half of keys to new child
    for (int j = 0; j < t - 1; j++) {
      new_child->keys.push_back(full_child->keys[j + t]);
    }

    // If not leaf, copy upper half of children
    if (!full_child->is_leaf) {
      for (int j = 0; j < t; j++) {
        new_child->children.push_back(std::move(full_child->children[j + t]));
      }
    }

    full_child->n = t - 1;

    // Insert new child into parent
    parent->children.insert(parent->children.begin() + index + 1, std::move(new_child));

    // Move median key up to parent
    parent->keys.insert(parent->keys.begin() + index, full_child->keys[t - 1]);
    parent->n++;

    // Trim vectors of full_child
    full_child->keys.resize(t - 1);
    if (!full_child->is_leaf) {
      full_child->children.resize(t);
    }
  }

  /**
   * @brief Insert key into non-full node
   *
   * @param node Node to insert into (guaranteed not full)
   * @param key Key to insert
   * @return true if inserted, false if duplicate
   */
  bool insert_non_full(Node* node, const T& key) {
    int i = node->n - 1;

    if (node->is_leaf) {
      // Find position and insert
      while (i >= 0 && key < node->keys[i]) {
        i--;
      }

      // Check for duplicate
      if (i >= 0 && key == node->keys[i]) {
        return false;
      }

      // Insert key
      node->keys.insert(node->keys.begin() + i + 1, key);
      node->n++;
      return true;

    } else {
      // Find child to insert into
      while (i >= 0 && key < node->keys[i]) {
        i--;
      }
      i++;

      // Check for duplicate at this level
      if (i > 0 && key == node->keys[i - 1]) {
        return false;
      }

      // If child is full, split it
      if (node->children[i]->n == MAX_KEYS) {
        split_child(node, i);

        // After split, determine which child to insert into
        if (key > node->keys[i]) {
          i++;
        } else if (key == node->keys[i]) {
          return false;  // Duplicate
        }
      }

      return insert_non_full(node->children[i].get(), key);
    }
  }

  /**
   * @brief In-order traversal helper
   */
  void in_order_helper(const Node* node, std::function<void(const T&)> visit) const {
    if (!node) {
      return;
    }

    for (int i = 0; i < node->n; i++) {
      // Visit left child
      if (!node->is_leaf) {
        in_order_helper(node->children[i].get(), visit);
      }
      // Visit key
      visit(node->keys[i]);
    }

    // Visit rightmost child
    if (!node->is_leaf) {
      in_order_helper(node->children[node->n].get(), visit);
    }
  }

  /**
   * @brief Calculate height of tree
   */
  int height_helper(const Node* node) const {
    if (!node) {
      return -1;
    }
    if (node->is_leaf) {
      return 0;
    }
    return 1 + height_helper(node->children[0].get());
  }

  /**
   * @brief Count nodes in tree
   */
  size_t count_nodes_helper(const Node* node) const {
    if (!node) {
      return 0;
    }

    size_t count = 1;  // This node
    if (!node->is_leaf) {
      for (int i = 0; i <= node->n; i++) {
        count += count_nodes_helper(node->children[i].get());
      }
    }
    return count;
  }

  /**
   * @brief Validate B-Tree properties
   */
  bool validate_helper(const Node* node, int min_keys, int max_keys, int level) const {
    if (!node) {
      return true;
    }

    // Check number of keys (root is exception for min)
    if (level > 0 && (node->n < min_keys || node->n > max_keys)) {
      return false;
    }

    // Check keys are sorted
    for (int i = 1; i < node->n; i++) {
      if (node->keys[i - 1] >= node->keys[i]) {
        return false;
      }
    }

    // If not leaf, check children
    if (!node->is_leaf) {
      // Should have n+1 children
      if (static_cast<int>(node->children.size()) != node->n + 1) {
        return false;
      }

      // Recursively validate children
      for (int i = 0; i <= node->n; i++) {
        if (!validate_helper(node->children[i].get(), MIN_KEYS, MAX_KEYS, level + 1)) {
          return false;
        }
      }
    }

    return true;
  }

public:
  /**
   * @brief Construct empty B-Tree
   */
  B_Tree() : root_(nullptr), size_(0) {}

  /**
   * @brief Destructor (automatic via unique_ptr)
   */
  ~B_Tree() = default;

  // Disable copy
  B_Tree(const B_Tree&) = delete;
  B_Tree& operator=(const B_Tree&) = delete;

  // Enable move
  B_Tree(B_Tree&&) noexcept = default;
  B_Tree& operator=(B_Tree&&) noexcept = default;

  /**
   * @brief Insert a key into the tree
   *
   * Performs proactive splitting during descent to ensure parent
   * is never full when child needs to split.
   *
   * @param key Key to insert
   * @return true if inserted, false if duplicate
   * @complexity O(t log_t n)
   */
  bool insert(const T& key) {
    if (!root_) {
      root_ = std::make_unique<Node>(true);
      root_->keys.push_back(key);
      root_->n = 1;
      size_++;
      return true;
    }

    // If root is full, split it
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

  /**
   * @brief Search for a key
   * @param key Key to search
   * @return true if found
   * @complexity O(t log_t n)
   */
  bool search(const T& key) const {
    return search_helper(root_.get(), key);
  }

  /**
   * @brief Alias for search
   */
  bool contains(const T& key) const {
    return search(key);
  }

  /**
   * @brief Get number of keys in tree
   * @complexity O(1)
   */
  size_t size() const {
    return size_;
  }

  /**
   * @brief Check if tree is empty
   * @complexity O(1)
   */
  bool is_empty() const {
    return size_ == 0;
  }

  /**
   * @brief Clear all keys
   * @complexity O(n)
   */
  void clear() {
    root_.reset();
    size_ = 0;
  }

  /**
   * @brief Get height of tree
   * @return Height (all leaves at same level)
   * @complexity O(log_t n)
   */
  int height() const {
    return height_helper(root_.get());
  }

  /**
   * @brief Get minimum degree
   * @return Minimum degree t
   */
  static constexpr int get_min_degree() {
    return t;
  }

  /**
   * @brief Get maximum keys per node
   */
  static constexpr int get_max_keys() {
    return MAX_KEYS;
  }

  /**
   * @brief Get minimum keys per node (except root)
   */
  static constexpr int get_min_keys() {
    return MIN_KEYS;
  }

  /**
   * @brief Count total number of nodes
   * @complexity O(n)
   */
  size_t count_nodes() const {
    return count_nodes_helper(root_.get());
  }

  /**
   * @brief Validate B-Tree properties
   *
   * Checks:
   * - Key counts within bounds
   * - Keys sorted within nodes
   * - All leaves at same level
   * - Correct number of children
   *
   * @return true if valid B-Tree
   * @complexity O(n)
   */
  bool validate_properties() const {
    if (!root_) {
      return true;
    }

    // Root can have 1 to 2t-1 keys
    return validate_helper(root_.get(), 1, MAX_KEYS, 0);
  }

  /**
   * @brief In-order traversal
   * @param visit Function to call for each key
   * @complexity O(n)
   */
  void in_order_traversal(std::function<void(const T&)> visit) const {
    in_order_helper(root_.get(), visit);
  }
};

}  // namespace ads::trees

//===--------------------------------------------------------------------------===//
