//===---------------------------------------------------------------------------===//
/**
 * @file Associative_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Reusable C++20 concepts for associative containers.
 * @version 0.1
 * @date 2026-06-07
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef ASSOCIATIVE_CONCEPTS_HPP
#define ASSOCIATIVE_CONCEPTS_HPP

#include "../../support/Support.hpp"
#include "../trees/Tree_Concepts.hpp"

namespace ads::associative {

namespace sup = ads::support;

template <typename Key> concept MapKey = ads::trees::OrderedTreeElement<Key>;

template <typename Value> concept MapValue = sup::NonReferenceDestructible<Value>;

template <typename Key> concept CopyMapKey = MapKey<Key> && sup::Copyable<Key>;

template <typename Key> concept MoveMapKey = MapKey<Key> && sup::Movable<Key>;

template <typename Value> concept CopyMapValue = MapValue<Value> && sup::Copyable<Value>;

template <typename Value> concept MoveMapValue = MapValue<Value> && sup::Movable<Value>;

template <typename Value> concept DefaultMapValue = MapValue<Value> && sup::DefaultInitializable<Value>;

template <typename Value, typename... Args> concept EmplaceMapValue = MapValue<Value> && sup::EmplaceAssignableValue<Value, Args...>;

template <typename Key, typename Value> concept CopyMapEntry = MapKey<Key> && MapValue<Value> && sup::CopyInsertableKeyValue<Key, Value>;

template <typename Key, typename Value> concept CopyKeyMoveMapEntry = MapKey<Key> && MapValue<Value> && sup::CopyKeyMoveValue<Key, Value>;

template <typename Key, typename Value> concept MoveMapEntry = MapKey<Key> && MapValue<Value> && sup::MoveInsertableKeyValue<Key, Value>;

} // namespace ads::associative

#endif // ASSOCIATIVE_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
