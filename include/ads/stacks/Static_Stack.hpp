//===---------------------------------------------------------------------------===//
/**
 * @file Static_Stack.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the StaticStack class template.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef STATIC_STACK_HPP
#define STATIC_STACK_HPP

#include "Stack.hpp"
#include "Stack_Exception.hpp"

#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace ads::stacks {

/**
 * @brief A fixed-capacity stack with inline storage (bounded LIFO).
 *
 * @details StaticStack stores up to N elements in an inline, stack-allocated buffer
 *          and never touches the heap. It implements the Stack interface and throws
 *          StackOverflowException when a push would exceed the capacity N. It
 *          complements ArrayStack (heap-backed, growable) and LinkedStack (node-based)
 *          with an allocation-free bounded variant, and gives StackOverflowException
 *          its intended purpose. Element lifetimes within the raw storage are managed
 *          manually via placement construction. Move-only, like the other stacks.
 *
 * @tparam T The type of elements stored in the stack.
 * @tparam N The fixed capacity (must be > 0).
 */
template <StackValue T, size_t N>
requires(N > 0)
class StaticStack : public Stack<T> {
public:
  using value_type = T;
  using size_type  = size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty stack (size 0, capacity N).
   * @complexity Time O(1), Space O(1)
   */
  StaticStack() noexcept;

  /**
   * @brief Move constructor. Move-constructs each element into inline storage.
   * @param other The stack to move from (left empty afterwards).
   * @complexity Time O(n), Space O(1)
   */
  StaticStack(StaticStack&& other) noexcept(std::is_nothrow_move_constructible_v<T>);

  /**
   * @brief Destructor. Destroys all live elements.
   * @complexity Time O(n), Space O(1)
   */
  ~StaticStack() override;

  /**
   * @brief Move assignment operator.
   * @param other The stack to move from (left empty afterwards).
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(StaticStack&& other) noexcept(std::is_nothrow_move_constructible_v<T>) -> StaticStack&;

  // Copy constructor and assignment are disabled (move-only type).
  StaticStack(const StaticStack&)                    = delete;
  auto operator=(const StaticStack&) -> StaticStack& = delete;

  //===----- INSERTION OPERATIONS ----------------------------------------------===//

  /**
   * @brief Constructs an element in-place on top of the stack.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   * @throws StackOverflowException if the stack is already full.
   * @complexity Time O(1), Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T& requires EmplaceStackValue<T, Args...>;

  /**
   * @brief Pushes an element onto the top of the stack (copy).
   * @param value The value to push.
   * @throws StackOverflowException if the stack is already full.
   * @complexity Time O(1), Space O(1)
   */
  void push(const T& value) override;

  /**
   * @brief Pushes an element onto the top of the stack (move).
   * @param value The value to push.
   * @throws StackOverflowException if the stack is already full.
   * @complexity Time O(1), Space O(1)
   */
  void push(T&& value) override;

  //===----- REMOVAL OPERATIONS ------------------------------------------------===//

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

  //===----- ACCESS OPERATIONS -------------------------------------------------===//

  /**
   * @brief Returns a reference to the top element.
   * @throws StackUnderflowException if the stack is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() -> T& override;

  /**
   * @brief Returns a const reference to the top element.
   * @throws StackUnderflowException if the stack is empty.
   * @complexity Time O(1), Space O(1)
   */
  auto top() const -> const T& override;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Checks if the stack is empty.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool override;

  /**
   * @brief Checks if the stack is full (size() == N).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_full() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the stack.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t override;

  /**
   * @brief Returns the fixed capacity N.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static constexpr auto capacity() noexcept -> size_t { return N; }

  /**
   * @brief Returns the maximum possible size (equal to capacity N).
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] static constexpr auto max_size() noexcept -> size_t { return N; }

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  /// Returns a typed pointer to the inline storage.
  auto data() noexcept -> T* { return reinterpret_cast<T*>(storage_); }

  /// Returns a typed const pointer to the inline storage.
  auto data() const noexcept -> const T* { return reinterpret_cast<const T*>(storage_); }

  //===----- DATA MEMBERS ------------------------------------------------------===//

  alignas(T) std::byte storage_[sizeof(T) * N]; ///< Raw inline storage for N elements.
  size_t size_;                                 ///< Number of constructed elements.
};

} // namespace ads::stacks

// Include the implementation file for templates.
#include "../../../src/ads/stacks/Static_Stack.tpp"

#endif // STATIC_STACK_HPP

//===---------------------------------------------------------------------------===//
