//===---------------------------------------------------------------------------===//
/**
 * @file B_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the B-Tree class.
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef B_TREE_HPP
#define B_TREE_HPP

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Binary_Tree_Exception.hpp"

namespace ads::trees {

/**
 * @brief B-Tree implementation
 *
 * @details A B-Tree is a self-balancing search tree in which nodes can have multiple keys
 *          and more than two children. It's optimized for systems that read/write large
 *          blocks of data (databases, file systems).
 *
 *          Properties (with minimum degree t):
 *          1. Every node has at most 2t-1 keys.
 *          2. Every non-root node has at least t-1 keys.
 *          3. Root has at least 1 key (if not empty).
 *          4. All leaves are at the same level.
 *          5. A non-leaf node with k keys has k+1 children.
 *
 *          Advantages:
 *          - Optimized for disk I/O (reduces seeks).
 *          - Very low height: O(log_t n).
 *          - Cache-friendly for large datasets.
 *          - Used in databases (B+ Tree variant) and file systems.
 *
 *          Performance:
 *          - Search: O(t log_t n).
 *          - Insert: O(t log_t n) with at most O(log_t n) splits.
 *          - Height: O(log_t n) where t is minimum degree.
 *
 *          Common Values:
 *          - t=2: 2-3-4 Tree (each node has 1-3 keys, 2-4 children).
 *          - t=3: Each node has 2-5 keys, 3-6 children.
 *          - t=128: Database block size optimization.
 *
 * @tparam T Type of elements stored (must be comparable with <).
 * @tparam t Minimum degree (t >= 2).
 *
 * @complexity
 * - Insert: O(t log_t n)
 * - Search: O(t log_t n)
 * - Height: O(log_t n)
 * - Space:  O(n)
 */
template <typename T, int MinDegree = 3>
class B_Tree {
public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Construct empty B-Tree.
   * @complexity Time O(1), Space O(1)
   */
  B_Tree();

  /**
   * @brief Destructor (automatic via unique_ptr).
   * @complexity Time O(n), Space O(1)
   */
  ~B_Tree() = default;

  /**
   * @brief Move constructor.
   * @param other The tree from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  B_Tree(B_Tree&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The tree from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(B_Tree&& other) noexcept -> B_Tree&;

  // Copy constructor and assignment are disabled (move-only type).
  B_Tree(const B_Tree&)                    = delete;
  auto operator=(const B_Tree&) -> B_Tree& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Insert a key into the tree.
   *
   * Performs proactive splitting during descent to ensure parent.
   * is never full when child needs to split.
   *
   * @param key Key to insert.
   * @return true if inserted, false if duplicate.
   * @complexity O(t log_t n)
   */
  auto insert(const T& key) -> bool;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Clear all keys.
   * @complexity O(n), Space O(1)
   */
  void clear();

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Check if tree is empty.
   * @return true if the tree contains no elements.
   * @complexity O(1)
   */
  [[nodiscard]] auto is_empty() const -> bool;

  /**
   * @brief Get number of keys in tree.
   * @return Number of keys in the tree.
   * @complexity O(1)
   */
  [[nodiscard]] auto size() const -> size_t;

  /**
   * @brief Get height of tree.
   * @return Height (all leaves at same level).
   * @complexity O(log_t n)
   */
  [[nodiscard]] auto height() const -> int;

  /**
   * @brief Alias for search.
   * @param key Key to search.
   * @return true if found.
   * @complexity O(t log_t n)
   */
  [[nodiscard]] auto contains(const T& key) const -> bool;

  /**
   * @brief Get smallest key in the tree.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity O(log_t n)
   */
  [[nodiscard]] auto find_min() const -> const T&;

  /**
   * @brief Get largest key in the tree.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity O(log_t n)
   */
  [[nodiscard]] auto find_max() const -> const T&;

  /**
   * @brief Search for a key.
   * @param key Key to search.
   * @return true if found.
   * @complexity O(t log_t n)
   */
  [[nodiscard]] auto search(const T& key) const -> bool;

  //===----------------------- B-TREE SPECIFIC OPERATIONS ----------------------===//

  /**
   * @brief Get minimum degree.
   * @return Minimum degree t.
   * @complexity Time O(1), Space O(1)
   */
  static constexpr auto get_min_degree() -> int;

  /**
   * @brief Get maximum keys per node.
   * @complexity Time O(1), Space O(1)
   */
  static constexpr auto get_max_keys() -> int;

  /**
   * @brief Get minimum keys per node (except root).
   * @complexity Time O(1), Space O(1)
   */
  static constexpr auto get_min_keys() -> int;

  /**
   * @brief Count total number of nodes.
   * @complexity O(n), Space O(h)
   */
  [[nodiscard]] auto count_nodes() const -> size_t;

  /**
   * @brief Validate B-Tree properties.
   *
   * Checks:
   * - Key counts within bounds;
   * - Keys sorted within nodes;
   * - All leaves at same level;
   * - Correct number of children.
   *
   * @return true if valid B-Tree.
   * @complexity O(n), Space O(h)
   */
  [[nodiscard]] auto validate_properties() const -> bool;

  //===------------------------- TRAVERSAL OPERATIONS --------------------------===//

  /**
   * @brief In-order traversal.
   * @param visit Function to call for each key.
   * @complexity O(n), Space O(h)
   */
  void in_order_traversal(std::function<void(const T&)> visit) const;

private:
  //====------------------------------ CONSTANTS -------------------------------===//

  // Check minimum degree.
  static_assert(MinDegree >= 2, "Minimum degree must be at least 2");

  // B-Tree properties.
  static constexpr int t        = MinDegree;
  static constexpr int MAX_KEYS = 2 * t - 1;
  static constexpr int MIN_KEYS = t - 1;

  //===------------------------ INTERNAL NODE STRUCTURE ------------------------===//

  /**
   * @brief Internal node structure.
   */
  struct Node {
    std::vector<T>                     keys;     // Sorted keys.
    std::vector<std::unique_ptr<Node>> children; // Child pointers.

    bool is_leaf; // True if leaf node.
    int  n = 0;   // Current number of keys.

    explicit Node(bool leaf);
  };

  //===============================================================================//
  //===------------------------- PRIVATE HELPER METHODS ------------------------===//

  /**
   * @brief Split full child of parent at given index.
   *
   * Splits a full child (2t-1 keys) into two nodes with t-1 keys each,
   * moving the median key up to parent.
   *
   * @param parent Parent node (not full).
   * @param index Index of full child in parent.
   */
  void split_child(Node* parent, int index);

  /**
   * @brief Insert key into non-full node.
   *
   * @param node Node to insert into (guaranteed not full).
   * @param key Key to insert.
   * @return true if inserted, false if duplicate.
   */
  auto insert_non_full(Node* node, const T& key) -> bool;

  /**
   * @brief Search for key in subtree.
   * @param node Current node.
   * @param key Key to search.
   * @return true if found.
   */
  [[nodiscard]] auto search_helper(const Node* node, const T& key) const -> bool;

  /**
   * @brief Calculate height of tree.
   * @param node Root of subtree.
   * @return Height of subtree (-1 if node is nullptr).
   */
  [[nodiscard]] auto height_helper(const Node* node) const -> int;

  /**
   * @brief Count nodes in tree.
   * @param node Current node.
   * @return Number of nodes in subtree.
   */
  [[nodiscard]] auto count_nodes_helper(const Node* node) const -> size_t;

  /**
   * @brief In-order traversal helper.
   * @param node Current node being visited.
   * @param visit Function to call for each key.
   */
  void in_order_helper(const Node* node, std::function<void(const T&)> visit) const;

  /**
   * @brief Validate B-Tree properties.
   * @param node Current node.
   * @param min_keys Minimum keys required for this node.
   * @param max_keys Maximum keys allowed for this node.
   * @param level Current level in the tree.
   * @return true if subtree is valid.
   */
  [[nodiscard]] auto validate_helper(const Node* node, int min_keys, int max_keys, int level) const -> bool;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<Node> root_; ///< Root node.
  size_t                size_; ///< Number of keys in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/B_Tree.tpp"

#endif // B_TREE_HPP

//===---------------------------------------------------------------------------===//
