//===---------------------------------------------------------------------------===//
/**
 * @file Gap_Buffer.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for GapBuffer.
 * @version 0.1
 * @date 2026-06-08
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/arrays/Gap_Buffer.hpp"

#include <algorithm>

namespace ads::arrays {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <ArrayElement T>
GapBuffer<T>::GapBuffer(size_t initial_capacity) :
    data_(nullptr, &deallocate),
    capacity_(std::max(initial_capacity, kMinCapacity)),
    gap_begin_(0),
    gap_end_(0) {
  // allocate() validates against capacity overflow before reserving storage.
  data_    = allocate(capacity_);
  gap_end_ = capacity_; // The whole buffer is initially one big gap.
}

template <ArrayElement T>
GapBuffer<T>::GapBuffer(std::initializer_list<T> values) requires CopyArrayElement<T>
    : data_(nullptr, &deallocate), capacity_(std::max(values.size(), kMinCapacity)), gap_begin_(0), gap_end_(0) {
  data_    = allocate(capacity_);
  gap_end_ = capacity_;

  // Copy elements into the front segment; the gap follows them (cursor at end).
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
  gap_begin_ = values.size();
}

template <ArrayElement T>
GapBuffer<T>::GapBuffer(GapBuffer&& other) noexcept :
    data_(std::move(other.data_)),
    capacity_(other.capacity_),
    gap_begin_(other.gap_begin_),
    gap_end_(other.gap_end_) {
  other.capacity_  = 0;
  other.gap_begin_ = 0;
  other.gap_end_   = 0;
}

template <ArrayElement T>
GapBuffer<T>::~GapBuffer() {
  clear();
}

template <ArrayElement T>
auto GapBuffer<T>::operator=(GapBuffer&& other) noexcept -> GapBuffer& {
  if (this != &other) {
    clear();
    data_            = std::move(other.data_);
    capacity_        = other.capacity_;
    gap_begin_       = other.gap_begin_;
    gap_end_         = other.gap_end_;
    other.capacity_  = 0;
    other.gap_begin_ = 0;
    other.gap_end_   = 0;
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <ArrayElement T>
template <typename... Args>
auto GapBuffer<T>::emplace(Args&&... args) -> T& requires AppendArrayElement<T, Args...>
{
  ensure_gap();
  T* ptr = data_.get() + gap_begin_;
  std::construct_at(ptr, std::forward<Args>(args)...);
  // Publish the new element only after construction so a throwing T is harmless.
  ++gap_begin_;
  return *ptr;
}

template <ArrayElement T>
auto GapBuffer<T>::insert(const T& value) -> void requires CopyArrayElement<T>
{
  emplace(value);
}

template <ArrayElement T>
auto GapBuffer<T>::insert(T&& value) -> void requires MoveArrayElement<T>
{
  emplace(std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <ArrayElement T>
auto GapBuffer<T>::erase() -> void {
  if (gap_begin_ == 0) {
    throw ArrayUnderflowException("erase at beginning of gap buffer");
  }
  --gap_begin_;
  std::destroy_at(data_.get() + gap_begin_);
}

template <ArrayElement T>
auto GapBuffer<T>::erase_forward() -> void {
  if (gap_end_ == capacity_) {
    throw ArrayUnderflowException("erase at end of gap buffer");
  }
  std::destroy_at(data_.get() + gap_end_);
  ++gap_end_;
}

template <ArrayElement T>
auto GapBuffer<T>::clear() noexcept -> void {
  if constexpr (!std::is_trivially_destructible_v<T>) {
    for (size_t i = 0; i < gap_begin_; ++i) {
      std::destroy_at(data_.get() + i);
    }
    for (size_t i = gap_end_; i < capacity_; ++i) {
      std::destroy_at(data_.get() + i);
    }
  }
  gap_begin_ = 0;
  gap_end_   = capacity_;
}

//===---------------------------- CURSOR OPERATIONS ----------------------------===//

template <ArrayElement T>
auto GapBuffer<T>::cursor() const noexcept -> size_t {
  return gap_begin_;
}

template <ArrayElement T>
auto GapBuffer<T>::move_cursor_to(size_t position) -> void {
  if (position > size()) {
    throw ArrayOutOfRangeException("cursor position out of range");
  }
  while (gap_begin_ < position) {
    advance_cursor();
  }
  while (gap_begin_ > position) {
    retreat_cursor();
  }
}

template <ArrayElement T>
auto GapBuffer<T>::advance_cursor() -> void {
  if (gap_end_ == capacity_) {
    throw ArrayUnderflowException("cursor already at end");
  }
  // Shift the first element after the gap to the front of the gap.
  std::construct_at(data_.get() + gap_begin_, std::move(data_.get()[gap_end_]));
  std::destroy_at(data_.get() + gap_end_);
  ++gap_begin_;
  ++gap_end_;
}

template <ArrayElement T>
auto GapBuffer<T>::retreat_cursor() -> void {
  if (gap_begin_ == 0) {
    throw ArrayUnderflowException("cursor already at beginning");
  }
  // Shift the last element before the gap to the back of the gap.
  std::construct_at(data_.get() + gap_end_ - 1, std::move(data_.get()[gap_begin_ - 1]));
  std::destroy_at(data_.get() + gap_begin_ - 1);
  --gap_begin_;
  --gap_end_;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <ArrayElement T>
auto GapBuffer<T>::operator[](size_t index) -> T& {
  return data_.get()[to_physical_index(index)];
}

template <ArrayElement T>
auto GapBuffer<T>::operator[](size_t index) const -> const T& {
  return data_.get()[to_physical_index(index)];
}

template <ArrayElement T>
auto GapBuffer<T>::at(size_t index) -> T& {
  if (index >= size()) {
    throw ArrayOutOfRangeException("GapBuffer index out of range");
  }
  return (*this)[index];
}

template <ArrayElement T>
auto GapBuffer<T>::at(size_t index) const -> const T& {
  if (index >= size()) {
    throw ArrayOutOfRangeException("GapBuffer index out of range");
  }
  return (*this)[index];
}

template <ArrayElement T>
auto GapBuffer<T>::front() -> T& {
  if (size() == 0) {
    throw ArrayUnderflowException("front on empty gap buffer");
  }
  return (*this)[0];
}

template <ArrayElement T>
auto GapBuffer<T>::front() const -> const T& {
  if (size() == 0) {
    throw ArrayUnderflowException("front on empty gap buffer");
  }
  return (*this)[0];
}

template <ArrayElement T>
auto GapBuffer<T>::back() -> T& {
  if (size() == 0) {
    throw ArrayUnderflowException("back on empty gap buffer");
  }
  return (*this)[size() - 1];
}

template <ArrayElement T>
auto GapBuffer<T>::back() const -> const T& {
  if (size() == 0) {
    throw ArrayUnderflowException("back on empty gap buffer");
  }
  return (*this)[size() - 1];
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <ArrayElement T>
auto GapBuffer<T>::size() const noexcept -> size_t {
  return gap_begin_ + (capacity_ - gap_end_);
}

template <ArrayElement T>
auto GapBuffer<T>::capacity() const noexcept -> size_t {
  return capacity_;
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <ArrayElement T>
auto GapBuffer<T>::begin() noexcept -> iterator {
  return iterator(0, this);
}

template <ArrayElement T>
auto GapBuffer<T>::begin() const noexcept -> const_iterator {
  return const_iterator(0, this);
}

template <ArrayElement T>
auto GapBuffer<T>::end() noexcept -> iterator {
  return iterator(static_cast<difference_type>(size()), this);
}

template <ArrayElement T>
auto GapBuffer<T>::end() const noexcept -> const_iterator {
  return const_iterator(static_cast<difference_type>(size()), this);
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <ArrayElement T>
auto GapBuffer<T>::allocate(size_t capacity) -> storage_ptr {
  if (capacity > max_elements()) {
    throw ArrayOverflowException("GapBuffer capacity overflow");
  }
  return storage_ptr(static_cast<T*>(::operator new[](capacity * sizeof(T))), &deallocate);
}

template <ArrayElement T>
auto GapBuffer<T>::to_physical_index(size_t logical_index) const noexcept -> size_t {
  return logical_index < gap_begin_ ? logical_index : logical_index + gap_size();
}

template <ArrayElement T>
auto GapBuffer<T>::gap_size() const noexcept -> size_t {
  return gap_end_ - gap_begin_;
}

template <ArrayElement T>
auto GapBuffer<T>::ensure_gap() -> void {
  if (gap_begin_ != gap_end_) {
    return;
  }
  // The gap is empty: grow the buffer (this repositions the gap at the cursor).
  if (capacity_ > std::numeric_limits<size_t>::max() / kGrowthFactor) {
    throw ArrayOverflowException("GapBuffer capacity overflow");
  }
  reallocate(std::max(capacity_ * kGrowthFactor, kMinCapacity));
}

template <ArrayElement T>
auto GapBuffer<T>::reallocate(size_t new_capacity) -> void {
  const size_t before = gap_begin_;           // elements before the gap
  const size_t after  = capacity_ - gap_end_; // elements after the gap

  if (new_capacity < before + after) {
    new_capacity = before + after;
  }

  // allocate() validates against capacity overflow.
  storage_ptr new_data = allocate(new_capacity);
  T*          src      = data_.get();
  T*          dst      = new_data.get();

  size_t built_before = 0;
  size_t built_after  = 0;
  try {
    for (; built_before < before; ++built_before) {
      if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
        std::construct_at(dst + built_before, std::move(src[built_before]));
      } else {
        std::construct_at(dst + built_before, src[built_before]);
      }
    }
    for (; built_after < after; ++built_after) {
      T* from = src + gap_end_ + built_after;
      T* to   = dst + (new_capacity - after) + built_after;
      if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
        std::construct_at(to, std::move(*from));
      } else {
        std::construct_at(to, *from);
      }
    }
  } catch (...) {
    // The old buffer remains authoritative; roll back the partial new copy.
    for (size_t i = 0; i < built_before; ++i) {
      std::destroy_at(dst + i);
    }
    for (size_t i = 0; i < built_after; ++i) {
      std::destroy_at(dst + (new_capacity - after) + i);
    }
    throw;
  }

  // Destroy the old elements only after all new elements are constructed.
  for (size_t i = 0; i < before; ++i) {
    std::destroy_at(src + i);
  }
  for (size_t i = 0; i < after; ++i) {
    std::destroy_at(src + gap_end_ + i);
  }

  data_     = std::move(new_data);
  capacity_ = new_capacity;
  gap_end_  = new_capacity - after; // gap_begin_ (the cursor) is unchanged.
}

} // namespace ads::arrays

//===---------------------------------------------------------------------------===//
