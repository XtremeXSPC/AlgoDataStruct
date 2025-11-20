//===--------------------------------------------------------------------------===//
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
//===--------------------------------------------------------------------------===//
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
  //========== CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ==========//

  /** @brief Constructs an empty stack. */
  LinkedStack() noexcept;

  /** @brief Destructor. */
  ~LinkedStack() override;

  // Copy constructor and assignment are disabled (move-only type)
  LinkedStack(const LinkedStack&)                    = delete;
  auto operator=(const LinkedStack&) -> LinkedStack& = delete;

  /**
   * @brief Move constructor.
   * @param other The stack from which to move resources.
   */
  LinkedStack(LinkedStack&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The stack from which to move resources.
   * @return A reference to this instance.
   */
  auto operator=(LinkedStack&& other) noexcept -> LinkedStack&;

  //========== INTERFACE INHERITED FROM Stack<T> ==========//

  void push(const T& value) override;
  void push(T&& value) override;
  void pop() override;

  auto top() -> T& override;
  auto top() const -> const T& override;

  [[nodiscard]] auto is_empty() const noexcept -> bool override;
  [[nodiscard]] auto size() const noexcept -> size_t override;

  void clear() noexcept override;

  //=========== ADDITIONAL FUNCTIONALITY ==========//

  /**
   * @brief Constructs an element in-place on top of the stack.
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
  std::unique_ptr<Node> head_; ///< Pointer to the top node
  size_t                size_; ///< The current number of elements
};

// Include the implementation file for templates
#include "../../../src/ads/stacks/Linked_Stack.tpp"

} // namespace ads::stack

#endif // LINKED_STACK_HPP

//===--------------------------------------------------------------------------===//
