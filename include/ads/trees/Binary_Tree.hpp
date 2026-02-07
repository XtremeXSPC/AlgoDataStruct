//===---------------------------------------------------------------------------===//
/**
 * @file Binary_Tree.hpp
 * @author Costantino Lombardi
 * @brief Abstract interface for a Binary Tree data structure.
 * @version 0.1
 * @date 2025-10-15
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef BINARY_TREE_HPP
#define BINARY_TREE_HPP

#include "Tree_Concepts.hpp"

#include <cstddef>
#include <functional>

namespace ads::trees {

/**
 * @brief Abstract base class that defines common operations for a binary tree data structure.
 *
 * @details A binary tree is a hierarchical data structure where each node has at most
 *          two children, referred to as left and right child. This interface provides
 *          the fundamental operations that any binary tree implementation must support.
 *
 *          Different implementations (Binary Search Tree, AVL Tree, Red-Black Tree, etc.)
 *          will provide specific invariants and performance guarantees while adhering
 *          to this common interface.
 *
 * @tparam T The data type to store in the tree. Must be copyable and support comparison
 *           operations for ordered tree implementations.
 */
template <TreeElement T>
class BinaryTree {
public:
  // A virtual destructor is mandatory for polymorphic base classes.
  virtual ~BinaryTree() = default;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts a value into the tree (copy).
   * @param value The value to insert.
   * @return true if the value was inserted, false if it already exists.
   */
  virtual auto insert(const T& value) -> bool = 0;

  /**
   * @brief Inserts a value into the tree (move).
   * @param value The r-value to move.
   * @return true if the value was inserted, false if it already exists.
   */
  virtual auto insert(T&& value) -> bool = 0;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes a value from the tree.
   * @param value The value to remove.
   * @return true if the value was found and removed, false otherwise.
   */
  virtual auto remove(const T& value) -> bool = 0;

  /**
   * @brief Empties the tree, removing all elements.
   * @details After this operation, the tree is empty and size() returns 0.
   */
  virtual void clear() noexcept = 0;

  //===--------------------------- SEARCH OPERATIONS ---------------------------===//

  /**
   * @brief Checks if a value exists in the tree.
   * @param value The value to search for.
   * @return true if the value exists in the tree, false otherwise.
   */
  [[nodiscard]] virtual auto contains(const T& value) const -> bool = 0;

  /**
   * @brief Finds and returns the minimum value in the tree.
   * @return const T& Reference to the minimum value.
   * @throws EmptyTreeException if the tree is empty.
   */
  [[nodiscard]] virtual auto find_min() const -> const T& = 0;

  /**
   * @brief Finds and returns the maximum value in the tree.
   * @return const T& Reference to the maximum value.
   * @throws EmptyTreeException if the tree is empty.
   */
  [[nodiscard]] virtual auto find_max() const -> const T& = 0;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the tree is empty.
   * @return true if the tree contains no elements.
   * @return false otherwise.
   */
  [[nodiscard]] virtual auto is_empty() const noexcept -> bool = 0;

  /**
   * @brief Returns the number of elements in the tree.
   * @return size_t The number of nodes in the tree.
   */
  [[nodiscard]] virtual auto size() const noexcept -> size_t = 0;

  /**
   * @brief Returns the height of the tree.
   * @details The height is defined as the number of edges on the longest path
   *          from the root to a leaf. An empty tree has height -1, and a tree
   *          with only a root has height 0.
   * @return int The height of the tree, or -1 if empty.
   */
  [[nodiscard]] virtual auto height() const noexcept -> int = 0;

  //===------------------------- TRAVERSAL OPERATIONS --------------------------===//

  /**
   * @brief Performs an in-order traversal of the tree.
   * @details In-order traversal visits nodes in the order: left subtree, root, right subtree.
   *          For a Binary Search Tree, this produces values in sorted ascending order.
   * @param visit A function or lambda to call for each element during traversal.
   */
  virtual void in_order_traversal(const std::function<void(const T&)>& visit) const = 0;

  /**
   * @brief Performs a pre-order traversal of the tree.
   * @details Pre-order traversal visits nodes in the order: root, left subtree, right subtree.
   *          Useful for creating a copy of the tree or for prefix expression evaluation.
   * @param visit A function or lambda to call for each element during traversal.
   */
  virtual void pre_order_traversal(const std::function<void(const T&)>& visit) const = 0;

  /**
   * @brief Performs a post-order traversal of the tree.
   * @details Post-order traversal visits nodes in the order: left subtree, right subtree, root.
   *          Useful for deleting the tree or for postfix expression evaluation.
   * @param visit A function or lambda to call for each element during traversal.
   */
  virtual void post_order_traversal(const std::function<void(const T&)>& visit) const = 0;

  /**
   * @brief Performs a level-order (breadth-first) traversal of the tree.
   * @details Level-order traversal visits nodes level by level, from left to right.
   *          Useful for finding the shortest path from root or for level-based operations.
   * @param visit A function or lambda to call for each element during traversal.
   */
  virtual void level_order_traversal(const std::function<void(const T&)>& visit) const = 0;
};

} // namespace ads::trees

#endif // BINARY_TREE_HPP

//===---------------------------------------------------------------------------===//
