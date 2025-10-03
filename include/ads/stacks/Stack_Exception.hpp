//===--------------------------------------------------------------------------===//
/**
 * @file StackException.hpp
 * @author Costantino Lombardi
 * @brief Defines custom exceptions for stack operations.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef STACK_EXCEPTION_HPP
#define STACK_EXCEPTION_HPP

#include <stdexcept>

namespace ads::stack {

/**
 * @brief Exception thrown for logical errors in stack operations.
 *
 * @details Thrown when an operation is called in an invalid state,
 *          such as attempting to pop from or access the top of an empty stack.
 */
class StackException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Exception thrown when a stack operation would exceed capacity limits.
 *
 * @details This exception is primarily used in fixed-capacity stack implementations
 *          when attempting to push an element would exceed the maximum allowed size.
 */
class StackOverflowException : public StackException {
public:
  StackOverflowException() : StackException("Stack overflow: maximum capacity exceeded") {}
  explicit StackOverflowException(const char* message) : StackException(message) {}
};

/**
 * @brief Exception thrown when attempting operations on an empty stack.
 */
class StackUnderflowException : public StackException {
public:
  StackUnderflowException() : StackException("Stack underflow: operation on empty stack") {}
  explicit StackUnderflowException(const char* message) : StackException(message) {}
};

} // namespace ads::stack

#endif // STACK_EXCEPTION_HPP

//===--------------------------------------------------------------------------===//