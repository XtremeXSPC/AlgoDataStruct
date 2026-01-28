//===---------------------------------------------------------------------------===//
/**
 * @file Dynamic_Array.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for DynamicArray.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/arrays/Dynamic_Array.hpp"

namespace ads::arrays {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
DynamicArray<T>::DynamicArray(size_t initial_capacity) :
    data_(nullptr, [](T* ptr) -> auto { ::operator delete[](ptr); }), size_(0), capacity_(std::max(initial_capacity, kMinCapacity)) {
  if (capacity_ > std::numeric_limits<size_t>::max() / sizeof(T)) {
    throw ArrayOverflowException("DynamicArray capacity overflow");
  }

  data_.reset(static_cast<T*>(::operator new[](capacity_ * sizeof(T))));
}

template <typename T>
DynamicArray<T>::DynamicArray(std::initializer_list<T> values) :
    data_(nullptr, [](T* ptr) -> auto { ::operator delete[](ptr); }), size_(0), capacity_(std::max(values.size(), kMinCapacity)) {
  if (capacity_ > std::numeric_limits<size_t>::max() / sizeof(T)) {
    throw ArrayOverflowException("DynamicArray capacity overflow");
  }

  // Allocate raw memory.
  data_.reset(static_cast<T*>(::operator new[](capacity_ * sizeof(T))));

  // Construct elements from initializer list.
  size_t constructed = 0;
  try {
    for (const auto& value : values) {
      std::construct_at(data_.get() + constructed, value);
      ++constructed;
    }
  } catch (...) {
    // Rollback constructed elements on failure.
    for (size_t i = 0; i < constructed; ++i) {
      std::destroy_at(data_.get() + i);
    }
    throw;
  }

  size_ = values.size();
}

template <typename T>
DynamicArray<T>::DynamicArray(size_t count, const T& value) :
    data_(nullptr, [](T* ptr) -> auto { ::operator delete[](ptr); }), size_(0), capacity_(std::max(count, kMinCapacity)) {
  if (capacity_ > std::numeric_limits<size_t>::max() / sizeof(T)) {
    throw ArrayOverflowException("DynamicArray capacity overflow");
  }

  // Allocate raw memory.
  data_.reset(static_cast<T*>(::operator new[](capacity_ * sizeof(T))));

  // Construct elements with the given value.
  size_t constructed = 0;
  try {
    for (; constructed < count; ++constructed) {
      std::construct_at(data_.get() + constructed, value);
    }
  } catch (...) {
    // Rollback constructed elements on failure.
    for (size_t i = 0; i < constructed; ++i) {
      std::destroy_at(data_.get() + i);
    }
    throw;
  }

  size_ = count;
}

template <typename T>
DynamicArray<T>::DynamicArray(DynamicArray&& other) noexcept :
    data_(std::move(other.data_)), size_(other.size_), capacity_(other.capacity_) {
  other.size_     = 0;
  other.capacity_ = 0;
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
  clear();
}

template <typename T>
auto DynamicArray<T>::operator=(DynamicArray&& other) noexcept -> DynamicArray<T>& {
  if (this != &other) {
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
auto DynamicArray<T>::emplace_back(Args&&... args) -> T& {
  ensure_capacity(size_ + 1);

  T* element_ptr = data_.get() + size_;
  std::construct_at(element_ptr, std::forward<Args>(args)...);
  ++size_;
  return *element_ptr;
}

template <typename T>
auto DynamicArray<T>::push_back(const T& value) -> void {
  emplace_back(value);
}

template <typename T>
auto DynamicArray<T>::push_back(T&& value) -> void {
  emplace_back(std::move(value));
}

template <typename T>
template <typename... Args>
auto DynamicArray<T>::emplace(size_t index, Args&&... args) -> T& {
  if (index > size_) {
    throw ArrayOutOfRangeException("insert position out of range");
  }

  // Special case: inserting at the end.
  if (index == size_) {
    return emplace_back(std::forward<Args>(args)...);
  }

  // Ensure enough capacity.
  ensure_capacity(size_ + 1);

  // Shift elements right to make space.
  T* data_ptr = data_.get();
  // Create one uninitialized slot at the end by move-constructing the last element.
  std::construct_at(data_ptr + size_, std::move_if_noexcept(data_ptr[size_ - 1]));
  // Shift the remaining elements one position to the right.
  // std::move_backward enables optimized bulk moves for trivially movable types.
  std::move_backward(data_ptr + index, data_ptr + size_ - 1, data_ptr + size_);

  // Replace the moved-from element at index with the new one.
  // Destroy first to avoid an extra temporary + assignment.
  std::destroy_at(data_ptr + index);
  std::construct_at(data_ptr + index, std::forward<Args>(args)...);
  ++size_;
  return data_ptr[index];
}

template <typename T>
auto DynamicArray<T>::insert(size_t index, const T& value) -> void {
  emplace(index, value);
}

template <typename T>
auto DynamicArray<T>::insert(size_t index, T&& value) -> void {
  emplace(index, std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
auto DynamicArray<T>::pop_back() -> void {
  if (is_empty()) {
    throw ArrayUnderflowException("pop_back on empty array");
  }

  // Destroy the last element.
  std::destroy_at(data_.get() + size_ - 1);
  --size_;

  // Optional: shrink if significantly underutilized.
  if (size_ > 0 && size_ * 4 <= capacity_ && capacity_ > kMinCapacity) {
    const size_t new_capacity = std::max(capacity_ / 2, kMinCapacity);
    try {
      reallocate(new_capacity);
    } catch (const std::bad_alloc&) {
      // Shrinking is an optimization - silently ignore allocation failures.
    }
  }
}

template <typename T>
auto DynamicArray<T>::erase(size_t index) -> void {
  if (index >= size_) {
    throw ArrayOutOfRangeException("erase position out of range");
  }

  // Shift elements left to fill the gap.
  T* data_ptr = data_.get();
  // std::move allows the library to optimize for trivially movable types.
  std::move(data_ptr + index + 1, data_ptr + size_, data_ptr + index);

  // Destroy the now-unused last element.
  std::destroy_at(data_.get() + size_ - 1);
  --size_;

  // Optional: shrink if significantly underutilized.
  if (size_ > 0 && size_ * 4 <= capacity_ && capacity_ > kMinCapacity) {
    const size_t new_capacity = std::max(capacity_ / 2, kMinCapacity);
    try {
      reallocate(new_capacity);
    } catch (const std::bad_alloc&) {
      // Shrinking is an optimization - silently ignore allocation failures.
    }
  }
}

template <typename T>
auto DynamicArray<T>::clear() noexcept -> void {
  // Skip destructor loop for trivially destructible types.
  if constexpr (!std::is_trivially_destructible_v<T>) {
    for (size_t i = 0; i < size_; ++i) {
      std::destroy_at(data_.get() + i);
    }
  }
  size_ = 0;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto DynamicArray<T>::operator[](size_t index) -> T& {
  return data_.get()[index];
}

template <typename T>
auto DynamicArray<T>::operator[](size_t index) const -> const T& {
  return data_.get()[index];
}

template <typename T>
auto DynamicArray<T>::at(size_t index) -> T& {
  if (index >= size_) {
    throw ArrayOutOfRangeException("index out of range");
  }
  return data_.get()[index];
}

template <typename T>
auto DynamicArray<T>::at(size_t index) const -> const T& {
  if (index >= size_) {
    throw ArrayOutOfRangeException("index out of range");
  }
  return data_.get()[index];
}

template <typename T>
auto DynamicArray<T>::front() -> T& {
  if (is_empty()) {
    throw ArrayUnderflowException("front on empty array");
  }
  return data_.get()[0];
}

template <typename T>
auto DynamicArray<T>::front() const -> const T& {
  if (is_empty()) {
    throw ArrayUnderflowException("front on empty array");
  }
  return data_.get()[0];
}

template <typename T>
auto DynamicArray<T>::back() -> T& {
  if (is_empty()) {
    throw ArrayUnderflowException("back on empty array");
  }
  return data_.get()[size_ - 1];
}

template <typename T>
auto DynamicArray<T>::back() const -> const T& {
  if (is_empty()) {
    throw ArrayUnderflowException("back on empty array");
  }
  return data_.get()[size_ - 1];
}

template <typename T>
auto DynamicArray<T>::data() noexcept -> T* {
  return data_.get();
}

template <typename T>
auto DynamicArray<T>::data() const noexcept -> const T* {
  return data_.get();
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto DynamicArray<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T>
auto DynamicArray<T>::size() const noexcept -> size_t {
  return size_;
}

template <typename T>
auto DynamicArray<T>::capacity() const noexcept -> size_t {
  return capacity_;
}

//===--------------------------- CAPACITY OPERATIONS ---------------------------===//

template <typename T>
auto DynamicArray<T>::reserve(size_t new_capacity) -> void {
  if (new_capacity > capacity_) {
    reallocate(new_capacity);
  }
}

template <typename T>
auto DynamicArray<T>::shrink_to_fit() -> void {
  if (size_ < capacity_) {
    const size_t new_capacity = std::max(size_, kMinCapacity);
    if (new_capacity != capacity_) {
      reallocate(new_capacity);
    }
  }
}

template <typename T>
auto DynamicArray<T>::resize(size_t new_size) -> void
  requires std::default_initializable<T>
{
  // Handle shrinking the array.
  if (new_size < size_) {
    for (size_t i = new_size; i < size_; ++i) {
      std::destroy_at(data_.get() + i);
    }
    size_ = new_size;
    return;
  }

  // Handle growing the array.
  if (new_size > size_) {
    ensure_capacity(new_size);
    for (size_t i = size_; i < new_size; ++i) {
      std::construct_at(data_.get() + i);
    }
    size_ = new_size;
  }
}

template <typename T>
auto DynamicArray<T>::resize(size_t new_size, const T& value) -> void {
  if (new_size < size_) {
    for (size_t i = new_size; i < size_; ++i) {
      std::destroy_at(data_.get() + i);
    }
    size_ = new_size;
    return;
  }

  if (new_size > size_) {
    ensure_capacity(new_size);
    for (size_t i = size_; i < new_size; ++i) {
      std::construct_at(data_.get() + i, value);
    }
    size_ = new_size;
  }
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <typename T>
auto DynamicArray<T>::begin() noexcept -> iterator {
  return data_.get();
}

template <typename T>
auto DynamicArray<T>::end() noexcept -> iterator {
  return data_.get() + size_;
}

template <typename T>
auto DynamicArray<T>::begin() const noexcept -> const_iterator {
  return data_.get();
}

template <typename T>
auto DynamicArray<T>::end() const noexcept -> const_iterator {
  return data_.get() + size_;
}

template <typename T>
auto DynamicArray<T>::cbegin() const noexcept -> const_iterator {
  return data_.get();
}

template <typename T>
auto DynamicArray<T>::cend() const noexcept -> const_iterator {
  return data_.get() + size_;
}

template <typename T>
auto DynamicArray<T>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template <typename T>
auto DynamicArray<T>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template <typename T>
auto DynamicArray<T>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template <typename T>
auto DynamicArray<T>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template <typename T>
auto DynamicArray<T>::crbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template <typename T>
auto DynamicArray<T>::crend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T>
auto DynamicArray<T>::ensure_capacity(size_t min_capacity) -> void {
  if (min_capacity <= capacity_) {
    return;
  }

  // Calculate new capacity with growth factor.
  size_t new_capacity = std::max(capacity_, kMinCapacity);
  while (new_capacity < min_capacity) {
    if (new_capacity > std::numeric_limits<size_t>::max() / kGrowthFactor) {
      throw ArrayOverflowException("DynamicArray capacity overflow");
    }
    new_capacity = std::max(new_capacity * kGrowthFactor, kMinCapacity);
  }

  // Reallocate to new capacity.
  reallocate(new_capacity);
}

template <typename T>
auto DynamicArray<T>::grow() -> void {
  if (capacity_ > std::numeric_limits<size_t>::max() / kGrowthFactor) {
    throw ArrayOverflowException("DynamicArray capacity overflow");
  }

  // Calculate new capacity and reallocate.
  const size_t new_capacity = std::max(capacity_ * kGrowthFactor, kMinCapacity);
  reallocate(new_capacity);
}

template <typename T>
auto DynamicArray<T>::reallocate(size_t new_capacity) -> void {
  if (new_capacity < size_) {
    new_capacity = size_;
  }

  // Check for overflow BEFORE multiplication.
  if (new_capacity > std::numeric_limits<size_t>::max() / sizeof(T)) {
    throw ArrayOverflowException("DynamicArray capacity overflow");
  }

  // Allocate new memory with custom deleter.
  std::unique_ptr<T[], void (*)(T*)> new_data(
      static_cast<T*>(::operator new[](new_capacity * sizeof(T))), [](T* ptr) { ::operator delete[](ptr); });

  // Move or copy existing elements to new storage.
  // Use uninitialized algorithms to enable bulk optimizations when possible.
  if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
    std::uninitialized_move_n(data_.get(), size_, new_data.get());
  } else {
    std::uninitialized_copy_n(data_.get(), size_, new_data.get());
  }

  // Destroy old elements only after all new elements are constructed.
  for (size_t i = 0; i < size_; ++i) {
    std::destroy_at(data_.get() + i);
  }

  // Update internal state.
  data_     = std::move(new_data);
  capacity_ = new_capacity;
}

} // namespace ads::arrays

//===---------------------------------------------------------------------------===//
