//===---------------------------------------------------------------------------===//
/**
 * @file Array_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Defines reusable C++20 concepts for array containers.
 * @version 0.1
 * @date 2026-02-06
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef ARRAY_CONCEPTS_HPP
#define ARRAY_CONCEPTS_HPP

#include "../../support/Support.hpp"

#include <compare>
#include <concepts>
#include <cstddef>
#include <ranges>

/**
 * @brief Concepts for array elements and their properties.
 * @details These concepts define the requirements for types used as elements
 *          in dynamic array containers, including destructibility, relocatability,
 *          and constructibility. They enable compile-time checks and constraints
 *          on template parameters for array classes.
 */
namespace ads::arrays {

namespace sup = ads::support;

/**
 * @brief Concepts describing the container-level interface of array structures.
 * @details Whereas the concepts above constrain the element type, these constrain
 *          the container type itself. They document and enforce the shared public
 *          surface that every array structure exposes, allowing generic algorithms
 *          and tests to be written once against the interface rather than against
 *          each concrete container.
 */

template <typename C> concept SizedContainer = requires(const C container) {
  { container.size() } -> std::convertible_to<size_t>;
  { container.is_empty() } -> std::convertible_to<bool>;
};

template <typename C> concept RandomAccessSequence =
    SizedContainer<C> && std::ranges::random_access_range<C> && requires(C c, const C cc, size_t i) {
      typename C::value_type;
      { c[i] } -> std::same_as<typename C::value_type&>;
      { cc[i] } -> std::same_as<const typename C::value_type&>;
      { c.at(i) } -> std::same_as<typename C::value_type&>;
    };

template <typename T> concept ArrayElement = sup::NonReferenceDestructible<T>;

template <typename T> concept RelocatableArrayElement = ArrayElement<T> && (sup::Movable<T> || sup::Copyable<T>);

template <typename T, typename... Args> concept EmplaceConstructible = ArrayElement<T> && sup::ConstructibleFrom<T, Args...>;

template <typename T, typename... Args> concept AppendArrayElement = EmplaceConstructible<T, Args...> && RelocatableArrayElement<T>;

template <typename T> concept ShiftAssignableArrayElement = ArrayElement<T> && sup::MoveAssignableFromValue<T>;

template <typename T, typename... Args> concept InsertArrayElement = AppendArrayElement<T, Args...> && ShiftAssignableArrayElement<T>;

template <typename T> concept EqualityComparableArrayElement = ArrayElement<T> && sup::EqualityComparable<T>;

template <typename T> concept ThreeWayComparableArrayElement = ArrayElement<T> && std::three_way_comparable<T>;

template <typename InputIt, typename T> concept RangeArrayElement = RelocatableArrayElement<T> && sup::InputConstructibleFor<InputIt, T>;

template <typename T> concept CopyArrayElement = RelocatableArrayElement<T> && sup::Copyable<T>;

template <typename T> concept MoveArrayElement = RelocatableArrayElement<T> && sup::Movable<T>;

template <typename T> concept InsertCopyArrayElement = ShiftAssignableArrayElement<T> && CopyArrayElement<T>;

template <typename T> concept InsertMoveArrayElement = ShiftAssignableArrayElement<T> && MoveArrayElement<T>;

template <typename T> concept ResizeArrayElement = RelocatableArrayElement<T> && sup::DefaultInitializable<T>;

template <typename T> concept CopyAssignableArrayElement = ArrayElement<T> && sup::CopyAssignableFromConst<T>;

template <typename T> concept MoveAssignableArrayElement = ArrayElement<T> && sup::MoveAssignableFromValue<T>;

template <typename T> concept StaticCopyArrayElement = sup::DefaultInitializable<T> && CopyAssignableArrayElement<T>;

template <typename T> concept StaticMoveArrayElement = sup::DefaultInitializable<T> && MoveAssignableArrayElement<T>;

template <typename T> concept SwappableArrayElement = ArrayElement<T> && sup::Swappable<T>;

template <size_t N> concept ValidStaticArrayExtent = (N > 0);

} // namespace ads::arrays

#endif // ARRAY_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
