//===---------------------------------------------------------------------------===//
/**
 * @file Tree_Set.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for TreeSet.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/associative/Tree_Set.hpp"

namespace ads::associative {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
TreeSet<T>::TreeSet(std::initializer_list<T> values) {
  for (const auto& value : values) {
    insert(value);
  }
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename T>
auto TreeSet<T>::insert(const T& value) -> bool {
  return tree_.insert(value);
}

template <typename T>
auto TreeSet<T>::insert(T&& value) -> bool {
  return tree_.insert(std::move(value));
}

template <typename T>
template <typename... Args>
auto TreeSet<T>::emplace(Args&&... args) -> bool {
  T value(std::forward<Args>(args)...);
  return insert(std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
auto TreeSet<T>::erase(const T& value) -> bool {
  return tree_.remove(value);
}

template <typename T>
auto TreeSet<T>::clear() noexcept -> void {
  tree_.clear();
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto TreeSet<T>::contains(const T& value) const -> bool {
  return tree_.contains(value);
}

template <typename T>
auto TreeSet<T>::is_empty() const noexcept -> bool {
  return tree_.is_empty();
}

template <typename T>
auto TreeSet<T>::size() const noexcept -> size_t {
  return tree_.size();
}

template <typename T>
auto TreeSet<T>::min() const -> const T& {
  return tree_.find_min();
}

template <typename T>
auto TreeSet<T>::max() const -> const T& {
  return tree_.find_max();
}

//===-------------------------- TRAVERSAL OPERATIONS ---------------------------===//

template <typename T>
auto TreeSet<T>::to_vector() const -> std::vector<T> {
  std::vector<T> result;
  result.reserve(size());
  tree_.in_order_traversal([&result](const T& value) { result.push_back(value); });
  return result;
}

template <typename T>
auto TreeSet<T>::for_each(const std::function<void(const T&)>& visit) const -> void {
  tree_.in_order_traversal(visit);
}

} // namespace ads::associative

//===---------------------------------------------------------------------------===//
