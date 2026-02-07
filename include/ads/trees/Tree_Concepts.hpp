//===---------------------------------------------------------------------------===//
/**
 * @file Tree_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Defines reusable C++20 concepts for tree containers.
 * @version 0.1
 * @date 2026-02-07
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef TREE_CONCEPTS_HPP
#define TREE_CONCEPTS_HPP

#include <concepts>
#include <type_traits>

namespace ads::trees {

template <typename T>
concept TreeElement = std::destructible<T> && !std::is_reference_v<T>;

template <typename T>
concept EqualityComparableTreeElement = TreeElement<T> && std::equality_comparable<T>;

template <typename T>
concept OrderedTreeElement = EqualityComparableTreeElement<T> && requires(const T& lhs, const T& rhs) {
  { lhs < rhs } -> std::convertible_to<bool>;
};

template <int MinDegree>
concept ValidBTreeDegree = (MinDegree >= 2);

template <typename T>
concept FenwickElement = requires(T a, T b) {
  { T{} };
  { a += b } -> std::same_as<T&>;
  { a - b } -> std::convertible_to<T>;
  { a < b } -> std::convertible_to<bool>;
};

} // namespace ads::trees

#endif // TREE_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
