//===--------------------------------------------------------------------------===//
/**
 * @file Binary_Search_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the BinarySearchTree class.
 * @version 0.1
 * @date 2025-10-15
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef BINARY_SEARCH_TREE_HPP
#define BINARY_SEARCH_TREE_HPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <queue>
#include <stack>
#include <utility>

#include "Binary_Tree.hpp"
#include "Binary_Tree_Exception.hpp"

namespace ads::trees {

/**
 * @brief An implementation of a Binary Search Tree.
 *
 * @details This class implements the BinaryTree interface using a binary search tree
 *          structure. A BST maintains the invariant that for every node, all values
 *          in its left subtree are less than the node's value, and all values in
 *          its right subtree are greater than the node's value.
 *
 *          This provides O(h) time complexity for search, insert, and delete operations,
 *          where h is the height of the tree. In the worst case (unbalanced tree),
 *          h = n and operations become O(n). In the average case with random insertions,
 *          h = O(log n).
 *
 *          The tree does not allow duplicate values. Attempting to insert a duplicate
 *          will return false but won't modify the tree.
 *
 *          Memory management is automated via std::unique_ptr, ensuring no memory leaks.
 *          The tree is move-only to prevent expensive deep copies.
 *
 * @tparam T The type of data to store. Must be copyable and support operator< and operator==.
 */
template <typename T>
class BinarySearchTree : public BinaryTree<T> {
private:
  struct Node;

public:
  //========== ITERATOR CLASS ==========//
  /**
   * @brief Forward iterator for in-order traversal of the BST.
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

    iterator() : current_(nullptr) {}

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> iterator&;
    auto operator++(int) -> iterator;
    auto operator==(const iterator& other) const -> bool;
    auto operator!=(const iterator& other) const -> bool;

  private:
    friend class BinarySearchTree<T>;

    // Private constructor used by begin() to initialize the iterator
    explicit iterator(Node* root);

    // Stack to maintain the path during in-order traversal
    std::stack<Node*> stack_;
    Node*             current_;

    // Helper to push all left children onto the stack
    void push_left(Node* node);
  };

  //========== CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ==========//

  /** @brief Constructs an empty binary search tree. */
  BinarySearchTree();

  /** @brief Destructor. Empties the tree and deallocates all nodes. */
  ~BinarySearchTree() override = default;

  // Copy constructor and assignment are disabled (move-only type)
  BinarySearchTree(const BinarySearchTree&)                    = delete;
  auto operator=(const BinarySearchTree&) -> BinarySearchTree& = delete;

  /**
   * @brief Move constructor.
   * @param other The tree from which to move resources.
   */
  BinarySearchTree(BinarySearchTree&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The tree from which to move resources.
   * @return A reference to this instance.
   */
  auto operator=(BinarySearchTree&& other) noexcept -> BinarySearchTree&;

  //========== INTERFACE INHERITED FROM BinaryTree<T> ==========//

  auto insert(const T& value) -> bool override;
  auto insert(T&& value) -> bool override;
  auto remove(const T& value) -> bool override;
  void clear() noexcept override;

  [[nodiscard]] auto contains(const T& value) const -> bool override;
  [[nodiscard]] auto find_min() const -> const T& override;
  [[nodiscard]] auto find_max() const -> const T& override;

  [[nodiscard]] auto is_empty() const noexcept -> bool override;
  [[nodiscard]] auto size() const noexcept -> size_t override;
  [[nodiscard]] auto height() const noexcept -> int override;

  void in_order_traversal(const std::function<void(const T&)>& visit) const override;
  void pre_order_traversal(const std::function<void(const T&)>& visit) const override;
  void post_order_traversal(const std::function<void(const T&)>& visit) const override;
  void level_order_traversal(const std::function<void(const T&)>& visit) const override;

  //=========== ADDITIONAL BST-SPECIFIC FUNCTIONALITY ==========//

  /**
   * @brief Constructs an element in-place in the tree.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return true if the element was inserted, false if it already exists.
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> bool;

  /**
   * @brief Returns the successor of a given value.
   * @details The successor is the smallest value greater than the given value.
   * @param value The value whose successor to find.
   * @return const T* Pointer to the successor, or nullptr if no successor exists.
   */
  [[nodiscard]] auto successor(const T& value) const -> const T*;

  /**
   * @brief Returns the predecessor of a given value.
   * @details The predecessor is the largest value smaller than the given value.
   * @param value The value whose predecessor to find.
   * @return const T* Pointer to the predecessor, or nullptr if no predecessor exists.
   */
  [[nodiscard]] auto predecessor(const T& value) const -> const T*;

  //=========== ITERATOR ACCESS ===========//

  /**
   * @brief Returns an iterator to the beginning (smallest element).
   * @return iterator pointing to the smallest element in the tree.
   */
  auto begin() -> iterator;

  /**
   * @brief Returns an iterator to the end (past the last element).
   * @return iterator representing the end of the iteration.
   */
  auto end() -> iterator;

  /**
   * @brief Returns a const iterator to the beginning.
   * @return const iterator pointing to the smallest element.
   */
  auto begin() const -> iterator;

  /**
   * @brief Returns a const iterator to the end.
   * @return const iterator representing the end of the iteration.
   */
  auto end() const -> iterator;

  /**
   * @brief Returns a const iterator to the beginning.
   * @return const iterator pointing to the smallest element.
   */
  auto cbegin() const -> iterator;

  /**
   * @brief Returns a const iterator to the end.
   * @return const iterator representing the end of the iteration.
   */
  auto cend() const -> iterator;

private:
  //================ INTERNAL NODE STRUCTURE ================//
  /**
   * @brief Internal node structure for the BST.
   *
   * @details Each node contains a value and owns its left and right children
   *          through unique_ptr, ensuring automatic memory management.
   */
  struct Node {
    T                     data;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    template <typename... Args>
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...), left(nullptr), right(nullptr) {}
  };

  //================ INTERNAL HELPER METHODS ================//

  /**
   * @brief Recursive helper for inserting a value.
   * @param node Reference to the node pointer (can be modified to point to new node).
   * @param value The value to insert.
   * @return true if inserted, false if duplicate.
   */
  template <typename U>
  auto insert_helper(std::unique_ptr<Node>& node, U&& value) -> bool;

  /**
   * @brief Recursive helper for removing a value.
   * @param node Reference to the node pointer.
   * @param value The value to remove.
   * @return true if removed, false if not found.
   */
  auto remove_helper(std::unique_ptr<Node>& node, const T& value) -> bool;

  /**
   * @brief Finds and detaches the minimum node from a subtree.
   * @details Used during deletion of nodes with two children.
   * @param node Reference to the subtree root.
   * @return unique_ptr to the detached minimum node.
   */
  auto detach_min(std::unique_ptr<Node>& node) -> std::unique_ptr<Node>;

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

  /**
   * @brief Recursive helper to compute the height of a subtree.
   * @param node Root of the subtree.
   * @return Height of the subtree (-1 if node is nullptr).
   */
  auto height_helper(const Node* node) const noexcept -> int;

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

  //================ DATA MEMBERS ================//
  std::unique_ptr<Node> root_; ///< Root of the tree
  size_t                size_; ///< Number of nodes in the tree
};

} // namespace ads::trees

// Include the implementation file for templates
#include "../../../src/ads/trees/Binary_Search_Tree.tpp"

#endif // BINARY_SEARCH_TREE_HPP

//===--------------------------------------------------------------------------===//
