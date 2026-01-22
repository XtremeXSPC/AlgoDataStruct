//===---------------------------------------------------------------------------===//
/**
 * @file Circular_Array_Queue.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the CircularArrayQueue class.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef CIRCULAR_ARRAY_QUEUE_HPP
#define CIRCULAR_ARRAY_QUEUE_HPP

#include <algorithm>
#include <limits>
#include <memory>
#include <new>
#include <utility>

#include "Queue.hpp"
#include "Queue_Exception.hpp"

namespace ads::queues {

/**
 * @brief A queue implementation based on a circular dynamic array.
 *
 * @details This class implements the Queue interface using a circular buffer
 *          that wraps around when the end is reached. This design provides O(1)
 *          enqueue and dequeue operations while efficiently using memory.
 *
 *          The circular buffer grows dynamically when needed, maintaining amortized
 *          O(1) complexity. The implementation uses std::unique_ptr for automatic
 *          memory management and follows the RAII principle. The queue is move-only
 *          to prevent expensive deep copies.
 *
 * @tparam T The type of data to store in the queue.
 */
template <typename T>
class CircularArrayQueue : public Queue<T> {
public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty queue with optional initial capacity.
   * @param initial_capacity The initial capacity (default: 16).
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(1), Space O(initial_capacity)
   */
  explicit CircularArrayQueue(size_t initial_capacity = 16);

  /**
   * @brief Destructor. Empties the queue and deallocates all elements.
   * @complexity Time O(n), Space O(1)
   */
  ~CircularArrayQueue() override;

  /**
   * @brief Move constructor.
   * @param other The queue from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  CircularArrayQueue(CircularArrayQueue&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The queue from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(CircularArrayQueue&& other) noexcept -> CircularArrayQueue&;

  // Copy constructor and assignment are disabled (move-only type).
  CircularArrayQueue(const CircularArrayQueue&)                    = delete;
  auto operator=(const CircularArrayQueue&) -> CircularArrayQueue& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Constructs an element in-place at the rear of the queue.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1) amortized, Space O(1)
   * @note May trigger a resize with O(n) time.
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T&;

  /**
   * @brief Adds an element to the rear of the queue (copy).
   * @param value The value to enqueue.
   * @complexity Time O(1) amortized, Space O(1)
   * @note May trigger a resize with O(n) time.
   */
  void enqueue(const T& value) override;

  /**
   * @brief Adds an element to the rear of the queue (move).
   * @param value The value to enqueue.
   * @complexity Time O(1) amortized, Space O(1)
   * @note May trigger a resize with O(n) time.
   */
  void enqueue(T&& value) override;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the element at the front of the queue.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1) amortized, Space O(1)
   * @note May trigger a shrink with O(n) time.
   */
  void dequeue() override;

  /**
   * @brief Removes all elements from the queue.
   * @complexity Time O(n), Space O(1)
   */
  void clear() noexcept override;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Returns a reference to the front element.
   * @return Reference to the front element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() -> T& override;

  /**
   * @brief Returns a const reference to the front element.
   * @return Const reference to the front element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() const -> const T& override;

  /**
   * @brief Returns a reference to the rear element.
   * @return Reference to the rear element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto rear() -> T& override;

  /**
   * @brief Returns a const reference to the rear element.
   * @return Const reference to the rear element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto rear() const -> const T& override;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the queue is empty.
   * @return true if the queue is empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool override;

  /**
   * @brief Returns the number of elements in the queue.
   * @return The number of elements.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t override;

  //===-------------------------- CAPACITY OPERATIONS --------------------------===//

  /**
   * @brief Returns the current capacity of the internal array.
   * @return The number of elements that can be stored without reallocation.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto capacity() const noexcept -> size_t { return capacity_; }

  /**
   * @brief Reserves capacity for at least n elements.
   * @param n The minimum capacity to reserve.
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(n) if reallocation occurs, Space O(n)
   */
  void reserve(size_t n);

  /**
   * @brief Shrinks the capacity to match the current size.
   * @details This can be used to release unused memory.
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(n), Space O(n)
   */
  void shrink_to_fit();

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Calculates the next index in the circular buffer.
   * @param index The current index.
   * @return The next index, wrapping around if necessary.
   */
  [[nodiscard]] auto next_index(size_t index) const noexcept -> size_t { return (index + 1) % capacity_; }

  /**
   * @brief Calculates the previous index in the circular buffer.
   * @param index The current index.
   * @return The previous index, wrapping around if necessary.
   */
  [[nodiscard]] auto prev_index(size_t index) const noexcept -> size_t { return (index + capacity_ - 1) % capacity_; }

  /**
   * @brief Checks if the queue is full.
   * @return true if the queue cannot accept more elements without growing.
   */
  [[nodiscard]] auto is_full() const noexcept -> bool {
    return size_ == capacity_ - 1; // One slot is always kept empty
  }

  /**
   * @brief Grows the internal array when capacity is reached.
   * @throws QueueOverflowException if capacity would overflow.
   * @complexity Time O(n), Space O(n)
   */
  void grow();

  /**
   * @brief Reallocates the internal array to new_capacity.
   * @param new_capacity The new capacity.
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(n), Space O(n)
   */
  void reallocate(size_t new_capacity);

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<T[], void (*)(T*)> data_;     ///< The dynamic array holding queue elements.
  size_t                             front_;    ///< Index of the front element.
  size_t                             rear_;     ///< Index where the next element will be inserted.
  size_t                             size_;     ///< The current number of elements.
  size_t                             capacity_; ///< The current capacity of the array.

  static constexpr size_t kGrowthFactor = 2; ///< Growth factor for dynamic resizing.
  static constexpr size_t kMinCapacity  = 8; ///< Minimum capacity to maintain.
};

} // namespace ads::queues

// Include the implementation file for templates.
#include "../../../src/ads/queues/Circular_Array_Queue.tpp"

#endif // CIRCULAR_ARRAY_QUEUE_HPP

//===---------------------------------------------------------------------------===//
