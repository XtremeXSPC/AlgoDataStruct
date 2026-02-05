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

#include "../include/ads/algorithms/Sorting.hpp"
#include "../include/ads/arrays/Dynamic_Array.hpp"
#include "../include/ads/arrays/Static_Array.hpp"
#include "../include/ads/lists/Singly_Linked_List.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <concepts>
#include <functional>
#include <iterator>
#include <numbers>
#include <vector>

using namespace ads::algorithms;
using namespace ads::lists;

namespace {

// Helper to convert iterator range to vector for easy comparison.
template <std::input_iterator Iter>
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
  std::vector<StableItem> items = {
      {.key = 2, .id = 0}, {.key = 1, .id = 0}, {.key = 2, .id = 1}, {.key = 1, .id = 1}, {.key = 2, .id = 2}};

  merge_sort(items.begin(), items.end(),
             [](const StableItem& lhs, const StableItem& rhs) { return lhs.key < rhs.key; });

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

TEST(SortingAlgorithmsTest, MergeSortForwardIterator) {
  SinglyLinkedList<int> list;
  list.push_back(4);
  list.push_back(1);
  list.push_back(3);
  list.push_back(2);

  merge_sort(list.begin(), list.end());

  std::vector<int> expected = {1, 2, 3, 4};
  EXPECT_EQ(to_vector(list.begin(), list.end()), expected);
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

TEST(SortingAlgorithmsTest, TimSortIsStable) {
  std::vector<StableItem> items = {
      {.key = 3, .id = 0}, {.key = 2, .id = 0}, {.key = 3, .id = 1}, {.key = 1, .id = 0}, {.key = 2, .id = 1}};

  tim_sort(items.begin(), items.end(), [](const StableItem& lhs, const StableItem& rhs) { return lhs.key < rhs.key; });

  std::vector<int> keys;
  std::vector<int> ids_for_threes;
  for (const auto& item : items) {
    keys.push_back(item.key);
    if (item.key == 3) {
      ids_for_threes.push_back(item.id);
    }
  }

  std::vector<int> expected_keys       = {1, 2, 2, 3, 3};
  std::vector<int> expected_ids_threes = {0, 1};
  EXPECT_EQ(keys, expected_keys);
  EXPECT_EQ(ids_for_threes, expected_ids_threes);
}

//===----------------------- COUNTING & RADIX SORT TESTS -----------------------===//

TEST(SortingAlgorithmsTest, CountingSortHandlesNegatives) {
  std::vector<int> data = {4, -1, 3, -2, 0, -1};
  counting_sort(data.begin(), data.end());

  std::vector<int> expected = {-2, -1, -1, 0, 3, 4};
  EXPECT_EQ(data, expected);
}

TEST(SortingAlgorithmsTest, CountingSortWithExplicitRange) {
  std::vector<int> data = {5, 2, 7, 2, 3, 6};
  counting_sort(data.begin(), data.end(), 2, 7);

  std::vector<int> expected = {2, 2, 3, 5, 6, 7};
  EXPECT_EQ(data, expected);
}

TEST(SortingAlgorithmsTest, RadixSortSignedIntegers) {
  std::vector<int> data = {170, 45, -75, 90, -802, 24, 2, 66};
  radix_sort(data.begin(), data.end());

  std::vector<int> expected = {-802, -75, 2, 24, 45, 66, 90, 170};
  EXPECT_EQ(data, expected);
}

TEST(SortingAlgorithmsTest, BucketSortFloatingPoints) {
  std::vector<double> data = {0.78, 0.17, 0.39, 0.26, 0.72, 0.94, 0.21, 0.12, 0.23, 0.68};
  bucket_sort(data.begin(), data.end());

  std::vector<double> expected = {0.12, 0.17, 0.21, 0.23, 0.26, 0.39, 0.68, 0.72, 0.78, 0.94};
  EXPECT_EQ(data, expected);
}

//===----------------------------- EDGE CASE TESTS -----------------------------===//

TEST(SortingEdgeCasesTest, EmptyRange) {
  std::vector<int> data;

  bubble_sort(data.begin(), data.end());
  EXPECT_TRUE(data.empty());

  selection_sort(data.begin(), data.end());
  EXPECT_TRUE(data.empty());

  insertion_sort(data.begin(), data.end());
  EXPECT_TRUE(data.empty());

  shell_sort(data.begin(), data.end());
  EXPECT_TRUE(data.empty());

  merge_sort(data.begin(), data.end());
  EXPECT_TRUE(data.empty());

  quick_sort(data.begin(), data.end());
  EXPECT_TRUE(data.empty());

  heap_sort(data.begin(), data.end());
  EXPECT_TRUE(data.empty());

  tim_sort(data.begin(), data.end());
  EXPECT_TRUE(data.empty());

  counting_sort(data.begin(), data.end());
  EXPECT_TRUE(data.empty());

  radix_sort(data.begin(), data.end());
  EXPECT_TRUE(data.empty());

  std::vector<double> float_data;
  bucket_sort(float_data.begin(), float_data.end());
  EXPECT_TRUE(float_data.empty());
}

TEST(SortingEdgeCasesTest, SingleElement) {
  auto test_single = [](auto sort_fn) {
    std::vector<int> data = {42};
    sort_fn(data.begin(), data.end());
    EXPECT_EQ(data, std::vector<int>{42});
  };

  test_single([](auto f, auto l) { bubble_sort(f, l); });
  test_single([](auto f, auto l) { selection_sort(f, l); });
  test_single([](auto f, auto l) { insertion_sort(f, l); });
  test_single([](auto f, auto l) { shell_sort(f, l); });
  test_single([](auto f, auto l) { merge_sort(f, l); });
  test_single([](auto f, auto l) { quick_sort(f, l); });
  test_single([](auto f, auto l) { heap_sort(f, l); });
  test_single([](auto f, auto l) { tim_sort(f, l); });
  test_single([](auto f, auto l) { counting_sort(f, l); });
  test_single([](auto f, auto l) { radix_sort(f, l); });

  std::vector<double> float_data = {std::numbers::pi};
  bucket_sort(float_data.begin(), float_data.end());
  EXPECT_EQ(float_data, std::vector<double>{std::numbers::pi});
}

TEST(SortingEdgeCasesTest, AlreadySorted) {
  std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto test_sorted = [&expected](auto sort_fn) {
    std::vector<int> data = expected;
    sort_fn(data.begin(), data.end());
    EXPECT_EQ(data, expected);
  };

  test_sorted([](auto f, auto l) { bubble_sort(f, l); });
  test_sorted([](auto f, auto l) { selection_sort(f, l); });
  test_sorted([](auto f, auto l) { insertion_sort(f, l); });
  test_sorted([](auto f, auto l) { shell_sort(f, l); });
  test_sorted([](auto f, auto l) { merge_sort(f, l); });
  test_sorted([](auto f, auto l) { quick_sort(f, l); });
  test_sorted([](auto f, auto l) { heap_sort(f, l); });
  test_sorted([](auto f, auto l) { tim_sort(f, l); });
  test_sorted([](auto f, auto l) { counting_sort(f, l); });
  test_sorted([](auto f, auto l) { radix_sort(f, l); });
}

TEST(SortingEdgeCasesTest, ReverseSorted) {
  std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto test_reverse = [&expected](auto sort_fn) {
    std::vector<int> data = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    sort_fn(data.begin(), data.end());
    EXPECT_EQ(data, expected);
  };

  test_reverse([](auto f, auto l) { bubble_sort(f, l); });
  test_reverse([](auto f, auto l) { selection_sort(f, l); });
  test_reverse([](auto f, auto l) { insertion_sort(f, l); });
  test_reverse([](auto f, auto l) { shell_sort(f, l); });
  test_reverse([](auto f, auto l) { merge_sort(f, l); });
  test_reverse([](auto f, auto l) { quick_sort(f, l); });
  test_reverse([](auto f, auto l) { heap_sort(f, l); });
  test_reverse([](auto f, auto l) { tim_sort(f, l); });
  test_reverse([](auto f, auto l) { counting_sort(f, l); });
  test_reverse([](auto f, auto l) { radix_sort(f, l); });
}

TEST(SortingEdgeCasesTest, AllEqualElements) {
  std::vector<int> expected = {5, 5, 5, 5, 5, 5, 5, 5};

  auto test_equal = [&expected](auto sort_fn) {
    std::vector<int> data = expected;
    sort_fn(data.begin(), data.end());
    EXPECT_EQ(data, expected);
  };

  test_equal([](auto f, auto l) { bubble_sort(f, l); });
  test_equal([](auto f, auto l) { selection_sort(f, l); });
  test_equal([](auto f, auto l) { insertion_sort(f, l); });
  test_equal([](auto f, auto l) { shell_sort(f, l); });
  test_equal([](auto f, auto l) { merge_sort(f, l); });
  test_equal([](auto f, auto l) { quick_sort(f, l); });
  test_equal([](auto f, auto l) { heap_sort(f, l); });
  test_equal([](auto f, auto l) { tim_sort(f, l); });
  test_equal([](auto f, auto l) { counting_sort(f, l); });
  test_equal([](auto f, auto l) { radix_sort(f, l); });

  std::vector<double> float_data = {2.5, 2.5, 2.5, 2.5};
  bucket_sort(float_data.begin(), float_data.end());
  EXPECT_EQ(float_data, (std::vector<double>{2.5, 2.5, 2.5, 2.5}));
}

//===----------------------- INTEGER TYPE VARIANT TESTS ------------------------===//

TEST(SortingIntegerTypesTest, CountingSortWithChar) {
  std::vector<char> data = {'z', 'a', 'm', 'b', 'y'};
  counting_sort(data.begin(), data.end());

  std::vector<char> expected = {'a', 'b', 'm', 'y', 'z'};
  EXPECT_EQ(data, expected);
}

TEST(SortingIntegerTypesTest, CountingSortWithShort) {
  std::vector<short> data = {1'000, -500, 0, 32'000, -32'000};
  counting_sort(data.begin(), data.end());

  std::vector<short> expected = {-32'000, -500, 0, 1'000, 32'000};
  EXPECT_EQ(data, expected);
}

TEST(SortingIntegerTypesTest, CountingSortWithInt8) {
  std::vector<int8_t> data = {127, -128, 0, 50, -50};
  counting_sort(data.begin(), data.end());

  std::vector<int8_t> expected = {-128, -50, 0, 50, 127};
  EXPECT_EQ(data, expected);
}

TEST(SortingIntegerTypesTest, RadixSortWithChar) {
  std::vector<char> data = {'z', 'a', 'm', 'b', 'y'};
  radix_sort(data.begin(), data.end());

  std::vector<char> expected = {'a', 'b', 'm', 'y', 'z'};
  EXPECT_EQ(data, expected);
}

TEST(SortingIntegerTypesTest, RadixSortWithShort) {
  std::vector<short> data = {1'000, -500, 0, 32'000, -32'000};
  radix_sort(data.begin(), data.end());

  std::vector<short> expected = {-32'000, -500, 0, 1'000, 32'000};
  EXPECT_EQ(data, expected);
}

TEST(SortingIntegerTypesTest, RadixSortWithUnsignedTypes) {
  std::vector<unsigned char> uchar_data = {255, 0, 128, 64, 192};
  radix_sort(uchar_data.begin(), uchar_data.end());
  EXPECT_EQ(uchar_data, (std::vector<unsigned char>{0, 64, 128, 192, 255}));

  std::vector<unsigned short> ushort_data = {65'535, 0, 32'768, 1'000};
  radix_sort(ushort_data.begin(), ushort_data.end());
  EXPECT_EQ(ushort_data, (std::vector<unsigned short>{0, 1'000, 32'768, 65'535}));
}

//===----------------------- BUCKET SORT SPECIAL VALUES ------------------------===//

TEST(SortingBucketSortTest, ThrowsOnNaN) {
  std::vector<double> data = {1.0, std::numeric_limits<double>::quiet_NaN(), 2.0};
  EXPECT_THROW(bucket_sort(data.begin(), data.end()), std::invalid_argument);
}

TEST(SortingBucketSortTest, HandlesInfinity) {
  std::vector<double> data = {3.0, std::numeric_limits<double>::infinity(), 1.0,
                              -std::numeric_limits<double>::infinity(), 2.0};
  bucket_sort(data.begin(), data.end());

  EXPECT_EQ(data[0], -std::numeric_limits<double>::infinity());
  EXPECT_EQ(data[1], 1.0);
  EXPECT_EQ(data[2], 2.0);
  EXPECT_EQ(data[3], 3.0);
  EXPECT_EQ(data[4], std::numeric_limits<double>::infinity());
}

TEST(SortingBucketSortTest, NegativeFloats) {
  std::vector<float> data = {-1.5f, 2.3f, -0.1f, 0.0f, -3.7f, 1.2f};
  bucket_sort(data.begin(), data.end());

  std::vector<float> expected = {-3.7f, -1.5f, -0.1f, 0.0f, 1.2f, 2.3f};
  EXPECT_EQ(data, expected);
}

//===---------------------- COUNTING SORT EXCEPTION TESTS ----------------------===//

TEST(SortingCountingSortTest, ThrowsOnInvalidRange) {
  std::vector<int> data = {1, 2, 3};
  EXPECT_THROW(counting_sort(data.begin(), data.end(), 10, 5), std::invalid_argument);
}

TEST(SortingCountingSortTest, FullInt8RangeWorks) {
  // Full int8_t range [-128, 127] should work correctly (256 values).
  std::vector<int8_t> data = {127, -128, 0, 50, -50};
  counting_sort(data.begin(), data.end(), static_cast<int8_t>(-128), static_cast<int8_t>(127));

  std::vector<int8_t> expected = {-128, -50, 0, 50, 127};
  EXPECT_EQ(data, expected);
}

//===---------------------------- LARGE DATA TESTS -----------------------------===//

TEST(SortingLargeDataTest, QuickSortLargeReversed) {
  const std::size_t size = 10'000;
  std::vector<int>  data(size);
  std::vector<int>  expected(size);
  for (std::size_t i = 0; i < size; ++i) {
    data[i]     = static_cast<int>(size - i);
    expected[i] = static_cast<int>(i + 1);
  }

  quick_sort(data.begin(), data.end());
  EXPECT_EQ(data, expected);
}

TEST(SortingLargeDataTest, TimSortNearlySorted) {
  const std::size_t size = 1'000;
  std::vector<int>  data(size);
  for (std::size_t i = 0; i < size; ++i) {
    data[i] = static_cast<int>(i);
  }
  // Introduce a few swaps to make it "nearly sorted".
  std::swap(data[10], data[20]);
  std::swap(data[100], data[200]);
  std::swap(data[500], data[600]);

  tim_sort(data.begin(), data.end());

  for (std::size_t i = 0; i < size; ++i) {
    EXPECT_EQ(data[i], static_cast<int>(i));
  }
}

TEST(SortingLargeDataTest, MergeSortRandomData) {
  const std::size_t size = 5'000;
  std::vector<int>  data(size);
  for (std::size_t i = 0; i < size; ++i) {
    data[i] = static_cast<int>((i * 31'337) % 10'000);
  }

  std::vector<int> expected = data;
  std::ranges::sort(expected);

  merge_sort(data.begin(), data.end());
  EXPECT_EQ(data, expected);
}

//===---------------------------------------------------------------------------===//
