//===---------------------------------------------------------------------------===//
/**
 * @file Circular_Linked_List.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the CircularLinkedList class.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef CIRCULAR_LINKED_LIST_HPP
#define CIRCULAR_LINKED_LIST_HPP

#include "List_Concepts.hpp"
#include "List_Exception.hpp"

#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace ads::lists {

/**
 * @brief A singly linked list with a circular logical interface.
 *
 * @details Internally the nodes form a linear, nullptr-terminated owning chain:
 *          each node owns its successor via std::unique_ptr and the tail's
 *          successor is null. The circular behavior is provided by the operations
 *          and the iterators, which loop back to the head after visiting size()
 *          elements rather than by physically linking the tail to the head. This
 *          keeps ownership a simple RAII chain while still serving round-robin
 *          scheduling, circular buffers, and turn-based games.
 *
 * @tparam T The type of data to store in the list.
 *
 * @note The polymorphic List<T> interface forces the copying
 *       push_front/push_back(const T&) overloads to exist even for
 *       move-only element types; those overloads then throw
 *       ListException at runtime. Prefer the rvalue overloads or the
 *       emplace functions for move-only payloads.
 */
template <ListElement T>
class CircularLinkedList {
private:
  struct Node;

public:
  using value_type = T;
  using size_type  = size_t;

  //===----- ITERATOR CLASS ----------------------------------------------------===//

  /**
   * @brief Forward iterator for CircularLinkedList.
   *
   * @details This iterator tracks both the current node and the number of
   *          elements visited to detect when a full cycle has been completed.
   */
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    ///@brief Default constructor for iterator.
    iterator() = default;

    ///@brief Constructs an iterator starting at @p node with @p remaining elements to visit.
    iterator(Node* node, size_type remaining) : node_(node), remaining_(remaining) {}

    ///@brief Returns a reference to the current element.
    auto operator*() const -> reference;

    ///@brief Returns a pointer to the current element.
    auto operator->() const -> pointer;

    ///@brief Advances to the next element (pre-increment).
    auto operator++() -> iterator&;

    ///@brief Advances to the next element (post-increment).
    auto operator++(int) -> iterator;

    ///@brief Returns true if both iterators point to the same position.
    auto operator==(const iterator& other) const -> bool;

  private:
    Node*     node_      = nullptr;
    size_type remaining_ = 0;
    friend class CircularLinkedList<T>;
  };

  //===----- CONST_ITERATOR CLASS ----------------------------------------------===//

  ///@brief Const forward iterator for CircularLinkedList.
  class const_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    ///@brief Default constructor for const_iterator.
    const_iterator() = default;

    ///@brief Constructs a const_iterator starting at @p node with @p remaining elements to visit.
    const_iterator(const Node* node, size_type remaining) : node_(node), remaining_(remaining) {}

    ///@brief Constructs a const_iterator from a non-const iterator.
    const_iterator(const iterator& it) : node_(it.node_), remaining_(it.remaining_) {}

    ///@brief Returns a const reference to the current element.
    auto operator*() const -> reference;

    ///@brief Returns a const pointer to the current element.
    auto operator->() const -> pointer;

    ///@brief Advances to the next element (pre-increment).
    auto operator++() -> const_iterator&;

    ///@brief Advances to the next element (post-increment).
    auto operator++(int) -> const_iterator;

    ///@brief Returns true if both iterators point to the same position.
    auto operator==(const const_iterator& other) const -> bool;

  private:
    const Node* node_      = nullptr;
    size_type   remaining_ = 0;
    friend class CircularLinkedList<T>;
  };

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty circular linked list.
   * @complexity Time O(1), Space O(1)
   */
  CircularLinkedList();

  /**
   * @brief Destructor. Empties the list and deallocates all nodes.
   * @complexity Time O(n), Space O(1)
   */
  ~CircularLinkedList();

  /**
   * @brief Move constructor.
   * @param other The list to move from.
   * @complexity Time O(1), Space O(1)
   */
  CircularLinkedList(CircularLinkedList&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The list to move from.
   * @return Reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(CircularLinkedList&& other) noexcept -> CircularLinkedList&;

  // Copy constructor and assignment are disabled.
  CircularLinkedList(const CircularLinkedList&)                    = delete;
  auto operator=(const CircularLinkedList&) -> CircularLinkedList& = delete;

  //===----- INSERTION OPERATIONS ----------------------------------------------===//

  /**
   * @brief Constructs an element in-place at the front of the list.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  requires EmplaceListElement<T, Args...>
  auto emplace_front(Args&&... args) -> T&;

  /**
   * @brief Inserts an element at the front of the list.
   * @param value The value to insert.
   * @complexity Time O(1), Space O(1)
   */
  auto push_front(const T& value) -> void;

  /**
   * @brief Inserts an element at the front of the list (move).
   * @param value The value to insert.
   * @complexity Time O(1), Space O(1)
   */
  auto push_front(T&& value) -> void requires MoveListElement<T>;

  /**
   * @brief Constructs an element in-place at the back of the list.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  requires EmplaceListElement<T, Args...>
  auto emplace_back(Args&&... args) -> T&;

  /**
   * @brief Inserts an element at the back of the list.
   * @param value The value to insert.
   * @complexity Time O(1), Space O(1)
   */
  auto push_back(const T& value) -> void;

  /**
   * @brief Inserts an element at the back of the list (move).
   * @param value The value to insert.
   * @complexity Time O(1), Space O(1)
   */
  auto push_back(T&& value) -> void requires MoveListElement<T>;

  //===----- REMOVAL OPERATIONS ------------------------------------------------===//

  /**
   * @brief Removes the first element from the list.
   * @throws ListException if the list is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto pop_front() -> void;

  /**
   * @brief Removes the last element from the list.
   * @throws ListException if the list is empty.
   * @complexity Time O(n), Space O(1)
   * @note O(n) because singly linked - must traverse to find second-to-last.
   */
  auto pop_back() -> void;

  /**
   * @brief Removes all elements from the list.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===----- ACCESS OPERATIONS -------------------------------------------------===//

  /**
   * @brief Returns a reference to the first element.
   * @throws ListException if the list is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() -> T&;

  /**
   * @brief Returns a const reference to the first element.
   * @throws ListException if the list is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() const -> const T&;

  /**
   * @brief Returns a reference to the last element.
   * @throws ListException if the list is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto back() -> T&;

  /**
   * @brief Returns a const reference to the last element.
   * @throws ListException if the list is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto back() const -> const T&;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Checks if the list is empty.
   * @return true if empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the list.
   * @return The current size.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_type;

  //===----- CIRCULAR-SPECIFIC OPERATIONS --------------------------------------===//

  /**
   * @brief Rotates the list by moving the head to the next element.
   * @complexity Time O(1), Space O(1)
   * @note Does nothing if list is empty or has only one element.
   */
  auto rotate() -> void;

  /**
   * @brief Reverses the order of elements in the list.
   * @complexity Time O(n), Space O(1)
   * @note Modifies pointers, does not copy data.
   */
  auto reverse() noexcept -> void;

  /**
   * @brief Searches for an element in the circular list.
   * @param value The value to search for.
   * @return true if found, false otherwise.
   * @complexity Time O(n), Space O(1)
   */
  [[nodiscard]] auto contains(const T& value) const -> bool;

  //===----- ITERATOR OPERATIONS -----------------------------------------------===//

  /// @brief Returns an iterator to the first element.
  auto begin() noexcept -> iterator;

  /// @brief Returns a const iterator to the first element.
  auto begin() const noexcept -> const_iterator;

  /// @brief Returns an iterator to one past the last element.
  auto end() noexcept -> iterator;

  /// @brief Returns a const iterator to one past the last element.
  auto end() const noexcept -> const_iterator;

  /// @brief Returns a const iterator to the first element.
  auto cbegin() const noexcept -> const_iterator;

  /// @brief Returns a const iterator to one past the last element.
  auto cend() const noexcept -> const_iterator;

  //===----- COMPARISON OPERATORS ----------------------------------------------===//

  /**
   * @brief Equality: two lists are equal when they have the same size and
   *        element-wise equal values. operator!= is synthesized by the compiler.
   * @complexity Time O(n), Space O(1)
   */
  friend auto operator==(const CircularLinkedList& lhs, const CircularLinkedList& rhs) -> bool requires EqualityComparableListElement<T>
  {
    if (lhs.size_ != rhs.size_) {
      return false;
    }
    const Node* a = lhs.head_.get();
    const Node* b = rhs.head_.get();
    while (a != nullptr) {
      if (!(a->data == b->data)) {
        return false;
      }
      a = a->next.get();
      b = b->next.get();
    }
    return true;
  }

private:
  //===----- INTERNAL NODE -----------------------------------------------------===//

  /**
   * @brief Internal node structure.
   *
   * @details Each node owns its successor. Circular behavior is exposed by the
   *          list operations while ownership stays as a linear RAII chain.
   */
  struct Node {
    T                     data;
    std::unique_ptr<Node> next;

    template <typename... Args>
    requires(!std::is_same_v<std::remove_cvref_t<Args>, Node> && ...)
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...), next(nullptr) {}
  };

  std::unique_ptr<Node> head_; ///< Owning pointer to the first node.
  Node*                 tail_; ///< Non-owning pointer to the last node.
  size_type             size_; ///< Number of elements in the list.
};

} // namespace ads::lists

// Include the implementation file for templates.
#include "../../../src/ads/lists/Circular_Linked_List.tpp"

#endif // CIRCULAR_LINKED_LIST_HPP

//===---------------------------------------------------------------------------===//
