//===---------------------------------------------------------------------------===//
/**
 * @file Hash_Map.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the HashMap class
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/associative/Hash_Map.hpp"

namespace ads::associative {

//===------------------------- ITERATOR IMPLEMENTATION -------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::iterator::operator*() const -> reference {
  // Return Entry& directly - works with structured bindings via tuple interface.
  return *list_it_;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::iterator::operator->() const -> pointer {
  return &(*list_it_);
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::iterator::operator++() -> iterator& {
  ++list_it_;

  // If reached end of current bucket, find next non-empty bucket.
  if (map_ && list_it_ == map_->table_.buckets_[bucket_idx_].end()) {
    advance_to_next_bucket();
  }

  return *this;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::iterator::operator++(int) -> iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::iterator::operator==(const iterator& other) const -> bool {
  if (map_ != other.map_)
    return false;
  if (map_ == nullptr)
    return true;
  // If bucket indices are different, iterators are different.
  if (bucket_idx_ != other.bucket_idx_)
    return false;
  // If we're at or past the end (capacity_), bucket index equality is enough.
  if (bucket_idx_ >= map_->table_.capacity_)
    return true;
  // Otherwise, compare list iterators too.
  return list_it_ == other.list_it_;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::iterator::operator!=(const iterator& other) const -> bool {
  return !(*this == other);
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::iterator::advance_to_next_bucket() -> void {
  if (!map_)
    return;

  ++bucket_idx_;

  // Find next non-empty bucket.
  while (bucket_idx_ < map_->table_.capacity_ && map_->table_.buckets_[bucket_idx_].empty()) {
    ++bucket_idx_;
  }

  if (bucket_idx_ < map_->table_.capacity_) {
    list_it_ = map_->table_.buckets_[bucket_idx_].begin();
  }
}

//===---------------------- CONST_ITERATOR IMPLEMENTATION ----------------------===//

// Constructor from const iterator.
template <typename Key, typename Value, typename Hash>
HashMap<Key, Value, Hash>::const_iterator::const_iterator(const iterator& it) :
    map_(it.map_), bucket_idx_(it.bucket_idx_), list_it_(it.list_it_) {
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::const_iterator::operator*() const -> reference {
  return *list_it_;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::const_iterator::operator->() const -> pointer {
  return &(*list_it_);
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::const_iterator::operator++() -> const_iterator& {
  ++list_it_;

  if (map_ && list_it_ == map_->table_.buckets_[bucket_idx_].end()) {
    advance_to_next_bucket();
  }

  return *this;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::const_iterator::operator++(int) -> const_iterator {
  const_iterator temp = *this;
  ++(*this);
  return temp;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::const_iterator::operator==(const const_iterator& other) const -> bool {
  if (map_ != other.map_)
    return false;
  if (map_ == nullptr)
    return true;
  // If bucket indices are different, iterators are different.
  if (bucket_idx_ != other.bucket_idx_)
    return false;
  // If we're at or past the end (capacity_), bucket index equality is enough.
  if (bucket_idx_ >= map_->table_.capacity_)
    return true;
  // Otherwise, compare list iterators too.
  return list_it_ == other.list_it_;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::const_iterator::operator!=(const const_iterator& other) const -> bool {
  return !(*this == other);
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::const_iterator::advance_to_next_bucket() -> void {
  if (!map_)
    return;

  ++bucket_idx_;

  while (bucket_idx_ < map_->table_.capacity_ && map_->table_.buckets_[bucket_idx_].empty()) {
    ++bucket_idx_;
  }

  if (bucket_idx_ < map_->table_.capacity_) {
    list_it_ = map_->table_.buckets_[bucket_idx_].begin();
  }
}

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

// Constructor with initial capacity and load factor.
template <typename Key, typename Value, typename Hash>
HashMap<Key, Value, Hash>::HashMap(size_t initial_capacity, float max_load_factor) : table_(initial_capacity, max_load_factor) {
}

// Constructor from initializer list.
template <typename Key, typename Value, typename Hash>
HashMap<Key, Value, Hash>::HashMap(std::initializer_list<value_type> init) : table_() {
  for (const auto& pair : init) {
    table_.insert(pair.first, pair.second);
  }
}

// Move constructor.
template <typename Key, typename Value, typename Hash>
HashMap<Key, Value, Hash>::HashMap(HashMap&& other) noexcept : table_(std::move(other.table_)) {
}

// Move assignment operator.
template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::operator=(HashMap&& other) noexcept -> HashMap& {
  if (this != &other) {
    table_ = std::move(other.table_);
  }
  return *this;
}

//===----------------------------- ELEMENT ACCESS ------------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::operator[](const Key& key) -> Value& {
  return table_[key];
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::at(const Key& key) -> Value& {
  return table_.at(key);
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::at(const Key& key) const -> const Value& {
  return table_.at(key);
}

//===------------------------- DICTIONARY INTERFACE ---------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::put(const Key& key, const Value& value) -> void {
  table_.insert(key, value);
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::put(Key&& key, Value&& value) -> void {
  table_.insert(std::move(key), std::move(value));
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::get(const Key& key) -> Value& {
  return table_.at(key);
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::get(const Key& key) const -> const Value& {
  return table_.at(key);
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::insert(const value_type& pair) -> std::pair<iterator, bool> {
  bool inserted = !table_.contains(pair.first);
  table_.insert(pair.first, pair.second);

  auto it = find(pair.first);
  return {it, inserted};
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::insert(value_type&& pair) -> std::pair<iterator, bool> {
  bool inserted = !table_.contains(pair.first);
  table_.insert(pair.first, std::move(pair.second));

  auto it = find(pair.first);
  return {it, inserted};
}

template <typename Key, typename Value, typename Hash>
template <typename... Args>
auto HashMap<Key, Value, Hash>::emplace(Args&&... args) -> std::pair<iterator, bool> {
  // Construct the pair from args
  value_type pair(std::forward<Args>(args)...);
  return insert(std::move(pair));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::erase(const Key& key) -> size_t {
  return table_.erase(key) ? 1 : 0;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::erase(iterator pos) -> iterator {
  iterator next = pos;
  ++next;

  if (pos.map_ == this && pos.bucket_idx_ < table_.capacity_) {
    table_.buckets_[pos.bucket_idx_].erase(pos.list_it_);
    --table_.size_;
  }

  return next;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::clear() noexcept -> void {
  table_.clear();
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::empty() const noexcept -> bool {
  return table_.is_empty();
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::size() const noexcept -> size_t {
  return table_.size();
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::load_factor() const noexcept -> float {
  return table_.load_factor();
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::find(const Key& key) -> iterator {
  size_t bucket_idx = table_.hash(key);
  auto&  bucket     = table_.buckets_[bucket_idx];

  for (auto it = bucket.begin(); it != bucket.end(); ++it) {
    if (it->first == key) {
      return iterator(this, bucket_idx, it);
    }
  }

  return end();
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::find(const Key& key) const -> const_iterator {
  size_t bucket_idx = table_.hash(key);
  auto&  bucket     = table_.buckets_[bucket_idx];

  for (auto it = bucket.begin(); it != bucket.end(); ++it) {
    if (it->first == key) {
      return const_iterator(this, bucket_idx, it);
    }
  }

  return end();
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::contains(const Key& key) const -> bool {
  return table_.contains(key);
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::remove(const Key& key) -> bool {
  return table_.erase(key);
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::count(const Key& key) const -> size_t {
  return table_.contains(key) ? 1 : 0;
}

//===--------------------------- CONVENIENCE METHODS ---------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::keys() const -> std::vector<Key> {
  std::vector<Key> result;
  result.reserve(size());

  for (size_t i = 0; i < table_.capacity_; ++i) {
    for (const auto& entry : table_.buckets_[i]) {
      result.push_back(entry.first);
    }
  }

  return result;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::values() const -> std::vector<Value> {
  std::vector<Value> result;
  result.reserve(size());

  for (size_t i = 0; i < table_.capacity_; ++i) {
    for (const auto& entry : table_.buckets_[i]) {
      result.push_back(entry.second);
    }
  }

  return result;
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::entries() const -> std::vector<std::pair<Key, Value>> {
  std::vector<std::pair<Key, Value>> result;
  result.reserve(size());

  for (size_t i = 0; i < table_.capacity_; ++i) {
    for (const auto& entry : table_.buckets_[i]) {
      result.emplace_back(entry.first, entry.second);
    }
  }

  return result;
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::begin() -> iterator {
  // Find first non-empty bucket
  for (size_t i = 0; i < table_.capacity_; ++i) {
    if (!table_.buckets_[i].empty()) {
      return iterator(this, i, table_.buckets_[i].begin());
    }
  }
  return end();
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::begin() const -> const_iterator {
  for (size_t i = 0; i < table_.capacity_; ++i) {
    if (!table_.buckets_[i].empty()) {
      return const_iterator(this, i, table_.buckets_[i].begin());
    }
  }
  return end();
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::end() -> iterator {
  using EntryList = typename hash::HashTableChaining<Key, Value>::Bucket;
  return iterator(this, table_.capacity_, typename EntryList::iterator());
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::end() const -> const_iterator {
  using EntryList = typename hash::HashTableChaining<Key, Value>::Bucket;
  return const_iterator(this, table_.capacity_, typename EntryList::const_iterator());
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::cbegin() const -> const_iterator {
  return begin();
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::cend() const -> const_iterator {
  return end();
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::find_in_table(const Key& key) -> std::pair<size_t, typename std::list<std::pair<Key, Value>>::iterator> {
  size_t bucket_idx = table_.hash(key);
  auto&  bucket     = table_.buckets_[bucket_idx];

  // Search for the key in the bucket.
  for (auto it = bucket.begin(); it != bucket.end(); ++it) {
    if (it->first == key) {
      return {bucket_idx, it};
    }
  }
  return {bucket_idx, bucket.end()};
}

template <typename Key, typename Value, typename Hash>
auto HashMap<Key, Value, Hash>::find_in_table(const Key& key) const
    -> std::pair<size_t, typename std::list<std::pair<Key, Value>>::const_iterator> {
  size_t      bucket_idx = table_.hash(key);
  const auto& bucket     = table_.buckets_[bucket_idx];

  // Search for the key in the bucket.
  for (auto it = bucket.begin(); it != bucket.end(); ++it) {
    if (it->first == key) {
      return {bucket_idx, it};
    }
  }
  return {bucket_idx, bucket.end()};
}

} // namespace ads::associative

//===--------------------------------------------------------------------------===//
