//===---------------------------------------------------------------------------===//
/**
 * @file Nary_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the NaryTree class (general rooted tree).
 * @version 0.1
 * @date 2026-06-19
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef NARY_TREE_HPP
#define NARY_TREE_HPP

#include "../arrays/Dynamic_Array.hpp"
#include "../queues/Linked_Queue.hpp"
#include "Tree_Concepts.hpp"
#include "exceptions/Binary_Tree_Exception.hpp"

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace ads::trees {

namespace arr = ads::arrays;

/**
 * @brief A general rooted tree where each node may have any number of children.
 *
 * @details Complements CompleteBinaryTree (which is restricted to a binary,
 *          heap-shaped layout) with an unconstrained arity. Nodes are referenced
 *          through non-owning Node* handles returned by the insertion methods:
 *          a child is grafted onto an existing parent via add_child()/emplace_child().
 *          Each node owns its children through std::unique_ptr stored in a
 *          DynamicArray, and keeps a non-owning back-pointer to its parent.
 *
 *          There is no in-order traversal (it is undefined for arity != 2);
 *          pre-, post- and level-order traversals are provided instead. Structural
 *          operations (height, contains, clear, traversals) are iterative so a
 *          deep or wide tree cannot overflow the call stack. The tree is move-only.
 *
 * @tparam T The stored value type. Must be equality comparable (for contains()).
 *         Copy insertion additionally requires copy construction.
 */
template <EqualityComparableTreeElement T>
class NaryTree {
public:
  //===---------------------------- NODE STRUCTURE -----------------------------===//
  /**
   * @brief A node in the tree, exposed as a non-owning handle.
   *
   * @details The tree owns nodes; callers observe and navigate them through the
   *          read-only accessors below and mutate stored values via value().
   *          Structural mutation happens only through NaryTree's insertion API.
   */
  class Node {
  public:
    /// @brief Returns a mutable reference to the stored value.
    [[nodiscard]] auto value() noexcept -> T&;
    /// @brief Returns a const reference to the stored value.
    [[nodiscard]] auto value() const noexcept -> const T&;

    /// @brief Returns the parent node, or nullptr for the root.
    [[nodiscard]] auto parent() noexcept -> Node*;
    /// @brief Returns the parent node, or nullptr for the root.
    [[nodiscard]] auto parent() const noexcept -> const Node*;

    /// @brief Returns the number of direct children.
    [[nodiscard]] auto child_count() const noexcept -> size_t;

    /// @brief Returns the child at @p index. Throws if out of range.
    [[nodiscard]] auto child(size_t index) -> Node*;
    /// @brief Returns the child at @p index. Throws if out of range.
    [[nodiscard]] auto child(size_t index) const -> const Node*;

    /// @brief Returns true if the node has no children.
    [[nodiscard]] auto is_leaf() const noexcept -> bool;

  private:
    friend class NaryTree<T>;

    template <typename... Args>
    requires(!std::is_same_v<std::remove_cvref_t<Args>, Node> && ...)
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...) {}

    T                                       data;
    Node*                                   parent_ = nullptr;
    arr::DynamicArray<std::unique_ptr<Node>> children_;
  };

  using value_type   = T;
  using size_type    = size_t;
  using visitor_type = std::function<void(const T&)>;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty tree (no root).
   * @complexity Time O(1), Space O(1)
   */
  NaryTree();

  /**
   * @brief Move constructor.
   * @complexity Time O(1), Space O(1)
   */
  NaryTree(NaryTree&& other) noexcept;

  /**
   * @brief Destructor. Empties the tree and deallocates all nodes.
   * @complexity Time O(n), Space O(n)
   */
  ~NaryTree();

  /**
   * @brief Move assignment operator.
   * @complexity Time O(n), Space O(n)
   */
  auto operator=(NaryTree&& other) noexcept -> NaryTree&;

  // Copy operations disabled (move-only type).
  NaryTree(const NaryTree&)                    = delete;
  auto operator=(const NaryTree&) -> NaryTree& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Creates the root from a value (copy).
   * @return Handle to the new root.
   * @throws InvalidOperationException if the tree already has a root.
   * @complexity Time O(1), Space O(1)
   */
  auto set_root(const T& value) -> Node* requires std::copy_constructible<T>;

  /**
   * @brief Creates the root from a value (move).
   * @return Handle to the new root.
   * @throws InvalidOperationException if the tree already has a root.
   * @complexity Time O(1), Space O(1)
   */
  auto set_root(T&& value) -> Node*;

  /**
   * @brief Constructs the root in place.
   * @return Handle to the new root.
   * @throws InvalidOperationException if the tree already has a root.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace_root(Args&&... args) -> Node*;

  /**
   * @brief Appends a child (copy) to @p parent.
   * @return Handle to the new child.
   * @throws InvalidOperationException if @p parent is null.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto add_child(Node* parent, const T& value) -> Node* requires std::copy_constructible<T>;

  /**
   * @brief Appends a child (move) to @p parent.
   * @return Handle to the new child.
   * @throws InvalidOperationException if @p parent is null.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto add_child(Node* parent, T&& value) -> Node*;

  /**
   * @brief Constructs a child in place under @p parent.
   * @return Handle to the new child.
   * @throws InvalidOperationException if @p parent is null.
   * @complexity Time O(1) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace_child(Node* parent, Args&&... args) -> Node*;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes all elements from the tree.
   * @complexity Time O(n), Space O(n)
   */
  void clear() noexcept;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /// @brief Returns the root node, or nullptr if empty.
  [[nodiscard]] auto root() noexcept -> Node*;
  /// @brief Returns the root node, or nullptr if empty.
  [[nodiscard]] auto root() const noexcept -> const Node*;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /// @brief Checks if the tree is empty.
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /// @brief Returns the number of nodes in the tree.
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the height of the tree.
   * @return Longest root-to-leaf edge count (empty tree = -1, single node = 0).
   * @complexity Time O(n), Space O(n)
   */
  [[nodiscard]] auto height() const noexcept -> int;

  /**
   * @brief Checks whether a value exists anywhere in the tree.
   * @complexity Time O(n), Space O(n)
   */
  [[nodiscard]] auto contains(const T& value) const -> bool;

  //===------------------------- TRAVERSAL OPERATIONS --------------------------===//

  /**
   * @brief Pre-order traversal (node, then children left-to-right).
   * @complexity Time O(n), Space O(n)
   */
  void pre_order_traversal(const visitor_type& visit) const;

  /**
   * @brief Post-order traversal (children left-to-right, then node).
   * @complexity Time O(n), Space O(n)
   */
  void post_order_traversal(const visitor_type& visit) const;

  /**
   * @brief Level-order (breadth-first) traversal.
   * @complexity Time O(n), Space O(n)
   */
  void level_order_traversal(const visitor_type& visit) const;

private:
  //===------------------------- PRIVATE HELPER METHODS ------------------------===//

  template <typename U>
  auto set_root_impl(U&& value) -> Node*;

  template <typename U>
  auto add_child_impl(Node* parent, U&& value) -> Node*;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<Node> root_; ///< Owning root of the tree.
  size_t                size_; ///< Number of nodes in the tree.
};

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Nary_Tree.tpp"

#endif // NARY_TREE_HPP

//===---------------------------------------------------------------------------===//
