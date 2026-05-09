//===---------------------------------------------------------------------------===//
/**
 * @file Dictionary.hpp
 * @author Costantino Lombardi
 * @brief Compile-time contract for dictionary (map) data structures.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <concepts>
#include <cstddef>
#include <utility>

namespace ads::associative {

/**
 * @brief Concept for move-capable dictionary (associative map) types.
 *
 * @details The contract is intentionally compile-time based: containers expose
 *          only operations their key and mapped types can actually support.
 *
 * @tparam Map The dictionary implementation type.
 * @tparam Key The key type.
 * @tparam Value The mapped value type.
 */
template <typename Map, typename Key, typename Value>
concept Dictionary =
    requires {
      typename Map::key_type;
      typename Map::mapped_type;
    } && std::same_as<typename Map::key_type, Key> && std::same_as<typename Map::mapped_type, Value>
    && requires(
        Map& dictionary, const Map& const_dictionary, const Key& key, Key&& movable_key, Value&& movable_value) {
         { dictionary.put(std::move(movable_key), std::move(movable_value)) } -> std::same_as<void>;
         { dictionary.get(key) } -> std::same_as<Value&>;
         { const_dictionary.get(key) } -> std::same_as<const Value&>;
         { const_dictionary.contains(key) } -> std::same_as<bool>;
         { dictionary.remove(key) } -> std::same_as<bool>;
         { const_dictionary.size() } noexcept -> std::same_as<size_t>;
       };

/**
 * @brief Concept for dictionaries that also support copy-based put.
 *
 * @tparam Map The dictionary implementation type.
 * @tparam Key The key type.
 * @tparam Value The mapped value type.
 */
template <typename Map, typename Key, typename Value>
concept CopyPutDictionary =
    Dictionary<Map, Key, Value> && std::copy_constructible<Key> && std::copy_constructible<Value>
    && std::assignable_from<Value&, const Value&> && requires(Map& dictionary, const Key& key, const Value& value) {
         { dictionary.put(key, value) } -> std::same_as<void>;
       };

} // namespace ads::associative

#endif // DICTIONARY_HPP

//===---------------------------------------------------------------------------===//
