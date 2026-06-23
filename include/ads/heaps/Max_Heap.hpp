//===---------------------------------------------------------------------------===//
/**
 * @file Max_Heap.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the MaxHeap class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef MAX_HEAP_HPP
#define MAX_HEAP_HPP

#include "../arrays/Dynamic_Array.hpp"
#include "Heap_Concepts.hpp"
#include "Heap_Exception.hpp"

#include <concepts>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <vector>

namespace ads::heaps {

/**
 * @brief A binary max heap implemented using an array.
 *
 * @details This class implements a binary max heap where the parent node is always
 *          greater than or equal to its children. The heap is stored in a dynamic
 *          array with the following properties:
 *          - Root at index 0
 *          - For node at index i:
 *            - Parent at (i-1)/2
 *            - Left child at 2*i+1
 *            - Right child at 2*i+2
 *
 *          Provides O(log n) insertion and deletion, O(1) access to maximum.
 *
 * @tparam T The type of elements stored in the heap (must be comparable).
 */
template <OrderedHeapValue T>
class MaxHeap {
public:
  using value_type = T;
  using size_type  = size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty heap with specified initial capacity.
   * @param initial_capacity Initial capacity of the heap.
   * @complexity Time O(1), Space O(n) where n is initial_capacity
   */
  explicit MaxHeap(size_type initial_capacity = kInitialCapacity);

  /**
   * @brief Constructs a heap from a vector (heapify in O(n)).
   * @param elements Vector of elements to heapify.
   * @complexity Time O(n), Space O(n)
   * @note Uses bottom-up heapify for O(n) construction
   */
  explicit MaxHeap(const std::vector<T>& elements);

  /**
   * @brief Constructs a heap from an iterator range.
   * @tparam InputIt Input iterator type.
   * @param first Iterator to the first element.
   * @param last Iterator past the last element.
   * @complexity Time O(n), Space O(n)
   * @note Uses bottom-up heapify for O(n) construction.
   */
  template <std::input_iterator InputIt>
  explicit MaxHeap(InputIt first, InputIt last) requires HeapRangeValue<InputIt, T>;

  /**
   * @brief Constructs a heap from an initializer list (heapify in O(n)).
   * @param values Initializer list of elements to heapify.
   * @complexity Time O(n), Space O(n)
   * @note Uses bottom-up heapify for O(n) construction.
   */
  MaxHeap(std::initializer_list<T> values);

  /**
   * @brief Move constructor.
   * @param other The heap to move from.
   * @complexity Time O(1), Space O(1)
   */
  MaxHeap(MaxHeap&& other) noexcept;

  /**
   * @brief Destructor. Deallocates the array and destroys all elements.
   * @complexity Time O(n), Space O(1)
   */
  ~MaxHeap();

  /**
   * @brief Move assignment operator.
   * @param other The heap to move from.
   * @return Reference to this.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(MaxHeap&& other) noexcept -> MaxHeap&;

  // Copy constructor and assignment are disabled (move-only type).
  MaxHeap(const MaxHeap&)                    = delete;
  auto operator=(const MaxHeap&) -> MaxHeap& = delete;

  //===----- INSERTION OPERATIONS ----------------------------------------------===//

  /**
   * @brief Inserts an element into the heap.
   * @param value The value to insert (lvalue reference).
   * @complexity Time O(log n) amortized, Space O(1)
   * @note May trigger reallocation if capacity is exceeded.
   */
  auto insert(const T& value) -> void requires CopyHeapValue<T>;

  /**
   * @brief Inserts an element into the heap (move version).
   * @param value The value to insert (rvalue reference).
   * @complexity Time O(log n) amortized, Space O(1)
   * @note May trigger reallocation if capacity is exceeded.
   */
  auto insert(T&& value) -> void requires MoveHeapValue<T>;

  /**
   * @brief Constructs an element in-place in the heap.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @return Reference to the newly constructed element.
   * @complexity Time O(log n) amortized, Space O(1)
   * @note May trigger reallocation if capacity is exceeded.
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T& requires EmplaceHeapValue<T, Args...>;

  //===----- ACCESS OPERATIONS -------------------------------------------------===//

  /**
   * @brief Returns a reference to the maximum element.
   * @return Reference to the maximum element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(1), Space O(1).
   */
  auto top() -> T&;

  /**
   * @brief Returns a const reference to the maximum element.
   * @return Const reference to the maximum element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() const -> const T&;

  //===----- REMOVAL OPERATIONS ------------------------------------------------===//

  /**
   * @brief Removes and returns the maximum element.
   * @return The maximum element.
   * @throws HeapException if the heap is empty.
   * @complexity Time O(log n), Space O(1)
   */
  auto extract_max() -> T;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

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
  [[nodiscard]] auto size() const noexcept -> size_type;

  /**
   * @brief Returns the current capacity of the heap.
   * @return The capacity.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto capacity() const noexcept -> size_type;

  /**
   * @brief Reserves storage for at least the specified number of elements.
   * @param new_capacity Minimum desired capacity.
   * @complexity Time O(n) if reallocation occurs, Space O(n)
   */
  auto reserve(size_type new_capacity) -> void;

  /**
   * @brief Removes all elements from the heap.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===----- ADVANCED OPERATIONS -----------------------------------------------===//

  /**
   * @brief Increases the value at a given index.
   * @param index Index of the element to increase.
   * @param new_value New value (must be greater than current value).
   * @throws HeapException if index is out of bounds or new_value <= old_value.
   * @complexity Time O(log n), Space O(1)
   */
  auto increase_key(size_type index, const T& new_value) -> void;

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  /**
   * @brief Restores heap property by moving element up.
   * @param index Index of the element to bubble up.
   * @return Final index of the element after bubbling up.
   * @complexity Time O(log n), Space O(1)
   */
  auto heapify_up(size_t index) -> size_t;

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
   * @brief Throws HeapException if the heap is empty.
   * @param operation Name of the calling operation, used in the message.
   */
  auto validate_non_empty(const char* operation) const -> void;

  /**
   * @brief Throws HeapException if the index is out of bounds.
   * @param index Index to validate.
   * @param operation Name of the calling operation, used in the message.
   */
  auto validate_index(size_type index, const char* operation) const -> void;

  //===----- INDEX CALCULATION METHODS -----------------------------------------===//

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

  //===----- DATA MEMBERS ------------------------------------------------------===//

  ads::arrays::DynamicArray<T> data_; ///< Contiguous heap storage.

  static constexpr size_t kInitialCapacity = 16; ///< Default initial capacity.
};

} // namespace ads::heaps

// Include the implementation file for templates.
#include "../../../src/ads/heaps/Max_Heap.tpp"

#endif // MAX_HEAP_HPP

//===---------------------------------------------------------------------------===//
