//===---------------------------------------------------------------------------===//
/**
 * @file Test_Heaps.cpp
 * @brief Google Test unit tests for MinHeap and MaxHeap implementations.
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/heaps/Max_Heap.hpp"
#include "../include/ads/heaps/Min_Heap.hpp"

using namespace ads::heaps;

// Test fixture for MinHeap.
class MinHeapTest : public ::testing::Test {
protected:
  MinHeap<int> heap;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(MinHeapTest, IsEmptyOnConstruction) {
  EXPECT_EQ(heap.size(), 0);
  EXPECT_TRUE(heap.is_empty());
}

TEST_F(MinHeapTest, InsertAndTop) {
  heap.insert(30);
  EXPECT_EQ(heap.top(), 30);

  heap.insert(10);
  EXPECT_EQ(heap.top(), 10);

  heap.insert(20);
  EXPECT_EQ(heap.top(), 10);
}

TEST_F(MinHeapTest, ExtractMin) {
  heap.insert(30);
  heap.insert(10);
  heap.insert(20);

  EXPECT_EQ(heap.extract_min(), 10);
  EXPECT_EQ(heap.extract_min(), 20);
  EXPECT_EQ(heap.extract_min(), 30);
  EXPECT_TRUE(heap.is_empty());
}

TEST_F(MinHeapTest, TopOnEmptyThrows) {
  EXPECT_THROW(heap.top(), HeapException);
}

TEST_F(MinHeapTest, ExtractOnEmptyThrows) {
  EXPECT_THROW(heap.extract_min(), HeapException);
}

TEST_F(MinHeapTest, Clear) {
  heap.insert(10);
  heap.insert(20);

  heap.clear();
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0);
}

TEST_F(MinHeapTest, BuildFromVector) {
  std::vector<int> data = {50, 30, 40, 10, 20};
  MinHeap<int>     built_heap(data);

  EXPECT_EQ(built_heap.size(), 5);
  EXPECT_EQ(built_heap.top(), 10);
}

TEST_F(MinHeapTest, Emplace) {
  MinHeap<std::string> str_heap;
  str_heap.emplace("Charlie");
  str_heap.emplace("Alice");
  str_heap.emplace("Bob");

  EXPECT_EQ(str_heap.top(), "Alice");
}

TEST_F(MinHeapTest, MoveSemantics) {
  heap.insert(30);
  heap.insert(10);
  heap.insert(20);

  // Move constructor.
  MinHeap<int> moved_heap = std::move(heap);
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(moved_heap.size(), 3);
  EXPECT_EQ(moved_heap.top(), 10);

  // Move assignment.
  heap = std::move(moved_heap);
  EXPECT_TRUE(moved_heap.is_empty());
  EXPECT_EQ(heap.size(), 3);
}

TEST_F(MinHeapTest, HeapPropertyMaintained) {
  std::vector<int> values = {5, 3, 7, 1, 9, 2, 8, 4, 6};
  for (int val : values) {
    heap.insert(val);
  }

  std::vector<int> sorted;
  while (!heap.is_empty()) {
    sorted.push_back(heap.extract_min());
  }

  std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(sorted, expected);
}

TEST_F(MinHeapTest, LargeHeapOperations) {
  const int N = 10000;
  for (int i = N; i > 0; --i) {
    heap.insert(i);
  }

  EXPECT_EQ(heap.size(), N);
  EXPECT_EQ(heap.top(), 1);

  // Extract first 10.
  for (int i = 1; i <= 10; ++i) {
    EXPECT_EQ(heap.extract_min(), i);
  }
}

//===----------------------------- MAX HEAP TESTS ------------------------------===//

class MaxHeapTest : public ::testing::Test {
protected:
  MaxHeap<int> heap;
};

TEST_F(MaxHeapTest, IsEmptyOnConstruction) {
  EXPECT_EQ(heap.size(), 0);
  EXPECT_TRUE(heap.is_empty());
}

TEST_F(MaxHeapTest, InsertAndTop) {
  heap.insert(10);
  EXPECT_EQ(heap.top(), 10);

  heap.insert(30);
  EXPECT_EQ(heap.top(), 30);

  heap.insert(20);
  EXPECT_EQ(heap.top(), 30);
}

TEST_F(MaxHeapTest, ExtractMax) {
  heap.insert(10);
  heap.insert(30);
  heap.insert(20);

  EXPECT_EQ(heap.extract_max(), 30);
  EXPECT_EQ(heap.extract_max(), 20);
  EXPECT_EQ(heap.extract_max(), 10);
  EXPECT_TRUE(heap.is_empty());
}

TEST_F(MaxHeapTest, TopOnEmptyThrows) {
  EXPECT_THROW(heap.top(), HeapException);
}

TEST_F(MaxHeapTest, ExtractOnEmptyThrows) {
  EXPECT_THROW(heap.extract_max(), HeapException);
}

TEST_F(MaxHeapTest, Clear) {
  heap.insert(10);
  heap.insert(20);

  heap.clear();
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0);
}

TEST_F(MaxHeapTest, BuildFromVector) {
  std::vector<int> data = {10, 30, 20, 50, 40};
  MaxHeap<int>     built_heap(data);

  EXPECT_EQ(built_heap.size(), 5);
  EXPECT_EQ(built_heap.top(), 50);
}

TEST_F(MaxHeapTest, Emplace) {
  MaxHeap<std::string> str_heap;
  str_heap.emplace("Alice");
  str_heap.emplace("Charlie");
  str_heap.emplace("Bob");

  EXPECT_EQ(str_heap.top(), "Charlie");
}

TEST_F(MaxHeapTest, MoveSemantics) {
  heap.insert(10);
  heap.insert(30);
  heap.insert(20);

  // Move constructor.
  MaxHeap<int> moved_heap = std::move(heap);
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(moved_heap.size(), 3);
  EXPECT_EQ(moved_heap.top(), 30);

  // Move assignment.
  heap = std::move(moved_heap);
  EXPECT_TRUE(moved_heap.is_empty());
  EXPECT_EQ(heap.size(), 3);
}

TEST_F(MaxHeapTest, HeapPropertyMaintained) {
  std::vector<int> values = {5, 3, 7, 1, 9, 2, 8, 4, 6};
  for (int val : values) {
    heap.insert(val);
  }

  std::vector<int> sorted;
  while (!heap.is_empty()) {
    sorted.push_back(heap.extract_max());
  }

  std::vector<int> expected = {9, 8, 7, 6, 5, 4, 3, 2, 1};
  EXPECT_EQ(sorted, expected);
}

TEST_F(MaxHeapTest, HeapsortApplication) {
  std::vector<int> data = {64, 34, 25, 12, 22, 11, 90};
  MaxHeap<int>     sort_heap(data);

  std::vector<int> descending;
  while (!sort_heap.is_empty()) {
    descending.push_back(sort_heap.extract_max());
  }

  std::vector<int> expected = {90, 64, 34, 25, 22, 12, 11};
  EXPECT_EQ(descending, expected);
}

TEST_F(MaxHeapTest, LargeHeapOperations) {
  const int N = 10000;
  for (int i = 1; i <= N; ++i) {
    heap.insert(i);
  }

  EXPECT_EQ(heap.size(), N);
  EXPECT_EQ(heap.top(), N);

  // Extract first 10
  for (int i = N; i > N - 10; --i) {
    EXPECT_EQ(heap.extract_max(), i);
  }
}

//===---------------------------- COMPARISON TESTS -----------------------------===//

TEST(HeapComparisonTest, MinMaxHeapWithSameData) {
  std::vector<int> data = {5, 2, 8, 1, 9, 3, 7};

  MinHeap<int> min_heap(data);
  MaxHeap<int> max_heap(data);

  EXPECT_EQ(min_heap.top(), 1);
  EXPECT_EQ(max_heap.top(), 9);
}

//===---------------------------------------------------------------------------===//
