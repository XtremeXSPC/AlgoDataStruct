//===---------------------------------------------------------------------------===//
/**
 * @file Circular_Linked_List.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for CircularLinkedList.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/lists/Circular_Linked_List.hpp"

namespace ads::lists {

//===------------------------- ITERATOR IMPLEMENTATION -------------------------===//

template <typename T>
auto CircularLinkedList<T>::iterator::operator*() const -> reference {
  return node_->data;
}

template <typename T>
auto CircularLinkedList<T>::iterator::operator->() const -> pointer {
  return &node_->data;
}

template <typename T>
auto CircularLinkedList<T>::iterator::operator++() -> iterator& {
  if (remaining_ > 0) {
    --remaining_;
    if (remaining_ == 0) {
      node_ = nullptr;
    } else {
      node_ = node_->next;
    }
  }
  return *this;
}

template <typename T>
auto CircularLinkedList<T>::iterator::operator++(int) -> iterator {
  iterator tmp = *this;
  ++(*this);
  return tmp;
}

template <typename T>
auto CircularLinkedList<T>::iterator::operator==(const iterator& other) const -> bool {
  return remaining_ == other.remaining_ && (remaining_ == 0 || node_ == other.node_);
}

template <typename T>
auto CircularLinkedList<T>::iterator::operator!=(const iterator& other) const -> bool {
  return !(*this == other);
}

//===---------------------- CONST_ITERATOR IMPLEMENTATION ----------------------===//

template <typename T>
auto CircularLinkedList<T>::const_iterator::operator*() const -> reference {
  return node_->data;
}

template <typename T>
auto CircularLinkedList<T>::const_iterator::operator->() const -> pointer {
  return &node_->data;
}

template <typename T>
auto CircularLinkedList<T>::const_iterator::operator++() -> const_iterator& {
  if (remaining_ > 0) {
    --remaining_;
    if (remaining_ == 0) {
      node_ = nullptr;
    } else {
      node_ = node_->next;
    }
  }
  return *this;
}

template <typename T>
auto CircularLinkedList<T>::const_iterator::operator++(int) -> const_iterator {
  const_iterator tmp = *this;
  ++(*this);
  return tmp;
}

template <typename T>
auto CircularLinkedList<T>::const_iterator::operator==(const const_iterator& other) const -> bool {
  return remaining_ == other.remaining_ && (remaining_ == 0 || node_ == other.node_);
}

template <typename T>
auto CircularLinkedList<T>::const_iterator::operator!=(const const_iterator& other) const -> bool {
  return !(*this == other);
}

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
CircularLinkedList<T>::CircularLinkedList() : tail_(nullptr), size_(0) {
}

template <typename T>
CircularLinkedList<T>::~CircularLinkedList() {
  clear();
}

template <typename T>
CircularLinkedList<T>::CircularLinkedList(CircularLinkedList&& other) noexcept : tail_(other.tail_), size_(other.size_) {
  other.tail_ = nullptr;
  other.size_ = 0;
}

template <typename T>
auto CircularLinkedList<T>::operator=(CircularLinkedList&& other) noexcept -> CircularLinkedList& {
  if (this != &other) {
    clear();
    tail_       = other.tail_;
    size_       = other.size_;
    other.tail_ = nullptr;
    other.size_ = 0;
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename T>
template <typename... Args>
auto CircularLinkedList<T>::emplace_front(Args&&... args) -> T& {
  Node* new_node = new Node(std::forward<Args>(args)...);

  if (is_empty()) {
    new_node->next = new_node;
    tail_          = new_node;
  } else {
    new_node->next = tail_->next;
    tail_->next    = new_node;
  }

  ++size_;
  return new_node->data;
}

template <typename T>
auto CircularLinkedList<T>::push_front(const T& value) -> void {
  emplace_front(value);
}

template <typename T>
auto CircularLinkedList<T>::push_front(T&& value) -> void {
  emplace_front(std::move(value));
}

template <typename T>
template <typename... Args>
auto CircularLinkedList<T>::emplace_back(Args&&... args) -> T& {
  Node* new_node = new Node(std::forward<Args>(args)...);

  if (is_empty()) {
    new_node->next = new_node;
    tail_          = new_node;
  } else {
    new_node->next = tail_->next;
    tail_->next    = new_node;
    tail_          = new_node;
  }

  ++size_;
  return new_node->data;
}

template <typename T>
auto CircularLinkedList<T>::push_back(const T& value) -> void {
  emplace_back(value);
}

template <typename T>
auto CircularLinkedList<T>::push_back(T&& value) -> void {
  emplace_back(std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
auto CircularLinkedList<T>::pop_front() -> void {
  if (is_empty()) {
    throw ListException("pop_front on empty circular list");
  }

  Node* head = tail_->next;

  if (size_ == 1) {
    tail_ = nullptr;
  } else {
    tail_->next = head->next;
  }

  delete head;
  --size_;
}

template <typename T>
auto CircularLinkedList<T>::pop_back() -> void {
  if (is_empty()) {
    throw ListException("pop_back on empty circular list");
  }

  if (size_ == 1) {
    delete tail_;
    tail_ = nullptr;
    --size_;
    return;
  }

  // Find the second-to-last node.
  Node* current = tail_->next;
  while (current->next != tail_) {
    current = current->next;
  }

  current->next = tail_->next;
  delete tail_;
  tail_ = current;
  --size_;
}

template <typename T>
auto CircularLinkedList<T>::clear() noexcept -> void {
  if (is_empty()) {
    return;
  }

  Node* head    = tail_->next;
  Node* current = head;

  // Break the circle first.
  tail_->next = nullptr;

  while (current != nullptr) {
    Node* next = current->next;
    delete current;
    current = next;
  }

  tail_ = nullptr;
  size_ = 0;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto CircularLinkedList<T>::front() -> T& {
  if (is_empty()) {
    throw ListException("front on empty circular list");
  }
  return tail_->next->data;
}

template <typename T>
auto CircularLinkedList<T>::front() const -> const T& {
  if (is_empty()) {
    throw ListException("front on empty circular list");
  }
  return tail_->next->data;
}

template <typename T>
auto CircularLinkedList<T>::back() -> T& {
  if (is_empty()) {
    throw ListException("back on empty circular list");
  }
  return tail_->data;
}

template <typename T>
auto CircularLinkedList<T>::back() const -> const T& {
  if (is_empty()) {
    throw ListException("back on empty circular list");
  }
  return tail_->data;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto CircularLinkedList<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T>
auto CircularLinkedList<T>::size() const noexcept -> size_t {
  return size_;
}

//===---------------------- CIRCULAR-SPECIFIC OPERATIONS -----------------------===//

template <typename T>
auto CircularLinkedList<T>::rotate() -> void {
  if (size_ <= 1) {
    return;
  }
  tail_ = tail_->next;
}

template <typename T>
auto CircularLinkedList<T>::contains(const T& value) const -> bool {
  if (is_empty()) {
    return false;
  }

  Node* current = tail_->next;
  do {
    if (current->data == value) {
      return true;
    }
    current = current->next;
  } while (current != tail_->next);

  return false;
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <typename T>
auto CircularLinkedList<T>::begin() -> iterator {
  if (is_empty()) {
    return end();
  }
  return iterator(tail_->next, size_, this);
}

template <typename T>
auto CircularLinkedList<T>::end() -> iterator {
  return iterator(nullptr, 0, this);
}

template <typename T>
auto CircularLinkedList<T>::begin() const -> const_iterator {
  if (is_empty()) {
    return end();
  }
  return const_iterator(tail_->next, size_, this);
}

template <typename T>
auto CircularLinkedList<T>::end() const -> const_iterator {
  return const_iterator(nullptr, 0, this);
}

template <typename T>
auto CircularLinkedList<T>::cbegin() const -> const_iterator {
  return begin();
}

template <typename T>
auto CircularLinkedList<T>::cend() const -> const_iterator {
  return end();
}

} // namespace ads::lists

//===---------------------------------------------------------------------------===//
