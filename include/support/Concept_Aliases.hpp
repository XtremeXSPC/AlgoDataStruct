//===---------------------------------------------------------------------------===//
/**
 * @file Concept_Aliases.hpp
 * @author Costantino Lombardi
 * @brief Shared concept aliases for readable template constraints.
 * @version 0.1
 * @date 2026-06-07
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef CONCEPT_ALIASES_HPP
#define CONCEPT_ALIASES_HPP

#include <concepts>
#include <iterator>
#include <type_traits>

namespace ads::support {

template <typename T> concept NonReferenceDestructible = std::destructible<T> && !std::is_reference_v<T>;

template <typename T> concept Copyable = std::copy_constructible<T>;

template <typename T> concept Movable = std::move_constructible<T>;

template <typename T> concept DefaultInitializable = std::default_initializable<T>;

template <typename T> concept EqualityComparable = std::equality_comparable<T>;

template <typename T> concept OrderedValue = std::totally_ordered<T>;

template <typename T> concept DefaultEqualityComparable = DefaultInitializable<T> && EqualityComparable<T>;

template <typename T, typename... Args> concept ConstructibleFrom = std::constructible_from<T, Args...>;

template <typename InputIt, typename T> concept InputConstructibleFor =
    std::input_iterator<InputIt> && std::constructible_from<T, std::iter_reference_t<InputIt>>;

template <typename T> concept CopyAssignableFromConst = std::assignable_from<T&, const T&>;

template <typename T> concept MoveAssignableFromValue = std::assignable_from<T&, T>;

template <typename T> concept Swappable = std::swappable<T>;

template <typename Value, typename... Args> concept EmplaceAssignableValue =
    ConstructibleFrom<Value, Args...> && MoveAssignableFromValue<Value>;

template <typename Value> concept CopyInsertableValue = Copyable<Value> && CopyAssignableFromConst<Value>;

template <typename Value> concept MoveInsertableValue = Movable<Value> && MoveAssignableFromValue<Value>;

template <typename Key, typename Value> concept CopyInsertableKeyValue = Copyable<Key> && Copyable<Value> && CopyAssignableFromConst<Value>;

template <typename Key, typename Value> concept CopyKeyMoveValue = Copyable<Key> && MoveInsertableValue<Value>;

template <typename Key, typename Value> concept MoveInsertableKeyValue = Movable<Key> && MoveInsertableValue<Value>;

} // namespace ads::support

#endif // CONCEPT_ALIASES_HPP

//===---------------------------------------------------------------------------===//
