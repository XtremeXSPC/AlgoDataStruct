//===---------------------------------------------------------------------------===//
/**
 * @file Hash_Table_Chaining.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the Hash Table with Chaining collision resolution.
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef HASH_TABLE_CHAINING_HPP
#define HASH_TABLE_CHAINING_HPP

#include "Hash_Table_Exception.hpp"

#include <cstddef>
#include <functional>
#include <list>
#include <memory>
#include <utility>

// Forward declaration for HashMap friend
namespace ads::associative {
template <typename, typename, typename>
class HashMap;
}

namespace ads::hash {

/**
 * @brief Hash table implementation using chaining for collision resolution.
 *
 * @details This class implements a hash table that uses separate chaining
 *          (linked lists) to handle collisions. When multiple keys hash to
 *          the same index, they are stored in a list at that bucket.
 *
 *          Features:
 *          - O(1) average time for insert, find, and erase
 *          - Dynamic resizing with configurable load factor
 *          - Uses std::hash for key hashing
 *          - Move semantics support
 *
 *          The load factor (size/capacity) determines when to rehash.
 *          When it exceeds max_load_factor, the table doubles in size
 *          and all entries are rehashed.
 *
 * @tparam Key The type of keys. Must be hashable with std::hash.
 * @tparam Value The type of values to store.
 */
template <typename Key, typename Value>
class HashTableChaining {
public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs a hash table with specified capacity and load factor.
   * @param initial_capacity Initial number of buckets (default: 16).
   * @param max_load_factor Maximum load factor before rehashing (default: 0.75).
   * @complexity Time O(n) to allocate buckets, Space O(n)
   * @throws InvalidOperationException if max_load_factor <= 0.
   */
  explicit HashTableChaining(size_t initial_capacity = kInitialCapacity, float max_load_factor = kDefaultMaxLoadFactor);

  /**
   * @brief Move constructor.
   * @param other The hash table from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  HashTableChaining(HashTableChaining&& other) noexcept;

  /**
   * @brief Destructor. Clears all entries and deallocates buckets.
   * @complexity Time O(n), Space O(1)
   */
  ~HashTableChaining() = default;

  /**
   * @brief Move assignment operator.
   * @param other The hash table from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(1), Space O(1)
   */
  HashTableChaining& operator=(HashTableChaining&& other) noexcept;

  // Copy constructor and assignment are disabled (move-only type).
  HashTableChaining(const HashTableChaining&)            = delete;
  HashTableChaining& operator=(const HashTableChaining&) = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts or updates a key-value pair.
   * @param key The key to insert.
   * @param value The value to associate with the key.
   * @details If the key already exists, its value is updated.
   * @complexity Time O(1) average, O(n) worst case.
   */
  void insert(const Key& key, const Value& value);

  /**
   * @brief Inserts or updates a key-value pair (move version).
   * @param key The key to insert.
   * @param value The value to move into the table.
   * @complexity Time O(1) average, O(n) worst case.
   */
  void insert(Key&& key, Value&& value);

  /**
   * @brief Constructs a value in-place for the given key.
   * @tparam Args Types of arguments to forward to the constructor of Value.
   * @param key The key to insert.
   * @param args Arguments to forward to the constructor of Value.
   * @return Reference to the inserted or updated value.
   * @complexity Time O(1) average, O(n) worst case.
   */
  template <typename... Args>
  Value& emplace(const Key& key, Args&&... args);

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Accesses value by key with bounds checking.
   * @param key The key to look up.
   * @return Reference to the value associated with the key.
   * @throws KeyNotFoundException if the key doesn't exist.
   * @complexity Time O(1) average, O(n) worst case.
   */
  Value& at(const Key& key);

  /**
   * @brief Accesses value by key with bounds checking (const version).
   * @param key The key to look up.
   * @return Const reference to the value associated with the key.
   * @throws KeyNotFoundException if the key doesn't exist.
   * @complexity Time O(1) average, O(n) worst case.
   */
  const Value& at(const Key& key) const;

  /**
   * @brief Accesses value by key, inserting default if not found.
   * @param key The key to look up.
   * @return Reference to the value associated with the key.
   * @details If the key doesn't exist, inserts default-constructed value.
   * @complexity Time O(1) average, O(n) worst case.
   */
  Value& operator[](const Key& key);

  //===--------------------------- SEARCH OPERATIONS ---------------------------===//

  /**
   * @brief Checks if a key exists in the table.
   * @param key The key to search for.
   * @return true if the key exists, false otherwise.
   * @complexity Time O(1) average, O(n) worst case.
   */
  [[nodiscard]] bool contains(const Key& key) const;

  /**
   * @brief Finds a value by key.
   * @param key The key to search for.
   * @return Pointer to the value if found, nullptr otherwise.
   * @complexity Time O(1) average, O(n) worst case.
   */
  [[nodiscard]] Value* find(const Key& key);

  /**
   * @brief Finds a value by key (const version).
   * @param key The key to search for.
   * @return Const pointer to the value if found, nullptr otherwise.
   * @complexity Time O(1) average, O(n) worst case.
   */
  [[nodiscard]] const Value* find(const Key& key) const;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes a key-value pair from the table.
   * @param key The key to remove.
   * @return true if the key was found and removed, false otherwise.
   * @complexity Time O(1) average, O(n) worst case.
   */
  bool erase(const Key& key);

  /**
   * @brief Removes all entries from the table.
   * @complexity Time O(n), Space O(1)
   */
  void clear() noexcept;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Returns the number of entries in the table.
   * @return Number of key-value pairs.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] size_t size() const noexcept;

  /**
   * @brief Returns the number of buckets.
   * @return Capacity of the hash table.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] size_t capacity() const noexcept;

  /**
   * @brief Checks if the table is empty.
   * @return true if size is 0, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] bool is_empty() const noexcept;

  /**
   * @brief Returns the current load factor.
   * @return size / capacity ratio.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] float load_factor() const noexcept;

  /**
   * @brief Returns the maximum load factor threshold.
   * @return Maximum allowed load factor before rehashing.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] float max_load_factor() const noexcept;

  //===----------------------- CONFIGURATION OPERATIONS ------------------------===//

  /**
   * @brief Sets the maximum load factor.
   * @param mlf New maximum load factor (must be positive).
   * @throws InvalidOperationException if mlf <= 0.
   * @complexity Time O(n) in the worst case due to rehashing.
   */
  void set_max_load_factor(float mlf);

  /**
   * @brief Reserves space for at least new_capacity buckets.
   * @param new_capacity Minimum capacity to reserve.
   * @details Forces a rehash if new_capacity > current capacity.
   * @complexity Time O(n) if rehashing occurs, Space O(n)
   */
  void reserve(size_t new_capacity);

private:
  //===-------------------------- INTERNAL STRUCTURES --------------------------===//

  // Friend declaration for HashMap to access internal structure.
  template <typename, typename, typename>
  friend class ::ads::associative::HashMap;

  /**
   * @brief Entry is std::pair for structured bindings support.
   */
  using Entry = std::pair<const Key, Value>;

  /**
   * @brief A bucket is a list of entries.
   */
  using Bucket = std::list<Entry>;

  //===============================================================================//
  //===------------------------ PRIVATE HASHING METHODS ------------------------===//

  /**
   * @brief Computes the hash of a key.
   * @param key The key to hash.
   * @return Bucket index for the key.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] size_t hash(const Key& key) const;

  //===------------------------- BUCKET SEARCH METHODS -------------------------===//

  /**
   * @brief Finds an entry in a bucket.
   * @param bucket The bucket to search.
   * @param key The key to find.
   * @return Iterator to the entry, or bucket.end() if not found.
   * @complexity Time O(k) where k is bucket size.
   */
  auto find_in_bucket(Bucket& bucket, const Key& key) -> typename Bucket::iterator;

  /**
   * @brief Finds an entry in a bucket (const version).
   * @param bucket The bucket to search.
   * @param key The key to find.
   * @return Const iterator to the entry, or bucket.end() if not found.
   * @complexity Time O(k) where k is bucket size.
   */
  auto find_in_bucket(const Bucket& bucket, const Key& key) const -> typename Bucket::const_iterator;

  //===------------------------- REHASHING OPERATIONS --------------------------===//

  /**
   * @brief Rehashes the table to a new capacity.
   * @param new_capacity The new number of buckets.
   * @complexity Time O(n), Space O(n)
   */
  void rehash(size_t new_capacity);

  /**
   * @brief Checks if rehashing is needed and performs it.
   * @complexity Time O(n) in the worst case due to rehashing.
   */
  void check_and_rehash();

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<Bucket[]> buckets_;         ///< Array of buckets.
  size_t                    capacity_;        ///< Number of buckets.
  size_t                    size_;            ///< Number of entries.
  float                     max_load_factor_; ///< Threshold for rehashing.

  static constexpr size_t kInitialCapacity      = 16;
  static constexpr float  kDefaultMaxLoadFactor = 0.75f;
  static constexpr size_t kGrowthFactor         = 2;
};

} // namespace ads::hash

// Include the implementation file for templates.
#include "../../../src/ads/hash/Hash_Table_Chaining.tpp"

#endif // HASH_TABLE_CHAINING_HPP

//===---------------------------------------------------------------------------===//
