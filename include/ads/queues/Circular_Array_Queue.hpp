//===--------------------------------------------------------------------------===//
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
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef CIRCULAR_ARRAY_QUEUE_HPP
#define CIRCULAR_ARRAY_QUEUE_HPP

#include "Queue.hpp"
#include "Queue_Exception.hpp"
#include <algorithm>
#include <memory>
#include <utility>

namespace ads::queue {

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
  //========== CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ==========//

  /**
   * @brief Constructs an empty queue with optional initial capacity.
   * @param initial_capacity The initial capacity (default: 16).
   * @throws std::bad_alloc if memory allocation fails.
   */
  explicit CircularArrayQueue(size_t initial_capacity = 16);

  /** @brief Destructor. */
  ~CircularArrayQueue() override;

  // Copy constructor and assignment are disabled (move-only type)
  CircularArrayQueue(const CircularArrayQueue&)            = delete;
  CircularArrayQueue& operator=(const CircularArrayQueue&) = delete;

  /**
   * @brief Move constructor.
   * @param other The queue from which to move resources.
   */
  CircularArrayQueue(CircularArrayQueue&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The queue from which to move resources.
   * @return A reference to this instance.
   */
  CircularArrayQueue& operator=(CircularArrayQueue&& other) noexcept;

  //========== INTERFACE INHERITED FROM Queue<T> ==========//

  void enqueue(const T& value) override;
  void enqueue(T&& value) override;
  void dequeue() override;

  T&       front() override;
  const T& front() const override;
  T&       rear() override;
  const T& rear() const override;

  bool   is_empty() const noexcept override;
  size_t size() const noexcept override;
  void   clear() noexcept override;

  //=========== ADDITIONAL FUNCTIONALITY ==========//

  /**
   * @brief Constructs an element in-place at the rear of the queue.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   */
  template <typename... Args>
  T& emplace(Args&&... args);

  /**
   * @brief Returns the current capacity of the internal array.
   * @return The number of elements that can be stored without reallocation.
   */
  [[nodiscard]] size_t capacity() const noexcept { return capacity_; }

  /**
   * @brief Reserves capacity for at least n elements.
   * @param n The minimum capacity to reserve.
   * @throws std::bad_alloc if memory allocation fails.
   */
  void reserve(size_t n);

  /**
   * @brief Shrinks the capacity to match the current size.
   * @details This can be used to release unused memory.
   * @throws std::bad_alloc if memory allocation fails.
   */
  void shrink_to_fit();

private:
  //================ INTERNAL HELPERS ================//

  /**
   * @brief Calculates the next index in the circular buffer.
   * @param index The current index.
   * @return The next index, wrapping around if necessary.
   */
  [[nodiscard]] size_t next_index(size_t index) const noexcept { return (index + 1) % capacity_; }

  /**
   * @brief Calculates the previous index in the circular buffer.
   * @param index The current index.
   * @return The previous index, wrapping around if necessary.
   */
  [[nodiscard]] size_t prev_index(size_t index) const noexcept { return (index + capacity_ - 1) % capacity_; }

  /**
   * @brief Checks if the queue is full.
   * @return true if the queue cannot accept more elements without growing.
   */
  [[nodiscard]] bool is_full() const noexcept {
    return size_ == capacity_ - 1; // One slot is always kept empty
  }

  /**
   * @brief Grows the internal array when capacity is reached.
   */
  void grow();

  /**
   * @brief Reallocates the internal array to new_capacity.
   * @param new_capacity The new capacity.
   */
  void reallocate(size_t new_capacity);

  //================ DATA MEMBERS ================//
  std::unique_ptr<T[]> data_;     ///< The dynamic array holding queue elements
  size_t               front_;    ///< Index of the front element
  size_t               rear_;     ///< Index where the next element will be inserted
  size_t               size_;     ///< The current number of elements
  size_t               capacity_; ///< The current capacity of the array

  static constexpr size_t kGrowthFactor = 2; ///< Growth factor for dynamic resizing
  static constexpr size_t kMinCapacity  = 8; ///< Minimum capacity to maintain
};

// Include the implementation file for templates
#include "../../../src/ads/queues/Circular_Array_Queue.tpp"

} // namespace ads::queue

#endif // CIRCULAR_ARRAY_QUEUE_HPP

//===--------------------------------------------------------------------------===//