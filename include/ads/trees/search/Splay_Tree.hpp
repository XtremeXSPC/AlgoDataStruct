//===---------------------------------------------------------------------------===//
/**
 * @file Splay_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the SplayTree class.
 * @version 0.1
 * @date 2026-06-19
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SPLAY_TREE_HPP
#define SPLAY_TREE_HPP

#include "../../arrays/Dynamic_Array.hpp"
#include "../../queues/Linked_Queue.hpp"
#include "../Tree_Concepts.hpp"
#include "../exceptions/Binary_Tree_Exception.hpp"

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace ads::trees {

namespace arr = ads::arrays;

/**
 * @brief A self-adjusting binary search tree (Sleator-Tarjan splay tree).
 *
 * @details A splay tree is an ordered search tree with no explicit balance
 *          invariant. Instead, every access (insert, remove, lookup) moves the
 *          touched node to the root via a sequence of rotations called a
 *          "splay". This yields O(log n) amortized time per operation and keeps
 *          recently used elements near the root.
 *
 *          The implementation uses bottom-up splaying over parent pointers (the
 *          same rotation machinery as Red_Black_Tree) and stores children through
 *          std::unique_ptr for automatic memory management. Structural traversals,
 *          height, validation and teardown are iterative so a degenerate (chain)
 *          shape cannot overflow the call stack.
 *
 *          Because a lookup splays the accessed node, the root pointer is declared
 *          mutable: contains(), find_min() and find_max() are logically const
 *          queries that still restructure the tree. The tree is move-only.
 *
 * @tparam T The stored value type. Must support "operator<" and "operator==".
 *         Copy insertion additionally requires copy construction.
 */
template <OrderedTreeElement T>
class SplayTree {
private:
  struct Node;

public:
  using value_type   = T;
  using size_type    = size_t;
  using visitor_type = std::function<void(const T&)>;

  //===---------------------------- ITERATOR CLASS -----------------------------===//
  /**
   * @brief Forward iterator performing an in-order traversal.
   *
   * @details Navigates through parent pointers (leftmost + successor), so it uses
   *          O(1) extra space and never recurses. Iteration does not splay, so the
   *          tree shape is stable for the lifetime of a traversal.
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
    friend class SplayTree<T>;

    explicit iterator(Node* current);
    static auto leftmost(Node* node) noexcept -> Node*;
    static auto successor(Node* node) noexcept -> Node*;

    Node* current_ = nullptr;
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty splay tree.
   * @complexity Time O(1), Space O(1)
   */
  SplayTree();

  /**
   * @brief Move constructor.
   * @param other The tree from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  SplayTree(SplayTree&& other) noexcept;

  /**
   * @brief Destructor. Empties the tree and deallocates all nodes.
   * @complexity Time O(n), Space O(1)
   */
  ~SplayTree();

  /**
   * @brief Move assignment operator.
   * @param other The tree from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(SplayTree&& other) noexcept -> SplayTree&;

  // Copy constructor and assignment are disabled (move-only type).
  SplayTree(const SplayTree&)                    = delete;
  auto operator=(const SplayTree&) -> SplayTree& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts a value into the tree (copy) and splays it to the root.
   * @param value The value to insert.
   * @return true if the value was inserted, false if it already exists.
   * @complexity Time O(log n) amortized.
   */
  auto insert(const T& value) -> bool requires std::copy_constructible<T>;

  /**
   * @brief Inserts a value into the tree (move) and splays it to the root.
   * @param value The r-value to move.
   * @return true if the value was inserted, false if it already exists.
   * @complexity Time O(log n) amortized.
   */
  auto insert(T&& value) -> bool;

  /**
   * @brief Constructs an element in-place, then splays it to the root.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return true if the element was inserted, false if it already exists.
   * @complexity Time O(log n) amortized.
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> bool;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes a value from the tree.
   * @details Splays the target to the root, then joins its two subtrees.
   * @param value The value to remove.
   * @return true if the value was found and removed, false otherwise.
   * @complexity Time O(log n) amortized.
   */
  auto remove(const T& value) -> bool;

  /**
   * @brief Removes all elements from the tree.
   * @complexity Time O(n), Space O(1)
   */
  void clear() noexcept;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the tree is empty.
   * @return true if the tree contains no elements.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the tree.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the height of the tree.
   * @return Height of the tree (empty tree = -1).
   * @complexity Time O(n), Space O(n)
   */
  [[nodiscard]] auto height() const noexcept -> int;

  /**
   * @brief Checks if a value exists, splaying the accessed node to the root.
   * @param value The value to search for.
   * @return true if the value exists, false otherwise.
   * @complexity Time O(log n) amortized.
   */
  [[nodiscard]] auto contains(const T& value) const -> bool;

  /**
   * @brief Returns the minimum value, splaying it to the root.
   * @return Const reference to the minimum value.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity Time O(log n) amortized.
   */
  [[nodiscard]] auto find_min() const -> const T&;

  /**
   * @brief Returns the maximum value, splaying it to the root.
   * @return Const reference to the maximum value.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity Time O(log n) amortized.
   */
  [[nodiscard]] auto find_max() const -> const T&;

  //===------------------------- TRAVERSAL OPERATIONS --------------------------===//

  /**
   * @brief Performs an in-order traversal of the tree.
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(1)
   */
  void in_order_traversal(const visitor_type& visit) const;

  /**
   * @brief Performs a pre-order traversal of the tree.
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(n)
   */
  void pre_order_traversal(const visitor_type& visit) const;

  /**
   * @brief Performs a post-order traversal of the tree.
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(n)
   */
  void post_order_traversal(const visitor_type& visit) const;

  /**
   * @brief Performs a level-order traversal of the tree.
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(n)
   */
  void level_order_traversal(const visitor_type& visit) const;

  /**
   * @brief Checks the BST ordering, parent-link and stored-size invariants.
   * @details For testing/debugging. Splay trees have no balance invariant.
   * @return true if the invariants hold, false otherwise.
   * @complexity Time O(n), Space O(n)
   */
  [[nodiscard]] auto validate_properties() const -> bool;

  //===-------------------------- ITERATOR OPERATIONS --------------------------===//

  auto begin() -> iterator;
  auto begin() const -> iterator;
  auto end() -> iterator;
  auto end() const -> iterator;
  auto cbegin() const -> iterator;
  auto cend() const -> iterator;

private:
  //===------------------------ INTERNAL NODE STRUCTURE ------------------------===//
  struct Node {
    T                     data;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    Node*                 parent = nullptr;

    template <typename... Args>
    requires(!std::is_same_v<std::remove_cvref_t<Args>, Node> && ...)
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...) {}
  };

  //===---------------------- STRUCTURAL (SPLAY) HELPERS -----------------------===//
  // These operate on the mutable root_ and are const-qualified so the logically
  // const queries (contains/find_min/find_max) may splay the accessed node.

  /**
   * @brief Returns the owning unique_ptr that holds @p node (root_ or a child slot).
   */
  auto owning_link(Node* node) const -> std::unique_ptr<Node>&;

  /**
   * @brief Rotates the subtree left; returns the new subtree root (caller links parent).
   */
  auto rotate_left(std::unique_ptr<Node> x_ptr) const -> std::unique_ptr<Node>;

  /**
   * @brief Rotates the subtree right; returns the new subtree root (caller links parent).
   */
  auto rotate_right(std::unique_ptr<Node> y_ptr) const -> std::unique_ptr<Node>;

  /**
   * @brief Rotates @p node's right child up into @p node's position.
   */
  void rotate_left_at(Node* node) const;

  /**
   * @brief Rotates @p node's left child up into @p node's position.
   */
  void rotate_right_at(Node* node) const;

  /**
   * @brief Splays @p node to the root via zig / zig-zig / zig-zag steps.
   */
  void splay(Node* node) const;

  //===---------------------------- SEARCH HELPERS -----------------------------===//

  /**
   * @brief Locates the node holding @p value without splaying.
   */
  auto find_node(const T& value) const -> Node*;

  static auto subtree_min(Node* node) noexcept -> Node*;
  static auto subtree_max(Node* node) noexcept -> Node*;

  /**
   * @brief Shared insert path used by the copy/move/emplace front ends.
   */
  template <typename U>
  auto insert_impl(U&& value) -> bool;

  //===------------------------ ITERATIVE TREE HELPERS -------------------------===//

  auto height_helper(const Node* node) const noexcept -> int;
  auto validate_helper() const -> bool;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  mutable std::unique_ptr<Node> root_; ///< Root of the tree (mutable: splay-on-access).
  size_t                        size_; ///< Number of nodes in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../../src/ads/trees/search/Splay_Tree.tpp"

#endif // SPLAY_TREE_HPP

//===---------------------------------------------------------------------------===//
