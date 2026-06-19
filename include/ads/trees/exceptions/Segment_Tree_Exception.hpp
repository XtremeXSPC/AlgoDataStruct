//===---------------------------------------------------------------------------===//
/**
 * @file Segment_Tree_Exception.hpp
 * @author Costantino Lombardi
 * @brief Defines custom exceptions for Segment Tree operations.
 * @version 0.1
 * @date 2026-02-03
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SEGMENT_TREE_EXCEPTION_HPP
#define SEGMENT_TREE_EXCEPTION_HPP

#include <stdexcept>

namespace ads::trees {

/**
 * @brief Base exception for Segment Tree operations.
 */
class SegmentTreeException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

} // namespace ads::trees

#endif // SEGMENT_TREE_EXCEPTION_HPP

//===---------------------------------------------------------------------------===//
