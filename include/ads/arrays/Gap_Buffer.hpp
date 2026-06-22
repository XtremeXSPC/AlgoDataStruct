//===---------------------------------------------------------------------------===//
/**
 * @file Gap_Buffer.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the GapBuffer class template.
 * @version 0.1
 * @date 2026-06-08
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef GAP_BUFFER_HPP
#define GAP_BUFFER_HPP

#include "../../support/Container_Facade.hpp"
#include "../../support/Indexed_Iterator.hpp"
#include "Array_Concepts.hpp"
#include "Array_Exception.hpp"

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
 * @brief A gap buffer: a sequence optimized for localized insertions and deletions.
 *
 * @details A gap buffer stores its elements in a single contiguous allocation that
 *          is split into two segments separated by an internal "gap" of unused
 *          slots. The gap sits at the cursor position, so insertions and deletions
 *          at the cursor are amortized O(1) (they simply shrink or grow the gap).
 *          Moving the cursor shifts elements across the gap in O(distance). This is
 *          the classic data structure behind text editors. Like the other dynamic
 *          array structures in this library, GapBuffer is move-only.
 *
 * @tparam T The type of elements stored in the buffer.
 */
template <ArrayElement T>
class GapBuffer : public ContainerFacade<GapBuffer<T>> {
public:
  using value_type      = T;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = T&;
  using const_reference = const T&;
  using pointer         = T*;
  using const_pointer   = const T*;

  //===----- ITERATOR TYPES ----------------------------------------------------===//

  // The logical (gap-skipping) iteration order is provided by IndexedIterator,
  // which dereferences through this container's operator[]. See Indexed_Iterator.hpp.
  using iterator               = IndexedIterator<GapBuffer<T>, false>;
  using const_iterator         = IndexedIterator<GapBuffer<T>, true>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty gap buffer with optional initial capacity.
   * @param initial_capacity The initial capacity (default: 16).
   * @throws ArrayOverflowException if the capacity would overflow.
   * @complexity Time O(1), Space O(initial_capacity)
   */
  explicit GapBuffer(size_t initial_capacity = kInitialCapacity);

  /**
   * @brief Constructs a gap buffer from an initializer list (cursor at the end).
   * @param values The initializer list of elements.
   * @complexity Time O(n), Space O(n)
   */
  GapBuffer(std::initializer_list<T> values) requires CopyArrayElement<T>;

  /**
   * @brief Move constructor.
   * @param other The buffer to move from.
   * @complexity Time O(1), Space O(1)
   */
  GapBuffer(GapBuffer&& other) noexcept;

  /**
   * @brief Destructor. Destroys all elements and deallocates memory.
   * @complexity Time O(n), Space O(1)
   */
  ~GapBuffer();

  /**
   * @brief Move assignment operator.
   * @param other The buffer to move from.
   * @return Reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(GapBuffer&& other) noexcept -> GapBuffer&;

  // Copy constructor and assignment are disabled (move-only type).
  GapBuffer(const GapBuffer&)                    = delete;
  auto operator=(const GapBuffer&) -> GapBuffer& = delete;

  //===----- INSERTION OPERATIONS ----------------------------------------------===//

  /**
   * @brief Constructs an element in-place at the cursor.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T& requires AppendArrayElement<T, Args...>;

  /**
   * @brief Inserts an element at the cursor (copy).
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto insert(const T& value) -> void requires CopyArrayElement<T>;

  /**
   * @brief Inserts an element at the cursor (move).
   * @param value The value to insert.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto insert(T&& value) -> void requires MoveArrayElement<T>;

  //===----- REMOVAL OPERATIONS ------------------------------------------------===//

  /**
   * @brief Removes the element immediately before the cursor (backspace).
   * @throws ArrayUnderflowException if the cursor is at the beginning.
   * @complexity Time O(1), Space O(1)
   */
  auto erase() -> void;

  /**
   * @brief Removes the element immediately after the cursor (delete).
   * @throws ArrayUnderflowException if the cursor is at the end.
   * @complexity Time O(1), Space O(1)
   */
  auto erase_forward() -> void;

  /**
   * @brief Removes all elements from the buffer.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===----- CURSOR OPERATIONS -------------------------------------------------===//

  /**
   * @brief Returns the current cursor position (logical index of the gap).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto cursor() const noexcept -> size_t;

  /**
   * @brief Moves the cursor to the given logical position.
   * @param position The target logical position (0..size()).
   * @throws ArrayOutOfRangeException if position > size().
   * @complexity Time O(|position - cursor()|), Space O(1)
   */
  auto move_cursor_to(size_t position) -> void;

  /**
   * @brief Moves the cursor one position toward the end.
   * @throws ArrayUnderflowException if the cursor is already at the end.
   * @complexity Time O(1), Space O(1)
   */
  auto advance_cursor() -> void;

  /**
   * @brief Moves the cursor one position toward the beginning.
   * @throws ArrayUnderflowException if the cursor is already at the beginning.
   * @complexity Time O(1), Space O(1)
   */
  auto retreat_cursor() -> void;

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
   * @throws ArrayUnderflowException if the buffer is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() -> T&;

  /**
   * @brief Returns a const reference to the first element.
   * @throws ArrayUnderflowException if the buffer is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() const -> const T&;

  /**
   * @brief Returns a reference to the last element.
   * @throws ArrayUnderflowException if the buffer is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto back() -> T&;

  /**
   * @brief Returns a const reference to the last element.
   * @throws ArrayUnderflowException if the buffer is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto back() const -> const T&;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Returns the number of elements in the buffer.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the current total capacity (elements plus gap).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto capacity() const noexcept -> size_t;

  // is_empty(), cbegin/cend, rbegin/rend, crbegin/crend, and the relational
  // operators (==, <=>) are inherited from ContainerFacade<GapBuffer<T>>.

  //===----- ITERATOR OPERATIONS -----------------------------------------------===//

  /// @brief Returns an iterator to the beginning of the buffer.
  auto begin() noexcept -> iterator;

  /// @brief Returns a const iterator to the beginning of the buffer.
  auto begin() const noexcept -> const_iterator;

  /// @brief Returns an iterator to the end of the buffer.
  auto end() noexcept -> iterator;

  /// @brief Returns a const iterator to the end of the buffer.
  auto end() const noexcept -> const_iterator;

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  /// Owning pointer to the raw element storage with a custom array deleter.
  using storage_ptr = std::unique_ptr<T[], void (*)(T*)>;

  /**
   * @brief Returns the maximum number of elements that can be allocated for T.
   */
  static constexpr auto max_elements() noexcept -> size_t { return std::numeric_limits<size_t>::max() / sizeof(T); }

  /**
   * @brief Releases raw storage previously obtained from allocate().
   * @param ptr Pointer to release (may be null).
   */
  static auto deallocate(T* ptr) noexcept -> void { ::operator delete[](ptr); }

  /**
   * @brief Allocates uninitialized storage for the given number of elements.
   * @param capacity Number of elements to reserve space for.
   * @return Owning pointer to the newly allocated storage.
   * @throws ArrayOverflowException if capacity exceeds max_elements().
   */
  static auto allocate(size_t capacity) -> storage_ptr;

  /**
   * @brief Converts a logical index to a physical index in the buffer.
   * @param logical_index The logical index (gap-skipping).
   * @return The physical index in the buffer.
   */
  [[nodiscard]] auto to_physical_index(size_t logical_index) const noexcept -> size_t;

  /**
   * @brief Returns the number of free slots in the gap.
   */
  [[nodiscard]] auto gap_size() const noexcept -> size_t;

  /**
   * @brief Ensures the gap has room for at least one element, growing if needed.
   */
  auto ensure_gap() -> void;

  /**
   * @brief Reallocates to new_capacity, repositioning the gap at the cursor.
   * @param new_capacity The new total capacity.
   */
  auto reallocate(size_t new_capacity) -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  storage_ptr data_;      ///< Raw storage for elements and the gap.
  size_t      capacity_;  ///< Total slots (elements + gap).
  size_t      gap_begin_; ///< Physical index where the gap starts (= cursor).
  size_t      gap_end_;   ///< Physical index just past the gap.

  static constexpr size_t kInitialCapacity = 16; ///< Default initial capacity.
  static constexpr size_t kGrowthFactor    = 2;  ///< Growth factor for resizing.
  static constexpr size_t kMinCapacity     = 8;  ///< Minimum capacity to maintain.
};

} // namespace ads::arrays

// Include the implementation file for templates.
#include "../../../src/ads/arrays/Gap_Buffer.tpp"

#endif // GAP_BUFFER_HPP

//===---------------------------------------------------------------------------===//
