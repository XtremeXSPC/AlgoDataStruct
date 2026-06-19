//===---------------------------------------------------------------------------===//
/**
 * @file Fenwick_Tree_Exception.hpp
 * @author Costantino Lombardi
 * @brief Defines custom exceptions for Fenwick tree operations.
 * @version 0.1
 * @date 2026-01-28
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef FENWICK_TREE_EXCEPTION_HPP
#define FENWICK_TREE_EXCEPTION_HPP

#include <stdexcept>

namespace ads::trees {

/**
 * @brief Base exception for Fenwick tree operations.
 */
class FenwickTreeException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

} // namespace ads::trees

#endif // FENWICK_TREE_EXCEPTION_HPP

//===---------------------------------------------------------------------------===//
