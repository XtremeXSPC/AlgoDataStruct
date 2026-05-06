//===---------------------------------------------------------------------------===//
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
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/hash/Hash_Table_Open_Addressing.hpp"

#include <algorithm>
#include <numeric>

namespace ads::hash {

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

template <typename Key, typename Value, typename Hash>
HashTableOpenAddressing<Key, Value, Hash>::HashTableOpenAddressing(
    size_t initial_capacity, ProbingStrategy strategy, float max_load_factor, Hash hasher) :
    table_(std::make_unique<Slot[]>(std::max<size_t>(initial_capacity, 2))),
    capacity_(std::max<size_t>(initial_capacity, 2)),
    size_(0),
    deleted_count_(0),
    max_load_factor_(max_load_factor),
    strategy_(strategy),
    hasher_(std::move(hasher)) {
  if (max_load_factor <= 0.0f || max_load_factor >= 1.0f) {
    throw InvalidOperationException("Max load factor must be in (0, 1)");
  }
}

template <typename Key, typename Value, typename Hash>
HashTableOpenAddressing<Key, Value, Hash>::HashTableOpenAddressing(HashTableOpenAddressing&& other) noexcept :
    table_(std::move(other.table_)),
    capacity_(other.capacity_),
    size_(other.size_),
    deleted_count_(other.deleted_count_),
    max_load_factor_(other.max_load_factor_),
    strategy_(other.strategy_),
    hasher_(std::move(other.hasher_)) {
  other.capacity_        = 0;
  other.size_            = 0;
  other.deleted_count_   = 0;
  other.max_load_factor_ = kDefaultMaxLoadFactor;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::operator=(HashTableOpenAddressing&& other) noexcept
    -> HashTableOpenAddressing<Key, Value, Hash>& {
  if (this != &other) {
    table_                 = std::move(other.table_);
    capacity_              = other.capacity_;
    size_                  = other.size_;
    deleted_count_         = other.deleted_count_;
    max_load_factor_       = other.max_load_factor_;
    strategy_              = other.strategy_;
    hasher_                = std::move(other.hasher_);
    other.capacity_        = 0;
    other.size_            = 0;
    other.deleted_count_   = 0;
    other.max_load_factor_ = kDefaultMaxLoadFactor;
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::insert(const Key& key, const Value& value) -> bool
  requires std::copy_constructible<Key> && std::copy_constructible<Value> && std::assignable_from<Value&, const Value&>
{
  if (capacity_ == 0) {
    rehash(kInitialCapacity);
  }

  Slot* slot = find_insert_slot(key);

  if (slot->state == SlotState::OCCUPIED) {
    // Existing keys update in place, avoiding resize churn for duplicates.
    slot->entry->value = value;
    return false;
  }

  ensure_capacity_for_insert();
  slot                        = find_insert_slot(key);
  const bool reused_tombstone = slot->state == SlotState::DELETED;
  slot->entry.emplace(key, value);
  slot->state = SlotState::OCCUPIED;
  ++size_;
  if (reused_tombstone) {
    --deleted_count_;
  }
  return true;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::insert(const Key& key, Value&& value) -> bool
  requires std::copy_constructible<Key> && std::move_constructible<Value> && std::assignable_from<Value&, Value>
{
  if (capacity_ == 0) {
    rehash(kInitialCapacity);
  }

  Slot* slot = find_insert_slot(key);

  if (slot->state == SlotState::OCCUPIED) {
    // Preserve the stored key and replace only the mapped resource.
    slot->entry->value = std::move(value);
    return false;
  }

  ensure_capacity_for_insert();
  slot                        = find_insert_slot(key);
  const bool reused_tombstone = slot->state == SlotState::DELETED;
  slot->entry.emplace(key, std::move(value));
  slot->state = SlotState::OCCUPIED;
  ++size_;
  if (reused_tombstone) {
    --deleted_count_;
  }
  return true;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::insert(Key&& key, Value&& value) -> bool
  requires std::move_constructible<Key> && std::move_constructible<Value> && std::assignable_from<Value&, Value>
{
  if (capacity_ == 0) {
    rehash(kInitialCapacity);
  }

  // Keep a reference for hashing and equality checks until the key is stored.
  const Key& key_ref = key;
  Slot*      slot    = find_insert_slot(key_ref);

  if (slot->state == SlotState::OCCUPIED) {
    // Keep the stored key stable on duplicate insert.
    slot->entry->value = std::move(value);
    return false;
  }

  ensure_capacity_for_insert();
  slot                        = find_insert_slot(key_ref);
  const bool reused_tombstone = slot->state == SlotState::DELETED;
  slot->entry.emplace(std::move(key), std::move(value));
  slot->state = SlotState::OCCUPIED;
  ++size_;
  if (reused_tombstone) {
    --deleted_count_;
  }
  return true;
}

template <typename Key, typename Value, typename Hash>
template <typename... Args>
auto HashTableOpenAddressing<Key, Value, Hash>::emplace(const Key& key, Args&&... args) -> Value&
  requires std::copy_constructible<Key> && std::constructible_from<Value, Args...>
           && std::assignable_from<Value&, Value>
{
  if (capacity_ == 0) {
    rehash(kInitialCapacity);
  }

  Slot* slot = find_insert_slot(key);

  if (slot->state == SlotState::OCCUPIED) {
    // Construct before assignment so a failed construction keeps the old value.
    slot->entry->value = Value(std::forward<Args>(args)...);
    return slot->entry->value;
  }

  ensure_capacity_for_insert();
  slot                        = find_insert_slot(key);
  const bool reused_tombstone = slot->state == SlotState::DELETED;
  slot->entry.emplace(key, std::forward<Args>(args)...);
  slot->state = SlotState::OCCUPIED;
  ++size_;
  if (reused_tombstone) {
    --deleted_count_;
  }

  return slot->entry->value;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::at(const Key& key) -> Value& {
  Slot* slot = find_slot(key);

  if (!slot) {
    throw KeyNotFoundException("Key not found in hash table");
  }

  return slot->entry->value;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::at(const Key& key) const -> const Value& {
  const Slot* slot = find_slot(key);

  if (!slot) {
    throw KeyNotFoundException("Key not found in hash table");
  }

  return slot->entry->value;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::operator[](const Key& key) -> Value& {
  Slot* slot = find_slot(key);

  if (slot) {
    return slot->entry->value;
  }

  // Key doesn't exist, insert with default value.
  insert(key, Value{});

  // Find it again (might be at different location after potential rehash).
  slot = find_slot(key);
  return slot->entry->value;
}

//===---------------------------- SEARCH OPERATIONS ----------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::contains(const Key& key) const -> bool {
  return find_slot(key) != nullptr;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::find(const Key& key) -> Value* {
  Slot* slot = find_slot(key);
  return slot ? &(slot->entry->value) : nullptr;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::find(const Key& key) const -> const Value* {
  const Slot* slot = find_slot(key);
  return slot ? &(slot->entry->value) : nullptr;
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::erase(const Key& key) -> bool {
  Slot* slot = find_slot(key);

  if (!slot) {
    return false;
  }

  // Tombstones preserve probe chains until a later rehash compacts them.
  slot->entry.reset();
  slot->state = SlotState::DELETED;
  --size_;
  ++deleted_count_;

  return true;
}

template <typename Key, typename Value, typename Hash>
void HashTableOpenAddressing<Key, Value, Hash>::clear() noexcept {
  for (size_t i = 0; i < capacity_; ++i) {
    table_[i].entry.reset();
    table_[i].state = SlotState::EMPTY;
  }
  size_          = 0;
  deleted_count_ = 0;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::size() const noexcept -> size_t {
  return size_;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::capacity() const noexcept -> size_t {
  return capacity_;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::load_factor() const noexcept -> float {
  return capacity_ > 0 ? static_cast<float>(size_) / static_cast<float>(capacity_) : 0.0f;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::max_load_factor() const noexcept -> float {
  return max_load_factor_;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::probing_strategy() const noexcept -> ProbingStrategy {
  return strategy_;
}

//===------------------------ CONFIGURATION OPERATIONS -------------------------===//

template <typename Key, typename Value, typename Hash>
void HashTableOpenAddressing<Key, Value, Hash>::reserve(size_t new_capacity) {
  if (new_capacity > capacity_) {
    rehash(new_capacity);
  }
}

template <typename Key, typename Value, typename Hash>
void HashTableOpenAddressing<Key, Value, Hash>::set_max_load_factor(float mlf) {
  if (mlf <= 0.0f || mlf >= 1.0f) {
    throw InvalidOperationException("Max load factor must be in (0, 1)");
  }
  max_load_factor_ = mlf;
  check_and_rehash();
}

//=================================================================================//
//===------------------------- PRIVATE HASHING METHODS -------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::hash1(const Key& key) const -> size_t {
  return hash1(key, capacity_);
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::hash1(const Key& key, size_t capacity) const -> size_t {
  return hasher_(key) % capacity;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::hash2(const Key& key) const -> size_t {
  return hash2(key, capacity_);
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::hash2(const Key& key, size_t capacity) const -> size_t {
  if (capacity <= 2) {
    return 1;
  }

  size_t result = (hasher_(key) % (capacity - 1)) + 1;
  while (std::gcd(result, capacity) != 1) {
    ++result;
    if (result >= capacity) {
      result = 1;
    }
  }
  return result;
}

//===----------------------------- PROBING METHODS -----------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::probe(const Key& key, size_t i) const -> size_t {
  return probe(key, i, capacity_);
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::probe(const Key& key, size_t i, size_t capacity) const -> size_t {
  switch (strategy_) {
  case ProbingStrategy::LINEAR:
    return (hash1(key, capacity) + i) % capacity;

  case ProbingStrategy::QUADRATIC:
    return (hash1(key, capacity) + i * i) % capacity;

  case ProbingStrategy::DOUBLE_HASH:
    return (hash1(key, capacity) + i * hash2(key, capacity)) % capacity;

  default:
    return (hash1(key, capacity) + i) % capacity;
  }
}

//===-------------------------- SLOT FINDING METHODS ---------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::find_slot(const Key& key) -> Slot* {
  if (capacity_ == 0) {
    return nullptr;
  }

  for (size_t i = 0; i < capacity_; ++i) {
    size_t idx = probe(key, i);

    if (table_[idx].state == SlotState::EMPTY) {
      // Key not found.
      return nullptr;
    }

    if (table_[idx].state == SlotState::OCCUPIED && table_[idx].entry->key == key) {
      // Key found.
      return &table_[idx];
    }

    // Continue probing (DELETED slots don't break the chain).
  }

  // Probed entire table without finding.
  return nullptr;
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::find_slot(const Key& key) const -> const Slot* {
  if (capacity_ == 0) {
    return nullptr;
  }

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

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::find_insert_slot(const Key& key) -> Slot* {
  return find_insert_slot(table_.get(), capacity_, key);
}

template <typename Key, typename Value, typename Hash>
auto HashTableOpenAddressing<Key, Value, Hash>::find_insert_slot(Slot* slots, size_t slot_count, const Key& key) const
    -> Slot* {
  Slot* first_deleted = nullptr;

  for (size_t i = 0; i < slot_count; ++i) {
    size_t idx = probe(key, i, slot_count);

    if (slots[idx].state == SlotState::EMPTY) {
      // Found empty slot, use it (or use first deleted if we found one earlier).
      return first_deleted ? first_deleted : &slots[idx];
    }

    if (slots[idx].state == SlotState::DELETED && !first_deleted) {
      // Remember first deleted slot.
      first_deleted = &slots[idx];
    }

    if (slots[idx].state == SlotState::OCCUPIED && slots[idx].entry->key == key) {
      // Key already exists, return its slot for update.
      return &slots[idx];
    }
  }

  // Table is full (should not happen if load factor is properly managed).
  if (first_deleted) {
    return first_deleted;
  }

  throw InvalidOperationException("Hash table is full");
}

//===-------------------------- REHASHING OPERATIONS ---------------------------===//

template <typename Key, typename Value, typename Hash>
void HashTableOpenAddressing<Key, Value, Hash>::rehash(size_t new_capacity) {
  const size_t            slot_count = std::max<size_t>(new_capacity, 2);
  std::unique_ptr<Slot[]> new_table  = std::make_unique<Slot[]>(slot_count);
  size_t                  new_size   = 0;

  for (size_t i = 0; i < capacity_; ++i) {
    if (table_[i].state == SlotState::OCCUPIED) {
      auto& entry = *table_[i].entry;
      Slot* slot  = find_insert_slot(new_table.get(), slot_count, entry.key);
      if constexpr (std::copy_constructible<Key> && std::copy_constructible<Value>) {
        slot->entry.emplace(entry.key, entry.value);
      } else {
        slot->entry.emplace(std::move(entry.key), std::move(entry.value));
      }
      slot->state = SlotState::OCCUPIED;
      ++new_size;
    }
  }

  table_         = std::move(new_table);
  capacity_      = slot_count;
  size_          = new_size;
  deleted_count_ = 0;
}

template <typename Key, typename Value, typename Hash>
void HashTableOpenAddressing<Key, Value, Hash>::check_and_rehash() {
  if (capacity_ == 0) {
    rehash(kInitialCapacity);
    return;
  }

  const float active_load   = static_cast<float>(size_) / static_cast<float>(capacity_);
  const float occupied_load = static_cast<float>(size_ + deleted_count_) / static_cast<float>(capacity_);
  if (active_load < max_load_factor_ && occupied_load < max_load_factor_) {
    return;
  }

  size_t target_capacity = capacity_;
  while (static_cast<float>(size_) / static_cast<float>(target_capacity) >= max_load_factor_) {
    target_capacity *= kGrowthFactor;
  }
  rehash(target_capacity);
}

template <typename Key, typename Value, typename Hash>
void HashTableOpenAddressing<Key, Value, Hash>::ensure_capacity_for_insert() {
  if (capacity_ == 0) {
    rehash(kInitialCapacity);
    return;
  }

  const float active_load_after_insert = static_cast<float>(size_ + 1) / static_cast<float>(capacity_);
  const float occupied_load_after_insert =
      static_cast<float>(size_ + deleted_count_ + 1) / static_cast<float>(capacity_);
  if (active_load_after_insert < max_load_factor_ && occupied_load_after_insert < max_load_factor_) {
    return;
  }

  size_t target_capacity = capacity_;
  while (static_cast<float>(size_ + 1) / static_cast<float>(target_capacity) >= max_load_factor_) {
    target_capacity *= kGrowthFactor;
  }
  rehash(target_capacity);
}

} // namespace ads::hash

//===--------------------------------------------------------------------------===//
