//===---------------------------------------------------------------------------===//
/**
 * @file Algebra_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Syntactic concepts for reusable algebraic policies.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef ALGEBRA_CONCEPTS_HPP
#define ALGEBRA_CONCEPTS_HPP

#include <concepts>
#include <cstddef>

namespace ads::algebra {

//===----- FUNDAMENTAL ALGEBRA CONCEPTS ---------------------------------------===//

/**
 * @brief Policy exposing a closed associative binary operation.
 * @note Associativity is a semantic requirement and cannot be checked by a concept.
 * @tparam Policy Candidate algebra policy.
 */
template <typename Policy> concept SemigroupPolicy = requires {
  typename Policy::value_type;
} && requires(const Policy& policy, const typename Policy::value_type& left, const typename Policy::value_type& right) {
  { policy.combine(left, right) } -> std::same_as<typename Policy::value_type>;
};

/**
 * @brief Semigroup policy explicitly declaring that its operation is commutative.
 * @tparam Policy Candidate algebra policy.
 */
template <typename Policy> concept CommutativeSemigroupPolicy = SemigroupPolicy<Policy> && requires {
  { Policy::is_commutative } -> std::convertible_to<bool>;
} && static_cast<bool>(Policy::is_commutative);

/**
 * @brief Semigroup policy explicitly declaring that its operation is idempotent.
 * @tparam Policy Candidate algebra policy.
 */
template <typename Policy> concept IdempotentSemigroupPolicy = SemigroupPolicy<Policy> && requires {
  { Policy::is_idempotent } -> std::convertible_to<bool>;
} && static_cast<bool>(Policy::is_idempotent);

/**
 * @brief Semigroup policy additionally exposing a two-sided identity element.
 * @note The identity laws are semantic requirements and cannot be checked by a concept.
 * @tparam Policy Candidate algebra policy.
 */
template <typename Policy> concept MonoidPolicy = SemigroupPolicy<Policy> && requires(const Policy& policy) {
  { policy.identity() } -> std::same_as<typename Policy::value_type>;
};

/**
 * @brief Monoid policy additionally exposing a two-sided inverse operation.
 * @note The inverse laws are semantic requirements and cannot be checked by a concept.
 * @tparam Policy Candidate algebra policy.
 */
template <typename Policy> concept GroupPolicy =
    MonoidPolicy<Policy> && requires(const Policy& policy, const typename Policy::value_type& value) {
      { policy.inverse(value) } -> std::same_as<typename Policy::value_type>;
    };

/**
 * @brief Group policy explicitly declaring that its operation is commutative.
 * @tparam Policy Candidate algebra policy.
 */
template <typename Policy> concept CommutativeGroupPolicy = GroupPolicy<Policy> && CommutativeSemigroupPolicy<Policy>;

//===----- MONOID-ACTION CONCEPTS ---------------------------------------------===//

/**
 * @brief Policy describing a length-aware monoid action on another monoid.
 *
 * @details Composition is chronological: @c compose(existing,fresh) represents
 *          applying @c existing first and @c fresh second. In addition to the two
 *          underlying monoid laws, a valid policy must satisfy:
 *          @code
 *          apply(combine(a,b), action, la+lb)
 *            == combine(apply(a,action,la), apply(b,action,lb))
 *          apply(apply(value,existing,len), fresh, len)
 *            == apply(value,compose(existing,fresh),len)
 *          @endcode
 *          These laws are semantic and cannot be checked by the concept.
 *
 * @tparam Policy Candidate acted-monoid policy.
 */
template <typename Policy> concept ActedMonoidPolicy =
    requires {
      typename Policy::value_monoid_type;
      typename Policy::action_monoid_type;
      typename Policy::value_type;
      typename Policy::action_type;
    } && MonoidPolicy<typename Policy::value_monoid_type> && MonoidPolicy<typename Policy::action_monoid_type>
    && requires(
        const Policy&                       policy,
        const typename Policy::value_type&  value,
        const typename Policy::value_type&  other,
        const typename Policy::action_type& existing,
        const typename Policy::action_type& fresh,
        std::size_t                         length) {
         { policy.value_identity() } -> std::same_as<typename Policy::value_type>;
         { policy.action_identity() } -> std::same_as<typename Policy::action_type>;
         { policy.combine(value, other) } -> std::same_as<typename Policy::value_type>;
         { policy.compose(existing, fresh) } -> std::same_as<typename Policy::action_type>;
         { policy.apply(value, fresh, length) } -> std::same_as<typename Policy::value_type>;
       };

} // namespace ads::algebra

#endif // ALGEBRA_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
