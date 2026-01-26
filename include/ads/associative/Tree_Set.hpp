//===---------------------------------------------------------------------------===//
/**
 * @file Tree_Set.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the TreeSet class - an ordered set based on AVL Tree.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef TREE_SET_HPP
#define TREE_SET_HPP

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <vector>

#include "../trees/AVL_Tree.hpp"

namespace ads::associative {

/**
 * @brief An ordered set implemented using an AVL Tree.
 *
 * @details TreeSet stores unique elements in sorted order, providing O(log n)
 *          insert, remove, and lookup operations. Elements are maintained in
 *          order according to the comparison function (default: operator<).
 *
 * @tparam T The type of elements (must be comparable with operator<).
 */
template <typename T>
class TreeSet {
public:
  using value_type = T;
  using size_type  = size_t;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty tree set.
   * @complexity Time O(1), Space O(1)
   */
  TreeSet() = default;

  /**
   * @brief Constructs a tree set from an initializer list.
   * @param values Elements to insert.
   * @complexity Time O(n log n), Space O(n)
   */
  TreeSet(std::initializer_list<T> values);

  /**
   * @brief Move constructor.
   * @complexity Time O(1), Space O(1)
   */
  TreeSet(TreeSet&& other) noexcept = default;

  /**
   * @brief Destructor.
   */
  ~TreeSet() = default;

  /**
   * @brief Move assignment operator.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(TreeSet&& other) noexcept -> TreeSet& = default;

  // Copy operations disabled.
  TreeSet(const TreeSet&)                    = delete;
  auto operator=(const TreeSet&) -> TreeSet& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Inserts an element into the set.
   * @param value The value to insert.
   * @return true if inserted, false if already exists.
   * @complexity Time O(log n), Space O(1)
   */
  auto insert(const T& value) -> bool;

  /**
   * @brief Inserts an element into the set (move).
   * @param value The r-value to move.
   * @return true if inserted, false if already exists.
   * @complexity Time O(log n), Space O(1)
   */
  auto insert(T&& value) -> bool;

  /**
   * @brief Constructs an element in-place.
   * @tparam Args Types of arguments to forward.
   * @param args Arguments to forward to T's constructor.
   * @return true if inserted, false if already exists.
   * @complexity Time O(log n), Space O(1)
   */
  template <typename... Args>
  auto emplace(Args&&... args) -> bool;

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Removes an element from the set.
   * @param value The value to remove.
   * @return true if removed, false if not found.
   * @complexity Time O(log n), Space O(1)
   */
  auto erase(const T& value) -> bool;

  /**
   * @brief Removes all elements from the set.
   * @complexity Time O(n), Space O(1)
   */
  auto clear() noexcept -> void;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Checks if an element exists in the set.
   * @param value The value to search for.
   * @return true if found, false otherwise.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto contains(const T& value) const -> bool;

  /**
   * @brief Checks if the set is empty.
   * @return true if empty, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of elements in the set.
   * @return The current size.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the smallest element in the set.
   * @return Reference to the minimum element.
   * @throws ads::trees::EmptyTreeException if set is empty.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto min() const -> const T&;

  /**
   * @brief Returns the largest element in the set.
   * @return Reference to the maximum element.
   * @throws ads::trees::EmptyTreeException if set is empty.
   * @complexity Time O(log n), Space O(1)
   */
  [[nodiscard]] auto max() const -> const T&;

  //===------------------------- TRAVERSAL OPERATIONS --------------------------===//

  /**
   * @brief Collects all elements in sorted order.
   * @return Vector of elements in ascending order.
   * @complexity Time O(n), Space O(n)
   */
  [[nodiscard]] auto to_vector() const -> std::vector<T>;

  /**
   * @brief Performs an in-order traversal (sorted order).
   * @param visit Function to call for each element.
   * @complexity Time O(n), Space O(h)
   */
  auto for_each(const std::function<void(const T&)>& visit) const -> void;

private:
  //===----------------------------- DATA MEMBERS ------------------------------===//

  ads::trees::AVLTree<T> tree_; ///< Underlying AVL tree.
};

} // namespace ads::associative

// Include the implementation file for templates.
#include "../../../src/ads/associative/Tree_Set.tpp"

#endif // TREE_SET_HPP

//===---------------------------------------------------------------------------===//
