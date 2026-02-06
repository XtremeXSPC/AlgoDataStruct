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
  requires(N > 0)
StaticArray<T, N>::StaticArray()
  requires std::default_initializable<T>
    : data_{} {
}

template <ArrayElement T, size_t N>
  requires(N > 0)
StaticArray<T, N>::StaticArray(std::initializer_list<T> values)
  requires std::default_initializable<T> && std::assignable_from<T&, const T&>
{
  if (values.size() != N) {
    throw ArrayOutOfRangeException("Initializer list size must match array size");
  }
  std::ranges::copy(values, data_);
}

template <ArrayElement T, size_t N>
  requires(N > 0)
StaticArray<T, N>::StaticArray(const T& value)
  requires std::default_initializable<T> && std::assignable_from<T&, const T&>
{
  std::ranges::fill(data_, value);
}

template <ArrayElement T, size_t N>
  requires(N > 0)
StaticArray<T, N>::StaticArray(const StaticArray& other)
  requires std::default_initializable<T> && std::assignable_from<T&, const T&>
{
  std::ranges::copy(other.data_, data_);
}

template <ArrayElement T, size_t N>
  requires(N > 0)
StaticArray<T, N>::StaticArray(StaticArray&& other) noexcept(std::is_nothrow_assignable_v<T&, T>)
  requires std::default_initializable<T> && std::assignable_from<T&, T>
{
  std::ranges::move(other.data_, data_);
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator=(const StaticArray& other) -> StaticArray&
  requires std::assignable_from<T&, const T&>
{
  if (this != &other) {
    std::ranges::copy(other.data_, data_);
  }
  return *this;
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator=(StaticArray&& other) noexcept(std::is_nothrow_assignable_v<T&, T>) -> StaticArray&
  requires std::assignable_from<T&, T>
{
  if (this != &other) {
    std::ranges::move(other.data_, data_);
  }
  return *this;
}

//===------------------------- MODIFICATION OPERATIONS -------------------------===//

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::fill(const T& value) -> void
  requires std::assignable_from<T&, const T&>
{
  std::ranges::fill(data_, value);
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::swap(StaticArray& other) noexcept(std::is_nothrow_swappable_v<T>) -> void
  requires std::swappable<T>
{
  std::swap_ranges(std::begin(data_), std::end(data_), std::begin(other.data_));
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator[](size_t index) -> T& {
  return data_[index];
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator[](size_t index) const -> const T& {
  return data_[index];
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::at(size_t index) -> T& {
  if (index >= N) {
    throw ArrayOutOfRangeException("StaticArray index out of range");
  }
  return data_[index];
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::at(size_t index) const -> const T& {
  if (index >= N) {
    throw ArrayOutOfRangeException("StaticArray index out of range");
  }
  return data_[index];
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::front() -> T& {
  return data_[0];
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::front() const -> const T& {
  return data_[0];
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::back() -> T& {
  return data_[N - 1];
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::back() const -> const T& {
  return data_[N - 1];
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::data() noexcept -> T* {
  return data_;
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::data() const noexcept -> const T* {
  return data_;
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::begin() noexcept -> iterator {
  return data_;
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::end() noexcept -> iterator {
  return data_ + N;
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::begin() const noexcept -> const_iterator {
  return data_;
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::end() const noexcept -> const_iterator {
  return data_ + N;
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::cbegin() const noexcept -> const_iterator {
  return data_;
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::cend() const noexcept -> const_iterator {
  return data_ + N;
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::crbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::crend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

//===-------------------------- COMPARISON OPERATORS ---------------------------===//

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator==(const StaticArray& other) const -> bool
  requires EqualityComparableArrayElement<T>
{
  return std::ranges::equal(data_, other.data_);
}

template <ArrayElement T, size_t N>
  requires(N > 0)
auto StaticArray<T, N>::operator<=>(const StaticArray& other) const
  requires ThreeWayComparableArrayElement<T>
{
  return std::lexicographical_compare_three_way(data_, data_ + N, other.data_, other.data_ + N);
}

} // namespace ads::arrays

//===---------------------------------------------------------------------------===//
