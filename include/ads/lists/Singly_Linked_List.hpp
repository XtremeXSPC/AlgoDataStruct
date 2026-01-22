//===---------------------------------------------------------------------------===//
/**
 * @file Singly_Linked_List.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the SinglyLinkedList class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SINGLY_LINKED_LIST_HPP
#define SINGLY_LINKED_LIST_HPP

#include <iterator>
#include <memory>
#include <utility>

#include "List.hpp"
#include "List_Exception.hpp"

namespace ads::lists {

/**
 * @brief An implementation of a singly linked list.
 *
 * @details This class implements the `ads::lists::List<T>` interface using
 *          a singly linked node structure. Each node has a pointer only to
 *          the next node (not the previous). It offers constant time (O(1))
 *          insertion and deletion at the front, and constant time insertion
 *          at the back (thanks to tail pointer), but O(n) deletion from back.
 *          Memory management is automated via std::unique_ptr.
 *
 * @tparam T The type of data to store in the list.
 */
template <typename T>
class SinglyLinkedList : public List<T> {
private:
  struct Node;

public:
  //===---------------------------- ITERATOR CLASS -----------------------------===//
  /**
   * @brief Forward iterator for SinglyLinkedList.
   *
   * @details Supports only forward iteration (no operator--).
   *          Complies with std::forward_iterator_tag requirements.
   */
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    iterator(Node* ptr = nullptr, SinglyLinkedList<T>* list = nullptr) : node_ptr_(ptr), list_ptr_(list) {}

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> iterator&;
    auto operator++(int) -> iterator;
    auto operator==(const iterator& other) const -> bool { return node_ptr_ == other.node_ptr_; }
    auto operator!=(const iterator& other) const -> bool { return node_ptr_ != other.node_ptr_; }

  private:
    Node*                node_ptr_;
    SinglyLinkedList<T>* list_ptr_;
    friend class SinglyLinkedList<T>;
  };

  //===------------------------- CONST_ITERATOR CLASS --------------------------===//
  /**
   * @brief Const forward iterator for SinglyLinkedList.
   *
   * @details Supports only forward iteration (no operator--).
   *          Complies with std::forward_iterator_tag requirements.
   */
  class const_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    const_iterator(const Node* ptr = nullptr, const SinglyLinkedList<T>* list = nullptr) : node_ptr_(ptr), list_ptr_(list) {}

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> const_iterator&;
    auto operator++(int) -> const_iterator;
    auto operator==(const const_iterator& other) const -> bool { return node_ptr_ == other.node_ptr_; }
    auto operator!=(const const_iterator& other) const -> bool { return node_ptr_ != other.node_ptr_; }

  private:
    const Node*                node_ptr_;
    const SinglyLinkedList<T>* list_ptr_;
    friend class SinglyLinkedList<T>;
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty list.
   * @complexity Time O(1), Space O(1)
   */
  SinglyLinkedList();

  /**
   * @brief Destructor. Empties the list and deallocates all nodes.
   * @complexity Time O(n), Space O(1)
   * @note Uses iterative deallocation to avoid stack overflow.
   */
  ~SinglyLinkedList();

  /**
   * @brief Move constructor.
   * @param other The list to move from
   * @complexity Time O(1), Space O(1)
   */
  SinglyLinkedList(SinglyLinkedList&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The list to move from
   * @return Reference to this
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(SinglyLinkedList&& other) noexcept -> SinglyLinkedList&;

  // Copy constructor and assignment are disabled.
  SinglyLinkedList(const SinglyLinkedList&)                    = delete;
  auto operator=(const SinglyLinkedList&) -> SinglyLinkedList& = delete;

  //====------------------------ INSERTION OPERATIONS -------------------------====//

  /**
   * @brief Constructs an element in-place at the front of the list.
   * @tparam Args Types of arguments to forward to T's constructor
   * @param args Arguments to forward to T's constructor
   * @return Reference to the newly constructed element
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace_front(Args&&... args) -> T&;

  /**
   * @brief Inserts an element at the front of the list.
   * @param value The value to insert (lvalue reference)
   * @complexity Time O(1), Space O(1)
   */
  auto push_front(const T& value) -> void override;

  /**
   * @brief Inserts an element at the front of the list (move version).
   * @param value The value to insert (rvalue reference)
   * @complexity Time O(1), Space O(1)
   */
  auto push_front(T&& value) -> void override;

  /**
   * @brief Constructs an element in-place at the back of the list.
   * @tparam Args Types of arguments to forward to T's constructor
   * @param args Arguments to forward to T's constructor
   * @return Reference to the newly constructed element
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace_back(Args&&... args) -> T&;

  /**
   * @brief Inserts an element at the back of the list.
   * @param value The value to insert (lvalue reference)
   * @complexity Time O(1), Space O(1)
   */
  auto push_back(const T& value) -> void override;

  /**
   * @brief Inserts an element at the back of the list (move version).
   * @param value The value to insert (rvalue reference)
   * @complexity Time O(1), Space O(1)
   */
  auto push_back(T&& value) -> void override;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the first element from the list.
   * @throws ListException if the list is empty
   * @complexity Time O(1), Space O(1)
   */
  auto pop_front() -> void override;

  /**
   * @brief Removes the last element from the list.
   * @throws ListException if the list is empty
   * @complexity Time O(n), Space O(1)
   * @note O(n) because we need to traverse to find the penultimate node
   */
  auto pop_back() -> void override;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Returns a reference to the first element.
   * @return Reference to the first element
   * @throws ListException if the list is empty
   * @complexity Time O(1), Space O(1)
   */
  auto front() -> T& override;

  /**
   * @brief Returns a const reference to the first element.
   * @return Const reference to the first element
   * @throws ListException if the list is empty
   * @complexity Time O(1), Space O(1)
   */
  auto front() const -> const T& override;

  /**
   * @brief Returns a reference to the last element.
   * @return Reference to the last element
   * @throws ListException if the list is empty
   * @complexity Time O(1), Space O(1)
   */
  auto back() -> T& override;

  /**
   * @brief Returns a const reference to the last element.
   * @return Const reference to the last element
   * @throws ListException if the list is empty
   * @complexity Time O(1), Space O(1)
   */
  auto back() const -> const T& override;

  //====-------------------------- QUERY OPERATIONS ---------------------------====//

  /**
   * @brief Checks if the list is empty.
   * @return true if the list is empty, false otherwise
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool override;

  /**
   * @brief Returns the number of elements in the list.
   * @return The number of elements
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t override;

  //===------------------------ MODIFICATION OPERATIONS ------------------------===//

  /**
   * @brief Removes all elements from the list.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void override;

  /**
   * @brief Reverses the order of elements in the list.
   * @complexity Time O(n), Space O(1)
   * @note Modifies pointers, does not copy data
   */
  auto reverse() noexcept -> void;

  //====------------------------- ITERATOR OPERATIONS -------------------------====//

  /**
   * @brief Returns an iterator to the beginning of the list.
   * @return Iterator to the first element
   * @complexity Time O(1), Space O(1)
   */
  auto begin() -> iterator;

  /**
   * @brief Returns a const iterator to the beginning of the list.
   * @return Const iterator to the first element
   * @complexity Time O(1), Space O(1)
   */
  auto begin() const -> const_iterator;

  /**
   * @brief Returns an iterator to the end of the list.
   * @return Iterator to one past the last element
   * @complexity Time O(1), Space O(1)
   */
  auto end() -> iterator;

  /**
   * @brief Returns a const iterator to the end of the list.
   * @return Const iterator to one past the last element
   * @complexity Time O(1), Space O(1)
   */
  auto end() const -> const_iterator;

  /**
   * @brief Returns a const iterator to the beginning of the list.
   * @return Const iterator to the first element
   * @complexity Time O(1), Space O(1)
   */
  auto cbegin() const -> const_iterator;

  /**
   * @brief Returns a const iterator to the end of the list.
   * @return Const iterator to one past the last element
   * @complexity Time O(1), Space O(1)
   */
  auto cend() const -> const_iterator;

private:
  //====---------------------------- INTERNAL NODE ----------------------------====//
  /**
   * @brief Internal node structure.
   *
   * @details Each node contains data and a unique_ptr to the next node.
   *          The unique_ptr provides automatic memory management.
   */
  struct Node {
    T                     data;
    std::unique_ptr<Node> next;

    template <typename... Args>
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...), next(nullptr) {}
  };

  std::unique_ptr<Node> head_; ///< Pointer to the first node (owns the node)
  Node*                 tail_; ///< Raw pointer to the last node (non-owning)
  size_t                size_; ///< Number of elements in the list
};

} // namespace ads::lists

// Include for the template implementation.
#include "../../../src/ads/lists/Singly_Linked_List.tpp"

#endif // SINGLY_LINKED_LIST_HPP
//===--------------------------------------------------------------------------===//
