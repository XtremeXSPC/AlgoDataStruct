//===---------------------------------------------------------------------------===//
/**
 * @file AVL_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the AVL Tree class.
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include "Binary_Tree.hpp"
#include "Binary_Tree_Exception.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <queue>
#include <stack>
#include <utility>

namespace ads::trees {

/**
 * @brief An implementation of an AVL Tree (Adelson-Velsky and Landis Tree).
 *
 * @details This class implements a self-balancing binary search tree where the
 *          height difference between left and right subtrees (balance factor)
 *          of every node never exceeds 1. This guarantees O(log n) time complexity
 *          for insert, delete, and search operations in the worst case.
 *
 *          The balance is maintained through rotations:
 *            - Left rotation (RR case).
 *            - Right rotation (LL case).
 *            - Left-Right rotation (LR case).
 *            - Right-Left rotation (RL case).
 *
 *          Balance factor = height(left subtree) - height(right subtree).
 *          Valid range: {-1, 0, 1}
 *
 *          The tree does not allow duplicate values. Attempting to insert a duplicate
 *          will return false but won't modify the tree.
 *
 *          Memory management is automated via std::unique_ptr, ensuring no memory leaks.
 *          The tree is move-only to prevent expensive deep copies.
 *
 * @tparam T The type of data to store.
 *         Must be copyable and support "operator<" and "operator==".
 */
template <typename T>
class AVLTree : public BinaryTree<T> {
private:
  struct Node;

public:
  //===---------------------------- ITERATOR CLASS -----------------------------===//
  /**
   * @brief Forward iterator for in-order traversal of the AVL Tree.
   *
   * @details This iterator traverses the tree in ascending order of values.
   *          It uses an internal stack to keep track of the traversal state.
   *          The iterator is a forward iterator (not bidirectional) for simplicity.
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
    friend class AVLTree<T>;

    // Private constructor used by begin() to initialize the iterator.
    explicit iterator(Node* root);

    // Stack to maintain the path during in-order traversal.
    std::stack<Node*> stack_;
    Node*             current_;

    // Helper to push all left children onto the stack.
    void push_left(Node* node);
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty AVL tree.
   * @complexity Time O(1), Space O(1)
   */
  AVLTree();

  /**
   * @brief Move constructor.
   * @param other The tree from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  AVLTree(AVLTree&& other) noexcept;

  /**
   * @brief Destructor. Empties the tree and deallocates all nodes.
   * @complexity Time O(n), Space O(1)
   */
  ~AVLTree() override = default;

  /**
   * @brief Move assignment operator.
   * @param other The tree from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(AVLTree&& other) noexcept -> AVLTree&;

  // Copy constructor and assignment are disabled (move-only type).
  AVLTree(const AVLTree&)                    = delete;
  auto operator=(const AVLTree&) -> AVLTree& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts a value into the tree (copy).
   * @param value The value to insert.
   * @return true if the value was inserted, false if it already exists.
   * @complexity Time O(log n), Space O(log n) due to recursion.
   */
  auto insert(const T& value) -> bool override;

  /**
   * @brief Inserts a value into the tree (move).
   * @param value The r-value to move.
   * @return true if the value was inserted, false if it already exists.
   * @complexity Time O(log n), Space O(log n) due to recursion.
   */
  auto insert(T&& value) -> bool override;

  /**
   * @brief Constructs an element in-place in the tree.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return true if the element was inserted, false if it already exists.
   * @complexity Time O(log n), Space O(log n) due to recursion.
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> bool;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes a value from the tree.
   * @param value The value to remove.
   * @return true if the value was found and removed, false otherwise.
   * @complexity Time O(log n), Space O(log n) due to recursion.
   */
  auto remove(const T& value) -> bool override;

  /**
   * @brief Removes all elements from the tree.
   * @complexity Time O(n), Space O(1)
   */
  void clear() noexcept override;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the tree is empty.
   * @return true if the tree contains no elements.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool override;

  /**
   * @brief Returns the number of elements in the tree.
   * @return The number of nodes in the tree.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t override;

  /**
   * @brief Returns the height of the tree.
   * @return Height of the tree (empty tree = 0, leaf = 1).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto height() const noexcept -> int override;

  /**
   * @brief Checks if a value exists in the tree.
   * @param value The value to search for.
   * @return true if the value exists, false otherwise.
   * @complexity Time O(log n), Space O(log n) due to recursion.
   */
  [[nodiscard]] auto contains(const T& value) const -> bool override;

  /**
   * @brief Finds and returns a pointer to a value in the tree.
   * @param value The value to search for.
   * @return Pointer to the value if found, nullptr otherwise.
   * @complexity Time O(log n), Space O(log n) due to recursion.
   */
  [[nodiscard]] auto find(const T& value) -> T*;

  /**
   * @brief Finds and returns a pointer to a value in the tree (const version).
   * @param value The value to search for.
   * @return Pointer to the value if found, nullptr otherwise.
   * @complexity Time O(log n), Space O(log n) due to recursion.
   */
  [[nodiscard]] auto find(const T& value) const -> const T*;

  /**
   * @brief Finds and returns the minimum value in the tree.
   * @return Const reference to the minimum value.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto find_min() const -> const T& override;

  /**
   * @brief Finds and returns the maximum value in the tree.
   * @return Const reference to the maximum value.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto find_max() const -> const T& override;

  //===------------------------- TRAVERSAL OPERATIONS --------------------------===//

  /**
   * @brief Performs an in-order traversal of the tree.
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(h)
   */
  void in_order_traversal(const std::function<void(const T&)>& visit) const override;

  /**
   * @brief Performs a pre-order traversal of the tree.
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(h)
   */
  void pre_order_traversal(const std::function<void(const T&)>& visit) const override;

  /**
   * @brief Performs a post-order traversal of the tree.
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(h)
   */
  void post_order_traversal(const std::function<void(const T&)>& visit) const override;

  /**
   * @brief Performs a level-order traversal of the tree.
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(n)
   */
  void level_order_traversal(const std::function<void(const T&)>& visit) const override;

  //===----------------- ADDITIONAL AVL-SPECIFIC FUNCTIONALITY -----------------===//

  /**
   * @brief Returns the balance factor of the tree (at the root).
   * @details This is primarily for testing/debugging purposes.
   * @return Balance factor of the root node.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto get_balance() const noexcept -> int;

  /**
   * @brief Checks if the tree is properly balanced (all balance factors in [-1, 1]).
   * @details This is primarily for testing/debugging purposes.
   * @return true if balanced, false otherwise.
   * @complexity Time O(n), Space O(h)
   */
  [[nodiscard]] auto is_balanced() const noexcept -> bool;

  //===-------------------------- ITERATOR OPERATIONS --------------------------===//

  /**
   * @brief Returns an iterator to the beginning (smallest element).
   * @return iterator pointing to the smallest element in the tree.
   * @complexity Time O(h), Space O(h)
   */
  auto begin() -> iterator;

  /**
   * @brief Returns a const iterator to the beginning.
   * @return const iterator pointing to the smallest element.
   * @complexity Time O(h), Space O(h)
   */
  auto begin() const -> iterator;

  /**
   * @brief Returns an iterator to the end (past the last element).
   * @return iterator representing the end of the iteration.
   * @complexity Time O(1), Space O(1)
   */
  auto end() -> iterator;

  /**
   * @brief Returns a const iterator to the end.
   * @return const iterator representing the end of the iteration.
   * @complexity Time O(1), Space O(1)
   */
  auto end() const -> iterator;

  /**
   * @brief Returns a const iterator to the beginning.
   * @return const iterator pointing to the smallest element.
   * @complexity Time O(h), Space O(h)
   */
  auto cbegin() const -> iterator;

  /**
   * @brief Returns a const iterator to the end.
   * @return const iterator representing the end of the iteration.
   * @complexity Time O(1), Space O(1)
   */
  auto cend() const -> iterator;

private:
  //===------------------------ INTERNAL NODE STRUCTURE ------------------------===//
  /**
   * @brief Internal node structure for the AVL Tree.
   *
   * @details Each node contains a value, height information, and owns its
   *          left and right children through unique_ptr, ensuring automatic
   *          memory management.
   *
   *          Height of a leaf node is 1, height of nullptr is 0.
   */
  struct Node {
    T   data;
    int height = 1; ///< Height of subtree rooted at this node.

    std::unique_ptr<Node> left  = nullptr;
    std::unique_ptr<Node> right = nullptr;

    template <typename... Args>
      requires(!std::is_same_v<std::remove_cvref_t<Args>, Node> && ...)
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...) {}
  };

  //===============================================================================//
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Gets the height of a node.
   * @param node Node to query (can be nullptr).
   * @return Height of the node (0 if nullptr).
   */
  [[nodiscard]] auto get_height(const Node* node) const noexcept -> int;

  /**
   * @brief Calculates the balance factor of a node.
   * @details Balance factor = height(left) - height(right)
   * @param node Node to query.
   * @return Balance factor (0 if nullptr).
   */
  [[nodiscard]] auto get_balance_factor(const Node* node) const noexcept -> int;

  /**
   * @brief Updates the height of a node based on its children.
   * @param node Node whose height to update.
   */
  void update_height(Node* node) noexcept;

  //===-------------------------- ROTATION OPERATIONS --------------------------===//

  /**
   * @brief Performs a right rotation (LL case).
   * @details
   *       y                    x
   *      / \                  / \
   *     x   C    ===>        A   y
   *    / \                      / \
   *   A   B                    B   C
   * @param y Root of subtree to rotate.
   * @return New root after rotation (x).
   */
  auto rotate_right(std::unique_ptr<Node> y) -> std::unique_ptr<Node>;

  /**
   * @brief Performs a left rotation (RR case).
   * @details
   *     x                      y
   *    / \                    / \
   *   A   y      ===>        x   C
   *      / \                / \
   *     B   C              A   B
   * @param x Root of subtree to rotate.
   * @return New root after rotation (y).
   */
  auto rotate_left(std::unique_ptr<Node> x) -> std::unique_ptr<Node>;

  /**
   * @brief Performs a left-right rotation (LR case).
   * @details First rotates left on left child, then right on node.
   * @param node Root of subtree to rotate.
   * @return New root after rotation.
   */
  auto rotate_left_right(std::unique_ptr<Node> node) -> std::unique_ptr<Node>;

  /**
   * @brief Performs a right-left rotation (RL case).
   * @details First rotates right on right child, then left on node.
   * @param node Root of subtree to rotate.
   * @return New root after rotation.
   */
  auto rotate_right_left(std::unique_ptr<Node> node) -> std::unique_ptr<Node>;

  //===------------------------------- BALANCING -------------------------------===//

  /**
   * @brief Balances a node after insertion or deletion.
   * @details Checks balance factor and performs appropriate rotations.
   * @param node Node to balance.
   * @return Potentially new root after balancing.
   */
  auto balance(std::unique_ptr<Node> node) -> std::unique_ptr<Node>;

  /**
   * @brief Recursive helper to check if tree is balanced.
   * @param node Current node to check.
   * @return true if subtree rooted at node is balanced.
   */
  auto is_balanced_helper(const Node* node) const noexcept -> bool;

  //===---------------------- MODIFICATION HELPER METHODS ----------------------===//

  /**
   * @brief Recursive helper for inserting a value.
   * @param node Reference to the node pointer (can be modified to point to new node).
   * @param value The value to insert.
   * @param inserted Output parameter: set to true if inserted, false if duplicate.
   * @return Potentially new root after insertion and balancing.
   */
  template <typename U>
  auto insert_helper(std::unique_ptr<Node> node, U&& value, bool& inserted) -> std::unique_ptr<Node>;

  /**
   * @brief Recursive helper for removing a value.
   * @param node Reference to the node pointer.
   * @param value The value to remove.
   * @param removed Output parameter: set to true if removed, false if not found.
   * @return Potentially new root after removal and balancing.
   */
  auto remove_helper(std::unique_ptr<Node> node, const T& value, bool& removed) -> std::unique_ptr<Node>;

  /**
   * @brief Finds and detaches the minimum node from a subtree.
   * @details Used during deletion of nodes with two children.
   * @param node Reference to the subtree root.
   * @return unique_ptr to the detached minimum node.
   */
  auto detach_min(std::unique_ptr<Node>& node) -> std::unique_ptr<Node>;

  //===---------------------------- SEARCH HELPERS -----------------------------===//
  /**
   * @brief Recursive helper for searching.
   * @param node Current node to examine.
   * @param value Value to search for.
   * @return Pointer to the node containing the value, or nullptr if not found.
   */
  auto find_helper(Node* node, const T& value) const -> Node*;

  /**
   * @brief Finds the node with the minimum value in a subtree.
   * @param node Root of the subtree.
   * @return Pointer to the node with minimum value.
   */
  auto find_min_node(Node* node) const -> Node*;

  /**
   * @brief Finds the node with the maximum value in a subtree.
   * @param node Root of the subtree.
   * @return Pointer to the node with maximum value.
   */
  auto find_max_node(Node* node) const -> Node*;

  //===--------------------------- TRAVERSAL HELPERS ---------------------------===//

  /**
   * @brief Recursive helper for in-order traversal.
   * @param node Current node being visited.
   * @param visit Function to call for each node's value.
   */
  void in_order_helper(const Node* node, const std::function<void(const T&)>& visit) const;

  /**
   * @brief Recursive helper for pre-order traversal.
   * @param node Current node being visited.
   * @param visit Function to call for each node's value.
   */
  void pre_order_helper(const Node* node, const std::function<void(const T&)>& visit) const;

  /**
   * @brief Recursive helper for post-order traversal.
   * @param node Current node being visited.
   * @param visit Function to call for each node's value.
   */
  void post_order_helper(const Node* node, const std::function<void(const T&)>& visit) const;

  //===----------------------------- DATA MEMBERS ------------------------------===//
  std::unique_ptr<Node> root_; ///< Root of the tree.
  size_t                size_; ///< Number of nodes in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/AVL_Tree.tpp"

#endif // AVL_TREE_HPP

//===---------------------------------------------------------------------------===//
