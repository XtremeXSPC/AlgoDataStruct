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

#include "../../support/Container_Facade.hpp"
#include "../../support/Indexed_Iterator.hpp"
#include "Array_Concepts.hpp"
#include "Array_Exception.hpp"

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace ads::arrays {

using ads::support::ContainerFacade;
using ads::support::IndexedIterator;

/**
 * @brief A dynamic circular array implementation supporting O(1) operations at both ends.
 *
 * @details This class implements a circular buffer with dynamic resizing. Unlike a
 *          standard dynamic array, it supports efficient push/pop operations at both
 *          the front and back. Internally uses modular arithmetic for wrap-around
 *          behavior. Suitable for implementing queues, deques, and ring buffers.
 *
 * @tparam T The type of elements stored in the array.
 *
 * @note Iterators address the LOGICAL position, not the element:
 *       they survive reallocation, but any front insertion/removal
 *       shifts the meaning of every live iterator by one position.
 *       Treat mutating calls as invalidating outstanding iterators.
 */
template <ArrayElement T>
class CircularArray : public ContainerFacade<CircularArray<T>> {
public:
  using value_type      = T;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = T&;
  using const_reference = const T&;
  using pointer         = T*;
  using const_pointer   = const T*;

  //===----- ITERATOR TYPES ----------------------------------------------------===//

  // The logical (wrap-around) iteration order is provided by IndexedIterator,
  // which dereferences through this container's operator[]. See Indexed_Iterator.hpp.
  using iterator               = IndexedIterator<CircularArray<T>, false>;
  using const_iterator         = IndexedIterator<CircularArray<T>, true>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

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

  //===----- INSERTION OPERATIONS ----------------------------------------------===//

  /**
   * @brief Constructs an element in-place at the front.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace_front(Args&&... args) -> T& requires AppendArrayElement<T, Args...>;

  /**
   * @brief Inserts an element at the front.
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_front(const T& value) -> void requires CopyArrayElement<T>;

  /**
   * @brief Inserts an element at the front (move).
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_front(T&& value) -> void requires MoveArrayElement<T>;

  /**
   * @brief Constructs an element in-place at the back.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace_back(Args&&... args) -> T& requires AppendArrayElement<T, Args...>;

  /**
   * @brief Inserts an element at the back.
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_back(const T& value) -> void requires CopyArrayElement<T>;

  /**
   * @brief Inserts an element at the back (move).
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto push_back(T&& value) -> void requires MoveArrayElement<T>;

  //===----- REMOVAL OPERATIONS ------------------------------------------------===//

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

  //===----- ACCESS OPERATIONS -------------------------------------------------===//

  /**
   * @brief Accesses an element by logical index without bounds checking.
   * @param index The logical index of the element.
   * @return Reference to the element.
   * @complexity Time O(1), Space O(1)
   */
  auto operator[](size_t index) -> T&;

  /**
   * @brief Accesses an element by logical index without bounds checking (const).
   * @param index The logical index of the element.
   * @return Const reference to the element.
   * @complexity Time O(1), Space O(1)
   */
  auto operator[](size_t index) const -> const T&;

  /**
   * @brief Accesses an element by logical index with bounds checking.
   * @param index The logical index of the element.
   * @return Reference to the element.
   * @throws ArrayOutOfRangeException if index >= size().
   * @complexity Time O(1), Space O(1)
   */
  auto at(size_t index) -> T&;

  /**
   * @brief Accesses an element by logical index with bounds checking (const).
   * @param index The logical index of the element.
   * @return Const reference to the element.
   * @throws ArrayOutOfRangeException if index >= size().
   * @complexity Time O(1), Space O(1)
   */
  auto at(size_t index) const -> const T&;

  /**
   * @brief Returns a reference to the first element.
   * @throws ArrayUnderflowException if the array is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() -> T&;

  /**
   * @brief Returns a const reference to the first element.
   * @throws ArrayUnderflowException if the array is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() const -> const T&;

  /**
   * @brief Returns a reference to the last element.
   * @throws ArrayUnderflowException if the array is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto back() -> T&;

  /**
   * @brief Returns a const reference to the last element.
   * @throws ArrayUnderflowException if the array is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto back() const -> const T&;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

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

  //===----- CAPACITY OPERATIONS -----------------------------------------------===//

  /**
   * @brief Reserves capacity for at least n elements.
   * @param new_capacity The minimum capacity to reserve.
   * @complexity Time O(n), Space O(new_capacity)
   */
  auto reserve(size_t new_capacity) -> void requires RelocatableArrayElement<T>;

  /**
   * @brief Shrinks the capacity to fit the current size.
   * @complexity Time O(n), Space O(size)
   */
  auto shrink_to_fit() -> void requires RelocatableArrayElement<T>;

  //===----- ITERATOR OPERATIONS -----------------------------------------------===//

  /// @brief Returns an iterator to the beginning of the array.
  auto begin() noexcept -> iterator;

  /// @brief Returns a const iterator to the beginning of the array.
  auto begin() const noexcept -> const_iterator;

  /// @brief Returns an iterator to the end of the array.
  auto end() noexcept -> iterator;

  /// @brief Returns a const iterator to the end of the array.
  auto end() const noexcept -> const_iterator;

  // cbegin/cend, rbegin/rend, crbegin/crend, and the relational operators
  // (==, <=>) are inherited from ContainerFacade<CircularArray<T>>.

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  /// Owning pointer to the raw element storage with a custom array deleter.
  using storage_ptr = std::unique_ptr<T[], void (*)(T*)>;

  /**
   * @brief Returns the maximum number of elements that can be allocated for T.
   * @return floor(SIZE_MAX / sizeof(T)).
   */
  static constexpr auto max_elements() noexcept -> size_t { return std::numeric_limits<size_t>::max() / sizeof(T); }

  /**
   * @brief Releases raw storage previously obtained from allocate().
   * @param ptr Pointer to release (may be null).
   */
  static auto deallocate(T* ptr) noexcept -> void {
    if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
      ::operator delete[](static_cast<void*>(ptr), std::align_val_t{alignof(T)});
    } else {
      ::operator delete[](static_cast<void*>(ptr));
    }
  }

  /**
   * @brief Allocates uninitialized storage for the given number of elements.
   * @param capacity Number of elements to reserve space for.
   * @return Owning pointer to the newly allocated storage.
   * @throws ArrayOverflowException if capacity exceeds max_elements().
   */
  static auto allocate(size_t capacity) -> storage_ptr;

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

  //===----- DATA MEMBERS ------------------------------------------------------===//

  storage_ptr data_;     ///< Raw storage for elements.
  size_t      head_;     ///< Physical index of the first element.
  size_t      size_;     ///< Number of elements.
  size_t      capacity_; ///< Allocated capacity.

  static constexpr size_t kInitialCapacity = 16; ///< Default initial capacity.
  static constexpr size_t kGrowthFactor    = 2;  ///< Growth factor for resizing.
  static constexpr size_t kMinCapacity     = 8;  ///< Minimum capacity to maintain.
};

} // namespace ads::arrays

// Include the implementation file for templates.
#include "../../../src/ads/arrays/Circular_Array.tpp"

#endif // CIRCULAR_ARRAY_HPP

//===---------------------------------------------------------------------------===//
