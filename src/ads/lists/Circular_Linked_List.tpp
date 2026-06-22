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

//===----- ITERATOR IMPLEMENTATION ---------------------------------------------===//

template <ListElement T>
auto CircularLinkedList<T>::iterator::operator*() const -> reference {
  return node_->data;
}

template <ListElement T>
auto CircularLinkedList<T>::iterator::operator->() const -> pointer {
  return &node_->data;
}

template <ListElement T>
auto CircularLinkedList<T>::iterator::operator++() -> iterator& {
  if (remaining_ > 0) {
    --remaining_;
    if (remaining_ == 0) {
      node_ = nullptr;
    } else {
      node_ = node_->next.get();
    }
  }
  return *this;
}

template <ListElement T>
auto CircularLinkedList<T>::iterator::operator++(int) -> iterator {
  iterator tmp = *this;
  ++(*this);
  return tmp;
}

template <ListElement T>
auto CircularLinkedList<T>::iterator::operator==(const iterator& other) const -> bool {
  return remaining_ == other.remaining_ && (remaining_ == 0 || node_ == other.node_);
}

//===----- CONST_ITERATOR IMPLEMENTATION ---------------------------------------===//

template <ListElement T>
auto CircularLinkedList<T>::const_iterator::operator*() const -> reference {
  return node_->data;
}

template <ListElement T>
auto CircularLinkedList<T>::const_iterator::operator->() const -> pointer {
  return &node_->data;
}

template <ListElement T>
auto CircularLinkedList<T>::const_iterator::operator++() -> const_iterator& {
  if (remaining_ > 0) {
    --remaining_;
    if (remaining_ == 0) {
      node_ = nullptr;
    } else {
      node_ = node_->next.get();
    }
  }
  return *this;
}

template <ListElement T>
auto CircularLinkedList<T>::const_iterator::operator++(int) -> const_iterator {
  const_iterator tmp = *this;
  ++(*this);
  return tmp;
}

template <ListElement T>
auto CircularLinkedList<T>::const_iterator::operator==(const const_iterator& other) const -> bool {
  return remaining_ == other.remaining_ && (remaining_ == 0 || node_ == other.node_);
}

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <ListElement T>
CircularLinkedList<T>::CircularLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {
}

template <ListElement T>
CircularLinkedList<T>::~CircularLinkedList() {
  clear();
}

template <ListElement T>
CircularLinkedList<T>::CircularLinkedList(CircularLinkedList&& other) noexcept :
    head_(std::move(other.head_)),
    tail_(other.tail_),
    size_(other.size_) {
  other.tail_ = nullptr;
  other.size_ = 0;
}

template <ListElement T>
auto CircularLinkedList<T>::operator=(CircularLinkedList&& other) noexcept -> CircularLinkedList& {
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

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <ListElement T>
template <typename... Args>
requires EmplaceListElement<T, Args...>
auto CircularLinkedList<T>::emplace_front(Args&&... args) -> T& {
  auto  new_node = std::make_unique<Node>(std::forward<Args>(args)...);
  Node* node_ptr = new_node.get();

  if (is_empty()) {
    tail_ = node_ptr;
  } else {
    new_node->next = std::move(head_);
  }

  head_ = std::move(new_node);
  ++size_;
  return head_->data;
}

template <ListElement T>
auto CircularLinkedList<T>::push_front(const T& value) -> void {
  if constexpr (CopyListElement<T>) {
    emplace_front(value);
  } else {
    throw ListException("push_front copy requires copy-constructible values");
  }
}

template <ListElement T>
auto CircularLinkedList<T>::push_front(T&& value) -> void requires MoveListElement<T>
{
  emplace_front(std::move(value));
}

template <ListElement T>
template <typename... Args>
requires EmplaceListElement<T, Args...>
auto CircularLinkedList<T>::emplace_back(Args&&... args) -> T& {
  auto  new_node = std::make_unique<Node>(std::forward<Args>(args)...);
  Node* node_ptr = new_node.get();

  if (is_empty()) {
    head_ = std::move(new_node);
    tail_ = node_ptr;
  } else {
    tail_->next = std::move(new_node);
    tail_       = node_ptr;
  }

  ++size_;
  return tail_->data;
}

template <ListElement T>
auto CircularLinkedList<T>::push_back(const T& value) -> void {
  if constexpr (CopyListElement<T>) {
    emplace_back(value);
  } else {
    throw ListException("push_back copy requires copy-constructible values");
  }
}

template <ListElement T>
auto CircularLinkedList<T>::push_back(T&& value) -> void requires MoveListElement<T>
{
  emplace_back(std::move(value));
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <ListElement T>
auto CircularLinkedList<T>::pop_front() -> void {
  if (is_empty()) {
    throw ListException("pop_front on empty circular list");
  }

  if (size_ == 1) {
    head_.reset();
    tail_ = nullptr;
  } else {
    head_ = std::move(head_->next);
  }

  --size_;
}

template <ListElement T>
auto CircularLinkedList<T>::pop_back() -> void {
  if (is_empty()) {
    throw ListException("pop_back on empty circular list");
  }

  if (size_ == 1) {
    head_.reset();
    tail_ = nullptr;
    --size_;
    return;
  }

  // Find the second-to-last node.
  Node* current = head_.get();
  while (current->next.get() != tail_) {
    current = current->next.get();
  }

  current->next.reset();
  tail_ = current;
  --size_;
}

template <ListElement T>
auto CircularLinkedList<T>::clear() noexcept -> void {
  while (head_) {
    // Detach each successor before destruction so clearing cannot recurse deeply.
    head_ = std::move(head_->next);
  }
  tail_ = nullptr;
  size_ = 0;
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <ListElement T>
auto CircularLinkedList<T>::front() -> T& {
  if (is_empty()) {
    throw ListException("front on empty circular list");
  }
  return head_->data;
}

template <ListElement T>
auto CircularLinkedList<T>::front() const -> const T& {
  if (is_empty()) {
    throw ListException("front on empty circular list");
  }
  return head_->data;
}

template <ListElement T>
auto CircularLinkedList<T>::back() -> T& {
  if (is_empty()) {
    throw ListException("back on empty circular list");
  }
  return tail_->data;
}

template <ListElement T>
auto CircularLinkedList<T>::back() const -> const T& {
  if (is_empty()) {
    throw ListException("back on empty circular list");
  }
  return tail_->data;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <ListElement T>
auto CircularLinkedList<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <ListElement T>
auto CircularLinkedList<T>::size() const noexcept -> size_type {
  return size_;
}

//===----- CIRCULAR-SPECIFIC OPERATIONS ----------------------------------------===//

template <ListElement T>
auto CircularLinkedList<T>::rotate() -> void {
  if (size_ <= 1) {
    return;
  }
  // Move ownership through the chain so rotation stays RAII-managed.
  auto old_head = std::move(head_);
  head_         = std::move(old_head->next);
  old_head->next.reset();
  tail_->next = std::move(old_head);
  tail_       = tail_->next.get();
}

template <ListElement T>
auto CircularLinkedList<T>::reverse() noexcept -> void {
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

template <ListElement T>
auto CircularLinkedList<T>::contains(const T& value) const -> bool {
  if (is_empty()) {
    return false;
  }

  Node* current = head_.get();
  while (current != nullptr) {
    if (current->data == value) {
      return true;
    }
    current = current->next.get();
  }

  return false;
}

//===----- ITERATOR OPERATIONS -------------------------------------------------===//

template <ListElement T>
auto CircularLinkedList<T>::begin() noexcept -> iterator {
  if (is_empty()) {
    return end();
  }
  return iterator(head_.get(), size_);
}

template <ListElement T>
auto CircularLinkedList<T>::end() noexcept -> iterator {
  return iterator(nullptr, 0);
}

template <ListElement T>
auto CircularLinkedList<T>::begin() const noexcept -> const_iterator {
  if (is_empty()) {
    return end();
  }
  return const_iterator(head_.get(), size_);
}

template <ListElement T>
auto CircularLinkedList<T>::end() const noexcept -> const_iterator {
  return const_iterator(nullptr, 0);
}

template <ListElement T>
auto CircularLinkedList<T>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template <ListElement T>
auto CircularLinkedList<T>::cend() const noexcept -> const_iterator {
  return end();
}

} // namespace ads::lists

//===---------------------------------------------------------------------------===//
