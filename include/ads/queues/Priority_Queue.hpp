//===---------------------------------------------------------------------------===//
/**
 * @file Priority_Queue.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the PriorityQueue class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include "Queue_Exception.hpp"

#include <functional>
#include <initializer_list>
#include <utility>
#include <vector>

namespace ads::queues {

/**
 * @brief A priority queue implemented using a binary heap.
 *
 * @details This class implements a priority queue where elements are served
 *          based on their priority rather than FIFO order. By default, it
 *          behaves as a max-heap (highest priority element first), but can
 *          be configured as a min-heap using std::greater<T>.
 *
 *          The underlying implementation uses a dynamic array-based binary
 *          heap with the following properties:
 *          - Root at index 0
 *          - For node at index i:
 *            - Parent at (i-1)/2
 *            - Left child at 2*i+1
 *            - Right child at 2*i+2
 *
 *          Provides O(log n) insertion and deletion, O(1) access to top element.
 *
 * @tparam T The type of elements stored in the queue (must be comparable).
 * @tparam Compare Comparison function object (default: std::less<T> for max-heap).
 *
 */
template <typename T, typename Compare = std::less<T>>
class PriorityQueue {
public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty priority queue.
   * @param comp Comparison function object.
   * @complexity Time O(1), Space O(1)
   */
  explicit PriorityQueue(Compare comp = Compare{});

  /**
   * @brief Constructs a priority queue from a vector (heapify in O(n)).
   * @param elements Vector of elements to heapify.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   * @note Uses bottom-up heapify for O(n) construction.
   */
  PriorityQueue(const std::vector<T>& elements, Compare comp = Compare{});

  /**
   * @brief Constructs a priority queue from an initializer list.
   * @param init Initializer list of elements.
   * @param comp Comparison function object.
   * @complexity Time O(n), Space O(n)
   */
  PriorityQueue(std::initializer_list<T> init, Compare comp = Compare{});

  /**
   * @brief Move constructor.
   * @param other The queue to move from.
   * @complexity Time O(1), Space O(1)
   */
  PriorityQueue(PriorityQueue&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The queue to move from.
   * @return Reference to this.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(PriorityQueue&& other) noexcept -> PriorityQueue&;

  /**
   * @brief Destructor.
   * @complexity Time O(n), Space O(1)
   */
  ~PriorityQueue() = default;

  // Copy constructor and assignment are disabled (move-only type).
  PriorityQueue(const PriorityQueue&)                    = delete;
  auto operator=(const PriorityQueue&) -> PriorityQueue& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts an element into the priority queue.
   * @param value The value to insert (lvalue reference).
   * @complexity Time O(log n) amortized, Space O(1)
   */
  auto push(const T& value) -> void;

  /**
   * @brief Inserts an element into the priority queue (move version).
   * @param value The value to insert (rvalue reference).
   * @complexity Time O(log n) amortized, Space O(1)
   */
  auto push(T&& value) -> void;

  /**
   * @brief Constructs an element in-place in the priority queue.
   * @tparam Args Types of arguments to forward to T's constructor.
   * @param args Arguments to forward to T's constructor.
   * @complexity Time O(log n) amortized, Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> void;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the element with highest priority.
   * @throws QueueException if the queue is empty.
   * @complexity Time O(log n), Space O(1)
   */
  auto pop() -> void;

  /**
   * @brief Removes all elements from the priority queue.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Returns a reference to the element with highest priority.
   * @return Reference to the top element.
   * @throws QueueException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() -> T&;

  /**
   * @brief Returns a const reference to the element with highest priority.
   * @return Const reference to the top element.
   * @throws QueueException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() const -> const T&;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the priority queue is empty.
   * @return true if the queue is empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto empty() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the queue.
   * @return The number of elements.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  //===-------------------------- UTILITY OPERATIONS ---------------------------===//

  /**
   * @brief Reserves capacity for at least the specified number of elements.
   * @param capacity Minimum capacity to reserve.
   * @complexity Time O(n) if reallocation occurs, Space O(n)
   * @throws std::bad_alloc if memory allocation fails.
   */
  auto reserve(size_t capacity) -> void;

  /**
   * @brief Extracts all elements in sorted order.
   * @return Vector of elements in sorted order.
   * @complexity Time O(n log n), Space O(n)
   * @note This operation empties the queue.
   */
  auto sorted_elements() -> std::vector<T>;

private:
  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::vector<T> heap_; ///< Dynamic array storing the heap.
  Compare        comp_; ///< Comparison function object.

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

} // namespace ads::queues

#include "../../../src/ads/queues/Priority_Queue.tpp"

#endif // PRIORITY_QUEUE_HPP
//===--------------------------------------------------------------------------===//
