//===--------------------------------------------------------------------------===//
/**
 * @file QueueException.hpp
 * @author Costantino Lombardi
 * @brief Defines custom exceptions for queue operations.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef QUEUE_EXCEPTION_HPP
#define QUEUE_EXCEPTION_HPP

#include <stdexcept>

namespace ads::queue {

/**
 * @brief Exception thrown for logical errors in queue operations.
 *
 * @details Thrown when an operation is called in an invalid state,
 *          such as attempting to dequeue from or access elements of an empty queue.
 */
class QueueException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Exception thrown when a queue operation would exceed capacity limits.
 *
 * @details This exception is primarily used in fixed-capacity queue implementations
 *          when attempting to enqueue an element would exceed the maximum allowed size.
 */
class QueueOverflowException : public QueueException {
public:
  QueueOverflowException() : QueueException("Queue overflow: maximum capacity exceeded") {}
  explicit QueueOverflowException(const char* message) : QueueException(message) {}
};

/**
 * @brief Exception thrown when attempting operations on an empty queue.
 */
class QueueUnderflowException : public QueueException {
public:
  QueueUnderflowException() : QueueException("Queue underflow: operation on empty queue") {}
  explicit QueueUnderflowException(const char* message) : QueueException(message) {}
};

} // namespace ads::queue

#endif // QUEUE_EXCEPTION_HPP

//===--------------------------------------------------------------------------===//