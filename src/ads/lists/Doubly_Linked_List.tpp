//===--------------------------------------------------------------------------===//
/**
 * @file Doubly_Linked_List.tpp
 * @author Costantino Lombardi
 * @brief
 * @version 0.1
 * @date 2025-06-30
 *
 * @copyright Copyright (c) 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#include <stdexcept> // Per std::logic_error
#include <utility>   // Per std::swap

#include "../../../include/ads/lists/Doubly_Linked_List.hpp"

// Il modo corretto: aprire lo stesso namespace dell'header.
namespace ads::list {

// --- Implementazione Iteratore (pulita) ---
template <typename T>
typename DoublyLinkedList<T>::iterator::reference DoublyLinkedList<T>::iterator::operator*() const {
  return node_ptr_->data;
}
// ... (tutte le altre definizioni dell'iteratore seguono questo pattern pulito)
template <typename T>
typename DoublyLinkedList<T>::iterator::pointer DoublyLinkedList<T>::iterator::operator->() const {
  return &node_ptr_->data;
}
template <typename T>
typename DoublyLinkedList<T>::iterator& DoublyLinkedList<T>::iterator::operator++() {
  node_ptr_ = node_ptr_->next.get();
  return *this;
}
template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::iterator::operator++(int) {
  iterator temp = *this;
  ++(*this);
  return temp;
}
template <typename T>
typename DoublyLinkedList<T>::iterator& DoublyLinkedList<T>::iterator::operator--() {
  node_ptr_ = node_ptr_->prev;
  return *this;
}
template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::iterator::operator--(int) {
  iterator temp = *this;
  --(*this);
  return temp;
}

// --- Implementazione DoublyLinkedList ---

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {
}

template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
  clear();
}

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(DoublyLinkedList&& other) noexcept :
    head_(std::move(other.head_)), tail_(other.tail_), size_(other.size_) {
  other.tail_ = nullptr;
  other.size_ = 0;
}

template <typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(DoublyLinkedList&& other) noexcept {
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

// ... push, pop, front, back, etc. rimangono invariati ...
template <typename T>
template <typename... Args>
T& DoublyLinkedList<T>::emplace_back(Args&&... args) {
  if (!tail_) {
    head_ = std::make_unique<Node>(nullptr, std::forward<Args>(args)...);
    tail_ = head_.get();
  } else {
    tail_->next = std::make_unique<Node>(tail_, std::forward<Args>(args)...);
    tail_       = tail_->next.get();
  }
  size_++;
  return tail_->data;
}

template <typename T>
void DoublyLinkedList<T>::push_back(const T& value) {
  emplace_back(value);
}

template <typename T>
void DoublyLinkedList<T>::push_back(T&& value) {
  emplace_back(std::move(value));
}

template <typename T>
template <typename... Args>
T& DoublyLinkedList<T>::emplace_front(Args&&... args) {
  auto newNode = std::make_unique<Node>(nullptr, std::forward<Args>(args)...);
  if (head_) {
    head_->prev   = newNode.get();
    newNode->next = std::move(head_);
  } else {
    tail_ = newNode.get();
  }
  head_ = std::move(newNode);
  size_++;
  return head_->data;
}

template <typename T>
void DoublyLinkedList<T>::push_front(const T& value) {
  emplace_front(value);
}

template <typename T>
void DoublyLinkedList<T>::push_front(T&& value) {
  emplace_front(std::move(value));
}

template <typename T>
void DoublyLinkedList<T>::pop_front() {
  if (is_empty())
    throw std::logic_error("pop_front on empty list");
  head_ = std::move(head_->next);
  if (head_)
    head_->prev = nullptr;
  else
    tail_ = nullptr;
  size_--;
}

template <typename T>
void DoublyLinkedList<T>::pop_back() {
  if (is_empty())
    throw std::logic_error("pop_back on empty list");
  if (size_ == 1) {
    pop_front();
  } else {
    tail_ = tail_->prev;
    tail_->next.reset();
    size_--;
  }
}

template <typename T>
T& DoublyLinkedList<T>::front() {
  if (is_empty())
    throw std::logic_error("front on empty list");
  return head_->data;
}

template <typename T>
const T& DoublyLinkedList<T>::front() const {
  if (is_empty())
    throw std::logic_error("front on empty list");
  return head_->data;
}

template <typename T>
T& DoublyLinkedList<T>::back() {
  if (is_empty())
    throw std::logic_error("back on empty list");
  return tail_->data;
}

template <typename T>
const T& DoublyLinkedList<T>::back() const {
  if (is_empty())
    throw std::logic_error("back on empty list");
  return tail_->data;
}

template <typename T>
bool DoublyLinkedList<T>::is_empty() const noexcept {
  return size_ == 0;
}

template <typename T>
size_t DoublyLinkedList<T>::size() const noexcept {
  return size_;
}

template <typename T>
void DoublyLinkedList<T>::clear() noexcept {
  head_.reset();
  tail_ = nullptr;
  size_ = 0;
}

// --- Funzione `insert` (Implementazione completa) ---
namespace { // Helper in un namespace anonimo per evitare conflitti
template <typename T, typename U>
typename DoublyLinkedList<T>::iterator insert_impl(DoublyLinkedList<T>& list, typename DoublyLinkedList<T>::iterator pos, U&& value) {
  // ... logica di insert ... (non esposta direttamente)
}
} // namespace

template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::insert(iterator pos, const T& value) {
  if (pos == begin()) {
    push_front(value);
    return begin();
  }
  if (pos == end()) {
    push_back(value);
    return iterator(tail_);
  }

  Node* pos_node  = pos.node_ptr_;
  Node* prev_node = pos_node->prev;

  auto new_node   = std::make_unique<Node>(prev_node, value);
  new_node->next  = std::move(prev_node->next);
  pos_node->prev  = new_node.get();
  prev_node->next = std::move(new_node);

  size_++;
  return iterator(pos_node->prev);
}

template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::insert(iterator pos, T&& value) {
  if (pos == begin()) {
    push_front(std::move(value));
    return begin();
  }
  if (pos == end()) {
    push_back(std::move(value));
    return iterator(tail_);
  }

  Node* pos_node  = pos.node_ptr_;
  Node* prev_node = pos_node->prev;

  auto new_node   = std::make_unique<Node>(prev_node, std::move(value));
  new_node->next  = std::move(prev_node->next);
  pos_node->prev  = new_node.get();
  prev_node->next = std::move(new_node);

  size_++;
  return iterator(pos_node->prev);
}

template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::erase(iterator pos) {
  if (pos == end() || is_empty()) {
    throw std::logic_error("cannot erase invalid iterator");
  }
  Node*    to_erase = pos.node_ptr_;
  iterator next_it(to_erase->next.get());

  if (to_erase->prev == nullptr) { // Testa
    pop_front();
  } else if (to_erase->next == nullptr) { // Coda
    pop_back();
  } else { // In mezzo
    to_erase->prev->next       = std::move(to_erase->next);
    to_erase->prev->next->prev = to_erase->prev;
    size_--;
  }
  return next_it;
}

template <typename T>
void DoublyLinkedList<T>::reverse() noexcept {
  if (size() < 2)
    return;

  std::unique_ptr<Node> current_head = std::move(head_);
  Node*                 new_tail     = nullptr;

  while (current_head) {
    if (!new_tail)
      new_tail = current_head.get();

    auto detached_node = std::move(current_head);
    current_head       = std::move(detached_node->next);

    detached_node->next = std::move(head_);
    head_               = std::move(detached_node);
    head_->prev         = nullptr;

    if (head_->next) {
      head_->next->prev = head_.get();
    }
  }
  tail_ = new_tail;
}

template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::begin() noexcept {
  return iterator(head_.get());
}

template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::end() noexcept {
  return iterator(nullptr);
}

} // namespace ads::list