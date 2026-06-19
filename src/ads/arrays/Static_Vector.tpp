//===---------------------------------------------------------------------------===//
/**
 * @file Static_Vector.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for StaticVector.
 * @version 0.1
 * @date 2026-06-08
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/arrays/Static_Vector.hpp"

namespace ads::arrays {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
StaticVector<T, N>::StaticVector() noexcept : size_(0) {
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
StaticVector<T, N>::StaticVector(std::initializer_list<T> values) requires CopyArrayElement<T>
    : size_(0) {
  if (values.size() > N) {
    throw ArrayOverflowException("StaticVector capacity exceeded");
  }
  // Construct elements one by one; the destructor cleans up on a throwing copy.
  for (const auto& value : values) {
    std::construct_at(data() + size_, value);
    ++size_;
  }
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
StaticVector<T, N>::StaticVector(size_t count, const T& value) requires CopyArrayElement<T>
    : size_(0) {
  if (count > N) {
    throw ArrayOverflowException("StaticVector capacity exceeded");
  }
  for (; size_ < count;) {
    std::construct_at(data() + size_, value);
    ++size_;
  }
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
StaticVector<T, N>::StaticVector(StaticVector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) requires MoveArrayElement<T>
    : size_(0) {
  // Inline storage cannot be stolen, so each element is moved individually.
  for (size_t i = 0; i < other.size_; ++i) {
    std::construct_at(data() + size_, std::move(other.data()[i]));
    ++size_;
  }
  other.clear();
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
StaticVector<T, N>::~StaticVector() {
  clear();
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::operator=(StaticVector&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
    -> StaticVector& requires MoveArrayElement<T>
{
  if (this != &other) {
    clear();
    for (size_t i = 0; i < other.size_; ++i) {
      std::construct_at(data() + size_, std::move(other.data()[i]));
      ++size_;
    }
    other.clear();
  }
  return *this;
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
template <typename... Args>
auto StaticVector<T, N>::emplace_back(Args&&... args) -> T& requires AppendArrayElement<T, Args...>
{
  if (size_ == N) {
    throw ArrayOverflowException("StaticVector capacity exceeded");
  }
  T* element_ptr = data() + size_;
  std::construct_at(element_ptr, std::forward<Args>(args)...);
  ++size_;
  return *element_ptr;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::push_back(const T& value) -> void requires CopyArrayElement<T>
{
  emplace_back(value);
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::push_back(T&& value) -> void requires MoveArrayElement<T>
{
  emplace_back(std::move(value));
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
template <typename... Args>
auto StaticVector<T, N>::emplace(size_t index, Args&&... args) -> T& requires InsertArrayElement<T, Args...>
{
  if (index > size_) {
    throw ArrayOutOfRangeException("insert position out of range");
  }
  if (size_ == N) {
    throw ArrayOverflowException("StaticVector capacity exceeded");
  }

  // Special case: inserting at the end.
  if (index == size_) {
    return emplace_back(std::forward<Args>(args)...);
  }

  T* data_ptr = data();

  // Construct value first so no mutation happens if construction throws.
  T value(std::forward<Args>(args)...);

  // Open a slot at the end by move/copy-constructing from the last element.
  std::construct_at(data_ptr + size_, std::move_if_noexcept(data_ptr[size_ - 1]));

  // Shift the remaining elements one position to the right.
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

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::insert(size_t index, const T& value) -> void requires InsertCopyArrayElement<T>
{
  emplace(index, value);
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::insert(size_t index, T&& value) -> void requires InsertMoveArrayElement<T>
{
  emplace(index, std::move(value));
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::pop_back() -> void {
  if (is_empty()) {
    throw ArrayUnderflowException("pop_back on empty static vector");
  }
  std::destroy_at(data() + size_ - 1);
  --size_;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::erase(size_t index) -> void {
  if (index >= size_) {
    throw ArrayOutOfRangeException("erase position out of range");
  }

  // Shift elements left to fill the gap, then destroy the vacated last element.
  T* data_ptr = data();
  std::move(data_ptr + index + 1, data_ptr + size_, data_ptr + index);
  std::destroy_at(data_ptr + size_ - 1);
  --size_;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::clear() noexcept -> void {
  if constexpr (!std::is_trivially_destructible_v<T>) {
    for (size_t i = 0; i < size_; ++i) {
      std::destroy_at(data() + i);
    }
  }
  size_ = 0;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::resize(size_t new_size) -> void requires ResizeArrayElement<T>
{
  if (new_size > N) {
    throw ArrayOverflowException("StaticVector capacity exceeded");
  }

  if (new_size < size_) {
    for (size_t i = new_size; i < size_; ++i) {
      std::destroy_at(data() + i);
    }
    size_ = new_size;
    return;
  }

  while (size_ < new_size) {
    std::construct_at(data() + size_);
    ++size_;
  }
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::resize(size_t new_size, const T& value) -> void requires CopyArrayElement<T>
{
  if (new_size > N) {
    throw ArrayOverflowException("StaticVector capacity exceeded");
  }

  if (new_size < size_) {
    for (size_t i = new_size; i < size_; ++i) {
      std::destroy_at(data() + i);
    }
    size_ = new_size;
    return;
  }

  while (size_ < new_size) {
    std::construct_at(data() + size_, value);
    ++size_;
  }
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::operator[](size_t index) -> T& {
  return data()[index];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::operator[](size_t index) const -> const T& {
  return data()[index];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::at(size_t index) -> T& {
  if (index >= size_) {
    throw ArrayOutOfRangeException("StaticVector index out of range");
  }
  return data()[index];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::at(size_t index) const -> const T& {
  if (index >= size_) {
    throw ArrayOutOfRangeException("StaticVector index out of range");
  }
  return data()[index];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::front() -> T& {
  if (is_empty()) {
    throw ArrayUnderflowException("front on empty static vector");
  }
  return data()[0];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::front() const -> const T& {
  if (is_empty()) {
    throw ArrayUnderflowException("front on empty static vector");
  }
  return data()[0];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::back() -> T& {
  if (is_empty()) {
    throw ArrayUnderflowException("back on empty static vector");
  }
  return data()[size_ - 1];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::back() const -> const T& {
  if (is_empty()) {
    throw ArrayUnderflowException("back on empty static vector");
  }
  return data()[size_ - 1];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::data() noexcept -> T* {
  return reinterpret_cast<T*>(storage_);
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::data() const noexcept -> const T* {
  return reinterpret_cast<const T*>(storage_);
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::is_full() const noexcept -> bool {
  return size_ == N;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::size() const noexcept -> size_t {
  return size_;
}

//===----- ITERATOR OPERATIONS -------------------------------------------------===//

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::begin() noexcept -> iterator {
  return data();
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::begin() const noexcept -> const_iterator {
  return data();
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::end() noexcept -> iterator {
  return data() + size_;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticVector<T, N>::end() const noexcept -> const_iterator {
  return data() + size_;
}

} // namespace ads::arrays

//===---------------------------------------------------------------------------===//
