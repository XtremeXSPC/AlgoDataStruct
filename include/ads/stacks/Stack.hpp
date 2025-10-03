//===--------------------------------------------------------------------------===//
/**
 * @file Stack.hpp
 * @author Costantino Lombardi
 * @brief Abstract interface for a Stack data structure.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once
#ifndef STACK_HPP
#define STACK_HPP

#include <cstddef>

namespace ads::stack {

/**
 * @brief Abstract base class that defines common operations for a stack data structure.
 *
 * @details A stack is a Last-In-First-Out (LIFO) data structure that supports
 *          insertion and deletion only at one end, called the top. This interface
 *          provides the fundamental operations that any stack implementation must support.
 *
 * @tparam T The data type to store in the stack.
 */
template <typename T>
class Stack {
public:
  // Virtual destructor is mandatory for polymorphic base classes
  virtual ~Stack() = default;

  /**
   * @brief Pushes an element onto the top of the stack (copy).
   * @param value The value to push.
   */
  virtual void push(const T& value) = 0;

  /**
   * @brief Pushes an element onto the top of the stack (move).
   * @param value The r-value to move.
   */
  virtual void push(T&& value) = 0;

  /**
   * @brief Removes the top element from the stack.
   * @throws StackException if the stack is empty.
   */
  virtual void pop() = 0;

  /**
   * @brief Returns a reference to the top element.
   * @return T& Reference to the top element.
   * @throws StackException if the stack is empty.
   */
  virtual auto top() -> T& = 0;

  /**
   * @brief Returns a constant reference to the top element.
   * @return const T& Constant reference to the top element.
   * @throws StackException if the stack is empty.
   */
  virtual auto top() const -> const T& = 0;

  /**
   * @brief Checks if the stack is empty.
   * @return true if the stack contains no elements.
   * @return false otherwise.
   */
  [[nodiscard]] virtual auto is_empty() const noexcept -> bool = 0;

  /**
   * @brief Returns the number of elements in the stack.
   * @return size_t The number of elements.
   */
  [[nodiscard]] virtual auto size() const noexcept -> size_t = 0;

  /**
   * @brief Empties the stack, removing all elements.
   */
  virtual void clear() noexcept = 0;
};

} // namespace ads::stack

#endif // STACK_HPP

//===--------------------------------------------------------------------------===//