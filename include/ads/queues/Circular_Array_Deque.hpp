//===---------------------------------------------------------------------------===//
/**
 * @file Circular_Array_Deque.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the CircularArrayDeque class.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef CIRCULAR_ARRAY_DEQUE_HPP
#define CIRCULAR_ARRAY_DEQUE_HPP

#include <algorithm>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include "Queue_Exception.hpp"

namespace ads::queues {

/**
 * @brief A deque implementation based on a circular dynamic array.
 *
 * @details This class implements a double-ended queue that supports
 *          efficient insertion and removal at both ends. The internal
 *          storage is a circular buffer that grows dynamically as needed.
 *
 * @tparam T The type of data to store in the deque.
 */
template <typename T>
class CircularArrayDeque {
public:
  //===---------------------------- ITERATOR CLASS -----------------------------===//
  /**
   * @brief Forward iterator for CircularArrayDeque.
   */
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    explicit iterator(size_t index = 0, CircularArrayDeque<T>* deque = nullptr) : index_(index), deque_(deque) {}

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> iterator&;
    auto operator++(int) -> iterator;
    auto operator==(const iterator& other) const -> bool { return index_ == other.index_ && deque_ == other.deque_; }

  private:
    size_t                 index_;
    CircularArrayDeque<T>* deque_;
    friend class CircularArrayDeque<T>;
  };

  //===------------------------- CONST_ITERATOR CLASS --------------------------===//
  /**
   * @brief Const forward iterator for CircularArrayDeque.
   */
  class const_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    explicit const_iterator(size_t index = 0, const CircularArrayDeque<T>* deque = nullptr) : index_(index), deque_(deque) {}
    explicit const_iterator(const iterator& it) : index_(it.index_), deque_(it.deque_) {}

    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator++() -> const_iterator&;
    auto operator++(int) -> const_iterator;
    auto operator==(const const_iterator& other) const -> bool { return index_ == other.index_ && deque_ == other.deque_; }

  private:
    size_t                       index_;
    const CircularArrayDeque<T>* deque_;
    friend class CircularArrayDeque<T>;
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty deque with optional initial capacity.
   * @param initial_capacity The initial capacity (default: 16).
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(1), Space O(initial_capacity)
   */
  explicit CircularArrayDeque(size_t initial_capacity = 16);

  /**
   * @brief Destructor. Empties the deque and deallocates all elements.
   * @complexity Time O(n), Space O(1)
   */
  ~CircularArrayDeque();

  /**
   * @brief Move constructor.
   * @param other The deque from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  CircularArrayDeque(CircularArrayDeque&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The deque from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(CircularArrayDeque&& other) noexcept -> CircularArrayDeque&;

  // Copy constructor and assignment are disabled (move-only type).
  CircularArrayDeque(const CircularArrayDeque&)                    = delete;
  auto operator=(const CircularArrayDeque&) -> CircularArrayDeque& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Constructs an element in-place at the front of the deque.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace_front(Args&&... args) -> T&;

  /**
   * @brief Constructs an element in-place at the back of the deque.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace_back(Args&&... args) -> T&;

  /**
   * @brief Inserts an element at the front of the deque (copy).
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_front(const T& value) -> void;

  /**
   * @brief Inserts an element at the front of the deque (move).
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_front(T&& value) -> void;

  /**
   * @brief Inserts an element at the back of the deque (copy).
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_back(const T& value) -> void;

  /**
   * @brief Inserts an element at the back of the deque (move).
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_back(T&& value) -> void;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the element at the front of the deque.
   * @throws QueueUnderflowException if the deque is empty.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto pop_front() -> void;

  /**
   * @brief Removes the element at the back of the deque.
   * @throws QueueUnderflowException if the deque is empty.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto pop_back() -> void;

  /**
   * @brief Removes all elements from the deque.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Returns a reference to the front element.
   * @return Reference to the front element.
   * @throws QueueUnderflowException if the deque is empty.
   */
  auto front() -> T&;

  /**
   * @brief Returns a const reference to the front element.
   * @return Const reference to the front element.
   * @throws QueueUnderflowException if the deque is empty.
   */
  auto front() const -> const T&;

  /**
   * @brief Returns a reference to the back element.
   * @return Reference to the back element.
   * @throws QueueUnderflowException if the deque is empty.
   */
  auto back() -> T&;

  /**
   * @brief Returns a const reference to the back element.
   * @return Const reference to the back element.
   * @throws QueueUnderflowException if the deque is empty.
   */
  auto back() const -> const T&;

  /**
   * @brief Accesses an element without bounds checking.
   * @param index The logical index.
   * @return Reference to the element.
   */
  auto operator[](size_t index) -> T&;

  /**
   * @brief Accesses an element without bounds checking (const).
   * @param index The logical index.
   * @return Const reference to the element.
   */
  auto operator[](size_t index) const -> const T&;

  /**
   * @brief Accesses an element with bounds checking.
   * @param index The logical index.
   * @return Reference to the element.
   * @throws QueueException if index is out of bounds.
   */
  auto at(size_t index) -> T&;

  /**
   * @brief Accesses an element with bounds checking (const).
   * @param index The logical index.
   * @return Const reference to the element.
   * @throws QueueException if index is out of bounds.
   */
  auto at(size_t index) const -> const T&;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the deque is empty.
   * @return true if the deque is empty, false otherwise.
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the deque.
   * @return The number of elements.
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the current capacity of the deque.
   * @return The capacity.
   */
  [[nodiscard]] auto capacity() const noexcept -> size_t;

  //===-------------------------- CAPACITY OPERATIONS --------------------------===//

  /**
   * @brief Reserves capacity for at least n elements.
   * @param new_capacity The minimum capacity to reserve.
   */
  auto reserve(size_t new_capacity) -> void;

  /**
   * @brief Shrinks the capacity to fit the current size.
   */
  auto shrink_to_fit() -> void;

  //===------------------------- ITERATOR OPERATIONS ---------------------------===//

  /**
   * @brief Returns an iterator/const_iterator to the beginning of the deque.
   */
  auto begin() noexcept -> iterator;
  auto begin() const noexcept -> const_iterator;

  /**
   * @brief Returns an iterator/const_iterator to the end of the deque.
   */
  auto end() noexcept -> iterator;
  auto end() const noexcept -> const_iterator;

  /**
   * @brief Returns a const_iterator to the beginning of the deque.
   */
  auto cbegin() const noexcept -> const_iterator;
  auto cend() const noexcept -> const_iterator;

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Calculates the next index in the circular buffer.
   * @param index The current index.
   * @return The next index, wrapping around if necessary.
   */
  [[nodiscard]] auto next_index(size_t index) const noexcept -> size_t { return (index + 1) % capacity_; }

  /**
   * @brief Calculates the previous index in the circular buffer.
   * @param index The current index.
   * @return The previous index, wrapping around if necessary.
   */
  [[nodiscard]] auto prev_index(size_t index) const noexcept -> size_t { return (index + capacity_ - 1) % capacity_; }

  /**
   * @brief Converts a logical offset to a physical index in the circular buffer.
   * @param offset The logical offset from the front.
   * @return The physical index in the underlying array.
   */
  [[nodiscard]] auto index_from_front(size_t offset) const noexcept -> size_t { return (front_ + offset) % capacity_; }

  /**
   * @brief Accesses the element at the given logical index without bounds checking.
   * @param index The logical index.
   * @return Reference to the element.
   */
  auto element_at(size_t index) -> T&;

  /**
   * @brief Accesses the element at the given logical index without bounds checking (const).
   * @param index The logical index.
   * @return Const reference to the element.
   */
  auto element_at(size_t index) const -> const T&;

  /**
   * @brief Ensures the internal array has at least the specified capacity.
   * @param min_capacity The minimum required capacity.
   * @complexity Time O(n) if reallocation occurs, Space O(n)
   * @throws std::bad_alloc if memory allocation fails.
   */
  auto ensure_capacity(size_t min_capacity) -> void;

  /**
   * @brief Grows the internal array when full.
   * @complexity Time O(n), Space O(n)
   * @throws std::bad_alloc if memory allocation fails.
   */
  auto reallocate(size_t new_capacity) -> void;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<T[], void (*)(T*)> data_;     ///< The dynamic array holding deque elements.
  size_t                             front_;    ///< Index of the front element.
  size_t                             size_;     ///< Current number of elements.
  size_t                             capacity_; ///< Current capacity of the array.

  static constexpr size_t kGrowthFactor = 2; ///< Growth factor for dynamic resizing.
  static constexpr size_t kMinCapacity  = 8; ///< Minimum capacity to maintain.
};

} // namespace ads::queues

// Include the implementation file for templates.
#include "../../../src/ads/queues/Circular_Array_Deque.tpp"

#endif // CIRCULAR_ARRAY_DEQUE_HPP

//===---------------------------------------------------------------------------===//
