//===---------------------------------------------------------------------------===//
/**
 * @file Skew_Heap.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the SkewHeap class (a self-adjusting meldable priority queue).
 * @version 0.1
 * @date 2026-07-06
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SKEW_HEAP_HPP
#define SKEW_HEAP_HPP

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
// Defined only in the unit tests; friended below for white-box heap-order checks.
template <typename Heap>
struct SkewHeapInspector;
} // namespace detail

/**
 * @brief A skew heap: a self-adjusting meldable (mergeable) priority queue.
 *
 * @details A skew heap is the self-adjusting cousin of the leftist heap. It keeps
 *          the same heap-order binary-tree shape but drops the null-path-length
 *          bookkeeping entirely: 'merge' walks down the two right spines and then
 *          unconditionally swaps every visited node's children. That single
 *          rule replaces the leftist invariant; balance is not maintained
 *          explicitly but is guaranteed in an amortized sense, giving O(log n)
 *          amortized 'insert', 'extract_top', and 'merge'. As with 'LeftistHeap',
 *          'insert' and 'extract_top' are expressed in terms of 'merge'.
 *
 *          This implementation performs the meld iteratively (top-down): it merges
 *          the two right spines into a single priority-ordered chain and swaps
 *          children along it, so the operation uses no recursion and cannot
 *          overflow the call stack on an adversarial insertion order.
 *
 *          Priority is defined by 'Compare'. With the default 'std::less<T>',
 *          'top()' yields the maximum element, matching 'DAryHeap', 'PriorityQueue',
 *          and 'LeftistHeap'; use 'MinSkewHeap' for min-heap semantics.
 *          The type is move-only and models the 'MeldableHeap' concept.
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
class SkewHeap {
public:
  using value_type = T;
  using size_type  = std::size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty heap.
   * @param comp Comparison function object.
   * @complexity Time O(1), Space O(1)
   */
  explicit SkewHeap(Compare comp = Compare{}) noexcept(std::is_nothrow_move_constructible_v<Compare>);

  /**
   * @brief Constructs a heap from an initializer list.
   * @param values Elements to insert.
   * @param comp Comparison function object.
   * @complexity Time O(n log n), Space O(n)
   */
  SkewHeap(std::initializer_list<T> values, Compare comp = Compare{}) requires CopyHeapValue<T>;

  /**
   * @brief Constructs a heap from an iterator range.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @param comp Comparison function object.
   * @complexity Time O(n log n), Space O(n)
   */
  template <std::input_iterator InputIt>
  SkewHeap(InputIt first, InputIt last, Compare comp = Compare{}) requires HeapRangeValue<InputIt, T>;

  /**
   * @brief Constructs a heap from a vector.
   * @param elements Elements to insert.
   * @param comp Comparison function object.
   * @complexity Time O(n log n), Space O(n)
   */
  explicit SkewHeap(const std::vector<T>& elements, Compare comp = Compare{}) requires CopyHeapValue<T>;

  /**
   * @brief Move constructor.
   * @param other The heap to move from; left empty.
   * @complexity Time O(1), Space O(1)
   */
  SkewHeap(SkewHeap&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The heap to move from; left empty.
   * @return Reference to this.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(SkewHeap&& other) noexcept -> SkewHeap&;

  /**
   * @brief Destructor. Destroys all nodes.
   * @complexity Time O(n), Space O(1)
   */
  ~SkewHeap();

  // Copy operations are disabled (move-only type).
  SkewHeap(const SkewHeap&)                    = delete;
  auto operator=(const SkewHeap&) -> SkewHeap& = delete;

  //===----- INSERTION OPERATIONS ----------------------------------------------===//

  /**
   * @brief Inserts an element into the heap.
   * @param value The value to insert (lvalue reference).
   * @complexity Time O(log n) amortized, Space O(1)
   */
  auto insert(const T& value) -> void requires CopyHeapValue<T>;

  /**
   * @brief Inserts an element into the heap (move version).
   * @param value The value to insert (rvalue reference).
   * @complexity Time O(log n) amortized, Space O(1)
   */
  auto insert(T&& value) -> void requires MoveHeapValue<T>;

  /**
   * @brief Constructs an element in-place and inserts it.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly inserted element. The reference stays valid
   *         until that element is extracted, since melding only relinks nodes.
   * @complexity Time O(log n) amortized, Space O(1)
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
   * @complexity Time O(log n) amortized, Space O(1)
   */
  auto extract_top() -> T;

  //===----- MELD OPERATIONS ---------------------------------------------------===//

  /**
   * @brief Merges another heap into this one (destructive meld).
   * @param other The heap to absorb; left empty afterwards.
   * @throws HeapException if the comparator objects are incompatible.
   * @complexity Time O(log n) amortized, Space O(1)
   * @note Self-merge is a no-op.
   * @note Stateless comparators are compatible by type. Stateful comparators
   *       must be equality-comparable and compare equal.
   */
  auto merge(SkewHeap&& other) -> void;

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
  // the internal tree to assert heap order. A skew heap keeps no structural
  // balance invariant, so heap order is the only thing to verify.
  // No production code depends on this friendship.
  friend struct detail::SkewHeapInspector<SkewHeap>;

  //===----- NODE TYPE ---------------------------------------------------------===//

  /**
   * @brief Internal tree node owning its children through unique_ptr.
   * @details Unlike a leftist node it carries no null-path length: the
   *          self-adjusting swap replaces the need for balance metadata.
   */
  struct Node {
    T                     value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    ///@brief Constructs the stored value in place from the forwarded arguments.
    template <typename... Args>
    explicit Node(Args&&... args) : value(std::forward<Args>(args)...) {}
  };

  using NodePtr = std::unique_ptr<Node>;

  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  /**
   * @brief Melds two skew subtrees and returns the merged root (iterative).
   * @details Merges the two right spines into one priority-ordered chain, then
   *          swaps the children of every node on that chain.
   * @complexity Time O(log n) amortized, Space O(1) — no recursion.
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

  NodePtr   root_;     ///< Root of the skew heap (null if empty).
  size_type size_ = 0; ///< Number of elements in the heap.
  Compare   comp_;     /// Comparison function object defining priority order.
};

template <typename T>
using MaxSkewHeap = SkewHeap<T, std::less<T>>;

template <typename T>
using MinSkewHeap = SkewHeap<T, std::greater<T>>;

} // namespace ads::heaps

#include "../../../src/ads/heaps/Skew_Heap.tpp"

#endif // SKEW_HEAP_HPP

//===---------------------------------------------------------------------------===//
