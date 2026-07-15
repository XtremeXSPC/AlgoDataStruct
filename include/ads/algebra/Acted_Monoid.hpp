//===---------------------------------------------------------------------------===//
/**
 * @file Acted_Monoid.hpp
 * @author Costantino Lombardi
 * @brief Composition of a value monoid, an action monoid, and an action applicator.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef ACTED_MONOID_HPP
#define ACTED_MONOID_HPP

#include "Algebra_Concepts.hpp"

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace ads::algebra {

/**
 * @brief Bridges a value monoid, an action monoid, and a length-aware applicator.
 *
 * @details Policies are stored as objects, so both zero-state static policies and
 *          stateful policies are supported. Empty policies occupy no storage through
 *          @c [[no_unique_address]]. Composition is chronological: the existing action
 *          is applied before the fresh action. This convention lets lazy structures
 *          accumulate actions without knowing their concrete representation.
 *
 *          A valid applicator must distribute an action over the value monoid and
 *          respect action composition; these semantic laws are described by
 *          @ref ActedMonoidPolicy.
 *
 * @tparam ValueMonoid Monoid used to aggregate stored values.
 * @tparam ActionMonoid Monoid used to compose deferred actions.
 * @tparam Applicator Length-aware operation applying an action to an aggregate.
 */
template <typename ValueMonoid, typename ActionMonoid, typename Applicator>
requires MonoidPolicy<ValueMonoid> && MonoidPolicy<ActionMonoid>
         && requires(
             const Applicator&                        applicator,
             const typename ValueMonoid::value_type&  value,
             const typename ActionMonoid::value_type& action,
             std::size_t                              length) {
              { applicator.apply(value, action, length) } -> std::same_as<typename ValueMonoid::value_type>;
            }
class ActedMonoid {
public:
  using value_monoid_type  = ValueMonoid;
  using action_monoid_type = ActionMonoid;
  using applicator_type    = Applicator;
  using value_type         = typename ValueMonoid::value_type;
  using action_type        = typename ActionMonoid::value_type;

  //===----- CONSTRUCTORS ------------------------------------------------------===//

  /**
   * @brief Constructs the policy from default-constructed components.
   * @complexity Time O(1), Space O(1)
   */
  constexpr ActedMonoid() requires(std::default_initializable<ValueMonoid> && std::default_initializable<ActionMonoid>
                                   && std::default_initializable<Applicator>)
  = default;

  /**
   * @brief Constructs the policy from explicit algebra and applicator objects.
   * @param value_monoid Monoid used to aggregate values.
   * @param action_monoid Monoid used to compose actions chronologically.
   * @param applicator Length-aware action applicator.
   * @complexity Time O(1), Space O(1)
   */
  constexpr explicit ActedMonoid(ValueMonoid value_monoid, ActionMonoid action_monoid = {}, Applicator applicator = {}) noexcept(
      std::is_nothrow_move_constructible_v<ValueMonoid> && std::is_nothrow_move_constructible_v<ActionMonoid>
      && std::is_nothrow_move_constructible_v<Applicator>) :
      value_monoid_(std::move(value_monoid)),
      action_monoid_(std::move(action_monoid)),
      applicator_(std::move(applicator)) {}

  //===----- ALGEBRA OPERATIONS -----------------------------------------------===//

  ///@brief Returns the identity aggregate of the value monoid.
  [[nodiscard]] constexpr auto value_identity() const noexcept(noexcept(value_monoid_.identity())) -> value_type {
    return value_monoid_.identity();
  }

  ///@brief Returns the identity action of the action monoid.
  [[nodiscard]] constexpr auto action_identity() const noexcept(noexcept(action_monoid_.identity())) -> action_type {
    return action_monoid_.identity();
  }

  ///@brief Combines two value aggregates in left-to-right order.
  [[nodiscard]] constexpr auto combine(const value_type& left, const value_type& right) const
      noexcept(noexcept(value_monoid_.combine(left, right))) -> value_type {
    return value_monoid_.combine(left, right);
  }

  ///@brief Composes @p existing followed chronologically by @p fresh.
  [[nodiscard]] constexpr auto compose(const action_type& existing, const action_type& fresh) const
      noexcept(noexcept(action_monoid_.combine(existing, fresh))) -> action_type {
    return action_monoid_.combine(existing, fresh);
  }

  ///@brief Applies @p action to an aggregate representing @p length elements.
  [[nodiscard]] constexpr auto apply(const value_type& value, const action_type& action, std::size_t length) const
      noexcept(noexcept(applicator_.apply(value, action, length))) -> value_type {
    return applicator_.apply(value, action, length);
  }

  //===----- POLICY ACCESSORS --------------------------------------------------===//

  ///@brief Returns the stored value-monoid policy.
  [[nodiscard]] constexpr auto value_monoid() const noexcept -> const ValueMonoid& { return value_monoid_; }

  ///@brief Returns the stored action-monoid policy.
  [[nodiscard]] constexpr auto action_monoid() const noexcept -> const ActionMonoid& { return action_monoid_; }

  ///@brief Returns the stored action applicator.
  [[nodiscard]] constexpr auto applicator() const noexcept -> const Applicator& { return applicator_; }

private:
  //===----- DATA MEMBERS ------------------------------------------------------===//

  [[no_unique_address]] ValueMonoid  value_monoid_;  ///< Value aggregation policy.
  [[no_unique_address]] ActionMonoid action_monoid_; ///< Chronological action-composition policy.
  [[no_unique_address]] Applicator   applicator_;    ///< Length-aware action applicator.
};

} // namespace ads::algebra

#endif // ACTED_MONOID_HPP

//===---------------------------------------------------------------------------===//
