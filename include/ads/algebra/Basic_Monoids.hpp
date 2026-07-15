//===---------------------------------------------------------------------------===//
/**
 * @file Basic_Monoids.hpp
 * @author Costantino Lombardi
 * @brief Fundamental semigroup, monoid, and group policies.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef BASIC_MONOIDS_HPP
#define BASIC_MONOIDS_HPP

#include <concepts>
#include <limits>
#include <numeric>
#include <optional>

namespace ads::algebra {

//===----- ADDITIVE POLICIES ---------------------------------------------------===//

/**
 * @brief Additive monoid using value initialization as the identity.
 *
 * @details The policy assumes that @c T{} is a two-sided identity for @c operator+.
 *          Associativity and identity laws are semantic requirements.
 *
 * @tparam T Value type closed under addition.
 */
template <typename T>
struct AddMonoid {
  using value_type = T;

  static constexpr bool is_commutative = true;
  static constexpr bool is_idempotent  = false;

  ///@brief Returns the additive identity.
  [[nodiscard]] static constexpr auto identity() noexcept(noexcept(value_type{})) -> value_type { return value_type{}; }

  ///@brief Adds @p left and @p right.
  [[nodiscard]] static constexpr auto combine(const value_type& left, const value_type& right) noexcept(noexcept(left + right))
      -> value_type {
    return left + right;
  }
};

/**
 * @brief Additive commutative group extending @ref AddMonoid.
 * @tparam T Value type supporting addition and unary negation.
 */
template <typename T>
struct AddGroup : AddMonoid<T> {
  using value_type = T;

  ///@brief Returns the additive inverse of @p value.
  [[nodiscard]] static constexpr auto inverse(const value_type& value) noexcept(noexcept(-value)) -> value_type { return -value; }
};

//===----- ORDER-BASED POLICIES ------------------------------------------------===//

/**
 * @brief Idempotent minimum semigroup without an artificial identity.
 * @tparam T Value type ordered by @c operator<.
 */
template <typename T>
struct MinSemigroup {
  using value_type = T;

  static constexpr bool is_commutative = true;
  static constexpr bool is_idempotent  = true;

  ///@brief Returns the smaller of @p left and @p right.
  [[nodiscard]] static constexpr auto combine(const value_type& left, const value_type& right) noexcept(noexcept(right < left))
      -> value_type {
    return (right < left) ? right : left;
  }
};

/**
 * @brief Idempotent maximum semigroup without an artificial identity.
 * @tparam T Value type ordered by @c operator<.
 */
template <typename T>
struct MaxSemigroup {
  using value_type = T;

  static constexpr bool is_commutative = true;
  static constexpr bool is_idempotent  = true;

  ///@brief Returns the greater of @p left and @p right.
  [[nodiscard]] static constexpr auto combine(const value_type& left, const value_type& right) noexcept(noexcept(left < right))
      -> value_type {
    return (left < right) ? right : left;
  }
};

/**
 * @brief Minimum monoid using the greatest representable value as its identity.
 * @warning The identity doubles as an empty-range sentinel for the range-add
 *          applicators: stored values must never equal
 *          @c std::numeric_limits<T>::max(), or range additions will silently
 *          skip them (see @ref RangeAddMinApplicator).
 * @tparam T Value type with a specialized @c std::numeric_limits.
 */
template <typename T>
requires std::numeric_limits<T>::is_specialized
struct MinMonoid : MinSemigroup<T> {
  using value_type = T;

  ///@brief Returns the greatest representable value.
  [[nodiscard]] static constexpr auto identity() noexcept -> value_type { return std::numeric_limits<value_type>::max(); }
};

/**
 * @brief Maximum monoid using the least representable value as its identity.
 * @warning The identity doubles as an empty-range sentinel for the range-add
 *          applicators: stored values must never equal
 *          @c std::numeric_limits<T>::lowest(), or range additions will
 *          silently skip them (see @ref RangeAddMaxApplicator).
 * @tparam T Value type with a specialized @c std::numeric_limits.
 */
template <typename T>
requires std::numeric_limits<T>::is_specialized
struct MaxMonoid : MaxSemigroup<T> {
  using value_type = T;

  ///@brief Returns the least representable value.
  [[nodiscard]] static constexpr auto identity() noexcept -> value_type { return std::numeric_limits<value_type>::lowest(); }
};

//===----- NUMBER-THEORETIC POLICIES ------------------------------------------===//

/**
 * @brief Greatest-common-divisor monoid over integral values.
 * @tparam T Integral value type accepted by @c std::gcd.
 */
template <std::integral T>
requires(!std::same_as<T, bool>)
struct GcdMonoid {
  using value_type = T;

  static constexpr bool is_commutative = true;
  static constexpr bool is_idempotent  = true;

  ///@brief Returns zero, the identity of the GCD operation.
  [[nodiscard]] static constexpr auto identity() noexcept -> value_type { return value_type{}; }

  ///@brief Returns the greatest common divisor of @p left and @p right.
  [[nodiscard]] static constexpr auto combine(const value_type& left, const value_type& right) noexcept -> value_type {
    return std::gcd(left, right);
  }
};

//===----- ACTION POLICIES -----------------------------------------------------===//

/**
 * @brief Assignment-action monoid where @c std::nullopt is the identity.
 *
 * @details Combining @c existing with @c fresh keeps the fresh assignment when
 *          present, thereby modelling chronological overwrite semantics.
 *
 * @tparam T Assigned value type.
 */
template <typename T>
struct AssignMonoid {
  using value_type = std::optional<T>;

  static constexpr bool is_commutative = false;
  static constexpr bool is_idempotent  = true;

  ///@brief Returns the no-assignment action.
  [[nodiscard]] static constexpr auto identity() noexcept -> value_type { return std::nullopt; }

  ///@brief Composes @p existing followed by @p fresh.
  [[nodiscard]] static constexpr auto combine(const value_type& existing, const value_type& fresh) -> value_type {
    return fresh.has_value() ? fresh : existing;
  }
};

/**
 * @brief Affine transformation @c f(x) = scale*x + shift.
 * @tparam T Coefficient and operand type.
 */
template <typename T>
struct AffineTransform {
  T scale{1}; ///< Multiplicative coefficient.
  T shift{};  ///< Additive coefficient.

  ///@brief Compares both affine coefficients.
  friend constexpr auto operator==(const AffineTransform&, const AffineTransform&) -> bool = default;
};

/**
 * @brief Monoid of affine transformations.
 *
 * @details @c combine(existing,fresh) applies @c existing first and @c fresh
 *          second. Function composition is associative but generally non-commutative.
 *
 * @tparam T Coefficient type supporting addition and multiplication.
 */
template <typename T>
struct AffineMonoid {
  using value_type = AffineTransform<T>;

  static constexpr bool is_commutative = false;
  static constexpr bool is_idempotent  = false;

  ///@brief Returns the identity transformation @c f(x)=x.
  [[nodiscard]] static constexpr auto identity() -> value_type { return {}; }

  ///@brief Composes @p existing followed chronologically by @p fresh.
  [[nodiscard]] static constexpr auto combine(const value_type& existing, const value_type& fresh) -> value_type {
    return {
        existing.scale * fresh.scale,
        existing.shift * fresh.scale + fresh.shift,
    };
  }
};

} // namespace ads::algebra

#endif // BASIC_MONOIDS_HPP

//===---------------------------------------------------------------------------===//
