//===--------------------------------------------------------------------------===//
/**
 * @file Linked_Stack.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for LinkedStack.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once
#include "../../../include/ads/stacks/Linked_Stack.hpp"

namespace ads::stack {

//===------------------- LinkedStack implementation --------------------===//

template <typename T>
LinkedStack<T>::LinkedStack() noexcept : head_(nullptr), size_(0) {
}

template <typename T>
LinkedStack<T>::~LinkedStack() {
  // The chain of unique_ptr will automatically deallocate all nodes
  // However, for deep stacks, this recursive destruction might cause
  // stack overflow. So we manually iterate to prevent this.
  clear();
}

template <typename T>
LinkedStack<T>::LinkedStack(LinkedStack&& other) noexcept : head_(std::move(other.head_)), size_(other.size_) {
  other.size_ = 0;
}

template <typename T>
auto LinkedStack<T>::operator=(LinkedStack&& other) noexcept -> LinkedStack<T>& {
  if (this != &other) {
    clear(); // Clear existing elements first
    head_       = std::move(other.head_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

template <typename T>
void LinkedStack<T>::push(const T& value) {
  emplace(value);
}

template <typename T>
void LinkedStack<T>::push(T&& value) {
  emplace(std::move(value));
}

template <typename T>
template <typename... Args>
auto LinkedStack<T>::emplace(Args&&... args) -> T& {
  // Create new node with forwarded arguments
  auto new_node = std::make_unique<Node>(std::forward<Args>(args)...);

  // Get reference to data before moving the unique_ptr
  T& data_ref = new_node->data;

  // Link the new node to the current head
  new_node->next = std::move(head_);

  // Make the new node the head
  head_ = std::move(new_node);

  size_++;

  return data_ref;
}

template <typename T>
void LinkedStack<T>::pop() {
  if (is_empty()) {
    throw StackUnderflowException("Cannot pop from empty stack");
  }

  // Move head to the next node, automatically deallocating the old head
  head_ = std::move(head_->next);
  size_--;
}

template <typename T>
auto LinkedStack<T>::top() -> T& {
  if (is_empty()) {
    throw StackUnderflowException("Cannot access top of empty stack");
  }
  return head_->data;
}

template <typename T>
auto LinkedStack<T>::top() const -> const T& {
  if (is_empty()) {
    throw StackUnderflowException("Cannot access top of empty stack");
  }
  return head_->data;
}

template <typename T>
auto LinkedStack<T>::is_empty() const noexcept -> bool {
  return head_ == nullptr;
}

template <typename T>
auto LinkedStack<T>::size() const noexcept -> size_t {
  return size_;
}

template <typename T>
void LinkedStack<T>::clear() noexcept {
  // Iterative destruction to avoid potential stack overflow
  // with recursive unique_ptr destruction
  while (head_) {
    head_ = std::move(head_->next);
  }
  size_ = 0;
}

} // namespace ads::stack

//===--------------------------------------------------------------------------===//
