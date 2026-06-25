//===---------------------------------------------------------------------------===//
/**
 * @file Set.hpp
 * @author Costantino Lombardi
 * @brief Compile-time contract for set (unique-element) data structures.
 * @version 0.1
 * @date 2026-06-25
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SET_HPP
#define SET_HPP

#include <concepts>
#include <cstddef>
#include <utility>

namespace ads::associative {

/**
 * @brief Concept for move-capable set (unique-element) types.
 *
 * @details The contract is intentionally compile-time based and structural:
 *          it pins down the operations every set exposes regardless of whether
 *          the element type is copyable.
 *
 * @tparam SetType The set implementation type.
 * @tparam T The element type.
 */
template <typename SetType, typename T> concept Set = requires {
  typename SetType::value_type;
} && std::same_as<typename SetType::value_type, T> && requires(SetType& set, const SetType& const_set, const T& value, T&& movable_value) {
  { set.insert(std::move(movable_value)) } -> std::same_as<bool>;
  { set.erase(value) } -> std::same_as<bool>;
  { const_set.contains(value) } -> std::same_as<bool>;
  { const_set.count(value) } -> std::same_as<size_t>;
  { const_set.size() } noexcept -> std::same_as<size_t>;
  { const_set.is_empty() } noexcept -> std::same_as<bool>;
};

/**
 * @brief Concept for sets that also support copy-based insertion.
 *
 * @tparam SetType The set implementation type.
 * @tparam T The element type.
 */
template <typename SetType, typename T> concept CopyInsertSet =
    Set<SetType, T> && std::copy_constructible<T> && requires(SetType& set, const T& value) {
      { set.insert(value) } -> std::same_as<bool>;
    };

} // namespace ads::associative

#endif // SET_HPP

//===---------------------------------------------------------------------------===//
