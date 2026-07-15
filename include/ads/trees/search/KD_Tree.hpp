//===---------------------------------------------------------------------------===//
/**
 * @file KD_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the KDTree class.
 * @version 0.1
 * @date 2026-07-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef KD_TREE_HPP
#define KD_TREE_HPP

#include "../../arrays/Dynamic_Array.hpp"
#include "../Tree_Concepts.hpp"
#include "../exceptions/Binary_Tree_Exception.hpp"

#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>

namespace ads::trees {

using ads::arrays::DynamicArray;

/**
 * @brief A k-dimensional binary search tree for spatial queries.
 *
 * @details A k-d tree organizes points in @c Dimensions-dimensional space by
 *          cycling the splitting axis with depth. Each split compares its primary
 *          axis first and uses the remaining axes as deterministic tie-breakers.
 *          This layout turns nearest-neighbour and orthogonal range queries into
 *          pruned tree descents.
 *
 *          Construction from a point set chooses the median under that ordering
 *          (via @c std::nth_element over a scratch buffer), yielding a balanced
 *          tree of height @c O(log n), including when exact duplicates are present.
 *          Incremental @ref insert uses the same axis-cycling order but does not
 *          rebalance, so a pathological insertion order can grow the height to
 *          @c n. To stay correct under that worst case every
 *          operation — teardown, traversal, and each query — is iterative and uses
 *          an explicit stack rather than recursion.
 *
 *          Supported queries:
 *          - @ref nearest / @ref k_nearest: closest stored point(s) to a query,
 *            using branch-and-bound pruning on the per-axis distance to each split
 *            plane. The far side of a split is revisited only if it can still hold a
 *            point at least as close as the current best.
 *          - @ref range_search: every stored point inside an axis-aligned box.
 *          - @ref contains: exact point membership.
 *
 *          Distances are squared Euclidean accumulated in @c long @c double (no
 *          @c sqrt; ordering is identical), so @c nearest returns the closest point
 *          but not its distance. Floating-point coordinates must be finite.
 *          Duplicate points are allowed. The type is move-only.
 *
 * @tparam Coordinate Arithmetic coordinate type of each axis.
 * @tparam Dimensions Number of axes; must be greater than zero.
 *
 * @note This k-d tree supports insertion and queries only: there is no
 *       remove() (rebuild the tree to drop points) and no self-balancing,
 *       so adversarial insertion orders degrade the O(log n) bounds.
 */
template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
class KDTree {
private:
  struct Node;

public:
  ///@brief Type aliases for convenience.
  using coordinate_type = Coordinate;
  using point_type      = std::array<Coordinate, Dimensions>;
  using distance_type   = long double;
  using size_type       = std::size_t;
  using visitor_type    = std::function<void(const point_type&)>;

  ///@brief Number of spatial dimensions.
  static constexpr size_type dimensions = Dimensions;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty k-d tree.
   * @complexity Time O(1), Space O(1)
   */
  KDTree() noexcept;

  /**
   * @brief Builds a balanced k-d tree from a set of points.
   * @param points Points to index; copied into a scratch buffer during the build.
   * @throws BinaryTreeException if a floating-point coordinate is not finite.
   * @complexity Time O(n log n), Space O(n)
   */
  explicit KDTree(const DynamicArray<point_type>& points);

  /**
   * @brief Builds a balanced k-d tree from an iterator range of points.
   * @tparam InputIt Input iterator whose reference type constructs a @c point_type.
   * @param first Beginning of the range.
   * @param last End of the range.
   * @throws BinaryTreeException if a floating-point coordinate is not finite.
   * @complexity Time O(n log n), Space O(n)
   */
  template <std::input_iterator InputIt>
  requires std::constructible_from<point_type, std::iter_reference_t<InputIt>>
  KDTree(InputIt first, InputIt last);

  /**
   * @brief Move constructor.
   * @param other Tree to move from; left empty.
   * @complexity Time O(1), Space O(1)
   */
  KDTree(KDTree&& other) noexcept;

  /**
   * @brief Destructor. Removes all nodes iteratively.
   * @complexity Time O(n), Space O(1)
   */
  ~KDTree();

  /**
   * @brief Move assignment operator.
   * @param other Tree to move from; left empty.
   * @return Reference to this tree.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(KDTree&& other) noexcept -> KDTree&;

  KDTree(const KDTree&)                    = delete;
  auto operator=(const KDTree&) -> KDTree& = delete;

  //===----- MUTATION OPERATIONS -----------------------------------------------===//

  /**
   * @brief Inserts a point, cycling the split axis with depth.
   * @param point Point to add; duplicates are permitted.
   * @throws BinaryTreeException if a floating-point coordinate is not finite.
   * @complexity Time O(h), where @c h is the current height.
   * @note Insertion does not rebalance; rebuild from a point set to restore balance.
   */
  auto insert(const point_type& point) -> void;

  /**
   * @brief Removes all nodes from the tree.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Checks whether the tree is empty.
   * @return true if empty.
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of stored points.
   * @return Point count.
   */
  [[nodiscard]] auto size() const noexcept -> size_type;

  /**
   * @brief Returns the height of the tree, with empty tree = -1.
   * @return Height in edges.
   * @complexity Time O(n), Space O(n)
   */
  [[nodiscard]] auto height() const -> int;

  /**
   * @brief Tests whether an exact point is stored.
   * @param point Point to search for.
   * @return true if a matching point is present.
   * @throws BinaryTreeException if a floating-point coordinate is not finite.
   * @complexity Time O(h), Space O(1)
   */
  [[nodiscard]] auto contains(const point_type& point) const -> bool;

  /**
   * @brief Returns the stored point closest to a query point.
   * @param query Query point.
   * @return The closest stored point under squared Euclidean distance.
   * @throws EmptyTreeException if the tree is empty.
   * @throws BinaryTreeException if a coordinate is not finite or the squared
   *         distance is not representable.
   * @complexity Time O(v), where v is the number of visited nodes and is O(n) in
   *             the worst case; Space O(h)
   */
  [[nodiscard]] auto nearest(const point_type& query) const -> point_type;

  /**
   * @brief Returns the @p k stored points closest to a query point.
   * @param query Query point.
   * @param k Maximum number of neighbours to return; clamped to @ref size.
   * @return Up to @p k points sorted by ascending distance to @p query.
   * @throws BinaryTreeException if a coordinate is not finite or the squared
   *         distance is not representable.
   * @complexity Time O(v log k + k log k), where v is the number of visited nodes;
   *             Space O(k + h)
   */
  [[nodiscard]] auto k_nearest(const point_type& query, size_type k) const -> DynamicArray<point_type>;

  /**
   * @brief Returns every stored point inside an axis-aligned box.
   * @param lower Inclusive lower corner of the box.
   * @param upper Inclusive upper corner of the box.
   * @return All stored points @c p with 'lower[i] <= p[i] <= upper[i]' on every axis.
   * @throws BinaryTreeException if a coordinate is not finite or if
   *         'lower[i] > upper[i]' on any axis.
   * @complexity Time O(n^(1 - 1/Dimensions) + m) typical for a balanced tree and
   *             O(n + m) worst case; Space O(h + m)
   */
  [[nodiscard]] auto range_search(const point_type& lower, const point_type& upper) const -> DynamicArray<point_type>;

  /**
   * @brief Visits every stored point in pre-order.
   * @param visit Callback invoked for each point.
   * @complexity Time O(n), Space O(n)
   */
  auto for_each(const visitor_type& visit) const -> void;

private:
  //===----- INTERNAL STORAGE --------------------------------------------------===//

  ///@brief Internal node holding a point and owning its two children.
  struct Node {
    point_type            point;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    explicit Node(const point_type& node_point) : point(node_point), left(nullptr), right(nullptr) {}
  };

  ///@brief A candidate neighbour paired with its squared distance, for k-nearest.
  struct Neighbor {
    distance_type distance;
    point_type    point;
  };

  std::unique_ptr<Node> root_; ///< Root of the tree.
  size_type             size_; ///< Number of stored points.

  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Returns the squared Euclidean distance between two points.
  [[nodiscard]] static auto distance_squared(const point_type& lhs, const point_type& rhs) -> distance_type;

  ///@brief Returns the squared distance between two coordinates on one axis.
  [[nodiscard]] static auto axis_distance_squared(const coordinate_type& lhs, const coordinate_type& rhs) -> distance_type;

  ///@brief Returns true if two points match on every axis.
  [[nodiscard]] static auto points_equal(const point_type& lhs, const point_type& rhs) noexcept -> bool;

  ///@brief Compares points by one axis and then cyclically by the remaining axes.
  [[nodiscard]] static auto point_less(const point_type& lhs, const point_type& rhs, size_type axis) noexcept -> bool;

  ///@brief Rejects non-finite floating-point coordinates.
  static auto validate_point(const point_type& point) -> void;

  ///@brief Iteratively builds a balanced tree by repeated median selection.
  auto build_balanced(DynamicArray<point_type>& scratch) -> void;

  ///@brief Returns true if @p point lies within the inclusive box [lower, upper].
  [[nodiscard]] static auto in_box(const point_type& point, const point_type& lower, const point_type& upper) noexcept -> bool;
};

} // namespace ads::trees

#include "../../../../src/ads/trees/search/KD_Tree.tpp"

#endif // KD_TREE_HPP

//===---------------------------------------------------------------------------===//
