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

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ads::graphs {

/**
 * @brief Base exception for disjoint set operations.
 */
class DisjointSetException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

/**
 * @brief Disjoint Set Union (Union-Find) data structure.
 *
 * @details Supports efficient union and find operations with path compression
 *          and union by rank optimizations.
 */
class DisjointSetUnion {
public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs a disjoint set with a given number of elements.
   * @param num_elements Number of elements to initialize.
   * @complexity Time O(n), Space O(n)
   */
  explicit DisjointSetUnion(size_t num_elements = 0);

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

  //===---------------------------- INITIALIZATION -----------------------------===//

  /**
   * @brief Resets the structure with a new number of elements.
   * @param num_elements Number of elements to initialize.
   * @complexity Time O(n), Space O(n)
   */
  auto reset(size_t num_elements) -> void;

  /**
   * @brief Adds a new element as a separate set.
   * @return The index of the newly added element.
   * @complexity Time O(1) amortized, Space O(1)
   */
  [[nodiscard]] auto add_element() -> size_t;

  //===---------------------------- FIND OPERATIONS ----------------------------===//

  /**
   * @brief Finds the representative of the set containing element (with compression).
   * @param element The element to find.
   * @return The root representative index.
   * @throws DisjointSetException if element is out of range.
   * @complexity Amortized inverse Ackermann.
   */
  [[nodiscard]] auto find(size_t element) -> size_t;

  /**
   * @brief Finds the representative of the set containing element (const version).
   * @param element The element to find.
   * @return The root representative index.
   * @throws DisjointSetException if element is out of range.
   * @complexity Amortized inverse Ackermann (no compression).
   */
  [[nodiscard]] auto find(size_t element) const -> size_t;

  //===--------------------------- UNION OPERATIONS ----------------------------===//

  /**
   * @brief Unites the sets containing two elements.
   * @param a First element.
   * @param b Second element.
   * @return true if a merge happened, false if already in same set.
   * @throws DisjointSetException if any element is out of range.
   * @complexity Amortized inverse Ackermann.
   */
  auto union_sets(size_t a, size_t b) -> bool;

  /**
   * @brief Checks if two elements belong to the same set.
   * @param a First element.
   * @param b Second element.
   * @return true if connected, false otherwise.
   * @throws DisjointSetException if any element is out of range.
   */
  [[nodiscard]] auto connected(size_t a, size_t b) -> bool;

  /**
   * @brief Checks if two elements belong to the same set (const version).
   * @param a First element.
   * @param b Second element.
   * @return true if connected, false otherwise.
   * @throws DisjointSetException if any element is out of range.
   */
  [[nodiscard]] auto connected(size_t a, size_t b) const -> bool;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Returns the number of elements.
   * @return The number of elements.
   */
  [[nodiscard]] auto size() const noexcept -> size_t;

  /**
   * @brief Returns the number of disjoint sets.
   * @return The number of sets.
   */
  [[nodiscard]] auto set_count() const noexcept -> size_t;

private:
  auto validate_index(size_t element) const -> void;

  std::vector<size_t> parent_;
  std::vector<size_t> rank_;
  size_t              set_count_ = 0;
};

//===-------------------------- INLINE IMPLEMENTATION --------------------------===//

inline DisjointSetUnion::DisjointSetUnion(size_t num_elements) {
  reset(num_elements);
}

inline DisjointSetUnion::DisjointSetUnion(DisjointSetUnion&& other) noexcept :
    parent_(std::move(other.parent_)),
    rank_(std::move(other.rank_)),
    set_count_(other.set_count_) {
  other.set_count_ = 0;
}

inline auto DisjointSetUnion::operator=(DisjointSetUnion&& other) noexcept -> DisjointSetUnion& {
  if (this != &other) {
    parent_          = std::move(other.parent_);
    rank_            = std::move(other.rank_);
    set_count_       = other.set_count_;
    other.set_count_ = 0;
  }
  return *this;
}

inline auto DisjointSetUnion::reset(size_t num_elements) -> void {
  parent_.resize(num_elements);
  rank_.assign(num_elements, 0);
  std::iota(parent_.begin(), parent_.end(), 0);
  set_count_ = num_elements;
}

inline auto DisjointSetUnion::add_element() -> size_t {
  const size_t index = parent_.size();
  parent_.push_back(index);
  rank_.push_back(0);
  ++set_count_;
  return index;
}

inline auto DisjointSetUnion::find(size_t element) -> size_t {
  validate_index(element);

  size_t root = element;
  while (parent_[root] != root) {
    root = parent_[root];
  }

  while (parent_[element] != element) {
    size_t next      = parent_[element];
    parent_[element] = root;
    element          = next;
  }

  return root;
}

inline auto DisjointSetUnion::find(size_t element) const -> size_t {
  validate_index(element);

  size_t root = element;
  while (parent_[root] != root) {
    root = parent_[root];
  }

  return root;
}

inline auto DisjointSetUnion::union_sets(size_t a, size_t b) -> bool {
  size_t root_a = find(a);
  size_t root_b = find(b);

  if (root_a == root_b) {
    return false;
  }

  if (rank_[root_a] < rank_[root_b]) {
    std::swap(root_a, root_b);
  }

  parent_[root_b] = root_a;
  if (rank_[root_a] == rank_[root_b]) {
    ++rank_[root_a];
  }

  --set_count_;
  return true;
}

inline auto DisjointSetUnion::connected(size_t a, size_t b) -> bool {
  return find(a) == find(b);
}

inline auto DisjointSetUnion::connected(size_t a, size_t b) const -> bool {
  return find(a) == find(b);
}

inline auto DisjointSetUnion::size() const noexcept -> size_t {
  return parent_.size();
}

inline auto DisjointSetUnion::set_count() const noexcept -> size_t {
  return set_count_;
}

inline auto DisjointSetUnion::validate_index(size_t element) const -> void {
  if (element >= parent_.size()) {
    throw DisjointSetException("Element index out of range");
  }
}

} // namespace ads::graphs

#endif // DISJOINT_SET_UNION_HPP

//===---------------------------------------------------------------------------===//
