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

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <ArrayElement T>
DynamicArray<T>::DynamicArray(size_t initial_capacity) :
    data_(nullptr, &deallocate),
    size_(0),
    capacity_(std::max(initial_capacity, kMinCapacity)) {
  // allocate() validates against capacity overflow before reserving storage.
  data_ = allocate(capacity_);
}

template <ArrayElement T>
DynamicArray<T>::DynamicArray(std::initializer_list<T> values) :
    data_(nullptr, &deallocate),
    size_(0),
    capacity_(std::max(values.size(), kMinCapacity)) {
  // Allocate raw memory (allocate() validates against capacity overflow).
  data_ = allocate(capacity_);

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

template <ArrayElement T>
DynamicArray<T>::DynamicArray(size_t count, const T& value) :
    data_(nullptr, &deallocate),
    size_(0),
    capacity_(std::max(count, kMinCapacity)) {
  // Allocate raw memory (allocate() validates against capacity overflow).
  data_ = allocate(capacity_);

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

template <ArrayElement T>
template <std::input_iterator InputIt>
DynamicArray<T>::DynamicArray(InputIt first, InputIt last) requires RangeArrayElement<InputIt, T>
    : data_(nullptr, &deallocate), size_(0), capacity_(kMinCapacity) {
  data_ = allocate(capacity_);

  if constexpr (std::forward_iterator<InputIt>) {
    const auto count = std::distance(first, last);
    if (count > 0) {
      reserve(static_cast<size_t>(count));
    }
  }

  try {
    for (; first != last; ++first) {
      emplace_back(*first);
    }
  } catch (...) {
    // Constructors do not call ~DynamicArray on failure, so clean live elements here.
    clear();
    throw;
  }
}

template <ArrayElement T>
DynamicArray<T>::DynamicArray(DynamicArray&& other) noexcept :
    data_(std::move(other.data_)),
    size_(other.size_),
    capacity_(other.capacity_) {
  other.size_     = 0;
  other.capacity_ = 0;
}

template <ArrayElement T>
DynamicArray<T>::~DynamicArray() {
  clear();
}

template <ArrayElement T>
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

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <ArrayElement T>
template <typename... Args>
auto DynamicArray<T>::emplace_back(Args&&... args) -> T& requires AppendArrayElement<T, Args...>
{
  ensure_capacity(size_ + 1);

  T* element_ptr = data_.get() + size_;
  std::construct_at(element_ptr, std::forward<Args>(args)...);
  ++size_;
  return *element_ptr;
}

template <ArrayElement T>
auto DynamicArray<T>::push_back(const T& value) -> void requires CopyArrayElement<T>
{
  emplace_back(value);
}

template <ArrayElement T>
auto DynamicArray<T>::push_back(T&& value) -> void requires MoveArrayElement<T>
{
  emplace_back(std::move(value));
}

template <ArrayElement T>
template <typename... Args>
auto DynamicArray<T>::emplace(size_t index, Args&&... args) -> T& requires InsertArrayElement<T, Args...>
{
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

  // Construct value first so no mutation happens if construction throws.
  T value(std::forward<Args>(args)...);

  // Create one uninitialized slot at the end by move/copy-constructing the last element.
  std::construct_at(data_ptr + size_, std::move_if_noexcept(data_ptr[size_ - 1]));

  // Shift the remaining elements one position to the right.
  // std::move_backward enables optimized bulk moves for trivially movable types.
  try {
    std::move_backward(data_ptr + index, data_ptr + size_ - 1, data_ptr + size_);
    data_ptr[index] = std::move(value);
  } catch (...) {
    // Keep logical size unchanged and avoid leaking the temporary last slot.
    std::destroy_at(data_ptr + size_);
    throw;
  }

  ++size_;
  return data_ptr[index];
}

template <ArrayElement T>
auto DynamicArray<T>::insert(size_t index, const T& value) -> void requires InsertCopyArrayElement<T>
{
  emplace(index, value);
}

template <ArrayElement T>
auto DynamicArray<T>::insert(size_t index, T&& value) -> void requires InsertMoveArrayElement<T>
{
  emplace(index, std::move(value));
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <ArrayElement T>
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
    } catch (...) {
      // Shrinking is an optimization - keep current storage on failures.
    }
  }
}

template <ArrayElement T>
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
    } catch (...) {
      // Shrinking is an optimization - keep current storage on failures.
    }
  }
}

template <ArrayElement T>
auto DynamicArray<T>::clear() noexcept -> void {
  // Skip destructor loop for trivially destructible types.
  if constexpr (!std::is_trivially_destructible_v<T>) {
    for (size_t i = 0; i < size_; ++i) {
      std::destroy_at(data_.get() + i);
    }
  }
  size_ = 0;
}

template <ArrayElement T>
auto DynamicArray<T>::assign(size_t count, const T& value) -> void requires CopyArrayElement<T>
{
  // Build first so a failed copy leaves the existing array untouched.
  DynamicArray replacement(count, value);
  *this = std::move(replacement);
}

template <ArrayElement T>
auto DynamicArray<T>::assign(std::initializer_list<T> values) -> void requires CopyArrayElement<T>
{
  assign(values.begin(), values.end());
}

template <ArrayElement T>
template <std::input_iterator InputIt>
auto DynamicArray<T>::assign(InputIt first, InputIt last) -> void requires RangeArrayElement<InputIt, T>
{
  // Build first so a failed range copy/move leaves the existing array untouched.
  DynamicArray replacement(first, last);
  *this = std::move(replacement);
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <ArrayElement T>
auto DynamicArray<T>::operator[](size_t index) -> T& {
  return data_.get()[index];
}

template <ArrayElement T>
auto DynamicArray<T>::operator[](size_t index) const -> const T& {
  return data_.get()[index];
}

template <ArrayElement T>
auto DynamicArray<T>::at(size_t index) -> T& {
  if (index >= size_) {
    throw ArrayOutOfRangeException("index out of range");
  }
  return data_.get()[index];
}

template <ArrayElement T>
auto DynamicArray<T>::at(size_t index) const -> const T& {
  if (index >= size_) {
    throw ArrayOutOfRangeException("index out of range");
  }
  return data_.get()[index];
}

template <ArrayElement T>
auto DynamicArray<T>::front() -> T& {
  if (is_empty()) {
    throw ArrayUnderflowException("front on empty array");
  }
  return data_.get()[0];
}

template <ArrayElement T>
auto DynamicArray<T>::front() const -> const T& {
  if (is_empty()) {
    throw ArrayUnderflowException("front on empty array");
  }
  return data_.get()[0];
}

template <ArrayElement T>
auto DynamicArray<T>::back() -> T& {
  if (is_empty()) {
    throw ArrayUnderflowException("back on empty array");
  }
  return data_.get()[size_ - 1];
}

template <ArrayElement T>
auto DynamicArray<T>::back() const -> const T& {
  if (is_empty()) {
    throw ArrayUnderflowException("back on empty array");
  }
  return data_.get()[size_ - 1];
}

template <ArrayElement T>
auto DynamicArray<T>::data() noexcept -> T* {
  return data_.get();
}

template <ArrayElement T>
auto DynamicArray<T>::data() const noexcept -> const T* {
  return data_.get();
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <ArrayElement T>
auto DynamicArray<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <ArrayElement T>
auto DynamicArray<T>::size() const noexcept -> size_t {
  return size_;
}

template <ArrayElement T>
auto DynamicArray<T>::capacity() const noexcept -> size_t {
  return capacity_;
}

//===----- CAPACITY OPERATIONS -------------------------------------------------===//

template <ArrayElement T>
auto DynamicArray<T>::reserve(size_t new_capacity) -> void requires RelocatableArrayElement<T>
{
  if (new_capacity > capacity_) {
    reallocate(new_capacity);
  }
}

template <ArrayElement T>
auto DynamicArray<T>::shrink_to_fit() -> void requires RelocatableArrayElement<T>
{
  if (size_ < capacity_) {
    const size_t new_capacity = std::max(size_, kMinCapacity);
    if (new_capacity != capacity_) {
      reallocate(new_capacity);
    }
  }
}

template <ArrayElement T>
auto DynamicArray<T>::resize(size_t new_size) -> void requires ResizeArrayElement<T>
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
    while (size_ < new_size) {
      std::construct_at(data_.get() + size_);
      ++size_;
    }
  }
}

template <ArrayElement T>
auto DynamicArray<T>::resize(size_t new_size, const T& value) -> void requires CopyArrayElement<T>
{
  if (new_size < size_) {
    for (size_t i = new_size; i < size_; ++i) {
      std::destroy_at(data_.get() + i);
    }
    size_ = new_size;
    return;
  }

  if (new_size > size_) {
    ensure_capacity(new_size);
    while (size_ < new_size) {
      std::construct_at(data_.get() + size_, value);
      ++size_;
    }
  }
}

//===----- ITERATOR OPERATIONS -------------------------------------------------===//

template <ArrayElement T>
auto DynamicArray<T>::begin() noexcept -> iterator {
  return data_.get();
}

template <ArrayElement T>
auto DynamicArray<T>::end() noexcept -> iterator {
  return data_.get() + size_;
}

template <ArrayElement T>
auto DynamicArray<T>::begin() const noexcept -> const_iterator {
  return data_.get();
}

template <ArrayElement T>
auto DynamicArray<T>::end() const noexcept -> const_iterator {
  return data_.get() + size_;
}

// cbegin/cend, the reverse-iterator accessors, and the relational operators are
// provided by ContainerFacade<DynamicArray<T>>; no out-of-line definitions needed.

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <ArrayElement T>
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

template <ArrayElement T>
auto DynamicArray<T>::allocate(size_t capacity) -> storage_ptr {
  if (capacity > max_elements()) {
    throw ArrayOverflowException("DynamicArray capacity overflow");
  }
  return storage_ptr(static_cast<T*>(::operator new[](capacity * sizeof(T))), &deallocate);
}

template <ArrayElement T>
auto DynamicArray<T>::reallocate(size_t new_capacity) -> void {
  if (new_capacity < size_) {
    new_capacity = size_;
  }

  // Allocate new storage (allocate() validates against capacity overflow).
  storage_ptr new_data = allocate(new_capacity);

  size_t constructed = 0;
  try {
    for (; constructed < size_; ++constructed) {
      if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
        std::construct_at(new_data.get() + constructed, std::move(data_.get()[constructed]));
      } else {
        std::construct_at(new_data.get() + constructed, data_.get()[constructed]);
      }
    }
  } catch (...) {
    // The old buffer remains authoritative, so rollback only the new partial copy.
    for (size_t i = 0; i < constructed; ++i) {
      std::destroy_at(new_data.get() + i);
    }
    throw;
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
