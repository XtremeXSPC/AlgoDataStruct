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

#include <compare>
#include <concepts>
#include <type_traits>

/**
 * @brief Concepts for array elements and their properties.
 * @details These concepts define the requirements for types used as elements
 *          in dynamic array containers, including destructibility, relocatability,
 *          and constructibility. They enable compile-time checks and constraints
 *          on template parameters for array classes.
 */
namespace ads::arrays {

template <typename T>
concept ArrayElement = std::destructible<T> && !std::is_reference_v<T>;

template <typename T>
concept RelocatableArrayElement = ArrayElement<T> && (std::move_constructible<T> || std::copy_constructible<T>);

template <typename T, typename... Args>
concept EmplaceConstructible = ArrayElement<T> && std::constructible_from<T, Args...>;

template <typename T>
concept ShiftAssignableArrayElement = ArrayElement<T> && std::assignable_from<T&, T>;

template <typename T>
concept EqualityComparableArrayElement = ArrayElement<T> && std::equality_comparable<T>;

template <typename T>
concept ThreeWayComparableArrayElement = ArrayElement<T> && std::three_way_comparable<T>;

} // namespace ads::arrays

#endif // ARRAY_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
