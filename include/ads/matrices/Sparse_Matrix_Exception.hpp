//===---------------------------------------------------------------------------===//
/**
 * @file Sparse_Matrix_Exception.hpp
 * @author Costantino Lombardi
 * @brief Exception types for sparse matrix operations.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SPARSE_MATRIX_EXCEPTION_HPP
#define SPARSE_MATRIX_EXCEPTION_HPP

#include <stdexcept>

namespace ads::matrices {

///@brief Exception thrown for invalid sparse matrix operations.
class SparseMatrixException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

} // namespace ads::matrices

#endif // SPARSE_MATRIX_EXCEPTION_HPP

//===---------------------------------------------------------------------------===//
