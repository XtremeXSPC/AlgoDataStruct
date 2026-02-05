//===---------------------------------------------------------------------------===//
/**
 * @file Linked_Queue.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the LinkedQueue class.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef LINKED_QUEUE_HPP
#define LINKED_QUEUE_HPP

#include "Queue.hpp"
#include "Queue_Exception.hpp"

#include <algorithm>
#include <memory>
#include <utility>

namespace ads::queues {

/**
 * @brief A queue implementation based on a singly linked list.
 *
 * @details This class implements the Queue interface using a singly linked list
 *          with pointers to both front and rear nodes. This design provides true O(1)
 *          enqueue and dequeue operations without any need for reallocation.
 *
 *          Elements are added at the rear and removed from the front, maintaining
 *          FIFO ordering. The implementation uses std::unique_ptr for automatic
 *          memory management, creating a chain of ownership from front through
 *          the list. The queue is move-only to prevent expensive deep copies.
 *
 * @tparam T The type of data to store in the queue.
 */
template <typename T>
class LinkedQueue : public Queue<T> {
private:
  struct Node;

public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty queue.
   * @complexity Time O(1), Space O(1)
   */
  LinkedQueue() noexcept;

  /**
   * @brief Move constructor.
   * @param other The queue from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  LinkedQueue(LinkedQueue&& other) noexcept;

  /**
   * @brief Destructor. Empties the queue and deallocates all nodes.
   * @complexity Time O(n), Space O(1)
   * @note Uses iterative deallocation to avoid stack overflow.
   */
  ~LinkedQueue() override;

  /**
   * @brief Move assignment operator.
   * @param other The queue from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(LinkedQueue&& other) noexcept -> LinkedQueue&;

  // Copy constructor and assignment are disabled (move-only type).
  LinkedQueue(const LinkedQueue&)                    = delete;
  auto operator=(const LinkedQueue&) -> LinkedQueue& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Adds an element to the rear of the queue (copy).
   * @param value The value to enqueue.
   * @complexity Time O(1), Space O(1)
   */
  void enqueue(const T& value) override;

  /**
   * @brief Adds an element to the rear of the queue (move).
   * @param value The value to enqueue.
   * @complexity Time O(1), Space O(1)
   */
  void enqueue(T&& value) override;

  /**
   * @brief Constructs an element in-place at the rear of the queue.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T&;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the element at the front of the queue.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  void dequeue() override;

  /**
   * @brief Removes all elements from the queue.
   * @complexity Time O(n), Space O(1)
   */
  void clear() noexcept override;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Returns a reference to the front element.
   * @return Reference to the front element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() -> T& override;

  /**
   * @brief Returns a const reference to the front element.
   * @return Const reference to the front element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto front() const -> const T& override;

  /**
   * @brief Returns a reference to the rear element.
   * @return Reference to the rear element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto rear() -> T& override;

  /**
   * @brief Returns a const reference to the rear element.
   * @return Const reference to the rear element.
   * @throws QueueUnderflowException if the queue is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto rear() const -> const T& override;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the queue is empty.
   * @return true if the queue is empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool override;

  /**
   * @brief Returns the number of elements in the queue.
   * @return The number of elements.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t override;

private:
  //===------------------------ INTERNAL NODE STRUCTURE ------------------------===//
  /**
   * @brief Internal node structure.
   *
   * @details Each node contains data and a unique_ptr to the next node.
   *          The unique_ptr provides automatic memory management.
   */
  struct Node {
    T                     data;
    std::unique_ptr<Node> next;

    template <typename... Args>
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...), next(nullptr) {}
  };

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<Node> front_; ///< Pointer to the front node (owns the node).
  Node*                 rear_;  ///< Raw pointer to the rear node (non-owning).
  size_t                size_;  ///< The current number of elements.
};

} // namespace ads::queues

// Include the implementation file for templates.
#include "../../../src/ads/queues/Linked_Queue.tpp"

#endif // LINKED_QUEUE_HPP

//===---------------------------------------------------------------------------===//
