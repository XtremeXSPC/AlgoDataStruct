//===---------------------------------------------------------------------------===//
/**
 * @file Min_Heap.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the MinHeap class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef MIN_HEAP_HPP
#define MIN_HEAP_HPP

#include "Heap_Exception.hpp"

#include <algorithm>
#include <memory>
#include <new>
#include <utility>
#include <vector>

namespace ads::heaps {

/**
 * @brief A binary min heap implemented using an array.
 *
 * @details This class implements a binary min heap where the parent node
 *          is always less than or equal to its children. The heap is
 *          stored in a dynamic array with the following properties:
 *          - Root at index 0
 *          - For node at index i:
 *            - Parent at (i-1)/2
 *            - Left child at 2*i+1
 *            - Right child at 2*i+2
 *
 *          Provides O(log n) insertion and deletion, O(1) access to minimum.
 *
 * @tparam T The type of elements stored in the heap (must be comparable).
 */
template <typename T>
class MinHeap {
public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty heap with specified initial capacity.
   * @param initial_capacity Initial capacity of the heap.
   * @complexity Time O(1), Space O(n) where n is initial_capacity
   */
  explicit MinHeap(size_t initial_capacity = kInitialCapacity);

  /**
   * @brief Constructs a heap from a vector (heapify in O(n)).
   * @param elements Vector of elements to heapify.
   * @complexity Time O(n), Space O(n)
   * @note Uses bottom-up heapify for O(n) construction.
   */
  explicit MinHeap(const std::vector<T>& elements);

  /**
   * @brief Move constructor.
   * @param other The heap to move from.
   * @complexity Time O(1), Space O(1)
   */
  MinHeap(MinHeap&& other) noexcept;

  /**
   * @brief Destructor. Deallocates the array and destroys all elements.
   * @complexity Time O(n), Space O(1)
   */
  ~MinHeap();

  /**
   * @brief Move assignment operator.
   * @param other The heap to move from.
   * @return Reference to this.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(MinHeap&& other) noexcept -> MinHeap&;

  // Copy constructor and assignment are disabled (move-only type).
  MinHeap(const MinHeap&)                    = delete;
  auto operator=(const MinHeap&) -> MinHeap& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts an element into the heap.
   * @param value The value to insert (lvalue reference).
   * @complexity Time O(log n) amortized, Space O(1)
   * @note May trigger reallocation if capacity is exceeded.
   */
  auto insert(const T& value) -> void;

  /**
   * @brief Inserts an element into the heap (move version).
   * @param value The value to insert (rvalue reference).
   * @complexity Time O(log n) amortized, Space O(1)
   * @note May trigger reallocation if capacity is exceeded.
   */
  auto insert(T&& value) -> void;

  /**
   * @brief Constructs an element in-place in the heap.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(log n) amortized, Space O(1)
   * @note May trigger reallocation if capacity is exceeded.
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T&;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Returns a reference to the minimum element.
   * @return Reference to the minimum element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() -> T&;

  /**
   * @brief Returns a const reference to the minimum element.
   * @return Const reference to the minimum element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() const -> const T&;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes and returns the minimum element.
   * @return The minimum element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(log n), Space O(1)
   */
  auto extract_min() -> T;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the heap is empty.
   * @return true if the heap is empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the heap.
   * @return The number of elements.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the current capacity of the heap.
   * @return The capacity.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto capacity() const noexcept -> size_t;

  /**
   * @brief Removes all elements from the heap.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===-------------------------- ADVANCED OPERATIONS --------------------------===//

  /**
   * @brief Decreases the value at a given index.
   * @param index Index of the element to decrease.
   * @param new_value New value (must be less than current value).
   * @throws HeapException if index is out of bounds or new_value >= old_value.
   * @complexity Time O(log n), Space O(1)
   */
  auto decrease_key(size_t index, const T& new_value) -> void;

private:
  //===----------------------------- DATA MEMBERS ------------------------------===//

  T*     data_;     ///< Pointer to the dynamic array.
  size_t size_;     ///< Number of elements in the heap.
  size_t capacity_; ///< Current capacity of the array.

  static constexpr size_t kInitialCapacity = 16; ///< Default initial capacity.
  static constexpr size_t kGrowthFactor    = 2;  ///< Growth factor for reallocation.

  //===============================================================================//
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Restores heap property by moving element up.
   * @param index Index of the element to bubble up.
   * @complexity Time O(log n), Space O(1)
   */
  auto heapify_up(size_t index) -> void;

  /**
   * @brief Restores heap property by moving element down.
   * @param index Index of the element to bubble down.
   * @complexity Time O(log n), Space O(1)
   */
  auto heapify_down(size_t index) -> void;

  /**
   * @brief Builds a heap from unordered array (bottom-up heapify).
   * @complexity Time O(n), Space O(1)
   */
  auto build_heap() -> void;

  /**
   * @brief Grows the heap capacity when full.
   * @complexity Time O(n), Space O(n)
   * @throws std::bad_alloc if allocation fails.
   */
  auto grow() -> void;

  //===----------------------- INDEX CALCULATION METHODS -----------------------===//

  /**
   * @brief Returns the index of the parent of node at index 'i'.
   * @param i Index of the node.
   * @return Index of the parent.
   * @complexity Time O(1), Space O(1)
   * @note Precondition: i > 0.
   */
  [[nodiscard]] static auto parent(size_t i) noexcept -> size_t { return (i - 1) / 2; }

  /**
   * @brief Returns the index of the left child of node at index 'i'.
   * @param i Index of the node.
   * @return Index of the left child.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static auto left_child(size_t i) noexcept -> size_t { return 2 * i + 1; }

  /**
   * @brief Returns the index of the right child of node at index 'i'.
   * @param i Index of the node.
   * @return Index of the right child.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static auto right_child(size_t i) noexcept -> size_t { return 2 * i + 2; }
};

} // namespace ads::heaps

#include "../../../src/ads/heaps/Min_Heap.tpp"

#endif // MIN_HEAP_HPP

//===---------------------------------------------------------------------------===//
