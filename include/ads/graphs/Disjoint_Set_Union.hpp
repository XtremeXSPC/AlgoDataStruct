//===---------------------------------------------------------------------------===//
/**
 * @file Disjoint_Set_Union.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the DisjointSetUnion class (Union-Find).
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef DISJOINT_SET_UNION_HPP
#define DISJOINT_SET_UNION_HPP

#include "../arrays/Dynamic_Array.hpp"
#include "Graph_Exception.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

namespace ads::graphs {

using ads::arrays::DynamicArray;

/**
 * @brief Base exception for disjoint set operations.
 */
class DisjointSetException : public GraphError {
public:
  using GraphError::GraphError;
};

/**
 * @brief Disjoint Set Union (Union-Find) data structure.
 *
 * @details Supports efficient union and find operations with path compression
 *          and union by rank optimizations.
 */
class DisjointSetUnion {
public:
  using Element   = size_t;
  using size_type = size_t;

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs a disjoint set with a given number of elements.
   * @param num_elements Number of elements to initialize.
   * @complexity Time O(n), Space O(n)
   */
  explicit DisjointSetUnion(size_type num_elements = 0);

  /**
   * @brief Move constructor.
   * @param other The structure to move from.
   * @complexity Time O(1), Space O(1)
   */
  DisjointSetUnion(DisjointSetUnion&& other) noexcept;

  /**
   * @brief Destructor.
   */
  ~DisjointSetUnion() = default;

  /**
   * @brief Move assignment operator.
   * @param other The structure to move from.
   * @return Reference to this instance.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(DisjointSetUnion&& other) noexcept -> DisjointSetUnion&;

  // Copy constructor and assignment are disabled (move-only type).
  DisjointSetUnion(const DisjointSetUnion&)                    = delete;
  auto operator=(const DisjointSetUnion&) -> DisjointSetUnion& = delete;

  //===----- INITIALIZATION ----------------------------------------------------===//

  /**
   * @brief Resets the structure with a new number of elements.
   * @param num_elements Number of elements to initialize.
   * @complexity Time O(n), Space O(n)
   */
  auto reset(size_type num_elements) -> void;

  /**
   * @brief Adds a new element as a separate set.
   * @return The index of the newly added element.
   * @complexity Time O(1) amortized, Space O(1)
   */
  [[nodiscard]] auto add_element() -> Element;

  //===----- FIND OPERATIONS ---------------------------------------------------===//

  /**
   * @brief Finds the representative of the set containing element (with compression).
   * @param element The element to find.
   * @return The root representative index.
   * @throws DisjointSetException if element is out of range.
   * @complexity Amortized inverse Ackermann.
   */
  [[nodiscard]] auto find(Element element) -> Element;

  /**
   * @brief Finds the representative of the set containing element (const version).
   * @param element The element to find.
   * @return The root representative index.
   * @throws DisjointSetException if element is out of range.
   * @complexity O(log n) worst case: without path compression only the
   *             union-by-rank bound applies.
   */
  [[nodiscard]] auto find(Element element) const -> Element;

  //===----- UNION OPERATIONS --------------------------------------------------===//

  /**
   * @brief Unites the sets containing two elements.
   * @param a First element.
   * @param b Second element.
   * @return true if a merge happened, false if already in same set.
   * @throws DisjointSetException if any element is out of range.
   * @complexity Amortized inverse Ackermann.
   */
  auto union_sets(Element a, Element b) -> bool;

  /**
   * @brief Checks if two elements belong to the same set.
   * @param a First element.
   * @param b Second element.
   * @return true if connected, false otherwise.
   * @throws DisjointSetException if any element is out of range.
   */
  [[nodiscard]] auto connected(Element a, Element b) -> bool;

  /**
   * @brief Checks if two elements belong to the same set (const version).
   * @param a First element.
   * @param b Second element.
   * @return true if connected, false otherwise.
   * @throws DisjointSetException if any element is out of range.
   */
  [[nodiscard]] auto connected(Element a, Element b) const -> bool;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Returns the number of elements.
   * @return The number of elements.
   */
  [[nodiscard]] auto size() const noexcept -> size_type;

  /**
   * @brief Returns the number of disjoint sets.
   * @return The number of sets.
   */
  [[nodiscard]] auto set_count() const noexcept -> size_type;

  /**
   * @brief Returns the number of elements in the set containing an element.
   * @param element The element whose set size is requested.
   * @return The size of the set (component) that element belongs to.
   * @throws DisjointSetException if element is out of range.
   * @complexity Amortized inverse Ackermann.
   */
  [[nodiscard]] auto component_size(Element element) -> size_type;

private:
  auto validate_index(Element element) const -> void;

  DynamicArray<Element>   parent_;
  DynamicArray<size_type> rank_;
  DynamicArray<size_type> set_size_;
  size_type               set_count_ = 0;
};

//===----- INLINE IMPLEMENTATION -----------------------------------------------===//

inline DisjointSetUnion::DisjointSetUnion(size_type num_elements) {
  reset(num_elements);
}

inline DisjointSetUnion::DisjointSetUnion(DisjointSetUnion&& other) noexcept :
    parent_(std::move(other.parent_)),
    rank_(std::move(other.rank_)),
    set_size_(std::move(other.set_size_)),
    set_count_(other.set_count_) {
  other.set_count_ = 0;
}

inline auto DisjointSetUnion::operator=(DisjointSetUnion&& other) noexcept -> DisjointSetUnion& {
  if (this != &other) {
    parent_    = std::move(other.parent_);
    rank_      = std::move(other.rank_);
    set_size_  = std::move(other.set_size_);
    set_count_ = other.set_count_;

    other.set_count_ = 0;
  }
  return *this;
}

inline auto DisjointSetUnion::reset(size_type num_elements) -> void {
  DynamicArray<Element>   new_parent(num_elements, 0);
  DynamicArray<size_type> new_rank(num_elements, 0);
  DynamicArray<size_type> new_size(num_elements, 1);

  for (size_t i = 0; i < num_elements; ++i) {
    new_parent[i] = i;
  }

  parent_    = std::move(new_parent);
  rank_      = std::move(new_rank);
  set_size_  = std::move(new_size);
  set_count_ = num_elements;
}

inline auto DisjointSetUnion::add_element() -> Element {
  const Element index = parent_.size();
  // Reserve every table before changing sizes so allocation failure cannot desynchronize them.
  parent_.reserve(index + 1);
  rank_.reserve(index + 1);
  set_size_.reserve(index + 1);
  parent_.push_back(index);
  rank_.push_back(0);
  set_size_.push_back(1);
  ++set_count_;
  return index;
}

inline auto DisjointSetUnion::find(Element element) -> Element {
  validate_index(element);

  Element root = element;
  while (parent_[root] != root) {
    root = parent_[root];
  }

  while (parent_[element] != element) {
    Element next     = parent_[element];
    parent_[element] = root;
    element          = next;
  }

  return root;
}

inline auto DisjointSetUnion::find(Element element) const -> Element {
  validate_index(element);

  Element root = element;
  while (parent_[root] != root) {
    root = parent_[root];
  }

  return root;
}

inline auto DisjointSetUnion::union_sets(Element a, Element b) -> bool {
  Element root_a = find(a);
  Element root_b = find(b);

  if (root_a == root_b) {
    return false;
  }

  if (rank_[root_a] < rank_[root_b]) {
    std::swap(root_a, root_b);
  }

  parent_[root_b] = root_a;
  set_size_[root_a] += set_size_[root_b];
  if (rank_[root_a] == rank_[root_b]) {
    ++rank_[root_a];
  }

  --set_count_;
  return true;
}

inline auto DisjointSetUnion::connected(Element a, Element b) -> bool {
  return find(a) == find(b);
}

inline auto DisjointSetUnion::connected(Element a, Element b) const -> bool {
  return find(a) == find(b);
}

inline auto DisjointSetUnion::size() const noexcept -> size_type {
  return parent_.size();
}

inline auto DisjointSetUnion::set_count() const noexcept -> size_type {
  return set_count_;
}

inline auto DisjointSetUnion::component_size(Element element) -> size_type {
  return set_size_[find(element)];
}

inline auto DisjointSetUnion::validate_index(Element element) const -> void {
  if (element >= parent_.size()) {
    throw DisjointSetException("Element index out of range");
  }
}

} // namespace ads::graphs

#endif // DISJOINT_SET_UNION_HPP

//===---------------------------------------------------------------------------===//
