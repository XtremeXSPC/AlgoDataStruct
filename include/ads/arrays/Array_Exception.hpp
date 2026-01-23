//===---------------------------------------------------------------------------===//
/**
 * @file Array_Exception.hpp
 * @author Costantino Lombardi
 * @brief Defines custom exceptions for dynamic array operations.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef ARRAY_EXCEPTION_HPP
#define ARRAY_EXCEPTION_HPP

#include <stdexcept>

namespace ads::arrays {

/**
 * @brief Base exception for logical errors in array operations.
 *
 * @details Thrown when an operation is called in an invalid state,
 *          such as accessing an element of an empty array.
 */
class ArrayException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Exception thrown when an array operation exceeds capacity limits.
 */
class ArrayOverflowException : public ArrayException {
public:
  using ArrayException::ArrayException;
  ArrayOverflowException() : ArrayException("Array overflow: maximum capacity exceeded") {}
};

/**
 * @brief Exception thrown when attempting operations on an empty array.
 */
class ArrayUnderflowException : public ArrayException {
public:
  using ArrayException::ArrayException;
  ArrayUnderflowException() : ArrayException("Array underflow: operation on empty array") {}
};

/**
 * @brief Exception thrown when accessing an index out of bounds.
 */
class ArrayOutOfRangeException : public ArrayException {
public:
  using ArrayException::ArrayException;
  ArrayOutOfRangeException() : ArrayException("Array index out of range") {}
};

} // namespace ads::arrays

#endif // ARRAY_EXCEPTION_HPP

//===---------------------------------------------------------------------------===//