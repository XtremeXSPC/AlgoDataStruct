//===---------------------------------------------------------------------------===//
/**
 * @file Dynamic_Array.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the DynamicArray class.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include "Array_Exception.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace ads::arrays {

/**
 * @brief A dynamic array implementation similar to std::vector.
 *
 * @details This class implements a resizable array with contiguous storage and
 *          amortized O(1) push_back operations. Memory is managed manually via
 *          raw allocation and placement new for fine-grained control and
 *          optimal performance.
 *
 * @tparam T The type of elements stored in the array.
 */
template <typename T>
class DynamicArray {
public:
  using value_type             = T;
  using size_type              = size_t;
  using difference_type        = std::ptrdiff_t;
  using reference              = T&;
  using const_reference        = const T&;
  using pointer                = T*;
  using const_pointer          = const T*;
  using iterator               = T*;
  using const_iterator         = const T*;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty dynamic array with optional initial capacity.
   * @param initial_capacity The initial capacity (default: 16).
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(1), Space O(initial_capacity)
   */
  explicit DynamicArray(size_t initial_capacity = kInitialCapacity);

  /**
   * @brief Constructs a dynamic array from an initializer list.
   * @param values The initializer list of elements.
   * @complexity Time O(n), Space O(n)
   */
  DynamicArray(std::initializer_list<T> values);

  /**
   * @brief Constructs a dynamic array with a given size and fill value.
   * @param count Number of elements to create.
   * @param value Value to fill the array with.
   * @complexity Time O(n), Space O(n)
   */
  DynamicArray(size_t count, const T& value);

  /**
   * @brief Move constructor.
   * @param other The array to move from.
   * @complexity Time O(1), Space O(1)
   */
  DynamicArray(DynamicArray&& other) noexcept;

  /**
   * @brief Destructor. Destroys all elements and deallocates memory.
   * @complexity Time O(n), Space O(1)
   */
  ~DynamicArray();

  /**
   * @brief Move assignment operator.
   * @param other The array to move from.
   * @return Reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(DynamicArray&& other) noexcept -> DynamicArray&;

  // Copy constructor and assignment are disabled (move-only type).
  DynamicArray(const DynamicArray&)                    = delete;
  auto operator=(const DynamicArray&) -> DynamicArray& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Constructs an element in-place at the end.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace_back(Args&&... args) -> T&;

  /**
   * @brief Appends an element to the end (copy).
   * @param value The value to append.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_back(const T& value) -> void;

  /**
   * @brief Appends an element to the end (move).
   * @param value The value to append.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_back(T&& value) -> void;

  /**
   * @brief Constructs an element in-place at the given index.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param index The insertion index.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @throws ArrayOutOfRangeException if index > size().
   * @complexity Time O(n), Space O(1)
   */
  template <typename... Args>
  auto emplace(size_t index, Args&&... args) -> T&;

  /**
   * @brief Inserts an element at the given index (copy).
   * @param index The insertion index.
   * @param value The value to insert.
   * @throws ArrayOutOfRangeException if index > size().
   * @complexity Time O(n), Space O(1)
   */
  auto insert(size_t index, const T& value) -> void;

  /**
   * @brief Inserts an element at the given index (move).
   * @param index The insertion index.
   * @param value The value to insert.
   * @throws ArrayOutOfRangeException if index > size().
   * @complexity Time O(n), Space O(1)
   */
  auto insert(size_t index, T&& value) -> void;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the last element.
   * @throws ArrayUnderflowException if the array is empty.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto pop_back() -> void;

  /**
   * @brief Removes the element at the given index.
   * @param index The index to erase.
   * @throws ArrayOutOfRangeException if index is out of bounds.
   * @complexity Time O(n), Space O(1)
   */
  auto erase(size_t index) -> void;

  /**
   * @brief Removes all elements from the array.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Accesses an element without bounds checking.
   * @param index The index of the element.
   * @return Reference to the element.
   */
  auto operator[](size_t index) -> T&;

  /**
   * @brief Accesses an element without bounds checking (const).
   * @param index The index of the element.
   * @return Const reference to the element.
   */
  auto operator[](size_t index) const -> const T&;

  /**
   * @brief Accesses an element with bounds checking.
   * @param index The index of the element.
   * @return Reference to the element.
   * @throws ArrayOutOfRangeException if index is out of bounds.
   */
  auto at(size_t index) -> T&;

  /**
   * @brief Accesses an element with bounds checking (const).
   * @param index The index of the element.
   * @return Const reference to the element.
   * @throws ArrayOutOfRangeException if index is out of bounds.
   */
  auto at(size_t index) const -> const T&;

  /**
   * @brief Returns a reference to the first element.
   * @throws ArrayUnderflowException if the array is empty.
   */
  auto front() -> T&;

  /**
   * @brief Returns a const reference to the first element.
   * @throws ArrayUnderflowException if the array is empty.
   */
  auto front() const -> const T&;

  /**
   * @brief Returns a reference to the last element.
   * @throws ArrayUnderflowException if the array is empty.
   */
  auto back() -> T&;

  /**
   * @brief Returns a const reference to the last element.
   * @throws ArrayUnderflowException if the array is empty.
   */
  auto back() const -> const T&;

  /**
   * @brief Returns the underlying raw data pointer.
   */
  auto data() noexcept -> T*;

  /**
   * @brief Returns the underlying raw data pointer (const).
   */
  auto data() const noexcept -> const T*;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the array is empty.
   * @return true if empty, false otherwise.
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the array.
   * @return The current size.
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the current capacity of the array.
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

  /**
   * @brief Resizes the array, default-initializing new elements.
   * @param new_size The desired size.
   */
  auto resize(size_t new_size) -> void
    requires std::default_initializable<T>;

  /**
   * @brief Resizes the array, filling new elements with a value.
   * @param new_size The desired size.
   * @param value The value to fill new elements with.
   */
  auto resize(size_t new_size, const T& value) -> void;

  //===------------------------- ITERATOR OPERATIONS ---------------------------===//

  /**
   * @brief Returns an iterator/const_iterator to the beginning of the array.
   */
  auto begin() noexcept -> iterator;
  auto begin() const noexcept -> const_iterator;

  /**
   * @brief Returns an iterator/const_iterator to the end of the array.
   */
  auto end() noexcept -> iterator;
  auto end() const noexcept -> const_iterator;

  /**
   * @brief Returns a const_iterator to the beginning/end of the array.
   */
  auto cbegin() const noexcept -> const_iterator;
  auto cend() const noexcept -> const_iterator;

  /**
   * @brief Returns a reverse_iterator/const_reverse_iterator to the beginning.
   */
  auto rbegin() noexcept -> reverse_iterator;
  auto rbegin() const noexcept -> const_reverse_iterator;

  /**
   * @brief Returns a reverse_iterator/const_reverse_iterator to the end.
   */
  auto rend() noexcept -> reverse_iterator;
  auto rend() const noexcept -> const_reverse_iterator;

  /**
   * @brief Returns a const_reverse_iterator to the beginning/end of the array.
   */
  auto crbegin() const noexcept -> const_reverse_iterator;
  auto crend() const noexcept -> const_reverse_iterator;

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Ensures capacity for at least min_capacity elements.
   * @param min_capacity The minimum capacity required.
   */
  auto ensure_capacity(size_t min_capacity) -> void;

  /**
   * @brief Grows the array capacity when full.
   */
  auto grow() -> void;

  /**
   * @brief Reallocates the internal buffer to new_capacity.
   * @param new_capacity The new capacity.
   */
  auto reallocate(size_t new_capacity) -> void;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<T[], void (*)(T*)> data_;     ///< Raw storage for elements.
  size_t                             size_;     ///< Number of constructed elements.
  size_t                             capacity_; ///< Allocated capacity.

  static constexpr size_t kInitialCapacity = 16; ///< Default initial capacity.
  static constexpr size_t kGrowthFactor    = 2;  ///< Growth factor for resizing.
  static constexpr size_t kMinCapacity     = 8;  ///< Minimum capacity to maintain.
};

} // namespace ads::arrays

// Include the implementation file for templates.
#include "../../../src/ads/arrays/Dynamic_Array.tpp"

#endif // DYNAMIC_ARRAY_HPP

//===---------------------------------------------------------------------------===//
