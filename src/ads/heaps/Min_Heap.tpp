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

namespace ads::heaps {

// DynamicArray keeps heap storage contiguous while centralizing RAII ownership.

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
MinHeap<T>::MinHeap(size_t initial_capacity) : data_(initial_capacity) {
}

template <typename T>
MinHeap<T>::MinHeap(const std::vector<T>& elements) : data_(elements.begin(), elements.end()) {
  build_heap();
}

template <typename T>
template <std::input_iterator InputIt>
MinHeap<T>::MinHeap(InputIt first, InputIt last)
  requires std::constructible_from<T, std::iter_reference_t<InputIt>>
    : data_(first, last) {
  build_heap();
}

template <typename T>
MinHeap<T>::MinHeap(MinHeap&& other) noexcept = default;

template <typename T>
MinHeap<T>::~MinHeap() = default;

template <typename T>
auto MinHeap<T>::operator=(MinHeap&& other) noexcept -> MinHeap& {
  if (this != &other) {
    data_ = std::move(other.data_);
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename T>
auto MinHeap<T>::insert(const T& value) -> void {
  data_.push_back(value);
  heapify_up(data_.size() - 1);
}

template <typename T>
auto MinHeap<T>::insert(T&& value) -> void {
  data_.push_back(std::move(value));
  heapify_up(data_.size() - 1);
}

template <typename T>
template <typename... Args>
auto MinHeap<T>::emplace(Args&&... args) -> T& {
  data_.emplace_back(std::forward<Args>(args)...);
  const size_t final_index = heapify_up(data_.size() - 1);
  return data_[final_index];
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto MinHeap<T>::top() -> T& {
  if (is_empty()) {
    throw HeapException("top() called on empty MinHeap");
  }
  return data_[0];
}

template <typename T>
auto MinHeap<T>::top() const -> const T& {
  if (is_empty()) {
    throw HeapException("top() called on empty MinHeap");
  }
  return data_[0];
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
auto MinHeap<T>::extract_min() -> T {
  if (is_empty()) {
    throw HeapException("extract_min() called on empty MinHeap");
  }

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

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto MinHeap<T>::is_empty() const noexcept -> bool {
  return data_.is_empty();
}

template <typename T>
auto MinHeap<T>::size() const noexcept -> size_t {
  return data_.size();
}

template <typename T>
auto MinHeap<T>::capacity() const noexcept -> size_t {
  return data_.capacity();
}

template <typename T>
auto MinHeap<T>::clear() noexcept -> void {
  data_.clear();
}

//===--------------------------- ADVANCED OPERATIONS ---------------------------===//

template <typename T>
auto MinHeap<T>::decrease_key(size_t index, const T& new_value) -> void {
  if (index >= data_.size()) {
    throw HeapException("decrease_key() called with invalid index");
  }

  if (new_value >= data_[index]) {
    throw HeapException("decrease_key() called with new_value >= current value");
  }

  data_[index] = new_value;
  heapify_up(index);
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T>
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

template <typename T>
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

template <typename T>
auto MinHeap<T>::build_heap() -> void {
  if (data_.size() <= 1) {
    return;
  }

  // Counting down with size_t avoids signed overflow on large heaps.
  for (size_t i = data_.size() / 2; i > 0; --i) {
    heapify_down(i - 1);
  }
}

} // namespace ads::heaps

//===---------------------------------------------------------------------------===//
