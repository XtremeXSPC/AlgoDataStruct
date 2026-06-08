//===---------------------------------------------------------------------------===//
/**
 * @file Static_Vector.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the StaticVector class template.
 * @version 0.1
 * @date 2026-06-08
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef STATIC_VECTOR_HPP
#define STATIC_VECTOR_HPP

#include "../../support/Container_Facade.hpp"
#include "Array_Concepts.hpp"
#include "Array_Exception.hpp"

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace ads::arrays {

using ads::support::ContainerFacade;

/**
 * @brief A fixed-capacity, variable-size array with inline storage.
 *
 * @details StaticVector stores up to N elements in an inline, stack-allocated
 *          buffer and never touches the heap. It fills the gap between StaticArray
 *          (size fixed at N) and DynamicArray (heap-backed, unbounded): the size
 *          varies at runtime between 0 and N while the capacity is fixed at N.
 *          Element lifetimes within the raw storage are managed manually via
 *          placement construction and explicit destruction. Like the other dynamic
 *          array structures in this library, StaticVector is move-only.
 *
 * @tparam T The type of elements stored in the vector.
 * @tparam N The fixed capacity (must be > 0).
 */
template <ArrayElement T, size_t N>
requires ValidStaticArrayExtent<N>
class StaticVector : public ContainerFacade<StaticVector<T, N>> {
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
   * @brief Constructs an empty vector (size 0, capacity N).
   * @complexity Time O(1), Space O(1)
   */
  StaticVector() noexcept;

  /**
   * @brief Constructs a vector from an initializer list.
   * @param values The initializer list of elements (size must be <= N).
   * @throws ArrayOverflowException if the list has more than N elements.
   * @complexity Time O(n), Space O(1)
   */
  StaticVector(std::initializer_list<T> values) requires CopyArrayElement<T>;

  /**
   * @brief Constructs a vector with count copies of value.
   * @param count Number of elements to create (must be <= N).
   * @param value Value to fill the vector with.
   * @throws ArrayOverflowException if count exceeds N.
   * @complexity Time O(n), Space O(1)
   */
  StaticVector(size_t count, const T& value) requires CopyArrayElement<T>;

  /**
   * @brief Move constructor. Move-constructs each element into inline storage.
   * @param other The vector to move from (left empty afterwards).
   * @complexity Time O(n), Space O(1)
   */
  StaticVector(StaticVector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) requires MoveArrayElement<T>;

  /**
   * @brief Destructor. Destroys all live elements.
   * @complexity Time O(n), Space O(1)
   */
  ~StaticVector();

  /**
   * @brief Move assignment operator.
   * @param other The vector to move from (left empty afterwards).
   * @return Reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(StaticVector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) -> StaticVector& requires MoveArrayElement<T>;

  // Copy constructor and assignment are disabled (move-only type).
  StaticVector(const StaticVector&)                    = delete;
  auto operator=(const StaticVector&) -> StaticVector& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Constructs an element in-place at the end.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @throws ArrayOverflowException if the vector is already full.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace_back(Args&&... args) -> T& requires AppendArrayElement<T, Args...>;

  /**
   * @brief Appends an element to the end (copy).
   * @param value The value to append.
   * @throws ArrayOverflowException if the vector is already full.
   * @complexity Time O(1), Space O(1)
   */
  auto push_back(const T& value) -> void requires CopyArrayElement<T>;

  /**
   * @brief Appends an element to the end (move).
   * @param value The value to append.
   * @throws ArrayOverflowException if the vector is already full.
   * @complexity Time O(1), Space O(1)
   */
  auto push_back(T&& value) -> void requires MoveArrayElement<T>;

  /**
   * @brief Constructs an element in-place at the given index.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param index The insertion index.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @throws ArrayOutOfRangeException if index > size().
   * @throws ArrayOverflowException if the vector is already full.
   * @complexity Time O(n), Space O(1)
   */
  template <typename... Args>
  auto emplace(size_t index, Args&&... args) -> T& requires InsertArrayElement<T, Args...>;

  /**
   * @brief Inserts an element at the given index (copy).
   * @param index The insertion index.
   * @param value The value to insert.
   * @throws ArrayOutOfRangeException if index > size().
   * @throws ArrayOverflowException if the vector is already full.
   * @complexity Time O(n), Space O(1)
   */
  auto insert(size_t index, const T& value) -> void requires InsertCopyArrayElement<T>;

  /**
   * @brief Inserts an element at the given index (move).
   * @param index The insertion index.
   * @param value The value to insert.
   * @throws ArrayOutOfRangeException if index > size().
   * @throws ArrayOverflowException if the vector is already full.
   * @complexity Time O(n), Space O(1)
   */
  auto insert(size_t index, T&& value) -> void requires InsertMoveArrayElement<T>;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the last element.
   * @throws ArrayUnderflowException if the vector is empty.
   * @complexity Time O(1), Space O(1)
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
   * @brief Removes all elements from the vector.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  /**
   * @brief Resizes the vector, default-initializing new elements.
   * @param new_size The desired size (must be <= N).
   * @throws ArrayOverflowException if new_size exceeds N.
   * @complexity Time O(n), Space O(1)
   */
  auto resize(size_t new_size) -> void requires ResizeArrayElement<T>;

  /**
   * @brief Resizes the vector, filling new elements with a value.
   * @param new_size The desired size (must be <= N).
   * @param value The value to fill new elements with.
   * @throws ArrayOverflowException if new_size exceeds N.
   * @complexity Time O(n), Space O(1)
   */
  auto resize(size_t new_size, const T& value) -> void requires CopyArrayElement<T>;

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
   * @throws ArrayOutOfRangeException if index >= size().
   * @complexity Time O(1), Space O(1)
   */
  auto at(size_t index) -> T&;

  /**
   * @brief Accesses an element with bounds checking (const).
   * @param index The index of the element.
   * @return Const reference to the element.
   * @throws ArrayOutOfRangeException if index >= size().
   * @complexity Time O(1), Space O(1)
   */
  auto at(size_t index) const -> const T&;

  /**
   * @brief Returns a reference to the first element.
   * @throws ArrayUnderflowException if the vector is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() -> T&;

  /**
   * @brief Returns a const reference to the first element.
   * @throws ArrayUnderflowException if the vector is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() const -> const T&;

  /**
   * @brief Returns a reference to the last element.
   * @throws ArrayUnderflowException if the vector is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto back() -> T&;

  /**
   * @brief Returns a const reference to the last element.
   * @throws ArrayUnderflowException if the vector is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto back() const -> const T&;

  /**
   * @brief Returns a pointer to the underlying contiguous storage.
   * @return Pointer to the first element.
   * @complexity Time O(1), Space O(1)
   */
  auto data() noexcept -> T*;

  /**
   * @brief Returns a const pointer to the underlying contiguous storage.
   * @return Const pointer to the first element.
   * @complexity Time O(1), Space O(1)
   */
  auto data() const noexcept -> const T*;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the vector is empty.
   * @return true if empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Checks if the vector is full (size() == N).
   * @return true if full, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_full() const noexcept -> bool;

  /**
   * @brief Returns the number of elements currently stored.
   * @return The current size.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the fixed capacity N.
   * @return The capacity.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static constexpr auto capacity() noexcept -> size_t { return N; }

  /**
   * @brief Returns the maximum possible size (equal to capacity N).
   * @return The fixed capacity N.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static constexpr auto max_size() noexcept -> size_t { return N; }

  //===-------------------------- ITERATOR OPERATIONS --------------------------===//

  /**
   * @brief Returns an iterator/const_iterator to the beginning of the vector.
   */
  auto begin() noexcept -> iterator;
  auto begin() const noexcept -> const_iterator;

  /**
   * @brief Returns an iterator/const_iterator to the end of the vector.
   */
  auto end() noexcept -> iterator;
  auto end() const noexcept -> const_iterator;

  // cbegin/cend, rbegin/rend, crbegin/crend, and the relational operators
  // (==, <=>) are inherited from ContainerFacade<StaticVector<T, N>>.

private:
  //===----------------------------- DATA MEMBERS ------------------------------===//

  alignas(T) std::byte storage_[sizeof(T) * N]; ///< Raw inline storage for N elements.
  size_t size_;                                 ///< Number of constructed elements.
};

} // namespace ads::arrays

// Include the implementation file for templates.
#include "../../../src/ads/arrays/Static_Vector.tpp"

#endif // STATIC_VECTOR_HPP

//===---------------------------------------------------------------------------===//
