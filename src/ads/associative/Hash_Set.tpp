//===---------------------------------------------------------------------------===//
/**
 * @file Hash_Set.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for HashSet.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/associative/Hash_Set.hpp"

#include <algorithm>

namespace ads::associative {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T, typename Hash>
HashSet<T, Hash>::HashSet(size_t initial_capacity, double max_load_factor) :
    buckets_(initial_capacity), size_(0), max_load_factor_(max_load_factor), hasher_() {
  if (max_load_factor_ <= 0) {
    max_load_factor_ = 0.75;
  }
}

template <typename T, typename Hash>
HashSet<T, Hash>::HashSet(std::initializer_list<T> values) : HashSet(values.size() * 2) {
  for (const auto& value : values) {
    insert(value);
  }
}

template <typename T, typename Hash>
HashSet<T, Hash>::HashSet(HashSet&& other) noexcept :
    buckets_(std::move(other.buckets_)), size_(other.size_), max_load_factor_(other.max_load_factor_), hasher_(std::move(other.hasher_)) {
  other.size_ = 0;
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::operator=(HashSet&& other) noexcept -> HashSet& {
  if (this != &other) {
    buckets_         = std::move(other.buckets_);
    size_            = other.size_;
    max_load_factor_ = other.max_load_factor_;
    hasher_          = std::move(other.hasher_);
    other.size_      = 0;
  }
  return *this;
}

//===------------------------- MODIFICATION OPERATIONS -------------------------===//

template <typename T, typename Hash>
auto HashSet<T, Hash>::insert(const T& value) -> bool {
  check_load_factor();

  // Find the appropriate bucket.
  size_t idx   = bucket_index(value);
  auto&  chain = buckets_[idx];

  // Check for duplicates.
  for (const auto& elem : chain) {
    if (elem == value) {
      return false;
    }
  }

  // Insert the new element.
  chain.push_back(value);
  ++size_;
  return true;
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::insert(T&& value) -> bool {
  check_load_factor();

  // Find the appropriate bucket.
  size_t idx   = bucket_index(value);
  auto&  chain = buckets_[idx];

  // Check for duplicates.
  for (const auto& elem : chain) {
    if (elem == value) {
      return false;
    }
  }

  // Insert the new element.
  chain.push_back(std::move(value));
  ++size_;
  return true;
}

template <typename T, typename Hash>
template <typename... Args>
auto HashSet<T, Hash>::emplace(Args&&... args) -> bool {
  T value(std::forward<Args>(args)...);
  return insert(std::move(value));
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::erase(const T& value) -> bool {
  size_t idx   = bucket_index(value);
  auto&  chain = buckets_[idx];

  for (auto it = chain.begin(); it != chain.end(); ++it) {
    if (*it == value) {
      chain.erase(it);
      --size_;
      return true;
    }
  }

  return false;
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::clear() noexcept -> void {
  for (auto& bucket : buckets_) {
    bucket.clear();
  }
  size_ = 0;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T, typename Hash>
auto HashSet<T, Hash>::contains(const T& value) const -> bool {
  size_t      idx   = bucket_index(value);
  const auto& chain = buckets_[idx];

  for (const auto& elem : chain) {
    if (elem == value) {
      return true;
    }
  }
  return false;
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::size() const noexcept -> size_t {
  return size_;
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::bucket_count() const noexcept -> size_t {
  return buckets_.size();
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::load_factor() const noexcept -> double {
  if (buckets_.empty()) {
    return 0.0;
  }
  return static_cast<double>(size_) / static_cast<double>(buckets_.size());
}

//===--------------------------- ITERATOR OPERATIONS ---------------------------===//

template <typename T, typename Hash>
auto HashSet<T, Hash>::begin() const -> iterator {
  for (size_t i = 0; i < buckets_.size(); ++i) {
    if (!buckets_[i].empty()) {
      return iterator(this, i, buckets_[i].begin());
    }
  }
  return end();
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::end() const -> iterator {
  return iterator(this, buckets_.size(), {});
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::cbegin() const -> const_iterator {
  return begin();
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::cend() const -> const_iterator {
  return end();
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T, typename Hash>
auto HashSet<T, Hash>::bucket_index(const T& value) const -> size_t {
  return hasher_(value) % buckets_.size();
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::rehash(size_t new_bucket_count) -> void {
  std::vector<std::list<T>> new_buckets(new_bucket_count);

  for (auto& bucket : buckets_) {
    for (auto& elem : bucket) {
      size_t idx = hasher_(elem) % new_bucket_count;
      new_buckets[idx].push_back(std::move(elem));
    }
  }

  buckets_ = std::move(new_buckets);
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::check_load_factor() -> void {
  if (load_factor() >= max_load_factor_) {
    rehash(buckets_.size() * 2);
  }
}

//===---------------------------- ITERATOR METHODS -----------------------------===//

template <typename T, typename Hash>
auto HashSet<T, Hash>::iterator::operator*() const -> reference {
  return *list_it_;
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::iterator::operator->() const -> pointer {
  return &(*list_it_);
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::iterator::operator++() -> iterator& {
  ++list_it_;
  if (list_it_ == set_->buckets_[bucket_idx_].end()) {
    advance_to_next_bucket();
  }
  return *this;
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::iterator::operator++(int) -> iterator {
  iterator tmp = *this;
  ++(*this);
  return tmp;
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::iterator::operator==(const iterator& other) const -> bool {
  if (bucket_idx_ == set_->buckets_.size() && other.bucket_idx_ == other.set_->buckets_.size()) {
    return true;
  }
  return bucket_idx_ == other.bucket_idx_ && list_it_ == other.list_it_;
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::iterator::operator!=(const iterator& other) const -> bool {
  return !(*this == other);
}

template <typename T, typename Hash>
auto HashSet<T, Hash>::iterator::advance_to_next_bucket() -> void {
  ++bucket_idx_;
  while (bucket_idx_ < set_->buckets_.size() && set_->buckets_[bucket_idx_].empty()) {
    ++bucket_idx_;
  }
  if (bucket_idx_ < set_->buckets_.size()) {
    list_it_ = set_->buckets_[bucket_idx_].begin();
  }
}

} // namespace ads::associative

//===---------------------------------------------------------------------------===//
