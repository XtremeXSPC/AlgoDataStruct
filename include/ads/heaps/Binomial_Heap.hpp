//===---------------------------------------------------------------------------===//
/**
 * @file Binomial_Heap.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the BinomialHeap class (a meldable priority queue with handles).
 * @version 0.1
 * @date 2026-07-07
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef BINOMIAL_HEAP_HPP
#define BINOMIAL_HEAP_HPP

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
// Defined only in the unit tests; friended below for white-box structural checks.
template <typename Heap>
struct BinomialHeapInspector;
} // namespace detail

/**
 * @brief A binomial heap: a meldable priority queue that also supports
 *        handle-based 'decrease_key' and 'erase'.
 *
 * @details A binomial heap is a forest of binomial trees kept in a root list
 *          sorted by ascending degree, with at most one tree of each degree, so
 *          the forest mirrors the binary representation of the element count.
 *          Each tree obeys heap order. 'merge' adds two such forests exactly like
 *          binary addition (link equal-degree trees, propagate the carry), which
 *          is the primitive that 'insert' and 'extract_top' build on.
 *
 *          Unlike the leftist and skew heaps, a binomial heap is the first
 *          "forest" heap, and it introduces the handle extension the meldable
 *          family shares: 'emplace' returns a stable @ref Handle that survives
 *          every meld and sift, letting callers later raise an element's priority
 *          with 'decrease_key' or remove it outright with 'erase'. Handle
 *          stability is maintained by an internal indirection record that travels
 *          with the element as sift-up swaps values between nodes; a handle stays
 *          valid until its element leaves the heap (via 'extract_top', 'erase',
 *          or 'clear'), after which using it is undefined behaviour.
 *
 *          Priority is defined by 'Compare'. With the default 'std::less<T>',
 *          'top()' yields the maximum element, matching the rest of the heap
 *          family; use 'MinBinomialHeap' for min-heap semantics. Following the
 *          codebase convention (see 'MinHeap::decrease_key'), 'decrease_key'
 *          adjusts an element to a value of higher priority under 'Compare'
 *          (for the default max-heap that means a larger value). The type is
 *          move-only and models the 'MeldableHeap' concept.
 *
 *          Complexity: O(log n) for 'insert', 'extract_top', 'merge',
 *          'decrease_key', and 'erase'; O(log n) for 'top'. (Insert is not
 *          amortized O(1) here: 'union_with' rebuilds the whole root list.)
 *
 *          Exception safety: 'top'/'extract_top' on an empty heap and an invalid
 *          'decrease_key'/'erase' throw 'HeapException' without modifying the
 *          heap; 'clear' and the move operations are 'noexcept'.
 *
 * @tparam T The element type.
 * @tparam Compare Comparator defining priority order (default 'std::less<T>').
 */
template <HeapValue T, typename Compare = std::less<T>>
class BinomialHeap {
private:
  struct Node;

  /**
   * @brief Stable indirection record. One exists per live element; it always
   *        points at the node currently holding that element. Sift-up swaps both
   *        values and these records so an external handle never goes stale.
   */
  struct HandleData {
    Node* node = nullptr;
  };

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
    [[nodiscard]] auto valid() const noexcept -> bool { return data_ != nullptr; }

  private:
    friend class BinomialHeap;

    ///@brief Creates a handle from the heap-owned indirection record.
    explicit Handle(HandleData* data) noexcept : data_(data) {}

    HandleData* data_ = nullptr;
  };

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty heap.
   * @param comp Comparison function object.
   * @complexity Time O(1), Space O(1)
   */
  explicit BinomialHeap(Compare comp = Compare{}) noexcept(std::is_nothrow_move_constructible_v<Compare>);

  /**
   * @brief Constructs a heap from an initializer list.
   * @param values Elements to insert.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   */
  BinomialHeap(std::initializer_list<T> values, Compare comp = Compare{}) requires CopyHeapValue<T>;

  /**
   * @brief Constructs a heap from an iterator range.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  BinomialHeap(InputIt first, InputIt last, Compare comp = Compare{}) requires HeapRangeValue<InputIt, T>;

  /**
   * @brief Constructs a heap from a vector.
   * @param elements Elements to insert.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   */
  explicit BinomialHeap(const std::vector<T>& elements, Compare comp = Compare{}) requires CopyHeapValue<T>;

  /**
   * @brief Move constructor.
   * @param other The heap to move from; left empty.
   * @complexity Time O(1), Space O(1)
   */
  BinomialHeap(BinomialHeap&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The heap to move from; left empty.
   * @return Reference to this.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(BinomialHeap&& other) noexcept -> BinomialHeap&;

  /**
   * @brief Destructor. Destroys all nodes.
   * @complexity Time O(n), Space O(1)
   */
  ~BinomialHeap();

  // Copy operations are disabled (move-only type).
  BinomialHeap(const BinomialHeap&)                    = delete;
  auto operator=(const BinomialHeap&) -> BinomialHeap& = delete;

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
   * @brief Constructs an element in-place, inserts it, and returns its handle.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return A stable @ref Handle to the inserted element, usable with
   *         'decrease_key' and 'erase' until the element is removed.
   * @complexity Time O(log n), Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> Handle requires EmplaceHeapValue<T, Args...>;

  //===----- ACCESS OPERATIONS -------------------------------------------------===//

  /**
   * @brief Returns a const reference to the top-priority element.
   * @return Const reference to the top element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(log n), Space O(1)
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

  /**
   * @brief Removes the element referred to by a handle.
   * @param handle A valid handle obtained from 'emplace'.
   * @throws HeapException if the handle is empty.
   * @complexity Time O(log n), Space O(1)
   * @note The handle is left empty and must not be reused.
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
   * @complexity Time O(log n), Space O(1)
   */
  auto decrease_key(const Handle& handle, const T& new_value) -> void requires CopyHeapValue<T>;

  //===----- MELD OPERATIONS ---------------------------------------------------===//

  /**
   * @brief Merges another heap into this one (destructive meld).
   * @param other The heap to absorb; left empty afterwards.
   * @throws HeapException if the comparator objects are incompatible.
   * @complexity Time O(log n), Space O(1)
   * @note Self-merge is a no-op.
   * @note Stateless comparators are compatible by type. Stateful comparators
   *       must be equality-comparable and compare equal.
   */
  auto merge(BinomialHeap&& other) -> void;

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
  // the internal forest to assert the binomial structure and heap order.
  // No production code depends on this friendship.
  friend struct detail::BinomialHeapInspector<BinomialHeap>;

  //===----- NODE TYPE ---------------------------------------------------------===//

  /**
   * @brief Internal forest node using the left-child / right-sibling layout.
   * @details A node owns its leftmost child and its next sibling; 'parent' is a
   *          non-owning back pointer used by sift-up.
   */
  struct Node {
    T                           value;
    int                         degree = 0;
    Node*                       parent = nullptr; ///< Non-owning; null for roots.
    std::unique_ptr<Node>       child;            ///< Leftmost (highest-degree) child.
    std::unique_ptr<Node>       sibling;          ///< Next sibling or next root.
    std::unique_ptr<HandleData> handle;           ///< Owns this element's handle record.

    /**
     * @brief Constructs the stored value in place from the forwarded arguments.
     * @param args Arguments to forward to the value constructor.
     */
    template <typename... Args>
    explicit Node(Args&&... args) : value(std::forward<Args>(args)...) {}
  };

  using NodePtr = std::unique_ptr<Node>;

  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  /**
   * @brief Makes 'child' the leftmost child of 'parent' (both degree k -> parent
   *        degree k+1). Assumes parent outranks child under the comparator.
   */
  static auto link(Node& parent, NodePtr child) -> void;

  ///@brief Merges two degree-sorted root lists into one degree-sorted list.
  static auto merge_root_lists(NodePtr lhs, NodePtr rhs) -> NodePtr;

  /**
   * @brief Unions this forest with a detached root list, coalescing equal
   *        degrees; the result replaces 'head_'.
   * @param other_head The detached, degree-ascending root list to meld in.
   * @param incoming_count Element count of the incoming list (an upper bound
   *        is fine); used to pre-reserve the coalescing buffer so an
   *        allocation failure cannot destroy melded nodes.
   */
  auto union_with(NodePtr other_head, size_type incoming_count) -> void;

  ///@brief Returns the root of highest priority (null only when the forest is empty).
  auto find_top_root() const -> Node*;

  /**
   * @brief Detaches 'root' from the root list and rebuilds the forest from the
   *        remaining roots plus 'root's reversed children.
   * @return The detached node, stripped of its children.
   */
  auto extract_root(Node* root) -> NodePtr;

  /**
   * @brief Sift-up: swaps the element (value + handle record) toward the root of
   *        its tree while it outranks its parent; if 'to_root' is set it bubbles
   *        unconditionally all the way to the tree root (used by 'erase').
   */
  static auto sift_up(Node* node, const Compare& comp, bool to_root) -> Node*;

  /**
   * @brief Swaps the values and handle records of two nodes, keeping each handle
   *        record pointing at the node that now holds its element.
   * @note Not noexcept: swapping T values may throw for throwing-swap types.
   */
  static auto swap_payload(Node& a, Node& b) -> void;

  /**
   * @brief Throws HeapException if the heap is empty.
   * @param operation Name of the calling operation, used in the message.
   */
  auto validate_non_empty(const char* operation) const -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  NodePtr   head_;     ///< First root of the degree-sorted root list (null if empty).
  size_type size_ = 0; ///< Number of elements in the heap.
  Compare   comp_;     /// Comparison function object defining priority order.
};

template <typename T>
using MaxBinomialHeap = BinomialHeap<T, std::less<T>>;

template <typename T>
using MinBinomialHeap = BinomialHeap<T, std::greater<T>>;

} // namespace ads::heaps

#include "../../../src/ads/heaps/Binomial_Heap.tpp"

#endif // BINOMIAL_HEAP_HPP

//===---------------------------------------------------------------------------===//
