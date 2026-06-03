//===---------------------------------------------------------------------------===//
/**
 * @file Treap.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the Treap class.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef TREAP_HPP
#define TREAP_HPP

#include "../arrays/Dynamic_Array.hpp"
#include "../queues/Linked_Queue.hpp"
#include "Binary_Tree_Exception.hpp"
#include "Tree_Concepts.hpp"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace ads::trees {

/**
 * @brief A randomized binary search tree that also satisfies a heap invariant on priorities.
 *
 * @details A Treap combines:
 *          - BST ordering on stored values.
 *          - Max-heap ordering on per-node priorities.
 *
 *          The default insertion path assigns priorities from an internal xorshift
 *          generator seeded at construction. Tests and advanced callers can bypass
 *          the internal generator through `insert_with_priority` and
 *          `emplace_with_priority`, which makes tree shapes fully deterministic.
 *
 *          Duplicate values are not allowed.
 *
 * @tparam T Stored value type.
 * @tparam Priority Numeric priority type.
 */
template <OrderedTreeElement T, typename Priority = std::uint32_t>
requires std::totally_ordered<Priority>
class Treap {
private:
  struct Node;

public:
  using value_type    = T;
  using priority_type = Priority;
  using size_type     = size_t;

  //===---------------------------- ITERATOR CLASS -----------------------------===//

  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    iterator() = default;
    iterator(const iterator& other);
    iterator(iterator&& other) noexcept = default;
    auto operator=(const iterator& other) -> iterator&;
    auto operator=(iterator&& other) noexcept -> iterator& = default;

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> iterator&;
    auto operator++(int) -> iterator;
    auto operator==(const iterator& other) const -> bool;

  private:
    friend class Treap<T, Priority>;

    ads::arrays::DynamicArray<Node*> stack_;
    Node*                            current_ = nullptr;

    explicit iterator(Node* root);
    void push_left(Node* node);
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty treap with a deterministic internal priority seed.
   * @param seed Seed for the internal xorshift priority generator.
   * @complexity Time O(1), Space O(1)
   */
  explicit Treap(std::uint64_t seed = kDefaultSeed);

  /**
   * @brief Move constructor.
   * @param other Treap to move from.
   * @complexity Time O(1), Space O(1)
   */
  Treap(Treap&& other) noexcept;

  /**
   * @brief Destructor. Removes all nodes.
   * @complexity Time O(n), Space O(1)
   */
  ~Treap();

  /**
   * @brief Move assignment operator.
   * @param other Treap to move from.
   * @return Reference to this treap.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(Treap&& other) noexcept -> Treap&;

  Treap(const Treap&)                    = delete;
  auto operator=(const Treap&) -> Treap& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts a value with an internally generated priority.
   * @param value Value to insert.
   * @return true if inserted, false if duplicate.
   * @complexity Expected O(log n), worst-case O(n).
   */
  auto insert(const T& value) -> bool requires std::copy_constructible<T>;

  /**
   * @brief Inserts a value with an internally generated priority using move semantics.
   * @param value Value to insert.
   * @return true if inserted, false if duplicate.
   * @complexity Expected O(log n), worst-case O(n).
   */
  auto insert(T&& value) -> bool;

  /**
   * @brief Constructs and inserts a value with an internally generated priority.
   * @tparam Args Constructor argument types.
   * @param args Constructor arguments for `T`.
   * @return true if inserted, false if duplicate.
   * @complexity Expected O(log n), worst-case O(n).
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> bool;

  /**
   * @brief Inserts a value with an explicit priority.
   * @param value Value to insert.
   * @param priority Priority to associate with the new node.
   * @return true if inserted, false if duplicate.
   * @complexity Time O(h), Space O(h) due to recursion.
   */
  auto insert_with_priority(const T& value, const Priority& priority) -> bool requires std::copy_constructible<T>;

  /**
   * @brief Inserts a value with an explicit priority using move semantics.
   * @param value Value to insert.
   * @param priority Priority to associate with the new node.
   * @return true if inserted, false if duplicate.
   * @complexity Time O(h), Space O(h) due to recursion.
   */
  auto insert_with_priority(T&& value, const Priority& priority) -> bool;

  /**
   * @brief Constructs and inserts a value with an explicit priority.
   * @tparam Args Constructor argument types.
   * @param priority Priority to associate with the new node.
   * @param args Constructor arguments for `T`.
   * @return true if inserted, false if duplicate.
   * @complexity Time O(h), Space O(h) due to recursion.
   */
  template <typename... Args>
  auto emplace_with_priority(const Priority& priority, Args&&... args) -> bool;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes a value from the treap.
   * @param value Value to remove.
   * @return true if removed, false if absent.
   * @complexity Expected O(log n), worst-case O(n).
   */
  auto remove(const T& value) -> bool;

  /**
   * @brief Removes all nodes from the treap.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks whether the treap is empty.
   * @return true if empty.
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of stored values.
   * @return Node count.
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the height of the treap, with empty tree = -1.
   * @return Height in edges.
   */
  [[nodiscard]] auto height() const noexcept -> int;

  /**
   * @brief Checks whether a value exists in the treap.
   * @param value Value to search for.
   * @return true if present.
   */
  [[nodiscard]] auto contains(const T& value) const -> bool;

  /**
   * @brief Returns the minimum value.
   * @return Const reference to the minimum value.
   * @throws EmptyTreeException if the treap is empty.
   */
  [[nodiscard]] auto find_min() const -> const T&;

  /**
   * @brief Returns the maximum value.
   * @return Const reference to the maximum value.
   * @throws EmptyTreeException if the treap is empty.
   */
  [[nodiscard]] auto find_max() const -> const T&;

  /**
   * @brief Returns the priority associated with a stored value.
   * @param value Value to search for.
   * @return Pointer to the stored priority, or nullptr if absent.
   * @complexity Time O(h), Space O(1)
   */
  [[nodiscard]] auto priority_of(const T& value) const -> const Priority*;

  //===--------------------------- TRAVERSAL API -------------------------------===//

  /**
   * @brief Traverses the treap in sorted order.
   * @param visit Callback invoked for each value.
   */
  auto in_order_traversal(const std::function<void(const T&)>& visit) const -> void;

  /**
   * @brief Traverses the treap in root-left-right order.
   * @param visit Callback invoked for each value.
   */
  auto pre_order_traversal(const std::function<void(const T&)>& visit) const -> void;

  /**
   * @brief Traverses the treap in left-right-root order.
   * @param visit Callback invoked for each value.
   */
  auto post_order_traversal(const std::function<void(const T&)>& visit) const -> void;

  /**
   * @brief Traverses the treap level by level.
   * @param visit Callback invoked for each value.
   */
  auto level_order_traversal(const std::function<void(const T&)>& visit) const -> void;

  /**
   * @brief Returns the successor of a given value.
   * @param value Value whose successor to find.
   * @return Pointer to the successor value, or nullptr if absent.
   */
  [[nodiscard]] auto successor(const T& value) const -> const T*;

  /**
   * @brief Returns the predecessor of a given value.
   * @param value Value whose predecessor to find.
   * @return Pointer to the predecessor value, or nullptr if absent.
   */
  [[nodiscard]] auto predecessor(const T& value) const -> const T*;

  /**
   * @brief Checks BST ordering, max-heap priority ordering, and stored size.
   * @return true if every treap invariant holds.
   * @complexity Time O(n), Space O(h)
   */
  [[nodiscard]] auto validate_properties() const -> bool;

  //===--------------------------- ITERATOR ACCESS -----------------------------===//

  /**
   * @brief Returns an iterator to the first value in sorted order.
   * @return Forward iterator to the smallest stored value, or end() if empty.
   */
  auto begin() -> iterator;

  /**
   * @brief Returns a const iterator to the first value in sorted order.
   * @return Forward iterator to the smallest stored value, or end() if empty.
   */
  auto begin() const -> iterator;

  /**
   * @brief Returns the sentinel iterator one past the last value.
   * @return End iterator.
   */
  auto end() -> iterator;

  /**
   * @brief Returns the const sentinel iterator one past the last value.
   * @return End iterator.
   */
  auto end() const -> iterator;

  /**
   * @brief Returns a const iterator to the first value in sorted order.
   * @return Forward iterator to the smallest stored value, or cend() if empty.
   */
  auto cbegin() const -> iterator;

  /**
   * @brief Returns the const sentinel iterator one past the last value.
   * @return End iterator.
   */
  auto cend() const -> iterator;

private:
  //===--------------------------- INTERNAL STORAGE ----------------------------===//

  struct Node {
    T                     data;
    Priority              priority;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    template <typename U>
    Node(U&& value, const Priority& node_priority) : data(std::forward<U>(value)), priority(node_priority), left(nullptr), right(nullptr) {}
  };

  std::unique_ptr<Node> root_;
  size_t                size_;
  std::uint64_t         random_state_;

  static constexpr std::uint64_t kDefaultSeed = 0x9E37'79B9'7F4A'7C15ULL;

  //===----------------------------- HELPER API --------------------------------===//

  template <typename U>
  auto insert_with_priority_impl(std::unique_ptr<Node>& node, U&& value, const Priority& priority) -> bool;
  auto remove_impl(std::unique_ptr<Node>& node, const T& value) -> bool;

  auto rotate_left(std::unique_ptr<Node>& node) -> void;
  auto rotate_right(std::unique_ptr<Node>& node) -> void;
  auto next_priority() -> Priority;

  auto find_node(Node* node, const T& value) const -> Node*;
  auto find_min_node(Node* node) const -> Node*;
  auto find_max_node(Node* node) const -> Node*;
  auto height_helper(const Node* node) const noexcept -> int;
  auto in_order_helper(const Node* node, const std::function<void(const T&)>& visit) const -> void;
  auto pre_order_helper(const Node* node, const std::function<void(const T&)>& visit) const -> void;
  auto post_order_helper(const Node* node, const std::function<void(const T&)>& visit) const -> void;
};

} // namespace ads::trees

#include "../../../src/ads/trees/Treap.tpp"

#endif // TREAP_HPP

//===---------------------------------------------------------------------------===//
