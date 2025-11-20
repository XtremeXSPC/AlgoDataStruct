//===--------------------------------------------------------------------------===//
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
//===--------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/hash/Hash_Table_Chaining.hpp"

using ads::hash::HashTableChaining;

//============================================================================//
// CONSTRUCTORS AND ASSIGNMENT
//============================================================================//

template <typename Key, typename Value>
HashTableChaining<Key, Value>::HashTableChaining(size_t initial_capacity, float max_load_factor)
    : buckets_(std::make_unique<Bucket[]>(initial_capacity)),
      capacity_(initial_capacity),
      size_(0),
      max_load_factor_(max_load_factor) {
  if (max_load_factor <= 0.0f) {
    throw InvalidOperationException("Max load factor must be positive");
  }
}

template <typename Key, typename Value>
HashTableChaining<Key, Value>::HashTableChaining(HashTableChaining&& other) noexcept
    : buckets_(std::move(other.buckets_)),
      capacity_(other.capacity_),
      size_(other.size_),
      max_load_factor_(other.max_load_factor_) {
  other.capacity_        = 0;
  other.size_            = 0;
  other.max_load_factor_ = kDefaultMaxLoadFactor;
}

template <typename Key, typename Value>
HashTableChaining<Key, Value>& HashTableChaining<Key, Value>::operator=(
    HashTableChaining&& other) noexcept {
  if (this != &other) {
    buckets_               = std::move(other.buckets_);
    capacity_              = other.capacity_;
    size_                  = other.size_;
    max_load_factor_       = other.max_load_factor_;
    other.capacity_        = 0;
    other.size_            = 0;
    other.max_load_factor_ = kDefaultMaxLoadFactor;
  }
  return *this;
}

//============================================================================//
// HASH FUNCTION
//============================================================================//

template <typename Key, typename Value>
size_t HashTableChaining<Key, Value>::hash(const Key& key) const {
  return std::hash<Key>{}(key) % capacity_;
}

//============================================================================//
// BUCKET SEARCH
//============================================================================//

template <typename Key, typename Value>
auto HashTableChaining<Key, Value>::find_in_bucket(Bucket& bucket, const Key& key) ->
    typename Bucket::iterator {
  for (auto it = bucket.begin(); it != bucket.end(); ++it) {
    if (it->key == key) {
      return it;
    }
  }
  return bucket.end();
}

template <typename Key, typename Value>
auto HashTableChaining<Key, Value>::find_in_bucket(const Bucket& bucket, const Key& key) const ->
    typename Bucket::const_iterator {
  for (auto it = bucket.begin(); it != bucket.end(); ++it) {
    if (it->key == key) {
      return it;
    }
  }
  return bucket.end();
}

//============================================================================//
// INSERTION
//============================================================================//

template <typename Key, typename Value>
void HashTableChaining<Key, Value>::insert(const Key& key, const Value& value) {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    // Key exists, update value
    it->value = value;
  } else {
    // Key doesn't exist, insert new entry
    buckets_[bucket_idx].emplace_back(key, value);
    ++size_;
    check_and_rehash();
  }
}

template <typename Key, typename Value>
void HashTableChaining<Key, Value>::insert(Key&& key, Value&& value) {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    // Key exists, update value
    it->value = std::move(value);
  } else {
    // Key doesn't exist, insert new entry
    buckets_[bucket_idx].emplace_back(std::move(key), std::move(value));
    ++size_;
    check_and_rehash();
  }
}

template <typename Key, typename Value>
template <typename... Args>
Value& HashTableChaining<Key, Value>::emplace(const Key& key, Args&&... args) {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    // Key exists, update value
    it->value = Value(std::forward<Args>(args)...);
    return it->value;
  } else {
    // Key doesn't exist, insert new entry
    buckets_[bucket_idx].emplace_back(key, std::forward<Args>(args)...);
    ++size_;
    check_and_rehash();
    // Return reference to the newly inserted value
    return buckets_[hash(key)].back().value;
  }
}

//============================================================================//
// ACCESS
//============================================================================//

template <typename Key, typename Value>
Value& HashTableChaining<Key, Value>::at(const Key& key) {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it == buckets_[bucket_idx].end()) {
    throw KeyNotFoundException("Key not found in hash table");
  }

  return it->value;
}

template <typename Key, typename Value>
const Value& HashTableChaining<Key, Value>::at(const Key& key) const {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it == buckets_[bucket_idx].end()) {
    throw KeyNotFoundException("Key not found in hash table");
  }

  return it->value;
}

template <typename Key, typename Value>
Value& HashTableChaining<Key, Value>::operator[](const Key& key) {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    return it->value;
  }

  // Key doesn't exist, insert with default value
  buckets_[bucket_idx].emplace_back(key, Value{});
  ++size_;
  check_and_rehash();

  // Return reference to newly inserted value
  // Need to rehash the key since capacity might have changed
  bucket_idx = hash(key);
  it         = find_in_bucket(buckets_[bucket_idx], key);
  return it->value;
}

//============================================================================//
// SEARCH
//============================================================================//

template <typename Key, typename Value>
bool HashTableChaining<Key, Value>::contains(const Key& key) const {
  size_t bucket_idx = hash(key);
  return find_in_bucket(buckets_[bucket_idx], key) != buckets_[bucket_idx].end();
}

template <typename Key, typename Value>
Value* HashTableChaining<Key, Value>::find(const Key& key) {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    return &(it->value);
  }
  return nullptr;
}

template <typename Key, typename Value>
const Value* HashTableChaining<Key, Value>::find(const Key& key) const {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    return &(it->value);
  }
  return nullptr;
}

//============================================================================//
// REMOVAL
//============================================================================//

template <typename Key, typename Value>
bool HashTableChaining<Key, Value>::erase(const Key& key) {
  size_t bucket_idx = hash(key);
  auto   it         = find_in_bucket(buckets_[bucket_idx], key);

  if (it != buckets_[bucket_idx].end()) {
    buckets_[bucket_idx].erase(it);
    --size_;
    return true;
  }
  return false;
}

template <typename Key, typename Value>
void HashTableChaining<Key, Value>::clear() noexcept {
  for (size_t i = 0; i < capacity_; ++i) {
    buckets_[i].clear();
  }
  size_ = 0;
}

//============================================================================//
// PROPERTIES
//============================================================================//

template <typename Key, typename Value>
size_t HashTableChaining<Key, Value>::size() const noexcept {
  return size_;
}

template <typename Key, typename Value>
size_t HashTableChaining<Key, Value>::capacity() const noexcept {
  return capacity_;
}

template <typename Key, typename Value>
bool HashTableChaining<Key, Value>::is_empty() const noexcept {
  return size_ == 0;
}

template <typename Key, typename Value>
float HashTableChaining<Key, Value>::load_factor() const noexcept {
  return capacity_ > 0 ? static_cast<float>(size_) / static_cast<float>(capacity_) : 0.0f;
}

template <typename Key, typename Value>
float HashTableChaining<Key, Value>::max_load_factor() const noexcept {
  return max_load_factor_;
}

//============================================================================//
// CONFIGURATION
//============================================================================//

template <typename Key, typename Value>
void HashTableChaining<Key, Value>::set_max_load_factor(float mlf) {
  if (mlf <= 0.0f) {
    throw InvalidOperationException("Max load factor must be positive");
  }
  max_load_factor_ = mlf;
  check_and_rehash();
}

template <typename Key, typename Value>
void HashTableChaining<Key, Value>::reserve(size_t new_capacity) {
  if (new_capacity > capacity_) {
    rehash(new_capacity);
  }
}

//============================================================================//
// REHASHING
//============================================================================//

template <typename Key, typename Value>
void HashTableChaining<Key, Value>::rehash(size_t new_capacity) {
  // Create new bucket array
  auto new_buckets = std::make_unique<Bucket[]>(new_capacity);

  // Save old data
  auto   old_buckets  = std::move(buckets_);
  size_t old_capacity = capacity_;

  // Update capacity and buckets
  buckets_  = std::move(new_buckets);
  capacity_ = new_capacity;
  size_     = 0;

  // Reinsert all entries
  for (size_t i = 0; i < old_capacity; ++i) {
    for (auto& entry : old_buckets[i]) {
      insert(std::move(entry.key), std::move(entry.value));
      // Decrement size since insert increments it
      --size_;
    }
  }

  // Restore correct size (we decremented it in the loop)
  for (size_t i = 0; i < old_capacity; ++i) {
    size_ += old_buckets[i].size();
  }
}

template <typename Key, typename Value>
void HashTableChaining<Key, Value>::check_and_rehash() {
  if (load_factor() > max_load_factor_) {
    rehash(capacity_ * kGrowthFactor);
  }
}

//===--------------------------------------------------------------------------===//
