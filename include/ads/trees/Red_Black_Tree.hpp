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

#include "../arrays/Dynamic_Array.hpp"
#include "../queues/Linked_Queue.hpp"
#include "Binary_Tree_Exception.hpp"
#include "Tree_Concepts.hpp"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>

namespace ads::trees {

/**
 * @brief Node color enum for Red-Black Tree
 */
enum class Color : std::uint8_t { Red, Black };

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
 *            - O(log n) worst-case for insert/delete/search.
 *            - Fewer rotations than AVL (better for insert-heavy workloads).
 *            - Maximum height: 2 * log(n + 1).
 *            - Used in std::map, std::set, Linux kernel's CFS scheduler.
 *
 *          Comparison with AVL:
 *            - AVL: More strictly balanced (faster search).
 *            - RB:  Fewer rotations (faster insert/delete).
 *            - AVL: Max 1 rotation on insert, O(log n) on delete.
 *            - RB:  Max 2 rotations on insert, max 3 on delete.
 *
 * @tparam T Type of elements stored (must be comparable with <).
 *
 * @complexity
 *   - Insert: O(log n) time, at most 2 rotations
 *   - Delete: O(log n) time, at most 3 rotations
 *   - Search: O(log n) time
 *   - Space:  O(n) with 1 bit per node for color
 */
template <OrderedTreeElement T>
class Red_Black_Tree {
private:
  struct Node;

public:
  using value_type = T;
  using size_type  = size_t;

  //===---------------------------- ITERATOR CLASS -----------------------------===//
  /**
   * @brief Forward iterator for in-order traversal of the Red-Black Tree.
   *
   * @details The iterator walks values in ascending order. It stores only the
   *          current node and uses parent links to find each in-order successor.
   */
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    iterator() = default;

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> iterator&;
    auto operator++(int) -> iterator;
    auto operator==(const iterator& other) const -> bool;

  private:
    friend class Red_Black_Tree<T>;

    Node* current_ = nullptr;

    explicit iterator(Node* root);

    static auto leftmost(Node* node) noexcept -> Node*;
    static auto successor(Node* node) noexcept -> Node*;
  };

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
  auto insert(const T& value) -> bool
    requires std::copy_constructible<T>;

  /**
   * @brief Insert a moved value into the tree.
   * @param value Value to move into the tree.
   * @return true if inserted, false if duplicate.
   * @complexity O(log n) time, at most 2 rotations
   */
  auto insert(T&& value) -> bool;

  /**
   * @brief Constructs an element in-place in the tree.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to the constructor of T.
   * @return true if inserted, false if duplicate.
   * @complexity O(log n) time, at most 2 rotations
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> bool
    requires std::constructible_from<T, Args...>;

  /**
   * @brief Remove a value from the tree.
   * @param value Value to remove.
   * @return true if removed, false if value was not found.
   * @complexity O(log n)
   */
  auto remove(const T& value) -> bool;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Check if tree is empty.
   * @return true if the tree contains no elements.
   * @complexity O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Get number of elements.
   * @return Number of nodes in the tree.
   * @complexity O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Clear all elements.
   * @complexity O(n), Space O(1)
   */
  void clear() noexcept;

  /**
   * @brief Get height of tree.
   * @return Height (empty tree = -1, single node = 0).
   * @complexity O(n), Space O(h)
   */
  [[nodiscard]] auto height() const noexcept -> int;

  /**
   * @brief Checks whether a value exists in the tree.
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
  void in_order_traversal(const std::function<void(const T&)>& visit) const;

  /**
   * @brief Pre-order traversal.
   * @param visit Function to call for each element.
   * @complexity O(n), Space O(h)
   */
  void pre_order_traversal(const std::function<void(const T&)>& visit) const;

  /**
   * @brief Post-order traversal.
   * @param visit Function to call for each element.
   * @complexity O(n), Space O(h)
   */
  void post_order_traversal(const std::function<void(const T&)>& visit) const;

  /**
   * @brief Level-order traversal.
   * @param visit Function to call for each element.
   * @complexity O(n), Space O(n)
   */
  void level_order_traversal(const std::function<void(const T&)>& visit) const;

  //===-------------------------- ITERATOR OPERATIONS --------------------------===//

  /**
   * @brief Returns an iterator to the smallest element.
   * @return Iterator pointing to the first in-order element.
   * @complexity O(log n), Space O(1)
   */
  auto begin() -> iterator;

  /**
   * @brief Returns a const iterator to the smallest element.
   * @return Iterator pointing to the first in-order element.
   * @complexity O(log n), Space O(1)
   */
  auto begin() const -> iterator;

  /**
   * @brief Returns a past-the-end iterator.
   * @return Iterator representing the end of in-order traversal.
   * @complexity O(1), Space O(1)
   */
  auto end() -> iterator;

  /**
   * @brief Returns a const past-the-end iterator.
   * @return Iterator representing the end of in-order traversal.
   * @complexity O(1), Space O(1)
   */
  auto end() const -> iterator;

  /**
   * @brief Returns a const iterator to the smallest element.
   * @return Iterator pointing to the first in-order element.
   * @complexity O(log n), Space O(1)
   */
  auto cbegin() const -> iterator;

  /**
   * @brief Returns a const past-the-end iterator.
   * @return Iterator representing the end of in-order traversal.
   * @complexity O(1), Space O(1)
   */
  auto cend() const -> iterator;

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

    explicit Node(const T& val, Color col = Color::Red, Node* par = nullptr)
      requires std::copy_constructible<T>;
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
  template <typename U>
  auto insert_helper(std::unique_ptr<Node>& node, U&& value, Node* parent) -> std::pair<Node*, bool>;

  //===---------------------------- REMOVAL HELPERS ----------------------------===//

  /**
   * @brief Find a node by value.
   * @param value Value to search for.
   * @return Pointer to matching node, or nullptr.
   */
  [[nodiscard]] auto find_node(const T& value) const -> Node*;

  /**
   * @brief Returns the unique_ptr slot owning the given node.
   * @param node Non-null node owned by this tree.
   * @return Reference to root_ or the matching parent child slot.
   */
  auto owning_link(Node* node) -> std::unique_ptr<Node>&;

  /**
   * @brief Rotate a subtree left in-place at the owning slot of node.
   * @param node Root of the subtree to rotate.
   */
  void rotate_left_at(Node* node);

  /**
   * @brief Rotate a subtree right in-place at the owning slot of node.
   * @param node Root of the subtree to rotate.
   */
  void rotate_right_at(Node* node);

  /**
   * @brief Detaches the minimum node from subtree.
   * @param subtree Owning subtree slot.
   * @param replacement Node that replaced the detached minimum, possibly nullptr.
   * @param replacement_parent Parent of replacement after detaching.
   * @return Detached minimum node.
   */
  auto detach_min_node(std::unique_ptr<Node>& subtree, Node*& replacement, Node*& replacement_parent)
      -> std::unique_ptr<Node>;

  /**
   * @brief Restore Red-Black properties after deleting a black node.
   * @param node Replacement child, possibly nullptr.
   * @param parent Parent of replacement child when node is nullptr.
   * @param node_is_left_child Side occupied by a nullptr replacement.
   */
  void delete_fixup(Node* node, Node* parent, bool node_is_left_child);

  //===---------------------------- SEARCH HELPERS -----------------------------===//

  /**
   * @brief Find minimum node in subtree.
   * @param node Root of subtree.
   * @return Pointer to the node with minimum value.
   */
  static auto find_min_node(Node* node) -> Node*;

  //===--------------------------- TRAVERSAL HELPERS ---------------------------===//

  /**
   * @brief Pre-order traversal helper.
   * @param node Current node being visited.
   * @param visit Function to call for each node's value.
   */
  void pre_order_helper(const Node* node, const std::function<void(const T&)>& visit) const;

  /**
   * @brief Post-order traversal helper.
   * @param node Current node being visited.
   * @param visit Function to call for each node's value.
   */
  void post_order_helper(const Node* node, const std::function<void(const T&)>& visit) const;

  //===-------------------------- HEIGHT/VALIDATION ----------------------------===//

  /**
   * @brief Calculate height of subtree.
   * @param node Root of subtree.
   * @return Height of subtree (-1 if node is nullptr).
   */
  [[nodiscard]] auto height_helper(const Node* node) const noexcept -> int;

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
  [[nodiscard]] auto validate_helper(const Node* node, const T* lower_bound, const T* upper_bound) const -> int;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<Node> root_;   ///< Root of the tree.
  size_t                size_{}; ///< Number of nodes in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Red_Black_Tree.tpp"

#endif // RED_BLACK_TREE_HPP

//===--------------------------------------------------------------------------===//
