//===---------------------------------------------------------------------------===//
/**
 * @file Tree_Map.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the TreeMap class.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/associative/Tree_Map.hpp"

namespace ads::associative {

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

template <typename Key, typename Value>
TreeMap<Key, Value>::TreeMap(std::initializer_list<std::pair<Key, Value>> init) {
  for (const auto& pair : init) {
    put(pair.first, pair.second);
  }
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename Key, typename Value>
auto TreeMap<Key, Value>::empty() const noexcept -> bool {
  return tree_.is_empty();
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::get(const Key& key) -> Value& {
  Entry* entry = find_entry(key);
  if (!entry || !entry->value.has_value()) {
    throw KeyNotFoundException();
  }
  return entry->value.value();
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::get(const Key& key) const -> const Value& {
  const Entry* entry = find_entry(key);
  if (!entry || !entry->value.has_value()) {
    throw KeyNotFoundException();
  }
  return entry->value.value();
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::size() const noexcept -> size_t {
  return tree_.size();
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::contains(const Key& key) const -> bool {
  return find_entry(key) != nullptr;
}

//===----------------------------- ELEMENT ACCESS ------------------------------===//

template <typename Key, typename Value>
auto TreeMap<Key, Value>::operator[](const Key& key) -> Value&
  requires std::default_initializable<Value>
{
  if (Entry* entry = find_entry(key)) {
    if (!entry->value.has_value()) {
      entry->value.emplace();
    }
    return entry->value.value();
  }

  tree_.insert(Entry(key, Value{}));
  return find_entry(key)->value.value();
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::at(const Key& key) -> Value& {
  return get(key);
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::at(const Key& key) const -> const Value& {
  return get(key);
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::find(const Key& key) -> Value* {
  Entry* entry = find_entry(key);
  if (!entry || !entry->value.has_value()) {
    return nullptr;
  }
  return &entry->value.value();
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::find(const Key& key) const -> const Value* {
  const Entry* entry = find_entry(key);
  if (!entry || !entry->value.has_value()) {
    return nullptr;
  }
  return &entry->value.value();
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename Key, typename Value>
auto TreeMap<Key, Value>::insert(const Key& key, const Value& value) -> bool {
  bool inserted = !contains(key);
  put(key, value);
  return inserted;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::insert(Key&& key, Value&& value) -> bool {
  bool inserted = !contains(key);
  put(std::move(key), std::move(value));
  return inserted;
}

template <typename Key, typename Value>
template <typename... Args>
auto TreeMap<Key, Value>::emplace(Key key, Args&&... args) -> bool {
  if (Entry* entry = find_entry(key)) {
    entry->value.emplace(std::forward<Args>(args)...);
    return false;
  }

  tree_.insert(Entry(std::piecewise_construct, std::move(key), std::forward<Args>(args)...));
  return true;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::put(const Key& key, const Value& value) -> void {
  if (Entry* entry = find_entry(key)) {
    entry->value = value;
    return;
  }

  tree_.insert(Entry(key, value));
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::put(Key&& key, Value&& value) -> void {
  if (Entry* entry = find_entry(key)) {
    entry->value = std::move(value);
    return;
  }

  tree_.insert(Entry(std::move(key), std::move(value)));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename Key, typename Value>
auto TreeMap<Key, Value>::remove(const Key& key) -> bool {
  return tree_.remove(Entry(key));
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::erase(const Key& key) -> bool {
  return remove(key);
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::clear() noexcept -> void {
  tree_.clear();
}

//===--------------------------- CONVENIENCE METHODS ---------------------------===//

template <typename Key, typename Value>
auto TreeMap<Key, Value>::keys() const -> std::vector<Key> {
  std::vector<Key> result;
  result.reserve(tree_.size());

  tree_.in_order_traversal([&result](const Entry& entry) { result.push_back(entry.key); });
  return result;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::values() const -> std::vector<Value> {
  std::vector<Value> result;
  result.reserve(tree_.size());

  tree_.in_order_traversal([&result](const Entry& entry) {
    if (entry.value.has_value()) {
      result.push_back(entry.value.value());
    }
  });
  return result;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::entries() const -> std::vector<std::pair<Key, Value>> {
  std::vector<std::pair<Key, Value>> result;
  result.reserve(tree_.size());

  tree_.in_order_traversal([&result](const Entry& entry) {
    if (entry.value.has_value()) {
      result.emplace_back(entry.key, entry.value.value());
    }
  });
  return result;
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename Key, typename Value>
auto TreeMap<Key, Value>::find_entry(const Key& key) -> Entry* {
  return tree_.find(Entry(key));
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::find_entry(const Key& key) const -> const Entry* {
  return tree_.find(Entry(key));
}

} // namespace ads::associative

//===---------------------------------------------------------------------------===//
