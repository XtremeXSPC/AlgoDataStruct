//===---------------------------------------------------------------------------===//
/**
 * @file Linked_Stack.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the LinkedStack class.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//
#pragma once

#ifndef LINKED_STACK_HPP
#define LINKED_STACK_HPP

#include <memory>
#include <utility>

#include "Stack.hpp"
#include "Stack_Exception.hpp"

namespace ads::stack {

/**
 * @brief A stack implementation based on a singly linked list.
 *
 * @details This class implements the Stack interface using a singly linked list
 *          where each push creates a new node at the head. This provides true O(1)
 *          push and pop operations without any need for reallocation.
 *
 *          The implementation uses std::unique_ptr for automatic memory management,
 *          creating a chain of ownership from the head through the list. The stack
 *          is move-only to prevent expensive deep copies.
 *
 * @tparam T The type of data to store in the stack.
 */
template <typename T>
class LinkedStack : public Stack<T> {
private:
  struct Node;

public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty stack.
   * @complexity Time O(1), Space O(1)
   */
  LinkedStack() noexcept;

  /**
   * @brief Move constructor.
   * @param other The stack from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  LinkedStack(LinkedStack&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The stack from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(LinkedStack&& other) noexcept -> LinkedStack&;

  /**
   * @brief Destructor. Empties the stack and deallocates all nodes.
   * @complexity Time O(n), Space O(1)
   * @note Uses iterative deallocation to avoid stack overflow.
   */
  ~LinkedStack() override;

  // Copy constructor and assignment are disabled (move-only type).
  LinkedStack(const LinkedStack&)                    = delete;
  auto operator=(const LinkedStack&) -> LinkedStack& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Constructs an element in-place on top of the stack.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T&;

  /**
   * @brief Pushes an element onto the top of the stack (copy).
   * @param value The value to push.
   * @complexity Time O(1), Space O(1)
   */
  void push(const T& value) override;

  /**
   * @brief Pushes an element onto the top of the stack (move).
   * @param value The value to push.
   * @complexity Time O(1), Space O(1)
   */
  void push(T&& value) override;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the top element from the stack.
   * @throws StackUnderflowException if the stack is empty.
   * @complexity Time O(1), Space O(1)
   */
  void pop() override;

  /**
   * @brief Removes all elements from the stack.
   * @complexity Time O(n), Space O(1)
   */
  void clear() noexcept override;

  //===--------------------------- ACCESS OPERATIONS ---------------------------===//

  /**
   * @brief Returns a reference to the top element.
   * @return Reference to the top element.
   * @throws StackUnderflowException if the stack is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() -> T& override;

  /**
   * @brief Returns a const reference to the top element.
   * @return Const reference to the top element.
   * @throws StackUnderflowException if the stack is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() const -> const T& override;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if the stack is empty.
   * @return true if the stack is empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool override;

  /**
   * @brief Returns the number of elements in the stack.
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
  std::unique_ptr<Node> head_; ///< Pointer to the top node (owns the node)
  size_t                size_; ///< The current number of elements
};

} // namespace ads::stack

// Include the implementation file for templates.
#include "../../../src/ads/stacks/Linked_Stack.tpp"

#endif // LINKED_STACK_HPP

//===---------------------------------------------------------------------------===//
