//===--------------------------------------------------------------------------===//
/**
 * @file Hash_Table_Open_Addressing.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the Hash Table with Open Addressing.
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/hash/Hash_Table_Open_Addressing.hpp"

using ads::hash::HashTableOpenAddressing;
using ads::hash::ProbingStrategy;

//============================================================================//
// CONSTRUCTORS AND ASSIGNMENT
//============================================================================//

template <typename Key, typename Value>
HashTableOpenAddressing<Key, Value>::HashTableOpenAddressing(size_t          initial_capacity,
                                                              float           max_load_factor,
                                                              ProbingStrategy strategy)
    : table_(std::make_unique<Slot[]>(initial_capacity)),
      capacity_(initial_capacity),
      size_(0),
      max_load_factor_(max_load_factor),
      strategy_(strategy) {
  if (max_load_factor <= 0.0f || max_load_factor >= 1.0f) {
    throw InvalidOperationException("Max load factor must be in (0, 1)");
  }
}

template <typename Key, typename Value>
HashTableOpenAddressing<Key, Value>::HashTableOpenAddressing(
    HashTableOpenAddressing&& other) noexcept
    : table_(std::move(other.table_)),
      capacity_(other.capacity_),
      size_(other.size_),
      max_load_factor_(other.max_load_factor_),
      strategy_(other.strategy_) {
  other.capacity_        = 0;
  other.size_            = 0;
  other.max_load_factor_ = kDefaultMaxLoadFactor;
}

template <typename Key, typename Value>
HashTableOpenAddressing<Key, Value>& HashTableOpenAddressing<Key, Value>::operator=(
    HashTableOpenAddressing&& other) noexcept {
  if (this != &other) {
    table_                 = std::move(other.table_);
    capacity_              = other.capacity_;
    size_                  = other.size_;
    max_load_factor_       = other.max_load_factor_;
    strategy_              = other.strategy_;
    other.capacity_        = 0;
    other.size_            = 0;
    other.max_load_factor_ = kDefaultMaxLoadFactor;
  }
  return *this;
}

//============================================================================//
// HASH FUNCTIONS
//============================================================================//

template <typename Key, typename Value>
size_t HashTableOpenAddressing<Key, Value>::hash1(const Key& key) const {
  return std::hash<Key>{}(key) % capacity_;
}

template <typename Key, typename Value>
size_t HashTableOpenAddressing<Key, Value>::hash2(const Key& key) const {
  // Secondary hash for double hashing
  // Use a different hash and ensure it's never 0 and coprime with capacity
  size_t h = std::hash<Key>{}(key);
  // Make it odd to be coprime with power-of-2 capacity
  size_t result = (h % (capacity_ - 1)) + 1;
  // Ensure it's odd
  if (result % 2 == 0) {
    result++;
  }
  return result;
}

//============================================================================//
// PROBING
//============================================================================//

template <typename Key, typename Value>
size_t HashTableOpenAddressing<Key, Value>::probe(const Key& key, size_t i) const {
  switch (strategy_) {
  case ProbingStrategy::LINEAR:
    return (hash1(key) + i) % capacity_;

  case ProbingStrategy::QUADRATIC:
    return (hash1(key) + i * i) % capacity_;

  case ProbingStrategy::DOUBLE_HASH:
    return (hash1(key) + i * hash2(key)) % capacity_;

  default:
    return (hash1(key) + i) % capacity_;
  }
}

//============================================================================//
// SLOT FINDING
//============================================================================//

template <typename Key, typename Value>
auto HashTableOpenAddressing<Key, Value>::find_slot(const Key& key) -> Slot* {
  for (size_t i = 0; i < capacity_; ++i) {
    size_t idx = probe(key, i);

    if (table_[idx].state == SlotState::EMPTY) {
      // Key not found
      return nullptr;
    }

    if (table_[idx].state == SlotState::OCCUPIED && table_[idx].entry->key == key) {
      // Key found
      return &table_[idx];
    }

    // Continue probing (DELETED slots don't break the chain)
  }

  // Probed entire table without finding
  return nullptr;
}

template <typename Key, typename Value>
auto HashTableOpenAddressing<Key, Value>::find_slot(const Key& key) const -> const Slot* {
  for (size_t i = 0; i < capacity_; ++i) {
    size_t idx = probe(key, i);

    if (table_[idx].state == SlotState::EMPTY) {
      return nullptr;
    }

    if (table_[idx].state == SlotState::OCCUPIED && table_[idx].entry->key == key) {
      return &table_[idx];
    }
  }

  return nullptr;
}

template <typename Key, typename Value>
auto HashTableOpenAddressing<Key, Value>::find_insert_slot(const Key& key) -> Slot* {
  Slot* first_deleted = nullptr;

  for (size_t i = 0; i < capacity_; ++i) {
    size_t idx = probe(key, i);

    if (table_[idx].state == SlotState::EMPTY) {
      // Found empty slot, use it (or use first deleted if we found one earlier)
      return first_deleted ? first_deleted : &table_[idx];
    }

    if (table_[idx].state == SlotState::DELETED && !first_deleted) {
      // Remember first deleted slot
      first_deleted = &table_[idx];
    }

    if (table_[idx].state == SlotState::OCCUPIED && table_[idx].entry->key == key) {
      // Key already exists, return its slot for update
      return &table_[idx];
    }
  }

  // Table is full (should not happen if load factor is properly managed)
  if (first_deleted) {
    return first_deleted;
  }

  throw InvalidOperationException("Hash table is full");
}

//============================================================================//
// INSERTION
//============================================================================//

template <typename Key, typename Value>
bool HashTableOpenAddressing<Key, Value>::insert(const Key& key, const Value& value) {
  check_and_rehash();

  Slot* slot = find_insert_slot(key);

  if (slot->state == SlotState::OCCUPIED) {
    // Update existing key
    slot->entry->value = value;
    return false;
  }

  // Insert new entry
  slot->entry = Entry(key, value);
  slot->state = SlotState::OCCUPIED;
  ++size_;
  return true;
}

template <typename Key, typename Value>
bool HashTableOpenAddressing<Key, Value>::insert(Key&& key, Value&& value) {
  check_and_rehash();

  // Need to search with const key reference
  const Key& key_ref = key;
  Slot*      slot    = find_insert_slot(key_ref);

  if (slot->state == SlotState::OCCUPIED) {
    // Update existing key
    slot->entry->value = std::move(value);
    return false;
  }

  // Insert new entry
  slot->entry = Entry(std::move(key), std::move(value));
  slot->state = SlotState::OCCUPIED;
  ++size_;
  return true;
}

template <typename Key, typename Value>
template <typename... Args>
Value& HashTableOpenAddressing<Key, Value>::emplace(const Key& key, Args&&... args) {
  check_and_rehash();

  Slot* slot = find_insert_slot(key);

  if (slot->state == SlotState::OCCUPIED) {
    // Update existing key
    slot->entry->value = Value(std::forward<Args>(args)...);
    return slot->entry->value;
  }

  // Insert new entry
  slot->entry = Entry(key, std::forward<Args>(args)...);
  slot->state = SlotState::OCCUPIED;
  ++size_;

  return slot->entry->value;
}

//============================================================================//
// ACCESS
//============================================================================//

template <typename Key, typename Value>
Value& HashTableOpenAddressing<Key, Value>::at(const Key& key) {
  Slot* slot = find_slot(key);

  if (!slot) {
    throw KeyNotFoundException("Key not found in hash table");
  }

  return slot->entry->value;
}

template <typename Key, typename Value>
const Value& HashTableOpenAddressing<Key, Value>::at(const Key& key) const {
  const Slot* slot = find_slot(key);

  if (!slot) {
    throw KeyNotFoundException("Key not found in hash table");
  }

  return slot->entry->value;
}

template <typename Key, typename Value>
Value& HashTableOpenAddressing<Key, Value>::operator[](const Key& key) {
  Slot* slot = find_slot(key);

  if (slot) {
    return slot->entry->value;
  }

  // Key doesn't exist, insert with default value
  insert(key, Value{});

  // Find it again (might be at different location after potential rehash)
  slot = find_slot(key);
  return slot->entry->value;
}

//============================================================================//
// SEARCH
//============================================================================//

template <typename Key, typename Value>
bool HashTableOpenAddressing<Key, Value>::contains(const Key& key) const {
  return find_slot(key) != nullptr;
}

template <typename Key, typename Value>
Value* HashTableOpenAddressing<Key, Value>::find(const Key& key) {
  Slot* slot = find_slot(key);
  return slot ? &(slot->entry->value) : nullptr;
}

template <typename Key, typename Value>
const Value* HashTableOpenAddressing<Key, Value>::find(const Key& key) const {
  const Slot* slot = find_slot(key);
  return slot ? &(slot->entry->value) : nullptr;
}

//============================================================================//
// REMOVAL
//============================================================================//

template <typename Key, typename Value>
bool HashTableOpenAddressing<Key, Value>::erase(const Key& key) {
  Slot* slot = find_slot(key);

  if (!slot) {
    return false;
  }

  // Mark as deleted (tombstone)
  slot->entry.reset();
  slot->state = SlotState::DELETED;
  --size_;

  return true;
}

template <typename Key, typename Value>
void HashTableOpenAddressing<Key, Value>::clear() noexcept {
  for (size_t i = 0; i < capacity_; ++i) {
    table_[i].entry.reset();
    table_[i].state = SlotState::EMPTY;
  }
  size_ = 0;
}

//============================================================================//
// PROPERTIES
//============================================================================//

template <typename Key, typename Value>
size_t HashTableOpenAddressing<Key, Value>::size() const noexcept {
  return size_;
}

template <typename Key, typename Value>
size_t HashTableOpenAddressing<Key, Value>::capacity() const noexcept {
  return capacity_;
}

template <typename Key, typename Value>
bool HashTableOpenAddressing<Key, Value>::is_empty() const noexcept {
  return size_ == 0;
}

template <typename Key, typename Value>
float HashTableOpenAddressing<Key, Value>::load_factor() const noexcept {
  return capacity_ > 0 ? static_cast<float>(size_) / static_cast<float>(capacity_) : 0.0f;
}

template <typename Key, typename Value>
float HashTableOpenAddressing<Key, Value>::max_load_factor() const noexcept {
  return max_load_factor_;
}

template <typename Key, typename Value>
ProbingStrategy HashTableOpenAddressing<Key, Value>::probing_strategy() const noexcept {
  return strategy_;
}

//============================================================================//
// CONFIGURATION
//============================================================================//

template <typename Key, typename Value>
void HashTableOpenAddressing<Key, Value>::set_max_load_factor(float mlf) {
  if (mlf <= 0.0f || mlf >= 1.0f) {
    throw InvalidOperationException("Max load factor must be in (0, 1)");
  }
  max_load_factor_ = mlf;
  check_and_rehash();
}

template <typename Key, typename Value>
void HashTableOpenAddressing<Key, Value>::reserve(size_t new_capacity) {
  if (new_capacity > capacity_) {
    rehash(new_capacity);
  }
}

//============================================================================//
// REHASHING
//============================================================================//

template <typename Key, typename Value>
void HashTableOpenAddressing<Key, Value>::rehash(size_t new_capacity) {
  // Create new table
  auto new_table = std::make_unique<Slot[]>(new_capacity);

  // Save old data
  auto   old_table    = std::move(table_);
  size_t old_capacity = capacity_;

  // Update to new table
  table_    = std::move(new_table);
  capacity_ = new_capacity;
  size_     = 0;

  // Reinsert all occupied entries (skip tombstones!)
  for (size_t i = 0; i < old_capacity; ++i) {
    if (old_table[i].state == SlotState::OCCUPIED) {
      // Reinsert the entry
      insert(std::move(old_table[i].entry->key), std::move(old_table[i].entry->value));
      // Decrement size since insert increments it
      --size_;
    }
  }

  // Restore correct size
  for (size_t i = 0; i < old_capacity; ++i) {
    if (old_table[i].state == SlotState::OCCUPIED) {
      ++size_;
    }
  }
}

template <typename Key, typename Value>
void HashTableOpenAddressing<Key, Value>::check_and_rehash() {
  if (load_factor() >= max_load_factor_) {
    rehash(capacity_ * kGrowthFactor);
  }
}

//===--------------------------------------------------------------------------===//
