//===---------------------------------------------------------------------------===//
/**
 * @file Static_Array.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the StaticArray class template.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef STATIC_ARRAY_HPP
#define STATIC_ARRAY_HPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>

#include "Array_Exception.hpp"

namespace ads::arrays {

/**
 * @brief A fixed-size array implementation similar to std::array.
 *
 * @details This class implements a compile-time fixed-size array with contiguous
 *          storage. Unlike DynamicArray, size is determined at compile time and
 *          cannot change. Provides O(1) access and modification operations.
 *
 * @tparam T The type of elements stored in the array.
 * @tparam N The fixed size of the array (must be > 0).
 */
template <typename T, size_t N>
  requires(N > 0)
class StaticArray {
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
   * @brief Default constructor. Value-initializes all elements.
   * @complexity Time O(N), Space O(1)
   */
  StaticArray()
    requires std::default_initializable<T>;

  /**
   * @brief Constructs a static array from an initializer list.
   * @param values The initializer list of elements (must have exactly N elements).
   * @throws ArrayOutOfRangeException if initializer list size != N.
   * @complexity Time O(N), Space O(1)
   */
  StaticArray(std::initializer_list<T> values);

  /**
   * @brief Constructs a static array filled with a single value.
   * @param value The value to fill the array with.
   * @complexity Time O(N), Space O(1)
   */
  explicit StaticArray(const T& value);

  /**
   * @brief Copy constructor.
   * @param other The array to copy from.
   * @complexity Time O(N), Space O(1)
   */
  StaticArray(const StaticArray& other)
    requires std::copy_constructible<T>;

  /**
   * @brief Move constructor.
   * @param other The array to move from.
   * @complexity Time O(N), Space O(1)
   */
  StaticArray(StaticArray&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
    requires std::move_constructible<T>;

  /**
   * @brief Destructor. Destroys all elements.
   * @complexity Time O(N), Space O(1)
   */
  ~StaticArray() = default;

  /**
   * @brief Copy assignment operator.
   * @param other The array to copy from.
   * @return Reference to this instance.
   * @complexity Time O(N), Space O(1)
   */
  auto operator=(const StaticArray& other) -> StaticArray&
    requires std::is_copy_assignable_v<T>;

  /**
   * @brief Move assignment operator.
   * @param other The array to move from.
   * @return Reference to this instance.
   * @complexity Time O(N), Space O(1)
   */
  auto operator=(StaticArray&& other) noexcept(std::is_nothrow_move_assignable_v<T>) -> StaticArray&
    requires std::is_move_assignable_v<T>;

  //===------------------------ MODIFICATION OPERATIONS -------------------------===//

  /**
   * @brief Fills the entire array with a given value.
   * @param value The value to fill with.
   * @complexity Time O(N), Space O(1)
   */
  auto fill(const T& value) -> void;

  /**
   * @brief Swaps contents with another StaticArray.
   * @param other The array to swap with.
   * @complexity Time O(N), Space O(1)
   */
  auto swap(StaticArray& other) noexcept(std::is_nothrow_swappable_v<T>) -> void;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Accesses an element without bounds checking.
   * @param index The index of the element.
   * @return Reference to the element.
   * @complexity Time O(1), Space O(1)
   */
  auto operator[](size_t index) -> T&;

  /**
   * @brief Accesses an element without bounds checking (const).
   * @param index The index of the element.
   * @return Const reference to the element.
   * @complexity Time O(1), Space O(1)
   */
  auto operator[](size_t index) const -> const T&;

  /**
   * @brief Accesses an element with bounds checking.
   * @param index The index of the element.
   * @return Reference to the element.
   * @throws ArrayOutOfRangeException if index >= N.
   * @complexity Time O(1), Space O(1)
   */
  auto at(size_t index) -> T&;

  /**
   * @brief Accesses an element with bounds checking (const).
   * @param index The index of the element.
   * @return Const reference to the element.
   * @throws ArrayOutOfRangeException if index >= N.
   * @complexity Time O(1), Space O(1)
   */
  auto at(size_t index) const -> const T&;

  /**
   * @brief Returns a reference to the first element.
   * @return Reference to the first element.
   * @complexity Time O(1), Space O(1)
   */
  auto front() -> T&;

  /**
   * @brief Returns a const reference to the first element.
   * @return Const reference to the first element.
   * @complexity Time O(1), Space O(1)
   */
  auto front() const -> const T&;

  /**
   * @brief Returns a reference to the last element.
   * @return Reference to the last element.
   * @complexity Time O(1), Space O(1)
   */
  auto back() -> T&;

  /**
   * @brief Returns a const reference to the last element.
   * @return Const reference to the last element.
   * @complexity Time O(1), Space O(1)
   */
  auto back() const -> const T&;

  /**
   * @brief Returns a pointer to the underlying array data.
   * @return Pointer to the first element.
   * @complexity Time O(1), Space O(1)
   */
  auto data() noexcept -> T*;

  /**
   * @brief Returns a const pointer to the underlying array data.
   * @return Const pointer to the first element.
   * @complexity Time O(1), Space O(1)
   */
  auto data() const noexcept -> const T*;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Returns the number of elements in the array.
   * @return The fixed size N.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static constexpr auto size() noexcept -> size_t { return N; }

  /**
   * @brief Returns the maximum possible size.
   * @return The fixed size N.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static constexpr auto max_size() noexcept -> size_t { return N; }

  /**
   * @brief Checks if the array is empty (always false for N > 0).
   * @return Always false since N > 0.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static constexpr auto is_empty() noexcept -> bool { return false; }

  //===------------------------- ITERATOR OPERATIONS ---------------------------===//

  /**
   * @brief Returns an iterator to the beginning of the array.
   */
  auto begin() noexcept -> iterator;
  auto begin() const noexcept -> const_iterator;

  /**
   * @brief Returns an iterator to the end of the array.
   */
  auto end() noexcept -> iterator;
  auto end() const noexcept -> const_iterator;

  /**
   * @brief Returns a const iterator to the beginning/end of the array.
   */
  auto cbegin() const noexcept -> const_iterator;
  auto cend() const noexcept -> const_iterator;

  /**
   * @brief Returns a reverse iterator to the beginning (end of array).
   */
  auto rbegin() noexcept -> reverse_iterator;
  auto rbegin() const noexcept -> const_reverse_iterator;

  /**
   * @brief Returns a reverse iterator to the end (beginning of array).
   */
  auto rend() noexcept -> reverse_iterator;
  auto rend() const noexcept -> const_reverse_iterator;

  /**
   * @brief Returns a const reverse iterator to the beginning/end of the array.
   */
  auto crbegin() const noexcept -> const_reverse_iterator;
  auto crend() const noexcept -> const_reverse_iterator;

  //===-------------------------- COMPARISON OPERATORS -------------------------===//

  /**
   * @brief Equality comparison operator.
   * @param other The array to compare with.
   * @return true if all elements are equal, false otherwise.
   * @complexity Time O(N), Space O(1)
   */
  auto operator==(const StaticArray& other) const -> bool
    requires std::equality_comparable<T>;

  /**
   * @brief Three-way comparison operator.
   * @param other The array to compare with.
   * @return Ordering result.
   * @complexity Time O(N), Space O(1)
   */
  auto operator<=>(const StaticArray& other) const
    requires std::three_way_comparable<T>;

private:
  //===----------------------------- DATA MEMBERS ------------------------------===//

  T data_[N]; ///< Fixed-size storage for elements.
};

} // namespace ads::arrays

// Include the implementation file for templates.
#include "../../../src/ads/arrays/Static_Array.tpp"

#endif // STATIC_ARRAY_HPP

//===---------------------------------------------------------------------------===//
