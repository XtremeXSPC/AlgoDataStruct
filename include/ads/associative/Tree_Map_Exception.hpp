//===---------------------------------------------------------------------------===//
/**
 * @file Tree_Map_Exception.hpp
 * @author Costantino Lombardi
 * @brief Defines custom exceptions for TreeMap operations.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef TREE_MAP_EXCEPTION_HPP
#define TREE_MAP_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace ads::associative {

/**
 * @brief Base exception for TreeMap operations.
 */
class TreeMapException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Exception thrown when attempting to access a non-existent key.
 */
class KeyNotFoundException : public TreeMapException {
public:
  KeyNotFoundException() : TreeMapException("Key not found in tree map") {}
  explicit KeyNotFoundException(const std::string& message) : TreeMapException(message) {}
};

} // namespace ads::associative

#endif // TREE_MAP_EXCEPTION_HPP

//===---------------------------------------------------------------------------===//
