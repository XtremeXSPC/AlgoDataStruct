//===--------------------------------------------------------------------------===//
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
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

#include <iterator>
#include <memory>
#include <utility>

#include "List.hpp"

namespace ads::list {

/**
 * @brief An implementation of a doubly linked list.
 *
 * @details This class implements the `ads::list::List<T>` interface using
 *          a doubly linked node structure. It offers constant time (O(1))
 *          insertion and deletion at the beginning and end, and
 *          constant time if you have an iterator to the position.
 *          Memory management is automated via std::unique_ptr.
 *
 * @tparam T The type of data to store in the list.
 */
template <typename T>
class DoublyLinkedList : public List<T> {
private:
  struct Node;

public:
  //========== ITERATOR CLASS ==========//
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
    Node*                 node_ptr_;
    DoublyLinkedList<T>*  list_ptr_;
    friend class DoublyLinkedList<T>;
  };

  //========== CONST_ITERATOR CLASS ==========//
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
    const Node*                 node_ptr_;
    const DoublyLinkedList<T>*  list_ptr_;
    friend class DoublyLinkedList<T>;
  };

  //========== CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ==========//

  /** @brief Constructs an empty list. */
  DoublyLinkedList();

  /** @brief Destructor. Empties the list and deallocates all nodes. */
  ~DoublyLinkedList() override;

  // Copy constructor and assignment are disabled.
  DoublyLinkedList(const DoublyLinkedList&)                    = delete;
  auto operator=(const DoublyLinkedList&) -> DoublyLinkedList& = delete;

  /**
   * @brief Move constructor.
   * @param other The list from which to move resources.
   */
  DoublyLinkedList(DoublyLinkedList&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The list from which to move resources.
   * @return A reference to this instance.
   */
  auto operator=(DoublyLinkedList&& other) noexcept -> DoublyLinkedList&;

  //========== INTERFACE INHERITED FROM List<T> ==========//

  void push_back(const T& value) override;
  void push_back(T&& value) override;

  void push_front(const T& value) override;
  void push_front(T&& value) override;

  void pop_back() override;
  void pop_front() override;

  auto front() -> T& override;
  auto front() const -> const T& override;

  auto back() -> T& override;
  auto back() const -> const T& override;

  [[nodiscard]] auto is_empty() const noexcept -> bool override;
  [[nodiscard]] auto size() const noexcept -> size_t override;
  void               clear() noexcept override;

  //=========== ADDITIONAL FUNCTIONALITY ==========//

  template <typename... Args>
  auto emplace_back(Args&&... args);
  template <typename... Args>
  auto emplace_front(Args&&... args);

  auto insert(iterator pos, const T& value);
  auto insert(iterator pos, T&& value);

  auto erase(iterator pos);
  void reverse() noexcept;

  //=========== ACCESS TO ITERATORS ===========//
  // (non-const)
  auto begin() noexcept -> iterator;
  auto end() noexcept -> iterator;
  // (const)
  auto begin() const noexcept -> const_iterator;
  auto end() const noexcept -> const_iterator;
  auto cbegin() const noexcept -> const_iterator;
  auto cend() const noexcept -> const_iterator;

private:
  //================ INTERNAL NODE ================//
  struct Node {
    T                     data;
    std::unique_ptr<Node> next;
    Node*                 prev;
    template <typename... Args>
    Node(Node* ptr, Args&&... args) : data(std::forward<Args>(args)...), next(nullptr), prev(ptr) {}
  };

  std::unique_ptr<Node> head_;
  Node*                 tail_;
  size_t                size_;
};

// Include the implementation file for templates
#include "../../../src/ads/lists/Doubly_Linked_List.tpp"

} // namespace ads::list

#endif // DOUBLY_LINKED_LIST_HPP

//===--------------------------------------------------------------------------===//
