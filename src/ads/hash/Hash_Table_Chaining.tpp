//===---------------------------------------------------------------------------===//
/**
 * @file Hash_Table_Chaining.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the Hash Table with Chaining.
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/hash/Hash_Table_Chaining.hpp"

#include <algorithm>

namespace ads::hash {

//===----- CONSTRUCTORS AND ASSIGNMENT -----------------------------------------===//

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
HashTableChaining<Key, Value, Hash>::HashTableChaining(size_type initial_capacity, float max_load_factor, Hash hasher) :
    buckets_(),
    capacity_(std::max<size_t>(initial_capacity, 1)),
    size_(0),
    max_load_factor_(max_load_factor),
    hasher_(std::move(hasher)) {
  if (max_load_factor <= 0.0f) {
    throw InvalidOperationException("Max load factor must be positive");
  }
  buckets_.resize(capacity_);
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
HashTableChaining<Key, Value, Hash>::HashTableChaining(HashTableChaining&& other) noexcept :
    buckets_(std::move(other.buckets_)),
    capacity_(other.capacity_),
    size_(other.size_),
    max_load_factor_(other.max_load_factor_),
    hasher_(std::move(other.hasher_)) {
  other.capacity_        = 0;
  other.size_            = 0;
  other.max_load_factor_ = kDefaultMaxLoadFactor;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::operator=(HashTableChaining&& other) noexcept -> HashTableChaining<Key, Value, Hash>& {
  if (this != &other) {
    buckets_               = std::move(other.buckets_);
    capacity_              = other.capacity_;
    size_                  = other.size_;
    max_load_factor_       = other.max_load_factor_;
    hasher_                = std::move(other.hasher_);
    other.capacity_        = 0;
    other.size_            = 0;
    other.max_load_factor_ = kDefaultMaxLoadFactor;
  }
  return *this;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
HashTableChaining<Key, Value, Hash>::HashTableChaining(
    std::initializer_list<std::pair<Key, Value>> entries, float max_load_factor, Hash hasher) requires CopyHashEntry<Key, Value>
    : HashTableChaining(kInitialCapacity, max_load_factor, std::move(hasher)) {
  for (const auto& [key, value] : entries) {
    insert(key, value);
  }
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
template <std::input_iterator InputIt>
HashTableChaining<Key, Value, Hash>::HashTableChaining(InputIt first, InputIt last, float max_load_factor, Hash hasher)
    requires CopyHashEntry<Key, Value>
    : HashTableChaining(kInitialCapacity, max_load_factor, std::move(hasher)) {
  for (auto it = first; it != last; ++it) {
    insert(it->first, it->second);
  }
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::insert(const Key& key, const Value& value) -> bool requires CopyHashEntry<Key, Value>
{
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    // Existing keys are updated before any resize so references stay valid.
    it->second = value;
    return false;
  }

  ensure_capacity_for_insert();
  bucket_idx = hash(key);
  buckets_[bucket_idx].emplace_back(key, value);
  ++size_;
  return true;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::insert(const Key& key, Value&& value) -> bool requires CopyKeyMoveHashEntry<Key, Value>
{
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    // The key is preserved while the mapped value adopts the caller's resource.
    it->second = std::move(value);
    return false;
  }

  ensure_capacity_for_insert();
  bucket_idx = hash(key);
  buckets_[bucket_idx].emplace_back(key, std::move(value));
  ++size_;
  return true;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::insert(Key&& key, Value&& value) -> bool requires MoveHashEntry<Key, Value>
{
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    // Keep the stored key stable; only the mapped value changes on duplicate insert.
    it->second = std::move(value);
    return false;
  }

  ensure_capacity_for_insert();
  bucket_idx = hash(key);
  buckets_[bucket_idx].emplace_back(std::move(key), std::move(value));
  ++size_;
  return true;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
template <typename... Args>
auto HashTableChaining<Key, Value, Hash>::emplace(const Key& key, Args&&... args)
    -> Value& requires CopyHashKey<Key> && EmplaceHashValue<Value, Args...>
{
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    // Construct first, then assign, so failed construction leaves the old value intact.
    it->second = Value(std::forward<Args>(args)...);
    return it->second;
  }

  ensure_capacity_for_insert();
  bucket_idx  = hash(key);
  auto& entry = buckets_[bucket_idx].emplace_back(key, std::forward<Args>(args)...);
  ++size_;
  return entry.second;
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::at(const Key& key) -> Value& {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it == buckets_[bucket_idx].end()) {
    throw KeyNotFoundException("Key not found in hash table");
  }

  return it->second;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::at(const Key& key) const -> const Value& {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it == buckets_[bucket_idx].end()) {
    throw KeyNotFoundException("Key not found in hash table");
  }

  return it->second;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::operator[](const Key& key) -> Value& requires CopyHashKey<Key> && DefaultHashValue<Value>
{
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    return it->second;
  }

  // Grow before insertion so the returned reference is never invalidated.
  ensure_capacity_for_insert();
  bucket_idx = hash(key);
  buckets_[bucket_idx].emplace_back(key, Value{});
  ++size_;
  it = find_in_bucket(buckets_[bucket_idx], key);
  return it->second;
}

//===----- SEARCH OPERATIONS ---------------------------------------------------===//

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::contains(const Key& key) const -> bool {
  size_t bucket_idx = hash(key);
  return find_in_bucket(buckets_[bucket_idx], key) != buckets_[bucket_idx].end();
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::find(const Key& key) -> Value* {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    return &(it->second);
  }
  return nullptr;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::find(const Key& key) const -> const Value* {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    return &(it->second);
  }
  return nullptr;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::count(const Key& key) const -> size_type {
  size_t bucket_idx = hash(key);
  return find_in_bucket(buckets_[bucket_idx], key) != buckets_[bucket_idx].end() ? 1 : 0;
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::erase(const Key& key) -> bool {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    buckets_[bucket_idx].erase(it);
    --size_;
    return true;
  }
  return false;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
void HashTableChaining<Key, Value, Hash>::clear() noexcept {
  for (size_t i = 0; i < capacity_; ++i) {
    buckets_[i].clear();
  }
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::size() const noexcept -> size_type {
  return size_;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::capacity() const noexcept -> size_type {
  return capacity_;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::load_factor() const noexcept -> float {
  return capacity_ > 0 ? static_cast<float>(size_) / static_cast<float>(capacity_) : 0.0f;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::max_load_factor() const noexcept -> float {
  return max_load_factor_;
}

//===----- CONFIGURATION OPERATIONS --------------------------------------------===//

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
void HashTableChaining<Key, Value, Hash>::set_max_load_factor(float mlf) {
  if (mlf <= 0.0f) {
    throw InvalidOperationException("Max load factor must be positive");
  }
  max_load_factor_ = mlf;
  check_and_rehash();
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
void HashTableChaining<Key, Value, Hash>::reserve(size_type new_capacity) {
  if (new_capacity > capacity_) {
    rehash(new_capacity);
  }
}

//=================================================================================//
//===----- PRIVATE HASHING METHODS ---------------------------------------------===//

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::hash(const Key& key) const -> size_t {
  return hasher_(key) % capacity_;
}

//===----- BUCKET SEARCH METHODS -----------------------------------------------===//

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::find_in_bucket(Bucket& bucket, const Key& key) -> typename Bucket::iterator {
  for (auto it = bucket.begin(); it != bucket.end(); ++it) {
    if (it->first == key) {
      return it;
    }
  }
  return bucket.end();
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
auto HashTableChaining<Key, Value, Hash>::find_in_bucket(const Bucket& bucket, const Key& key) const -> typename Bucket::const_iterator {
  for (auto it = bucket.begin(); it != bucket.end(); ++it) {
    if (it->first == key) {
      return it;
    }
  }
  return bucket.end();
}

//===----- REHASHING OPERATIONS ------------------------------------------------===//

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
void HashTableChaining<Key, Value, Hash>::rehash(size_t new_capacity) {
  // Copy when possible for rollback-friendly rehashing; move-only values keep the table usable.
  const size_t                      bucket_count = std::max<size_t>(new_capacity, 1);
  ads::arrays::DynamicArray<Bucket> new_buckets;
  new_buckets.resize(bucket_count);

  for (size_t i = 0; i < capacity_; ++i) {
    for (auto& entry : buckets_[i]) {
      size_t new_idx = hasher_(entry.first) % bucket_count;
      if constexpr (CopyHashValue<Value>) {
        new_buckets[new_idx].emplace_back(entry.first, entry.second);
      } else {
        new_buckets[new_idx].emplace_back(entry.first, std::move(entry.second));
      }
    }
  }

  buckets_  = std::move(new_buckets);
  capacity_ = bucket_count;
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
void HashTableChaining<Key, Value, Hash>::check_and_rehash() {
  if (load_factor() > max_load_factor_) {
    rehash(std::max<size_t>(capacity_ * kGrowthFactor, 1));
  }
}

template <CopyHashKey Key, HashValue Value, typename Hash>
requires HashFor<Hash, Key>
void HashTableChaining<Key, Value, Hash>::ensure_capacity_for_insert() {
  if (capacity_ == 0 || static_cast<float>(size_ + 1) / static_cast<float>(capacity_) > max_load_factor_) {
    rehash(std::max<size_t>(capacity_ * kGrowthFactor, 1));
  }
}

} // namespace ads::hash

//===---------------------------------------------------------------------------===//
