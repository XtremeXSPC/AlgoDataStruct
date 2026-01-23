//===---------------------------------------------------------------------------===//
/**
 * @file Hash_Table_Open_Addressing.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the Hash Table with Open Addressing collision resolution.
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef HASH_TABLE_OPEN_ADDRESSING_HPP
#define HASH_TABLE_OPEN_ADDRESSING_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <utility>

#include "Hash_Table_Exception.hpp"

namespace ads::hash {

/**
 * @brief Probing strategy for open addressing.
 */
enum class ProbingStrategy : std::uint8_t { LINEAR, QUADRATIC, DOUBLE_HASH };

/**
 * @brief Hash table implementation using open addressing for collision resolution.
 *
 * @details This class implements a hash table that stores all entries directly
 *          in an array. When a collision occurs, it probes for the next available
 *          slot using one of three strategies:
 *          - Linear probing: h(k) + i
 *          - Quadratic probing: h(k) + i^2
 *          - Double hashing: h1(k) + i * h2(k)
 *
 *          Features:
 *          - O(1) average time for insert, find, and erase (with good load factor).
 *          - Better cache locality than chaining.
 *          - Tombstone markers for proper deletion.
 *          - Dynamic resizing when load factor threshold is reached.
 *
 *          The load factor must be kept below 1.0 for proper operation.
 *          Recommended max load factor: 0.5-0.7 for good performance.
 *
 * @tparam Key The type of keys. Must be hashable with std::hash.
 * @tparam Value The type of values to store.
 */
template <typename Key, typename Value>
class HashTableOpenAddressing {
public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs a hash table with specified capacity, load factor,
   *        and probing strategy.
   * @param initial_capacity Initial number of slots (default: 16).
   * @param max_load_factor Maximum load factor before rehashing (default: 0.5).
   * @param strategy Probing strategy to use (default: LINEAR).
   * @complexity Time O(n) to allocate slots, Space O(n)
   * @throws InvalidOperationException if max_load_factor is not in (0, 1).
   */
  explicit HashTableOpenAddressing(
      size_t          initial_capacity = kInitialCapacity,
      ProbingStrategy strategy         = ProbingStrategy::LINEAR,
      float           max_load_factor  = kDefaultMaxLoadFactor);

  /**
   * @brief Move constructor.
   * @param other The hash table from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  HashTableOpenAddressing(HashTableOpenAddressing&& other) noexcept;

  /**
   * @brief Destructor. Clears all entries and deallocates storage.
   * @complexity Time O(n), Space O(1)
   */
  ~HashTableOpenAddressing() = default;

  /**
   * @brief Move assignment operator.
   * @param other The hash table from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(HashTableOpenAddressing&& other) noexcept -> HashTableOpenAddressing&;

  // Copy constructor and assignment are disabled (move-only type).
  HashTableOpenAddressing(const HashTableOpenAddressing&)                    = delete;
  auto operator=(const HashTableOpenAddressing&) -> HashTableOpenAddressing& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts or updates a key-value pair.
   * @param key The key to insert.
   * @param value The value to associate with the key.
   * @return true if inserted, false if updated existing key.
   * @details If the key already exists, its value is updated.
   * @complexity Time O(1) average, O(n) worst case.
   */
  auto insert(const Key& key, const Value& value) -> bool;

  /**
   * @brief Inserts or updates a key-value pair (move version).
   * @param key The key to insert.
   * @param value The value to move into the table.
   * @return true if inserted, false if updated existing key.
   * @complexity Time O(1) average, O(n) worst case.
   */
  auto insert(Key&& key, Value&& value) -> bool;

  /**
   * @brief Constructs a value in-place for the given key.
   * @tparam Args Types of arguments to forward to the constructor of Value.
   * @param key The key to insert.
   * @param args Arguments to forward to the constructor of Value.
   * @return Reference to the inserted or updated value.
   * @complexity Time O(1) average, O(n) worst case.
   */
  template <typename... Args>
  auto emplace(const Key& key, Args&&... args) -> Value&;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Accesses value by key with bounds checking.
   * @param key The key to look up.
   * @return Reference to the value associated with the key.
   * @throws KeyNotFoundException if the key doesn't exist.
   * @complexity Time O(1) average, O(n) worst case.
   */
  auto at(const Key& key) -> Value&;

  /**
   * @brief Accesses value by key with bounds checking (const version).
   * @param key The key to look up.
   * @return Const reference to the value associated with the key.
   * @throws KeyNotFoundException if the key doesn't exist.
   * @complexity Time O(1) average, O(n) worst case.
   */
  auto at(const Key& key) const -> const Value&;

  /**
   * @brief Accesses value by key, inserting default if not found.
   * @param key The key to look up.
   * @return Reference to the value associated with the key.
   * @details If the key doesn't exist, inserts default-constructed value.
   * @complexity Time O(1) average, O(n) worst case.
   */
  auto operator[](const Key& key) -> Value&;

  //===--------------------------- SEARCH OPERATIONS ---------------------------===//

  /**
   * @brief Checks if a key exists in the table.
   * @param key The key to search for.
   * @return true if the key exists, false otherwise.
   * @complexity Time O(1) average, O(n) worst case.
   */
  [[nodiscard]] auto contains(const Key& key) const -> bool;

  /**
   * @brief Finds a value by key.
   * @param key The key to search for.
   * @return Pointer to the value if found, nullptr otherwise.
   * @complexity Time O(1) average, O(n) worst case.
   */
  [[nodiscard]] auto find(const Key& key) -> Value*;

  /**
   * @brief Finds a value by key (const version).
   * @param key The key to search for.
   * @return Const pointer to the value if found, nullptr otherwise.
   * @complexity Time O(1) average, O(n) worst case.
   */
  [[nodiscard]] auto find(const Key& key) const -> const Value*;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes a key-value pair from the table.
   * @param key The key to remove.
   * @return true if the key was found and removed, false otherwise.
   * @details Uses tombstone marker to maintain probe chains.
   * @complexity Time O(1) average, O(n) worst case.
   */
  auto erase(const Key& key) -> bool;

  /**
   * @brief Removes all entries from the table.
   * @complexity Time O(n), Space O(1)
   */
  void clear() noexcept;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Returns the number of active entries in the table.
   * @return Number of key-value pairs (excludes tombstones).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the number of slots in the table.
   * @return Capacity of the hash table.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto capacity() const noexcept -> size_t;

  /**
   * @brief Checks if the table is empty.
   * @return true if size is 0, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the current load factor (size/capacity).
   * @return size / capacity ratio.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto load_factor() const noexcept -> float;

  /**
   * @brief Returns the maximum load factor threshold.
   * @return Maximum allowed load factor before rehashing.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto max_load_factor() const noexcept -> float;

  /**
   * @brief Returns the current probing strategy.
   * @return Probing strategy enum value.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto probing_strategy() const noexcept -> ProbingStrategy;

  //===----------------------- CONFIGURATION OPERATIONS ------------------------===//

  /**
   * @brief Reserves space for at least new_capacity slots.
   * @param new_capacity Minimum capacity to reserve.
   * @details Forces a rehash if new_capacity > current capacity.
   * @complexity Time O(n) if rehashing occurs, Space O(n)
   */
  void reserve(size_t new_capacity);

  /**
   * @brief Sets the maximum load factor.
   * @param mlf New maximum load factor (must be > 0 and < 1).
   * @throws InvalidOperationException if mlf is invalid.
   * @complexity Time O(n) in the worst case due to rehashing.
   */
  void set_max_load_factor(float mlf);

private:
  //===-------------------------- INTERNAL STRUCTURES --------------------------===//

  /**
   * @brief State of a slot in the hash table.
   */
  enum class SlotState : std::uint8_t { EMPTY, OCCUPIED, DELETED };

  /**
   * @brief Entry in the hash table.
   */
  struct Entry {
    Key   key;
    Value value;

    Entry() = default;
    Entry(const Key& k, const Value& v) : key(k), value(v) {}
    Entry(Key&& k, Value&& v) : key(std::move(k)), value(std::move(v)) {}

    template <typename... Args>
    explicit Entry(const Key& k, Args&&... args) : key(k), value(std::forward<Args>(args)...) {}
  };

  /**
   * @brief A slot can be empty, occupied, or deleted (tombstone).
   */
  struct Slot {
    std::optional<Entry> entry;
    SlotState            state;

    Slot() : entry(std::nullopt), state(SlotState::EMPTY) {}
  };

  //===============================================================================//
  //===------------------------ PRIVATE HASHING METHODS ------------------------===//

  /**
   * @brief Primary hash function.
   * @param key The key to hash.
   * @return Hash value modulo capacity.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto hash1(const Key& key) const -> size_t;

  /**
   * @brief Secondary hash function for double hashing.
   * @param key The key to hash.
   * @return Secondary hash value (must be coprime with capacity).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto hash2(const Key& key) const -> size_t;

  //===---------------------------- PROBING METHODS ----------------------------===//
  /**
   * @brief Computes probe sequence index.
   * @param key The key being probed.
   * @param i Probe attempt number (0, 1, 2, ...).
   * @return Slot index for this probe.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto probe(const Key& key, size_t i) const -> size_t;

  //===------------------------- SLOT FINDING METHODS --------------------------===//
  /**
   * @brief Finds a slot for a given key.
   * @param key The key to search for.
   * @return Pointer to slot if found, nullptr otherwise.
   * @complexity Time O(1) average, O(n) worst case.
   */
  auto find_slot(const Key& key) -> Slot*;

  /**
   * @brief Finds a slot for a given key (const version).
   * @param key The key to search for.
   * @return Const pointer to slot if found, nullptr otherwise.
   * @complexity Time O(1) average, O(n) worst case.
   */
  auto find_slot(const Key& key) const -> const Slot*;

  /**
   * @brief Finds first empty or deleted slot for insertion.
   * @param key The key to insert.
   * @return Pointer to available slot.
   * @complexity Time O(1) average, O(n) worst case.
   */
  auto find_insert_slot(const Key& key) -> Slot*;

  //===------------------------- REHASHING OPERATIONS --------------------------===//

  /**
   * @brief Rehashes the table to a new capacity.
   * @param new_capacity The new number of slots.
   * @details Tombstones are not copied during rehashing.
   * @complexity Time O(n), Space O(n)
   */
  void rehash(size_t new_capacity);

  /**
   * @brief Checks if rehashing is needed and performs it.
   * @complexity Time O(n) in the worst case due to rehashing.
   */
  void check_and_rehash();

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<Slot[]> table_;           ///< Array of slots.
  size_t                  capacity_;        ///< Number of slots.
  size_t                  size_;            ///< Number of occupied slots.
  float                   max_load_factor_; ///< Threshold for rehashing.
  ProbingStrategy         strategy_;        ///< Probing strategy.

  static constexpr size_t kInitialCapacity      = 16;
  static constexpr float  kDefaultMaxLoadFactor = 0.5f;
  static constexpr size_t kGrowthFactor         = 2;
};

} // namespace ads::hash

// Include the implementation file for templates.
#include "../../../src/ads/hash/Hash_Table_Open_Addressing.tpp"

#endif // HASH_TABLE_OPEN_ADDRESSING_HPP

//===---------------------------------------------------------------------------===//
