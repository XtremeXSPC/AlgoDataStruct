//===---------------------------------------------------------------------------===//
/**
 * @file Doubly_Linked_List.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the DoublyLinkedList class.
 * @version 0.2
 * @date 2025-06-30
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//
#pragma once

#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

#include <iterator>
#include <memory>
#include <utility>

#include "List.hpp"
#include "List_Exception.hpp"

namespace ads::lists {

/**
 * @brief An implementation of a doubly linked list.
 *
 * @details This class implements the `ads::lists::List<T>` interface using
 *          a doubly linked node structure. It offers constant time (O(1))
 *          insertion and deletion at the beginning and end, and constant
 *          time insertion/erasure when a valid iterator is available.
 *          Memory management is automated via std::unique_ptr.
 *
 * @tparam T The type of data to store in the list.
 */

template <typename T>
class DoublyLinkedList : public List<T> {
private:
  struct Node;

public:
  //===---------------------------- ITERATOR CLASS -----------------------------===//
  /**
   * @brief Forward iterator for DoublyLinkedList.
   *
   * @details Supports both forward and backward iteration.
   *          Complies with std::bidirectional_iterator_tag requirements.
   */
  class iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    iterator(Node* ptr = nullptr, DoublyLinkedList<T>* list = nullptr) : node_ptr_(ptr), list_ptr_(list) {}

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> iterator&;
    auto operator++(int) -> iterator;
    auto operator--() -> iterator&;
    auto operator--(int) -> iterator;
    auto operator==(const iterator& other) const -> bool { return node_ptr_ == other.node_ptr_; }
    auto operator!=(const iterator& other) const -> bool { return node_ptr_ != other.node_ptr_; }

  private:
    Node*                node_ptr_;
    DoublyLinkedList<T>* list_ptr_;
    friend class DoublyLinkedList<T>;
  };

  //===------------------------- CONST_ITERATOR CLASS --------------------------===//
  /**
   * @brief Constant bidirectional iterator for DoublyLinkedList.
   *
   * @details Supports both forward and backward iteration.
   *          Complies with std::bidirectional_iterator_tag requirements.
   */
  class const_iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    const_iterator(const Node* ptr = nullptr, const DoublyLinkedList<T>* list = nullptr) : node_ptr_(ptr), list_ptr_(list) {}

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> const_iterator&;
    auto operator++(int) -> const_iterator;
    auto operator--() -> const_iterator&;
    auto operator--(int) -> const_iterator;
    auto operator==(const const_iterator& other) const -> bool { return node_ptr_ == other.node_ptr_; }
    auto operator!=(const const_iterator& other) const -> bool { return node_ptr_ != other.node_ptr_; }

  private:
    const Node*                node_ptr_;
    const DoublyLinkedList<T>* list_ptr_;
    friend class DoublyLinkedList<T>;
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty list.
   * @complexity Time O(1), Space O(1)
   */
  DoublyLinkedList();

  /**
   * @brief Destructor. Empties the list and deallocates all nodes.
   * @complexity Time O(n), Space O(1)
   */
  ~DoublyLinkedList() override;

  /**
   * @brief Move constructor.
   * @param other The list from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  DoublyLinkedList(DoublyLinkedList&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The list from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(DoublyLinkedList&& other) noexcept -> DoublyLinkedList&;

  // Copy constructor and assignment are disabled.
  DoublyLinkedList(const DoublyLinkedList&)                    = delete;
  auto operator=(const DoublyLinkedList&) -> DoublyLinkedList& = delete;

  //====------------------------ INSERTION OPERATIONS -------------------------====//

  /**
   * @brief Inserts an element at the back of the list.
   * @param value The value to insert (lvalue reference)
   * @complexity Time O(1), Space O(1)
   */
  void push_back(const T& value) override;

  /**
   * @brief Inserts an element at the back of the list (move version).
   * @param value The value to insert (rvalue reference)
   * @complexity Time O(1), Space O(1)
   */
  void push_back(T&& value) override;

  /**
   * @brief Inserts an element at the front of the list.
   * @param value The value to insert (lvalue reference)
   * @complexity Time O(1), Space O(1)
   */
  void push_front(const T& value) override;

  /**
   * @brief Inserts an element at the front of the list (move version).
   * @param value The value to insert (rvalue reference)
   * @complexity Time O(1), Space O(1)
   */
  void push_front(T&& value) override;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the last element from the list.
   * @throws ListException if the list is empty
   * @complexity Time O(1), Space O(1)
   */
  void pop_back() override;

  /**
   * @brief Removes the first element from the list.
   * @throws ListException if the list is empty
   * @complexity Time O(1), Space O(1)
   */
  void pop_front() override;

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

  /**
   * @brief Removes all elements from the list.
   * @complexity Time O(n), Space O(1)
   */
  void               clear() noexcept override;

  //====------------------------ ADDITIONAL OPERATIONS ------------------------====//

  /**
   * @brief Constructs an element in-place at the back of the list.
   * @tparam Args Types of arguments to forward to T's constructor
   * @param args Arguments to forward to T's constructor
   * @return Reference to the newly constructed element
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace_back(Args&&... args);

  /**
   * @brief Constructs an element in-place at the front of the list.
   * @tparam Args Types of arguments to forward to T's constructor
   * @param args Arguments to forward to T's constructor
   * @return Reference to the newly constructed element
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace_front(Args&&... args);

  /**
   * @brief Inserts an element before the position indicated by the iterator.
   * @param pos Iterator to the insertion position
   * @param value The value to insert (lvalue reference)
   * @return Iterator to the inserted element
   * @complexity Time O(1), Space O(1)
   * @note Undefined behavior if pos does not belong to this list.
   */
  auto insert(iterator pos, const T& value);

  /**
   * @brief Inserts an element before the position indicated by the iterator (move version).
   * @param pos Iterator to the insertion position
   * @param value The value to insert (rvalue reference)
   * @return Iterator to the inserted element
   * @complexity Time O(1), Space O(1)
   * @note Undefined behavior if pos does not belong to this list.
   */
  auto insert(iterator pos, T&& value);

  /**
   * @brief Removes the element at the given iterator position.
   * @param pos Iterator to the element to remove
   * @return Iterator to the element following the erased one
   * @throws ListException if pos is end() or the list is empty
   * @complexity Time O(1), Space O(1)
   * @note Undefined behavior if pos does not belong to this list.
   */
  auto erase(iterator pos);

  /**
   * @brief Reverses the order of elements in the list.
   * @complexity Time O(n), Space O(1)
   * @note Modifies pointers, does not copy data
   */
  void reverse() noexcept;

  //====------------------------- ITERATOR OPERATIONS -------------------------====//

  /**
   * @brief Returns an iterator to the beginning of the list.
   * @return Iterator to the first element
   * @complexity Time O(1), Space O(1)
   */
  auto begin() noexcept -> iterator;

  /**
   * @brief Returns an iterator to the end of the list.
   * @return Iterator to one past the last element
   * @complexity Time O(1), Space O(1)
   */
  auto end() noexcept -> iterator;

  /**
   * @brief Returns a const iterator to the beginning of the list.
   * @return Const iterator to the first element
   * @complexity Time O(1), Space O(1)
   */
  auto begin() const noexcept -> const_iterator;

  /**
   * @brief Returns a const iterator to the end of the list.
   * @return Const iterator to one past the last element
   * @complexity Time O(1), Space O(1)
   */
  auto end() const noexcept -> const_iterator;

  /**
   * @brief Returns a const iterator to the beginning of the list.
   * @return Const iterator to the first element
   * @complexity Time O(1), Space O(1)
   */
  auto cbegin() const noexcept -> const_iterator;

  /**
   * @brief Returns a const iterator to the end of the list.
   * @return Const iterator to one past the last element
   * @complexity Time O(1), Space O(1)
   */
  auto cend() const noexcept -> const_iterator;

private:
  //====---------------------------- INTERNAL NODE ----------------------------====//
  /**
   * @brief Internal node structure.
   *
   * @details Each node contains data, a unique_ptr to the next node,
   *          and a raw pointer to the previous node.
   */
  struct Node {
    T                     data;
    std::unique_ptr<Node> next;
    Node*                 prev;
    template <typename... Args>
    Node(Node* ptr, Args&&... args) : data(std::forward<Args>(args)...), next(nullptr), prev(ptr) {}
  };

  std::unique_ptr<Node> head_; ///< Pointer to the first node (owns the node)
  Node*                 tail_; ///< Raw pointer to the last node (non-owning)
  size_t                size_; ///< Number of elements in the list
};

} // namespace ads::lists

// Include for the template implementation.
#include "../../../src/ads/lists/Doubly_Linked_List.tpp" // IWYU pragma: export

#endif // DOUBLY_LINKED_LIST_HPP

//===---------------------------------------------------------------------------===//
