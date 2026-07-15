//===---------------------------------------------------------------------------===//
/**
 * @file Range_Exception.hpp
 * @author Costantino Lombardi
 * @brief Defines custom exceptions for range-query structures.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef RANGE_EXCEPTION_HPP
#define RANGE_EXCEPTION_HPP

#include <stdexcept>

namespace ads::range {

/**
 * @brief Base exception for logical errors in range-query structures.
 *
 * @details The inherited constructors allow callers and derived exceptions to
 *          provide operation-specific diagnostic messages.
 */
class RangeException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Exception thrown when an index is outside the valid element range.
 */
class RangeIndexException : public RangeException {
public:
  using RangeException::RangeException;

  ///@brief Constructs the exception with the canonical out-of-bounds message.
  RangeIndexException() : RangeException("Range index out of bounds") {}
};

/**
 * @brief Exception thrown when an inclusive range is invalid or out of bounds.
 */
class InvalidRangeException : public RangeException {
public:
  using RangeException::RangeException;

  ///@brief Constructs the exception with the canonical invalid-range message.
  InvalidRangeException() : RangeException("Invalid query range") {}
};

} // namespace ads::range

#endif // RANGE_EXCEPTION_HPP

//===---------------------------------------------------------------------------===//
