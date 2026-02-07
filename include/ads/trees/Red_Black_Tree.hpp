//===---------------------------------------------------------------------------===//
/**
 * @file Red_Black_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the Red-Black Tree class.
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//

#pragma once

#ifndef RED_BLACK_TREE_HPP
#define RED_BLACK_TREE_HPP

#include "Binary_Tree_Exception.hpp"
#include "Tree_Concepts.hpp"

#include <algorithm>
#include <functional>
#include <memory>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ads::trees {

/**
 * @brief Node color enum for Red-Black Tree
 */
enum class Color { Red, Black };

/**
 * @brief Red-Black Tree implementation.
 *
 * @details A Red-Black Tree is a self-balancing binary search tree where each node stores
 *          an extra bit representing "color" (red or black). These colors ensure the tree
 *          remains approximately balanced during insertions and deletions.
 *
 *          Red-Black Properties:
 *          1. Every node is either red or black.
 *          2. The root is always black.
 *          3. All leaves (NIL) are black.
 *          4. If a node is red, both children are black (no two reds in a row).
 *          5. Every path from root to leaf contains the same number of black nodes.
 *
 *          Performance Advantages:
 *          - O(log n) worst-case for insert/delete/search.
 *          - Fewer rotations than AVL (better for insert-heavy workloads).
 *          - Maximum height: 2 * log(n + 1).
 *          - Used in std::map, std::set, Linux kernel's CFS scheduler.
 *
 *          Comparison with AVL:
 *          - AVL: More strictly balanced (faster search).
 *          - RB:  Fewer rotations (faster insert/delete).
 *          - AVL: Max 1 rotation on insert, O(log n) on delete.
 *          - RB:  Max 2 rotations on insert, max 3 on delete.
 *
 * @tparam T Type of elements stored (must be comparable with <).
 *
 * @complexity
 * - Insert: O(log n) time, at most 2 rotations
 * - Delete: O(log n) time, at most 3 rotations
 * - Search: O(log n) time
 * - Space:  O(n) with 1 bit per node for color
 */
template <OrderedTreeElement T>
class Red_Black_Tree {
public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Construct empty Red-Black Tree.
   * @complexity Time O(1), Space O(1)
   */
  Red_Black_Tree();

  /**
   * @brief Destructor (automatic via unique_ptr).
   * @complexity Time O(n), Space O(1)
   */
  ~Red_Black_Tree() = default;

  /**
   * @brief Move constructor.
   * @param other The tree from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  Red_Black_Tree(Red_Black_Tree&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The tree from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(Red_Black_Tree&& other) noexcept -> Red_Black_Tree&;

  // Copy constructor and assignment are disabled (move-only type).
  Red_Black_Tree(const Red_Black_Tree&)                    = delete;
  auto operator=(const Red_Black_Tree&) -> Red_Black_Tree& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Insert a value into the tree.
   *
   * Performs standard BST insert, then fixes Red-Black properties.
   * New nodes are initially red.
   *
   * @param value Value to insert.
   * @return true if inserted, false if duplicate.
   * @complexity O(log n) time, at most 2 rotations
   */
  auto insert(const T& value) -> bool;

  /**
   * @brief Remove a value from the tree.
   * @param value Value to remove.
   * @return true if removed, false if value was not found.
   * @complexity O(n log n)
   * @note This implementation rebuilds the tree after removal to preserve
   *       Red-Black properties with strong exception safety.
   */
  auto remove(const T& value) -> bool;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Check if tree is empty.
   * @return true if the tree contains no elements.
   * @complexity O(1)
   */
  [[nodiscard]] auto is_empty() const -> bool;

  /**
   * @brief Get number of elements.
   * @return Number of nodes in the tree.
   * @complexity O(1)
   */
  [[nodiscard]] auto size() const -> size_t;

  /**
   * @brief Clear all elements.
   * @complexity O(n), Space O(1)
   */
  void clear();

  /**
   * @brief Get height of tree.
   * @return Height (empty tree = -1, single node = 0).
   * @complexity O(n), Space O(h)
   */
  [[nodiscard]] auto height() const -> int;

  /**
   * @brief Alias for search.
   * @complexity O(log n)
   */
  [[nodiscard]] auto contains(const T& value) const -> bool;

  /**
   * @brief Get smallest value in the tree.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity O(log n)
   */
  [[nodiscard]] auto find_min() const -> const T&;

  /**
   * @brief Get largest value in the tree.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity O(log n)
   */
  [[nodiscard]] auto find_max() const -> const T&;

  /**
   * @brief Search for a value.
   * @param value Value to search.
   * @return true if found.
   * @complexity O(log n)
   */
  [[nodiscard]] auto search(const T& value) const -> bool;

  //===------------------ RED-BLACK TREE SPECIFIC OPERATIONS -------------------===//

  /**
   * @brief Get black height of tree.
   * @return Number of black nodes from root to leaf.
   * @complexity O(log n), Space O(h)
   */
  [[nodiscard]] auto black_height() const -> int;

  /**
   * @brief Validate Red-Black Tree properties.
   *
   * Checks:
   * 1. Root is black;
   * 2. No red node has red child;
   * 3. All paths have equal black height.
   *
   * @return true if all properties satisfied.
   * @complexity O(n), Space O(h)
   */
  [[nodiscard]] auto validate_properties() const -> bool;

  //===------------------------- TRAVERSAL OPERATIONS --------------------------===//

  /**
   * @brief In-order traversal.
   * @param visit Function to call for each element.
   * @complexity O(n), Space O(h)
   */
  void in_order_traversal(std::function<void(const T&)> visit) const;

  /**
   * @brief Pre-order traversal.
   * @param visit Function to call for each element.
   * @complexity O(n), Space O(h)
   */
  void pre_order_traversal(std::function<void(const T&)> visit) const;

  /**
   * @brief Level-order traversal.
   * @param visit Function to call for each element.
   * @complexity O(n), Space O(n)
   */
  void level_order_traversal(std::function<void(const T&)> visit) const;

private:
  //===------------------------ INTERNAL NODE STRUCTURE ------------------------===//
  /**
   * @brief Internal node structure.
   */
  struct Node {
    T     data;  ///< Value stored in the node.
    Color color; ///< Color of the node (Red or Black).

    std::unique_ptr<Node> left;   ///< Left child.
    std::unique_ptr<Node> right;  ///< Right child.
    Node*                 parent; // Non-owning pointer for upward traversal.

    explicit Node(const T& val, Color col = Color::Red, Node* par = nullptr);
    explicit Node(T&& val, Color col = Color::Red, Node* par = nullptr);
  };

  //===============================================================================//
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Get color of node (nullptr is considered black).
   * @param node Node to query (can be nullptr).
   * @return Color of the node. 
   */
  static auto get_color(const Node* node) -> Color;

  /**
   * @brief Set color of node (safe for nullptr).
   * @param node Node to modify (can be nullptr).
   * @param color Color to set.
   */
  static void set_color(Node* node, Color color);

  //===--------------------------- ROTATION HELPERS ----------------------------===//

  /**
   * @brief Left rotation around node x.
   *
   *     x                y
   *    / \              / \
   *   a   y     =>     x   c
   *      / \          / \
   *     b   c        a   b
   *
   * @param x_ptr Unique pointer to node x (will be modified).
   * @return Unique pointer to new subtree root (y).
   */
  auto rotate_left(std::unique_ptr<Node> x_ptr) -> std::unique_ptr<Node>;

  /**
   * @brief Right rotation around node y.
   *
   *       y            x
   *      / \          / \
   *     x   c   =>   a   y
   *    / \              / \
   *   a   b            b   c
   *
   * @param y_ptr Unique pointer to node y (will be modified).
   * @return Unique pointer to new subtree root (x).
   */
  auto rotate_right(std::unique_ptr<Node> y_ptr) -> std::unique_ptr<Node>;

  //===--------------------------- INSERTION HELPERS ---------------------------===//

  /**
   * @brief Fix Red-Black properties after insertion.
   *
   * After standard BST insert of a RED node, this fixes violations.
   * Three cases based on uncle's color and node position.
   *
   * @param node Newly inserted node (initially red).
   */
  void insert_fixup(Node* node);

  /**
   * @brief Recursive insert helper.
   *
   * @param node Reference to the node pointer (can be modified to point to new node).
   * @param value Value to insert.
   * @return Pair of (pointer to inserted node, true if inserted, false if duplicate).
   */
  auto insert_helper(std::unique_ptr<Node>& node, const T& value, Node* parent) -> std::pair<Node*, bool>;

  //===---------------------------- SEARCH HELPERS -----------------------------===//

  /**
   * @brief Find minimum node in subtree.
   * @param node Root of subtree.
   * @return Pointer to the node with minimum value.
   */
  static auto find_min_node(Node* node) -> Node*;

  /**
   * @brief Recursive search helper.
   * @param node Current node.
   * @param value Value to search for.
   * @return true if found.
   */
  [[nodiscard]] auto search_helper(const Node* node, const T& value) const -> bool;

  //===--------------------------- TRAVERSAL HELPERS ---------------------------===//

  /**
   * @brief In-order traversal helper.
   * @param node Current node being visited.
   * @param visit Function to call for each node's value.
   */
  void in_order_helper(const Node* node, std::function<void(const T&)> visit) const;

  /**
   * @brief Pre-order traversal helper.
   * @param node Current node being visited.
   * @param visit Function to call for each node's value.
   */
  void pre_order_helper(const Node* node, std::function<void(const T&)> visit) const;

  //===-------------------------- HEIGHT/VALIDATION ----------------------------===//

  /**
   * @brief Calculate height of subtree.
   * @param node Root of subtree.
   * @return Height of subtree (-1 if node is nullptr).
   */
  [[nodiscard]] auto height_helper(const Node* node) const -> int;

  /**
   * @brief Calculate black height (number of black nodes from node to leaf),
   * @param node Current node.
   * @return Black height of subtree.
   */
  [[nodiscard]] auto black_height_helper(const Node* node) const -> int;

  /**
   * @brief Validate Red-Black properties recursively.
   * @param node Current node.
   * @return Black height of subtree, or -1 if invalid.
   */
  [[nodiscard]] auto validate_helper(const Node* node) const -> int;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<Node> root_;   ///< Root of the tree.
  size_t                size_{}; ///< Number of nodes in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Red_Black_Tree.tpp"

#endif // RED_BLACK_TREE_HPP

//===--------------------------------------------------------------------------===//
