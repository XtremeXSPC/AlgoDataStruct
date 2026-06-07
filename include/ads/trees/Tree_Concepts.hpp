//===---------------------------------------------------------------------------===//
/**
 * @file Tree_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Defines reusable C++20 concepts for tree containers.
 * @version 0.2
 * @date 2026-02-07
 *
 * @details These concepts form the semantic tree layer built on top of the
 *          generic `ads::support` vocabulary. They describe what it means for
 *          a type to behave well as a tree payload without pushing repetitive
 *          low-level constraints into every tree header.
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef TREE_CONCEPTS_HPP
#define TREE_CONCEPTS_HPP

#include "../../support/Support.hpp"

#include <compare>
#include <concepts>
#include <functional>

namespace ads::trees {

namespace sup = ads::support;

template <typename T> concept TreeElement = sup::NonReferenceDestructible<T>;

template <typename T> concept EqualityComparableTreeElement = TreeElement<T> && sup::EqualityComparable<T>;

template <typename T> concept OrderedTreeElement = EqualityComparableTreeElement<T> && requires(const T& lhs, const T& rhs) {
  { lhs < rhs } -> std::convertible_to<bool>;
};

// clang-format off
template <typename Tree, typename T>
concept OrderedSearchTree = OrderedTreeElement<T> && requires(
  Tree&       tree,
  const Tree& const_tree,
  const T&    value,
  T&&         movable_value,

  const std::function<void(const T&)>& visit) {
    typename Tree::value_type;
    typename Tree::size_type;
    typename Tree::iterator;

    requires std::same_as<typename Tree::value_type, T>;
    requires std::same_as<typename Tree::size_type, size_t>;
    requires std::forward_iterator<typename Tree::iterator>;

    { tree.insert(std::move(movable_value)) } -> std::same_as<bool>;
    { tree.remove(value) } -> std::same_as<bool>;
    { tree.clear() } noexcept -> std::same_as<void>;

    { const_tree.contains(value) } -> std::same_as<bool>;
    { const_tree.find_min() } -> std::same_as<const T&>;
    { const_tree.find_max() } -> std::same_as<const T&>;
    { const_tree.is_empty() } noexcept -> std::same_as<bool>;
    { const_tree.size() } noexcept -> std::same_as<size_t>;
    { const_tree.height() } noexcept -> std::same_as<int>;

    { const_tree.in_order_traversal(visit) } -> std::same_as<void>;
    { const_tree.pre_order_traversal(visit) } -> std::same_as<void>;
    { const_tree.post_order_traversal(visit) } -> std::same_as<void>;
    { const_tree.level_order_traversal(visit) } -> std::same_as<void>;

    { tree.begin() } -> std::same_as<typename Tree::iterator>;
    { tree.end() } -> std::same_as<typename Tree::iterator>;
    { const_tree.begin() } -> std::same_as<typename Tree::iterator>;
    { const_tree.end() } -> std::same_as<typename Tree::iterator>;
    { const_tree.cbegin() } -> std::same_as<typename Tree::iterator>;
    { const_tree.cend() } -> std::same_as<typename Tree::iterator>;
    { *const_tree.begin() } -> std::same_as<const T&>;
  };
//clang-format on

template <typename Tree, typename T>
concept CopyInsertableOrderedSearchTree =
    OrderedSearchTree<Tree, T> && sup::Copyable<T> && requires(Tree& tree, const T& value) {
      { tree.insert(value) } -> std::same_as<bool>;
    };

template <typename Tree, typename T>
concept ValidatableOrderedSearchTree = OrderedSearchTree<Tree, T> && requires(const Tree& tree) {
  { tree.validate_properties() } -> std::same_as<bool>;
};

template <int MinDegree>
concept ValidBTreeDegree = (MinDegree >= 2);

template <typename T>
concept FenwickElement = sup::DefaultInitializable<T> && requires(T a, T b) {
  { T{} };
  { a += b } -> std::same_as<T&>;
  { a - b } -> std::convertible_to<T>;
  { a < b } -> std::convertible_to<bool>;
};

} // namespace ads::trees

#endif // TREE_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
