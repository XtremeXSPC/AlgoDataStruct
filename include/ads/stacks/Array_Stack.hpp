//===---------------------------------------------------------------------------===//
/**
 * @file Array_Stack.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the ArrayStack class.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef ARRAY_STACK_HPP
#define ARRAY_STACK_HPP

#include <algorithm>
#include <cstring>
#include <limits>
#include <memory>
#include <new>
#include <utility>

#include "Stack.hpp"
#include "Stack_Exception.hpp"

namespace ads::stacks {

/**
 * @brief A stack implementation based on a dynamic array.
 *
 * @details This class implements the Stack interface using a dynamically allocated
 *          array that grows automatically when needed. It provides amortized O(1)
 *          push and pop operations with excellent cache locality.
 *
 *          The implementation uses std::unique_ptr for automatic memory management
 *          and follows the RAII principle. The stack is move-only to prevent
 *          expensive deep copies.
 *
 * @tparam T The type of data to store in the stack.
 */
template <typename T>
class ArrayStack : public Stack<T> {
public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty stack with optional initial capacity.
   * @param initial_capacity The initial capacity (default: 16).
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(1), Space O(initial_capacity)
   */
  explicit ArrayStack(size_t initial_capacity = 16);

  /**
   * @brief Move constructor.
   * @param other The stack from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  ArrayStack(ArrayStack&& other) noexcept;

  /**
   * @brief Destructor. Empties the stack and deallocates all elements.
   * @complexity Time O(n), Space O(1)
   */
  ~ArrayStack() override = default;

  /**
   * @brief Move assignment operator.
   * @param other The stack from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(ArrayStack&& other) noexcept -> ArrayStack&;

  // Copy constructor and assignment are disabled (move-only type).
  ArrayStack(const ArrayStack&)                    = delete;
  auto operator=(const ArrayStack&) -> ArrayStack& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Constructs an element in-place on top of the stack.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   * @complexity Time O(1) amortized, Space O(1)
   * @note May trigger a resize with O(n) time.
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T&;

  /**
   * @brief Pushes an element onto the top of the stack (copy).
   * @param value The value to push.
   * @complexity Time O(1) amortized, Space O(1)
   * @note May trigger a resize with O(n) time.
   */
  void push(const T& value) override;

  /**
   * @brief Pushes an element onto the top of the stack (move).
   * @param value The value to push.
   * @complexity Time O(1) amortized, Space O(1)
   * @note May trigger a resize with O(n) time.
   */
  void push(T&& value) override;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes the top element from the stack.
   * @throws StackUnderflowException if the stack is empty.
   * @complexity Time O(1) amortized, Space O(1)
   * @note May trigger a shrink with O(n) time.
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

  //===-------------------------- CAPACITY OPERATIONS --------------------------===//

  /**
   * @brief Reserves capacity for at least n elements.
   * @param n The minimum capacity to reserve.
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(n) if reallocation occurs, Space O(n)
   */
  void reserve(size_t n);

  /**
   * @brief Shrinks the capacity to match the current size.
   * @details This can be used to release unused memory after many pops.
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(n), Space O(n)
   */
  void shrink_to_fit();

  /**
   * @brief Returns the current capacity of the internal array.
   * @return The number of elements that can be stored without reallocation.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto capacity() const noexcept -> size_t { return capacity_; }

private:
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//
  /**
   * @brief Grows the internal array when capacity is reached.
   * @details Typically doubles the capacity.
   * @throws StackOverflowException if capacity would overflow.
   * @complexity Time O(n), Space O(n)
   */
  void grow();

  /**
   * @brief Reallocates the internal array to new_capacity.
   * @param new_capacity The new capacity.
   * @throws std::bad_alloc if memory allocation fails.
   * @complexity Time O(n), Space O(n)
   */
  void reallocate(size_t new_capacity);

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<T[], void (*)(T*)> data_;     ///< The dynamic array holding stack elements.
  size_t                             size_;     ///< The current number of elements.
  size_t                             capacity_; ///< The current capacity of the array.

  static constexpr size_t kGrowthFactor = 2; ///< Growth factor for dynamic resizing.
  static constexpr size_t kMinCapacity  = 8; ///< Minimum capacity to maintain.
};

} // namespace ads::stack

// Include the implementation file for templates.
#include "../../../src/ads/stacks/Array_Stack.tpp"

#endif // ARRAY_STACK_HPP

//===---------------------------------------------------------------------------===//
