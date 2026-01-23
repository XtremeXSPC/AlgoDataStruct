//===---------------------------------------------------------------------------===//
/**
 * @file Heap_Exception.hpp
 * @author Costantino Lombardi
 * @brief Exception class for heap data structures.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef HEAP_EXCEPTION_HPP
#define HEAP_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace ads::heaps {

/**
 * @brief Exception class for heap-related errors
 *
 * @details This exception is thrown when operations on heap data structures
 *          fail due to precondition violations or invalid states.
 *          Common cases include:
 *            - Attempting to access or remove from an empty heap.
 *            - Invalid index operations.
 *            - Capacity overflow in fixed-size heaps.
 */
class HeapException : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

} // namespace ads::heaps

#endif // HEAP_EXCEPTION_HPP
//===--------------------------------------------------------------------------===//
