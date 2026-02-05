//===---------------------------------------------------------------------------===//
/**
 * @file Hash_Table_Exception.hpp
 * @author Costantino Lombardi
 * @brief Defines custom exceptions for hash table operations.
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef HASH_TABLE_EXCEPTION_HPP
#define HASH_TABLE_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace ads::hash {

/**
 * @brief Base exception for hash table operations.
 *
 * @details Thrown when an operation is called in an invalid state,
 *          such as attempting to access a non-existent key.
 */
class HashTableException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Exception thrown when attempting to access a non-existent key.
 *
 * @details This exception is thrown when trying to access or modify
 *          a key that doesn't exist in the hash table.
 */
class KeyNotFoundException : public HashTableException {
public:
  using HashTableException::HashTableException;

  KeyNotFoundException() : HashTableException("Key not found in hash table") {}
};

/**
 * @brief Exception thrown when attempting operations on an empty table.
 *
 * @details This exception is thrown when trying to perform operations
 *          that require at least one element in an empty hash table.
 */
class EmptyTableException : public HashTableException {
public:
  using HashTableException::HashTableException;

  EmptyTableException() : HashTableException("Operation on empty hash table") {}
};

/**
 * @brief Exception thrown when an invalid operation is attempted.
 *
 * @details This exception covers various invalid operations such as
 *          setting invalid load factors or capacity values.
 */
class InvalidOperationException : public HashTableException {
public:
  using HashTableException::HashTableException;

  InvalidOperationException() : HashTableException("Invalid hash table operation") {}
};

} // namespace ads::hash

#endif // HASH_TABLE_EXCEPTION_HPP

//===---------------------------------------------------------------------------===//
