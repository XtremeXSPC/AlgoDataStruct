//===---------------------------------------------------------------------------===//
/**
 * @file Test_Sorting.cpp
 * @brief Google Test unit tests for sorting algorithms.
 * @version 0.1
 * @date 2026-01-27
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>

#include <functional>
#include <iterator>
#include <vector>

#include "../include/ads/algorithms/Sorting.hpp"
#include "../include/ads/arrays/Dynamic_Array.hpp"
#include "../include/ads/arrays/Static_Array.hpp"

using namespace ads::algorithms;

namespace {

// Helper to convert iterator range to vector for easy comparison.
template <typename Iter>
auto to_vector(Iter first, Iter last) -> std::vector<typename std::iterator_traits<Iter>::value_type> {
  return {first, last};
}

struct StableItem {
  int key = 0;
  int id  = 0;
};

} // namespace

//===---------------------------- BASIC SORT TESTS -----------------------------===//

TEST(SortingAlgorithmsTest, BubbleSortWorks) {
  std::vector<int> data = {5, 1, 4, 2, 8};
  bubble_sort(data.begin(), data.end());

  std::vector<int> expected = {1, 2, 4, 5, 8};
  EXPECT_EQ(data, expected);
}

TEST(SortingAlgorithmsTest, SelectionSortStaticArray) {
  ads::arrays::StaticArray<int, 6> array{9, 3, 5, 1, 8, 2};
  selection_sort(array.begin(), array.end());

  std::vector<int> expected = {1, 2, 3, 5, 8, 9};
  EXPECT_EQ(to_vector(array.begin(), array.end()), expected);
}

TEST(SortingAlgorithmsTest, InsertionSortDynamicArray) {
  ads::arrays::DynamicArray<int> array;
  array.push_back(7);
  array.push_back(3);
  array.push_back(9);
  array.push_back(1);
  array.push_back(5);

  insertion_sort(array.begin(), array.end());

  std::vector<int> expected = {1, 3, 5, 7, 9};
  EXPECT_EQ(to_vector(array.begin(), array.end()), expected);
}

TEST(SortingAlgorithmsTest, ShellSortHandlesNegatives) {
  std::vector<int> data = {3, -1, 0, -5, 2};
  shell_sort(data.begin(), data.end());

  std::vector<int> expected = {-5, -1, 0, 2, 3};
  EXPECT_EQ(data, expected);
}

TEST(SortingAlgorithmsTest, MergeSortIsStable) {
  std::vector<StableItem> items = {{.key = 2, .id = 0}, {.key = 1, .id = 0}, {.key = 2, .id = 1}, {.key = 1, .id = 1}, {.key = 2, .id = 2}};

  merge_sort(items.begin(), items.end(), [](const StableItem& lhs, const StableItem& rhs) { return lhs.key < rhs.key; });

  std::vector<int> keys;
  std::vector<int> ids_for_twos;
  for (const auto& item : items) {
    keys.push_back(item.key);
    if (item.key == 2) {
      ids_for_twos.push_back(item.id);
    }
  }

  std::vector<int> expected_keys     = {1, 1, 2, 2, 2};
  std::vector<int> expected_ids_twos = {0, 1, 2};
  EXPECT_EQ(keys, expected_keys);
  EXPECT_EQ(ids_for_twos, expected_ids_twos);
}

TEST(SortingAlgorithmsTest, QuickSortWithDuplicates) {
  std::vector<int> data = {4, 1, 3, 4, 2, 1, 5};
  quick_sort(data.begin(), data.end());

  std::vector<int> expected = {1, 1, 2, 3, 4, 4, 5};
  EXPECT_EQ(data, expected);
}

TEST(SortingAlgorithmsTest, HeapSortWithCustomComparator) {
  std::vector<int> data = {1, 4, 2, 8, 5};
  heap_sort(data.begin(), data.end(), std::greater<>{});

  std::vector<int> expected = {8, 5, 4, 2, 1};
  EXPECT_EQ(data, expected);
}

//===---------------------------------------------------------------------------===//
