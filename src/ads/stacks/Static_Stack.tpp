//===---------------------------------------------------------------------------===//
/**
 * @file Static_Stack.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for StaticStack.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/stacks/Static_Stack.hpp"

namespace ads::stacks {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <StackValue T, size_t N>
requires(N > 0)
StaticStack<T, N>::StaticStack() noexcept : size_(0) {
}

template <StackValue T, size_t N>
requires(N > 0)
StaticStack<T, N>::StaticStack(StaticStack&& other) noexcept(std::is_nothrow_move_constructible_v<T>) : size_(0) {
  // Inline storage cannot be stolen, so each element is moved individually.
  for (size_t i = 0; i < other.size_; ++i) {
    std::construct_at(data() + size_, std::move(other.data()[i]));
    ++size_;
  }
  other.clear();
}

template <StackValue T, size_t N>
requires(N > 0)
StaticStack<T, N>::~StaticStack() {
  clear();
}

template <StackValue T, size_t N>
requires(N > 0)
auto StaticStack<T, N>::operator=(StaticStack&& other) noexcept(std::is_nothrow_move_constructible_v<T>) -> StaticStack& {
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

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <StackValue T, size_t N>
requires(N > 0)
template <typename... Args>
auto StaticStack<T, N>::emplace(Args&&... args) -> T& requires EmplaceStackValue<T, Args...>
{
  if (size_ == N) {
    throw StackOverflowException("StaticStack capacity exceeded");
  }
  T* top_ptr = data() + size_;
  std::construct_at(top_ptr, std::forward<Args>(args)...);
  ++size_;
  return *top_ptr;
}

template <StackValue T, size_t N>
requires(N > 0)
void StaticStack<T, N>::push(const T& value) {
  emplace(value);
}

template <StackValue T, size_t N>
requires(N > 0)
void StaticStack<T, N>::push(T&& value) {
  emplace(std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <StackValue T, size_t N>
requires(N > 0)
void StaticStack<T, N>::pop() {
  if (is_empty()) {
    throw StackUnderflowException("Cannot pop from empty stack");
  }
  --size_;
  std::destroy_at(data() + size_);
}

template <StackValue T, size_t N>
requires(N > 0)
void StaticStack<T, N>::clear() noexcept {
  if constexpr (!std::is_trivially_destructible_v<T>) {
    for (size_t i = 0; i < size_; ++i) {
      std::destroy_at(data() + i);
    }
  }
  size_ = 0;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <StackValue T, size_t N>
requires(N > 0)
auto StaticStack<T, N>::top() -> T& {
  if (is_empty()) {
    throw StackUnderflowException("Cannot access top of empty stack");
  }
  return data()[size_ - 1];
}

template <StackValue T, size_t N>
requires(N > 0)
auto StaticStack<T, N>::top() const -> const T& {
  if (is_empty()) {
    throw StackUnderflowException("Cannot access top of empty stack");
  }
  return data()[size_ - 1];
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <StackValue T, size_t N>
requires(N > 0)
auto StaticStack<T, N>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <StackValue T, size_t N>
requires(N > 0)
auto StaticStack<T, N>::is_full() const noexcept -> bool {
  return size_ == N;
}

template <StackValue T, size_t N>
requires(N > 0)
auto StaticStack<T, N>::size() const noexcept -> size_t {
  return size_;
}

} // namespace ads::stacks

//===---------------------------------------------------------------------------===//
