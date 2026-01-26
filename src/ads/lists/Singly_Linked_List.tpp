//===---------------------------------------------------------------------------===//
/**
 * @file Singly_Linked_List.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the SinglyLinkedList class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/lists/Singly_Linked_List.hpp"

namespace ads::lists {

//===------------------------- ITERATOR IMPLEMENTATION -------------------------===//

template <typename T>
auto SinglyLinkedList<T>::iterator::operator*() const -> reference {
  return node_ptr_->data;
}

template <typename T>
auto SinglyLinkedList<T>::iterator::operator->() const -> pointer {
  return &(node_ptr_->data);
}

template <typename T>
auto SinglyLinkedList<T>::iterator::operator++() -> iterator& {
  if (node_ptr_) {
    node_ptr_ = node_ptr_->next.get();
  }
  return *this;
}

template <typename T>
auto SinglyLinkedList<T>::iterator::operator++(int) -> iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

//===---------------------- CONST_ITERATOR IMPLEMENTATION ----------------------===//

template <typename T>
auto SinglyLinkedList<T>::const_iterator::operator*() const -> reference {
  return node_ptr_->data;
}

template <typename T>
auto SinglyLinkedList<T>::const_iterator::operator->() const -> pointer {
  return &(node_ptr_->data);
}

template <typename T>
auto SinglyLinkedList<T>::const_iterator::operator++() -> const_iterator& {
  if (node_ptr_) {
    node_ptr_ = node_ptr_->next.get();
  }
  return *this;
}

template <typename T>
auto SinglyLinkedList<T>::const_iterator::operator++(int) -> const_iterator {
  const_iterator temp = *this;
  ++(*this);
  return temp;
}

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
SinglyLinkedList<T>::SinglyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {
}

template <typename T>
SinglyLinkedList<T>::~SinglyLinkedList() {
  clear();
}

template <typename T>
SinglyLinkedList<T>::SinglyLinkedList(SinglyLinkedList&& other) noexcept :
    head_(std::move(other.head_)), tail_(other.tail_), size_(other.size_) {
  other.tail_ = nullptr;
  other.size_ = 0;
}

template <typename T>
auto SinglyLinkedList<T>::operator=(SinglyLinkedList&& other) noexcept -> SinglyLinkedList& {
  if (this != &other) {
    clear();
    head_       = std::move(other.head_);
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
auto SinglyLinkedList<T>::emplace_front(Args&&... args) -> T& {
  auto new_node  = std::make_unique<Node>(std::forward<Args>(args)...);
  T&   ref       = new_node->data;
  new_node->next = std::move(head_);
  head_          = std::move(new_node);

  if (size_ == 0) {
    tail_ = head_.get();
  }

  ++size_;
  return ref;
}

template <typename T>
auto SinglyLinkedList<T>::push_front(const T& value) -> void {
  auto new_node  = std::make_unique<Node>(value);
  new_node->next = std::move(head_);
  head_          = std::move(new_node);

  if (size_ == 0) {
    tail_ = head_.get();
  }

  ++size_;
}

template <typename T>
auto SinglyLinkedList<T>::push_front(T&& value) -> void {
  auto new_node  = std::make_unique<Node>(std::move(value));
  new_node->next = std::move(head_);
  head_          = std::move(new_node);

  if (size_ == 0) {
    tail_ = head_.get();
  }

  ++size_;
}

template <typename T>
template <typename... Args>
auto SinglyLinkedList<T>::emplace_back(Args&&... args) -> T& {
  auto new_node = std::make_unique<Node>(std::forward<Args>(args)...);
  T&   ref      = new_node->data;

  if (is_empty()) {
    head_ = std::move(new_node);
    tail_ = head_.get();
  } else {
    tail_->next = std::move(new_node);
    tail_       = tail_->next.get();
  }

  ++size_;
  return ref;
}

template <typename T>
auto SinglyLinkedList<T>::push_back(const T& value) -> void {
  auto new_node = std::make_unique<Node>(value);

  if (is_empty()) {
    head_ = std::move(new_node);
    tail_ = head_.get();
  } else {
    tail_->next = std::move(new_node);
    tail_       = tail_->next.get();
  }

  ++size_;
}

template <typename T>
auto SinglyLinkedList<T>::push_back(T&& value) -> void {
  auto new_node = std::make_unique<Node>(std::move(value));

  if (is_empty()) {
    head_ = std::move(new_node);
    tail_ = head_.get();
  } else {
    tail_->next = std::move(new_node);
    tail_       = tail_->next.get();
  }

  ++size_;
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
auto SinglyLinkedList<T>::pop_front() -> void {
  if (is_empty()) {
    throw ListException("pop_front() called on empty SinglyLinkedList");
  }

  head_ = std::move(head_->next);
  --size_;

  if (size_ == 0) {
    tail_ = nullptr;
  }
}

template <typename T>
auto SinglyLinkedList<T>::pop_back() -> void {
  if (is_empty()) {
    throw ListException("pop_back() called on empty SinglyLinkedList");
  }

  if (size_ == 1) {
    head_.reset();
    tail_ = nullptr;
  } else {
    // Find the penultimate node (O(n) operation).
    Node* current = head_.get();
    while (current->next.get() != tail_) {
      current = current->next.get();
    }
    current->next.reset();
    tail_ = current;
  }

  --size_;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto SinglyLinkedList<T>::front() -> T& {
  if (is_empty()) {
    throw ListException("front() called on empty SinglyLinkedList");
  }
  return head_->data;
}

template <typename T>
auto SinglyLinkedList<T>::front() const -> const T& {
  if (is_empty()) {
    throw ListException("front() called on empty SinglyLinkedList");
  }
  return head_->data;
}

template <typename T>
auto SinglyLinkedList<T>::back() -> T& {
  if (is_empty()) {
    throw ListException("back() called on empty SinglyLinkedList");
  }
  return tail_->data;
}

template <typename T>
auto SinglyLinkedList<T>::back() const -> const T& {
  if (is_empty()) {
    throw ListException("back() called on empty SinglyLinkedList");
  }
  return tail_->data;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto SinglyLinkedList<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T>
auto SinglyLinkedList<T>::size() const noexcept -> size_t {
  return size_;
}

//===------------------------- MODIFICATION OPERATIONS -------------------------===//

template <typename T>
auto SinglyLinkedList<T>::clear() noexcept -> void {
  // Iterative deallocation to avoid stack overflow with large lists.
  while (head_) {
    head_ = std::move(head_->next);
  }
  tail_ = nullptr;
  size_ = 0;
}

template <typename T>
auto SinglyLinkedList<T>::reverse() noexcept -> void {
  if (size_ <= 1) {
    return; // Nothing to reverse.
  }

  std::unique_ptr<Node> prev    = nullptr;
  std::unique_ptr<Node> current = std::move(head_);
  tail_                         = current.get(); // The old head becomes the new tail.

  while (current) {
    std::unique_ptr<Node> next = std::move(current->next);
    current->next              = std::move(prev);
    prev                       = std::move(current);
    current                    = std::move(next);
  }

  head_ = std::move(prev);
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <typename T>
auto SinglyLinkedList<T>::begin() -> iterator {
  return iterator(head_.get(), this);
}

template <typename T>
auto SinglyLinkedList<T>::begin() const -> const_iterator {
  return const_iterator(head_.get(), this);
}

template <typename T>
auto SinglyLinkedList<T>::end() -> iterator {
  return iterator(nullptr, this);
}

template <typename T>
auto SinglyLinkedList<T>::end() const -> const_iterator {
  return const_iterator(nullptr, this);
}

template <typename T>
auto SinglyLinkedList<T>::cbegin() const -> const_iterator {
  return const_iterator(head_.get(), this);
}

template <typename T>
auto SinglyLinkedList<T>::cend() const -> const_iterator {
  return const_iterator(nullptr, this);
}

} // namespace ads::lists

//===---------------------------------------------------------------------------===//
