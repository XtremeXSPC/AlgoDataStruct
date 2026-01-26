//===---------------------------------------------------------------------------===//
/**
 * @file Complete_Binary_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the CompleteBinaryTree class.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef COMPLETE_BINARY_TREE_HPP
#define COMPLETE_BINARY_TREE_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

#include "Binary_Tree_Exception.hpp"

namespace ads::trees {

/**
 * @brief A concrete binary tree implementation with level-order insertion.
 *
 * @details This class implements a binary tree where new nodes are inserted
 *          in level-order (breadth-first), creating a complete binary tree.
 *          Unlike Binary Search Trees, elements are not ordered; they are
 *          simply stored in the tree structure.
 *
 *          A complete binary tree is one where all levels except possibly
 *          the last are completely filled, and all nodes in the last level
 *          are as far left as possible.
 *
 *          Useful for: heap implementations, complete tree representations,
 *          level-based algorithms, and educational purposes.
 *
 * @tparam T The type of data to store in the tree.
 */
template <typename T>
class CompleteBinaryTree {
public:
  //===---------------------------- NODE STRUCTURE -----------------------------===//

  /**
   * @brief A node in the binary tree.
   */
  struct Node {
    T                     data;
    std::unique_ptr<Node> left  = nullptr;
    std::unique_ptr<Node> right = nullptr;

    template <typename... Args>
      requires(!std::same_as<std::remove_cvref_t<Args>, Node> && ...)
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...) {}
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty binary tree.
   * @complexity Time O(1), Space O(1)
   */
  CompleteBinaryTree();

  /**
   * @brief Constructs a binary tree from an initializer list (level-order).
   * @param values Elements to insert in level-order.
   * @complexity Time O(n), Space O(n)
   */
  CompleteBinaryTree(std::initializer_list<T> values);

  /**
   * @brief Move constructor.
   * @param other The tree to move from.
   * @complexity Time O(1), Space O(1)
   */
  CompleteBinaryTree(CompleteBinaryTree&& other) noexcept;

  /**
   * @brief Destructor.
   * @complexity Time O(n), Space O(h) where h is height
   */
  ~CompleteBinaryTree() = default;

  /**
   * @brief Move assignment operator.
   * @param other The tree to move from.
   * @return Reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(CompleteBinaryTree&& other) noexcept -> CompleteBinaryTree&;

  // Copy operations disabled (move-only type).
  CompleteBinaryTree(const CompleteBinaryTree&)                    = delete;
  auto operator=(const CompleteBinaryTree&) -> CompleteBinaryTree& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts a value into the tree in level-order position.
   * @param value The value to insert.
   * @complexity Time O(n), Space O(n) for queue
   */
  auto insert(const T& value) -> void;

  /**
   * @brief Inserts a value into the tree in level-order position (move).
   * @param value The r-value to move.
   * @complexity Time O(n), Space O(n) for queue
   */
  auto insert(T&& value) -> void;

  /**
   * @brief Constructs a value in-place in the tree.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(n), Space O(n)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T&;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes all elements from the tree.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Returns a reference to the root element.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto root() -> T&;

  /**
   * @brief Returns a const reference to the root element.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto root() const -> const T&;

  /**
   * @brief Returns the root node (for tree traversal).
   * @return Pointer to the root node, or nullptr if empty.
   * @complexity Time O(1), Space O(1)
   */
  auto root_node() -> Node*;

  /**
   * @brief Returns the root node (const version).
   * @return Pointer to the root node, or nullptr if empty.
   * @complexity Time O(1), Space O(1)
   */
  auto root_node() const -> const Node*;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the tree is empty.
   * @return true if empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the tree.
   * @return The current size.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the height of the tree.
   * @return The height (-1 if empty, 0 if only root).
   * @complexity Time O(n), Space O(h)
   */
  [[nodiscard]] auto height() const noexcept -> int;

  /**
   * @brief Checks if a value exists in the tree.
   * @param value The value to search for.
   * @return true if found, false otherwise.
   * @complexity Time O(n), Space O(n)
   */
  [[nodiscard]] auto contains(const T& value) const -> bool;

  //===------------------------- TRAVERSAL OPERATIONS --------------------------===//

  /**
   * @brief Performs an in-order traversal (left, root, right).
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(h)
   */
  auto in_order_traversal(const std::function<void(const T&)>& visit) const -> void;

  /**
   * @brief Performs a pre-order traversal (root, left, right).
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(h)
   */
  auto pre_order_traversal(const std::function<void(const T&)>& visit) const -> void;

  /**
   * @brief Performs a post-order traversal (left, right, root).
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(h)
   */
  auto post_order_traversal(const std::function<void(const T&)>& visit) const -> void;

  /**
   * @brief Performs a level-order (breadth-first) traversal.
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(n)
   */
  auto level_order_traversal(const std::function<void(const T&)>& visit) const -> void;

  /**
   * @brief Collects all elements in level-order into a vector.
   * @return Vector of elements in level-order.
   * @complexity Time O(n), Space O(n)
   */
  [[nodiscard]] auto to_vector() const -> std::vector<T>;

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Computes the height of a subtree rooted at the given node.
   * @param node The root of the subtree.
   * @return Height of the subtree.
   */
  [[nodiscard]]
  auto compute_height(const Node* node) const noexcept -> int;

  /**
   * @brief In-order traversal implementation.
   * @param node Current node.
   * @param visit Function to call for each element.
   */
  auto in_order_impl(const Node* node, const std::function<void(const T&)>& visit) const -> void;

  /**
   * @brief Pre-order traversal implementation.
   * @param node Current node.
   * @param visit Function to call for each element.
   */
  auto pre_order_impl(const Node* node, const std::function<void(const T&)>& visit) const -> void;

  /**
   * @brief Post-order traversal implementation.
   * @param node Current node.
   * @param visit Function to call for each element.
   */
  auto post_order_impl(const Node* node, const std::function<void(const T&)>& visit) const -> void;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<Node> root_; ///< Pointer to the root node.
  size_t                size_; ///< Number of elements in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Complete_Binary_Tree.tpp"

#endif // COMPLETE_BINARY_TREE_HPP

//===---------------------------------------------------------------------------===//
