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

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

//===-------------------- SORTING ALGORITHM IMPLEMENTATIONS --------------------===//

namespace ads::algorithms {

namespace detail {

// Insertion sort for small partitions.
constexpr std::size_t kInsertionThreshold = 16;
constexpr std::size_t kRadixBase          = 256;
constexpr std::size_t kRadixBits          = 8;

// Ciura gap sequence for Shell sort (empirically optimal for practical sizes).
constexpr std::array<std::size_t, 8> kCiuraGaps = {701, 301, 132, 57, 23, 10, 4, 1};

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
template <std::forward_iterator Iter, typename Compare, typename Buffer>
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

// Computes floor(log2(n)) for depth limit calculation.
inline auto log2_floor(std::size_t n) -> std::size_t {
  std::size_t result = 0;
  while (n > 1) {
    n >>= 1;
    ++result;
  }
  return result;
}

// Recursive introsort implementation (quick sort with heap sort fallback).
template <std::random_access_iterator Iter, typename Compare>
auto quick_sort_impl(Iter first, Iter last, Compare& comp, std::size_t depth_limit) -> void {
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

    // Fall back to heap sort if recursion is too deep (introsort behavior).
    if (depth_limit == 0) {
      heap_sort(first, last, comp);
      return;
    }
    --depth_limit;

    Iter pivot = detail::partition(first, last, comp);

    const auto left_size  = pivot - first;
    const auto right_size = last - (pivot + 1);

    // Recurse on the smaller side to keep stack depth bounded.
    if (left_size < right_size) {
      quick_sort_impl(first, pivot, comp, depth_limit);
      first = pivot + 1;
    } else {
      quick_sort_impl(pivot + 1, last, comp, depth_limit);
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

// Builds a max-heap from the range.
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

// Merge sort for forward iterators (non-random-access).
template <std::forward_iterator Iter, typename Compare, typename Buffer>
auto merge_sort_forward_impl(Iter first, Iter last, Compare& comp, Buffer& buffer) -> void {
  auto count = std::distance(first, last);
  if (count <= 1) {
    return;
  }

  Iter mid = first;
  std::advance(mid, count / 2);

  merge_sort_forward_impl(first, mid, comp, buffer);
  merge_sort_forward_impl(mid, last, comp, buffer);

  merge_ranges(first, mid, last, comp, buffer);
}

// Returns the number of distinct values in the inclusive range.
template <typename T>
auto range_size(T min_value, T max_value) -> std::size_t {
  // Use std::size_t for arithmetic to avoid overflow with small types.
  using Unsigned       = std::make_unsigned_t<T>;
  const auto umin      = static_cast<std::size_t>(static_cast<Unsigned>(min_value));
  const auto umax      = static_cast<std::size_t>(static_cast<Unsigned>(max_value));
  const std::size_t range = (umax >= umin) ? (umax - umin) : (std::numeric_limits<Unsigned>::max() - umin + umax + 1);
  const std::size_t size  = range + 1;

  if (size == 0 || size <= range) {
    throw std::length_error("counting_sort range too large");
  }

  return size;
}

// Maps a value to its index in the counting array (handles signed types).
template <typename T>
auto to_index(T value, T min_value) -> std::size_t {
  if constexpr (std::is_signed_v<T>) {
    // For signed types, compute the distance from min_value in a way that avoids overflow.
    // Use the wider signed type to avoid issues.
    using WideType = std::conditional_t<sizeof(T) < sizeof(long long), long long, T>;
    return static_cast<std::size_t>(static_cast<WideType>(value) - static_cast<WideType>(min_value));
  } else {
    return static_cast<std::size_t>(value - min_value);
  }
}

// Extracts the radix key, adjusting for signed types.
template <typename T>
auto radix_key(T value) -> std::make_unsigned_t<T> {
  using Unsigned = std::make_unsigned_t<T>;
  auto key       = static_cast<Unsigned>(value);

  if constexpr (std::is_signed_v<T>) {
    // Flip the sign bit so unsigned order matches signed order.
    constexpr Unsigned sign_mask = Unsigned{1} << (sizeof(T) * kRadixBits - 1);
    key ^= sign_mask;
  }

  return key;
}

// Structure to represent a run in Tim sort.
template <std::random_access_iterator Iter>
struct Run {
  Iter        base;
  std::size_t length;
};

// Computes the minimum run length for Tim sort.
inline auto min_run_length(std::size_t n) -> std::size_t {
  std::size_t r = 0;
  while (n >= 64) {
    r |= (n & 1U);
    n >>= 1U;
  }
  return n + r;
}

// Counts a run and makes it ascending.
template <std::random_access_iterator Iter, typename Compare>
auto count_run_and_make_ascending(Iter first, Iter last, Compare& comp) -> std::size_t {
  Iter run_end = first + 1;
  if (run_end == last) {
    return 1;
  }

  if (std::invoke(comp, *run_end, *first)) {
    // Descending run: extend and reverse to make it ascending.
    while (run_end < last && std::invoke(comp, *run_end, *(run_end - 1))) {
      ++run_end;
    }
    std::reverse(first, run_end);
  } else {
    while (run_end < last && !std::invoke(comp, *run_end, *(run_end - 1))) {
      ++run_end;
    }
  }

  return static_cast<std::size_t>(run_end - first);
}

// Merges the runs at index and index + 1.
template <std::random_access_iterator Iter, typename Compare, typename Buffer>
auto merge_at(std::vector<Run<Iter>>& runs, std::size_t index, Compare& comp, Buffer& buffer) -> void {
  Run<Iter> left  = runs[index];
  Run<Iter> right = runs[index + 1];

  Iter mid = left.base + static_cast<std::iter_difference_t<Iter>>(left.length);
  Iter end = mid + static_cast<std::iter_difference_t<Iter>>(right.length);

  merge_ranges(left.base, mid, end, comp, buffer);

  runs[index].length = left.length + right.length;
  runs.erase(runs.begin() + static_cast<std::ptrdiff_t>(index + 1));
}

// Merges runs while maintaining Tim sort invariants.
template <std::random_access_iterator Iter, typename Compare, typename Buffer>
auto merge_collapse(std::vector<Run<Iter>>& runs, Compare& comp, Buffer& buffer) -> void {
  while (runs.size() > 1) {
    const std::size_t n = runs.size();

    if (n >= 3) {
      const std::size_t a = runs[n - 3].length;
      const std::size_t b = runs[n - 2].length;
      const std::size_t c = runs[n - 1].length;

      // Tim sort invariants: A > B + C and B > C must hold.
      if (a <= b + c || b <= c) {
        if (a < c) {
          merge_at(runs, n - 3, comp, buffer);
        } else {
          merge_at(runs, n - 2, comp, buffer);
        }
        continue;
      }
    }

    if (runs[n - 2].length <= runs[n - 1].length) {
      merge_at(runs, n - 2, comp, buffer);
      continue;
    }

    break;
  }
}

// Merges all remaining runs.
template <std::random_access_iterator Iter, typename Compare, typename Buffer>
auto merge_force_collapse(std::vector<Run<Iter>>& runs, Compare& comp, Buffer& buffer) -> void {
  while (runs.size() > 1) {
    const std::size_t n = runs.size();
    if (n >= 3 && runs[n - 3].length < runs[n - 1].length) {
      merge_at(runs, n - 3, comp, buffer);
    } else {
      merge_at(runs, n - 2, comp, buffer);
    }
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
  const auto count = static_cast<std::size_t>(last - first);
  if (count <= 1) {
    return;
  }

  // Use Ciura gap sequence for better performance than Shell's original.
  // For large arrays, extend beyond Ciura with 2.25x multiplier.
  std::size_t gap = 1;
  while (gap < count / 3) {
    gap = static_cast<std::size_t>(static_cast<double>(gap) * 2.25) + 1;
  }

  // Find starting index in Ciura sequence or use computed gap.
  std::size_t ciura_idx = 0;
  while (ciura_idx < detail::kCiuraGaps.size() && detail::kCiuraGaps[ciura_idx] >= count) {
    ++ciura_idx;
  }

  auto next_gap = [&]() -> std::size_t {
    if (ciura_idx < detail::kCiuraGaps.size()) {
      return detail::kCiuraGaps[ciura_idx++];
    }
    gap = gap / 2;
    return gap;
  };

  for (std::size_t g = next_gap(); g >= 1; g = (g == 1) ? 0 : next_gap()) {
    for (std::size_t i = g; i < count; ++i) {
      auto        temp = std::move(*detail::iter_at(first, i));
      std::size_t j    = i;

      // Perform gapped insertion sort.
      while (j >= g && std::invoke(comp, temp, *detail::iter_at(first, j - g))) {
        *detail::iter_at(first, j) = std::move(*detail::iter_at(first, j - g));
        j -= g;
      }

      // Place temp in its correct location.
      *detail::iter_at(first, j) = std::move(temp);
    }

    if (g == 1) {
      break;
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

template <std::forward_iterator Iter, typename Compare>
  requires(!std::random_access_iterator<Iter> && std::sortable<Iter, Compare>)
auto merge_sort(Iter first, Iter last, Compare comp) -> void {
  if (first == last) {
    return;
  }

  using value_type = std::iter_value_t<Iter>;
  std::vector<value_type> buffer;
  buffer.reserve(static_cast<std::size_t>(std::distance(first, last)));

  detail::merge_sort_forward_impl(first, last, comp, buffer);
}

//===------------------------------- QUICK SORT --------------------------------===//

template <std::random_access_iterator Iter, typename Compare>
  requires std::sortable<Iter, Compare>
auto quick_sort(Iter first, Iter last, Compare comp) -> void {
  const auto count = static_cast<std::size_t>(last - first);
  if (count <= 1) {
    return;
  }
  // Use 2 * log2(n) as depth limit (introsort strategy).
  const std::size_t depth_limit = 2 * detail::log2_floor(count);
  detail::quick_sort_impl(first, last, comp, depth_limit);
}

//===-------------------------------- HEAP SORT --------------------------------===//

template <std::random_access_iterator Iter, typename Compare>
  requires std::sortable<Iter, Compare>
auto heap_sort(Iter first, Iter last, Compare comp) -> void {
  const auto count = static_cast<std::size_t>(last - first);
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

//===-------------------------------- TIM SORT ---------------------------------===//

template <std::random_access_iterator Iter, typename Compare>
  requires std::sortable<Iter, Compare>
auto tim_sort(Iter first, Iter last, Compare comp) -> void {
  const std::size_t count = static_cast<std::size_t>(last - first);
  if (count <= 1) {
    return;
  }

  using value_type = std::iter_value_t<Iter>;

  std::vector<value_type> buffer;
  buffer.reserve(count);

  std::vector<detail::Run<Iter>> runs;
  runs.reserve(count / detail::kInsertionThreshold + 1);

  const std::size_t min_run = detail::min_run_length(count);
  Iter              cursor  = first;

  while (cursor != last) {
    const std::size_t remaining = static_cast<std::size_t>(last - cursor);
    std::size_t       run_len   = detail::count_run_and_make_ascending(cursor, last, comp);

    if (run_len < min_run) {
      const std::size_t target = std::min(min_run, remaining);
      insertion_sort(cursor, cursor + static_cast<std::iter_difference_t<Iter>>(target), comp);
      run_len = target;
    }

    runs.push_back({cursor, run_len});
    detail::merge_collapse(runs, comp, buffer);

    cursor += static_cast<std::iter_difference_t<Iter>>(run_len);
  }

  detail::merge_force_collapse(runs, comp, buffer);
}

//===------------------------------ COUNTING SORT -------------------------------===//

template <std::random_access_iterator Iter>
  requires std::integral<std::iter_value_t<Iter>>
auto counting_sort(Iter first, Iter last) -> void {
  if (last - first <= 1) {
    return;
  }

  // Find min and max values.
  auto min_value = *first;
  auto max_value = *first;

  for (Iter it = first + 1; it != last; ++it) {
    if (*it < min_value) {
      min_value = *it;
    }
    if (*it > max_value) {
      max_value = *it;
    }
  }

  counting_sort(first, last, min_value, max_value);
}

template <std::random_access_iterator Iter>
  requires std::integral<std::iter_value_t<Iter>>
auto counting_sort(Iter first, Iter last, std::iter_value_t<Iter> min_value, std::iter_value_t<Iter> max_value) -> void {
  if (last - first <= 1) {
    return;
  }

  if (min_value > max_value) {
    throw std::invalid_argument("counting_sort min_value greater than max_value");
  }

  using value_type = std::iter_value_t<Iter>;

  const std::size_t count      = static_cast<std::size_t>(last - first);
  const std::size_t range_size = detail::range_size(min_value, max_value);

  std::vector<std::size_t> counts(range_size, 0);

  for (std::size_t i = 0; i < count; ++i) {
    const value_type value = *detail::iter_at(first, i);
    ++counts[detail::to_index(value, min_value)];
  }

  for (std::size_t i = 1; i < range_size; ++i) {
    counts[i] += counts[i - 1];
  }

  std::vector<value_type> buffer(count);
  for (std::size_t i = count; i-- > 0;) {
    const value_type  value = *detail::iter_at(first, i);
    const std::size_t index = detail::to_index(value, min_value);
    buffer[--counts[index]] = value;
  }

  for (std::size_t i = 0; i < count; ++i) {
    *detail::iter_at(first, i) = buffer[i];
  }
}

//===------------------------------- RADIX SORT --------------------------------===//

template <std::random_access_iterator Iter>
  requires std::integral<std::iter_value_t<Iter>>
auto radix_sort(Iter first, Iter last) -> void {
  const auto count = static_cast<std::size_t>(last - first);
  if (count <= 1) {
    return;
  }

  using value_type = std::iter_value_t<Iter>;
  using Unsigned   = std::make_unsigned_t<value_type>;

  std::vector<value_type>                     buffer(count);
  std::array<std::size_t, detail::kRadixBase> counts{};

  for (std::size_t pass = 0; pass < sizeof(value_type); ++pass) {
    counts.fill(0);
    const std::size_t shift = pass * detail::kRadixBits;

    for (std::size_t i = 0; i < count; ++i) {
      const Unsigned key    = detail::radix_key(*detail::iter_at(first, i));
      const auto     bucket = static_cast<std::size_t>((key >> shift) & (detail::kRadixBase - 1));
      ++counts[bucket];
    }

    for (std::size_t i = 1; i < counts.size(); ++i) {
      counts[i] += counts[i - 1];
    }

    for (std::size_t i = count; i-- > 0;) {
      const value_type value   = *detail::iter_at(first, i);
      const Unsigned   key     = detail::radix_key(value);
      const auto       bucket  = static_cast<std::size_t>((key >> shift) & (detail::kRadixBase - 1));
      buffer[--counts[bucket]] = value;
    }

    for (std::size_t i = 0; i < count; ++i) {
      *detail::iter_at(first, i) = buffer[i];
    }
  }
}

//===------------------------------- BUCKET SORT -------------------------------===//

template <std::random_access_iterator Iter>
  requires std::floating_point<std::iter_value_t<Iter>>
auto bucket_sort(Iter first, Iter last, std::size_t bucket_count) -> void {
  const auto count = static_cast<std::size_t>(last - first);
  if (count <= 1) {
    return;
  }

  using value_type = std::iter_value_t<Iter>;

  value_type min_value = std::numeric_limits<value_type>::infinity();
  value_type max_value = -std::numeric_limits<value_type>::infinity();

  // Find min and max values, checking for NaN.
  for (Iter it = first; it != last; ++it) {
    if (std::isnan(*it)) {
      throw std::invalid_argument("bucket_sort: NaN values not supported");
    }
    if (*it < min_value) {
      min_value = *it;
    }
    if (*it > max_value) {
      max_value = *it;
    }
  }

  // Handle infinite values by falling back to comparison sort.
  if (std::isinf(min_value) || std::isinf(max_value)) {
    insertion_sort(first, last, std::less<>{});
    return;
  }

  if (min_value == max_value) {
    return;
  }

  if (bucket_count == 0) {
    bucket_count = static_cast<std::size_t>(std::sqrt(static_cast<long double>(count)));
  }

  if (bucket_count <= 1) {
    insertion_sort(first, last, std::less<>{});
    return;
  }

  std::vector<std::vector<value_type>> buckets(bucket_count);
  const long double                    range = static_cast<long double>(max_value) - static_cast<long double>(min_value);

  // Distribute elements into buckets.
  for (std::size_t i = 0; i < count; ++i) {
    const value_type  value      = *detail::iter_at(first, i);
    const long double normalized = (static_cast<long double>(value) - static_cast<long double>(min_value)) / range;
    const long double scaled     = normalized * static_cast<long double>(bucket_count - 1);
    // Clamp to protect against rounding that would place a value past the last bucket.
    const long double clamped = std::clamp(scaled, 0.0L, static_cast<long double>(bucket_count - 1));
    const auto        index   = static_cast<std::size_t>(clamped);
    buckets[index].push_back(value);
  }

  // Sort individual buckets and concatenate.
  Iter out = first;
  for (auto& bucket : buckets) {
    insertion_sort(bucket.begin(), bucket.end(), std::less<>{});
    for (auto& value : bucket) {
      *out = std::move(value);
      ++out;
    }
  }
}

} // namespace ads::algorithms

//===---------------------------------------------------------------------------===//
