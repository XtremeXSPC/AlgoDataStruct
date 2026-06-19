//===---------------------------------------------------------------------------===//
/**
 * @file Indexed_Iterator.hpp
 * @author Costantino Lombardi
 * @brief Reusable random-access proxy iterator over a container's operator[].
 * @version 0.1
 * @date 2026-06-08
 *
 * @details Provides the iteration boilerplate shared by container structures whose
 *          logical element order is not linear in memory (a circular buffer wraps
 *          around; a gap buffer skips an internal gap), so a raw pointer cannot
 *          represent the sequence. Such a container exposes value_type and a logical
 *          operator[]; IndexedIterator holds a logical index plus a back-pointer and
 *          resolves the element through that subscript on each access. Contiguous
 *          structures keep using plain pointer iterators. Shared across the array and
 *          queue categories.
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef INDEXED_ITERATOR_HPP
#define INDEXED_ITERATOR_HPP

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace ads::support {

/**
 * @brief Random-access iterator that addresses a container by logical index.
 *
 * @details The iterator stores a logical index and a pointer to the container,
 *          and dereferences via the container's operator[]. This lets it traverse
 *          containers whose logical order differs from their physical memory layout
 *          (wrap-around buffers, gap buffers) while remaining a model of
 *          std::random_access_iterator. The container must expose a @c value_type
 *          member type and both const and non-const @c operator[](size_t).
 *
 * @tparam Container The owning container type.
 * @tparam IsConst   Whether this is a const iterator.
 */
template <typename Container, bool IsConst>
class IndexedIterator {
public:
  using container_type    = std::conditional_t<IsConst, const Container, Container>;
  using iterator_category = std::random_access_iterator_tag;
  using value_type        = typename Container::value_type;
  using difference_type   = std::ptrdiff_t;
  using pointer           = std::conditional_t<IsConst, const value_type*, value_type*>;
  using reference         = std::conditional_t<IsConst, const value_type&, value_type&>;

  IndexedIterator() = default;

  /**
   * @brief Constructs an iterator at a logical index into a container.
   * @param index The logical index.
   * @param container Pointer to the owning container.
   */
  IndexedIterator(difference_type index, container_type* container) : index_(index), container_(container) {}

  /**
   * @brief Converting constructor from iterator to const_iterator.
   * @details Templated so it is never treated as a copy constructor (which would
   *          otherwise suppress the implicitly generated copy constructor and make
   *          the iterator non-copyable).
   * @tparam RHSConst Const-ness of the source iterator (must be non-const).
   * @param other The iterator to convert from.
   */
  template <bool RHSConst>
  requires(IsConst && !RHSConst)
  IndexedIterator(const IndexedIterator<Container, RHSConst>& other) : index_(other.index_), container_(other.container_) {}

  //===----- DEREFERENCE OPERATORS ---------------------------------------------===//

  auto operator*() const -> reference { return (*container_)[static_cast<size_t>(index_)]; }

  auto operator->() const -> pointer { return &(*container_)[static_cast<size_t>(index_)]; }

  auto operator[](difference_type n) const -> reference { return (*container_)[static_cast<size_t>(index_ + n)]; }

  //===----- INCREMENT/DECREMENT OPERATORS -------------------------------------===//

  auto operator++() -> IndexedIterator& {
    ++index_;
    return *this;
  }

  auto operator++(int) -> IndexedIterator {
    IndexedIterator tmp = *this;
    ++index_;
    return tmp;
  }

  auto operator--() -> IndexedIterator& {
    --index_;
    return *this;
  }

  auto operator--(int) -> IndexedIterator {
    IndexedIterator tmp = *this;
    --index_;
    return tmp;
  }

  //===----- ARITHMETIC OPERATORS ----------------------------------------------===//

  auto operator+=(difference_type n) -> IndexedIterator& {
    index_ += n;
    return *this;
  }

  auto operator-=(difference_type n) -> IndexedIterator& {
    index_ -= n;
    return *this;
  }

  friend auto operator+(IndexedIterator it, difference_type n) -> IndexedIterator { return it += n; }

  friend auto operator+(difference_type n, IndexedIterator it) -> IndexedIterator { return it += n; }

  friend auto operator-(IndexedIterator it, difference_type n) -> IndexedIterator { return it -= n; }

  friend auto operator-(const IndexedIterator& lhs, const IndexedIterator& rhs) -> difference_type { return lhs.index_ - rhs.index_; }

  //===----- COMPARISON OPERATORS ----------------------------------------------===//

  auto operator<=>(const IndexedIterator& other) const noexcept        = default;
  auto operator==(const IndexedIterator& other) const noexcept -> bool = default;

private:
  difference_type index_     = 0;
  container_type* container_ = nullptr;

  // Grant the complementary const-ness specialization access for conversion.
  template <typename, bool>
  friend class IndexedIterator;
};

} // namespace ads::support

#endif // INDEXED_ITERATOR_HPP

//===---------------------------------------------------------------------------===//
