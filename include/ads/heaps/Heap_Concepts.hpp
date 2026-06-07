//===---------------------------------------------------------------------------===//
/**
 * @file Heap_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Reusable C++20 concepts for heap containers.
 * @version 0.1
 * @date 2026-06-07
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef HEAP_CONCEPTS_HPP
#define HEAP_CONCEPTS_HPP

#include "../../support/Support.hpp"

namespace ads::heaps {

namespace sup = ads::support;

template <typename T> concept HeapValue = sup::NonReferenceDestructible<T>;

template <typename T> concept CopyHeapValue = HeapValue<T> && sup::Copyable<T>;

template <typename T> concept MoveHeapValue = HeapValue<T> && sup::Movable<T>;

template <typename T, typename... Args> concept EmplaceHeapValue = HeapValue<T> && sup::ConstructibleFrom<T, Args...>;

template <typename InputIt, typename T> concept HeapRangeValue = HeapValue<T> && sup::InputConstructibleFor<InputIt, T>;

} // namespace ads::heaps

#endif // HEAP_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
