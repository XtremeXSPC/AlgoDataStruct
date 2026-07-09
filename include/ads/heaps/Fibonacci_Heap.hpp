//===---------------------------------------------------------------------------===//
/**
 * @file Fibonacci_Heap.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the FibonacciHeap class (a meldable priority queue with handles).
 * @version 0.1
 * @date 2026-07-09
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef FIBONACCI_HEAP_HPP
#define FIBONACCI_HEAP_HPP

#include "Heap_Concepts.hpp"
#include "Heap_Exception.hpp"

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::heaps {

namespace detail {
// Defined only in the unit tests; friended below for white-box structural checks.
template <typename Heap>
struct FibonacciHeapInspector;
} // namespace detail

/**
 * @brief A Fibonacci heap: a meldable priority queue with O(1) amortized
 *        'decrease_key' and handle-based 'erase'.
 *
 * @details A Fibonacci heap is a lazy forest of heap-ordered trees held in a
 *          circular doubly-linked root list, with a pointer to the top element.
 *          'insert' and 'merge' are O(1): they just splice into the root list and
 *          defer all cleanup. The structural work happens in 'extract_top', which
 *          moves the removed root's children to the root list and **consolidates**
 *          trees of equal degree. 'decrease_key' cuts the node to the root list
 *          and performs **cascading cuts** on marked ancestors, which is what
 *          bounds every node's degree by O(log n) and yields the O(1) amortized
 *          decrease-key that makes Dijkstra/Prim run in O(E + V log V).
 *
 *          As in the pairing heap, operations relink nodes and never move a value
 *          between nodes, so a @ref Handle (returned by 'emplace') is just a node
 *          pointer and stays valid until its element leaves the heap (via
 *          'extract_top', 'erase', or 'clear'), after which using it is undefined
 *          behaviour.
 *
 *          Node lifetime is owned centrally by an arena (a list of nodes) so the
 *          circular raw links never own anything; teardown clears the arena in
 *          one pass. Priority is defined by 'Compare': with the default
 *          'std::less<T>', 'top()' yields the **maximum** element, matching the
 *          rest of the heap family; use 'MinFibonacciHeap' for min-heap
 *          semantics. Following the codebase convention (see 'MinHeap'),
 *          'decrease_key' adjusts an element to a value of **higher priority**
 *          under 'Compare'. The type is move-only and models 'MeldableHeap'.
 *
 *          Complexity: O(1) 'insert'/'merge'/'top'; O(1) amortized
 *          'decrease_key'; O(log n) amortized 'extract_top' and 'erase'.
 *
 *          Exception safety: 'top'/'extract_top' on an empty heap and an invalid
 *          'decrease_key'/'erase' throw 'HeapException' without modifying the
 *          heap; 'clear' and the move operations are 'noexcept'.
 *
 * @tparam T The element type.
 * @tparam Compare Comparator defining priority order (default 'std::less<T>').
 */
template <HeapValue T, typename Compare = std::less<T>>
class FibonacciHeap {
private:
  struct Node;
  using Pool     = std::list<std::unique_ptr<Node>>;
  using PoolIter = typename Pool::iterator;

public:
  using value_type = T;
  using size_type  = std::size_t;

  /**
   * @brief Opaque, copyable reference to an inserted element.
   *
   * @details Returned by 'emplace' and consumed by 'decrease_key' / 'erase'. A
   *          default-constructed handle is empty (@ref valid returns false). A
   *          handle remains usable until its element is removed from the heap.
   */
  class Handle {
  public:
    Handle() = default;

    ///@brief Returns true if this handle refers to an element.
    [[nodiscard]] auto valid() const noexcept -> bool { return node_ != nullptr; }

  private:
    friend class FibonacciHeap;

    ///@brief Creates a handle from the node currently holding the element.
    explicit Handle(Node* node) noexcept : node_(node) {}

    Node* node_ = nullptr;
  };

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty heap.
   * @param comp Comparison function object.
   * @complexity Time O(1), Space O(1)
   */
  explicit FibonacciHeap(Compare comp = Compare{}) noexcept(std::is_nothrow_move_constructible_v<Compare>);

  /**
   * @brief Constructs a heap from an initializer list.
   * @param values Elements to insert.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   */
  FibonacciHeap(std::initializer_list<T> values, Compare comp = Compare{}) requires CopyHeapValue<T>;

  /**
   * @brief Constructs a heap from an iterator range.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  FibonacciHeap(InputIt first, InputIt last, Compare comp = Compare{}) requires HeapRangeValue<InputIt, T>;

  /**
   * @brief Constructs a heap from a vector.
   * @param elements Elements to insert.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   */
  explicit FibonacciHeap(const std::vector<T>& elements, Compare comp = Compare{}) requires CopyHeapValue<T>;

  /**
   * @brief Move constructor.
   * @param other The heap to move from; left empty.
   * @complexity Time O(1), Space O(1)
   */
  FibonacciHeap(FibonacciHeap&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The heap to move from; left empty.
   * @return Reference to this.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(FibonacciHeap&& other) noexcept -> FibonacciHeap&;

  /**
   * @brief Destructor. Destroys all nodes.
   * @complexity Time O(n), Space O(1)
   */
  ~FibonacciHeap();

  // Copy operations are disabled (move-only type).
  FibonacciHeap(const FibonacciHeap&)                    = delete;
  auto operator=(const FibonacciHeap&) -> FibonacciHeap& = delete;

  //===----- INSERTION OPERATIONS ----------------------------------------------===//

  /**
   * @brief Inserts an element into the heap.
   * @param value The value to insert (lvalue reference).
   * @complexity Time O(1), Space O(1)
   */
  auto insert(const T& value) -> void requires CopyHeapValue<T>;

  /**
   * @brief Inserts an element into the heap (move version).
   * @param value The value to insert (rvalue reference).
   * @complexity Time O(1), Space O(1)
   */
  auto insert(T&& value) -> void requires MoveHeapValue<T>;

  /**
   * @brief Constructs an element in-place, inserts it, and returns its handle.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return A stable @ref Handle to the inserted element, usable with
   *         'decrease_key' and 'erase' until the element is removed.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> Handle requires EmplaceHeapValue<T, Args...>;

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
   * @complexity Time O(log n) amortized, Space O(log n)
   */
  auto extract_top() -> T;

  /**
   * @brief Removes the element referred to by a handle.
   * @param handle A valid handle obtained from 'emplace'.
   * @throws HeapException if the handle is empty.
   * @complexity Time O(log n) amortized, Space O(log n)
   * @note The handle is left dangling and must not be reused.
   */
  auto erase(const Handle& handle) -> void;

  //===----- UPDATE OPERATIONS -------------------------------------------------===//

  /**
   * @brief Raises an element's priority to a new value.
   * @param handle A valid handle obtained from 'emplace'.
   * @param new_value The replacement value; must be of priority higher than or
   *        equal to the current value under 'Compare'.
   * @throws HeapException if the handle is empty or the new value would lower the
   *         element's priority.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto decrease_key(const Handle& handle, const T& new_value) -> void requires CopyHeapValue<T>;

  //===----- MELD OPERATIONS ---------------------------------------------------===//

  /**
   * @brief Merges another heap into this one (destructive meld).
   * @param other The heap to absorb; left empty afterwards.
   * @throws HeapException if the comparator objects are incompatible.
   * @complexity Time O(1), Space O(1)
   * @note Self-merge is a no-op.
   * @note Stateless comparators are compatible by type. Stateful comparators
   *       must be equality-comparable and compare equal.
   */
  auto merge(FibonacciHeap&& other) -> void;

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
  // the internal forest to assert heap order, the circular-list wiring, and the
  // degree bookkeeping. No production code depends on this friendship.
  friend struct detail::FibonacciHeapInspector<FibonacciHeap>;

  //===----- NODE TYPE ---------------------------------------------------------===//

  /**
   * @brief Internal forest node. Siblings (and roots) form a circular doubly-
   *        linked list through 'left'/'right'; 'child' points at one child in
   *        that circle. All links are raw and non-owning.
   * @details 'self' is the node's own position in the owning arena, so an O(1)
   *          'erase'/'extract_top' can release it. 'mark' records whether the
   *          node has already lost a child since becoming non-root (for cascading
   *          cuts).
   */
  struct Node {
    T        value;
    Node*    parent = nullptr;
    Node*    child  = nullptr;
    Node*    left   = nullptr;
    Node*    right  = nullptr;
    int      degree = 0;
    bool     mark   = false;
    PoolIter self;

    ///@brief Constructs the stored value in place from the forwarded arguments.
    template <typename... Args>
    explicit Node(Args&&... args) : value(std::forward<Args>(args)...) {}
  };

  //===----- ARENA HELPERS -----------------------------------------------------===//

  ///@brief Allocates a node in the arena and records its position in 'self'.
  template <typename... Args>
  auto arena_alloc(Args&&... args) -> Node*;

  ///@brief Releases a node back to the arena in O(1).
  auto arena_free(Node* node) -> void;

  //===----- CIRCULAR-LIST HELPERS ---------------------------------------------===//

  ///@brief Makes 'node' a one-element circular list.
  static auto make_single(Node* node) noexcept -> void;

  ///@brief Inserts single 'node' immediately after 'pos' in pos's circle.
  static auto insert_after(Node* pos, Node* node) noexcept -> void;

  ///@brief Splices circular list 'other' into circular list 'list'.
  static auto concat_lists(Node* list, Node* other) noexcept -> void;

  ///@brief Unlinks 'node' from its sibling circle (node's own links left as-is).
  static auto unlink(Node* node) noexcept -> void;

  //===----- STRUCTURAL HELPERS ------------------------------------------------===//

  ///@brief Adds a node to the root list and updates 'top_'.
  auto add_to_root_list(Node* node) -> void;

  ///@brief Makes 'child' a child of 'parent' (used during consolidation).
  auto link_child(Node* child, Node* parent) noexcept -> void;

  ///@brief Merges roots of equal degree until each degree occurs at most once.
  auto consolidate() -> void;

  ///@brief Cuts 'node' from parent 'parent' and moves it to the root list.
  auto cut(Node* node, Node* parent) -> void;

  ///@brief Propagates cuts up the marked ancestor chain (iterative).
  auto cascading_cut(Node* node) -> void;

  /**
   * @brief Throws HeapException if the heap is empty.
   * @param operation Name of the calling operation, used in the message.
   */
  auto validate_non_empty(const char* operation) const -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  Pool      pool_;           ///< Arena owning every node; raw links point into it.
  Node*     top_  = nullptr; ///< Highest-priority root (null if empty).
  size_type size_ = 0;       ///< Number of elements in the heap.
  Compare   comp_;           /// Comparison function object defining priority order.
};

template <typename T>
using MaxFibonacciHeap = FibonacciHeap<T, std::less<T>>;

template <typename T>
using MinFibonacciHeap = FibonacciHeap<T, std::greater<T>>;

} // namespace ads::heaps

#include "../../../src/ads/heaps/Fibonacci_Heap.tpp"

#endif // FIBONACCI_HEAP_HPP

//===---------------------------------------------------------------------------===//
