//===---------------------------------------------------------------------------===//
/**
 * @file Test_Tree_Set.cpp
 * @brief Google Test unit tests for TreeSet
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/associative/Tree_Set.hpp"

using namespace ads::associative;

class TreeSetTest : public ::testing::Test {
protected:
  TreeSet<int> set;
};

TEST_F(TreeSetTest, IsEmptyOnConstruction) {
  EXPECT_EQ(set.size(), 0u);
  EXPECT_TRUE(set.is_empty());
}

TEST_F(TreeSetTest, InsertAndContains) {
  EXPECT_TRUE(set.insert(10));
  EXPECT_TRUE(set.insert(20));
  EXPECT_TRUE(set.insert(30));

  EXPECT_EQ(set.size(), 3u);
  EXPECT_TRUE(set.contains(10));
  EXPECT_TRUE(set.contains(20));
  EXPECT_TRUE(set.contains(30));
  EXPECT_FALSE(set.contains(40));
}

TEST_F(TreeSetTest, InsertDuplicateReturnsFalse) {
  EXPECT_TRUE(set.insert(10));
  EXPECT_FALSE(set.insert(10));
  EXPECT_EQ(set.size(), 1u);
}

TEST_F(TreeSetTest, EraseOperation) {
  set.insert(10);
  set.insert(20);
  set.insert(30);

  EXPECT_TRUE(set.erase(20));
  EXPECT_FALSE(set.contains(20));
  EXPECT_EQ(set.size(), 2u);

  EXPECT_FALSE(set.erase(40));
  EXPECT_EQ(set.size(), 2u);
}

TEST_F(TreeSetTest, ClearOperation) {
  set.insert(10);
  set.insert(20);
  set.insert(30);

  set.clear();
  EXPECT_TRUE(set.is_empty());
  EXPECT_EQ(set.size(), 0u);
}

TEST_F(TreeSetTest, InitializerListConstruction) {
  TreeSet<int> s{5, 3, 7, 1, 9};
  EXPECT_EQ(s.size(), 5u);
  EXPECT_TRUE(s.contains(1));
  EXPECT_TRUE(s.contains(9));
}

TEST_F(TreeSetTest, MoveSemantics) {
  set.insert(10);
  set.insert(20);

  TreeSet<int> moved = std::move(set);
  EXPECT_TRUE(set.is_empty());
  EXPECT_EQ(moved.size(), 2u);

  set = std::move(moved);
  EXPECT_TRUE(moved.is_empty());
  EXPECT_EQ(set.size(), 2u);
}

TEST_F(TreeSetTest, MinAndMax) {
  set.insert(50);
  set.insert(30);
  set.insert(70);
  set.insert(10);
  set.insert(90);

  EXPECT_EQ(set.min(), 10);
  EXPECT_EQ(set.max(), 90);
}

TEST_F(TreeSetTest, ToVectorReturnsSortedOrder) {
  set.insert(50);
  set.insert(30);
  set.insert(70);
  set.insert(10);
  set.insert(90);

  auto vec = set.to_vector();
  std::vector<int> expected{10, 30, 50, 70, 90};
  EXPECT_EQ(vec, expected);
}

TEST_F(TreeSetTest, ForEachTraversal) {
  set.insert(3);
  set.insert(1);
  set.insert(2);

  std::vector<int> values;
  set.for_each([&values](int val) { values.push_back(val); });

  std::vector<int> expected{1, 2, 3};
  EXPECT_EQ(values, expected);
}

TEST_F(TreeSetTest, EmplaceOperation) {
  TreeSet<std::string> strings;
  EXPECT_TRUE(strings.emplace("Hello"));
  EXPECT_TRUE(strings.emplace("World"));
  EXPECT_FALSE(strings.emplace("Hello"));

  EXPECT_EQ(strings.size(), 2u);
}

TEST_F(TreeSetTest, StringSet) {
  TreeSet<std::string> strings;
  strings.insert("cherry");
  strings.insert("apple");
  strings.insert("banana");

  auto vec = strings.to_vector();
  std::vector<std::string> expected{"apple", "banana", "cherry"};
  EXPECT_EQ(vec, expected);
}

TEST_F(TreeSetTest, LargeSetMaintainsOrder) {
  for (int i = 100; i >= 1; --i) {
    set.insert(i);
  }

  auto vec = set.to_vector();
  for (size_t i = 0; i < vec.size(); ++i) {
    EXPECT_EQ(vec[i], static_cast<int>(i + 1));
  }
}

//===---------------------------------------------------------------------------===//
