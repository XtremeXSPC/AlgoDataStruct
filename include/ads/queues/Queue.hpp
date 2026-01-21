//===---------------------------------------------------------------------------===//
/**
 * @file Queue.hpp
 * @author Costantino Lombardi
 * @brief Abstract interface for a Queue data structure.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//
#pragma once
#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <cstddef>

namespace ads::queue {

/**
 * @brief Abstract base class that defines common operations for a queue data structure.
 *
 * @details A queue is a First-In-First-Out (FIFO) data structure that supports
 *          insertion at one end (rear) and deletion from the other end (front).
 *          This interface provides the fundamental operations that any queue
 *          implementation must support.
 *
 * @tparam T The data type to store in the queue.
 */
template <typename T>
class Queue {
public:
  // A virtual destructor is mandatory in polymorphic base classes.
  virtual ~Queue() = default;

  /**
   * @brief Adds an element to the rear of the queue (copy).
   * @param value The value to enqueue.
   */
  virtual void enqueue(const T& value) = 0;

  /**
   * @brief Adds an element to the rear of the queue (move).
   * @param value The r-value to move.
   */
  virtual void enqueue(T&& value) = 0;

  /**
   * @brief Removes the element at the front of the queue.
   * @throws QueueException if the queue is empty.
   */
  virtual void dequeue() = 0;

  /**
   * @brief Returns a reference to the front element.
   * @return T& Reference to the front element.
   * @throws QueueException if the queue is empty.
   */
  virtual auto front() -> T& = 0;

  /**
   * @brief Returns a constant reference to the front element.
   * @return const T& Constant reference to the front element.
   * @throws QueueException if the queue is empty.
   */
  virtual auto front() const -> const T& = 0;

  /**
   * @brief Returns a reference to the rear element.
   * @return T& Reference to the rear element.
   * @throws QueueException if the queue is empty.
   */
  virtual auto rear() -> T& = 0;

  /**
   * @brief Returns a constant reference to the rear element.
   * @return const T& Constant reference to the rear element.
   * @throws QueueException if the queue is empty.
   */
  virtual auto rear() const -> const T& = 0;

  /**
   * @brief Checks if the queue is empty.
   * @return true if the queue contains no elements.
   * @return false otherwise.
   */
  [[nodiscard]] virtual auto is_empty() const noexcept -> bool = 0;

  /**
   * @brief Returns the number of elements in the queue.
   * @return size_t The number of elements.
   */
  [[nodiscard]] virtual auto size() const noexcept -> size_t = 0;

  /**
   * @brief Empties the queue, removing all elements.
   */
  virtual void clear() noexcept = 0;
};

} // namespace ads::queue

#endif // QUEUE_HPP

//===---------------------------------------------------------------------------===//
