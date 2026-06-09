//===---------------------------------------------------------------------------===//
/**
 * @file Circular_Array_Deque.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for CircularArrayDeque.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/queues/Circular_Array_Deque.hpp"

namespace ads::queues {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

template <QueueValue T>
CircularArrayDeque<T>::CircularArrayDeque(size_t initial_capacity) :
    data_(nullptr, &deallocate),
    front_(0),
    size_(0),
    capacity_(std::max(initial_capacity, kMinCapacity)) {
  // allocate() validates against capacity overflow before reserving storage.
  data_ = allocate(capacity_);
}

template <QueueValue T>
CircularArrayDeque<T>::~CircularArrayDeque() {
  clear();
}

template <QueueValue T>
CircularArrayDeque<T>::CircularArrayDeque(CircularArrayDeque&& other) noexcept :
    data_(std::move(other.data_)),
    front_(other.front_),
    size_(other.size_),
    capacity_(other.capacity_) {
  other.front_    = 0;
  other.size_     = 0;
  other.capacity_ = 0;
}

template <QueueValue T>
auto CircularArrayDeque<T>::operator=(CircularArrayDeque&& other) noexcept -> CircularArrayDeque<T>& {
  if (this != &other) {
    clear();
    data_           = std::move(other.data_);
    front_          = other.front_;
    size_           = other.size_;
    capacity_       = other.capacity_;
    other.front_    = 0;
    other.size_     = 0;
    other.capacity_ = 0;
  }
  return *this;
}

// The iterator and const_iterator types are aliases of IndexedIterator
// (see Indexed_Iterator.hpp); their operations need no out-of-line definitions.

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <QueueValue T>
template <typename... Args>
auto CircularArrayDeque<T>::emplace_front(Args&&... args) -> T& {
  ensure_capacity(size_ + 1);

  const size_t new_front   = prev_index(front_);
  T*           element_ptr = data_.get() + new_front;
  std::construct_at(element_ptr, std::forward<Args>(args)...);
  // Publish the new front only after construction so a throwing T leaves the deque unchanged.
  front_ = new_front;
  ++size_;
  return *element_ptr;
}

template <QueueValue T>
template <typename... Args>
auto CircularArrayDeque<T>::emplace_back(Args&&... args) -> T& {
  ensure_capacity(size_ + 1);

  // Calculate the index for the new back element.
  const size_t insert_index = index_from_front(size_);
  T*           element_ptr  = data_.get() + insert_index;
  std::construct_at(element_ptr, std::forward<Args>(args)...);
  ++size_;
  return *element_ptr;
}

template <QueueValue T>
auto CircularArrayDeque<T>::push_front(const T& value) -> void {
  emplace_front(value);
}

template <QueueValue T>
auto CircularArrayDeque<T>::push_front(T&& value) -> void {
  emplace_front(std::move(value));
}

template <QueueValue T>
auto CircularArrayDeque<T>::push_back(const T& value) -> void {
  emplace_back(value);
}

template <QueueValue T>
auto CircularArrayDeque<T>::push_back(T&& value) -> void {
  emplace_back(std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <QueueValue T>
auto CircularArrayDeque<T>::pop_front() -> void {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot pop_front from empty deque");
  }

  // Destroy the front element.
  std::destroy_at(data_.get() + front_);
  front_ = next_index(front_);
  --size_;

  // Reset front_ if deque is now empty.
  if (size_ == 0) {
    front_ = 0;
  }

  // Shrink capacity if underutilized.
  if (size_ > 0 && size_ * 4 <= capacity_ && capacity_ > kMinCapacity) {
    const size_t new_capacity = std::max(capacity_ / 2, kMinCapacity);
    try {
      reallocate(new_capacity);
    } catch (const std::bad_alloc&) {
    }
  }
}

template <QueueValue T>
auto CircularArrayDeque<T>::pop_back() -> void {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot pop_back from empty deque");
  }

  // Destroy the back element.
  const size_t back_index = index_from_front(size_ - 1);
  std::destroy_at(data_.get() + back_index);
  --size_;

  // Reset front_ if deque is now empty.
  if (size_ == 0) {
    front_ = 0;
  }

  // Shrink capacity if underutilized.
  if (size_ > 0 && size_ * 4 <= capacity_ && capacity_ > kMinCapacity) {
    const size_t new_capacity = std::max(capacity_ / 2, kMinCapacity);
    try {
      reallocate(new_capacity);
    } catch (const std::bad_alloc&) {
    }
  }
}

template <QueueValue T>
auto CircularArrayDeque<T>::clear() noexcept -> void {
  size_t current = front_;
  for (size_t i = 0; i < size_; ++i) {
    std::destroy_at(data_.get() + current);
    current = next_index(current);
  }
  size_  = 0;
  front_ = 0;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <QueueValue T>
auto CircularArrayDeque<T>::front() -> T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty deque");
  }
  return data_[front_];
}

template <QueueValue T>
auto CircularArrayDeque<T>::front() const -> const T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty deque");
  }
  return data_[front_];
}

template <QueueValue T>
auto CircularArrayDeque<T>::back() -> T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access back of empty deque");
  }
  return data_[index_from_front(size_ - 1)];
}

template <QueueValue T>
auto CircularArrayDeque<T>::back() const -> const T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access back of empty deque");
  }
  return data_[index_from_front(size_ - 1)];
}

template <QueueValue T>
auto CircularArrayDeque<T>::operator[](size_t index) -> T& {
  return element_at(index);
}

template <QueueValue T>
auto CircularArrayDeque<T>::operator[](size_t index) const -> const T& {
  return element_at(index);
}

template <QueueValue T>
auto CircularArrayDeque<T>::at(size_t index) -> T& {
  if (index >= size_) {
    throw QueueException("Index out of range");
  }
  return element_at(index);
}

template <QueueValue T>
auto CircularArrayDeque<T>::at(size_t index) const -> const T& {
  if (index >= size_) {
    throw QueueException("Index out of range");
  }
  return element_at(index);
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <QueueValue T>
auto CircularArrayDeque<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <QueueValue T>
auto CircularArrayDeque<T>::size() const noexcept -> size_t {
  return size_;
}

template <QueueValue T>
auto CircularArrayDeque<T>::capacity() const noexcept -> size_t {
  return capacity_;
}

//===--------------------------- CAPACITY OPERATIONS ---------------------------===//

template <QueueValue T>
auto CircularArrayDeque<T>::reserve(size_t new_capacity) -> void {
  if (new_capacity > capacity_) {
    reallocate(new_capacity);
  }
}

template <QueueValue T>
auto CircularArrayDeque<T>::shrink_to_fit() -> void {
  if (size_ < capacity_) {
    const size_t new_capacity = std::max(size_, kMinCapacity);
    if (new_capacity != capacity_) {
      reallocate(new_capacity);
    }
  }
}

//===------------------------- ITERATOR OPERATIONS -----------------------------===//

template <QueueValue T>
auto CircularArrayDeque<T>::begin() noexcept -> iterator {
  return iterator(0, this);
}

template <QueueValue T>
auto CircularArrayDeque<T>::end() noexcept -> iterator {
  return iterator(static_cast<difference_type>(size_), this);
}

template <QueueValue T>
auto CircularArrayDeque<T>::begin() const noexcept -> const_iterator {
  return const_iterator(0, this);
}

template <QueueValue T>
auto CircularArrayDeque<T>::end() const noexcept -> const_iterator {
  return const_iterator(static_cast<difference_type>(size_), this);
}

// cbegin/cend are inherited from ContainerFacade<CircularArrayDeque<T>>.

//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <QueueValue T>
auto CircularArrayDeque<T>::element_at(size_t index) -> T& {
  return data_[index_from_front(index)];
}

template <QueueValue T>
auto CircularArrayDeque<T>::element_at(size_t index) const -> const T& {
  return data_[index_from_front(index)];
}

template <QueueValue T>
auto CircularArrayDeque<T>::ensure_capacity(size_t min_capacity) -> void {
  if (min_capacity <= capacity_) {
    return;
  }

  // Calculate new capacity with growth factor.
  size_t new_capacity = std::max(capacity_, kMinCapacity);
  while (new_capacity < min_capacity) {
    if (new_capacity > std::numeric_limits<size_t>::max() / kGrowthFactor) {
      throw QueueOverflowException("Deque capacity overflow");
    }
    new_capacity = std::max(new_capacity * kGrowthFactor, kMinCapacity);
  }

  // Reallocate to new capacity.
  reallocate(new_capacity);
}

template <QueueValue T>
auto CircularArrayDeque<T>::allocate(size_t capacity) -> storage_ptr {
  if (capacity > max_elements()) {
    throw QueueOverflowException("Deque capacity overflow");
  }
  return storage_ptr(static_cast<T*>(::operator new[](capacity * sizeof(T))), &deallocate);
}

template <QueueValue T>
auto CircularArrayDeque<T>::reallocate(size_t new_capacity) -> void {
  if (new_capacity < size_) {
    new_capacity = size_;
  }

  // Allocate new storage (allocate() validates against capacity overflow).
  storage_ptr new_data = allocate(new_capacity);

  // Move existing elements to new storage.
  size_t constructed = 0;
  size_t current     = front_;

  try {
    // Move elements in logical order.
    for (; constructed < size_; ++constructed) {
      if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
        std::construct_at(new_data.get() + constructed, std::move(data_[current]));
      } else {
        std::construct_at(new_data.get() + constructed, data_[current]);
      }
      current = next_index(current);
    }
  } catch (...) {
    // The old deque remains authoritative, so only the partial replacement is rolled back.
    for (size_t i = 0; i < constructed; ++i) {
      std::destroy_at(new_data.get() + i);
    }
    throw;
  }

  // Destroy old elements only after all new elements are constructed.
  current = front_;
  for (size_t i = 0; i < size_; ++i) {
    std::destroy_at(data_.get() + current);
    current = next_index(current);
  }

  // Update internal state.
  data_     = std::move(new_data);
  capacity_ = new_capacity;
  front_    = 0;
}

} // namespace ads::queues

//===---------------------------------------------------------------------------===//
