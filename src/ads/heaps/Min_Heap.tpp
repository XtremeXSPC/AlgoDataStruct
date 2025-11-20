//===--------------------------------------------------------------------------===//
/**
 * @file Min_Heap.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the MinHeap class
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#include "../../../include/ads/heaps/Min_Heap.hpp"
#include <new>

namespace ads::heap {

//========== CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ==========//

template <typename T>
MinHeap<T>::MinHeap(size_t initial_capacity) :
    data_(static_cast<T*>(::operator new(initial_capacity * sizeof(T)))), size_(0), capacity_(initial_capacity) {
}

template <typename T>
MinHeap<T>::MinHeap(const std::vector<T>& elements) :
    data_(static_cast<T*>(::operator new(elements.size() * sizeof(T)))), size_(elements.size()), capacity_(elements.size()) {
  // Copy elements to the array using placement new
  for (size_t i = 0; i < size_; ++i) {
    new (&data_[i]) T(elements[i]);
  }

  // Build heap in O(n) time
  build_heap();
}

template <typename T>
MinHeap<T>::~MinHeap() {
  clear();
  ::operator delete(data_);
}

template <typename T>
MinHeap<T>::MinHeap(MinHeap&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
  other.data_     = nullptr;
  other.size_     = 0;
  other.capacity_ = 0;
}

template <typename T>
auto MinHeap<T>::operator=(MinHeap&& other) noexcept -> MinHeap& {
  if (this != &other) {
    clear();
    ::operator delete(data_);

    data_     = other.data_;
    size_     = other.size_;
    capacity_ = other.capacity_;

    other.data_     = nullptr;
    other.size_     = 0;
    other.capacity_ = 0;
  }
  return *this;
}

//========== INSERTION OPERATIONS ==========//

template <typename T>
auto MinHeap<T>::insert(const T& value) -> void {
  if (size_ == capacity_) {
    grow();
  }

  // Construct element at the end using placement new
  new (&data_[size_]) T(value);
  heapify_up(size_);
  ++size_;
}

template <typename T>
auto MinHeap<T>::insert(T&& value) -> void {
  if (size_ == capacity_) {
    grow();
  }

  // Construct element at the end using placement new (move version)
  new (&data_[size_]) T(std::move(value));
  heapify_up(size_);
  ++size_;
}

template <typename T>
template <typename... Args>
auto MinHeap<T>::emplace(Args&&... args) -> T& {
  if (size_ == capacity_) {
    grow();
  }

  // Construct element in-place using placement new
  new (&data_[size_]) T(std::forward<Args>(args)...);
  heapify_up(size_);
  T& ref = data_[size_];
  ++size_;
  return ref;
}

//========== REMOVAL OPERATIONS ==========//

template <typename T>
auto MinHeap<T>::extract_min() -> T {
  if (is_empty()) {
    throw HeapException("extract_min() called on empty MinHeap");
  }

  T min_value = std::move(data_[0]);

  // Move last element to root
  --size_;
  if (size_ > 0) {
    data_[0] = std::move(data_[size_]);
  }

  // Destroy the last element
  data_[size_].~T();

  // Restore heap property
  if (size_ > 0) {
    heapify_down(0);
  }

  return min_value;
}

//========== ACCESS OPERATIONS ==========//

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

//========== QUERY OPERATIONS ==========//

template <typename T>
auto MinHeap<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T>
auto MinHeap<T>::size() const noexcept -> size_t {
  return size_;
}

template <typename T>
auto MinHeap<T>::capacity() const noexcept -> size_t {
  return capacity_;
}

template <typename T>
auto MinHeap<T>::clear() noexcept -> void {
  // Explicitly destroy all elements
  for (size_t i = 0; i < size_; ++i) {
    data_[i].~T();
  }
  size_ = 0;
}

//========== ADVANCED OPERATIONS ==========//

template <typename T>
auto MinHeap<T>::decrease_key(size_t index, const T& new_value) -> void {
  if (index >= size_) {
    throw HeapException("decrease_key() called with invalid index");
  }

  if (new_value >= data_[index]) {
    throw HeapException("decrease_key() called with new_value >= current value");
  }

  data_[index] = new_value;
  heapify_up(index);
}

//========== PRIVATE HELPER METHODS ==========//

template <typename T>
auto MinHeap<T>::heapify_up(size_t index) -> void {
  while (index > 0) {
    size_t parent_idx = parent(index);

    if (data_[index] >= data_[parent_idx]) {
      break;
    }

    std::swap(data_[index], data_[parent_idx]);
    index = parent_idx;
  }
}

template <typename T>
auto MinHeap<T>::heapify_down(size_t index) -> void {
  while (true) {
    size_t left     = left_child(index);
    size_t right    = right_child(index);
    size_t smallest = index;

    if (left < size_ && data_[left] < data_[smallest]) {
      smallest = left;
    }

    if (right < size_ && data_[right] < data_[smallest]) {
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
  // Start from the first non-leaf node and heapify down
  // Last non-leaf node is at index (size/2 - 1)
  if (size_ <= 1) {
    return;
  }

  for (int i = static_cast<int>(size_ / 2) - 1; i >= 0; --i) {
    heapify_down(static_cast<size_t>(i));
  }
}

template <typename T>
auto MinHeap<T>::grow() -> void {
  size_t new_capacity = capacity_ * kGrowthFactor;

  // Allocate new array
  T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));

  // Move elements to new array
  for (size_t i = 0; i < size_; ++i) {
    new (&new_data[i]) T(std::move(data_[i]));
    data_[i].~T();
  }

  // Deallocate old array
  ::operator delete(data_);

  // Update pointers and capacity
  data_     = new_data;
  capacity_ = new_capacity;
}

} // namespace ads::heap

//===--------------------------------------------------------------------------===//
