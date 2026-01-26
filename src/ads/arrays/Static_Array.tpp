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

template <typename T, size_t N>
  requires(N > 0)
StaticArray<T, N>::StaticArray()
  requires std::default_initializable<T>
    : data_{} {}

template <typename T, size_t N>
  requires(N > 0)
StaticArray<T, N>::StaticArray(std::initializer_list<T> values) {
  if (values.size() != N) {
    throw ArrayOutOfRangeException("Initializer list size must match array size");
  }
  std::copy(values.begin(), values.end(), data_);
}

template <typename T, size_t N>
  requires(N > 0)
StaticArray<T, N>::StaticArray(const T& value) {
  std::fill(std::begin(data_), std::end(data_), value);
}

template <typename T, size_t N>
  requires(N > 0)
StaticArray<T, N>::StaticArray(const StaticArray& other)
  requires std::copy_constructible<T>
{
  std::copy(other.data_, other.data_ + N, data_);
}

template <typename T, size_t N>
  requires(N > 0)
StaticArray<T, N>::StaticArray(StaticArray&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
  requires std::move_constructible<T>
{
  std::move(other.data_, other.data_ + N, data_);
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator=(const StaticArray& other) -> StaticArray&
  requires std::is_copy_assignable_v<T>
{
  if (this != &other) {
    std::copy(other.data_, other.data_ + N, data_);
  }
  return *this;
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator=(StaticArray&& other) noexcept(std::is_nothrow_move_assignable_v<T>) -> StaticArray&
  requires std::is_move_assignable_v<T>
{
  if (this != &other) {
    std::move(other.data_, other.data_ + N, data_);
  }
  return *this;
}

//===------------------------ MODIFICATION OPERATIONS ---------------------------===//

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::fill(const T& value) -> void {
  std::fill(std::begin(data_), std::end(data_), value);
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::swap(StaticArray& other) noexcept(std::is_nothrow_swappable_v<T>) -> void {
  std::swap_ranges(std::begin(data_), std::end(data_), std::begin(other.data_));
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator[](size_t index) -> T& {
  return data_[index];
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator[](size_t index) const -> const T& {
  return data_[index];
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::at(size_t index) -> T& {
  if (index >= N) {
    throw ArrayOutOfRangeException("StaticArray index out of range");
  }
  return data_[index];
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::at(size_t index) const -> const T& {
  if (index >= N) {
    throw ArrayOutOfRangeException("StaticArray index out of range");
  }
  return data_[index];
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::front() -> T& {
  return data_[0];
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::front() const -> const T& {
  return data_[0];
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::back() -> T& {
  return data_[N - 1];
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::back() const -> const T& {
  return data_[N - 1];
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::data() noexcept -> T* {
  return data_;
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::data() const noexcept -> const T* {
  return data_;
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::begin() noexcept -> iterator {
  return data_;
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::end() noexcept -> iterator {
  return data_ + N;
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::begin() const noexcept -> const_iterator {
  return data_;
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::end() const noexcept -> const_iterator {
  return data_ + N;
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::cbegin() const noexcept -> const_iterator {
  return data_;
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::cend() const noexcept -> const_iterator {
  return data_ + N;
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::crbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::crend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

//===-------------------------- COMPARISON OPERATORS ---------------------------===//

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator==(const StaticArray& other) const -> bool
  requires std::equality_comparable<T>
{
  return std::equal(data_, data_ + N, other.data_);
}

template <typename T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator<=>(const StaticArray& other) const
  requires std::three_way_comparable<T>
{
  return std::lexicographical_compare_three_way(data_, data_ + N, other.data_, other.data_ + N);
}

} // namespace ads::arrays

//===---------------------------------------------------------------------------===//
