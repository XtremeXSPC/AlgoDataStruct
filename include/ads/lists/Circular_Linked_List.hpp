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

#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>

#include "List_Exception.hpp"

namespace ads::lists {

/**
 * @brief A singly linked circular list implementation.
 *
 * @details This class implements a circular linked list where the last node
 *          points back to the first node. Unlike a standard singly linked list,
 *          there is no nullptr termination - the list forms a continuous ring.
 *          Useful for round-robin scheduling, circular buffers, and games.
 *
 * @tparam T The type of data to store in the list.
 */
template <typename T>
class CircularLinkedList {
private:
  struct Node;

public:
  //===---------------------------- ITERATOR CLASS -----------------------------===//

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

    iterator() = default;
    iterator(Node* node, size_t remaining, CircularLinkedList<T>* list) : node_(node), remaining_(remaining), list_(list) {}

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> iterator&;
    auto operator++(int) -> iterator;
    auto operator==(const iterator& other) const -> bool;
    auto operator!=(const iterator& other) const -> bool;

  private:
    Node*                  node_      = nullptr;
    size_t                 remaining_ = 0;
    CircularLinkedList<T>* list_      = nullptr;
    friend class CircularLinkedList<T>;
  };

  //===------------------------- CONST_ITERATOR CLASS --------------------------===//

  /**
   * @brief Const forward iterator for CircularLinkedList.
   */
  class const_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    const_iterator() = default;
    const_iterator(const Node* node, size_t remaining, const CircularLinkedList<T>* list) :
        node_(node), remaining_(remaining), list_(list) {}
    const_iterator(const iterator& it) : node_(it.node_), remaining_(it.remaining_), list_(it.list_) {}

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> const_iterator&;
    auto operator++(int) -> const_iterator;
    auto operator==(const const_iterator& other) const -> bool;
    auto operator!=(const const_iterator& other) const -> bool;

  private:
    const Node*                  node_      = nullptr;
    size_t                       remaining_ = 0;
    const CircularLinkedList<T>* list_      = nullptr;
    friend class CircularLinkedList<T>;
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

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

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Constructs an element in-place at the front of the list.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
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
  auto push_front(T&& value) -> void;

  /**
   * @brief Constructs an element in-place at the back of the list.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
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
  auto push_back(T&& value) -> void;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

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

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

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

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

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
  [[nodiscard]] auto size() const noexcept -> size_t;

  //===--------------------- CIRCULAR-SPECIFIC OPERATIONS ----------------------===//

  /**
   * @brief Rotates the list by moving the head to the next element.
   * @complexity Time O(1), Space O(1)
   * @note Does nothing if list is empty or has only one element.
   */
  auto rotate() -> void;

  /**
   * @brief Searches for an element in the circular list.
   * @param value The value to search for.
   * @return true if found, false otherwise.
   * @complexity Time O(n), Space O(1)
   */
  [[nodiscard]] auto contains(const T& value) const -> bool;

  //===-------------------------- ITERATOR OPERATIONS --------------------------===//

  /**
   * @brief Returns an iterator/const_iterator to the beginning of the list.
   */
  auto begin() -> iterator;
  auto begin() const -> const_iterator;

  /**
   * @brief Returns an iterator/const_iterator to the end of the list.
   */
  auto end() -> iterator;
  auto end() const -> const_iterator;

  /**
   * @brief Returns a const iterator to the beginning/end of the list.
   */
  auto cbegin() const -> const_iterator;
  auto cend() const -> const_iterator;

private:
  //===----------------------------- INTERNAL NODE -----------------------------===//

  /**
   * @brief Internal node structure.
   *
   * @details Each node contains data and a raw pointer to the next node.
   *          Unlike non-circular lists, the tail's next always points to head.
   */
  struct Node {
    T     data;
    Node* next = nullptr;

    template <typename... Args>
      requires(!std::is_same_v<std::remove_cvref_t<Args>..., Node>)
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...) {}
  };

  Node*  tail_; ///< Pointer to the last node (tail->next is head).
  size_t size_; ///< Number of elements in the list.
};

} // namespace ads::lists

// Include the implementation file for templates.
#include "../../../src/ads/lists/Circular_Linked_List.tpp"

#endif // CIRCULAR_LINKED_LIST_HPP

//===---------------------------------------------------------------------------===//
