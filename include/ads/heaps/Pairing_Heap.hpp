//===---------------------------------------------------------------------------===//
/**
 * @file Pairing_Heap.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the PairingHeap class (a meldable priority queue with handles).
 * @version 0.1
 * @date 2026-07-08
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef PAIRING_HEAP_HPP
#define PAIRING_HEAP_HPP

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
struct PairingHeapInspector;
} // namespace detail

/**
 * @brief A pairing heap: a meldable priority queue with fast, handle-based
 *        'decrease_key' and 'erase'.
 *
 * @details A pairing heap is a single multi-way tree stored with the left-child /
 *          right-sibling layout. Every operation is built on one primitive,
 *          'meld', which makes the lower-priority of two roots the leftmost child
 *          of the other. 'insert' melds a single node; 'extract_top' removes the
 *          root and recombines its children with the classic **two-pass** pairing
 *          (meld neighbours left to right, then fold right to left).
 *
 *          Like the binomial heap it offers a stable @ref Handle from 'emplace',
 *          but the mechanism is different and simpler: 'decrease_key' **cuts** the
 *          node's subtree and melds it back at the root, and no operation ever
 *          moves a value between nodes. A node therefore keeps its address for
 *          life, so a handle is just a node pointer and stays valid until its
 *          element leaves the heap (via 'extract_top', 'erase', or 'clear'), after
 *          which using it is undefined behaviour. This relink-based 'decrease_key'
 *          is the pairing heap's claim to fame: O(1) amortized in practice.
 *
 *          Priority is defined by 'Compare'. With the default 'std::less<T>',
 *          'top()' yields the **maximum** element, matching the rest of the heap
 *          family; use 'MinPairingHeap' for min-heap semantics. Following the
 *          codebase convention (see 'MinHeap::decrease_key'), 'decrease_key'
 *          adjusts an element to a value of **higher priority** under 'Compare'
 *          (for the default max-heap that means a larger value). The type is
 *          move-only and models the 'MeldableHeap' concept.
 *
 *          Complexity: O(1) 'insert'/'merge'/'top'; O(log n) amortized
 *          'extract_top' and 'erase'; O(1) amortized 'decrease_key'.
 *
 *          Exception safety: 'top'/'extract_top' on an empty heap and an invalid
 *          'decrease_key'/'erase' throw 'HeapException' without modifying the
 *          heap; 'clear' and the move operations are 'noexcept'.
 *
 * @tparam T The element type.
 * @tparam Compare Comparator defining priority order (default 'std::less<T>').
 */
template <HeapValue T, typename Compare = std::less<T>>
class PairingHeap {
private:
  struct Node;

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
    friend class PairingHeap;

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
  explicit PairingHeap(Compare comp = Compare{}) noexcept(std::is_nothrow_move_constructible_v<Compare>);

  /**
   * @brief Constructs a heap from an initializer list.
   * @param values Elements to insert.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   */
  PairingHeap(std::initializer_list<T> values, Compare comp = Compare{}) requires CopyHeapValue<T>;

  /**
   * @brief Constructs a heap from an iterator range.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  PairingHeap(InputIt first, InputIt last, Compare comp = Compare{}) requires HeapRangeValue<InputIt, T>;

  /**
   * @brief Constructs a heap from a vector.
   * @param elements Elements to insert.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   */
  explicit PairingHeap(const std::vector<T>& elements, Compare comp = Compare{}) requires CopyHeapValue<T>;

  /**
   * @brief Move constructor.
   * @param other The heap to move from; left empty.
   * @complexity Time O(1), Space O(1)
   */
  PairingHeap(PairingHeap&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The heap to move from; left empty.
   * @return Reference to this.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(PairingHeap&& other) noexcept -> PairingHeap&;

  /**
   * @brief Destructor. Destroys all nodes.
   * @complexity Time O(n), Space O(1)
   */
  ~PairingHeap();

  // Copy operations are disabled (move-only type).
  PairingHeap(const PairingHeap&)                    = delete;
  auto operator=(const PairingHeap&) -> PairingHeap& = delete;

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
   * @complexity Time O(log n) amortized, Space O(n) for the two-pass buffer.
   */
  auto extract_top() -> T;

  /**
   * @brief Removes the element referred to by a handle.
   * @param handle A valid handle obtained from 'emplace'.
   * @throws HeapException if the handle is empty.
   * @complexity Time O(log n) amortized, Space O(n)
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
  auto merge(PairingHeap&& other) -> void;

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
   * @complexity Time O(n), Space O(n) for the teardown buffer.
   */
  auto clear() noexcept -> void;

private:
  // White-box test access (defined only in the unit tests): lets the suite walk
  // the internal tree to assert heap order and the prev-pointer wiring. No
  // production code depends on this friendship.
  friend struct detail::PairingHeapInspector<PairingHeap>;

  //===----- NODE TYPE ---------------------------------------------------------===//

  /**
   * @brief Internal tree node using the left-child / right-sibling layout.
   * @details 'prev' is a non-owning back pointer: it points to the parent when
   *          the node is a leftmost child, otherwise to its left sibling. That is
   *          exactly what an O(1) cut in 'decrease_key' / 'erase' needs.
   */
  struct Node {
    T                     value;
    Node*                 prev = nullptr; ///< Parent (if leftmost child) or left sibling.
    std::unique_ptr<Node> child;          ///< Leftmost child (owning).
    std::unique_ptr<Node> sibling;        ///< Next sibling (owning).

    ///@brief Constructs the stored value in place from the forwarded arguments.
    template <typename... Args>
    explicit Node(Args&&... args) : value(std::forward<Args>(args)...) {}
  };

  using NodePtr = std::unique_ptr<Node>;

  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  /**
   * @brief Melds two trees and returns the merged root (lower priority becomes
   *        the leftmost child of the higher).
   */
  static auto meld(NodePtr lhs, NodePtr rhs, const Compare& comp) -> NodePtr;

  /**
   * @brief Recombines a sibling list of subtrees into one tree with the classic
   *        two-pass pairing. The returned root has a null 'prev'.
   */
  static auto combine_siblings(NodePtr first, const Compare& comp) -> NodePtr;

  /**
   * @brief Detaches a non-root node (with its subtree) from its sibling list,
   *        splicing the gap, and returns ownership of it. 'prev'/'sibling' of the
   *        returned node are cleared.
   */
  auto detach(Node* node) -> NodePtr;

  /**
   * @brief Removes a node from the heap and melds its children back in.
   */
  auto remove_node(Node* node) -> void;

  /**
   * @brief Throws HeapException if the heap is empty.
   * @param operation Name of the calling operation, used in the message.
   */
  auto validate_non_empty(const char* operation) const -> void;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  NodePtr   root_;     ///< Root of the pairing tree (null if empty).
  size_type size_ = 0; ///< Number of elements in the heap.
  Compare   comp_;     /// Comparison function object defining priority order.
};

template <typename T>
using MaxPairingHeap = PairingHeap<T, std::less<T>>;

template <typename T>
using MinPairingHeap = PairingHeap<T, std::greater<T>>;

} // namespace ads::heaps

#include "../../../src/ads/heaps/Pairing_Heap.tpp"

#endif // PAIRING_HEAP_HPP

//===---------------------------------------------------------------------------===//
