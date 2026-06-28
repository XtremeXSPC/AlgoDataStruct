//===---------------------------------------------------------------------------===//
/**
 * @file Leftist_Heap.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the LeftistHeap class (a meldable priority queue).
 * @version 0.1
 * @date 2026-06-28
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef LEFTIST_HEAP_HPP
#define LEFTIST_HEAP_HPP

#include "Heap_Concepts.hpp"
#include "Heap_Exception.hpp"

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::heaps {

namespace detail {
// Forward declaration of the white-box test inspector. It is defined only in the
// unit-test translation unit and grants tests read access to the internal tree
// so the structural leftist invariant can be verified directly.
template <typename Heap>
struct LeftistHeapInspector;
} // namespace detail

/**
 * @brief A leftist heap: a meldable (mergeable) priority queue.
 *
 * @details A leftist heap is a binary tree that satisfies heap order and the
 *          leftist property: for every node, the null-path length (npl) of its
 *          left child is at least that of its right child. The npl is the length
 *          of the shortest path from a node to a descendant with a missing child;
 *          by convention 'npl(nullptr) == -1' and a leaf has 'npl == 0'. The
 *          leftist property keeps the right spine of length O(log n), which is
 *          where the defining 'merge' operation does its work; 'insert' and
 *          'extract_top' are expressed in terms of 'merge'.
 *
 *          Priority is defined by 'Compare'. With the default 'std::less<T>',
 *          'top()' yields the **maximum** element, matching 'DAryHeap' and
 *          'PriorityQueue'; use 'MinLeftistHeap' for min-heap semantics. The
 *          type is move-only and models the 'MeldableHeap' concept.
 *
 *          Exception safety: 'top' and 'extract_top' on an empty heap throw
 *          'HeapException' without modifying the heap; with a non-throwing
 *          comparator 'insert'/'emplace' provide the strong guarantee (the node
 *          is allocated before it is spliced in, so a failed allocation leaves
 *          the heap unchanged); 'clear' and the move operations are 'noexcept'.
 *
 * @tparam T The element type.
 * @tparam Compare Comparator defining priority order (default 'std::less<T>').
 */
template <HeapValue T, typename Compare = std::less<T>>
class LeftistHeap {
public:
  using value_type = T;
  using size_type  = std::size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty heap.
   * @param comp Comparison function object.
   * @complexity Time O(1), Space O(1)
   */
  explicit LeftistHeap(Compare comp = Compare{}) noexcept(std::is_nothrow_move_constructible_v<Compare>);

  /**
   * @brief Constructs a heap from an initializer list.
   * @param values Elements to insert.
   * @param comp Comparison function object.
   * @complexity Time O(n log n), Space O(n)
   */
  LeftistHeap(std::initializer_list<T> values, Compare comp = Compare{}) requires CopyHeapValue<T>;

  /**
   * @brief Constructs a heap from an iterator range.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @param comp Comparison function object.
   * @complexity Time O(n log n), Space O(n)
   */
  template <std::input_iterator InputIt>
  LeftistHeap(InputIt first, InputIt last, Compare comp = Compare{}) requires HeapRangeValue<InputIt, T>;

  /**
   * @brief Constructs a heap from a vector.
   * @param elements Elements to insert.
   * @param comp Comparison function object.
   * @complexity Time O(n log n), Space O(n)
   */
  explicit LeftistHeap(const std::vector<T>& elements, Compare comp = Compare{}) requires CopyHeapValue<T>;

  /**
   * @brief Move constructor.
   * @param other The heap to move from; left empty.
   * @complexity Time O(1), Space O(1)
   */
  LeftistHeap(LeftistHeap&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The heap to move from; left empty.
   * @return Reference to this.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(LeftistHeap&& other) noexcept -> LeftistHeap&;

  /**
   * @brief Destructor. Destroys all nodes.
   * @complexity Time O(n), Space O(1)
   */
  ~LeftistHeap();

  // Copy operations are disabled (move-only type).
  LeftistHeap(const LeftistHeap&)                    = delete;
  auto operator=(const LeftistHeap&) -> LeftistHeap& = delete;

  //===----- INSERTION OPERATIONS ----------------------------------------------===//

  /**
   * @brief Inserts an element into the heap.
   * @param value The value to insert (lvalue reference).
   * @complexity Time O(log n), Space O(1)
   */
  auto insert(const T& value) -> void requires CopyHeapValue<T>;

  /**
   * @brief Inserts an element into the heap (move version).
   * @param value The value to insert (rvalue reference).
   * @complexity Time O(log n), Space O(1)
   */
  auto insert(T&& value) -> void requires MoveHeapValue<T>;

  /**
   * @brief Constructs an element in-place and inserts it.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly inserted element. The reference stays valid
   *         until that element is extracted, since melding only relinks nodes.
   * @complexity Time O(log n), Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T& requires EmplaceHeapValue<T, Args...>;

  //===----- ACCESS OPERATIONS -------------------------------------------------===//

  /**
   * @brief Returns a const reference to the top-priority element.
   * @return Const reference to the top element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto top() const -> const T&;

  //===----- REMOVAL OPERATIONS ------------------------------------------------===//

  /**
   * @brief Removes and returns the top-priority element.
   * @return The removed element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(log n), Space O(1)
   */
  auto extract_top() -> T;

  //===----- MELD OPERATIONS ---------------------------------------------------===//

  /**
   * @brief Merges another heap into this one (destructive meld).
   * @param other The heap to absorb; left empty afterwards.
   * @complexity Time O(log n), Space O(1)
   * @note Self-merge is a no-op.
   */
  auto merge(LeftistHeap&& other) -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Checks whether the heap is empty.
   * @return true if the heap is empty.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of stored elements.
   * @return Element count.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_type;

  /**
   * @brief Removes all elements from the heap.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

private:
  // White-box test access (defined only in the unit tests): lets the suite walk
  // the internal tree to assert the structural leftist invariant. No production
  // code depends on this friendship.
  friend struct detail::LeftistHeapInspector<LeftistHeap>;

  //===----- NODE TYPE ---------------------------------------------------------===//

  ///@brief Internal tree node owning its children through unique_ptr.
  struct Node {
    T                     value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    int                   npl = 0; ///< Null-path length; leaf == 0.

    ///@brief Constructs the stored value in place from the forwarded arguments.
    template <typename... Args>
    explicit Node(Args&&... args) : value(std::forward<Args>(args)...) {}
  };

  using NodePtr = std::unique_ptr<Node>;

  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Null-path length of a (possibly null) node. 'npl(nullptr) == -1'.
  [[nodiscard]] static auto npl_of(const NodePtr& node) noexcept -> int;

  /**
   * @brief Merges two leftist subtrees and returns the merged root.
   * @complexity Time O(log n), Space O(log n) recursion.
   */
  static auto merge_nodes(NodePtr lhs, NodePtr rhs, const Compare& comp) -> NodePtr;

  /**
   * @brief Melds a freshly created single node into the root.
   * @return Reference to the inserted node (its value reference stays stable).
   */
  auto attach(NodePtr node) -> Node&;

  /**
   * @brief Throws HeapException if the heap is empty.
   * @param operation Name of the calling operation, used in the message.
   */
  auto validate_non_empty(const char* operation) const -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  NodePtr   root_;     ///< Root of the leftist heap (null if empty).
  size_type size_ = 0; ///< Number of elements in the heap.
  Compare   comp_;     /// Comparison function object defining priority order.
};

template <typename T>
using MaxLeftistHeap = LeftistHeap<T, std::less<T>>;

template <typename T>
using MinLeftistHeap = LeftistHeap<T, std::greater<T>>;

} // namespace ads::heaps

#include "../../../src/ads/heaps/Leftist_Heap.tpp"

#endif // LEFTIST_HEAP_HPP

//===---------------------------------------------------------------------------===//
