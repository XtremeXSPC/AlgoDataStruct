//===---------------------------------------------------------------------------===//
/**
 * @file D_Ary_Heap.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the DAryHeap class.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef D_ARY_HEAP_HPP
#define D_ARY_HEAP_HPP

#include "../arrays/Dynamic_Array.hpp"
#include "Heap_Concepts.hpp"
#include "Heap_Exception.hpp"

#include <algorithm>
#include <concepts>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::heaps {

/**
 * @brief A configurable d-ary heap backed by a dynamic array.
 *
 * @details This class generalizes the binary heap by allowing each node to have
 *          'd' children, where 'd >= 2' is chosen at construction time. Elements
 *          are stored in level order inside a contiguous 'DynamicArray', so no
 *          node allocation is needed and parent/child navigation is pure index
 *          arithmetic.
 *
 *          Increasing the arity shortens the heap height, which can reduce the
 *          number of upward moves during 'insert' and 'update_key'. The tradeoff
 *          is that 'heapify_down' must scan up to 'd' children at every level, so
 *          extraction costs O(d log_d n). This makes the structure useful when a
 *          wider, cache-friendly heap is preferable to a strictly binary one.
 *
 *          Priority is defined by 'Compare'. With the default 'std::less<T>',
 *          'top()' yields the maximum element, matching 'PriorityQueue' and the
 *          rest of the heap family; use 'MinDAryHeap' for min-heap semantics or
 *          'MaxDAryHeap' for the explicit max-heap alias. The type is move-only,
 *          array-backed, and intentionally not meldable.
 *
 *          Complexity: 'top' is O(1); 'insert' and upward 'update_key' are
 *          O(log_d n); 'extract_top', downward 'update_key', and bottom-up
 *          repair steps are O(d log_d n); construction from a range uses
 *          bottom-up heapify in O(n).
 *
 *          Exception safety: 'top'/'extract_top' on an empty heap and
 *          'update_key' with an invalid index throw 'HeapException' without
 *          modifying the heap; 'clear' is 'noexcept', and move operations follow
 *          the comparator's move guarantees.
 *
 *          For a node at index 'i':
 *            - Parent at '(i - 1) / d'
 *            - First child at 'd * i + 1'
 *            - Last child at 'd * i + d'
 *
 * @tparam T The element type.
 * @tparam Compare Comparator defining priority order.
 */
template <HeapValue T, typename Compare = std::less<T>>
class DAryHeap {
public:
  using value_type = T;
  using size_type  = size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty d-ary heap.
   * @param arity Number of children per node. Must be at least 2.
   * @param initial_capacity Initial storage capacity.
   * @param comp Comparison function object.
   * @throws HeapException if arity < 2.
   * @complexity Time O(1), Space O(initial_capacity)
   */
  explicit DAryHeap(size_type arity = kDefaultArity, size_type initial_capacity = kInitialCapacity, Compare comp = Compare{});

  /**
   * @brief Constructs a heap from a vector using bottom-up heapify.
   * @param elements Elements to heapify.
   * @param arity Number of children per node. Must be at least 2.
   * @param comp Comparison function object.
   * @throws HeapException if arity < 2.
   * @complexity Time O(n), Space O(n)
   */
  explicit DAryHeap(const std::vector<T>& elements, size_type arity = kDefaultArity, Compare comp = Compare{});

  /**
   * @brief Constructs a heap from an iterator range using bottom-up heapify.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @param arity Number of children per node. Must be at least 2.
   * @param comp Comparison function object.
   * @throws HeapException if arity < 2.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  DAryHeap(InputIt first, InputIt last, size_type arity = kDefaultArity, Compare comp = Compare{}) requires HeapRangeValue<InputIt, T>;

  /**
   * @brief Constructs a heap from an initializer list using bottom-up heapify.
   * @param init Initializer list of elements.
   * @param arity Number of children per node. Must be at least 2.
   * @param comp Comparison function object.
   * @throws HeapException if arity < 2.
   * @complexity Time O(n), Space O(n)
   */
  DAryHeap(std::initializer_list<T> init, size_type arity = kDefaultArity, Compare comp = Compare{});

  /**
   * @brief Move constructor.
   * @param other Heap to move from.
   * @complexity Time O(1), Space O(1)
   */
  DAryHeap(DAryHeap&& other) noexcept(std::is_nothrow_move_constructible_v<Compare>);

  /**
   * @brief Move assignment operator.
   * @param other Heap to move from.
   * @return Reference to this heap.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(DAryHeap&& other) noexcept(std::is_nothrow_move_assignable_v<Compare>) -> DAryHeap&;

  /**
   * @brief Destructor.
   * @complexity Time O(n), Space O(1)
   */
  ~DAryHeap() = default;

  DAryHeap(const DAryHeap&)                    = delete;
  auto operator=(const DAryHeap&) -> DAryHeap& = delete;

  //===----- INSERTION OPERATIONS ----------------------------------------------===//

  /**
   * @brief Inserts an element into the heap.
   * @param value Value to insert.
   * @complexity Time O(log_d n) amortized, Space O(1)
   */
  auto insert(const T& value) -> void requires CopyHeapValue<T>;

  /**
   * @brief Inserts an element into the heap using move semantics.
   * @param value Value to insert.
   * @complexity Time O(log_d n) amortized, Space O(1)
   */
  auto insert(T&& value) -> void requires MoveHeapValue<T>;

  /**
   * @brief Constructs an element in-place inside the heap.
   * @tparam Args Constructor argument types.
   * @param args Constructor arguments.
   * @return Reference to the inserted element at its final heap position.
   * @complexity Time O(log_d n) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T& requires EmplaceHeapValue<T, Args...>;

  //===----- ACCESS OPERATIONS -------------------------------------------------===//

  /**
   * @brief Returns a reference to the top-priority element.
   * @return Reference to the top element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() -> T&;

  /**
   * @brief Returns a const reference to the top-priority element.
   * @return Const reference to the top element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() const -> const T&;

  //===----- REMOVAL OPERATIONS ------------------------------------------------===//

  /**
   * @brief Removes and returns the top-priority element.
   * @return The removed element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(log_d n), Space O(1)
   */
  auto extract_top() -> T;

  /**
   * @brief Replaces the value stored at an index and restores heap order.
   * @param index Index to update.
   * @param new_value Replacement value.
   * @throws HeapException if index is out of bounds.
   * @complexity Time O(log_d n), Space O(1)
   */
  auto update_key(size_type index, const T& new_value) -> void requires CopyHeapValue<T>;

  /**
   * @brief Replaces the value stored at an index using move semantics and restores heap order.
   * @param index Index to update.
   * @param new_value Replacement value.
   * @throws HeapException if index is out of bounds.
   * @complexity Time O(log_d n), Space O(1)
   */
  auto update_key(size_type index, T&& new_value) -> void requires MoveHeapValue<T>;

  /**
   * @brief Removes all elements from the heap.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

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
   * @brief Returns the current storage capacity.
   * @return Internal array capacity.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto capacity() const noexcept -> size_type;

  /**
   * @brief Returns the configured arity.
   * @return Number of children per node.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto arity() const noexcept -> size_type;

  /**
   * @brief Reserves storage for at least the specified number of elements.
   * @param new_capacity Minimum desired capacity.
   * @complexity Time O(n) if reallocation occurs, Space O(n)
   */
  auto reserve(size_type new_capacity) -> void;

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  ///@brief Returns true if 'lhs' has strictly higher priority than 'rhs'.
  [[nodiscard]] auto has_higher_priority(const T& lhs, const T& rhs) const -> bool;

  ///@brief Returns the parent index of a non-root node.
  [[nodiscard]] auto parent(size_t index) const noexcept -> size_t;

  ///@brief Returns the first child index for the node at 'index'.
  [[nodiscard]] auto first_child(size_t index) const noexcept -> size_t;

  ///@brief Sifts an element toward the root and returns its final index.
  auto heapify_up(size_t index) -> size_t;

  ///@brief Sifts an element toward the leaves until heap order is restored.
  auto heapify_down(size_t index) -> void;

  ///@brief Establishes the heap invariant over all stored elements bottom-up.
  auto build_heap() -> void;

  ///@brief Throws if the heap is empty before 'operation'.
  auto validate_non_empty(const char* operation) const -> void;

  ///@brief Throws if 'index' is out of bounds before 'operation'.
  auto validate_index(size_t index, const char* operation) const -> void;

  ///@brief Validates an arity value and returns it unchanged.
  static auto validate_arity(size_t arity) -> size_t;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  ads::arrays::DynamicArray<T> data_;  ///< Heap storage in level-order layout.
  size_t                       arity_; ///< Number of children per internal node.
  Compare                      comp_;  ///< Comparator defining priority order.

  static constexpr size_t kDefaultArity    = 2;  ///< Default binary-heap arity.
  static constexpr size_t kInitialCapacity = 16; ///< Default reserved capacity.
};

template <typename T>
using MaxDAryHeap = DAryHeap<T, std::less<T>>;

template <typename T>
using MinDAryHeap = DAryHeap<T, std::greater<T>>;

} // namespace ads::heaps

#include "../../../src/ads/heaps/D_Ary_Heap.tpp"

#endif // D_ARY_HEAP_HPP

//===---------------------------------------------------------------------------===//
