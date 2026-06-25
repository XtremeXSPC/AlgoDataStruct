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

//===----- CONSTRUCTORS AND ASSIGNMENT -----------------------------------------===//

template <MapKey Key, MapValue Value>
TreeMap<Key, Value>::TreeMap(std::initializer_list<std::pair<Key, Value>> init) requires CopyMapKey<Key> && CopyMapValue<Value>
{
  for (const auto& pair : init) {
    put(pair.first, pair.second);
  }
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::empty() const noexcept -> bool {
  return tree_.is_empty();
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::size() const noexcept -> size_type {
  return tree_.size();
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::contains(const Key& key) const -> bool {
  return find_entry(key) != nullptr;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::count(const Key& key) const -> size_type {
  return find_entry(key) != nullptr ? 1 : 0;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::min() const -> const Key& {
  return tree_.find_min().key;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::max() const -> const Key& {
  return tree_.find_max().key;
}

//===----- ELEMENT ACCESS ------------------------------------------------------===//

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::operator[](const Key& key) -> Value& requires DefaultMapValue<Value>
{
  if (const Entry* entry = find_entry(key)) {
    return entry->value;
  }

  tree_.insert(Entry(key, Value{}));
  const Entry* inserted = find_entry(key);
  if (!inserted) {
    throw TreeMapException("TreeMap::operator[]: failed to locate entry after insertion");
  }
  return inserted->value;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::at(const Key& key) -> Value& {
  const Entry* entry = find_entry(key);
  if (!entry) {
    throw KeyNotFoundException();
  }
  return entry->value;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::at(const Key& key) const -> const Value& {
  const Entry* entry = find_entry(key);
  if (!entry) {
    throw KeyNotFoundException();
  }
  return entry->value;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::find(const Key& key) -> Value* {
  const Entry* entry = find_entry(key);
  if (!entry) {
    return nullptr;
  }
  return &entry->value;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::find(const Key& key) const -> const Value* {
  const Entry* entry = find_entry(key);
  if (!entry) {
    return nullptr;
  }
  return &entry->value;
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::insert(const Key& key, const Value& value) -> bool requires CopyMapEntry<Key, Value>
{
  bool inserted = !contains(key);
  put(key, value);
  return inserted;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::insert(const Key& key, Value&& value) -> bool requires CopyKeyMoveMapEntry<Key, Value>
{
  bool inserted = !contains(key);
  put(key, std::move(value));
  return inserted;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::insert(Key&& key, Value&& value) -> bool requires MoveMapEntry<Key, Value>
{
  bool inserted = !contains(key);
  put(std::move(key), std::move(value));
  return inserted;
}

template <MapKey Key, MapValue Value>
template <typename... Args>
auto TreeMap<Key, Value>::emplace(Key key, Args&&... args) -> bool requires EmplaceMapValue<Value, Args...>
{
  if (const Entry* entry = find_entry(key)) {
    entry->value = Value(std::forward<Args>(args)...);
    return false;
  }

  tree_.insert(Entry(std::piecewise_construct, std::move(key), std::forward<Args>(args)...));
  return true;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::put(const Key& key, const Value& value) -> void requires CopyMapEntry<Key, Value>
{
  if (const Entry* entry = find_entry(key)) {
    entry->value = value;
    return;
  }

  tree_.insert(Entry(key, value));
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::put(const Key& key, Value&& value) -> void requires CopyKeyMoveMapEntry<Key, Value>
{
  if (const Entry* entry = find_entry(key)) {
    entry->value = std::move(value);
    return;
  }

  tree_.insert(Entry(key, std::move(value)));
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::put(Key&& key, Value&& value) -> void requires MoveMapEntry<Key, Value>
{
  if (const Entry* entry = find_entry(key)) {
    entry->value = std::move(value);
    return;
  }

  tree_.insert(Entry(std::move(key), std::move(value)));
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::erase(const Key& key) -> bool {
  const Entry* entry = find_entry(key);
  return entry != nullptr && tree_.remove(*entry);
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::clear() noexcept -> void {
  tree_.clear();
}

//===----- CONVENIENCE METHODS -------------------------------------------------===//

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::keys() const -> std::vector<Key> requires CopyMapKey<Key>
{
  std::vector<Key> result;
  result.reserve(tree_.size());

  tree_.in_order_traversal([&result](const Entry& entry) { result.push_back(entry.key); });
  return result;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::values() const -> std::vector<Value> requires CopyMapValue<Value>
{
  std::vector<Value> result;
  result.reserve(tree_.size());

  tree_.in_order_traversal([&result](const Entry& entry) { result.push_back(entry.value); });
  return result;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::entries() const -> std::vector<std::pair<Key, Value>> requires CopyMapKey<Key> && CopyMapValue<Value>
{
  std::vector<std::pair<Key, Value>> result;
  result.reserve(tree_.size());

  tree_.in_order_traversal([&result](const Entry& entry) { result.emplace_back(entry.key, entry.value); });
  return result;
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::for_each(const std::function<void(const Key&, const Value&)>& visit) const -> void {
  tree_.in_order_traversal([&visit](const Entry& entry) { visit(entry.key, entry.value); });
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::find_entry(const Key& key) -> const Entry* {
  return tree_.find_equivalent(key, EntryKeyLess{});
}

template <MapKey Key, MapValue Value>
auto TreeMap<Key, Value>::find_entry(const Key& key) const -> const Entry* {
  return tree_.find_equivalent(key, EntryKeyLess{});
}

} // namespace ads::associative

//===---------------------------------------------------------------------------===//
