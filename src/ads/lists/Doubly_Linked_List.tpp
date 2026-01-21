//===---------------------------------------------------------------------------===//
/**
 * @file Doubly_Linked_List.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for DoublyLinkedList.
 * @version 0.2
 * @date 2025-06-30
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//
#pragma once
#include "../../../include/ads/lists/Doubly_Linked_List.hpp"

namespace ads::lists {

//===------------------------- ITERATOR IMPLEMENTATION -------------------------===//

template <typename T>
auto DoublyLinkedList<T>::iterator::operator*() const -> typename DoublyLinkedList<T>::iterator::reference {
  return node_ptr_->data;
}

template <typename T>
auto DoublyLinkedList<T>::iterator::operator->() const -> typename DoublyLinkedList<T>::iterator::pointer {
  return &node_ptr_->data;
}

template <typename T>
auto DoublyLinkedList<T>::iterator::operator++() -> typename DoublyLinkedList<T>::iterator& {
  node_ptr_ = node_ptr_->next.get();
  return *this;
}

template <typename T>
auto DoublyLinkedList<T>::iterator::operator++(int) -> typename DoublyLinkedList<T>::iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <typename T>
auto DoublyLinkedList<T>::iterator::operator--() -> typename DoublyLinkedList<T>::iterator& {
  if (node_ptr_ == nullptr) { // -- from end()
    // Decrementing end() requires a valid list pointer
    if (list_ptr_ != nullptr) {
      node_ptr_ = list_ptr_->tail_;
    }
  } else {
    node_ptr_ = node_ptr_->prev;
  }
  return *this;
}

template <typename T>
auto DoublyLinkedList<T>::iterator::operator--(int) -> typename DoublyLinkedList<T>::iterator {
  iterator temp = *this;
  --(*this);
  return temp;
}

//===---------------------- CONST_ITERATOR IMPLEMENTATION ----------------------===//

template <typename T>
auto DoublyLinkedList<T>::const_iterator::operator*() const -> typename DoublyLinkedList<T>::const_iterator::reference {
  return node_ptr_->data;
}

template <typename T>
auto DoublyLinkedList<T>::const_iterator::operator->() const -> typename DoublyLinkedList<T>::const_iterator::pointer {
  return &node_ptr_->data;
}

template <typename T>
auto DoublyLinkedList<T>::const_iterator::operator++() -> typename DoublyLinkedList<T>::const_iterator& {
  node_ptr_ = node_ptr_->next.get();
  return *this;
}

template <typename T>
auto DoublyLinkedList<T>::const_iterator::operator++(int) -> typename DoublyLinkedList<T>::const_iterator {
  const_iterator temp = *this;
  ++(*this);
  return temp;
}

template <typename T>
auto DoublyLinkedList<T>::const_iterator::operator--() -> typename DoublyLinkedList<T>::const_iterator& {
  if (node_ptr_ == nullptr) { // -- from cend()
    // Decrementing cend() requires a valid list pointer
    if (list_ptr_ != nullptr) {
      node_ptr_ = list_ptr_->tail_;
    }
  } else {
    node_ptr_ = node_ptr_->prev;
  }
  return *this;
}

template <typename T>
auto DoublyLinkedList<T>::const_iterator::operator--(int) -> typename DoublyLinkedList<T>::const_iterator {
  const_iterator temp = *this;
  --(*this);
  return temp;
}

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

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
auto DoublyLinkedList<T>::operator=(DoublyLinkedList&& other) noexcept -> DoublyLinkedList<T>& {
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
auto DoublyLinkedList<T>::emplace_front(Args&&... args) {
  auto newNode = std::make_unique<Node>(nullptr, std::forward<Args>(args)...);
  if (head_) {
    head_->prev   = newNode.get();
    newNode->next = std::move(head_);
  } else { // List was empty
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
template <typename... Args>
auto DoublyLinkedList<T>::emplace_back(Args&&... args) {
  if (!tail_) { // Empty list
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

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
void DoublyLinkedList<T>::pop_front() {
  if (is_empty()) {
    throw ListException("pop_front on empty list");
  }
  head_ = std::move(head_->next);
  if (head_) {
    head_->prev = nullptr;
  } else { // The list became empty
    tail_ = nullptr;
  }
  size_--;
}

template <typename T>
void DoublyLinkedList<T>::pop_back() {
  if (is_empty()) {
    throw ListException("pop_back on empty list");
  }
  if (size_ == 1) {
    pop_front();
  } else {
    tail_ = tail_->prev;
    tail_->next.reset(); // Deallocates the old tail node
    size_--;
  }
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto DoublyLinkedList<T>::front() -> T& {
  if (is_empty()) {
    throw ListException("front on empty list");
  }
  return head_->data;
}

template <typename T>
auto DoublyLinkedList<T>::front() const -> const T& {
  if (is_empty()) {
    throw ListException("front on empty list");
  }
  return head_->data;
}

template <typename T>
auto DoublyLinkedList<T>::back() -> T& {
  if (is_empty()) {
    throw ListException("back on empty list");
  }
  return tail_->data;
}

template <typename T>
auto DoublyLinkedList<T>::back() const -> const T& {
  if (is_empty()) {
    throw ListException("back on empty list");
  }
  return tail_->data;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto DoublyLinkedList<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T>
auto DoublyLinkedList<T>::size() const noexcept -> size_t {
  return size_;
}

//===------------------------- MODIFICATION OPERATIONS -------------------------===//

template <typename T>
void DoublyLinkedList<T>::clear() noexcept {
  head_.reset(); // The chain destruction of "unique_ptr" deallocates all nodes.
  tail_ = nullptr;
  size_ = 0;
}

template <typename T>
auto DoublyLinkedList<T>::insert(iterator pos, const T& value) {
  return insert(pos, T(value)); // Calls the T&& version
}

template <typename T>
auto DoublyLinkedList<T>::insert(iterator pos, T&& value) {
  // Edge cases: insertion at head or tail
  if (pos == begin()) {
    push_front(std::move(value));
    return begin();
  }
  if (pos == end()) {
    push_back(std::move(value));
    // After push_back, `tail_` points to the new node
    return iterator(tail_, this);
  }

  // Insertion in the middle
  Node* pos_node  = pos.node_ptr_;
  Node* prev_node = pos_node->prev;

  // 1. Create the new node, which points to the previous node
  auto     new_node = std::make_unique<Node>(prev_node, std::move(value));
  iterator new_it(new_node.get(), this);

  // 2. The new node takes ownership of the pointer to the next node
  new_node->next = std::move(prev_node->next);
  // 3. The `pos` node (next) now must point back to the new node
  pos_node->prev = new_node.get();
  // 4. The previous node now points to the new node, transferring ownership
  prev_node->next = std::move(new_node);

  size_++;
  return new_it;
}

template <typename T>
auto DoublyLinkedList<T>::erase(iterator pos) {
  if (pos == end() || is_empty()) {
    throw ListException("cannot erase an invalid or end iterator");
  }

  // 1. Identify the nodes before and after the one to be removed.
  Node* node_to_remove = pos.node_ptr_;
  Node* prev_node      = node_to_remove->prev;
  Node* next_node      = node_to_remove->next.get();

  // 2. Reconnect the `next` pointer of the previous node.
  if (prev_node) {
    // If there is a predecessor, its `next` must point to the successor of the node to be removed.
    // std::move transfers ownership of the unique_ptr.
    // This is when node_to_remove is deallocated.
    prev_node->next = std::move(node_to_remove->next);
  } else {
    // If there is no predecessor, we are removing the head. Update `head_`.
    head_ = std::move(node_to_remove->next);
  }

  // 3. Reconnect the `prev` pointer of the next node.
  if (next_node) {
    // If there is a successor, its `prev` must point to the predecessor of the removed node.
    next_node->prev = prev_node;
  } else {
    // If there is no successor, we are removing the tail. Update `tail_`.
    tail_ = prev_node;
  }

  // 4. Update the size and return the iterator to the next node.
  size_--;
  return iterator(next_node, this);
}

template <typename T>
void DoublyLinkedList<T>::reverse() noexcept {
  if (size() < 2) {
    return;
  }

  // The approach is to detach nodes from the old list
  // and insert them one by one at the head of a new list (reusing head_).
  // `current_list` holds the nodes not yet processed.
  std::unique_ptr<Node> current_list = std::move(head_);
  tail_                              = current_list.get(); // The old head becomes the new tail

  while (current_list) {
    // 1. Detach the node at the head of `current_list`
    auto detached_node = std::move(current_list);
    // 2. Advance `current_list` to the next node
    current_list = std::move(detached_node->next);

    // 3. Insert `detached_node` at the head of the resulting list (managed by `head_`)
    detached_node->next = std::move(head_);
    if (detached_node->next) { // If the list was not empty
      detached_node->next->prev = detached_node.get();
    }
    head_       = std::move(detached_node);
    head_->prev = nullptr;
  }
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

// (non-const)
template <typename T>
auto DoublyLinkedList<T>::begin() noexcept -> typename DoublyLinkedList<T>::iterator {
  return iterator(head_.get(), this);
}

template <typename T>
auto DoublyLinkedList<T>::end() noexcept -> typename DoublyLinkedList<T>::iterator {
  return iterator(nullptr, this);
}

// (const)
template <typename T>
auto DoublyLinkedList<T>::begin() const noexcept -> typename DoublyLinkedList<T>::const_iterator {
  return const_iterator(head_.get(), this);
}

template <typename T>
auto DoublyLinkedList<T>::end() const noexcept -> typename DoublyLinkedList<T>::const_iterator {
  return const_iterator(nullptr, this);
}

template <typename T>
auto DoublyLinkedList<T>::cbegin() const noexcept -> typename DoublyLinkedList<T>::const_iterator {
  return begin();
}

template <typename T>
auto DoublyLinkedList<T>::cend() const noexcept -> typename DoublyLinkedList<T>::const_iterator {
  return end();
}

} // namespace ads::lists

//===---------------------------------------------------------------------------===//
