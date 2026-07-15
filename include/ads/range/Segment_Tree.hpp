//===---------------------------------------------------------------------------===//
/**
 * @file Segment_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the monoid-driven SegmentTree class.
 * @version 2.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SEGMENT_TREE_HPP
#define SEGMENT_TREE_HPP

#include "../../support/Container_Facade.hpp"
#include "../algebra/Basic_Monoids.hpp"
#include "../arrays/Dynamic_Array.hpp"
#include "Range_Concepts.hpp"
#include "Range_Exception.hpp"

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::range {

using ads::arrays::DynamicArray;
using ads::support::ContainerFacade;

/**
 * @brief Iterative Segment Tree parameterized by an explicit monoid policy.
 *
 * @details @c Monoid owns the aggregate type, associative combination, and identity.
 *          @c LeafBuilder maps externally stored @c Value objects into monoid values,
 *          allowing leaf and aggregate types to differ. The iterative tree pads its
 *          leaf level with identities to the next power of two. This preserves the
 *          sequence order even for non-commutative monoids.
 *
 *          Rebuilds and point changes are transactional. All potentially throwing
 *          policy work is completed off-structure, then committed with non-throwing
 *          swaps. Consequently, build and point updates provide the strong guarantee.
 *
 *          Complexity:
 *            - Build: O(n) time and O(n) auxiliary space.
 *            - Point update: O(log n) time and O(log n) auxiliary space.
 *            - Range query: O(log n) time and O(1) auxiliary space.
 *
 * @note The monoid laws are semantic requirements; see @ref algebra::MonoidPolicy.
 *
 * @tparam Value Externally stored leaf value type.
 * @tparam Monoid Aggregate monoid policy (default: additive monoid over @c Value).
 * @tparam LeafBuilder Maps @c Value to @c Monoid::value_type.
 */
template <
    typename Value,
    typename Monoid      = algebra::AddMonoid<Value>,
    typename LeafBuilder = DefaultLeafBuilder<Value, typename Monoid::value_type>>
requires SegmentTreePolicy<Value, Monoid, LeafBuilder> && std::is_nothrow_swappable_v<Value> && std::copyable<typename Monoid::value_type>
         && std::is_nothrow_swappable_v<typename Monoid::value_type>
class SegmentTree : public ContainerFacade<SegmentTree<Value, Monoid, LeafBuilder>> {
public:
  using value_type      = Value;
  using monoid_type     = Monoid;
  using node_type       = typename Monoid::value_type;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = Value&;
  using const_reference = const Value&;
  using pointer         = Value*;
  using const_pointer   = const Value*;

  using const_iterator         = typename DynamicArray<Value>::const_iterator;
  using const_reverse_iterator = typename DynamicArray<Value>::const_reverse_iterator;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /// @brief Constructs an empty tree with default policy objects.
  SegmentTree() requires(std::default_initializable<Monoid> && std::default_initializable<LeafBuilder>)
  = default;

  /// @brief Constructs an empty tree with explicit policy objects.
  explicit SegmentTree(Monoid monoid, LeafBuilder leaf_builder = {});

  /// @brief Constructs @p size default-initialized values.
  explicit SegmentTree(size_type size)
      requires std::default_initializable<Value> && std::default_initializable<Monoid> && std::default_initializable<LeafBuilder>;

  /// @brief Constructs @p size default-initialized values with explicit policies.
  SegmentTree(size_type size, Monoid monoid, LeafBuilder leaf_builder = {}) requires std::default_initializable<Value>;

  /// @brief Constructs @p size copies of @p value.
  SegmentTree(size_type size, const Value& value, Monoid monoid = {}, LeafBuilder leaf_builder = {});

  /// @brief Constructs from a vector by copy.
  explicit SegmentTree(const std::vector<Value>& values, Monoid monoid = {}, LeafBuilder leaf_builder = {});

  /// @brief Constructs from a vector by move.
  explicit SegmentTree(std::vector<Value>&& values, Monoid monoid = {}, LeafBuilder leaf_builder = {});

  /// @brief Constructs from an initializer list.
  SegmentTree(std::initializer_list<Value> values, Monoid monoid = {}, LeafBuilder leaf_builder = {});

  /// @brief Constructs from an iterator range.
  template <std::input_iterator InputIt>
  SegmentTree(InputIt first, InputIt last, Monoid monoid = {}, LeafBuilder leaf_builder = {});

  /// @brief Move constructor. Leaves @p other empty.
  SegmentTree(SegmentTree&& other) noexcept(
      std::is_nothrow_move_constructible_v<Monoid> && std::is_nothrow_move_constructible_v<LeafBuilder>);

  /// @brief Move assignment. Leaves @p other empty.
  auto operator=(SegmentTree&& other) noexcept(std::is_nothrow_move_assignable_v<Monoid> && std::is_nothrow_move_assignable_v<LeafBuilder>)
      -> SegmentTree&;

  ///@brief Destroys the tree and its contiguous storage.
  ~SegmentTree() = default;

  SegmentTree(const SegmentTree&)                    = delete;
  auto operator=(const SegmentTree&) -> SegmentTree& = delete;

  //===----- MODIFICATION OPERATIONS -------------------------------------------===//

  ///@brief Rebuilds the tree from a copied vector in O(n) time.
  auto build(const std::vector<Value>& values) -> void;

  ///@brief Rebuilds the tree from a moved vector in O(n) time.
  auto build(std::vector<Value>&& values) -> void;

  ///@brief Rebuilds the tree from an initializer list in O(n) time.
  auto build(std::initializer_list<Value> values) -> void;

  ///@brief Rebuilds the tree from the input range [@p first, @p last) in O(n) time.
  template <std::input_iterator InputIt>
  auto build(InputIt first, InputIt last) -> void;

  /**
   * @brief Replaces one value, preserving the tree on failure.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  auto set(size_type index, const Value& value) -> void;

  /**
   * @brief Replaces one value by move, preserving the tree on policy failure.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  auto set(size_type index, Value&& value) -> void;

  /**
   * @brief Adds @p delta to one external value and rebuilds its aggregate path.
   * @throws RangeIndexException if @p index is out of bounds.
   * @complexity Time O(log n), auxiliary Space O(log n)
   */
  auto add(size_type index, const Value& delta) -> void requires Addable<Value>;

  ///@brief Replaces the sequence with @p size value-initialized leaves in O(n) time.
  auto reset(size_type size) -> void requires std::default_initializable<Value>;

  ///@brief Removes every element in O(n) time.
  auto clear() noexcept -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Returns the monoid aggregate over inclusive range [@p left, @p right].
   * @throws InvalidRangeException if the range is empty, reversed, or out of bounds.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto range_query(size_type left, size_type right) const -> node_type;

  ///@brief Returns the aggregate of all elements, or the identity when empty.
  [[nodiscard]] auto total() const -> node_type;

  ///@brief Returns a const reference to the external value at @p index.
  [[nodiscard]] auto value_at(size_type index) const -> const_reference;

  ///@brief Returns a copy of the stored leaf aggregate at @p index.
  [[nodiscard]] auto node_at(size_type index) const -> node_type;

  ///@brief Returns the number of external values.
  [[nodiscard]] auto size() const noexcept -> size_type;

  ///@brief Returns whether the tree contains no values.
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  //===----- ITERATOR ACCESS ---------------------------------------------------===//

  ///@brief Returns a const iterator to the first external value.
  [[nodiscard]] auto begin() const noexcept -> const_iterator;

  ///@brief Returns a const iterator past the final external value.
  [[nodiscard]] auto end() const noexcept -> const_iterator;

  //===----- POLICY ACCESSORS --------------------------------------------------===//

  ///@brief Returns the stored aggregate monoid policy.
  [[nodiscard]] auto get_monoid() const noexcept -> const Monoid&;

  ///@brief Returns the stored leaf-construction policy.
  [[nodiscard]] auto get_leaf_builder() const noexcept -> const LeafBuilder&;

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Builds fresh value and aggregate arrays, then commits them atomically.
  auto rebuild(DynamicArray<Value>&& values) -> void;

  ///@brief Stages a point replacement and its ancestor aggregates before commit.
  auto set_transactional(size_type index, Value&& value) -> void;

  ///@brief Throws if @p index is outside the stored sequence.
  auto validate_index(size_type index) const -> void;

  ///@brief Throws if [@p left, @p right] is not a valid inclusive range.
  auto validate_range(size_type left, size_type right) const -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  [[no_unique_address]] Monoid      monoid_;         ///< Aggregate monoid policy.
  [[no_unique_address]] LeafBuilder leaf_builder_;   ///< Leaf-construction policy.
  DynamicArray<Value>               values_{};       ///< Materialized external values.
  DynamicArray<node_type>           tree_{};         ///< One-based complete-tree storage.
  size_type                         leaf_count_ = 0; ///< Power-of-two leaf capacity.
  size_type                         size_       = 0; ///< Number of logical leaves.
};

} // namespace ads::range

#include "../../../src/ads/range/Segment_Tree.tpp"

#endif // SEGMENT_TREE_HPP

//===---------------------------------------------------------------------------===//
