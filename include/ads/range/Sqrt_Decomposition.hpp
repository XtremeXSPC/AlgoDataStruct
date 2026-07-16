//===---------------------------------------------------------------------------===//
/**
 * @file Sqrt_Decomposition.hpp
 * @author Costantino Lombardi
 * @brief Declaration of an acted-monoid square-root decomposition.
 * @version 1.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SQRT_DECOMPOSITION_HPP
#define SQRT_DECOMPOSITION_HPP

#include "../algebra/Range_Actions.hpp"
#include "../arrays/Dynamic_Array.hpp"
#include "Range_Concepts.hpp"
#include "Range_Exception.hpp"

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::range {

using ads::arrays::DynamicArray;

/**
 * @brief Square-root decomposition with lazy per-block range actions.
 *
 * @details The sequence is partitioned into O(sqrt(n)) blocks. Each block keeps
 *          a value-monoid aggregate and an optional pending action. The acted
 *          monoid is the same abstraction consumed by @ref LazySegmentTree, so
 *          the aggregation, action, and chronological composition laws have one
 *          shared definition.
 *
 *          A modification stages both boundary-block materializations and all
 *          interior lazy tags before committing them with non-throwing swaps.
 *          Thus @c apply and @c range_apply provide the strong exception guarantee
 *          across the complete requested range.
 *
 *          Construction takes O(n) time and space. Point and range operations
 *          inspect at most O(sqrt(n)) values or block aggregates.
 *
 * @tparam Value Aggregate and element value type.
 * @tparam Acted Acted-monoid policy. Its @c value_type must be @p Value.
 */
template <typename Value, typename Acted = algebra::RangeAddRangeSum<Value>>
requires RangeActionPolicy<Value, Acted>
class SqrtDecomposition {
public:
  using value_type        = Value;
  using acted_monoid_type = Acted;
  using tag_type          = typename Acted::action_type;
  using size_type         = std::size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /// @brief Constructs an empty decomposition with a default policy.
  SqrtDecomposition() noexcept(std::is_nothrow_default_constructible_v<Acted>) requires std::default_initializable<Acted>
  = default;

  /// @brief Constructs an empty decomposition with an explicit policy.
  explicit SqrtDecomposition(Acted acted) noexcept(std::is_nothrow_move_constructible_v<Acted>);

  /// @brief Constructs from a copied vector. @complexity O(n)
  explicit SqrtDecomposition(const std::vector<Value>& values, Acted acted = {});

  /// @brief Constructs from a moved vector. @complexity O(n)
  explicit SqrtDecomposition(std::vector<Value>&& values, Acted acted = {});

  /// @brief Constructs from an initializer list. @complexity O(n)
  SqrtDecomposition(std::initializer_list<Value> values, Acted acted = {});

  /// @brief Constructs from an input range. @complexity O(n)
  template <std::input_iterator InputIt>
  SqrtDecomposition(InputIt first, InputIt last, Acted acted = {});

  /// @brief Move constructor; leaves @p other empty.
  SqrtDecomposition(SqrtDecomposition&& other) noexcept(std::is_nothrow_move_constructible_v<Acted>);

  /// @brief Move assignment; leaves @p other empty.
  auto operator=(SqrtDecomposition&& other) noexcept(std::is_nothrow_move_assignable_v<Acted>) -> SqrtDecomposition&;

  ///@brief Destroys all values, block aggregates, and pending actions.
  ~SqrtDecomposition() = default;

  SqrtDecomposition(const SqrtDecomposition&)                    = delete;
  auto operator=(const SqrtDecomposition&) -> SqrtDecomposition& = delete;

  //===----- MODIFICATION OPERATIONS -------------------------------------------===//

  /// @brief Rebuilds from a copied vector. @complexity O(n)
  auto build(const std::vector<Value>& values) -> void;

  /// @brief Rebuilds from a moved vector. @complexity O(n)
  auto build(std::vector<Value>&& values) -> void;

  /// @brief Rebuilds from an initializer list. @complexity O(n)
  auto build(std::initializer_list<Value> values) -> void;

  /// @brief Rebuilds from an input range. @complexity O(n)
  template <std::input_iterator InputIt>
  auto build(InputIt first, InputIt last) -> void requires std::constructible_from<Value, std::iter_reference_t<InputIt>>;

  /// @brief Removes every element. @complexity O(n)
  auto clear() noexcept -> void;

  /**
   * @brief Replaces the value at @p index and refreshes its block aggregate.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(sqrt(n)), auxiliary Space O(sqrt(n))
   */
  auto set(size_type index, const Value& value) -> void;

  /**
   * @brief Applies @p action at one position.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(sqrt(n)), auxiliary Space O(sqrt(n))
   */
  auto apply(size_type index, const tag_type& action) -> void;

  /**
   * @brief Applies @p action to the inclusive range [left, right].
   * @throws InvalidRangeException if the range is empty, reversed, or out of bounds.
   * @complexity Time O(sqrt(n)), auxiliary Space O(sqrt(n))
   */
  auto range_apply(size_type left, size_type right, const tag_type& action) -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Returns the aggregate over inclusive range [@p left, @p right].
   * @throws InvalidRangeException if the range is empty, reversed, or out of bounds.
   * @complexity Time O(sqrt(n)), Space O(1)
   */
  [[nodiscard]] auto range_query(size_type left, size_type right) const -> Value;

  /**
   * @brief Returns the materialized value at @p index.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto value_at(size_type index) const -> Value;

  /// @brief Materializes the current sequence. @complexity O(n)
  [[nodiscard]] auto to_vector() const -> std::vector<Value>;

  /// @brief Returns the aggregate of all values, or the identity when empty. @complexity O(sqrt(n))
  [[nodiscard]] auto total() const -> Value;

  ///@brief Returns the number of stored elements.
  [[nodiscard]] auto size() const noexcept -> size_type;

  ///@brief Returns whether the decomposition contains no elements.
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  ///@brief Returns the target number of elements per block.
  [[nodiscard]] auto block_size() const noexcept -> size_type;

  /// @brief Returns the acted-monoid policy.
  [[nodiscard]] auto get_acted_monoid() const noexcept -> const Acted&;

private:
  //===----- STAGED UPDATE TYPES ----------------------------------------------===//

  ///@brief Fully materialized replacement for a partially covered block.
  struct PartialBlockUpdate {
    size_type           block;     ///< Block index to replace.
    DynamicArray<Value> values;    ///< Replacement materialized values.
    Value               aggregate; ///< Replacement block aggregate.
  };

  ///@brief Replacement lazy state for a fully covered block.
  struct LazyBlockUpdate {
    size_type               block; ///< Block index to update.
    std::optional<tag_type> lazy;  ///< Composed pending action.
  };

  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Returns the block containing @p position.
  [[nodiscard]] auto block_of(size_type position) const noexcept -> size_type { return position / block_size_; }

  ///@brief Returns the first logical position in @p block.
  [[nodiscard]] auto block_start(size_type block) const noexcept -> size_type { return block * block_size_; }

  ///@brief Returns the one-past-end logical position of @p block.
  [[nodiscard]] auto block_end(size_type block) const noexcept -> size_type;

  ///@brief Returns the number of logical elements in @p block.
  [[nodiscard]] auto block_length(size_type block) const noexcept -> size_type { return block_end(block) - block_start(block); }

  ///@brief Returns one value after applying its block's pending action.
  [[nodiscard]] auto true_value(size_type index) const -> Value;

  ///@brief Returns one block aggregate after applying its pending action.
  [[nodiscard]] auto true_block_aggregate(size_type block) const -> Value;

  ///@brief Stages a materialized update for part of one block.
  [[nodiscard]] auto make_partial_update(size_type block, size_type left, size_type right, const tag_type& action) const
      -> PartialBlockUpdate;

  ///@brief Stages a materialized point replacement for one block.
  [[nodiscard]] auto make_set_update(size_type index, const Value& value) const -> PartialBlockUpdate;

  ///@brief Commits all precomputed block updates through non-throwing swaps.
  auto commit_updates(DynamicArray<PartialBlockUpdate>& partial, DynamicArray<LazyBlockUpdate>& lazy) noexcept -> void;

  ///@brief Builds fresh block storage, then commits it atomically.
  auto rebuild(DynamicArray<Value>&& values) -> void;

  ///@brief Throws if @p index is outside the stored sequence.
  auto validate_index(size_type index) const -> void;

  ///@brief Throws if [@p left, @p right] is not a valid inclusive range.
  auto validate_range(size_type left, size_type right) const -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  [[no_unique_address]] Acted           acted_;             ///< Stored acted-monoid policy.
  DynamicArray<Value>                   data_{};            ///< Materialized block-local values.
  DynamicArray<Value>                   block_aggregate_{}; ///< Aggregate for each block.
  DynamicArray<std::optional<tag_type>> block_lazy_{};      ///< Deferred action for each block.
  size_type                             block_size_ = 0;    ///< Target block length.
  size_type                             num_blocks_ = 0;    ///< Number of allocated blocks.
  size_type                             size_       = 0;    ///< Number of logical elements.
};

} // namespace ads::range

#include "../../../src/ads/range/Sqrt_Decomposition.tpp"

#endif // SQRT_DECOMPOSITION_HPP

//===---------------------------------------------------------------------------===//
