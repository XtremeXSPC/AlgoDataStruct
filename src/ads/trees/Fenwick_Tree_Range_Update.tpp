//===---------------------------------------------------------------------------===//
/**
 * @file Fenwick_Tree_Range_Update.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the FenwickTreeRangeUpdate class.
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/trees/Fenwick_Tree_Range_Update.hpp"

namespace ads::trees {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <FenwickElement T>
FenwickTreeRangeUpdate<T>::FenwickTreeRangeUpdate() : tree_(), size_(0) {
}

template <FenwickElement T>
FenwickTreeRangeUpdate<T>::FenwickTreeRangeUpdate(size_t size) : tree_(), size_(0) {
  reset(size);
}

template <FenwickElement T>
FenwickTreeRangeUpdate<T>::FenwickTreeRangeUpdate(FenwickTreeRangeUpdate&& other) noexcept :
    tree_(std::move(other.tree_)),
    size_(other.size_) {
  other.size_ = 0;
}

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::operator=(FenwickTreeRangeUpdate&& other) noexcept -> FenwickTreeRangeUpdate<T>& {
  if (this != &other) {
    tree_       = std::move(other.tree_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===------------------------- MODIFICATION OPERATIONS -------------------------===//

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::reset(size_t size) -> void {
  size_ = size;
  tree_.assign(size_ + 1, T{});
}

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::clear() noexcept -> void {
  tree_.clear();
  size_ = 0;
}

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::range_add(size_t left, size_t right, const T& delta) -> void {
  validate_range(left, right);

  // Difference array technique: add delta at left, subtract at right+1.
  add_internal(left, delta);
  if (right + 1 < size_) {
    add_internal(right + 1, T{} - delta);
  }
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::point_query(size_t index) const -> T {
  validate_index(index);
  return prefix_sum_internal(index);
}

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::size() const noexcept -> size_t {
  return size_;
}

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <FenwickElement T>
constexpr auto FenwickTreeRangeUpdate<T>::lsb(size_t index) noexcept -> size_t {
  return index & (~index + 1);
}

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::add_internal(size_t index, const T& delta) -> void {
  size_t i = index + 1; // Convert to 1-based.
  while (i <= size_) {
    tree_[i] += delta;
    i += lsb(i);
  }
}

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::prefix_sum_internal(size_t index) const -> T {
  T      result{};
  size_t i = index + 1; // Convert to 1-based.
  while (i > 0) {
    result += tree_[i];
    i -= lsb(i);
  }
  return result;
}

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::validate_index(size_t index) const -> void {
  if (index >= size_) {
    throw FenwickTreeException("FenwickTreeRangeUpdate index out of range");
  }
}

template <FenwickElement T>
auto FenwickTreeRangeUpdate<T>::validate_range(size_t left, size_t right) const -> void {
  if (left > right) {
    throw FenwickTreeException("FenwickTreeRangeUpdate invalid range: left > right");
  }
  if (right >= size_) {
    throw FenwickTreeException("FenwickTreeRangeUpdate range out of bounds");
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
