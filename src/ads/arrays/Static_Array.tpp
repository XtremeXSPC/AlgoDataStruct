//===---------------------------------------------------------------------------===//
/**
 * @file Static_Array.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for StaticArray.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/arrays/Static_Array.hpp"

namespace ads::arrays {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
StaticArray<T, N>::StaticArray() requires sup::DefaultInitializable<T>
    : data_{} {
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
StaticArray<T, N>::StaticArray(std::initializer_list<T> values) requires StaticCopyArrayElement<T>
{
  if (values.size() != N) {
    throw ArrayOutOfRangeException("Initializer list size must match array size");
  }
  std::ranges::copy(values, data_);
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
StaticArray<T, N>::StaticArray(const T& value) requires StaticCopyArrayElement<T>
{
  std::ranges::fill(data_, value);
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
StaticArray<T, N>::StaticArray(const StaticArray& other) requires StaticCopyArrayElement<T>
{
  std::ranges::copy(other.data_, data_);
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
StaticArray<T, N>::StaticArray(StaticArray&& other) noexcept(std::is_nothrow_assignable_v<T&, T>) requires StaticMoveArrayElement<T>
{
  std::ranges::move(other.data_, data_);
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::operator=(const StaticArray& other) -> StaticArray& requires CopyAssignableArrayElement<T>
{
  if (this != &other) {
    std::ranges::copy(other.data_, data_);
  }
  return *this;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::operator=(StaticArray&& other) noexcept(std::is_nothrow_assignable_v<T&, T>)
    -> StaticArray& requires MoveAssignableArrayElement<T>
{
  if (this != &other) {
    std::ranges::move(other.data_, data_);
  }
  return *this;
}

//===------------------------- MODIFICATION OPERATIONS -------------------------===//

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::fill(const T& value) -> void requires CopyAssignableArrayElement<T>
{
  std::ranges::fill(data_, value);
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::swap(StaticArray& other) noexcept(std::is_nothrow_swappable_v<T>) -> void requires SwappableArrayElement<T>
{
  std::swap_ranges(std::begin(data_), std::end(data_), std::begin(other.data_));
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::operator[](size_t index) -> T& {
  return data_[index];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::operator[](size_t index) const -> const T& {
  return data_[index];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::at(size_t index) -> T& {
  if (index >= N) {
    throw ArrayOutOfRangeException("StaticArray index out of range");
  }
  return data_[index];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::at(size_t index) const -> const T& {
  if (index >= N) {
    throw ArrayOutOfRangeException("StaticArray index out of range");
  }
  return data_[index];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::front() -> T& {
  return data_[0];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::front() const -> const T& {
  return data_[0];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::back() -> T& {
  return data_[N - 1];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::back() const -> const T& {
  return data_[N - 1];
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::data() noexcept -> T* {
  return data_;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::data() const noexcept -> const T* {
  return data_;
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::begin() noexcept -> iterator {
  return data_;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::end() noexcept -> iterator {
  return data_ + N;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::begin() const noexcept -> const_iterator {
  return data_;
}

template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
auto StaticArray<T, N>::end() const noexcept -> const_iterator {
  return data_ + N;
}

// cbegin/cend, the reverse-iterator accessors, and the relational operators are
// provided by ContainerFacade<StaticArray<T, N>>; no out-of-line definitions needed.

} // namespace ads::arrays

//===---------------------------------------------------------------------------===//
