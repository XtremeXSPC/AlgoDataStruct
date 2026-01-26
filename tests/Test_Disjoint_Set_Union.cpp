//===---------------------------------------------------------------------------===//
/**
 * @file Test_Disjoint_Set_Union.cpp
 * @brief Google Test unit tests for DisjointSetUnion
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>

#include "../include/ads/graphs/Disjoint_Set_Union.hpp"

using namespace ads::graphs;

class DisjointSetUnionTest : public ::testing::Test {
protected:
  DisjointSetUnion dsu{5};
};

TEST_F(DisjointSetUnionTest, ConstructionInitializesSets) {
  EXPECT_EQ(dsu.size(), 5);
  EXPECT_EQ(dsu.set_count(), 5);
  for (size_t i = 0; i < dsu.size(); ++i) {
    EXPECT_EQ(dsu.find(i), i);
  }
}

TEST_F(DisjointSetUnionTest, UnionAndConnected) {
  EXPECT_TRUE(dsu.union_sets(0, 1));
  EXPECT_TRUE(dsu.union_sets(2, 3));
  EXPECT_TRUE(dsu.connected(0, 1));
  EXPECT_FALSE(dsu.connected(1, 2));

  EXPECT_TRUE(dsu.union_sets(1, 2));
  EXPECT_TRUE(dsu.connected(0, 3));
  EXPECT_EQ(dsu.set_count(), 2);
}

TEST_F(DisjointSetUnionTest, UnionSameSetReturnsFalse) {
  EXPECT_TRUE(dsu.union_sets(0, 1));
  EXPECT_FALSE(dsu.union_sets(0, 1));
  EXPECT_EQ(dsu.set_count(), 4);
}

TEST_F(DisjointSetUnionTest, AddElementCreatesNewSet) {
  size_t index = dsu.add_element();
  EXPECT_EQ(index, 5);
  EXPECT_EQ(dsu.size(), 6);
  EXPECT_EQ(dsu.set_count(), 6);
  EXPECT_EQ(dsu.find(index), index);
}

TEST_F(DisjointSetUnionTest, OutOfRangeThrows) {
  EXPECT_THROW(dsu.find(99), DisjointSetException);
  EXPECT_THROW(dsu.connected(1, 99), DisjointSetException);
}

//===---------------------------------------------------------------------------===//
