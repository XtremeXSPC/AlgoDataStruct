//===---------------------------------------------------------------------------===//
/**
 * @file Range_Actions.hpp
 * @author Costantino Lombardi
 * @brief Ready-to-use acted monoids for common range updates and queries.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef RANGE_ACTIONS_HPP
#define RANGE_ACTIONS_HPP

#include "Acted_Monoid.hpp"
#include "Basic_Monoids.hpp"

#include <cstddef>

namespace ads::algebra {

//===----- RANGE-ADDITION POLICIES --------------------------------------------===//

/**
 * @brief Applies an additive action to a segment sum.
 * @tparam T Aggregate and action value type.
 */
template <typename T>
struct RangeAddSumApplicator {
  ///@brief Adds @p action once for every element represented by @p value.
  ///@note For integral @p T the product @c action*length can overflow; the
  ///      applicator inherits the wrap-around or UB semantics of @p T.
  [[nodiscard]] static constexpr auto apply(const T& value, const T& action, std::size_t length) -> T {
    return value + action * static_cast<T>(length);
  }
};

/**
 * @brief Acted monoid for range addition and range-sum queries.
 * @tparam T Aggregate, element, and action value type.
 */
template <typename T>
using RangeAddRangeSum = ActedMonoid<AddMonoid<T>, AddMonoid<T>, RangeAddSumApplicator<T>>;

/**
 * @brief Applies an additive action to a segment minimum.
 * @tparam T Aggregate and action value type.
 */
template <typename T>
struct RangeAddMinApplicator {
  ///@brief Shifts a non-empty minimum aggregate by @p action.
  ///@warning The guard treats @c MinMonoid identity (the greatest representable
  ///         value) as "empty range": stored values must never equal that
  ///         sentinel, or they silently stop receiving range additions.
  [[nodiscard]] static constexpr auto apply(const T& value, const T& action, std::size_t) -> T {
    return (value == MinMonoid<T>::identity()) ? value : value + action;
  }
};

/**
 * @brief Acted monoid for range addition and range-minimum queries.
 * @tparam T Aggregate, element, and action value type.
 */
template <typename T>
using RangeAddRangeMin = ActedMonoid<MinMonoid<T>, AddMonoid<T>, RangeAddMinApplicator<T>>;

/**
 * @brief Applies an additive action to a segment maximum.
 * @tparam T Aggregate and action value type.
 */
template <typename T>
struct RangeAddMaxApplicator {
  ///@brief Shifts a non-empty maximum aggregate by @p action.
  ///@warning The guard treats @c MaxMonoid identity (the least representable
  ///         value) as "empty range": stored values must never equal that
  ///         sentinel, or they silently stop receiving range additions.
  [[nodiscard]] static constexpr auto apply(const T& value, const T& action, std::size_t) -> T {
    return (value == MaxMonoid<T>::identity()) ? value : value + action;
  }
};

/**
 * @brief Acted monoid for range addition and range-maximum queries.
 * @tparam T Aggregate, element, and action value type.
 */
template <typename T>
using RangeAddRangeMax = ActedMonoid<MaxMonoid<T>, AddMonoid<T>, RangeAddMaxApplicator<T>>;

//===----- RANGE-ASSIGNMENT POLICIES ------------------------------------------===//

/**
 * @brief Applies an optional assignment action to a segment sum.
 * @tparam T Aggregate, element, and assigned value type.
 */
template <typename T>
struct RangeAssignSumApplicator {
  using action_type = typename AssignMonoid<T>::value_type;

  ///@brief Replaces the aggregate when @p action contains an assignment.
  [[nodiscard]] static constexpr auto apply(const T& value, const action_type& action, std::size_t length) -> T {
    return action.has_value() ? (*action) * static_cast<T>(length) : value;
  }
};

/**
 * @brief Acted monoid for range assignment and range-sum queries.
 * @tparam T Aggregate, element, and assigned value type.
 */
template <typename T>
using RangeAssignRangeSum = ActedMonoid<AddMonoid<T>, AssignMonoid<T>, RangeAssignSumApplicator<T>>;

//===----- RANGE-AFFINE POLICIES ----------------------------------------------===//

/**
 * @brief Applies an affine transformation to a segment sum.
 * @tparam T Aggregate, element, and coefficient type.
 */
template <typename T>
struct RangeAffineSumApplicator {
  using action_type = AffineTransform<T>;

  ///@brief Applies @p action to the sum of @p length elements.
  ///@note For integral @p T the products can overflow; the applicator inherits
  ///      the wrap-around or UB semantics of @p T.
  [[nodiscard]] static constexpr auto apply(const T& value, const action_type& action, std::size_t length) -> T {
    return value * action.scale + static_cast<T>(length) * action.shift;
  }
};

/**
 * @brief Acted monoid for affine range transformations and range-sum queries.
 * @tparam T Aggregate, element, and coefficient type.
 */
template <typename T>
using RangeAffineRangeSum = ActedMonoid<AddMonoid<T>, AffineMonoid<T>, RangeAffineSumApplicator<T>>;

} // namespace ads::algebra

#endif // RANGE_ACTIONS_HPP

//===---------------------------------------------------------------------------===//
