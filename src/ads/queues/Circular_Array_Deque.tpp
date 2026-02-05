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

template <typename T>
CircularArrayDeque<T>::CircularArrayDeque(size_t initial_capacity) :
    data_(static_cast<T*>(::operator new[](std::max(initial_capacity, kMinCapacity) * sizeof(T))),
          [](T* ptr) -> auto { ::operator delete[](ptr); }),
    front_(0),
    size_(0),
    capacity_(std::max(initial_capacity, kMinCapacity)) {
}

template <typename T>
CircularArrayDeque<T>::~CircularArrayDeque() {
  clear();
}

template <typename T>
CircularArrayDeque<T>::CircularArrayDeque(CircularArrayDeque&& other) noexcept :
    data_(std::move(other.data_)),
    front_(other.front_),
    size_(other.size_),
    capacity_(other.capacity_) {
  other.front_    = 0;
  other.size_     = 0;
  other.capacity_ = 0;
}

template <typename T>
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

//===---------------------------- ITERATOR METHODS -----------------------------===//

template <typename T>
auto CircularArrayDeque<T>::iterator::operator*() const -> reference {
  return deque_->element_at(index_);
}

template <typename T>
auto CircularArrayDeque<T>::iterator::operator->() const -> pointer {
  return std::addressof(deque_->element_at(index_));
}

template <typename T>
auto CircularArrayDeque<T>::iterator::operator++() -> iterator& {
  ++index_;
  return *this;
}

template <typename T>
auto CircularArrayDeque<T>::iterator::operator++(int) -> iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <typename T>
auto CircularArrayDeque<T>::const_iterator::operator*() const -> reference {
  return deque_->element_at(index_);
}

template <typename T>
auto CircularArrayDeque<T>::const_iterator::operator->() const -> pointer {
  return std::addressof(deque_->element_at(index_));
}

template <typename T>
auto CircularArrayDeque<T>::const_iterator::operator++() -> const_iterator& {
  ++index_;
  return *this;
}

template <typename T>
auto CircularArrayDeque<T>::const_iterator::operator++(int) -> const_iterator {
  const_iterator temp = *this;
  ++(*this);
  return temp;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <typename T>
template <typename... Args>
auto CircularArrayDeque<T>::emplace_front(Args&&... args) -> T& {
  ensure_capacity(size_ + 1);

  // Move front index backward.
  front_         = prev_index(front_);
  T* element_ptr = data_.get() + front_;
  new (element_ptr) T(std::forward<Args>(args)...);
  ++size_;
  return *element_ptr;
}

template <typename T>
template <typename... Args>
auto CircularArrayDeque<T>::emplace_back(Args&&... args) -> T& {
  ensure_capacity(size_ + 1);

  // Calculate the index for the new back element.
  const size_t insert_index = index_from_front(size_);
  T*           element_ptr  = data_.get() + insert_index;
  new (element_ptr) T(std::forward<Args>(args)...);
  ++size_;
  return *element_ptr;
}

template <typename T>
auto CircularArrayDeque<T>::push_front(const T& value) -> void {
  emplace_front(value);
}

template <typename T>
auto CircularArrayDeque<T>::push_front(T&& value) -> void {
  emplace_front(std::move(value));
}

template <typename T>
auto CircularArrayDeque<T>::push_back(const T& value) -> void {
  emplace_back(value);
}

template <typename T>
auto CircularArrayDeque<T>::push_back(T&& value) -> void {
  emplace_back(std::move(value));
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <typename T>
auto CircularArrayDeque<T>::pop_front() -> void {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot pop_front from empty deque");
  }

  // Destroy the front element.
  data_[front_].~T();
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

template <typename T>
auto CircularArrayDeque<T>::pop_back() -> void {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot pop_back from empty deque");
  }

  // Destroy the back element.
  const size_t back_index = index_from_front(size_ - 1);
  data_[back_index].~T();
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

template <typename T>
auto CircularArrayDeque<T>::clear() noexcept -> void {
  size_t current = front_;
  for (size_t i = 0; i < size_; ++i) {
    data_[current].~T();
    current = next_index(current);
  }
  size_  = 0;
  front_ = 0;
}

//===---------------------------- ACCESS OPERATIONS ----------------------------===//

template <typename T>
auto CircularArrayDeque<T>::front() -> T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty deque");
  }
  return data_[front_];
}

template <typename T>
auto CircularArrayDeque<T>::front() const -> const T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access front of empty deque");
  }
  return data_[front_];
}

template <typename T>
auto CircularArrayDeque<T>::back() -> T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access back of empty deque");
  }
  return data_[index_from_front(size_ - 1)];
}

template <typename T>
auto CircularArrayDeque<T>::back() const -> const T& {
  if (is_empty()) {
    throw QueueUnderflowException("Cannot access back of empty deque");
  }
  return data_[index_from_front(size_ - 1)];
}

template <typename T>
auto CircularArrayDeque<T>::operator[](size_t index) -> T& {
  return element_at(index);
}

template <typename T>
auto CircularArrayDeque<T>::operator[](size_t index) const -> const T& {
  return element_at(index);
}

template <typename T>
auto CircularArrayDeque<T>::at(size_t index) -> T& {
  if (index >= size_) {
    throw QueueException("Index out of range");
  }
  return element_at(index);
}

template <typename T>
auto CircularArrayDeque<T>::at(size_t index) const -> const T& {
  if (index >= size_) {
    throw QueueException("Index out of range");
  }
  return element_at(index);
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename T>
auto CircularArrayDeque<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename T>
auto CircularArrayDeque<T>::size() const noexcept -> size_t {
  return size_;
}

template <typename T>
auto CircularArrayDeque<T>::capacity() const noexcept -> size_t {
  return capacity_;
}

//===--------------------------- CAPACITY OPERATIONS ---------------------------===//

template <typename T>
auto CircularArrayDeque<T>::reserve(size_t new_capacity) -> void {
  if (new_capacity > capacity_) {
    reallocate(new_capacity);
  }
}

template <typename T>
auto CircularArrayDeque<T>::shrink_to_fit() -> void {
  if (size_ < capacity_) {
    const size_t new_capacity = std::max(size_, kMinCapacity);
    if (new_capacity != capacity_) {
      reallocate(new_capacity);
    }
  }
}

//===------------------------- ITERATOR OPERATIONS -----------------------------===//

template <typename T>
auto CircularArrayDeque<T>::begin() noexcept -> iterator {
  return iterator(0, this);
}

template <typename T>
auto CircularArrayDeque<T>::end() noexcept -> iterator {
  return iterator(size_, this);
}

template <typename T>
auto CircularArrayDeque<T>::begin() const noexcept -> const_iterator {
  return const_iterator(0, this);
}

template <typename T>
auto CircularArrayDeque<T>::end() const noexcept -> const_iterator {
  return const_iterator(size_, this);
}

template <typename T>
auto CircularArrayDeque<T>::cbegin() const noexcept -> const_iterator {
  return const_iterator(0, this);
}

template <typename T>
auto CircularArrayDeque<T>::cend() const noexcept -> const_iterator {
  return const_iterator(size_, this);
}

//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename T>
auto CircularArrayDeque<T>::element_at(size_t index) -> T& {
  return data_[index_from_front(index)];
}

template <typename T>
auto CircularArrayDeque<T>::element_at(size_t index) const -> const T& {
  return data_[index_from_front(index)];
}

template <typename T>
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

template <typename T>
auto CircularArrayDeque<T>::reallocate(size_t new_capacity) -> void {
  std::unique_ptr<T[], void (*)(T*)> new_data(static_cast<T*>(::operator new[](new_capacity * sizeof(T))),
                                              [](T* ptr) -> auto { ::operator delete[](ptr); });

  // Move existing elements to new storage.
  size_t constructed = 0;
  size_t current     = front_;

  try {
    // Move elements in logical order.
    for (; constructed < size_; ++constructed) {
      if constexpr (std::is_nothrow_move_constructible_v<T>) {
        new (new_data.get() + constructed) T(std::move(data_[current]));
      } else {
        new (new_data.get() + constructed) T(data_[current]);
      }
      current = next_index(current);
    }
  } catch (...) {
    // Rollback: destroy any constructed elements in new_data.
    for (size_t i = 0; i < constructed; ++i) {
      new_data[i].~T();
    }
    throw;
  }

  // Destroy old elements only after all new elements are constructed.
  current = front_;
  for (size_t i = 0; i < size_; ++i) {
    data_[current].~T();
    current = next_index(current);
  }

  // Update internal state.
  data_     = std::move(new_data);
  capacity_ = new_capacity;
  front_    = 0;
}

} // namespace ads::queues

//===---------------------------------------------------------------------------===//
