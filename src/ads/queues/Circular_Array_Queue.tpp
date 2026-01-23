//===---------------------------------------------------------------------------===//
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
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/queues/Circular_Array_Queue.hpp"

namespace ads::queues {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
CircularArrayQueue<T>::CircularArrayQueue(size_t initial_capacity) :
    data_(
        static_cast<T*>(::operator new[](initial_capacity * sizeof(T))),
        [](T* ptr) -> auto { ::operator delete[](ptr); }), // Custom deleter.
    front_(0), rear_(0), size_(0), capacity_(initial_capacity) {
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
auto CircularArrayQueue<T>::operator=(CircularArrayQueue&& other) noexcept -> CircularArrayQueue<T>& {
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

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename T>
template <typename... Args>
auto CircularArrayQueue<T>::emplace(Args&&... args) -> T& {
  if (is_full()) {
    grow();
  }

  // Construct the element in-place at the rear position.
  T* rear_ptr = &data_[rear_];
  new (rear_ptr) T(std::forward<Args>(args)...);

  rear_ = next_index(rear_);
  size_++;

  return *rear_ptr;
}

template <typename T>
void CircularArrayQueue<T>::enqueue(const T& value) {
  emplace(value);
}

template <typename T>
void CircularArrayQueue<T>::enqueue(T&& value) {
  emplace(std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
void CircularArrayQueue<T>::dequeue() {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot dequeue from empty queue");
  }

  // Explicitly destroy the front element.
  data_[front_].~T();

  front_ = next_index(front_);
  size_--;

  // Optional: shrink if significantly underutilized.
  if (size_ > 0 && size_ * 4 <= capacity_ && capacity_ > kMinCapacity) {
    size_t new_capacity = std::max(capacity_ / 2, kMinCapacity);
    try {
      reallocate(new_capacity);
    } catch (...) {
      // If reallocation fails, continue with current capacity.
    }
  }
}

template <typename T>
void CircularArrayQueue<T>::clear() noexcept {
  // Explicitly destroy all elements in the queue.
  while (!is_empty()) {
    data_[front_].~T();
    front_ = next_index(front_);
    size_--;
  }
  front_ = 0;
  rear_  = 0;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto CircularArrayQueue<T>::front() -> T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty queue");
  }
  return data_[front_];
}

template <typename T>
auto CircularArrayQueue<T>::front() const -> const T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty queue");
  }
  return data_[front_];
}

template <typename T>
auto CircularArrayQueue<T>::rear() -> T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access rear of empty queue");
  }
  size_t rear_element_index = prev_index(rear_);
  return data_[rear_element_index];
}

template <typename T>
auto CircularArrayQueue<T>::rear() const -> const T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access rear of empty queue");
  }
  size_t rear_element_index = prev_index(rear_);
  return data_[rear_element_index];
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto CircularArrayQueue<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T>
auto CircularArrayQueue<T>::size() const noexcept -> size_t {
  return size_;
}

//===--------------------------- CAPACITY OPERATIONS ---------------------------===//

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
  // Check for overflow BEFORE multiplication.
  if (capacity_ > std::numeric_limits<size_t>::max() / kGrowthFactor) {
    throw QueueOverflowException("Queue capacity overflow");
  }

  size_t new_capacity = capacity_ * kGrowthFactor;
  reallocate(new_capacity);
}

template <typename T>
void CircularArrayQueue<T>::reallocate(size_t new_capacity) {
  // Allocate raw memory with custom deleter.
  std::unique_ptr<T[], void (*)(T*)> new_data(
      static_cast<T*>(::operator new[](new_capacity * sizeof(T))), [](T* ptr) -> auto { ::operator delete[](ptr); });

  // Copy elements to new array in logical order with exception safety.
  size_t constructed_count = 0;
  size_t current           = front_;

  try {
    for (; constructed_count < size_; ++constructed_count) {
      if constexpr (std::is_nothrow_move_constructible_v<T>) {
        new (new_data.get() + constructed_count) T(std::move(data_[current]));
      } else {
        new (new_data.get() + constructed_count) T(data_[current]);
      }
      current = next_index(current);
    }
  } catch (...) {
    // Destroy already-constructed elements in new array.
    for (size_t i = 0; i < constructed_count; ++i) {
      new_data[i].~T();
    }
    throw;
  }

  // Destroy old elements only after all new elements are constructed.
  current = front_;
  for (size_t i = 0; i < size_; ++i) {
    data_[current].~T();
    current = next_index(current);
  }

  data_     = std::move(new_data);
  capacity_ = new_capacity;
  front_    = 0;
  rear_     = size_;
}

} // namespace ads::queues

//===---------------------------------------------------------------------------===//
