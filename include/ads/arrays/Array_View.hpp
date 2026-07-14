//===---------------------------------------------------------------------------===//
/**
 * @file Array_View.hpp
 * @author Costantino Lombardi
 * @brief Declaration and definition of the ArrayView class template.
 * @version 0.1
 * @date 2026-06-08
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef ARRAY_VIEW_HPP
#define ARRAY_VIEW_HPP

#include "../../support/Container_Facade.hpp"
#include "Array_Exception.hpp"

#include <concepts>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace ads::arrays {

using ads::support::ContainerFacade;

/**
 * @brief A non-owning, copyable view over a contiguous sequence of elements.
 *
 * @details ArrayView is a lightweight {pointer, length} pair, analogous to
 *          std::span, that refers to memory owned elsewhere (a StaticArray,
 *          DynamicArray, StaticVector, or a raw array). It performs no allocation
 *          and copies cheaply by value; const-ness is shallow (a const ArrayView
 *          still grants mutable access to its elements when T is non-const).
 *          Use ArrayView<const T> for a read-only view. As a non-owning reference,
 *          it must not outlive the storage it observes.
 *
 *          ArrayView only views contiguous storage; it cannot view a CircularArray.
 *
 * @tparam T The (possibly const-qualified) element type being viewed.
 *
 * @note operator== (from ContainerFacade) compares the viewed ELEMENTS,
 *       not the identity of the underlying storage - two views over
 *       different buffers with equal contents compare equal (std::span,
 *       by contrast, has no operator==).
 */
template <typename T>
class ArrayView : public ContainerFacade<ArrayView<T>> {
public:
  using element_type    = T;
  using value_type      = std::remove_cv_t<T>;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = T&;
  using const_reference = const T&;
  using pointer         = T*;
  using const_pointer   = const T*;
  using iterator        = T*;
  using const_iterator  = const T*;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty view (null pointer, zero length).
   * @complexity Time O(1), Space O(1)
   */
  constexpr ArrayView() noexcept : data_(nullptr), size_(0) {}

  /**
   * @brief Constructs a view from a pointer and a length.
   * @param data Pointer to the first element.
   * @param size Number of elements in the view.
   * @complexity Time O(1), Space O(1)
   */
  constexpr ArrayView(T* data, size_t size) noexcept : data_(data), size_(size) {}

  /**
   * @brief Constructs a view from a pointer range [first, last).
   * @param first Pointer to the first element.
   * @param last Pointer past the last element.
   * @complexity Time O(1), Space O(1)
   */
  constexpr ArrayView(T* first, T* last) noexcept : data_(first), size_(static_cast<size_t>(last - first)) {}

  /**
   * @brief Constructs a view over any contiguous container exposing data()/size().
   * @tparam Container A container type with data() convertible to T* and size().
   * @param container The container to view.
   * @complexity Time O(1), Space O(1)
   */
  template <typename Container>
  requires(!std::same_as<std::remove_cvref_t<Container>, ArrayView>) && requires(Container& c) {
    { c.data() } -> std::convertible_to<T*>;
    { c.size() } -> std::convertible_to<size_t>;
  }
  constexpr ArrayView(Container& container) noexcept : data_(container.data()), size_(container.size()) {}

  // ArrayView is a trivially copyable value type (non-owning).
  constexpr ArrayView(const ArrayView&)                        = default;
  constexpr ArrayView(ArrayView&&) noexcept                    = default;
  constexpr auto operator=(const ArrayView&) -> ArrayView&     = default;
  constexpr auto operator=(ArrayView&&) noexcept -> ArrayView& = default;
  ~ArrayView()                                                 = default;

  //===----- ACCESS OPERATIONS -------------------------------------------------===//

  /**
   * @brief Accesses an element without bounds checking.
   * @param index The index of the element.
   * @return Reference to the element.
   * @complexity Time O(1), Space O(1)
   */
  constexpr auto operator[](size_t index) const -> T& { return data_[index]; }

  /**
   * @brief Accesses an element with bounds checking.
   * @param index The index of the element.
   * @return Reference to the element.
   * @throws ArrayOutOfRangeException if index >= size().
   * @complexity Time O(1), Space O(1)
   */
  constexpr auto at(size_t index) const -> T& {
    if (index >= size_) {
      throw ArrayOutOfRangeException("ArrayView index out of range");
    }
    return data_[index];
  }

  /**
   * @brief Returns a reference to the first element.
   * @throws ArrayUnderflowException if the view is empty.
   * @complexity Time O(1), Space O(1)
   */
  constexpr auto front() const -> T& {
    if (size_ == 0) {
      throw ArrayUnderflowException("front on empty array view");
    }
    return data_[0];
  }

  /**
   * @brief Returns a reference to the last element.
   * @throws ArrayUnderflowException if the view is empty.
   * @complexity Time O(1), Space O(1)
   */
  constexpr auto back() const -> T& {
    if (size_ == 0) {
      throw ArrayUnderflowException("back on empty array view");
    }
    return data_[size_ - 1];
  }

  /**
   * @brief Returns the underlying data pointer.
   * @complexity Time O(1), Space O(1)
   */
  constexpr auto data() const noexcept -> T* { return data_; }

  //===----- SUBVIEW OPERATIONS ------------------------------------------------===//

  /**
   * @brief Returns a view of the first count elements.
   * @param count Number of leading elements to view.
   * @throws ArrayOutOfRangeException if count > size().
   * @complexity Time O(1), Space O(1)
   */
  constexpr auto first(size_t count) const -> ArrayView {
    if (count > size_) {
      throw ArrayOutOfRangeException("ArrayView::first count out of range");
    }
    return ArrayView(data_, count);
  }

  /**
   * @brief Returns a view of the last count elements.
   * @param count Number of trailing elements to view.
   * @throws ArrayOutOfRangeException if count > size().
   * @complexity Time O(1), Space O(1)
   */
  constexpr auto last(size_t count) const -> ArrayView {
    if (count > size_) {
      throw ArrayOutOfRangeException("ArrayView::last count out of range");
    }
    return ArrayView(data_ + (size_ - count), count);
  }

  /**
   * @brief Returns a sub-view starting at offset spanning count elements.
   * @param offset Index of the first element of the sub-view.
   * @param count Number of elements in the sub-view.
   * @throws ArrayOutOfRangeException if offset + count > size().
   * @complexity Time O(1), Space O(1)
   */
  constexpr auto subview(size_t offset, size_t count) const -> ArrayView {
    if (offset > size_ || count > size_ - offset) {
      throw ArrayOutOfRangeException("ArrayView::subview range out of range");
    }
    return ArrayView(data_ + offset, count);
  }

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Returns the number of elements in the view.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] constexpr auto size() const noexcept -> size_t { return size_; }

  // is_empty(), cbegin/cend, rbegin/rend, crbegin/crend, and the relational
  // operators (==, <=>) are inherited from ContainerFacade<ArrayView<T>>.

  //===----- ITERATOR OPERATIONS -----------------------------------------------===//

  /// @brief Returns an iterator to the beginning of the view.
  constexpr auto begin() const noexcept -> iterator { return data_; }

  /// @brief Returns an iterator to the end of the view.
  constexpr auto end() const noexcept -> iterator { return data_ + size_; }

private:
  //===----- DATA MEMBERS ------------------------------------------------------===//

  T*     data_; ///< Pointer to the first viewed element (not owned).
  size_t size_; ///< Number of viewed elements.
};

//===----- DEDUCTION GUIDES ----------------------------------------------------===//

/// Deduce ArrayView<T> from a contiguous container exposing data()/size().
template <typename Container>
ArrayView(Container&) -> ArrayView<std::remove_pointer_t<decltype(std::declval<Container&>().data())>>;

/// Deduce ArrayView<T> from a pointer and a length.
template <typename T>
ArrayView(T*, size_t) -> ArrayView<T>;

} // namespace ads::arrays

#endif // ARRAY_VIEW_HPP

//===---------------------------------------------------------------------------===//
