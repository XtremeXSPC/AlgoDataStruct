//===---------------------------------------------------------------------------===//
/**
 * @file Sorting.tpp
 * @author Costantino Lombardi
 * @brief Implementation of classic sorting algorithms.
 * @version 0.1
 * @date 2026-01-27
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/algorithms/Sorting.hpp"

#include <cstddef>
#include <vector>

//===-------------------- SORTING ALGORITHM IMPLEMENTATIONS --------------------===//

namespace ads::algorithms {

namespace detail {

// Insertion sort used as a subroutine for small partitions.
constexpr std::size_t kInsertionThreshold = 16;

// Helper to get iterator at index.
template <std::random_access_iterator Iter>
auto iter_at(Iter first, std::size_t index) -> Iter {
  using diff_t = std::iter_difference_t<Iter>;
  return first + static_cast<diff_t>(index);
}

// Median-of-three pivot selection.
template <std::random_access_iterator Iter, typename Compare>
auto median_of_three(Iter a, Iter b, Iter c, Compare& comp) -> Iter {
  // Select the median element to reduce the chance of worst-case partitions.
  if (std::invoke(comp, *a, *b)) {
    if (std::invoke(comp, *b, *c)) {
      return b;
    }
    if (std::invoke(comp, *a, *c)) {
      return c;
    }
    return a;
  }

  if (std::invoke(comp, *a, *c)) {
    return a;
  }
  if (std::invoke(comp, *b, *c)) {
    return c;
  }
  return b;
}

// Chooses and places pivot at the end for partitioning.
template <std::random_access_iterator Iter, typename Compare>
auto choose_pivot(Iter first, Iter last, Compare& comp) -> Iter {
  // Use median-of-three and place pivot at the end for partitioning.
  Iter       pivot_it = last - 1;
  const auto count    = last - first;
  if (count >= 3) {
    Iter mid   = first + count / 2;
    Iter pivot = median_of_three(first, mid, pivot_it, comp);
    if (pivot != pivot_it) {
      std::iter_swap(pivot, pivot_it);
    }
  }
  return pivot_it;
}

// Partitions the range around the pivot.
template <std::random_access_iterator Iter, typename Compare>
auto partition(Iter first, Iter last, Compare& comp) -> Iter {
  Iter pivot_it = choose_pivot(first, last, comp);
  Iter store    = first;

  // Partition elements strictly less than pivot to the front.
  for (Iter it = first; it < pivot_it; ++it) {
    if (std::invoke(comp, *it, *pivot_it)) {
      std::iter_swap(it, store);
      ++store;
    }
  }

  std::iter_swap(store, pivot_it);
  return store;
}

// Merges two sorted ranges into one.
template <std::random_access_iterator Iter, typename Compare, typename Buffer>
auto merge_ranges(Iter first, Iter mid, Iter last, Compare& comp, Buffer& buffer) -> void {
  buffer.clear();

  Iter left  = first;
  Iter right = mid;

  // Merge while preserving stability: left element wins on equality.
  while (left != mid && right != last) {
    if (std::invoke(comp, *right, *left)) {
      buffer.emplace_back(std::move(*right));
      ++right;
    } else {
      buffer.emplace_back(std::move(*left));
      ++left;
    }
  }

  while (left != mid) {
    buffer.emplace_back(std::move(*left));
    ++left;
  }

  while (right != last) {
    buffer.emplace_back(std::move(*right));
    ++right;
  }

  Iter out = first;
  for (auto& value : buffer) {
    *out = std::move(value);
    ++out;
  }
}

// Recursive merge sort implementation.
template <std::random_access_iterator Iter, typename Compare, typename Buffer>
auto merge_sort_impl(Iter first, Iter last, Compare& comp, Buffer& buffer) -> void {
  const auto count = last - first;
  if (count <= 1) {
    return;
  }

  if (static_cast<std::size_t>(count) <= kInsertionThreshold) {
    // Small partitions are faster with insertion sort.
    insertion_sort(first, last, comp);
    return;
  }

  Iter mid = first + count / 2;
  merge_sort_impl(first, mid, comp, buffer);
  merge_sort_impl(mid, last, comp, buffer);

  // Skip merge if already in order.
  if (!std::invoke(comp, *mid, *(mid - 1))) {
    return;
  }

  merge_ranges(first, mid, last, comp, buffer);
}

// Recursive quick sort implementation.
template <std::random_access_iterator Iter, typename Compare>
auto quick_sort_impl(Iter first, Iter last, Compare& comp) -> void {
  while (true) {
    const auto count = last - first;
    if (count <= 1) {
      return;
    }

    // For small ranges, insertion sort reduces overhead and recursion.
    if (static_cast<std::size_t>(count) <= kInsertionThreshold) {
      insertion_sort(first, last, comp);
      return;
    }

    Iter pivot = detail::partition(first, last, comp);

    const auto left_size  = pivot - first;
    const auto right_size = last - (pivot + 1);

    // Recurse on the smaller side to keep stack depth bounded.
    if (left_size < right_size) {
      quick_sort_impl(first, pivot, comp);
      first = pivot + 1;
    } else {
      quick_sort_impl(pivot + 1, last, comp);
      last = pivot;
    }
  }
}

// Sifts down the element at 'start' to maintain the heap property.
template <std::random_access_iterator Iter, typename Compare>
auto sift_down(Iter first, std::size_t start, std::size_t end, Compare& comp) -> void {
  std::size_t root = start;

  while (true) {
    std::size_t child = root * 2 + 1;
    if (child >= end) {
      return;
    }

    // Select the child that should be swapped with the root.
    std::size_t swap_idx = root;
    if (std::invoke(comp, *iter_at(first, swap_idx), *iter_at(first, child))) {
      swap_idx = child;
    }
    if (child + 1 < end && std::invoke(comp, *iter_at(first, swap_idx), *iter_at(first, child + 1))) {
      swap_idx = child + 1;
    }

    if (swap_idx == root) {
      return;
    }

    std::iter_swap(iter_at(first, root), iter_at(first, swap_idx));
    root = swap_idx;
  }
}

template <std::random_access_iterator Iter, typename Compare>
auto build_heap(Iter first, std::size_t count, Compare& comp) -> void {
  if (count < 2) {
    return;
  }

  // Start from the last parent and sift down to build the heap.
  for (std::size_t start = (count / 2); start > 0; --start) {
    sift_down(first, start - 1, count, comp);
  }
}

} // namespace detail

//===------------------------------- BUBBLE SORT -------------------------------===//

template <std::random_access_iterator Iter, typename Compare>
  requires std::sortable<Iter, Compare>
auto bubble_sort(Iter first, Iter last, Compare comp) -> void {
  // Get the number of elements.
  auto count = last - first;
  if (count <= 1) {
    return;
  }

  // Perform passes until no swaps occur.
  for (std::iter_difference_t<Iter> pass = 0; pass + 1 < count; ++pass) {
    bool swapped = false;
    Iter end_it  = last - pass;

    // Perform a pass bubbling the largest element to the end.
    for (Iter it = first; it + 1 < end_it; ++it) {
      Iter next = it + 1;
      if (std::invoke(comp, *next, *it)) {
        std::iter_swap(it, next);
        swapped = true;
      }
    }

    // Early exit if no swaps occurred.
    if (!swapped) {
      break;
    }
  }
}

//===----------------------------- SELECTION SORT ------------------------------===//

template <std::random_access_iterator Iter, typename Compare>
  requires std::sortable<Iter, Compare>
auto selection_sort(Iter first, Iter last, Compare comp) -> void {
  // Get the number of elements.
  for (Iter it = first; it != last; ++it) {
    Iter best = it;
    // Find the minimum element in the unsorted portion.
    for (Iter jt = it + 1; jt != last; ++jt) {
      if (std::invoke(comp, *jt, *best)) {
        best = jt;
      }
    }
    // Swap the found minimum element with the current element.
    if (best != it) {
      std::iter_swap(it, best);
    }
  }
}

//===----------------------------- INSERTION SORT ------------------------------===//

template <std::random_access_iterator Iter, typename Compare>
  requires std::sortable<Iter, Compare>
auto insertion_sort(Iter first, Iter last, Compare comp) -> void {
  // Check for trivial case.
  if (last - first <= 1) {
    return;
  }

  // Build the sorted portion one element at a time.
  for (Iter it = first + 1; it != last; ++it) {
    auto value = std::move(*it);
    Iter hole  = it;

    // Shift larger elements to the right.
    while (hole > first && std::invoke(comp, value, *(hole - 1))) {
      *hole = std::move(*(hole - 1));
      --hole;
    }

    *hole = std::move(value);
  }
}

//===------------------------------- SHELL SORT --------------------------------===//

template <std::random_access_iterator Iter, typename Compare>
  requires std::sortable<Iter, Compare>
auto shell_sort(Iter first, Iter last, Compare comp) -> void {
  const std::size_t count = static_cast<std::size_t>(last - first);
  if (count <= 1) {
    return;
  }

  // Use Shell's original gap sequence: N/2, N/4, ..., 1.
  for (std::size_t gap = count / 2; gap > 0; gap /= 2) {
    for (std::size_t i = gap; i < count; ++i) {
      auto        temp = std::move(*detail::iter_at(first, i));
      std::size_t j    = i;

      // Perform gapped insertion sort.
      while (j >= gap && std::invoke(comp, temp, *detail::iter_at(first, j - gap))) {
        *detail::iter_at(first, j) = std::move(*detail::iter_at(first, j - gap));
        j -= gap;
      }

      // Place temp in its correct location.
      *detail::iter_at(first, j) = std::move(temp);
    }
  }
}

//===------------------------------- MERGE SORT --------------------------------===//

template <std::random_access_iterator Iter, typename Compare>
  requires std::sortable<Iter, Compare>
auto merge_sort(Iter first, Iter last, Compare comp) -> void {
  const auto count = last - first;
  if (count <= 1) {
    return;
  }

  // Allocate auxiliary buffer for merging.
  using value_type = std::iter_value_t<Iter>;
  std::vector<value_type> buffer;
  buffer.reserve(static_cast<std::size_t>(count));

  // Start the recursive merge sort.
  detail::merge_sort_impl(first, last, comp, buffer);
}

//===------------------------------- QUICK SORT --------------------------------===//

template <std::random_access_iterator Iter, typename Compare>
  requires std::sortable<Iter, Compare>
auto quick_sort(Iter first, Iter last, Compare comp) -> void {
  detail::quick_sort_impl(first, last, comp);
}

//===-------------------------------- HEAP SORT --------------------------------===//

template <std::random_access_iterator Iter, typename Compare>
  requires std::sortable<Iter, Compare>
auto heap_sort(Iter first, Iter last, Compare comp) -> void {
  const std::size_t count = static_cast<std::size_t>(last - first);
  if (count <= 1) {
    return;
  }

  // Build the initial heap.
  detail::build_heap(first, count, comp);

  // Repeatedly extract the maximum element from the heap.
  for (std::size_t end = count; end > 1; --end) {
    std::iter_swap(first, detail::iter_at(first, end - 1));
    detail::sift_down(first, 0, end - 1, comp);
  }
}

} // namespace ads::algorithms

//===---------------------------------------------------------------------------===//
