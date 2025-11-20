//===--------------------------------------------------------------------------===//
/**
 * @file List.hpp
 * @author Costantino Lombardi
 * @brief Abstract interface for a List data structure.
 * @version 0.2
 * @date 2025-06-30
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once
#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>

namespace ads::list {

/**
 * @brief Interface (abstract base class) that defines common operations
 *        for a list data structure.
 *
 * @tparam T The data type to store in the list.
 */
template <typename T>
class List {
public:
  // A virtual destructor is mandatory in polymorphic base classes.
  virtual ~List() = default;

  /**
   * @brief Adds an element to the end of the list (copy).
   * @param value The value to add.
   */
  virtual void push_back(const T& value) = 0;

  /**
   * @brief Adds an element to the end of the list (move).
   * @param value The r-value to move.
   */
  virtual void push_back(T&& value) = 0;

  /**
   * @brief Adds an element to the front of the list (copy).
   * @param value The value to add.
   */
  virtual void push_front(const T& value) = 0;

  /**
   * @brief Adds an element to the front of the list (move).
   * @param value The r-value to move.
   */
  virtual void push_front(T&& value) = 0;

  /**
   * @brief Removes the element at the end of the list.
   * @details Behavior is undefined if the list is empty.
   */
  virtual void pop_back() = 0;

  /**
   * @brief Removes the element at the front of the list.
   * @details Behavior is undefined if the list is empty.
   */
  virtual void pop_front() = 0;

  /**
   * @brief Returns a reference to the front element.
   * @return T& Reference to the first element.
   */
  virtual auto front() -> T& = 0;

  /**
   * @brief Returns a constant reference to the front element.
   * @return const T& Constant reference to the first element.
   */
  virtual auto front() const -> const T& = 0;

  /**
   * @brief Returns a reference to the back element.
   * @return T& Reference to the last element.
   */
  virtual auto back() -> T& = 0;

  /**
   * @brief Returns a constant reference to the back element.
   * @return const T& Constant reference to the last element.
   */
  virtual auto back() const -> const T& = 0;

  /**
   * @brief Checks if the list is empty.
   * @return true if the list contains no elements.
   * @return false otherwise.
   */
  [[nodiscard]] virtual auto is_empty() const noexcept -> bool = 0;

  /**
   * @brief Returns the number of elements in the list.
   * @return size_t The number of elements.
   */
  [[nodiscard]] virtual auto size() const noexcept -> size_t = 0;

  /**
   * @brief Empties the list, removing all elements.
   */
  virtual void clear() = 0;
};

} // namespace ads::list

#endif // LIST_HPP

//===--------------------------------------------------------------------------===//
