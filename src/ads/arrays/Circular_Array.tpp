//===---------------------------------------------------------------------------===//
/**
 * @file Circular_Array.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for CircularArray.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/arrays/Circular_Array.hpp"

namespace ads::arrays {

// The iterator and const_iterator types are aliases of IndexedIterator
// (see Indexed_Iterator.hpp); their operations need no out-of-line definitions.

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <ArrayElement T>
CircularArray<T>::CircularArray(size_t initial_capacity) :
    data_(nullptr, &deallocate),
    head_(0),
    size_(0),
    capacity_(std::max(initial_capacity, kMinCapacity)) {
  // allocate() validates against capacity overflow before reserving storage.
  data_ = allocate(capacity_);
}

template <ArrayElement T>
CircularArray<T>::CircularArray(std::initializer_list<T> values) :
    data_(nullptr, &deallocate),
    head_(0),
    size_(0),
    capacity_(std::max(values.size(), kMinCapacity)) {
  // Allocate raw memory (allocate() validates against capacity overflow).
  data_ = allocate(capacity_);

  size_t constructed = 0;
  try {
    for (const auto& value : values) {
      std::construct_at(data_.get() + constructed, value);
      ++constructed;
    }
  } catch (...) {
    for (size_t i = 0; i < constructed; ++i) {
      std::destroy_at(data_.get() + i);
    }
    throw;
  }
  size_ = values.size();
}

template <ArrayElement T>
CircularArray<T>::CircularArray(CircularArray&& other) noexcept :
    data_(std::move(other.data_)),
    head_(other.head_),
    size_(other.size_),
    capacity_(other.capacity_) {
  other.head_     = 0;
  other.size_     = 0;
  other.capacity_ = 0;
}

template <ArrayElement T>
CircularArray<T>::~CircularArray() {
  clear();
}

template <ArrayElement T>
auto CircularArray<T>::operator=(CircularArray&& other) noexcept -> CircularArray& {
  if (this != &other) {
    clear();
    data_           = std::move(other.data_);
    head_           = other.head_;
    size_           = other.size_;
    capacity_       = other.capacity_;
    other.head_     = 0;
    other.size_     = 0;
    other.capacity_ = 0;
  }
  return *this;
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <ArrayElement T>
template <typename... Args>
auto CircularArray<T>::emplace_front(Args&&... args) -> T& requires AppendArrayElement<T, Args...>
{
  if (size_ == capacity_) {
    // Growing reallocates and would invalidate arguments that alias an element
    // of this array (e.g. push_front(arr[0])): materialize the value first.
    T value(std::forward<Args>(args)...);
    ensure_capacity(size_ + 1);
    return emplace_front(std::move(value));
  }

  const size_t new_head = (head_ + capacity_ - 1) % capacity_;
  T*           ptr      = data_.get() + new_head;
  std::construct_at(ptr, std::forward<Args>(args)...);
  // Publish the new head only after construction so a throwing T leaves the array unchanged.
  head_ = new_head;
  ++size_;
  return *ptr;
}

template <ArrayElement T>
auto CircularArray<T>::push_front(const T& value) -> void requires CopyArrayElement<T>
{
  emplace_front(value);
}

template <ArrayElement T>
auto CircularArray<T>::push_front(T&& value) -> void requires MoveArrayElement<T>
{
  emplace_front(std::move(value));
}

template <ArrayElement T>
template <typename... Args>
auto CircularArray<T>::emplace_back(Args&&... args) -> T& requires AppendArrayElement<T, Args...>
{
  if (size_ == capacity_) {
    // Growing reallocates and would invalidate arguments that alias an element
    // of this array (e.g. push_back(arr[0])): materialize the value first.
    T value(std::forward<Args>(args)...);
    ensure_capacity(size_ + 1);
    return emplace_back(std::move(value));
  }

  size_t physical_index = to_physical_index(size_);
  T*     ptr            = data_.get() + physical_index;
  std::construct_at(ptr, std::forward<Args>(args)...);
  ++size_;
  return *ptr;
}

template <ArrayElement T>
auto CircularArray<T>::push_back(const T& value) -> void requires CopyArrayElement<T>
{
  emplace_back(value);
}

template <ArrayElement T>
auto CircularArray<T>::push_back(T&& value) -> void requires MoveArrayElement<T>
{
  emplace_back(std::move(value));
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <ArrayElement T>
auto CircularArray<T>::pop_front() -> void {
  if (is_empty()) {
    throw ArrayUnderflowException("pop_front on empty circular array");
  }

  std::destroy_at(data_.get() + head_);
  head_ = (head_ + 1) % capacity_;
  --size_;
}

template <ArrayElement T>
auto CircularArray<T>::pop_back() -> void {
  if (is_empty()) {
    throw ArrayUnderflowException("pop_back on empty circular array");
  }

  size_t physical_index = to_physical_index(size_ - 1);
  std::destroy_at(data_.get() + physical_index);
  --size_;
}

template <ArrayElement T>
auto CircularArray<T>::clear() noexcept -> void {
  for (size_t i = 0; i < size_; ++i) {
    std::destroy_at(data_.get() + to_physical_index(i));
  }
  size_ = 0;
  head_ = 0;
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <ArrayElement T>
auto CircularArray<T>::operator[](size_t index) -> T& {
  return data_.get()[to_physical_index(index)];
}

template <ArrayElement T>
auto CircularArray<T>::operator[](size_t index) const -> const T& {
  return data_.get()[to_physical_index(index)];
}

template <ArrayElement T>
auto CircularArray<T>::at(size_t index) -> T& {
  if (index >= size_) {
    throw ArrayOutOfRangeException("CircularArray index out of range");
  }
  return (*this)[index];
}

template <ArrayElement T>
auto CircularArray<T>::at(size_t index) const -> const T& {
  if (index >= size_) {
    throw ArrayOutOfRangeException("CircularArray index out of range");
  }
  return (*this)[index];
}

template <ArrayElement T>
auto CircularArray<T>::front() -> T& {
  if (is_empty()) {
    throw ArrayUnderflowException("front on empty circular array");
  }
  return data_.get()[head_];
}

template <ArrayElement T>
auto CircularArray<T>::front() const -> const T& {
  if (is_empty()) {
    throw ArrayUnderflowException("front on empty circular array");
  }
  return data_.get()[head_];
}

template <ArrayElement T>
auto CircularArray<T>::back() -> T& {
  if (is_empty()) {
    throw ArrayUnderflowException("back on empty circular array");
  }
  return (*this)[size_ - 1];
}

template <ArrayElement T>
auto CircularArray<T>::back() const -> const T& {
  if (is_empty()) {
    throw ArrayUnderflowException("back on empty circular array");
  }
  return (*this)[size_ - 1];
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <ArrayElement T>
auto CircularArray<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <ArrayElement T>
auto CircularArray<T>::size() const noexcept -> size_t {
  return size_;
}

template <ArrayElement T>
auto CircularArray<T>::capacity() const noexcept -> size_t {
  return capacity_;
}

//===----- CAPACITY OPERATIONS -------------------------------------------------===//

template <ArrayElement T>
auto CircularArray<T>::reserve(size_t new_capacity) -> void requires RelocatableArrayElement<T>
{
  if (new_capacity > capacity_) {
    reallocate(new_capacity);
  }
}

template <ArrayElement T>
auto CircularArray<T>::shrink_to_fit() -> void requires RelocatableArrayElement<T>
{
  if (size_ < capacity_) {
    const size_t new_capacity = std::max(size_, kMinCapacity);
    if (new_capacity != capacity_) {
      reallocate(new_capacity);
    }
  }
}

//===----- ITERATOR OPERATIONS -------------------------------------------------===//

template <ArrayElement T>
auto CircularArray<T>::begin() noexcept -> iterator {
  return iterator(0, this);
}

template <ArrayElement T>
auto CircularArray<T>::end() noexcept -> iterator {
  return iterator(static_cast<iterator::difference_type>(size_), this);
}

template <ArrayElement T>
auto CircularArray<T>::begin() const noexcept -> const_iterator {
  return const_iterator(0, this);
}

template <ArrayElement T>
auto CircularArray<T>::end() const noexcept -> const_iterator {
  return const_iterator(static_cast<const_iterator::difference_type>(size_), this);
}

// cbegin/cend, the reverse-iterator accessors, and the relational operators are
// provided by ContainerFacade<CircularArray<T>>; no out-of-line definitions needed.

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <ArrayElement T>
auto CircularArray<T>::allocate(size_t capacity) -> storage_ptr {
  if (capacity > max_elements()) {
    throw ArrayOverflowException("CircularArray capacity overflow");
  }
  if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
    // Over-aligned element types need the aligned operator new[] overload.
    return storage_ptr(static_cast<T*>(::operator new[](capacity * sizeof(T), std::align_val_t{alignof(T)})), &deallocate);
  }
  return storage_ptr(static_cast<T*>(::operator new[](capacity * sizeof(T))), &deallocate);
}

template <ArrayElement T>
auto CircularArray<T>::to_physical_index(size_t logical_index) const noexcept -> size_t {
  return (head_ + logical_index) % capacity_;
}

template <ArrayElement T>
auto CircularArray<T>::ensure_capacity(size_t min_capacity) -> void {
  if (min_capacity <= capacity_) {
    return;
  }

  size_t new_capacity = std::max(capacity_, kMinCapacity);
  while (new_capacity < min_capacity) {
    if (new_capacity > std::numeric_limits<size_t>::max() / kGrowthFactor) {
      throw ArrayOverflowException("CircularArray capacity overflow");
    }
    new_capacity = std::max(new_capacity * kGrowthFactor, kMinCapacity);
  }

  reallocate(new_capacity);
}

template <ArrayElement T>
auto CircularArray<T>::reallocate(size_t new_capacity) -> void {
  if (new_capacity < size_) {
    new_capacity = size_;
  }

  // Allocate new storage (allocate() validates against capacity overflow).
  storage_ptr new_data = allocate(new_capacity);

  // Move elements to new buffer in linear order.
  size_t constructed = 0;
  try {
    for (size_t i = 0; i < size_; ++i) {
      size_t old_physical = to_physical_index(i);
      if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
        std::construct_at(new_data.get() + constructed, std::move(data_[old_physical]));
      } else {
        std::construct_at(new_data.get() + constructed, data_[old_physical]);
      }
      ++constructed;
    }
  } catch (...) {
    for (size_t i = 0; i < constructed; ++i) {
      std::destroy_at(new_data.get() + i);
    }
    throw;
  }

  // Destroy old elements.
  for (size_t i = 0; i < size_; ++i) {
    std::destroy_at(data_.get() + to_physical_index(i));
  }

  data_     = std::move(new_data);
  head_     = 0;
  capacity_ = new_capacity;
}

} // namespace ads::arrays

//===---------------------------------------------------------------------------===//
