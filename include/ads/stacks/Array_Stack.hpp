//===--------------------------------------------------------------------------===//
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
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef ARRAY_STACK_HPP
#define ARRAY_STACK_HPP

#include <algorithm>
#include <cstring>
#include <memory>
#include <new>
#include <utility>

#include "Stack.hpp"
#include "Stack_Exception.hpp"

namespace ads::stack {

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
  //========== CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ==========//

  /**
   * @brief Constructs an empty stack with optional initial capacity.
   * @param initial_capacity The initial capacity (default: 16).
   * @throws std::bad_alloc if memory allocation fails.
   */
  explicit ArrayStack(size_t initial_capacity = 16);

  /** @brief Destructor. */
  ~ArrayStack() override = default;

  // Copy constructor and assignment are disabled (move-only type)
  ArrayStack(const ArrayStack&)            = delete;
  ArrayStack& operator=(const ArrayStack&) = delete;

  /**
   * @brief Move constructor.
   * @param other The stack from which to move resources.
   */
  ArrayStack(ArrayStack&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The stack from which to move resources.
   * @return A reference to this instance.
   */
  ArrayStack& operator=(ArrayStack&& other) noexcept;

  //========== INTERFACE INHERITED FROM Stack<T> ==========//

  void push(const T& value) override;
  void push(T&& value) override;
  void pop() override;

  auto top() -> T& override;
  auto top() const -> const T& override;

  [[nodiscard]] auto is_empty() const noexcept -> bool override;
  [[nodiscard]] auto size() const noexcept -> size_t override;
  void               clear() noexcept override;

  //=========== ADDITIONAL FUNCTIONALITY ==========//

  /**
   * @brief Constructs an element in-place on top of the stack.
   * @tparam Args Types of arguments to forward to the constructor of T.
   * @param args Arguments to forward to the constructor of T.
   * @return Reference to the newly constructed element.
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> T&;

  /**
   * @brief Returns the current capacity of the internal array.
   * @return The number of elements that can be stored without reallocation.
   */
  [[nodiscard]] auto capacity() const noexcept -> size_t { return capacity_; }

  /**
   * @brief Reserves capacity for at least n elements.
   * @param n The minimum capacity to reserve.
   * @throws std::bad_alloc if memory allocation fails.
   */
  void reserve(size_t n);

  /**
   * @brief Shrinks the capacity to match the current size.
   * @details This can be used to release unused memory after many pops.
   * @throws std::bad_alloc if memory allocation fails.
   */
  void shrink_to_fit();

private:
  //================ INTERNAL HELPERS ================//

  /**
   * @brief Grows the internal array when capacity is reached.
   * @details Typically doubles the capacity.
   */
  void grow();

  /**
   * @brief Reallocates the internal array to new_capacity.
   * @param new_capacity The new capacity.
   */
  void reallocate(size_t new_capacity);

  //================ DATA MEMBERS ================//
  std::unique_ptr<T[], void (*)(T*)> data_;     ///< The dynamic array holding stack elements
  size_t                             size_;     ///< The current number of elements
  size_t                             capacity_; ///< The current capacity of the array

  static constexpr size_t kGrowthFactor = 2; ///< Growth factor for dynamic resizing
  static constexpr size_t kMinCapacity  = 8; ///< Minimum capacity to maintain
};

// Include the implementation file for templates
#include "../../../src/ads/stacks/Array_Stack.tpp"

} // namespace ads::stack

#endif // ARRAY_STACK_HPP

//===--------------------------------------------------------------------------===//