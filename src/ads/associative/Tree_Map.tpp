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
TreeMap<Key, Value>::TreeMap(std::initializer_list<std::pair<Key, Value>> init)
  requires std::copy_constructible<Key> && std::copy_constructible<Value>
{
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
  const Entry* entry = find_entry(key);
  if (!entry) {
    throw KeyNotFoundException();
  }
  return entry->value;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::get(const Key& key) const -> const Value& {
  const Entry* entry = find_entry(key);
  if (!entry) {
    throw KeyNotFoundException();
  }
  return entry->value;
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
  const Entry* entry = find_entry(key);
  if (!entry) {
    return nullptr;
  }
  return &entry->value;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::find(const Key& key) const -> const Value* {
  const Entry* entry = find_entry(key);
  if (!entry) {
    return nullptr;
  }
  return &entry->value;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename Key, typename Value>
auto TreeMap<Key, Value>::insert(const Key& key, const Value& value) -> bool
  requires std::copy_constructible<Key> && std::copy_constructible<Value> && std::assignable_from<Value&, const Value&>
{
  bool inserted = !contains(key);
  put(key, value);
  return inserted;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::insert(const Key& key, Value&& value) -> bool
  requires std::copy_constructible<Key> && std::move_constructible<Value> && std::assignable_from<Value&, Value>
{
  bool inserted = !contains(key);
  put(key, std::move(value));
  return inserted;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::insert(Key&& key, Value&& value) -> bool
  requires std::move_constructible<Key> && std::move_constructible<Value> && std::assignable_from<Value&, Value>
{
  bool inserted = !contains(key);
  put(std::move(key), std::move(value));
  return inserted;
}

template <typename Key, typename Value>
template <typename... Args>
auto TreeMap<Key, Value>::emplace(Key key, Args&&... args) -> bool
  requires std::constructible_from<Value, Args...> && std::assignable_from<Value&, Value>
{
  if (const Entry* entry = find_entry(key)) {
    entry->value = Value(std::forward<Args>(args)...);
    return false;
  }

  tree_.insert(Entry(std::piecewise_construct, std::move(key), std::forward<Args>(args)...));
  return true;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::put(const Key& key, const Value& value) -> void
  requires std::copy_constructible<Key> && std::copy_constructible<Value> && std::assignable_from<Value&, const Value&>
{
  if (const Entry* entry = find_entry(key)) {
    entry->value = value;
    return;
  }

  tree_.insert(Entry(key, value));
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::put(const Key& key, Value&& value) -> void
  requires std::copy_constructible<Key> && std::move_constructible<Value> && std::assignable_from<Value&, Value>
{
  if (const Entry* entry = find_entry(key)) {
    entry->value = std::move(value);
    return;
  }

  tree_.insert(Entry(key, std::move(value)));
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::put(Key&& key, Value&& value) -> void
  requires std::move_constructible<Key> && std::move_constructible<Value> && std::assignable_from<Value&, Value>
{
  if (const Entry* entry = find_entry(key)) {
    entry->value = std::move(value);
    return;
  }

  tree_.insert(Entry(std::move(key), std::move(value)));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename Key, typename Value>
auto TreeMap<Key, Value>::remove(const Key& key) -> bool {
  const Entry* entry = find_entry(key);
  return entry != nullptr && tree_.remove(*entry);
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
auto TreeMap<Key, Value>::keys() const -> std::vector<Key>
  requires std::copy_constructible<Key>
{
  std::vector<Key> result;
  result.reserve(tree_.size());

  tree_.in_order_traversal([&result](const Entry& entry) { result.push_back(entry.key); });
  return result;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::values() const -> std::vector<Value>
  requires std::copy_constructible<Value>
{
  std::vector<Value> result;
  result.reserve(tree_.size());

  tree_.in_order_traversal([&result](const Entry& entry) { result.push_back(entry.value); });
  return result;
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::entries() const -> std::vector<std::pair<Key, Value>>
  requires std::copy_constructible<Key> && std::copy_constructible<Value>
{
  std::vector<std::pair<Key, Value>> result;
  result.reserve(tree_.size());

  tree_.in_order_traversal([&result](const Entry& entry) { result.emplace_back(entry.key, entry.value); });
  return result;
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename Key, typename Value>
auto TreeMap<Key, Value>::find_entry(const Key& key) -> const Entry* {
  return tree_.find_equivalent(key, EntryKeyLess{});
}

template <typename Key, typename Value>
auto TreeMap<Key, Value>::find_entry(const Key& key) const -> const Entry* {
  return tree_.find_equivalent(key, EntryKeyLess{});
}

} // namespace ads::associative

//===---------------------------------------------------------------------------===//
