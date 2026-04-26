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

namespace ads::stacks {

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

template <typename T>
ArrayStack<T>::ArrayStack(size_t initial_capacity) :
    data_(nullptr, [](T* ptr) -> auto { ::operator delete[](ptr); }),
    size_(0),
    capacity_(std::max(initial_capacity, kMinCapacity)) {
  // Keep even zero-capacity requests growable through the normal push path.
  if (capacity_ > std::numeric_limits<size_t>::max() / sizeof(T)) {
    throw StackOverflowException("Stack capacity overflow");
  }

  data_.reset(static_cast<T*>(::operator new[](capacity_ * sizeof(T))));
}

template <typename T>
ArrayStack<T>::~ArrayStack() {
  clear();
}

template <typename T>
ArrayStack<T>::ArrayStack(ArrayStack&& other) noexcept :
    data_(std::move(other.data_)),
    size_(other.size_),
    capacity_(other.capacity_) {
  other.size_     = 0;
  other.capacity_ = 0;
}

template <typename T>
auto ArrayStack<T>::operator=(ArrayStack&& other) noexcept -> ArrayStack<T>& {
  if (this != &other) {
    // Raw storage does not know which slots are live, so destroy them before replacing it.
    clear();
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
template <typename... Args>
auto ArrayStack<T>::emplace(Args&&... args) -> T& {
  if (size_ == capacity_) {
    grow();
  }

  T* top_ptr = data_.get() + size_;
  std::construct_at(top_ptr, std::forward<Args>(args)...);
  ++size_;

  return *top_ptr;
}

template <typename T>
void ArrayStack<T>::push(const T& value) {
  emplace(value);
}

template <typename T>
void ArrayStack<T>::push(T&& value) {
  emplace(std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
void ArrayStack<T>::pop() {
  if (is_empty()) {
    throw StackUnderflowException("Cannot pop from empty stack");
  }

  // Explicitly destroy the top element.
  size_--;
  std::destroy_at(data_.get() + size_);

  // Optional: shrink the array if it's significantly underutilized.
  // This prevents memory waste after many pops.
  if (size_ > 0 && size_ * 4 <= capacity_ && capacity_ > kMinCapacity) {
    size_t new_capacity = std::max(capacity_ / 2, kMinCapacity);
    try {
      reallocate(new_capacity);
    } catch (const std::bad_alloc&) {
      // If reallocation fails, continue with current capacity.
      // This is a non-critical optimization - the stack remains functional
      // with the current (larger) capacity.
    }
  }
}

template <typename T>
void ArrayStack<T>::clear() noexcept {
  // Explicitly destroy all elements.
  while (size_ > 0) {
    --size_;
    std::destroy_at(data_.get() + size_);
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
  if (capacity_ > std::numeric_limits<size_t>::max() / kGrowthFactor) {
    throw StackOverflowException("Stack capacity overflow");
  }

  // A moved-from stack has zero capacity; grow it back to a usable invariant.
  const size_t new_capacity = std::max(capacity_ * kGrowthFactor, kMinCapacity);
  reallocate(new_capacity);
}

//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T>
void ArrayStack<T>::reallocate(size_t new_capacity) {
  if (new_capacity < size_) {
    new_capacity = size_;
  }

  if (new_capacity > std::numeric_limits<size_t>::max() / sizeof(T)) {
    throw StackOverflowException("Stack capacity overflow");
  }

  // Allocate raw memory with custom deleter.
  std::unique_ptr<T[], void (*)(T*)> new_data(
      static_cast<T*>(::operator new[](new_capacity * sizeof(T))), [](T* ptr) -> auto { ::operator delete[](ptr); });

  // Move/copy elements to new array with exception safety.
  size_t constructed_count = 0;
  try {
    for (; constructed_count < size_; ++constructed_count) {
      if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
        std::construct_at(new_data.get() + constructed_count, std::move(data_[constructed_count]));
      } else {
        std::construct_at(new_data.get() + constructed_count, data_[constructed_count]);
      }
    }
  } catch (...) {
    // The old stack stays authoritative, so only the partial replacement is rolled back.
    for (size_t i = 0; i < constructed_count; ++i) {
      std::destroy_at(new_data.get() + i);
    }
    throw;
  }

  // Destroy old elements only after all new elements are constructed.
  for (size_t i = 0; i < size_; ++i) {
    std::destroy_at(data_.get() + i);
  }

  // Replace the old array with the new one.
  data_     = std::move(new_data);
  capacity_ = new_capacity;
}

} // namespace ads::stacks

//===---------------------------------------------------------------------------===//
