//===---------------------------------------------------------------------------===//
/**
 * @file Priority_Queue.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the PriorityQueue class
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/queues/Priority_Queue.hpp"

namespace ads::queues {

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

template <typename T, typename Compare>
PriorityQueue<T, Compare>::PriorityQueue(Compare comp) : heap_(), comp_(comp) {
}

template <typename T, typename Compare>
PriorityQueue<T, Compare>::PriorityQueue(const std::vector<T>& elements, Compare comp) : heap_(elements), comp_(comp) {
  build_heap();
}

template <typename T, typename Compare>
PriorityQueue<T, Compare>::PriorityQueue(std::initializer_list<T> init, Compare comp) : heap_(init), comp_(comp) {
  build_heap();
}

template <typename T, typename Compare>
PriorityQueue<T, Compare>::PriorityQueue(PriorityQueue&& other) noexcept : heap_(std::move(other.heap_)), comp_(std::move(other.comp_)) {
}

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::operator=(PriorityQueue&& other) noexcept -> PriorityQueue& {
  if (this != &other) {
    heap_ = std::move(other.heap_);
    comp_ = std::move(other.comp_);
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::push(const T& value) -> void {
  heap_.push_back(value);
  heapify_up(heap_.size() - 1);
}

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::push(T&& value) -> void {
  heap_.push_back(std::move(value));
  heapify_up(heap_.size() - 1);
}

template <typename T, typename Compare>
template <typename... Args>
auto PriorityQueue<T, Compare>::emplace(Args&&... args) -> void {
  heap_.emplace_back(std::forward<Args>(args)...);
  heapify_up(heap_.size() - 1);
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::pop() -> void {
  if (empty()) {
    throw QueueException("Cannot pop from empty priority queue");
  }

  // Move last element to root.
  heap_[0] = std::move(heap_.back());
  heap_.pop_back();

  // Restore heap property if queue not empty.
  if (!empty()) {
    heapify_down(0);
  }
}

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::clear() noexcept -> void {
  heap_.clear();
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::top() -> T& {
  if (empty()) {
    throw QueueException("Cannot access top of empty priority queue");
  }
  return heap_[0];
}

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::top() const -> const T& {
  if (empty()) {
    throw QueueException("Cannot access top of empty priority queue");
  }
  return heap_[0];
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::empty() const noexcept -> bool {
  return heap_.empty();
}

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::size() const noexcept -> size_t {
  return heap_.size();
}

//===--------------------------- UTILITY OPERATIONS ----------------------------===//

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::reserve(size_t capacity) -> void {
  heap_.reserve(capacity);
}

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::sorted_elements() -> std::vector<T> {
  std::vector<T> result;
  result.reserve(heap_.size());

  // Extract elements one by one (they will be in sorted order).
  while (!empty()) {
    result.push_back(std::move(heap_[0]));
    pop();
  }

  return result;
}

//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::heapify_up(size_t index) -> void {
  while (index > 0) {
    size_t parent_index = parent(index);

    // If heap property is satisfied, we're done.
    if (!comp_(heap_[parent_index], heap_[index])) {
      break;
    }

    // Swap with parent and continue.
    std::swap(heap_[index], heap_[parent_index]);
    index = parent_index;
  }
}

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::heapify_down(size_t index) -> void {
  const size_t heap_size = heap_.size();

  while (true) {
    size_t largest     = index;
    size_t left_index  = left_child(index);
    size_t right_index = right_child(index);

    // Find the largest among node, left child, and right child.
    if (left_index < heap_size && comp_(heap_[largest], heap_[left_index])) {
      largest = left_index;
    }

    if (right_index < heap_size && comp_(heap_[largest], heap_[right_index])) {
      largest = right_index;
    }

    // If heap property is satisfied, we're done.
    if (largest == index) {
      break;
    }

    // Swap with largest child and continue.
    std::swap(heap_[index], heap_[largest]);
    index = largest;
  }
}

template <typename T, typename Compare>
auto PriorityQueue<T, Compare>::build_heap() -> void {
  // Bottom-up heapification for O(n) construction.
  if (heap_.size() <= 1) {
    return;
  }

  // Start from last non-leaf node and heapify down.
  for (int i = static_cast<int>(heap_.size() / 2) - 1; i >= 0; --i) {
    heapify_down(static_cast<size_t>(i));
  }
}

} // namespace ads::queue
//===---------------------------------------------------------------------------===//
