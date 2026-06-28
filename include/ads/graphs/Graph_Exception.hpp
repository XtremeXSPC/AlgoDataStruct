//===---------------------------------------------------------------------------===//
/**
 * @file Graph_Exception.hpp
 * @author Costantino Lombardi
 * @brief Shared exception root for the graph module.
 * @version 0.1
 * @date 2026-06-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef GRAPH_EXCEPTION_HPP
#define GRAPH_EXCEPTION_HPP

#include <stdexcept>

namespace ads::graphs {

/**
 * @brief Common base for every exception raised by the graph module.
 *
 * @details Containers, the Union-Find structure, and the reusable algorithms all
 *          throw types derived from GraphError, so callers can catch any graph
 *          failure with a single handler while still being able to catch the
 *          concrete subclasses when finer granularity is needed.
 */
class GraphError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

} // namespace ads::graphs

#endif // GRAPH_EXCEPTION_HPP

//===---------------------------------------------------------------------------===//
