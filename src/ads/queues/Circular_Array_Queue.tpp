//===--------------------------------------------------------------------------===//
/**
 * @file Circular_Array_Queue.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for CircularArrayQueue.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#include "../../../include/ads/queues/Circular_Array_Queue.hpp"
#include <new>
#include <utility>

using namespace ads::queue;

//===------------------- CircularArrayQueue implementation --------------------===//

template <typename T>
CircularArrayQueue<T>::CircularArrayQueue(size_t initial_capacity) :
    data_(std::make_unique_for_overwrite<T[]>(initial_capacity)), front_(0), rear_(0), size_(0), capacity_(initial_capacity) {
}

template <typename T>
CircularArrayQueue<T>::~CircularArrayQueue() {
  clear();
}

template <typename T>
CircularArrayQueue<T>::CircularArrayQueue(CircularArrayQueue&& other) noexcept :
    data_(std::move(other.data_)), front_(other.front_), rear_(other.rear_), size_(other.size_), capacity_(other.capacity_) {
  other.front_    = 0;
  other.rear_     = 0;
  other.size_     = 0;
  other.capacity_ = 0;
}

template <typename T>
CircularArrayQueue<T>& CircularArrayQueue<T>::operator=(CircularArrayQueue&& other) noexcept {
  if (this != &other) {
    clear();
    data_           = std::move(other.data_);
    front_          = other.front_;
    rear_           = other.rear_;
    size_           = other.size_;
    capacity_       = other.capacity_;
    other.front_    = 0;
    other.rear_     = 0;
    other.size_     = 0;
    other.capacity_ = 0;
  }
  return *this;
}

template <typename T>
void CircularArrayQueue<T>::enqueue(const T& value) {
  emplace(value);
}

template <typename T>
void CircularArrayQueue<T>::enqueue(T&& value) {
  emplace(std::move(value));
}

template <typename T>
template <typename... Args>
T& CircularArrayQueue<T>::emplace(Args&&... args) {
  if (is_full()) {
    grow();
  }

  // Construct the element in-place at the rear position
  T* rear_ptr = &data_[rear_];
  new (rear_ptr) T(std::forward<Args>(args)...);

  rear_ = next_index(rear_);
  size_++;

  return *rear_ptr;
}

template <typename T>
void CircularArrayQueue<T>::dequeue() {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot dequeue from empty queue");
  }

  // Explicitly destroy the front element
  data_[front_].~T();

  front_ = next_index(front_);
  size_--;

  // Optional: shrink if significantly underutilized
  if (size_ > 0 && size_ * 4 <= capacity_ && capacity_ > kMinCapacity) {
    size_t new_capacity = std::max(capacity_ / 2, kMinCapacity);
    try {
      reallocate(new_capacity);
    } catch (...) {
      // If reallocation fails, continue with current capacity
    }
  }
}

template <typename T>
T& CircularArrayQueue<T>::front() {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty queue");
  }
  return data_[front_];
}

template <typename T>
const T& CircularArrayQueue<T>::front() const {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty queue");
  }
  return data_[front_];
}

template <typename T>
T& CircularArrayQueue<T>::rear() {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access rear of empty queue");
  }
  size_t rear_element_index = prev_index(rear_);
  return data_[rear_element_index];
}

template <typename T>
const T& CircularArrayQueue<T>::rear() const {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access rear of empty queue");
  }
  size_t rear_element_index = prev_index(rear_);
  return data_[rear_element_index];
}

template <typename T>
bool CircularArrayQueue<T>::is_empty() const noexcept {
  return size_ == 0;
}

template <typename T>
size_t CircularArrayQueue<T>::size() const noexcept {
  return size_;
}

template <typename T>
void CircularArrayQueue<T>::clear() noexcept {
  // Explicitly destroy all elements in the queue
  while (!is_empty()) {
    data_[front_].~T();
    front_ = next_index(front_);
    size_--;
  }
  front_ = 0;
  rear_  = 0;
}

template <typename T>
void CircularArrayQueue<T>::reserve(size_t n) {
  if (n > capacity_) {
    reallocate(n);
  }
}

template <typename T>
void CircularArrayQueue<T>::shrink_to_fit() {
  if (size_ < capacity_ - 1) {
    size_t new_capacity = std::max(size_ + 1, kMinCapacity);
    reallocate(new_capacity);
  }
}

template <typename T>
void CircularArrayQueue<T>::grow() {
  size_t new_capacity = capacity_ * kGrowthFactor;

  // Handle potential overflow
  if (new_capacity < capacity_) {
    throw QueueOverflowException("Queue capacity overflow");
  }

  reallocate(new_capacity);
}

template <typename T>
void CircularArrayQueue<T>::reallocate(size_t new_capacity) {
  // Create new array
  auto new_data = std::make_unique_for_overwrite<T[]>(new_capacity);

  // Copy elements to new array in logical order
  size_t new_index = 0;
  size_t current   = front_;

  for (size_t i = 0; i < size_; ++i) {
    // Use move construction if T is nothrow move constructible
    if constexpr (std::is_nothrow_move_constructible_v<T>) {
      new (new_data.get() + new_index) T(std::move(data_[current]));
    } else {
      // Use copy construction as fallback for exception safety
      new (new_data.get() + new_index) T(data_[current]);
    }

    // Destroy the old element
    data_[current].~T();

    current = next_index(current);
    new_index++;
  }

  // Update the queue state
  data_     = std::move(new_data);
  capacity_ = new_capacity;
  front_    = 0;
  rear_     = size_; // Next insertion position
}

//===--------------------------------------------------------------------------===//