//===---------------------------------------------------------------------------===//
/**
 * @file Min_Heap.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the MinHeap class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/heaps/Min_Heap.hpp"

#include <string>

namespace ads::heaps {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <OrderedHeapValue T>
MinHeap<T>::MinHeap(size_type initial_capacity) : data_(initial_capacity) {
}

template <OrderedHeapValue T>
MinHeap<T>::MinHeap(const std::vector<T>& elements) : data_(elements.begin(), elements.end()) {
  build_heap();
}

template <OrderedHeapValue T>
template <std::input_iterator InputIt>
MinHeap<T>::MinHeap(InputIt first, InputIt last) requires HeapRangeValue<InputIt, T>
    : data_(first, last) {
  build_heap();
}

template <OrderedHeapValue T>
MinHeap<T>::MinHeap(std::initializer_list<T> values) : data_(values) {
  build_heap();
}

template <OrderedHeapValue T>
MinHeap<T>::MinHeap(MinHeap&& other) noexcept = default;

template <OrderedHeapValue T>
MinHeap<T>::~MinHeap() = default;

template <OrderedHeapValue T>
auto MinHeap<T>::operator=(MinHeap&& other) noexcept -> MinHeap& {
  if (this != &other) {
    data_ = std::move(other.data_);
  }
  return *this;
}

//===----- INSERTION OPERATIONS ------------------------------------------------===//

template <OrderedHeapValue T>
auto MinHeap<T>::insert(const T& value) -> void requires CopyHeapValue<T>
{
  data_.push_back(value);
  heapify_up(data_.size() - 1);
}

template <OrderedHeapValue T>
auto MinHeap<T>::insert(T&& value) -> void requires MoveHeapValue<T>
{
  data_.push_back(std::move(value));
  heapify_up(data_.size() - 1);
}

template <OrderedHeapValue T>
template <typename... Args>
auto MinHeap<T>::emplace(Args&&... args) -> T& requires EmplaceHeapValue<T, Args...>
{
  data_.emplace_back(std::forward<Args>(args)...);
  const size_t final_index = heapify_up(data_.size() - 1);
  return data_[final_index];
}

//===----- ACCESS OPERATIONS ---------------------------------------------------===//

template <OrderedHeapValue T>
auto MinHeap<T>::top() -> T& {
  validate_non_empty("top()");
  return data_[0];
}

template <OrderedHeapValue T>
auto MinHeap<T>::top() const -> const T& {
  validate_non_empty("top()");
  return data_[0];
}

//===----- REMOVAL OPERATIONS --------------------------------------------------===//

template <OrderedHeapValue T>
auto MinHeap<T>::extract_min() -> T {
  validate_non_empty("extract_min()");

  T min_value = std::move(data_[0]);

  if (data_.size() == 1) {
    // Removing directly avoids self-move assignment for move-sensitive types.
    data_.pop_back();
    return min_value;
  }

  data_[0] = std::move(data_.back());
  data_.pop_back();

  heapify_down(0);

  return min_value;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <OrderedHeapValue T>
auto MinHeap<T>::is_empty() const noexcept -> bool {
  return data_.is_empty();
}

template <OrderedHeapValue T>
auto MinHeap<T>::size() const noexcept -> size_type {
  return data_.size();
}

template <OrderedHeapValue T>
auto MinHeap<T>::capacity() const noexcept -> size_type {
  return data_.capacity();
}

template <OrderedHeapValue T>
auto MinHeap<T>::reserve(size_type new_capacity) -> void {
  data_.reserve(new_capacity);
}

template <OrderedHeapValue T>
auto MinHeap<T>::clear() noexcept -> void {
  data_.clear();
}

//===----- ADVANCED OPERATIONS -------------------------------------------------===//

template <OrderedHeapValue T>
auto MinHeap<T>::decrease_key(size_type index, const T& new_value) -> void {
  validate_index(index, "decrease_key()");

  if (new_value >= data_[index]) {
    throw HeapException("decrease_key() called with new_value >= current value");
  }

  data_[index] = new_value;
  heapify_up(index);
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <OrderedHeapValue T>
auto MinHeap<T>::heapify_up(size_t index) -> size_t {
  while (index > 0) {
    size_t parent_idx = parent(index);

    if (data_[index] >= data_[parent_idx]) {
      break;
    }

    std::swap(data_[index], data_[parent_idx]);
    index = parent_idx;
  }
  return index;
}

template <OrderedHeapValue T>
auto MinHeap<T>::heapify_down(size_t index) -> void {
  const size_t heap_size = data_.size();

  while (true) {
    size_t left     = left_child(index);
    size_t right    = right_child(index);
    size_t smallest = index;

    if (left < heap_size && data_[left] < data_[smallest]) {
      smallest = left;
    }

    if (right < heap_size && data_[right] < data_[smallest]) {
      smallest = right;
    }

    if (smallest == index) {
      break;
    }

    std::swap(data_[index], data_[smallest]);
    index = smallest;
  }
}

template <OrderedHeapValue T>
auto MinHeap<T>::build_heap() -> void {
  if (data_.size() <= 1) {
    return;
  }

  // Counting down with size_t avoids signed overflow on large heaps.
  for (size_t i = data_.size() / 2; i > 0; --i) {
    heapify_down(i - 1);
  }
}

template <OrderedHeapValue T>
auto MinHeap<T>::validate_non_empty(const char* operation) const -> void {
  if (is_empty()) {
    throw HeapException(std::string(operation) + " called on empty MinHeap");
  }
}

template <OrderedHeapValue T>
auto MinHeap<T>::validate_index(size_type index, const char* operation) const -> void {
  if (index >= data_.size()) {
    throw HeapException(std::string(operation) + " called with invalid index");
  }
}

} // namespace ads::heaps

//===---------------------------------------------------------------------------===//
