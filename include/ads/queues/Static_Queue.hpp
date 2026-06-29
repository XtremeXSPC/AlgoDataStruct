//===---------------------------------------------------------------------------===//
/**
 * @file Static_Queue.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the StaticQueue class template.
 * @version 0.1
 * @date 2026-06-09
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef STATIC_QUEUE_HPP
#define STATIC_QUEUE_HPP

#include "Queue.hpp"
#include "Queue_Exception.hpp"

#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace ads::queues {

/**
 * @brief A fixed-capacity queue with inline storage (bounded circular FIFO).
 *
 * @details StaticQueue stores up to N elements in an inline, stack-allocated circular
 *          buffer and never touches the heap. It implements the Queue interface and
 *          throws QueueOverflowException when an enqueue would exceed the capacity N.
 *          It complements CircularArrayQueue (heap-backed, growable) and LinkedQueue
 *          (node-based) with an allocation-free bounded variant, and gives
 *          QueueOverflowException its intended purpose. Element lifetimes within the
 *          raw storage are managed manually. Move-only, like the other queues.
 *
 * @tparam T The type of elements stored in the queue.
 * @tparam N The fixed capacity (must be > 0).
 */
template <QueueValue T, size_t N>
requires(N > 0)
class StaticQueue : public Queue<T> {
public:
  using value_type = T;
  using size_type  = size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty queue (size 0, capacity N).
   * @complexity Time O(1), Space O(1)
   */
  StaticQueue() noexcept;

  /**
   * @brief Move constructor. Move-constructs each element into inline storage,
   *        linearizing the elements so the new front is at index 0.
   * @param other The queue to move from (left empty afterwards).
   * @complexity Time O(n), Space O(1)
   */
  StaticQueue(StaticQueue&& other) noexcept(std::is_nothrow_move_constructible_v<T>);

  /**
   * @brief Destructor. Destroys all live elements.
   * @complexity Time O(n), Space O(1)
   */
  ~StaticQueue() override;

  /**
   * @brief Move assignment operator.
   * @param other The queue to move from (left empty afterwards).
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(StaticQueue&& other) noexcept(std::is_nothrow_move_constructible_v<T>) -> StaticQueue&;

  // Copy constructor and assignment are disabled (move-only type).
  StaticQueue(const StaticQueue&)                    = delete;
  auto operator=(const StaticQueue&) -> StaticQueue& = delete;

  //===----- INSERTION OPERATIONS ----------------------------------------------===//

  /**
   * @brief Constructs an element in-place at the rear of the queue.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   * @throws QueueOverflowException if the queue is already full.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T& requires EmplaceQueueValue<T, Args...>;

  /**
   * @brief Adds an element to the rear of the queue (copy).
   * @param value The value to enqueue.
   * @throws QueueOverflowException if the queue is already full.
   * @complexity Time O(1), Space O(1)
   */
  void enqueue(const T& value) override;

  /**
   * @brief Adds an element to the rear of the queue (move).
   * @param value The value to enqueue.
   * @throws QueueOverflowException if the queue is already full.
   * @complexity Time O(1), Space O(1)
   */
  void enqueue(T&& value) override;

  //===----- REMOVAL OPERATIONS ------------------------------------------------===//

  /**
   * @brief Removes the element at the front of the queue.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  void dequeue() override;

  /**
   * @brief Removes all elements from the queue.
   * @complexity Time O(n), Space O(1)
   */
  void clear() noexcept override;

  //===----- ACCESS OPERATIONS -------------------------------------------------===//

  /**
   * @brief Returns a reference to the front element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() -> T& override;

  /**
   * @brief Returns a const reference to the front element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() const -> const T& override;

  /**
   * @brief Returns a reference to the rear element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto rear() -> T& override;

  /**
   * @brief Returns a const reference to the rear element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto rear() const -> const T& override;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Checks if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool override;

  /**
   * @brief Checks if the queue is full (size() == N).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_full() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the queue.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t override;

  /**
   * @brief Returns the fixed capacity N.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static constexpr auto capacity() noexcept -> size_t { return N; }

  /**
   * @brief Returns the maximum possible size (equal to capacity N).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static constexpr auto max_size() noexcept -> size_t { return N; }

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Returns a typed pointer to the inline storage.
  auto data() noexcept -> T* { return reinterpret_cast<T*>(storage_); }

  ///@brief Returns a typed const pointer to the inline storage.
  auto data() const noexcept -> const T* { return reinterpret_cast<const T*>(storage_); }

  ///@brief Maps a logical offset from the front to a physical index in the ring buffer.
  [[nodiscard]] auto physical(size_t logical) const noexcept -> size_t { return (front_ + logical) % N; }

  //===----- DATA MEMBERS ------------------------------------------------------===//

  size_t front_; ///< Physical index of the front element.
  size_t size_;  ///< Number of constructed elements.

  alignas(T) std::byte storage_[sizeof(T) * N]; ///< Raw inline storage for N elements.
};

} // namespace ads::queues

// Include the implementation file for templates.
#include "../../../src/ads/queues/Static_Queue.tpp"

#endif // STATIC_QUEUE_HPP

//===---------------------------------------------------------------------------===//
