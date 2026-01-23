//===---------------------------------------------------------------------------===//
/**
 * @file Dictionary.hpp
 * @author Costantino Lombardi
 * @brief Abstract interface for a Dictionary (Map) data structure.
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

#include <cstddef>

namespace ads::associative {

/**
 * @brief Interface (abstract base class) that defines common operations
 *        for a dictionary (associative map).
 *
 * @tparam Key The key type.
 * @tparam Value The mapped value type.
 */
template <typename Key, typename Value>
class Dictionary {
public:
  // A virtual destructor is mandatory in polymorphic base classes.
  virtual ~Dictionary() = default;

  /**
   * @brief Inserts or updates a key-value pair (copy).
   * @param key The key to insert.
   * @param value The value to associate with the key.
   */
  virtual auto put(const Key& key, const Value& value) -> void = 0;

  /**
   * @brief Inserts or updates a key-value pair (move).
   * @param key The key to insert.
   * @param value The value to associate with the key.
   */
  virtual auto put(Key&& key, Value&& value) -> void = 0;

  /**
   * @brief Retrieves the value associated with a key.
   * @param key The key to access.
   * @return Reference to the associated value.
   */
  virtual auto get(const Key& key) -> Value& = 0;

  /**
   * @brief Retrieves the value associated with a key (const).
   * @param key The key to access.
   * @return Const reference to the associated value.
   */
  virtual auto get(const Key& key) const -> const Value& = 0;

  /**
   * @brief Checks if a key exists in the dictionary.
   * @param key The key to check.
   * @return true if key exists, false otherwise.
   */
  [[nodiscard]] virtual auto contains(const Key& key) const -> bool = 0;

  /**
   * @brief Removes the element with the given key.
   * @param key The key to remove.
   * @return true if an element was removed, false otherwise.
   */
  virtual auto remove(const Key& key) -> bool = 0;

  /**
   * @brief Returns the number of key-value pairs stored.
   * @return The number of elements.
   */
  [[nodiscard]] virtual auto size() const noexcept -> size_t = 0;
};

} // namespace ads::associative

#endif // DICTIONARY_HPP

//===---------------------------------------------------------------------------===//
