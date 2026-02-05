//===---------------------------------------------------------------------------===//
/**
 * @file Tree_Map.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the TreeMap class - an ordered map based on AVL Tree.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef TREE_MAP_HPP
#define TREE_MAP_HPP

#include "../trees/AVL_Tree.hpp"
#include "Dictionary.hpp"
#include "Tree_Map_Exception.hpp"

#include <concepts>
#include <initializer_list>
#include <optional>
#include <utility>
#include <vector>

namespace ads::associative {

/**
 * @brief An ordered map implemented using an AVL Tree.
 *
 * @details TreeMap stores key-value pairs ordered by key, providing O(log n)
 *          insert, remove, and lookup operations. It is implemented on top of
 *          an AVL Tree to guarantee balanced height.
 *
 * @tparam Key The type of keys (must be comparable with operator<).
 * @tparam Value The type of mapped values.
 */
template <typename Key, typename Value>
class TreeMap : public Dictionary<Key, Value> {
private:
  struct Entry {
    Key                  key;
    std::optional<Value> value = std::nullopt;

    Entry(const Key& k, const Value& v) : key(k), value(v) {}

    Entry(Key&& k, Value&& v) : key(std::move(k)), value(std::move(v)) {}

    explicit Entry(const Key& k) : key(k) {}

    explicit Entry(Key&& k) : key(std::move(k)) {}

    template <typename... Args>
    Entry(std::piecewise_construct_t, Key&& k, Args&&... args) :
        key(std::move(k)),
        value(std::in_place, std::forward<Args>(args)...) {}

    auto operator<=>(const Entry& other) const { return key <=> other.key; }

    auto operator==(const Entry& other) const -> bool { return key == other.key; }
  };

  using TreeType = ads::trees::AVLTree<Entry>;

public:
  using key_type    = Key;
  using mapped_type = Value;
  using value_type  = std::pair<const Key, Value>;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty TreeMap.
   * @complexity Time O(1), Space O(1)
   */
  TreeMap() = default;

  /**
   * @brief Constructs a TreeMap from an initializer list.
   * @param init Initializer list of key-value pairs.
   * @complexity Time O(n log n), Space O(n)
   */
  TreeMap(std::initializer_list<std::pair<Key, Value>> init);

  /**
   * @brief Move constructor.
   * @complexity Time O(1), Space O(1)
   */
  TreeMap(TreeMap&& other) noexcept = default;

  /**
   * @brief Destructor.
   */
  ~TreeMap() override = default;

  /**
   * @brief Move assignment operator.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(TreeMap&& other) noexcept -> TreeMap& = default;

  // Copy constructor and assignment are disabled (move-only type).
  TreeMap(const TreeMap&)                    = delete;
  auto operator=(const TreeMap&) -> TreeMap& = delete;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the map is empty.
   * @return true if empty, false otherwise.
   */
  [[nodiscard]] auto empty() const noexcept -> bool;

  /**
   * @brief Retrieves the value associated with a key.
   * @param key The key to look up.
   * @return Reference to the associated value.
   * @throws KeyNotFoundException if the key does not exist.
   */
  auto get(const Key& key) -> Value& override;

  /**
   * @brief Retrieves the value associated with a key (const version).
   * @param key The key to look up.
   * @return Const reference to the associated value.
   * @throws KeyNotFoundException if the key does not exist.
   */
  auto get(const Key& key) const -> const Value& override;

  /**
   * @brief Returns the number of elements in the map.
   * @return The current size.
   */
  [[nodiscard]] auto size() const noexcept -> size_t override;

  /**
   * @brief Checks if a key exists in the map.
   * @param key The key to search for.
   * @return true if found, false otherwise.
   */
  [[nodiscard]] auto contains(const Key& key) const -> bool override;

  //===---------------------------- ELEMENT ACCESS -----------------------------===//

  /**
   * @brief Access or insert element with given key.
   * @param key The key to access.
   * @return Reference to the associated value.
   * @note Inserts default-constructed value if key doesn't exist.
   */
  auto operator[](const Key& key) -> Value&
    requires std::default_initializable<Value>;

  /**
   * @brief Access element with bounds checking.
   * @param key The key to access.
   * @return Reference to the associated value.
   * @throws KeyNotFoundException if key doesn't exist.
   */
  auto at(const Key& key) -> Value&;

  /**
   * @brief Access element with bounds checking (const version).
   * @param key The key to access.
   * @return Const reference to the associated value.
   * @throws KeyNotFoundException if key doesn't exist.
   */
  auto at(const Key& key) const -> const Value&;

  /**
   * @brief Finds the value pointer for a given key.
   * @param key The key to search for.
   * @return Pointer to value or nullptr if not found.
   */
  [[nodiscard]] auto find(const Key& key) -> Value*;

  /**
   * @brief Finds the value pointer for a given key (const).
   * @param key The key to search for.
   * @return Pointer to value or nullptr if not found.
   */
  [[nodiscard]] auto find(const Key& key) const -> const Value*;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts a key-value pair (copy).
   * @param key The key to insert.
   * @param value The value to associate.
   * @return true if inserted, false if updated.
   */
  auto insert(const Key& key, const Value& value) -> bool;

  /**
   * @brief Inserts a key-value pair (move).
   * @param key The key to insert.
   * @param value The value to associate.
   * @return true if inserted, false if updated.
   */
  auto insert(Key&& key, Value&& value) -> bool;

  /**
   * @brief Constructs a value in-place for the given key.
   * @tparam Args Types of arguments to forward to Value's constructor.
   * @param key The key to insert.
   * @param args Arguments to forward to Value's constructor.
   * @return true if inserted, false if updated.
   */
  template <typename... Args>
  auto emplace(Key key, Args&&... args) -> bool;

  /**
   * @brief Inserts or updates a key-value pair.
   * @param key The key to insert or update.
   * @param value The value to associate with the key.
   */
  auto put(const Key& key, const Value& value) -> void override;

  /**
   * @brief Inserts or updates a key-value pair (move).
   * @param key The key to insert or update.
   * @param value The value to associate with the key.
   */
  auto put(Key&& key, Value&& value) -> void override;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes an element with the given key.
   * @param key The key to remove.
   * @return true if an element was removed, false otherwise.
   */
  auto remove(const Key& key) -> bool override;

  /**
   * @brief Removes an element with the given key.
   * @param key The key to remove.
   * @return true if an element was removed, false otherwise.
   */
  auto erase(const Key& key) -> bool;

  /**
   * @brief Removes all elements.
   */
  auto clear() noexcept -> void;

  //===-------------------------- CONVENIENCE METHODS --------------------------===//

  /**
   * @brief Returns vector of all keys in sorted order.
   */
  [[nodiscard]] auto keys() const -> std::vector<Key>;

  /**
   * @brief Returns vector of all values ordered by key.
   */
  [[nodiscard]] auto values() const -> std::vector<Value>;

  /**
   * @brief Returns vector of all key-value pairs ordered by key.
   */
  [[nodiscard]] auto entries() const -> std::vector<std::pair<Key, Value>>;

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Finds the Entry pointer for a given key.
   * @param key The key to search for.
   * @return Pointer to Entry or nullptr if not found.
   */
  auto find_entry(const Key& key) -> Entry*;

  /**
   * @brief Finds the Entry pointer for a given key (const).
   * @param key The key to search for.
   * @return Pointer to Entry or nullptr if not found.
   */
  auto find_entry(const Key& key) const -> const Entry*;

  TreeType tree_;
};

} // namespace ads::associative

// Include the implementation file for templates.
#include "../../../src/ads/associative/Tree_Map.tpp"

#endif // TREE_MAP_HPP

//===---------------------------------------------------------------------------===//
