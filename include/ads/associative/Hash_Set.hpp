//===---------------------------------------------------------------------------===//
/**
 * @file Hash_Set.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the HashSet class - an unordered set based on hash table.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef HASH_SET_HPP
#define HASH_SET_HPP

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <list>
#include <vector>

namespace ads::associative {

/**
 * @brief An unordered set implemented using a hash table with chaining.
 *
 * @details HashSet stores unique elements with O(1) average time for
 *          insert, remove, and lookup operations. It uses separate chaining
 *          for collision resolution and automatically rehashes when the
 *          load factor exceeds the threshold.
 *
 * @tparam T The type of elements (must be hashable).
 * @tparam Hash The hash function object (default: std::hash<T>).
 */
template <typename T, typename Hash = std::hash<T>>
class HashSet {
public:
  using value_type = T;
  using size_type  = size_t;

  //===---------------------------- ITERATOR CLASS -----------------------------===//

  /**
   * @brief Forward iterator for HashSet.
   */
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    iterator() = default;

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> iterator&;
    auto operator++(int) -> iterator;
    auto operator==(const iterator& other) const -> bool;
    auto operator!=(const iterator& other) const -> bool;

  private:
    friend class HashSet<T, Hash>;

    using BucketType     = std::list<T>;
    using BucketIterator = typename BucketType::const_iterator;

    const HashSet<T, Hash>* set_        = nullptr;
    size_t                  bucket_idx_ = 0;
    BucketIterator          list_it_;

    iterator(const HashSet<T, Hash>* set, size_t bucket_idx, BucketIterator list_it) :
        set_(set), bucket_idx_(bucket_idx), list_it_(list_it) {}

    auto advance_to_next_bucket() -> void;
  };

  using const_iterator = iterator;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty hash set.
   * @param initial_capacity Initial number of buckets (default: 16).
   * @param max_load_factor Load factor threshold for rehashing (default: 0.75).
   * @complexity Time O(n), Space O(n)
   */
  explicit HashSet(size_t initial_capacity = 16, double max_load_factor = 0.75);

  /**
   * @brief Constructs a hash set from an initializer list.
   * @param values Elements to insert.
   * @complexity Time O(n) average, Space O(n)
   */
  HashSet(std::initializer_list<T> values);

  /**
   * @brief Destructor.
   */
  ~HashSet() = default;

  /**
   * @brief Move constructor.
   * @complexity Time O(1), Space O(1)
   */
  HashSet(HashSet&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(HashSet&& other) noexcept -> HashSet&;

  // Copy operations disabled.
  HashSet(const HashSet&)                    = delete;
  auto operator=(const HashSet&) -> HashSet& = delete;

  //===------------------------ MODIFICATION OPERATIONS ------------------------===//

  /**
   * @brief Inserts an element into the set.
   * @param value The value to insert.
   * @return true if inserted, false if already exists.
   * @complexity Time O(1) average, Space O(1)
   */
  auto insert(const T& value) -> bool;

  /**
   * @brief Inserts an element into the set (move).
   * @param value The r-value to move.
   * @return true if inserted, false if already exists.
   * @complexity Time O(1) average, Space O(1)
   */
  auto insert(T&& value) -> bool;

  /**
   * @brief Constructs an element in-place.
   * @tparam Args Types of arguments to forward.
   * @param args Arguments to forward to T's constructor.
   * @return true if inserted, false if already exists.
   * @complexity Time O(1) average, Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> bool;

  /**
   * @brief Removes an element from the set.
   * @param value The value to remove.
   * @return true if removed, false if not found.
   * @complexity Time O(1) average, Space O(1)
   */
  auto erase(const T& value) -> bool;

  /**
   * @brief Removes all elements from the set.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if an element exists in the set.
   * @param value The value to search for.
   * @return true if found, false otherwise.
   * @complexity Time O(1) average, Space O(1)
   */
  [[nodiscard]] auto contains(const T& value) const -> bool;

  /**
   * @brief Checks if the set is empty.
   * @return true if empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the set.
   * @return The current size.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the number of buckets.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto bucket_count() const noexcept -> size_t;

  /**
   * @brief Returns the current load factor.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto load_factor() const noexcept -> double;

  //===-------------------------- ITERATOR OPERATIONS --------------------------===//

  /**
   * @brief Returns an iterator to the beginning.
   * @return Iterator to the first element.
   */
  auto begin() const -> iterator;

  /**
   * @brief Returns an iterator to the end.
   * @return Iterator to one past the last element.
   */
  auto end() const -> iterator;

  /**
   * @brief Returns a const iterator to the beginning.
   * @return Const iterator to the first element.
   */
  auto cbegin() const -> const_iterator;

  /**
   * @brief Returns a const iterator to the end.
   * @return Const iterator to one past the last element.
   */
  auto cend() const -> const_iterator;

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Computes the bucket index for a given value.
   * @param value The value to hash.
   * @return The bucket index.
   */
  auto bucket_index(const T& value) const -> size_t;

  /**
   * @brief Rehashes the table to a new bucket count.
   * @param new_bucket_count The new number of buckets.
   */
  auto rehash(size_t new_bucket_count) -> void;

  /**
   * @brief Checks and maintains the load factor by rehashing if necessary.
   */
  auto check_load_factor() -> void;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::vector<std::list<T>> buckets_;         ///< Buckets for separate chaining.
  size_t                    size_;            ///< Number of elements in the set.
  double                    max_load_factor_; ///< Load factor threshold.
  Hash                      hasher_;          ///< Hash function object.
};

} // namespace ads::associative

// Include the implementation file for templates.
#include "../../../src/ads/associative/Hash_Set.tpp"

#endif // HASH_SET_HPP

//===---------------------------------------------------------------------------===//
