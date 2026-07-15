//===---------------------------------------------------------------------------===//
/**
 * @file Range_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Domain constraints for algebra-driven range-query structures.
 * @version 1.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef RANGE_CONCEPTS_HPP
#define RANGE_CONCEPTS_HPP

#include "../algebra/Algebra_Concepts.hpp"

#include <concepts>
#include <type_traits>

namespace ads::range {

//===----- VALUE AND LEAF CONCEPTS --------------------------------------------===//

/**
 * @brief A value type supporting the convenience point-add operation.
 * @tparam T Candidate value type.
 */
template <typename T> concept Addable = requires(const T& left, const T& right) {
  { left + right } -> std::convertible_to<T>;
};

/**
 * @brief Constructs a Segment Tree leaf aggregate from an external value.
 * @tparam Value External leaf value type.
 * @tparam Node Internal aggregate type.
 */
template <typename Value, typename Node>
struct DefaultLeafBuilder {
  ///@brief Converts @p value to the aggregate representation.
  [[nodiscard]] constexpr auto operator()(const Value& value) const noexcept(std::is_nothrow_constructible_v<Node, const Value&>) -> Node {
    return Node{value};
  }
};

//===----- STRUCTURE POLICY CONCEPTS ------------------------------------------===//

/**
 * @brief Complete algebra and leaf-construction contract for @ref SegmentTree.
 * @tparam Value External leaf value type.
 * @tparam Monoid Aggregate monoid policy.
 * @tparam LeafBuilder Callable converting leaves to aggregate values.
 */
template <typename Value, typename Monoid, typename LeafBuilder> concept SegmentTreePolicy =
    algebra::MonoidPolicy<Monoid> && requires(const LeafBuilder& leaf_builder, const Value& value) {
      { leaf_builder(value) } -> std::same_as<typename Monoid::value_type>;
    };

/**
 * @brief Complete algebra and storage contract for lazy range structures.
 *
 * @details Values and actions are copied while a modification is staged and are
 *          committed through non-throwing swaps. This permits a strong exception
 *          guarantee even when policy operations throw.
 *
 * @tparam Value Stored and aggregated value type.
 * @tparam Acted Candidate acted-monoid policy.
 */
template <typename Value, typename Acted> concept RangeActionPolicy =
    algebra::ActedMonoidPolicy<Acted> && std::same_as<typename Acted::value_type, Value>
    && std::copy_constructible<typename Acted::value_type> && std::copy_constructible<typename Acted::action_type>
    && std::is_nothrow_swappable_v<typename Acted::value_type> && std::is_nothrow_move_constructible_v<typename Acted::action_type>
    && std::is_nothrow_swappable_v<typename Acted::action_type>;

/**
 * @brief Algebra and storage contract for Fenwick Tree implementations.
 *
 * @details A Fenwick layout may reorder contributions, so its operation must be
 *          commutative. Inverses provide range subtraction and point assignment.
 *
 * @tparam Value Stored and aggregated value type.
 * @tparam Group Candidate commutative-group policy.
 */
template <typename Value, typename Group> concept FenwickPolicy = algebra::CommutativeGroupPolicy<Group>
                                                                  && std::same_as<typename Group::value_type, Value>
                                                                  && std::copy_constructible<Value> && std::is_nothrow_swappable_v<Value>;

} // namespace ads::range

#endif // RANGE_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
