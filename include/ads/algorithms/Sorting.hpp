//===---------------------------------------------------------------------------===//
/**
 * @file Sorting.hpp
 * @author Costantino Lombardi
 * @brief Declarations of classic sorting algorithms.
 * @version 0.1
 * @date 2026-01-27
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SORTING_HPP
#define SORTING_HPP

#include <algorithm>
#include <concepts>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace ads::algorithms {

/**
 * @brief Bubble sort for random access iterators.
 *
 * @details Bubble sort repeatedly steps through the range, compares adjacent
 *          elements, and swaps them if they are out of order. Each pass "bubbles"
 *          the largest element to the end of the unsorted portion. The algorithm
 *          stops early if a pass performs no swaps.
 *
 * @tparam Iter Random access iterator type.
 * @tparam Compare Comparator defining the strict weak ordering.
 * @param first Iterator to the first element.
 * @param last Iterator one past the last element.
 * @param comp Comparison function object (default: std::less<>).
 * @complexity Time O(n^2) worst-case, O(n) best-case; Space O(1).
 * @note Stable and in-place.
 */
template <std::random_access_iterator Iter, typename Compare = std::less<>>
  requires std::sortable<Iter, Compare>
auto bubble_sort(Iter first, Iter last, Compare comp = Compare{}) -> void;

/**
 * @brief Selection sort for random access iterators.
 *
 * @details Selection sort divides the range into sorted and unsorted parts.
 *          For each position, it selects the minimum (according to comp) from
 *          the unsorted part and swaps it into place.
 *
 * @tparam Iter Random access iterator type.
 * @tparam Compare Comparator defining the strict weak ordering.
 * @param first Iterator to the first element.
 * @param last Iterator one past the last element.
 * @param comp Comparison function object (default: std::less<>).
 * @complexity Time O(n^2) in all cases; Space O(1).
 * @note Not stable, in-place.
 */
template <std::random_access_iterator Iter, typename Compare = std::less<>>
  requires std::sortable<Iter, Compare>
auto selection_sort(Iter first, Iter last, Compare comp = Compare{}) -> void;

/**
 * @brief Insertion sort for random access iterators.
 *
 * @details Insertion sort builds a sorted prefix one element at a time by
 *          inserting each new element into its correct position in the prefix.
 *          Efficient for small ranges and nearly sorted data.
 *
 * @tparam Iter Random access iterator type.
 * @tparam Compare Comparator defining the strict weak ordering.
 * @param first Iterator to the first element.
 * @param last Iterator one past the last element.
 * @param comp Comparison function object (default: std::less<>).
 * @complexity Time O(n^2) worst-case, O(n) best-case; Space O(1).
 * @note Stable and in-place.
 */
template <std::random_access_iterator Iter, typename Compare = std::less<>>
  requires std::sortable<Iter, Compare>
auto insertion_sort(Iter first, Iter last, Compare comp = Compare{}) -> void;

/**
 * @brief Shell sort for random access iterators.
 *
 * @details Shell sort performs insertion sort over elements separated by a gap
 *          sequence, reducing the gap until it reaches 1. This accelerates
 *          ordering of distant elements and improves performance over plain
 *          insertion sort on larger ranges.
 *
 * @tparam Iter Random access iterator type.
 * @tparam Compare Comparator defining the strict weak ordering.
 * @param first Iterator to the first element.
 * @param last Iterator one past the last element.
 * @param comp Comparison function object (default: std::less<>).
 * @complexity Time O(n^2) worst-case (gap-dependent), typically faster in practice;
 *             Space O(1).
 * @note Not stable, in-place.
 */
template <std::random_access_iterator Iter, typename Compare = std::less<>>
  requires std::sortable<Iter, Compare>
auto shell_sort(Iter first, Iter last, Compare comp = Compare{}) -> void;

/**
 * @brief Merge sort for random access iterators.
 *
 * @details Merge sort recursively divides the range into halves, sorts each half,
 *          and merges the sorted halves into a single sorted range. It guarantees
 *          stable ordering and O(n log n) time complexity.
 *
 * @tparam Iter Random access iterator type.
 * @tparam Compare Comparator defining the strict weak ordering.
 * @param first Iterator to the first element.
 * @param last Iterator one past the last element.
 * @param comp Comparison function object (default: std::less<>).
 * @complexity Time O(n log n) in all cases; Space O(n).
 * @note Stable but not in-place due to the auxiliary buffer.
 */
template <std::random_access_iterator Iter, typename Compare = std::less<>>
  requires std::sortable<Iter, Compare>
auto merge_sort(Iter first, Iter last, Compare comp = Compare{}) -> void;

/**
 * @brief Quick sort for random access iterators.
 *
 * @details Quick sort partitions the range around a pivot so that elements less
 *          than the pivot precede it and elements greater-or-equal follow it.
 *          This implementation uses median-of-three pivot selection and falls
 *          back to insertion sort for small partitions to reduce overhead.
 *
 * @tparam Iter Random access iterator type.
 * @tparam Compare Comparator defining the strict weak ordering.
 * @param first Iterator to the first element.
 * @param last Iterator one past the last element.
 * @param comp Comparison function object (default: std::less<>).
 * @complexity Average time O(n log n), worst-case O(n^2); Space O(log n) stack.
 * @note Not stable, in-place.
 */
template <std::random_access_iterator Iter, typename Compare = std::less<>>
  requires std::sortable<Iter, Compare>
auto quick_sort(Iter first, Iter last, Compare comp = Compare{}) -> void;

/**
 * @brief Heap sort for random access iterators.
 *
 * @details Heap sort builds a heap in-place and repeatedly extracts the root
 *          to the end of the range. It guarantees O(n log n) time complexity
 *          without extra memory allocations.
 *
 * @tparam Iter Random access iterator type.
 * @tparam Compare Comparator defining the strict weak ordering.
 * @param first Iterator to the first element.
 * @param last Iterator one past the last element.
 * @param comp Comparison function object (default: std::less<>).
 * @complexity Time O(n log n) in all cases; Space O(1).
 * @note Not stable, in-place.
 */
template <std::random_access_iterator Iter, typename Compare = std::less<>>
  requires std::sortable<Iter, Compare>
auto heap_sort(Iter first, Iter last, Compare comp = Compare{}) -> void;

} // namespace ads::algorithms

// Include the implementation file for templates.
#include "../../../src/ads/algorithms/Sorting.tpp"

#endif // SORTING_HPP

//===---------------------------------------------------------------------------===//
