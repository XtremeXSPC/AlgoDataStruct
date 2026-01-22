//===---------------------------------------------------------------------------===//
/**
 * @file Hash_Map.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the HashMap class - a user-friendly wrapper around
 *        HashTableChaining
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef HASH_MAP_HPP
#define HASH_MAP_HPP

#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../hash/Hash_Table_Chaining.hpp"

namespace ads::associative {

/**
 * @brief A hash map providing a user-friendly interface with iterators.
 *
 * @details HashMap is a wrapper around HashTableChaining that provides:
 *          - Iterator support for range-based for loops.
 *          - API similar to std::unordered_map.
 *          - Convenience methods: keys(), values(), entries().
 *          - operator[] for easy element access/insertion.
 *
 *          All operations have the same complexity as the underlying hash table:
 *          - Insert/Find/Erase: O(1) average, O(n) worst case.
 *          - Iteration: O(n) where n is size.
 *
 * @tparam Key The type of keys (must be hashable).
 * @tparam Value The type of mapped values.
 * @tparam Hash The hash function object (default: std::hash<Key>)
 */
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class HashMap {
public:
  // Type aliases for convenience.
  using key_type    = Key;
  using mapped_type = Value;
  using value_type  = std::pair<const Key, Value>;

  //===---------------------------- ITERATOR CLASS -----------------------------===//

  /**
   * @brief Forward iterator for traversing the hash map.
   */
  class iterator {
  public:
    using Entry             = typename hash::HashTableChaining<Key, Value>::Entry;
    using iterator_category = std::forward_iterator_tag;
    using value_type        = std::pair<const Key, Value>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = Entry*;
    using reference         = Entry&;

    iterator() : map_(nullptr), bucket_idx_(0), list_it_() {}

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> iterator&;
    auto operator++(int) -> iterator;
    auto operator==(const iterator& other) const -> bool;
    auto operator!=(const iterator& other) const -> bool;

  private:
    friend class HashMap;

    using BucketIterator = typename std::list<Entry>::iterator;

    HashMap*       map_;        ///< Pointer to the hash map being iterated.
    size_t         bucket_idx_; ///< Current bucket index.
    BucketIterator list_it_;    ///< Iterator within the current bucket's list.

    /**
     * @brief Construct a new iterator object.
     * @param map Pointer to the hash map.
     * @param bucket_idx Current bucket index.
     * @param list_it Iterator within the bucket's list.
     */
    iterator(HashMap* map, size_t bucket_idx, BucketIterator list_it) : map_(map), bucket_idx_(bucket_idx), list_it_(list_it) {}

    auto advance_to_next_bucket() -> void;
  };

  //===------------------------- CONST_ITERATOR CLASS --------------------------===//

  /**
   * @brief Const forward iterator for traversing the hash map.
   */
  class const_iterator {
  public:
    using Entry             = typename hash::HashTableChaining<Key, Value>::Entry;
    using iterator_category = std::forward_iterator_tag;
    using value_type        = const std::pair<const Key, Value>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const Entry*;
    using reference         = const Entry&;

    const_iterator() : map_(nullptr), bucket_idx_(0), list_it_() {}
    const_iterator(const iterator& it);

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> const_iterator&;
    auto operator++(int) -> const_iterator;
    auto operator==(const const_iterator& other) const -> bool;
    auto operator!=(const const_iterator& other) const -> bool;

  private:
    friend class HashMap;

    using BucketIterator = typename std::list<Entry>::const_iterator;

    const HashMap* map_;
    size_t         bucket_idx_;
    BucketIterator list_it_;

    /**
     * @brief Construct a new const_iterator object.
     * @param map Pointer to the hash map.
     * @param bucket_idx Current bucket index.
     * @param list_it Iterator within the bucket's list.
     */
    const_iterator(const HashMap* map, size_t bucket_idx, BucketIterator list_it) : map_(map), bucket_idx_(bucket_idx), list_it_(list_it) {}

    auto advance_to_next_bucket() -> void;
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty hash map.
   * @param initial_capacity Initial number of buckets.
   * @param max_load_factor Load factor threshold for rehashing.
   */
  explicit HashMap(size_t initial_capacity = 16, float max_load_factor = 0.75f);

  /**
   * @brief Constructs a hash map from an initializer list.
   * @param init Initializer list of key-value pairs.
   */
  HashMap(std::initializer_list<value_type> init);

  /**
   * @brief Move constructor
   */
  HashMap(HashMap&& other) noexcept;

  /**
   * @brief Destructor
   */
  ~HashMap() = default;

  /**
   * @brief Move assignment operator.
   */
  auto operator=(HashMap&& other) noexcept -> HashMap&;

  // Copy constructor and assignment are disabled (move-only type).
  HashMap(const HashMap&)                    = delete;
  auto operator=(const HashMap&) -> HashMap& = delete;

  //===---------------------------- ELEMENT ACCESS -----------------------------===//

  /**
   * @brief Access or insert element with given key.
   * @param key The key to access.
   * @return Reference to the value associated with key.
   * @note If key doesn't exist, inserts it with default-constructed value.
   */
  auto operator[](const Key& key) -> Value&;

  /**
   * @brief Access element with bounds checking.
   * @param key The key to access.
   * @return Reference to the value associated with key.
   * @throws std::out_of_range if key doesn't exist.
   */
  auto at(const Key& key) -> Value&;

  /**
   * @brief Access element with bounds checking (const version).
   * @param key The key to access.
   * @return Const reference to the value associated with key.
   * @throws std::out_of_range if key doesn't exist.
   */
  auto at(const Key& key) const -> const Value&;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts a key-value pair.
   * @param pair The key-value pair to insert.
   * @return Pair of iterator to element and bool indicating insertion.
   */
  auto insert(const value_type& pair) -> std::pair<iterator, bool>;

  /**
   * @brief Inserts a key-value pair (move).
   * @param pair The r-value key-value pair to insert.
   * @return Pair of iterator to element and bool indicating insertion.
   */
  auto insert(value_type&& pair) -> std::pair<iterator, bool>;

  /**
   * @brief Constructs element in-place.
   * @param args Arguments to forward to the constructor.
   * @return Pair of iterator to element and bool indicating insertion.
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> std::pair<iterator, bool>;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Erases element with given key.
   * @param key The key to erase.
   * @return Number of elements erased (0 or 1).
   */
  auto erase(const Key& key) -> size_t;

  /**
   * @brief Erases element at iterator position.
   * @param pos Iterator to element to erase.
   * @return Iterator to next element.
   */
  auto erase(iterator pos) -> iterator;

  /**
   * @brief Removes all elements.
   */
  auto clear() noexcept -> void;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  [[nodiscard]] auto empty() const noexcept -> bool;

  [[nodiscard]] auto size() const noexcept -> size_t;

  [[nodiscard]] auto load_factor() const noexcept -> float;

  /**
   * @brief Finds element with given key.
   * @param key The key to search for.
   * @return Iterator to element or end() if not found.
   */
  [[nodiscard]]
  auto find(const Key& key) -> iterator;

  /**
   * @brief Finds element with given key (const version).
   * @param key The key to search for.
   * @return Const iterator to element or end() if not found.
   */
  [[nodiscard]]
  auto find(const Key& key) const -> const_iterator;

  /**
   * @brief Checks if key exists.
   * @param key The key to check.
   * @return true if key exists.
   */
  [[nodiscard]]
  auto contains(const Key& key) const -> bool;

  /**
   * @brief Counts elements with given key.
   * @param key The key to count.
   * @return 0 or 1 (no duplicates allowed).
   */
  [[nodiscard]]
  auto count(const Key& key) const -> size_t;

  //===-------------------------- CONVENIENCE METHODS --------------------------===//

  /**
   * @brief Returns vector of all keys.
   */
  [[nodiscard]] auto keys() const -> std::vector<Key>;

  /**
   * @brief Returns vector of all values.
   */
  [[nodiscard]] auto values() const -> std::vector<Value>;

  /**
   * @brief Returns vector of all key-value pairs.
   */
  [[nodiscard]] auto entries() const -> std::vector<std::pair<Key, Value>>;

  //===-------------------------- ITERATOR OPERATIONS --------------------------===//

  auto begin() -> iterator;

  auto begin() const -> const_iterator;

  auto end() -> iterator;

  auto end() const -> const_iterator;

  auto cbegin() const -> const_iterator;

  auto cend() const -> const_iterator;

private:
  //===============================================================================//
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  hash::HashTableChaining<Key, Value> table_; ///< Underlying hash table.

  auto find_in_table(const Key& key) -> std::pair<size_t, typename std::list<std::pair<Key, Value>>::iterator>;

  auto find_in_table(const Key& key) const -> std::pair<size_t, typename std::list<std::pair<Key, Value>>::const_iterator>;

  // Access to internal table structure (for iterators).
  friend class iterator;
  friend class const_iterator;
};

} // namespace ads::associative

// Include the implementation file for templates.
#include "../../../src/ads/associative/Hash_Map.tpp"

#endif // HASH_MAP_HPP
//===--------------------------------------------------------------------------===//
