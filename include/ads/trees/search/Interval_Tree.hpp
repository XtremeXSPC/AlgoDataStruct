//===---------------------------------------------------------------------------===//
/**
 * @file Interval_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the IntervalTree class.
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef INTERVAL_TREE_HPP
#define INTERVAL_TREE_HPP

#include "../../arrays/Dynamic_Array.hpp"
#include "../Tree_Concepts.hpp"
#include "../exceptions/Binary_Tree_Exception.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>

namespace ads::trees {

using ads::arrays::DynamicArray;

/**
 * @brief A balanced, augmented binary search tree over closed intervals.
 *
 * @details An interval tree stores closed intervals @c [low,high] keyed by the
 *          pair @c (low,high) and augments every node with @c max_end, the largest
 *          right endpoint in its subtree. That augmentation turns stabbing and
 *          overlap queries into pruned descents: a subtree can be skipped entirely
 *          when its @c max_end already lies to the left of the query.
 *
 *          Balance is maintained exactly as in an AVL tree — the same rotations and
 *          balance factor in @c {-1,0,1} — with @c max_end recomputed alongside the
 *          height whenever a subtree is rewired. Every mutation therefore keeps the
 *          height at @c O(log n), so the recursive descents used internally cannot
 *          grow past logarithmic depth.
 *
 *          Each interval carries an associated @c Value, giving the structure a
 *          map-like payload. Duplicate intervals are permitted (multiset semantics):
 *          equal keys follow the right branch during insertion, while AVL rotations
 *          may subsequently place equivalent keys on either side. @ref remove
 *          deletes a single matching occurrence.
 *
 *          Supported queries:
 *          - @ref find_any_overlap / @ref overlaps: one overlapping interval, or
 *            merely whether any exists, in @c O(log n).
 *          - @ref find_overlapping: every interval overlapping a query interval.
 *          - @ref find_containing: every interval containing a query point.
 *          - @ref find / @ref contains: exact interval membership.
 *
 *          Two closed intervals @c a and @c b overlap iff
 *          @c a.low <= b.high && b.low <= a.high. The type is move-only.
 *
 * @tparam Endpoint Copyable coordinate type with non-throwing strict ordering.
 * @tparam Value Payload stored with each interval.
 */
template <IntervalEndpoint Endpoint, TreeElement Value>
class IntervalTree {
private:
  struct Node;

public:
  //===----- PUBLIC VALUE TYPES ------------------------------------------------===//

  /// @brief A closed interval @c [low,high].
  struct Interval {
    Endpoint low;
    Endpoint high;
  };

  /// @brief An interval together with its associated value (the node payload).
  struct Entry {
    Interval interval;
    Value    value;
  };

  ///@brief Type aliases for convenience.
  using endpoint_type = Endpoint;
  using mapped_type   = Value;
  using value_type    = Entry;
  using size_type     = std::size_t;
  using visitor_type  = std::function<void(const Interval&, const Value&)>;

  //===----- ITERATOR CLASS ----------------------------------------------------===//
  /**
   * @brief Forward iterator for in-order traversal over stored entries.
   *
   * @details Yields entries in non-decreasing @c (low,high) key order. It keeps an
   *          explicit stack of pending nodes and dereferences to @c const @c Entry&.
   */
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Entry;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const Entry*;
    using reference         = const Entry&;

    ///@brief Default constructor for iterator (past-the-end).
    iterator() = default;

    ///@brief Copy constructor for iterator.
    iterator(const iterator& other);

    ///@brief Move constructor for iterator.
    iterator(iterator&& other) noexcept = default;

    ///@brief Copy assignment operator for iterator.
    auto operator=(const iterator& other) -> iterator&;

    ///@brief Move assignment operator for iterator.
    auto operator=(iterator&& other) noexcept -> iterator& = default;

    ///@brief Returns a const reference to the current entry.
    auto operator*() const -> reference;

    ///@brief Returns a const pointer to the current entry.
    auto operator->() const -> pointer;

    ///@brief Advances to the next in-order entry (pre-increment).
    auto operator++() -> iterator&;

    ///@brief Advances to the next in-order entry (post-increment).
    auto operator++(int) -> iterator;

    ///@brief Returns true if both iterators point to the same position.
    auto operator==(const iterator& other) const -> bool;

  private:
    friend class IntervalTree;

    ///@brief Constructs an iterator at the in-order first node of @p root's subtree.
    explicit iterator(Node* root);

    // DynamicArray keeps forward-iterator copies independent for postfix traversal.
    DynamicArray<Node*> stack_;
    Node*               current_ = nullptr;

    ///@brief Pushes @p node and all its left descendants onto the traversal stack.
    void push_left(Node* node);
  };

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty interval tree.
   * @complexity Time O(1), Space O(1)
   */
  IntervalTree() noexcept;

  /**
   * @brief Builds an interval tree from a set of entries.
   * @param entries Entries to insert; each is copied into the tree.
   * @throws BinaryTreeException if an interval has 'low > high' or a NaN endpoint.
   * @complexity Time O(n log n), Space O(n)
   */
  explicit IntervalTree(const DynamicArray<Entry>& entries) requires std::copy_constructible<Value>;

  /**
   * @brief Builds an interval tree from an iterator range of entries.
   * @tparam InputIt Input iterator whose reference type constructs an @c Entry.
   * @param first Beginning of the range.
   * @param last End of the range.
   * @throws BinaryTreeException if an interval has 'low > high' or a NaN endpoint.
   * @complexity Time O(n log n), Space O(n)
   */
  template <std::input_iterator InputIt>
  requires std::constructible_from<Entry, std::iter_reference_t<InputIt>>
  IntervalTree(InputIt first, InputIt last);

  /**
   * @brief Move constructor.
   * @param other Tree to move from; left empty.
   * @complexity Time O(1), Space O(1)
   */
  IntervalTree(IntervalTree&& other) noexcept;

  /**
   * @brief Destructor. Removes all nodes.
   * @complexity Time O(n), Space O(log n)
   */
  ~IntervalTree() = default;

  /**
   * @brief Move assignment operator.
   * @param other Tree to move from; left empty.
   * @return Reference to this tree.
   * @complexity Time O(n), Space O(log n)
   */
  auto operator=(IntervalTree&& other) noexcept -> IntervalTree&;

  IntervalTree(const IntervalTree&)                    = delete;
  auto operator=(const IntervalTree&) -> IntervalTree& = delete;

  //===----- MUTATION OPERATIONS -----------------------------------------------===//

  /**
   * @brief Inserts an interval with an associated value (copy).
   * @param interval Interval to store; duplicates are permitted.
   * @param value Value copied into the new node.
   * @throws BinaryTreeException if 'interval.low > interval.high' or either endpoint is NaN.
   * @complexity Time O(log n), Space O(log n)
   */
  auto insert(const Interval& interval, const Value& value) -> void requires std::copy_constructible<Value>;

  /**
   * @brief Inserts an interval with an associated value (move).
   * @param interval Interval to store; duplicates are permitted.
   * @param value Value moved into the new node.
   * @throws BinaryTreeException if 'interval.low > interval.high' or either endpoint is NaN.
   * @complexity Time O(log n), Space O(log n)
   */
  auto insert(const Interval& interval, Value&& value) -> void requires std::move_constructible<Value>;

  /**
   * @brief Removes one occurrence of an interval.
   * @param interval Interval to remove.
   * @return true if a matching interval was found and removed.
   * @throws BinaryTreeException if 'interval.low > interval.high' or either endpoint is NaN.
   * @complexity Time O(log n), Space O(log n)
   */
  auto remove(const Interval& interval) -> bool;

  /**
   * @brief Removes all nodes from the tree.
   * @complexity Time O(n), Space O(log n)
   */
  auto clear() noexcept -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Checks whether the tree is empty.
   * @return true if empty.
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of stored intervals.
   * @return Interval count.
   */
  [[nodiscard]] auto size() const noexcept -> size_type;

  /**
   * @brief Returns the height of the tree (empty tree = 0, leaf = 1).
   * @return Height counting nodes on the longest root-to-leaf path.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto height() const noexcept -> int;

  /**
   * @brief Tests whether an exact interval is stored.
   * @param interval Interval to search for.
   * @return true if a matching interval is present.
   * @throws BinaryTreeException if 'interval.low > interval.high' or either endpoint is NaN.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto contains(const Interval& interval) const -> bool;

  /**
   * @brief Finds one entry whose interval exactly matches @p interval.
   * @param interval Interval to search for.
   * @return Pointer to a matching entry, or nullptr if none exists.
   * @throws BinaryTreeException if 'interval.low > interval.high' or either endpoint is NaN.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto find(const Interval& interval) const -> const Entry*;

  /**
   * @brief Reports whether any stored interval overlaps @p query.
   * @param query Query interval.
   * @return true if at least one stored interval overlaps @p query.
   * @throws BinaryTreeException if 'query.low > query.high' or either endpoint is NaN.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto overlaps(const Interval& query) const -> bool;

  /**
   * @brief Finds one stored entry whose interval overlaps @p query.
   * @param query Query interval.
   * @return Pointer to an overlapping entry, or nullptr if none exists.
   * @throws BinaryTreeException if 'query.low > query.high' or either endpoint is NaN.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto find_any_overlap(const Interval& query) const -> const Entry*;

  /**
   * @brief Visits every stored interval overlapping @p query, in key order.
   * @param query Query interval.
   * @param visit Callback invoked with each overlapping interval and its value.
   * @throws BinaryTreeException if 'query.low > query.high' or either endpoint is NaN.
   * @complexity Time O(m + log n), where m is the number of reported intervals;
   *             Space O(log n)
   */
  auto find_overlapping(const Interval& query, const visitor_type& visit) const -> void;

  /**
   * @brief Visits every stored interval containing @p point, in key order.
   * @param point Query point.
   * @param visit Callback invoked with each containing interval and its value.
   * @throws BinaryTreeException if @p point is NaN.
   * @complexity Time O(m + log n), where m is the number of reported intervals;
   *             Space O(log n)
   */
  auto find_containing(const Endpoint& point, const visitor_type& visit) const -> void;

  //===----- TRAVERSAL OPERATIONS ----------------------------------------------===//

  /**
   * @brief Visits every stored entry in non-decreasing key order.
   * @param visit Callback invoked with each interval and its value.
   * @complexity Time O(n), Space O(log n)
   */
  auto in_order_traversal(const visitor_type& visit) const -> void;

  //===----- VALIDATION --------------------------------------------------------===//

  /**
   * @brief Checks the size, ordering, AVL balance, stored-height, and augmentation invariants.
   * @details Primarily for testing: verifies the in-order key sequence is non-decreasing,
   *          every balance factor lies in [-1, 1], each stored height is correct, and each
   *          node's @c max_end equals the maximum right endpoint of its subtree, and
   *          the traversed node count matches @ref size.
   * @return true if every invariant holds.
   * @complexity Time O(n), Space O(log n)
   */
  [[nodiscard]] auto validate_properties() const -> bool;

  //===----- ITERATOR OPERATIONS -----------------------------------------------===//

  ///@brief Returns an iterator to the first entry in key order. Complexity: O(log n)
  auto begin() -> iterator;

  ///@brief Returns a const iterator to the first entry in key order. Complexity: O(log n)
  auto begin() const -> iterator;

  ///@brief Returns a past-the-end iterator. Complexity: O(1)
  auto end() -> iterator;

  ///@brief Returns a const past-the-end iterator. Complexity: O(1)
  auto end() const -> iterator;

  ///@brief Returns a const iterator to the first entry in key order. Complexity: O(log n)
  auto cbegin() const -> iterator;

  ///@brief Returns a const past-the-end iterator. Complexity: O(1)
  auto cend() const -> iterator;

private:
  //===----- INTERNAL NODE STRUCTURE -------------------------------------------===//
  /**
   * @brief Internal node: an entry, the subtree maximum endpoint, height, and children.
   */
  struct Node {
    Entry           entry;
    const Endpoint* max_end;    ///< Points to the largest right endpoint in this subtree.
    int             height = 1; ///< Height of subtree rooted at this node (leaf = 1).

    std::unique_ptr<Node> left  = nullptr;
    std::unique_ptr<Node> right = nullptr;

    template <typename V>
    Node(const Interval& node_interval, V&& node_value) :
        entry{node_interval, std::forward<V>(node_value)},
        max_end(&entry.interval.high) {}
  };

  std::unique_ptr<Node> root_; ///< Root of the tree.
  size_type             size_; ///< Number of stored intervals.

  //===----- INTERVAL PREDICATES -----------------------------------------------===//

  ///@brief Strict-weak order on intervals by (low, high).
  [[nodiscard]] static auto interval_less(const Interval& lhs, const Interval& rhs) noexcept -> bool;

  ///@brief Returns true if two closed intervals overlap.
  [[nodiscard]] static auto intervals_overlap(const Interval& lhs, const Interval& rhs) noexcept -> bool;

  ///@brief Rejects a malformed interval (low greater than high).
  static auto validate_interval(const Interval& interval) -> void;

  ///@brief Returns true when an endpoint is admissible for ordered queries.
  [[nodiscard]] static auto is_valid_endpoint(const Endpoint& endpoint) noexcept -> bool;

  ///@brief Rejects an endpoint that cannot participate in ordered queries.
  static auto validate_endpoint(const Endpoint& endpoint) -> void;

  //===----- AUGMENTATION HELPERS ----------------------------------------------===//

  ///@brief Returns the stored height of a node (0 if nullptr).
  [[nodiscard]] auto get_height(const Node* node) const noexcept -> int;

  ///@brief Returns the balance factor of a node (0 if nullptr).
  [[nodiscard]] auto get_balance_factor(const Node* node) const noexcept -> int;

  ///@brief Recomputes a node's stored height and @c max_end from its children.
  void update_metrics(Node* node) noexcept;

  //===----- ROTATION OPERATIONS -----------------------------------------------===//

  ///@brief Right rotation (LL case); returns the new subtree root.
  auto rotate_right(std::unique_ptr<Node> y) noexcept -> std::unique_ptr<Node>;

  ///@brief Left rotation (RR case); returns the new subtree root.
  auto rotate_left(std::unique_ptr<Node> x) noexcept -> std::unique_ptr<Node>;

  ///@brief Left-right rotation (LR case); returns the new subtree root.
  auto rotate_left_right(std::unique_ptr<Node> node) noexcept -> std::unique_ptr<Node>;

  ///@brief Right-left rotation (RL case); returns the new subtree root.
  auto rotate_right_left(std::unique_ptr<Node> node) noexcept -> std::unique_ptr<Node>;

  ///@brief Rebalances a node after a mutation; returns the new subtree root.
  auto balance(std::unique_ptr<Node> node) noexcept -> std::unique_ptr<Node>;

  //===----- MODIFICATION HELPERS ----------------------------------------------===//

  ///@brief Recursive insertion helper; duplicates descend to the right.
  template <typename V>
  auto insert_helper(std::unique_ptr<Node>& node, const Interval& interval, V&& value) -> void;

  ///@brief Recursive removal helper.
  auto remove_helper(std::unique_ptr<Node>& node, const Interval& interval, bool& removed) noexcept -> void;

  ///@brief Detaches the minimum node of a subtree, rebalancing on the way up.
  auto detach_min(std::unique_ptr<Node>& node) noexcept -> std::unique_ptr<Node>;

  //===----- QUERY HELPERS -----------------------------------------------------===//

  ///@brief Recursive overlap-reporting helper with max_end / low pruning.
  void report_overlapping(const Node* node, const Interval& query, const visitor_type& visit) const;

  ///@brief Recursive in-order traversal helper.
  void in_order_helper(const Node* node, const visitor_type& visit) const;

  ///@brief Recursive validation helper.
  auto validate_helper(const Node* node, const Node*& prev, int& height, size_type& count, const Endpoint*& out_max) const noexcept -> bool;
};

} // namespace ads::trees

#include "../../../../src/ads/trees/search/Interval_Tree.tpp"

#endif // INTERVAL_TREE_HPP

//===---------------------------------------------------------------------------===//
