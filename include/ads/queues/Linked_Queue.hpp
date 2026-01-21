//===--------------------------------------------------------------------------===//
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
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef LINKED_QUEUE_HPP
#define LINKED_QUEUE_HPP

#include <algorithm>
#include <memory>
#include <utility>

#include "Queue.hpp"
#include "Queue_Exception.hpp"

namespace ads::queue {

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
  //========== CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ==========//

  /** @brief Constructs an empty queue. */
  LinkedQueue() noexcept;

  /** @brief Destructor. */
  ~LinkedQueue() override;

  // Copy constructor and assignment are disabled (move-only type)
  LinkedQueue(const LinkedQueue&)                    = delete;
  auto operator=(const LinkedQueue&) -> LinkedQueue& = delete;

  /**
   * @brief Move constructor.
   * @param other The queue from which to move resources.
   */
  LinkedQueue(LinkedQueue&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The queue from which to move resources.
   * @return A reference to this instance.
   */
  auto operator=(LinkedQueue&& other) noexcept -> LinkedQueue&;

  //========== INTERFACE INHERITED FROM Queue<T> ==========//

  void enqueue(const T& value) override;
  void enqueue(T&& value) override;
  void dequeue() override;

  auto front() -> T& override;
  auto rear() -> T& override;

  auto front() const -> const T& override;
  auto rear() const -> const T& override;

  [[nodiscard]] auto is_empty() const noexcept -> bool override;
  [[nodiscard]] auto size() const noexcept -> size_t override;

  void clear() noexcept override;

  //=========== ADDITIONAL FUNCTIONALITY ==========//

  /**
   * @brief Constructs an element in-place at the rear of the queue.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T&;

private:
  //================ INTERNAL NODE ================//
  struct Node {
    T                     data;
    std::unique_ptr<Node> next;

    template <typename... Args>
    explicit Node(Args&&... args) : data(std::forward<Args>(args)...), next(nullptr) {}
  };

  //================ DATA MEMBERS ================//
  std::unique_ptr<Node> front_; ///< Pointer to the front node
  Node*                 rear_;  ///< Raw pointer to the rear node (non-owning)
  size_t                size_;  ///< The current number of elements
};

} // namespace ads::queue

// Include the implementation file for templates
#include "../../../src/ads/queues/Linked_Queue.tpp"

#endif // LINKED_QUEUE_HPP

//===--------------------------------------------------------------------------===//
