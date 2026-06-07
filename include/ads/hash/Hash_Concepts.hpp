//===---------------------------------------------------------------------------===//
/**
 * @file Hash_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Reusable C++20 concepts for hash-based containers.
 * @version 0.1
 * @date 2026-06-07
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef HASH_CONCEPTS_HPP
#define HASH_CONCEPTS_HPP

#include "../../support/Support.hpp"

#include <concepts>
#include <cstddef>

namespace ads::hash {

namespace sup = ads::support;

template <typename Key> concept HashKey = sup::NonReferenceDestructible<Key> && sup::EqualityComparable<Key>;

template <typename Value> concept HashValue = sup::NonReferenceDestructible<Value>;

template <typename Key> concept CopyHashKey = HashKey<Key> && sup::Copyable<Key>;

template <typename Key> concept MoveHashKey = HashKey<Key> && sup::Movable<Key>;

template <typename Hash, typename Key> concept HashFor = requires(Hash hasher, const Key& key) {
  { hasher(key) } -> std::convertible_to<size_t>;
};

template <typename Value> concept CopyHashValue = HashValue<Value> && sup::CopyInsertableValue<Value>;

template <typename Value> concept MoveHashValue = HashValue<Value> && sup::MoveInsertableValue<Value>;

template <typename Value> concept DefaultHashValue = HashValue<Value> && sup::DefaultInitializable<Value>;

template <typename Value, typename... Args> concept EmplaceHashValue = HashValue<Value> && sup::EmplaceAssignableValue<Value, Args...>;

template <typename Key, typename Value> concept CopyHashEntry = CopyHashKey<Key> && CopyHashValue<Value>;

template <typename Key, typename Value> concept CopyKeyMoveHashEntry = CopyHashKey<Key> && MoveHashValue<Value>;

template <typename Key, typename Value> concept MoveHashEntry = MoveHashKey<Key> && MoveHashValue<Value>;

} // namespace ads::hash

#endif // HASH_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
