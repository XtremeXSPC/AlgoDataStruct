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

using namespace ads::list;

//===----- Implementazione Iteratore (pulita) -----====//
template <typename T>
typename DoublyLinkedList<T>::iterator::reference DoublyLinkedList<T>::iterator::operator*() const {
  return node_ptr_->data;
}

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

//===----- Implementazione DoublyLinkedList -----====//

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

template <typename T>
template <typename U>
typename DoublyLinkedList<T>::iterator
DoublyLinkedList<T>::insert_impl(DoublyLinkedList<T>& list, typename DoublyLinkedList<T>::iterator pos, U&& value) {
  // 1) casi limite
  if (pos == list.begin()) {
    list.emplace_front(std::forward<U>(value));
    return list.begin();
  }
  if (pos == list.end()) {
    list.emplace_back(std::forward<U>(value));
    return typename DoublyLinkedList<T>::iterator(list.tail_);
  }

  // 2) alias al Node interno
  using Node = typename DoublyLinkedList<T>::Node;

  // 3) inserimento nel mezzo
  Node* pos_node  = pos.node_ptr_;
  Node* prev_node = pos_node->prev;

  auto                                   new_node = std::make_unique<Node>(prev_node, std::forward<U>(value));
  typename DoublyLinkedList<T>::iterator new_it(new_node.get());

  // ricolleghiamo
  new_node->next  = std::move(prev_node->next);
  prev_node->next = std::move(new_node);
  pos_node->prev  = prev_node->next.get();

  // aggiorniamo la size
  list.size_++;

  return new_it;
}

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

  using Node      = typename DoublyLinkedList<T>::Node;
  Node* pos_node  = pos.node_ptr_;
  Node* prev_node = pos_node->prev;

  // 1) costruiamo il nuovo nodo
  auto new_node = std::make_unique<Node>(prev_node, value);

  // 2) ricolleghiamo avanti
  new_node->next = std::move(prev_node->next);

  // 3) IMPORTANTISSIMO: aggiorniamo anche il prev del nodo che segue il new_node
  if (new_node->next)
    new_node->next->prev = new_node.get();

  // 4) infine inseriamo new_node nella lista
  prev_node->next = std::move(new_node);

  // 5) aggiorniamo size
  ++size_;

  // restituiamo l’iteratore sul nodo appena inserito
  return iterator(prev_node->next.get());
}

template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::insert(iterator pos, T&& value) {
  if (pos == begin()) {
    push_front(value);
    return begin();
  }
  if (pos == end()) {
    push_back(value);
    return iterator(tail_);
  }

  using Node      = typename DoublyLinkedList<T>::Node;
  Node* pos_node  = pos.node_ptr_;
  Node* prev_node = pos_node->prev;

  // 1) costruiamo il nuovo nodo
  auto new_node = std::make_unique<Node>(prev_node, value);

  // 2) ricolleghiamo avanti
  new_node->next = std::move(prev_node->next);

  // 3) IMPORTANTISSIMO: aggiorniamo anche il prev del nodo che segue il new_node
  if (new_node->next)
    new_node->next->prev = new_node.get();

  // 4) infine inseriamo new_node nella lista
  prev_node->next = std::move(new_node);

  // 5) aggiorniamo size
  ++size_;

  // restituiamo l’iteratore sul nodo appena inserito
  return iterator(prev_node->next.get());
}

template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::erase(iterator pos) {
  if (pos == end() || is_empty()) {
    throw std::logic_error("cannot erase invalid iterator");
  }

  // 1) Prendi il nodo da rimuovere e il suo successore
  Node* node_to_remove = pos.node_ptr_;
  Node* next_node      = node_to_remove->next.get();

  // 2) Ricollega il prev del successore (se esiste)
  if (next_node) {
    next_node->prev = node_to_remove->prev;
  } else {
    // se non c'è successore, stiamo cancellando la tail
    tail_ = node_to_remove->prev;
  }

  // 3) Ricollega il next del precedente (se esiste)
  if (node_to_remove->prev) {
    node_to_remove->prev->next = std::move(node_to_remove->next);
  } else {
    // se non c'è precedente, stiamo cancellando la head
    head_ = std::move(node_to_remove->next);
  }

  // 4) Aggiorna la size
  --size_;

  // 5) Restituisci l’iteratore al successore (posizione “next”)
  return iterator(next_node);
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
template <typename T>
auto DoublyLinkedList<T>::begin() const noexcept -> iterator {
  return iterator(head_.get());
}

template <typename T>
auto DoublyLinkedList<T>::end() const noexcept -> iterator {
  return iterator(nullptr);
}
