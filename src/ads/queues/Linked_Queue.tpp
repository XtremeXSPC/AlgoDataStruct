//===---------------------------------------------------------------------------===//
/**
 * @file Linked_Queue.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for LinkedQueue.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//
#pragma once
#include "../../../include/ads/queues/Linked_Queue.hpp"

namespace ads::queue {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
LinkedQueue<T>::LinkedQueue() noexcept : front_(nullptr), rear_(nullptr), size_(0) {
}

template <typename T>
LinkedQueue<T>::~LinkedQueue() {
  // The chain of unique_ptr will automatically deallocate all nodes
  // However, for deep queues, this recursive destruction might cause
  // stack overflow. So we manually iterate to prevent this.
  clear();
}

template <typename T>
LinkedQueue<T>::LinkedQueue(LinkedQueue&& other) noexcept : front_(std::move(other.front_)), rear_(other.rear_), size_(other.size_) {
  other.rear_ = nullptr;
  other.size_ = 0;
}

template <typename T>
auto LinkedQueue<T>::operator=(LinkedQueue&& other) noexcept -> LinkedQueue<T>& {
  if (this != &other) {
    clear(); // Clear existing elements first
    front_      = std::move(other.front_);
    rear_       = other.rear_;
    size_       = other.size_;
    other.rear_ = nullptr;
    other.size_ = 0;
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename T>
void LinkedQueue<T>::enqueue(const T& value) {
  emplace(value);
}

template <typename T>
void LinkedQueue<T>::enqueue(T&& value) {
  emplace(std::move(value));
}

template <typename T>
template <typename... Args>
auto LinkedQueue<T>::emplace(Args&&... args) -> T& {
  // Create new node with forwarded arguments
  auto new_node = std::make_unique<Node>(std::forward<Args>(args)...);

  // Get reference to data before moving the unique_ptr
  T& data_ref = new_node->data;

  // Get raw pointer to the new node (will become the new rear)
  Node* new_rear = new_node.get();

  if (is_empty()) {
    // First element: both front and rear point to it
    front_ = std::move(new_node);
    rear_  = new_rear;
  } else {
    // Add to rear: link current rear to new node
    rear_->next = std::move(new_node);
    rear_       = new_rear;
  }

  size_++;

  return data_ref;
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
void LinkedQueue<T>::dequeue() {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot dequeue from empty queue");
  }

  // Move front to the next node, automatically deallocating the old front
  front_ = std::move(front_->next);

  // If queue became empty, update rear
  if (!front_) {
    rear_ = nullptr;
  }

  size_--;
}

template <typename T>
void LinkedQueue<T>::clear() noexcept {
  // Iterative destruction to avoid potential stack overflow
  // with recursive unique_ptr destruction
  while (front_) {
    front_ = std::move(front_->next);
  }
  rear_ = nullptr;
  size_ = 0;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto LinkedQueue<T>::front() -> T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty queue");
  }
  return front_->data;
}

template <typename T>
auto LinkedQueue<T>::front() const -> const T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty queue");
  }
  return front_->data;
}

template <typename T>
auto LinkedQueue<T>::rear() -> T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access rear of empty queue");
  }
  return rear_->data;
}

template <typename T>
auto LinkedQueue<T>::rear() const -> const T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access rear of empty queue");
  }
  return rear_->data;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto LinkedQueue<T>::is_empty() const noexcept -> bool {
  return front_ == nullptr;
}

template <typename T>
auto LinkedQueue<T>::size() const noexcept -> size_t {
  return size_;
}

} // namespace ads::queue

//===---------------------------------------------------------------------------===//
