//===---------------------------------------------------------------------------===//
/**
 * @file Cartesian_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the CartesianTree class.
 * @version 0.1
 * @date 2026-07-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef CARTESIAN_TREE_HPP
#define CARTESIAN_TREE_HPP

#include "../../arrays/Dynamic_Array.hpp"
#include "../../queues/Linked_Queue.hpp"
#include "../Tree_Concepts.hpp"
#include "../exceptions/Binary_Tree_Exception.hpp"

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace ads::trees {

using ads::arrays::DynamicArray;

/**
 * @brief A binary tree derived from a sequence: heap-ordered on values, in-order
 *        on positions.
 *
 * @details A Cartesian tree is built from a sequence of values and satisfies two
 *          invariants simultaneously:
 *          - "In-order = sequence". An in-order traversal reproduces the original
 *             insertion order, so each node's stored @c index is its position in the
 *             sequence (0-based).
 *          - "Heap order on values". Under the comparator @c Compare every node
 *            is an extremum of its subtree; with the default @c std::less this is a
 *            @e min-Cartesian tree (each parent's value is '<=' its children's).
 *
 *          This is the mirror image of a @ref Treap: a treap chooses random
 *          priorities to shape a search tree keyed by value, whereas a Cartesian
 *          tree treats the array position as the key and the value as the priority.
 *          The pay-off is the tie to the @e range-minimum query: the extremum of a
 *          contiguous range '[l, r]' is the lowest common ancestor of positions
 *          @c l and @c r, reachable by a single root-to-node descent
 *          (see @ref range_extremum_index).
 *
 *          Unlike an ordered search tree the structure is @b sequence-derived: it
 *          exposes @ref push_back (append one element to the end of the sequence)
 *          and batch construction, but no keyed @c insert / @c remove, and it
 *          admits duplicate values. Construction from @c n elements is @c O(n) via
 *          a monotonic stack over the current right spine.
 *
 *          The type is move-only. Because a Cartesian tree built from a monotone
 *          sequence degenerates into a linear chain of length @c n, every teardown
 *          and traversal is implemented iteratively so that no operation recurses
 *          to a depth of @c n.
 *
 * @tparam T Stored value type.
 * @tparam Compare Strict-weak-ordering functor; 'Compare{}(a, b)' means @c a has
 *         higher heap priority than @c b (so it becomes the ancestor). Defaults to
 *         @c std::less, yielding a min-Cartesian tree.
 */
template <OrderedTreeElement T, typename Compare = std::less<T>>
class CartesianTree {
private:
  struct Node;

public:
  ///@brief Type aliases for convenience.
  using value_type   = T;
  using compare_type = Compare;
  using visitor_type = std::function<void(const T&)>;
  using size_type    = size_t;

  //===----- ITERATOR CLASS ----------------------------------------------------===//
  /**
   * @brief Forward iterator for in-order traversal of the Cartesian tree.
   *
   * @details Yields values in sequence (in-order) order using an explicit stack,
   *          so iteration never recurses. The iterator is a forward iterator (not
   *          bidirectional) for simplicity.
   */
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    ///@brief Default constructor for iterator.
    iterator() = default;

    ///@brief Copy constructor for iterator.
    iterator(const iterator& other);

    ///@brief Move constructor for iterator.
    iterator(iterator&& other) noexcept = default;

    ///@brief Copy assignment operator for iterator.
    auto operator=(const iterator& other) -> iterator&;

    ///@brief Move assignment operator for iterator.
    auto operator=(iterator&& other) noexcept -> iterator& = default;

    ///@brief Returns a const reference to the current element.
    auto operator*() const -> reference;

    ///@brief Returns a const pointer to the current element.
    auto operator->() const -> pointer;

    ///@brief Advances to the next in-order element (pre-increment).
    auto operator++() -> iterator&;

    ///@brief Advances to the next in-order element (post-increment).
    auto operator++(int) -> iterator;

    ///@brief Returns true if both iterators point to the same position.
    auto operator==(const iterator& other) const -> bool;

  private:
    friend class CartesianTree<T, Compare>;

    DynamicArray<Node*> stack_;
    Node*               current_ = nullptr;

    ///@brief Constructs an iterator starting at the in-order first node of @p root's subtree.
    explicit iterator(Node* root);

    ///@brief Pushes @p node and all its left descendants onto the traversal stack.
    void push_left(Node* node);
  };

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty Cartesian tree.
   * @param compare Comparator establishing heap priority.
   * @complexity Time O(1), Space O(1)
   */
  explicit CartesianTree(Compare compare = Compare{});

  /**
   * @brief Builds a Cartesian tree from a sequence of values.
   * @param sequence Values inserted in order; element @c i becomes in-order position @c i.
   * @param compare Comparator establishing heap priority.
   * @complexity Time O(n), Space O(n)
   */
  explicit CartesianTree(const DynamicArray<T>& sequence, Compare compare = Compare{}) requires std::copy_constructible<T>;

  /**
   * @brief Builds a Cartesian tree from an iterator range.
   * @tparam InputIt Input iterator whose reference type constructs a @c T.
   * @param first Beginning of the range.
   * @param last End of the range.
   * @param compare Comparator establishing heap priority.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  requires std::constructible_from<T, std::iter_reference_t<InputIt>>
  CartesianTree(InputIt first, InputIt last, Compare compare = Compare{});

  /**
   * @brief Move constructor.
   * @param other Tree to move from; left empty.
   * @complexity Time O(1), Space O(1)
   */
  CartesianTree(CartesianTree&& other) noexcept(std::is_nothrow_move_constructible_v<Compare>);

  /**
   * @brief Destructor. Removes all nodes iteratively.
   * @complexity Time O(n), Space O(1)
   */
  ~CartesianTree();

  /**
   * @brief Move assignment operator.
   * @param other Tree to move from; left empty.
   * @return Reference to this tree.
   * @note Available only when @c Compare is nothrow move-assignable.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(CartesianTree&& other) noexcept -> CartesianTree& requires std::is_nothrow_move_assignable_v<Compare>;

  CartesianTree(const CartesianTree&)                    = delete;
  auto operator=(const CartesianTree&) -> CartesianTree& = delete;

  //===----- MUTATION OPERATIONS -----------------------------------------------===//

  /**
   * @brief Appends a value to the end of the sequence.
   * @param value Value becoming the new last in-order position.
   * @complexity Amortized O(1), worst-case O(n); the total cost of @c n appends is O(n).
   */
  auto push_back(const T& value) -> void requires std::copy_constructible<T>;

  /**
   * @brief Appends a value to the end of the sequence using move semantics.
   * @param value Value becoming the new last in-order position.
   * @complexity Amortized O(1), worst-case O(n); the total cost of @c n appends is O(n).
   */
  auto push_back(T&& value) -> void;

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
   * @brief Returns the number of stored values.
   * @return Node count.
   */
  [[nodiscard]] auto size() const noexcept -> size_type;

  /**
   * @brief Returns the height of the tree, with empty tree = -1.
   * @return Height in edges.
   * @complexity Time O(n), Space O(n)
   */
  [[nodiscard]] auto height() const -> int;

  /**
   * @brief Returns the global extremum (the value stored at the root).
   * @return Const reference to the root value.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto extremum() const -> const T&;

  /**
   * @brief Returns the value at a given sequence position.
   * @param index Position in [0, size()).
   * @return Const reference to the value at @p index.
   * @throws BinaryTreeException if @p index is out of range.
   * @complexity Time O(h), Space O(1)
   */
  [[nodiscard]] auto value_at(size_type index) const -> const T&;

  /**
   * @brief Returns the position of the extremum within a sequence range.
   * @param left Inclusive left bound.
   * @param right Inclusive right bound.
   * @return Position in [left, right] whose value is the range extremum.
   * @throws BinaryTreeException if the range is invalid or out of bounds.
   * @complexity Time O(h), Space O(1)
   */
  [[nodiscard]] auto range_extremum_index(size_type left, size_type right) const -> size_type;

  /**
   * @brief Returns the extremum value within a sequence range.
   * @param left Inclusive left bound.
   * @param right Inclusive right bound.
   * @return Const reference to the range extremum value.
   * @throws BinaryTreeException if the range is invalid or out of bounds.
   * @complexity Time O(h), Space O(1)
   */
  [[nodiscard]] auto range_extremum(size_type left, size_type right) const -> const T&;

  /**
   * @brief Checks in-order sequencing, heap ordering, and stored size.
   * @return true if every Cartesian-tree invariant holds.
   * @complexity Time O(n), Space O(n)
   */
  [[nodiscard]] auto validate_properties() const -> bool;

  //===----- TRAVERSAL API -----------------------------------------------------===//

  /**
   * @brief Traverses the tree in sequence (in-order).
   * @param visit Callback invoked for each value.
   * @complexity Time O(n), Space O(n)
   */
  auto in_order_traversal(const visitor_type& visit) const -> void;

  /**
   * @brief Traverses the tree in root-left-right order.
   * @param visit Callback invoked for each value.
   * @complexity Time O(n), Space O(n)
   */
  auto pre_order_traversal(const visitor_type& visit) const -> void;

  /**
   * @brief Traverses the tree in left-right-root order.
   * @param visit Callback invoked for each value.
   * @complexity Time O(n), Space O(n)
   */
  auto post_order_traversal(const visitor_type& visit) const -> void;

  /**
   * @brief Traverses the tree level by level.
   * @param visit Callback invoked for each value.
   * @complexity Time O(n), Space O(n)
   */
  auto level_order_traversal(const visitor_type& visit) const -> void;

  //===----- ITERATOR ACCESS ---------------------------------------------------===//

  ///@brief Returns an iterator to the first element in sequence. O(h)
  auto begin() -> iterator;

  ///@brief Returns a const iterator to the first element in sequence. O(h)
  auto begin() const -> iterator;

  ///@brief Returns a past-the-end iterator. O(1)
  auto end() -> iterator;

  ///@brief Returns a const past-the-end iterator. O(1)
  auto end() const -> iterator;

  ///@brief Returns a const iterator to the first element in sequence. O(h)
  auto cbegin() const -> iterator;

  ///@brief Returns a const past-the-end iterator. O(1)
  auto cend() const -> iterator;

private:
  //===----- INTERNAL STORAGE --------------------------------------------------===//

  ///@brief Internal node holding a value, its in-order position, and owning its two children.
  struct Node {
    T                     data;
    size_type             index;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    template <typename U>
    Node(U&& value, size_type node_index) : data(std::forward<U>(value)), index(node_index), left(nullptr), right(nullptr) {}
  };

  Compare               comp_;  ///< Comparator establishing heap priority.
  std::unique_ptr<Node> root_;  ///< Root of the tree; holds the global extremum.
  size_type             size_;  ///< Number of nodes in the tree.
  DynamicArray<Node*>   spine_; ///< Right spine (root..rightmost) enabling O(1) amortized append.

  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Appends a value, maintaining both invariants via the monotonic-stack rule.
  template <typename U>
  auto append(U&& value) -> void;

  ///@brief Returns the node whose subtree root is the extremum of [left, right].
  [[nodiscard]] auto range_node(size_type left, size_type right) const -> Node*;

  ///@brief Returns the node at sequence position @p index.
  [[nodiscard]] auto node_at(size_type index) const -> Node*;

  ///@brief Throws if @p index is not a valid sequence position.
  auto validate_index(size_type index) const -> void;

  ///@brief Throws if [left, right] is not a valid, in-bounds range.
  auto validate_range(size_type left, size_type right) const -> void;
};

} // namespace ads::trees

#include "../../../../src/ads/trees/search/Cartesian_Tree.tpp"

#endif // CARTESIAN_TREE_HPP

//===---------------------------------------------------------------------------===//
