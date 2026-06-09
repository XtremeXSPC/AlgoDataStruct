//===---------------------------------------------------------------------------===//
/**
 * @file Static_Queue.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for StaticQueue.
 * @version 0.1
 * @date 2026-06-09
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/queues/Static_Queue.hpp"

namespace ads::queues {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <QueueValue T, size_t N>
requires(N > 0)
StaticQueue<T, N>::StaticQueue() noexcept : front_(0), size_(0) {
}

template <QueueValue T, size_t N>
requires(N > 0)
StaticQueue<T, N>::StaticQueue(StaticQueue&& other) noexcept(std::is_nothrow_move_constructible_v<T>) : front_(0), size_(0) {
  // Move elements in logical (FIFO) order; the new front is linearized to index 0.
  for (size_t i = 0; i < other.size_; ++i) {
    std::construct_at(data() + i, std::move(other.data()[other.physical(i)]));
    ++size_;
  }
  other.clear();
}

template <QueueValue T, size_t N>
requires(N > 0)
StaticQueue<T, N>::~StaticQueue() {
  clear();
}

template <QueueValue T, size_t N>
requires(N > 0)
auto StaticQueue<T, N>::operator=(StaticQueue&& other) noexcept(std::is_nothrow_move_constructible_v<T>) -> StaticQueue& {
  if (this != &other) {
    clear();
    for (size_t i = 0; i < other.size_; ++i) {
      std::construct_at(data() + i, std::move(other.data()[other.physical(i)]));
      ++size_;
    }
    other.clear();
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <QueueValue T, size_t N>
requires(N > 0)
template <typename... Args>
auto StaticQueue<T, N>::emplace(Args&&... args) -> T& requires EmplaceQueueValue<T, Args...>
{
  if (size_ == N) {
    throw QueueOverflowException("StaticQueue capacity exceeded");
  }
  T* rear_ptr = data() + physical(size_);
  std::construct_at(rear_ptr, std::forward<Args>(args)...);
  ++size_;
  return *rear_ptr;
}

template <QueueValue T, size_t N>
requires(N > 0)
void StaticQueue<T, N>::enqueue(const T& value) {
  emplace(value);
}

template <QueueValue T, size_t N>
requires(N > 0)
void StaticQueue<T, N>::enqueue(T&& value) {
  emplace(std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <QueueValue T, size_t N>
requires(N > 0)
void StaticQueue<T, N>::dequeue() {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot dequeue from empty queue");
  }
  std::destroy_at(data() + front_);
  front_ = (front_ + 1) % N;
  --size_;
}

template <QueueValue T, size_t N>
requires(N > 0)
void StaticQueue<T, N>::clear() noexcept {
  if constexpr (!std::is_trivially_destructible_v<T>) {
    for (size_t i = 0; i < size_; ++i) {
      std::destroy_at(data() + physical(i));
    }
  }
  front_ = 0;
  size_  = 0;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <QueueValue T, size_t N>
requires(N > 0)
auto StaticQueue<T, N>::front() -> T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty queue");
  }
  return data()[front_];
}

template <QueueValue T, size_t N>
requires(N > 0)
auto StaticQueue<T, N>::front() const -> const T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty queue");
  }
  return data()[front_];
}

template <QueueValue T, size_t N>
requires(N > 0)
auto StaticQueue<T, N>::rear() -> T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access rear of empty queue");
  }
  return data()[physical(size_ - 1)];
}

template <QueueValue T, size_t N>
requires(N > 0)
auto StaticQueue<T, N>::rear() const -> const T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access rear of empty queue");
  }
  return data()[physical(size_ - 1)];
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <QueueValue T, size_t N>
requires(N > 0)
auto StaticQueue<T, N>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <QueueValue T, size_t N>
requires(N > 0)
auto StaticQueue<T, N>::is_full() const noexcept -> bool {
  return size_ == N;
}

template <QueueValue T, size_t N>
requires(N > 0)
auto StaticQueue<T, N>::size() const noexcept -> size_t {
  return size_;
}

} // namespace ads::queues

//===---------------------------------------------------------------------------===//
