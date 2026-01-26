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

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
CircularArray<T>::CircularArray(size_t initial_capacity) :
    data_(nullptr, [](T* ptr) { ::operator delete[](ptr); }), head_(0), size_(0), capacity_(std::max(initial_capacity, kMinCapacity)) {
  if (capacity_ > std::numeric_limits<size_t>::max() / sizeof(T)) {
    throw ArrayOverflowException("CircularArray capacity overflow");
  }
  data_.reset(static_cast<T*>(::operator new[](capacity_ * sizeof(T))));
}

template <typename T>
CircularArray<T>::CircularArray(std::initializer_list<T> values) :
    data_(nullptr, [](T* ptr) { ::operator delete[](ptr); }), head_(0), size_(0), capacity_(std::max(values.size(), kMinCapacity)) {
  if (capacity_ > std::numeric_limits<size_t>::max() / sizeof(T)) {
    throw ArrayOverflowException("CircularArray capacity overflow");
  }

  data_.reset(static_cast<T*>(::operator new[](capacity_ * sizeof(T))));

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

template <typename T>
CircularArray<T>::CircularArray(CircularArray&& other) noexcept :
    data_(std::move(other.data_)), head_(other.head_), size_(other.size_), capacity_(other.capacity_) {
  other.head_     = 0;
  other.size_     = 0;
  other.capacity_ = 0;
}

template <typename T>
CircularArray<T>::~CircularArray() {
  clear();
}

template <typename T>
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

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename T>
template <typename... Args>
auto CircularArray<T>::emplace_front(Args&&... args) -> T& {
  ensure_capacity(size_ + 1);

  head_  = (head_ + capacity_ - 1) % capacity_;
  T* ptr = data_.get() + head_;
  std::construct_at(ptr, std::forward<Args>(args)...);
  ++size_;
  return *ptr;
}

template <typename T>
auto CircularArray<T>::push_front(const T& value) -> void {
  emplace_front(value);
}

template <typename T>
auto CircularArray<T>::push_front(T&& value) -> void {
  emplace_front(std::move(value));
}

template <typename T>
template <typename... Args>
auto CircularArray<T>::emplace_back(Args&&... args) -> T& {
  ensure_capacity(size_ + 1);

  size_t physical_index = to_physical_index(size_);
  T*     ptr            = data_.get() + physical_index;
  std::construct_at(ptr, std::forward<Args>(args)...);
  ++size_;
  return *ptr;
}

template <typename T>
auto CircularArray<T>::push_back(const T& value) -> void {
  emplace_back(value);
}

template <typename T>
auto CircularArray<T>::push_back(T&& value) -> void {
  emplace_back(std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
auto CircularArray<T>::pop_front() -> void {
  if (is_empty()) {
    throw ArrayUnderflowException("pop_front on empty circular array");
  }

  std::destroy_at(data_.get() + head_);
  head_ = (head_ + 1) % capacity_;
  --size_;
}

template <typename T>
auto CircularArray<T>::pop_back() -> void {
  if (is_empty()) {
    throw ArrayUnderflowException("pop_back on empty circular array");
  }

  size_t physical_index = to_physical_index(size_ - 1);
  std::destroy_at(data_.get() + physical_index);
  --size_;
}

template <typename T>
auto CircularArray<T>::clear() noexcept -> void {
  for (size_t i = 0; i < size_; ++i) {
    std::destroy_at(data_.get() + to_physical_index(i));
  }
  size_ = 0;
  head_ = 0;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto CircularArray<T>::operator[](size_t index) -> T& {
  return data_.get()[to_physical_index(index)];
}

template <typename T>
auto CircularArray<T>::operator[](size_t index) const -> const T& {
  return data_.get()[to_physical_index(index)];
}

template <typename T>
auto CircularArray<T>::at(size_t index) -> T& {
  if (index >= size_) {
    throw ArrayOutOfRangeException("CircularArray index out of range");
  }
  return (*this)[index];
}

template <typename T>
auto CircularArray<T>::at(size_t index) const -> const T& {
  if (index >= size_) {
    throw ArrayOutOfRangeException("CircularArray index out of range");
  }
  return (*this)[index];
}

template <typename T>
auto CircularArray<T>::front() -> T& {
  if (is_empty()) {
    throw ArrayUnderflowException("front on empty circular array");
  }
  return data_.get()[head_];
}

template <typename T>
auto CircularArray<T>::front() const -> const T& {
  if (is_empty()) {
    throw ArrayUnderflowException("front on empty circular array");
  }
  return data_.get()[head_];
}

template <typename T>
auto CircularArray<T>::back() -> T& {
  if (is_empty()) {
    throw ArrayUnderflowException("back on empty circular array");
  }
  return (*this)[size_ - 1];
}

template <typename T>
auto CircularArray<T>::back() const -> const T& {
  if (is_empty()) {
    throw ArrayUnderflowException("back on empty circular array");
  }
  return (*this)[size_ - 1];
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto CircularArray<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T>
auto CircularArray<T>::size() const noexcept -> size_t {
  return size_;
}

template <typename T>
auto CircularArray<T>::capacity() const noexcept -> size_t {
  return capacity_;
}

//===--------------------------- CAPACITY OPERATIONS ---------------------------===//

template <typename T>
auto CircularArray<T>::reserve(size_t new_capacity) -> void {
  if (new_capacity > capacity_) {
    reallocate(new_capacity);
  }
}

template <typename T>
auto CircularArray<T>::shrink_to_fit() -> void {
  if (size_ < capacity_) {
    const size_t new_capacity = std::max(size_, kMinCapacity);
    if (new_capacity != capacity_) {
      reallocate(new_capacity);
    }
  }
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <typename T>
auto CircularArray<T>::begin() noexcept -> iterator {
  return iterator(0, this);
}

template <typename T>
auto CircularArray<T>::end() noexcept -> iterator {
  return iterator(size_, this);
}

template <typename T>
auto CircularArray<T>::begin() const noexcept -> const_iterator {
  return const_iterator(0, this);
}

template <typename T>
auto CircularArray<T>::end() const noexcept -> const_iterator {
  return const_iterator(size_, this);
}

template <typename T>
auto CircularArray<T>::cbegin() const noexcept -> const_iterator {
  return const_iterator(0, this);
}

template <typename T>
auto CircularArray<T>::cend() const noexcept -> const_iterator {
  return const_iterator(size_, this);
}

template <typename T>
auto CircularArray<T>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template <typename T>
auto CircularArray<T>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template <typename T>
auto CircularArray<T>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template <typename T>
auto CircularArray<T>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template <typename T>
auto CircularArray<T>::crbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template <typename T>
auto CircularArray<T>::crend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T>
auto CircularArray<T>::to_physical_index(size_t logical_index) const noexcept -> size_t {
  return (head_ + logical_index) % capacity_;
}

template <typename T>
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

template <typename T>
auto CircularArray<T>::reallocate(size_t new_capacity) -> void {
  if (new_capacity < size_) {
    new_capacity = size_;
  }

  if (new_capacity > std::numeric_limits<size_t>::max() / sizeof(T)) {
    throw ArrayOverflowException("CircularArray capacity overflow");
  }

  std::unique_ptr<T[], void (*)(T*)> new_data(
      static_cast<T*>(::operator new[](new_capacity * sizeof(T))), [](T* ptr) { ::operator delete[](ptr); });

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

//===---------------------------- ITERATOR METHODS -----------------------------===//

// iterator methods
template <typename T>
auto CircularArray<T>::iterator::operator*() const -> reference {
  return (*array_)[logical_index_];
}

template <typename T>
auto CircularArray<T>::iterator::operator->() const -> pointer {
  return &(*array_)[logical_index_];
}

template <typename T>
auto CircularArray<T>::iterator::operator[](difference_type n) const -> reference {
  return (*array_)[logical_index_ + n];
}

template <typename T>
auto CircularArray<T>::iterator::operator++() -> iterator& {
  ++logical_index_;
  return *this;
}

template <typename T>
auto CircularArray<T>::iterator::operator++(int) -> iterator {
  iterator tmp = *this;
  ++(*this);
  return tmp;
}

template <typename T>
auto CircularArray<T>::iterator::operator--() -> iterator& {
  --logical_index_;
  return *this;
}

template <typename T>
auto CircularArray<T>::iterator::operator--(int) -> iterator {
  iterator tmp = *this;
  --(*this);
  return tmp;
}

template <typename T>
auto CircularArray<T>::iterator::operator+=(difference_type n) -> iterator& {
  logical_index_ += n;
  return *this;
}

template <typename T>
auto CircularArray<T>::iterator::operator-=(difference_type n) -> iterator& {
  logical_index_ -= n;
  return *this;
}

template <typename T>
auto CircularArray<T>::iterator::operator+(difference_type n) const -> iterator {
  return iterator(logical_index_ + n, array_);
}

template <typename T>
auto CircularArray<T>::iterator::operator-(difference_type n) const -> iterator {
  return iterator(logical_index_ - n, array_);
}

template <typename T>
auto CircularArray<T>::iterator::operator-(const iterator& other) const -> difference_type {
  return static_cast<difference_type>(logical_index_) - static_cast<difference_type>(other.logical_index_);
}

//===------------------------- CONST_ITERATOR METHODS --------------------------===//

template <typename T>
auto CircularArray<T>::const_iterator::operator*() const -> reference {
  return (*array_)[logical_index_];
}

template <typename T>
auto CircularArray<T>::const_iterator::operator->() const -> pointer {
  return &(*array_)[logical_index_];
}

template <typename T>
auto CircularArray<T>::const_iterator::operator[](difference_type n) const -> reference {
  return (*array_)[logical_index_ + n];
}

template <typename T>
auto CircularArray<T>::const_iterator::operator++() -> const_iterator& {
  ++logical_index_;
  return *this;
}

template <typename T>
auto CircularArray<T>::const_iterator::operator++(int) -> const_iterator {
  const_iterator tmp = *this;
  ++(*this);
  return tmp;
}

template <typename T>
auto CircularArray<T>::const_iterator::operator--() -> const_iterator& {
  --logical_index_;
  return *this;
}

template <typename T>
auto CircularArray<T>::const_iterator::operator--(int) -> const_iterator {
  const_iterator tmp = *this;
  --(*this);
  return tmp;
}

template <typename T>
auto CircularArray<T>::const_iterator::operator+=(difference_type n) -> const_iterator& {
  logical_index_ += n;
  return *this;
}

template <typename T>
auto CircularArray<T>::const_iterator::operator-=(difference_type n) -> const_iterator& {
  logical_index_ -= n;
  return *this;
}

template <typename T>
auto CircularArray<T>::const_iterator::operator+(difference_type n) const -> const_iterator {
  return const_iterator(logical_index_ + n, array_);
}

template <typename T>
auto CircularArray<T>::const_iterator::operator-(difference_type n) const -> const_iterator {
  return const_iterator(logical_index_ - n, array_);
}

template <typename T>
auto CircularArray<T>::const_iterator::operator-(const const_iterator& other) const -> difference_type {
  return static_cast<difference_type>(logical_index_) - static_cast<difference_type>(other.logical_index_);
}

} // namespace ads::arrays

//===---------------------------------------------------------------------------===//
