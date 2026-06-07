//===---------------------------------------------------------------------------===//
/**
 * @file D_Ary_Heap.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the DAryHeap class.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/heaps/D_Ary_Heap.hpp"

#include <string>

namespace ads::heaps {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <HeapValue T, typename Compare>
DAryHeap<T, Compare>::DAryHeap(size_t arity, size_t initial_capacity, Compare comp) :
    data_(initial_capacity),
    arity_(validate_arity(arity)),
    comp_(std::move(comp)) {
}

template <HeapValue T, typename Compare>
DAryHeap<T, Compare>::DAryHeap(const std::vector<T>& elements, size_t arity, Compare comp) :
    data_(elements.begin(), elements.end()),
    arity_(validate_arity(arity)),
    comp_(std::move(comp)) {
  build_heap();
}

template <HeapValue T, typename Compare>
template <std::input_iterator InputIt>
DAryHeap<T, Compare>::DAryHeap(InputIt first, InputIt last, size_t arity, Compare comp) requires HeapRangeValue<InputIt, T>
    : data_(first, last), arity_(validate_arity(arity)), comp_(std::move(comp)) {
  build_heap();
}

template <HeapValue T, typename Compare>
DAryHeap<T, Compare>::DAryHeap(std::initializer_list<T> init, size_t arity, Compare comp) :
    data_(init),
    arity_(validate_arity(arity)),
    comp_(std::move(comp)) {
  build_heap();
}

template <HeapValue T, typename Compare>
DAryHeap<T, Compare>::DAryHeap(DAryHeap&& other) noexcept(std::is_nothrow_move_constructible_v<Compare>) = default;

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::operator=(DAryHeap&& other) noexcept(std::is_nothrow_move_assignable_v<Compare>) -> DAryHeap& {
  if (this != &other) {
    data_  = std::move(other.data_);
    arity_ = other.arity_;
    comp_  = std::move(other.comp_);
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::insert(const T& value) -> void {
  data_.push_back(value);
  heapify_up(data_.size() - 1);
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::insert(T&& value) -> void {
  data_.push_back(std::move(value));
  heapify_up(data_.size() - 1);
}

template <HeapValue T, typename Compare>
template <typename... Args>
auto DAryHeap<T, Compare>::emplace(Args&&... args) -> T& {
  data_.emplace_back(std::forward<Args>(args)...);
  const size_t final_index = heapify_up(data_.size() - 1);
  return data_[final_index];
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::top() -> T& {
  validate_non_empty("top()");
  return data_[0];
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::top() const -> const T& {
  validate_non_empty("top()");
  return data_[0];
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::extract_top() -> T {
  validate_non_empty("extract_top()");

  T top_value = std::move(data_[0]);
  if (data_.size() == 1) {
    data_.pop_back();
    return top_value;
  }

  data_[0] = std::move(data_.back());
  data_.pop_back();
  heapify_down(0);
  return top_value;
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::update_key(size_t index, const T& new_value) -> void {
  validate_index(index, "update_key()");

  data_[index] = new_value;
  if (index > 0 && has_higher_priority(data_[index], data_[parent(index)])) {
    heapify_up(index);
  } else {
    heapify_down(index);
  }
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::update_key(size_t index, T&& new_value) -> void {
  validate_index(index, "update_key()");

  data_[index] = std::move(new_value);
  if (index > 0 && has_higher_priority(data_[index], data_[parent(index)])) {
    heapify_up(index);
  } else {
    heapify_down(index);
  }
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::clear() noexcept -> void {
  data_.clear();
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::is_empty() const noexcept -> bool {
  return data_.is_empty();
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::size() const noexcept -> size_t {
  return data_.size();
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::capacity() const noexcept -> size_t {
  return data_.capacity();
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::arity() const noexcept -> size_t {
  return arity_;
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::reserve(size_t new_capacity) -> void {
  data_.reserve(new_capacity);
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::has_higher_priority(const T& lhs, const T& rhs) const -> bool {
  return comp_(rhs, lhs);
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::parent(size_t index) const noexcept -> size_t {
  return (index - 1) / arity_;
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::first_child(size_t index) const noexcept -> size_t {
  return arity_ * index + 1;
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::heapify_up(size_t index) -> size_t {
  while (index > 0) {
    const size_t parent_index = parent(index);
    if (!has_higher_priority(data_[index], data_[parent_index])) {
      break;
    }

    std::swap(data_[index], data_[parent_index]);
    index = parent_index;
  }

  return index;
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::heapify_down(size_t index) -> void {
  const size_t heap_size = data_.size();

  while (true) {
    size_t       best_index = index;
    const size_t begin      = first_child(index);
    const size_t end        = std::min(begin + arity_, heap_size);

    for (size_t child_index = begin; child_index < end; ++child_index) {
      if (has_higher_priority(data_[child_index], data_[best_index])) {
        best_index = child_index;
      }
    }

    if (best_index == index) {
      break;
    }

    std::swap(data_[index], data_[best_index]);
    index = best_index;
  }
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::build_heap() -> void {
  if (data_.size() <= 1) {
    return;
  }

  for (size_t index = (data_.size() - 2) / arity_ + 1; index > 0; --index) {
    heapify_down(index - 1);
  }
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::validate_non_empty(const char* operation) const -> void {
  if (is_empty()) {
    throw HeapException(std::string(operation) + " called on empty DAryHeap");
  }
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::validate_index(size_t index, const char* operation) const -> void {
  if (index >= data_.size()) {
    throw HeapException(std::string(operation) + " called with invalid index");
  }
}

template <HeapValue T, typename Compare>
auto DAryHeap<T, Compare>::validate_arity(size_t arity) -> size_t {
  if (arity < 2) {
    throw HeapException("DAryHeap arity must be at least 2");
  }
  return arity;
}

} // namespace ads::heaps

//===---------------------------------------------------------------------------===//
