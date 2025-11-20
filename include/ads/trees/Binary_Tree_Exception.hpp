//===--------------------------------------------------------------------------===//
/**
 * @file Binary_Tree_Exception.hpp
 * @author Costantino Lombardi
 * @brief Defines custom exceptions for binary tree operations.
 * @version 0.1
 * @date 2025-10-15
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef BINARY_TREE_EXCEPTION_HPP
#define BINARY_TREE_EXCEPTION_HPP

#include <stdexcept>

namespace ads::trees {

/**
 * @brief Base exception for logical errors in binary tree operations.
 *
 * @details Thrown when an operation is called in an invalid state,
 *          such as attempting to access elements in an empty tree or
 *          performing invalid operations on tree nodes.
 */
class BinaryTreeException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Exception thrown when attempting to access a non-existent element.
 *
 * @details This exception is thrown when searching for an element that
 *          doesn't exist in the tree, or when trying to access the root
 *          of an empty tree.
 */
class ElementNotFoundException : public BinaryTreeException {
public:
  ElementNotFoundException() : BinaryTreeException("Element not found in tree") {}
  explicit ElementNotFoundException(const char* message) : BinaryTreeException(message) {}
};

/**
 * @brief Exception thrown when attempting operations on an empty tree.
 *
 * @details This exception is thrown when trying to perform operations
 *          that require at least one element, such as finding the minimum
 *          or maximum value in an empty tree.
 */
class EmptyTreeException : public BinaryTreeException {
public:
  EmptyTreeException() : BinaryTreeException("Operation on empty tree") {}
  explicit EmptyTreeException(const char* message) : BinaryTreeException(message) {}
};

/**
 * @brief Exception thrown when an invalid tree operation is attempted.
 *
 * @details This exception covers various invalid operations such as
 *          attempting to remove from an invalid iterator or performing
 *          operations that would violate tree invariants.
 */
class InvalidOperationException : public BinaryTreeException {
public:
  InvalidOperationException() : BinaryTreeException("Invalid tree operation") {}
  explicit InvalidOperationException(const char* message) : BinaryTreeException(message) {}
};

} // namespace ads::trees

#endif // BINARY_TREE_EXCEPTION_HPP

//===--------------------------------------------------------------------------===//
