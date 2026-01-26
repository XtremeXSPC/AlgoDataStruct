//===---------------------------------------------------------------------------===//
/**
 * @file Circular_Array.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the CircularArray class template.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef CIRCULAR_ARRAY_HPP
#define CIRCULAR_ARRAY_HPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include "Array_Exception.hpp"

namespace ads::arrays {

/**
 * @brief A dynamic circular array implementation supporting O(1) operations at both ends.
 *
 * @details This class implements a circular buffer with dynamic resizing. Unlike a
 *          standard dynamic array, it supports efficient push/pop operations at both
 *          the front and back. Internally uses modular arithmetic for wrap-around
 *          behavior. Suitable for implementing queues, deques, and ring buffers.
 *
 * @tparam T The type of elements stored in the array.
 */
template <typename T>
class CircularArray {
public:
  //===---------------------------- ITERATOR CLASS -----------------------------===//

  /**
   * @brief Random access iterator for CircularArray.
   */
  class iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    iterator() = default;
    iterator(size_t logical_index, CircularArray<T>* array) : logical_index_(logical_index), array_(array) {}

    // Dereference operators.
    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator[](difference_type n) const -> reference;

    // Increment and decrement operators.
    auto operator++() -> iterator&;
    auto operator++(int) -> iterator;
    auto operator--() -> iterator&;
    auto operator--(int) -> iterator;

    // Arithmetic operators.
    auto operator+=(difference_type n) -> iterator&;
    auto operator-=(difference_type n) -> iterator&;
    auto operator+(difference_type n) const -> iterator;
    auto operator-(difference_type n) const -> iterator;
    auto operator-(const iterator& other) const -> difference_type;

    // Comparison operators.
    auto operator==(const iterator& other) const -> bool { return logical_index_ == other.logical_index_; }
    auto operator!=(const iterator& other) const -> bool { return logical_index_ != other.logical_index_; }
    auto operator<(const iterator& other) const -> bool { return logical_index_ < other.logical_index_; }
    auto operator<=(const iterator& other) const -> bool { return logical_index_ <= other.logical_index_; }
    auto operator>(const iterator& other) const -> bool { return logical_index_ > other.logical_index_; }
    auto operator>=(const iterator& other) const -> bool { return logical_index_ >= other.logical_index_; }

  private:
    size_t            logical_index_ = 0;
    CircularArray<T>* array_         = nullptr;
    friend class CircularArray<T>;
  };

  //===------------------------- CONST_ITERATOR CLASS --------------------------===//

  /**
   * @brief Const random access iterator for CircularArray.
   */
  class const_iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    const_iterator() = default;
    const_iterator(size_t logical_index, const CircularArray<T>* array) : logical_index_(logical_index), array_(array) {}
    explicit const_iterator(const iterator& it) : logical_index_(it.logical_index_), array_(it.array_) {}

    // Dereference operators.
    auto operator*() const -> reference;
    auto operator->() const -> pointer;
    auto operator[](difference_type n) const -> reference;

    // Increment and decrement operators.
    auto operator++() -> const_iterator&;
    auto operator++(int) -> const_iterator;
    auto operator--() -> const_iterator&;
    auto operator--(int) -> const_iterator;

    // Arithmetic operators.
    auto operator+=(difference_type n) -> const_iterator&;
    auto operator-=(difference_type n) -> const_iterator&;
    auto operator+(difference_type n) const -> const_iterator;
    auto operator-(difference_type n) const -> const_iterator;
    auto operator-(const const_iterator& other) const -> difference_type;

    // Comparison operators.
    auto operator==(const const_iterator& other) const -> bool { return logical_index_ == other.logical_index_; }
    auto operator!=(const const_iterator& other) const -> bool { return logical_index_ != other.logical_index_; }
    auto operator<(const const_iterator& other) const -> bool { return logical_index_ < other.logical_index_; }
    auto operator<=(const const_iterator& other) const -> bool { return logical_index_ <= other.logical_index_; }
    auto operator>(const const_iterator& other) const -> bool { return logical_index_ > other.logical_index_; }
    auto operator>=(const const_iterator& other) const -> bool { return logical_index_ >= other.logical_index_; }

  private:
    size_t                  logical_index_ = 0;
    const CircularArray<T>* array_         = nullptr;
    friend class CircularArray<T>;
  };

  // Using declarations for iterator types.
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty circular array with optional initial capacity.
   * @param initial_capacity The initial capacity (default: 16).
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(1), Space O(initial_capacity)
   */
  explicit CircularArray(size_t initial_capacity = kInitialCapacity);

  /**
   * @brief Constructs a circular array from an initializer list.
   * @param values The initializer list of elements.
   * @complexity Time O(n), Space O(n)
   */
  CircularArray(std::initializer_list<T> values);

  /**
   * @brief Move constructor.
   * @param other The array to move from.
   * @complexity Time O(1), Space O(1)
   */
  CircularArray(CircularArray&& other) noexcept;

  /**
   * @brief Destructor. Destroys all elements and deallocates memory.
   * @complexity Time O(n), Space O(1)
   */
  ~CircularArray();

  /**
   * @brief Move assignment operator.
   * @param other The array to move from.
   * @return Reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(CircularArray&& other) noexcept -> CircularArray&;

  // Copy constructor and assignment are disabled (move-only type).
  CircularArray(const CircularArray&)                    = delete;
  auto operator=(const CircularArray&) -> CircularArray& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Constructs an element in-place at the front.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace_front(Args&&... args) -> T&;

  /**
   * @brief Inserts an element at the front.
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_front(const T& value) -> void;
  auto push_front(T&& value) -> void;

  /**
   * @brief Constructs an element in-place at the back.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace_back(Args&&... args) -> T&;

  /**
   * @brief Inserts an element at the back.
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_back(const T& value) -> void;
  auto push_back(T&& value) -> void;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the first element.
   * @throws ArrayUnderflowException if the array is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto pop_front() -> void;

  /**
   * @brief Removes the last element.
   * @throws ArrayUnderflowException if the array is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto pop_back() -> void;

  /**
   * @brief Removes all elements from the array.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Accesses an element by logical index without bounds checking.
   * @param index The logical index of the element.
   * @return Reference to the element.
   * @complexity Time O(1), Space O(1)
   */
  auto operator[](size_t index) -> T&;
  auto operator[](size_t index) const -> const T&;

  /**
   * @brief Accesses an element by logical index with bounds checking.
   * @param index The logical index of the element.
   * @return Reference to the element.
   * @throws ArrayOutOfRangeException if index >= size().
   * @complexity Time O(1), Space O(1)
   */
  auto at(size_t index) -> T&;
  auto at(size_t index) const -> const T&;

  /**
   * @brief Returns a reference to the first element.
   * @throws ArrayUnderflowException if the array is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() -> T&;
  auto front() const -> const T&;

  /**
   * @brief Returns a reference to the last element.
   * @throws ArrayUnderflowException if the array is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto back() -> T&;
  auto back() const -> const T&;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the array is empty.
   * @return true if empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the array.
   * @return The current size.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the current capacity of the array.
   * @return The capacity.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto capacity() const noexcept -> size_t;

  //===-------------------------- CAPACITY OPERATIONS --------------------------===//

  /**
   * @brief Reserves capacity for at least n elements.
   * @param new_capacity The minimum capacity to reserve.
   * @complexity Time O(n), Space O(new_capacity)
   */
  auto reserve(size_t new_capacity) -> void;

  /**
   * @brief Shrinks the capacity to fit the current size.
   * @complexity Time O(n), Space O(size)
   */
  auto shrink_to_fit() -> void;

  //===------------------------- ITERATOR OPERATIONS ---------------------------===//

  /**
   * @brief Returns an iterator/const_iterator to the beginning.
   */
  auto begin() noexcept -> iterator;
  auto begin() const noexcept -> const_iterator;

  /**
   * @brief Returns an iterator/const_iterator to the end.
   */
  auto end() noexcept -> iterator;
  auto end() const noexcept -> const_iterator;

  /**
   * @brief Returns a const_iterator to the beginning/end.
   */
  auto cbegin() const noexcept -> const_iterator;
  auto cend() const noexcept -> const_iterator;

  /**
   * @brief Returns a reverse iterator/const_reverse_iterator to the beginning.
   */
  auto rbegin() noexcept -> reverse_iterator;
  auto rbegin() const noexcept -> const_reverse_iterator;

  /**
   * @brief Returns a reverse iterator/const_reverse_iterator to the end.
   */
  auto rend() noexcept -> reverse_iterator;
  auto rend() const noexcept -> const_reverse_iterator;

  /**
   * @brief Returns a const reverse iterator to the beginning/end.
   */
  auto crbegin() const noexcept -> const_reverse_iterator;
  auto crend() const noexcept -> const_reverse_iterator;

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Converts a logical index to a physical index in the buffer.
   * @param logical_index The logical index (0 = front).
   * @return The physical index in the buffer.
   */
  [[nodiscard]] auto to_physical_index(size_t logical_index) const noexcept -> size_t;

  /**
   * @brief Ensures capacity for at least min_capacity elements.
   * @param min_capacity The minimum capacity required.
   */
  auto ensure_capacity(size_t min_capacity) -> void;

  /**
   * @brief Reallocates the buffer to a new capacity, linearizing elements.
   * @param new_capacity The new capacity.
   */
  auto reallocate(size_t new_capacity) -> void;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<T[], void (*)(T*)> data_;     ///< Raw storage for elements.
  size_t                             head_;     ///< Physical index of the first element.
  size_t                             size_;     ///< Number of elements.
  size_t                             capacity_; ///< Allocated capacity.

  static constexpr size_t kInitialCapacity = 16; ///< Default initial capacity.
  static constexpr size_t kGrowthFactor    = 2;  ///< Growth factor for resizing.
  static constexpr size_t kMinCapacity     = 8;  ///< Minimum capacity to maintain.
};

} // namespace ads::arrays

// Include the implementation file for templates.
#include "../../../src/ads/arrays/Circular_Array.tpp"

#endif // CIRCULAR_ARRAY_HPP

//===---------------------------------------------------------------------------===//
