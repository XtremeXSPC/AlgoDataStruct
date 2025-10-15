//===--------------------------------------------------------------------------===//
/**
 * @file List_Exception.hpp
 * @author Costantino Lombardi
 * @brief Defines custom exceptions for data structures.
 * @version 0.2
 * @date 2025-06-30
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef ADS_EXCEPTION_HPP
#define ADS_EXCEPTION_HPP

#include <stdexcept>

namespace ads::list {

/**
 * @brief Base exception for logical errors in list operations.
 *
 * Thrown when an operation is called in an invalid state
 * (e.g., accessing an element of an empty list).
 */
class ListException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

} // namespace ads::list

#endif // ADS_EXCEPTION_HPP

//===--------------------------------------------------------------------------===//