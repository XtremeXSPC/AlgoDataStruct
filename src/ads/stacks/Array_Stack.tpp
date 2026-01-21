//===---------------------------------------------------------------------------===//
/**
 * @file Array_Stack.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for ArrayStack.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//
#pragma once
#include "../../../include/ads/stacks/Array_Stack.hpp"

namespace ads::stack {

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

template <typename T>
ArrayStack<T>::ArrayStack(size_t initial_capacity) :
    data_(
        static_cast<T*>(::operator new[](initial_capacity * sizeof(T))),
        [](T* ptr) -> auto { ::operator delete[](ptr); }), // Custom deleter
    size_(0), capacity_(initial_capacity) {
}

template <typename T>
ArrayStack<T>::ArrayStack(ArrayStack&& other) noexcept : data_(std::move(other.data_)), size_(other.size_), capacity_(other.capacity_) {
  other.size_     = 0;
  other.capacity_ = 0;
}

template <typename T>
auto ArrayStack<T>::operator=(ArrayStack&& other) noexcept -> ArrayStack<T>& {
  if (this != &other) {
    data_           = std::move(other.data_);
    size_           = other.size_;
    capacity_       = other.capacity_;
    other.size_     = 0;
    other.capacity_ = 0;
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//
template <typename T>
void ArrayStack<T>::push(const T& value) {
  emplace(value);
}

template <typename T>
void ArrayStack<T>::push(T&& value) {
  emplace(std::move(value));
}

template <typename T>
template <typename... Args>
auto ArrayStack<T>::emplace(Args&&... args) -> T& {
  if (size_ == capacity_) {
    grow();
  }

  // Construct the element in-place at the top of the stack
  // Using placement new for perfect forwarding
  T* top_ptr = &data_[size_];
  new (top_ptr) T(std::forward<Args>(args)...);
  size_++;

  return *top_ptr;
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//
template <typename T>
void ArrayStack<T>::pop() {
  if (is_empty()) {
    throw StackUnderflowException("Cannot pop from empty stack");
  }

  // Explicitly destroy the top element
  size_--;
  data_[size_].~T();

  // Optional: shrink the array if it's significantly underutilized
  // This prevents memory waste after many pops
  if (size_ > 0 && size_ * 4 <= capacity_ && capacity_ > kMinCapacity) {
    size_t new_capacity = std::max(capacity_ / 2, kMinCapacity);
    try {
      reallocate(new_capacity);
    } catch (const std::bad_alloc&) {
      // If reallocation fails, continue with current capacity
      // This is a non-critical optimization - the stack remains functional
      // with the current (larger) capacity
    }
  }
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//
template <typename T>
auto ArrayStack<T>::top() -> T& {
  if (is_empty()) {
    throw StackUnderflowException("Cannot access top of empty stack");
  }
  return data_[size_ - 1];
}

template <typename T>
auto ArrayStack<T>::top() const -> const T& {
  if (is_empty()) {
    throw StackUnderflowException("Cannot access top of empty stack");
  }
  return data_[size_ - 1];
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//
template <typename T>
auto ArrayStack<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T>
auto ArrayStack<T>::size() const noexcept -> size_t {
  return size_;
}

template <typename T>
void ArrayStack<T>::clear() noexcept {
  // Explicitly destroy all elements
  while (size_ > 0) {
    size_--;
    data_[size_].~T();
  }
}

//===--------------------------- CAPACITY MANAGEMENT ---------------------------===//
template <typename T>
void ArrayStack<T>::reserve(size_t n) {
  if (n > capacity_) {
    reallocate(n);
  }
}

template <typename T>
void ArrayStack<T>::shrink_to_fit() {
  if (size_ < capacity_) {
    size_t new_capacity = std::max(size_, kMinCapacity);
    reallocate(new_capacity);
  }
}

template <typename T>
void ArrayStack<T>::grow() {
  // Check for overflow BEFORE multiplication
  if (capacity_ > std::numeric_limits<size_t>::max() / kGrowthFactor) {
    throw StackOverflowException("Stack capacity overflow");
  }

  size_t new_capacity = capacity_ * kGrowthFactor;
  reallocate(new_capacity);
}

template <typename T>
void ArrayStack<T>::reallocate(size_t new_capacity) {
  // Allocate raw memory with custom deleter
  std::unique_ptr<T[], void (*)(T*)> new_data(
      static_cast<T*>(::operator new[](new_capacity * sizeof(T))), [](T* ptr) { ::operator delete[](ptr); });

  // Move/copy elements to new array with exception safety
  size_t constructed_count = 0;
  try {
    for (; constructed_count < size_; ++constructed_count) {
      // Use move construction if T is nothrow move constructible
      if constexpr (std::is_nothrow_move_constructible_v<T>) {
        new (new_data.get() + constructed_count) T(std::move(data_[constructed_count]));
      } else {
        // Use copy construction as fallback for exception safety
        new (new_data.get() + constructed_count) T(data_[constructed_count]);
      }
    }
  } catch (...) {
    // Destroy already-constructed elements in new array
    for (size_t i = 0; i < constructed_count; ++i) {
      new_data[i].~T();
    }
    throw;
  }

  // Destroy old elements only after all new elements are constructed
  for (size_t i = 0; i < size_; ++i) {
    data_[i].~T();
  }

  // Replace the old array with the new one
  data_     = std::move(new_data);
  capacity_ = new_capacity;
}

} // namespace ads::stack

//===---------------------------------------------------------------------------===//
