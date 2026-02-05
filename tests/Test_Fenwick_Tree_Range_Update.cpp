//===---------------------------------------------------------------------------===//
/**
 * @file Test_Fenwick_Tree_Range_Update.cpp
 * @brief Google Test unit tests for FenwickTreeRangeUpdate implementation.
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/trees/Fenwick_Tree_Range_Update.hpp"

#include <gtest/gtest.h>

using namespace ads::trees;

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST(FenwickTreeRangeUpdateBasicTest, DefaultConstruction) {
  FenwickTreeRangeUpdate<int> tree;
  EXPECT_EQ(tree.size(), 0);
  EXPECT_TRUE(tree.is_empty());
}

TEST(FenwickTreeRangeUpdateBasicTest, ConstructionFromSize) {
  FenwickTreeRangeUpdate<int> tree(5);
  EXPECT_EQ(tree.size(), 5);
  EXPECT_FALSE(tree.is_empty());

  for (size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(tree.point_query(i), 0);
  }
}

//===----------------------------- RANGE ADD TESTS -----------------------------===//

TEST(FenwickTreeRangeUpdateTest, SingleRangeAdd) {
  FenwickTreeRangeUpdate<int> tree(5);

  tree.range_add(1, 3, 10); // Add 10 to indices 1, 2, 3

  EXPECT_EQ(tree.point_query(0), 0);
  EXPECT_EQ(tree.point_query(1), 10);
  EXPECT_EQ(tree.point_query(2), 10);
  EXPECT_EQ(tree.point_query(3), 10);
  EXPECT_EQ(tree.point_query(4), 0);
}

TEST(FenwickTreeRangeUpdateTest, MultipleRangeAdds) {
  FenwickTreeRangeUpdate<int> tree(5);

  tree.range_add(0, 2, 5); // [5, 5, 5, 0, 0]
  tree.range_add(2, 4, 3); // [5, 5, 8, 3, 3]

  EXPECT_EQ(tree.point_query(0), 5);
  EXPECT_EQ(tree.point_query(1), 5);
  EXPECT_EQ(tree.point_query(2), 8);
  EXPECT_EQ(tree.point_query(3), 3);
  EXPECT_EQ(tree.point_query(4), 3);
}

TEST(FenwickTreeRangeUpdateTest, OverlappingRanges) {
  FenwickTreeRangeUpdate<int> tree(6);

  tree.range_add(0, 3, 10); // [10, 10, 10, 10, 0, 0]
  tree.range_add(2, 5, 5);  // [10, 10, 15, 15, 5, 5]

  EXPECT_EQ(tree.point_query(0), 10);
  EXPECT_EQ(tree.point_query(1), 10);
  EXPECT_EQ(tree.point_query(2), 15);
  EXPECT_EQ(tree.point_query(3), 15);
  EXPECT_EQ(tree.point_query(4), 5);
  EXPECT_EQ(tree.point_query(5), 5);
}

TEST(FenwickTreeRangeUpdateTest, SingleElementRange) {
  FenwickTreeRangeUpdate<int> tree(5);

  tree.range_add(2, 2, 100); // Add 100 to index 2 only

  EXPECT_EQ(tree.point_query(0), 0);
  EXPECT_EQ(tree.point_query(1), 0);
  EXPECT_EQ(tree.point_query(2), 100);
  EXPECT_EQ(tree.point_query(3), 0);
  EXPECT_EQ(tree.point_query(4), 0);
}

TEST(FenwickTreeRangeUpdateTest, FullRangeUpdate) {
  FenwickTreeRangeUpdate<int> tree(4);

  tree.range_add(0, 3, 7); // Add 7 to all elements

  for (size_t i = 0; i < 4; ++i) {
    EXPECT_EQ(tree.point_query(i), 7);
  }
}

TEST(FenwickTreeRangeUpdateTest, NegativeDelta) {
  FenwickTreeRangeUpdate<int> tree(5);

  tree.range_add(0, 4, 20); // [20, 20, 20, 20, 20]
  tree.range_add(1, 3, -5); // [20, 15, 15, 15, 20]

  EXPECT_EQ(tree.point_query(0), 20);
  EXPECT_EQ(tree.point_query(1), 15);
  EXPECT_EQ(tree.point_query(2), 15);
  EXPECT_EQ(tree.point_query(3), 15);
  EXPECT_EQ(tree.point_query(4), 20);
}

//===--------------------------- RESET & CLEAR TESTS ---------------------------===//

TEST(FenwickTreeRangeUpdateTest, ClearTree) {
  FenwickTreeRangeUpdate<int> tree(5);
  tree.range_add(0, 4, 10);

  tree.clear();

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0);
}

TEST(FenwickTreeRangeUpdateTest, ResetTree) {
  FenwickTreeRangeUpdate<int> tree(5);
  tree.range_add(0, 4, 10);

  tree.reset(3);

  EXPECT_EQ(tree.size(), 3);
  for (size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(tree.point_query(i), 0);
  }
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST(FenwickTreeRangeUpdateTest, MoveConstructor) {
  FenwickTreeRangeUpdate<int> tree(5);
  tree.range_add(0, 4, 10);

  FenwickTreeRangeUpdate<int> moved = std::move(tree);

  EXPECT_EQ(moved.size(), 5);
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(moved.point_query(2), 10);
}

TEST(FenwickTreeRangeUpdateTest, MoveAssignment) {
  FenwickTreeRangeUpdate<int> tree(5);
  tree.range_add(0, 4, 10);

  FenwickTreeRangeUpdate<int> assigned;
  assigned = std::move(tree);

  EXPECT_EQ(assigned.size(), 5);
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(assigned.point_query(2), 10);
}

//===----------------------------- EDGE CASE TESTS -----------------------------===//

TEST(FenwickTreeRangeUpdateEdgeCaseTest, SingleElement) {
  FenwickTreeRangeUpdate<int> tree(1);

  tree.range_add(0, 0, 42);
  EXPECT_EQ(tree.point_query(0), 42);

  tree.range_add(0, 0, -10);
  EXPECT_EQ(tree.point_query(0), 32);
}

TEST(FenwickTreeRangeUpdateEdgeCaseTest, LastIndexRange) {
  FenwickTreeRangeUpdate<int> tree(5);

  tree.range_add(4, 4, 100); // Only last element

  EXPECT_EQ(tree.point_query(3), 0);
  EXPECT_EQ(tree.point_query(4), 100);
}

TEST(FenwickTreeRangeUpdateEdgeCaseTest, LargeValues) {
  FenwickTreeRangeUpdate<long long> tree(3);

  tree.range_add(0, 2, 1'000'000'000LL);
  tree.range_add(0, 2, 2'000'000'000LL);

  EXPECT_EQ(tree.point_query(0), 3'000'000'000LL);
  EXPECT_EQ(tree.point_query(1), 3'000'000'000LL);
  EXPECT_EQ(tree.point_query(2), 3'000'000'000LL);
}

//===-------------------------- ERROR HANDLING TESTS ---------------------------===//

TEST(FenwickTreeRangeUpdateErrorTest, PointQueryOutOfRange) {
  FenwickTreeRangeUpdate<int> tree(3);

  EXPECT_THROW({ [[maybe_unused]] auto _ = tree.point_query(3); }, FenwickTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto _ = tree.point_query(100); }, FenwickTreeException);
}

TEST(FenwickTreeRangeUpdateErrorTest, RangeAddInvalidRange) {
  FenwickTreeRangeUpdate<int> tree(5);

  EXPECT_THROW(tree.range_add(3, 2, 10), FenwickTreeException); // left > right
}

TEST(FenwickTreeRangeUpdateErrorTest, RangeAddOutOfBounds) {
  FenwickTreeRangeUpdate<int> tree(5);

  EXPECT_THROW(tree.range_add(0, 5, 10), FenwickTreeException); // right >= size
  EXPECT_THROW(tree.range_add(3, 10, 10), FenwickTreeException);
}

TEST(FenwickTreeRangeUpdateErrorTest, EmptyTreeOperations) {
  FenwickTreeRangeUpdate<int> tree;

  EXPECT_THROW({ [[maybe_unused]] auto _ = tree.point_query(0); }, FenwickTreeException);
  EXPECT_THROW(tree.range_add(0, 0, 10), FenwickTreeException);
}

//===---------------------------------------------------------------------------===//
