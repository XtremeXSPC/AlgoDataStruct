//===---------------------------------------------------------------------------===//
/**
 * @file Matrix_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Reusable C++20 concepts for matrix containers.
 * @version 0.1
 * @date 2026-06-07
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef MATRIX_CONCEPTS_HPP
#define MATRIX_CONCEPTS_HPP

#include "../../support/Support.hpp"

namespace ads::matrices {

namespace sup = ads::support;

template <typename Value> concept MatrixValue = sup::DefaultEqualityComparable<Value>;

template <typename Value> concept CopyMatrixValue = MatrixValue<Value> && sup::Copyable<Value>;

template <typename InputIt, typename Entry> concept MatrixEntryRange = sup::InputConstructibleFor<InputIt, Entry>;

} // namespace ads::matrices

#endif // MATRIX_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
