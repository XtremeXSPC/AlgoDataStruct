//===---------------------------------------------------------------------------===//
/**
 * @file Max_Heap.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the MaxHeap class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/heaps/Max_Heap.hpp"

namespace ads::heaps {

// DynamicArray keeps heap storage contiguous while centralizing RAII ownership.

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <typename T>
MaxHeap<T>::MaxHeap(size_t initial_capacity) : data_(initial_capacity) {
}

template <typename T>
MaxHeap<T>::MaxHeap(const std::vector<T>& elements) : data_(elements.begin(), elements.end()) {
  build_heap();
}

template <typename T>
template <std::input_iterator InputIt>
MaxHeap<T>::MaxHeap(InputIt first, InputIt last)
  requires std::constructible_from<T, std::iter_reference_t<InputIt>>
    : data_(first, last) {
  build_heap();
}

template <typename T>
MaxHeap<T>::MaxHeap(MaxHeap&& other) noexcept = default;

template <typename T>
MaxHeap<T>::~MaxHeap() = default;

template <typename T>
auto MaxHeap<T>::operator=(MaxHeap&& other) noexcept -> MaxHeap& {
  if (this != &other) {
    data_ = std::move(other.data_);
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename T>
auto MaxHeap<T>::insert(const T& value) -> void {
  data_.push_back(value);
  heapify_up(data_.size() - 1);
}

template <typename T>
auto MaxHeap<T>::insert(T&& value) -> void {
  data_.push_back(std::move(value));
  heapify_up(data_.size() - 1);
}

template <typename T>
template <typename... Args>
auto MaxHeap<T>::emplace(Args&&... args) -> T& {
  data_.emplace_back(std::forward<Args>(args)...);
  const size_t final_index = heapify_up(data_.size() - 1);
  return data_[final_index];
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto MaxHeap<T>::top() -> T& {
  if (is_empty()) {
    throw HeapException("top() called on empty MaxHeap");
  }
  return data_[0];
}

template <typename T>
auto MaxHeap<T>::top() const -> const T& {
  if (is_empty()) {
    throw HeapException("top() called on empty MaxHeap");
  }
  return data_[0];
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
auto MaxHeap<T>::extract_max() -> T {
  if (is_empty()) {
    throw HeapException("extract_max() called on empty MaxHeap");
  }

  T max_value = std::move(data_[0]);

  if (data_.size() == 1) {
    // Removing directly avoids self-move assignment for move-sensitive types.
    data_.pop_back();
    return max_value;
  }

  data_[0] = std::move(data_.back());
  data_.pop_back();

  heapify_down(0);

  return max_value;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto MaxHeap<T>::is_empty() const noexcept -> bool {
  return data_.is_empty();
}

template <typename T>
auto MaxHeap<T>::size() const noexcept -> size_t {
  return data_.size();
}

template <typename T>
auto MaxHeap<T>::capacity() const noexcept -> size_t {
  return data_.capacity();
}

template <typename T>
auto MaxHeap<T>::clear() noexcept -> void {
  data_.clear();
}

//===--------------------------- ADVANCED OPERATIONS ---------------------------===//

template <typename T>
auto MaxHeap<T>::increase_key(size_t index, const T& new_value) -> void {
  if (index >= data_.size()) {
    throw HeapException("increase_key() called with invalid index");
  }

  if (new_value <= data_[index]) {
    throw HeapException("increase_key() called with new_value <= current value");
  }

  data_[index] = new_value;
  heapify_up(index);
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T>
auto MaxHeap<T>::heapify_up(size_t index) -> size_t {
  while (index > 0) {
    size_t parent_idx = parent(index);

    // Max heap: parent should be >= children.
    if (data_[index] <= data_[parent_idx]) {
      break;
    }

    std::swap(data_[index], data_[parent_idx]);
    index = parent_idx;
  }
  return index;
}

template <typename T>
auto MaxHeap<T>::heapify_down(size_t index) -> void {
  const size_t heap_size = data_.size();

  while (true) {
    size_t left    = left_child(index);
    size_t right   = right_child(index);
    size_t largest = index;

    // Max heap: find the largest among node and its children.
    if (left < heap_size && data_[left] > data_[largest]) {
      largest = left;
    }

    if (right < heap_size && data_[right] > data_[largest]) {
      largest = right;
    }

    if (largest == index) {
      break;
    }

    std::swap(data_[index], data_[largest]);
    index = largest;
  }
}

template <typename T>
auto MaxHeap<T>::build_heap() -> void {
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
