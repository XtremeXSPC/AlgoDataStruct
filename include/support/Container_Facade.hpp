//===---------------------------------------------------------------------------===//
/**
 * @file Container_Facade.hpp
 * @author Costantino Lombardi
 * @brief CRTP facade providing derived sequence operations for containers.
 * @version 0.1
 * @date 2026-06-08
 *
 * @details Supplies the mechanically derivable portion of a sequence container's
 *          interface (emptiness query, const iterators, reverse iterators, and the
 *          relational operators) in terms of a minimal core that each derived
 *          container must implement: begin(), end() and size(). Containers mix this
 *          in via the Curiously Recurring Template Pattern to avoid duplicating
 *          these uniform definitions. Shared across the array and queue categories.
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef CONTAINER_FACADE_HPP
#define CONTAINER_FACADE_HPP

#include <algorithm>
#include <compare>
#include <concepts>
#include <iterator>

namespace ads::support {

/**
 * @brief CRTP base deriving common sequence operations from a minimal core.
 *
 * @details A derived container provides only begin(), end() (both const and
 *          non-const overloads) and size(); this facade then supplies is_empty(),
 *          the const iterator pair, the four reverse-iterator accessors, and the
 *          equality and three-way comparison operators. The relational operators
 *          are declared as hidden friends so that argument-dependent lookup finds
 *          them only for the concrete derived type.
 *
 * @tparam Derived The container type mixing in this facade (CRTP self type).
 */
template <typename Derived>
class ContainerFacade {
public:
  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks whether the container holds no elements.
   * @return true if size() is zero, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool { return self().size() == 0; }

  //===-------------------------- ITERATOR OPERATIONS --------------------------===//

  /**
   * @brief Returns a const iterator to the beginning of the container.
   * @complexity Time O(1), Space O(1)
   */
  auto cbegin() const noexcept { return self().begin(); }

  /**
   * @brief Returns a const iterator to the end of the container.
   * @complexity Time O(1), Space O(1)
   */
  auto cend() const noexcept { return self().end(); }

  /**
   * @brief Returns a reverse iterator to the beginning (the last element).
   * @complexity Time O(1), Space O(1)
   */
  auto rbegin() noexcept { return std::reverse_iterator(self().end()); }

  /**
   * @brief Returns a reverse iterator to the end (before the first element).
   * @complexity Time O(1), Space O(1)
   */
  auto rend() noexcept { return std::reverse_iterator(self().begin()); }

  /**
   * @brief Returns a const reverse iterator to the beginning (the last element).
   * @complexity Time O(1), Space O(1)
   */
  auto rbegin() const noexcept { return std::reverse_iterator(self().end()); }

  /**
   * @brief Returns a const reverse iterator to the end (before the first element).
   * @complexity Time O(1), Space O(1)
   */
  auto rend() const noexcept { return std::reverse_iterator(self().begin()); }

  /**
   * @brief Returns a const reverse iterator to the beginning (the last element).
   * @complexity Time O(1), Space O(1)
   */
  auto crbegin() const noexcept { return std::reverse_iterator(self().end()); }

  /**
   * @brief Returns a const reverse iterator to the end (before the first element).
   * @complexity Time O(1), Space O(1)
   */
  auto crend() const noexcept { return std::reverse_iterator(self().begin()); }

  //===------------------------- COMPARISON OPERATORS --------------------------===//

  /**
   * @brief Equality comparison between two derived containers.
   *
   * @details Declared as a templated hidden friend whose constraint references
   *          @c Derived. The @c std::same_as clause keeps each instantiation's
   *          operator distinct (avoiding ODR redefinition across derived types),
   *          while deferring the element-type constraint until the operator is
   *          used keeps it SFINAE-friendly under CRTP, where @c Derived is still
   *          incomplete when this base is instantiated.
   *
   * @tparam D Deduced derived container type (must be @c Derived).
   * @param lhs The left-hand container.
   * @param rhs The right-hand container.
   * @return true if both contain equal elements in the same order.
   * @complexity Time O(n), Space O(1)
   */
  template <typename D>
  requires std::same_as<D, Derived> && std::equality_comparable<typename D::value_type>
  friend auto operator==(const D& lhs, const D& rhs) -> bool {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  /**
   * @brief Three-way comparison between two derived containers.
   *
   * @tparam D Deduced derived container type (must be @c Derived).
   * @param lhs The left-hand container.
   * @param rhs The right-hand container.
   * @return The lexicographical ordering of the two element sequences.
   * @complexity Time O(n), Space O(1)
   */
  template <typename D>
  requires std::same_as<D, Derived> && std::three_way_comparable<typename D::value_type>
  friend auto operator<=>(const D& lhs, const D& rhs) {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Returns a reference to the derived instance.
   */
  auto self() noexcept -> Derived& { return static_cast<Derived&>(*this); }

  /**
   * @brief Returns a const reference to the derived instance.
   */
  auto self() const noexcept -> const Derived& { return static_cast<const Derived&>(*this); }
};

} // namespace ads::support

#endif // CONTAINER_FACADE_HPP

//===---------------------------------------------------------------------------===//
