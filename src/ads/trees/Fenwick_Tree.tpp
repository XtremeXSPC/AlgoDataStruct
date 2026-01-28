//===---------------------------------------------------------------------------===//
/**
 * @file Fenwick_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the FenwickTree class.
 * @version 0.1
 * @date 2026-01-28
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/trees/Fenwick_Tree.hpp"

namespace ads::trees {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
FenwickTree<T>::FenwickTree() : values_(), tree_(), size_(0) {
}

template <typename T>
FenwickTree<T>::FenwickTree(size_t size) : values_(), tree_(), size_(0) {
  reset(size);
}

template <typename T>
FenwickTree<T>::FenwickTree(size_t size, const T& value) : values_(size, value), tree_(), size_(size) {
  build_tree();
}

template <typename T>
FenwickTree<T>::FenwickTree(const std::vector<T>& values) : values_(), tree_(), size_(0) {
  build(values);
}

template <typename T>
FenwickTree<T>::FenwickTree(std::initializer_list<T> values) : values_(), tree_(), size_(0) {
  build(values);
}

template <typename T>
FenwickTree<T>::FenwickTree(FenwickTree&& other) noexcept :
    values_(std::move(other.values_)), tree_(std::move(other.tree_)), size_(other.size_) {
  other.size_ = 0;
}

template <typename T>
auto FenwickTree<T>::operator=(FenwickTree&& other) noexcept -> FenwickTree<T>& {
  if (this != &other) {
    values_     = std::move(other.values_);
    tree_       = std::move(other.tree_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===------------------------- MODIFICATION OPERATIONS --------------------------===//

template <typename T>
auto FenwickTree<T>::reset(size_t size) -> void {
  values_.assign(size, T{});
  size_ = size;
  tree_.assign(size_ + 1, T{});
}

template <typename T>
auto FenwickTree<T>::build(const std::vector<T>& values) -> void {
  values_ = values;
  size_   = values_.size();
  build_tree();
}

template <typename T>
auto FenwickTree<T>::build(std::initializer_list<T> values) -> void {
  values_.assign(values.begin(), values.end());
  size_ = values_.size();
  build_tree();
}

template <typename T>
auto FenwickTree<T>::clear() noexcept -> void {
  values_.clear();
  tree_.clear();
  size_ = 0;
}

template <typename T>
auto FenwickTree<T>::add(size_t index, const T& delta) -> void {
  validate_index(index);
  values_[index] += delta;

  size_t i = index + 1;
  while (i <= size_) {
    tree_[i] += delta;
    i += lsb(i);
  }
}

template <typename T>
auto FenwickTree<T>::set(size_t index, const T& value) -> void {
  validate_index(index);
  const T delta = value - values_[index];
  add(index, delta);
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto FenwickTree<T>::prefix_sum(size_t index) const -> T {
  validate_index(index);

  T      result{};
  size_t i = index + 1;
  while (i > 0) {
    result += tree_[i];
    i -= lsb(i);
  }
  return result;
}

template <typename T>
auto FenwickTree<T>::range_sum(size_t left, size_t right) const -> T {
  validate_range(left, right);
  if (left == 0) {
    return prefix_sum(right);
  }
  return prefix_sum(right) - prefix_sum(left - 1);
}

template <typename T>
auto FenwickTree<T>::total_sum() const -> T {
  if (is_empty()) {
    return T{};
  }
  return prefix_sum(size_ - 1);
}

template <typename T>
auto FenwickTree<T>::value_at(size_t index) const -> const T& {
  validate_index(index);
  return values_[index];
}

template <typename T>
auto FenwickTree<T>::size() const noexcept -> size_t {
  return size_;
}

template <typename T>
auto FenwickTree<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T>
constexpr auto FenwickTree<T>::lsb(size_t index) noexcept -> size_t {
  return index & (~index + 1);
}

template <typename T>
auto FenwickTree<T>::build_tree() -> void {
  tree_.assign(size_ + 1, T{});

  for (size_t i = 0; i < size_; ++i) {
    const size_t tree_index = i + 1;
    tree_[tree_index] += values_[i];

    const size_t parent = tree_index + lsb(tree_index);
    if (parent <= size_) {
      tree_[parent] += tree_[tree_index];
    }
  }
}

template <typename T>
auto FenwickTree<T>::validate_index(size_t index) const -> void {
  if (index >= size_) {
    throw FenwickTreeException("FenwickTree index out of range");
  }
}

template <typename T>
auto FenwickTree<T>::validate_range(size_t left, size_t right) const -> void {
  if (left > right) {
    throw FenwickTreeException("FenwickTree invalid range: left > right");
  }
  if (right >= size_) {
    throw FenwickTreeException("FenwickTree range out of bounds");
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
