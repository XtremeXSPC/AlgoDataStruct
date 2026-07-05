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

#include <concepts>
#include <type_traits>
#include <utility>

namespace ads::heaps {

namespace sup = ads::support;

namespace detail {

///@brief Returns true when two comparator objects define a compatible ordering.
template <typename Compare>
[[nodiscard]] constexpr auto compatible_comparators(const Compare& lhs, const Compare& rhs) -> bool {
  if constexpr (std::is_empty_v<Compare>) {
    return true;
  } else if constexpr (std::equality_comparable<Compare>) {
    return lhs == rhs;
  } else {
    return false;
  }
}

} // namespace detail

template <typename T> concept HeapValue = sup::NonReferenceDestructible<T>;

template <typename T> concept CopyHeapValue = HeapValue<T> && sup::Copyable<T>;

template <typename T> concept MoveHeapValue = HeapValue<T> && sup::Movable<T>;

template <typename T> concept OrderedHeapValue = HeapValue<T> && sup::OrderedValue<T>;

template <typename T, typename... Args> concept EmplaceHeapValue = HeapValue<T> && sup::ConstructibleFrom<T, Args...>;

template <typename InputIt, typename T> concept HeapRangeValue = HeapValue<T> && sup::InputConstructibleFor<InputIt, T>;

/**
 * @brief Models a meldable (mergeable) heap: the shared contract of the
 *        Leftist / Skew / Binomial / Pairing / Fibonacci family.
 *
 * @details Captures the operations generic code may rely on across every
 *          meldable heap: priority access (@c top), removal (@c extract_top),
 *          insertion, the defining @c merge primitive, and the size queries.
 *          The contract requires a movable value type so the @c insert probe is
 *          well-formed; this holds for every practical heap element type. The
 *          handle-based @c decrease_key / @c erase extension owned by the forest
 *          heaps is intentionally kept outside this concept.
 */
template <typename H> concept MeldableHeap =
    requires {
      typename H::value_type;
      typename H::size_type;
    } && HeapValue<typename H::value_type> && MoveHeapValue<typename H::value_type>
    && requires(H heap, H other, const H const_heap, typename H::value_type value) {
         { const_heap.top() } -> std::same_as<const typename H::value_type&>;
         { heap.extract_top() } -> std::same_as<typename H::value_type>;
         { heap.insert(std::move(value)) } -> std::same_as<void>;
         { heap.merge(std::move(other)) } -> std::same_as<void>;
         { const_heap.is_empty() } -> std::same_as<bool>;
         { const_heap.size() } -> std::same_as<typename H::size_type>;
         { heap.clear() } -> std::same_as<void>;
       };

} // namespace ads::heaps

#endif // HEAP_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
