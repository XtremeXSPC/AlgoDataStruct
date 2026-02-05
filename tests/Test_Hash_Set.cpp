//===---------------------------------------------------------------------------===//
/**
 * @file Test_Hash_Set.cpp
 * @brief Google Test unit tests for HashSet.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/associative/Hash_Set.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ads::associative;

class HashSetTest : public ::testing::Test {
protected:
  HashSet<int> set;
};

TEST_F(HashSetTest, IsEmptyOnConstruction) {
  EXPECT_EQ(set.size(), 0u);
  EXPECT_TRUE(set.is_empty());
}

TEST_F(HashSetTest, InsertAndContains) {
  EXPECT_TRUE(set.insert(10));
  EXPECT_TRUE(set.insert(20));
  EXPECT_TRUE(set.insert(30));

  EXPECT_EQ(set.size(), 3u);
  EXPECT_TRUE(set.contains(10));
  EXPECT_TRUE(set.contains(20));
  EXPECT_TRUE(set.contains(30));
  EXPECT_FALSE(set.contains(40));
}

TEST_F(HashSetTest, InsertDuplicateReturnsFalse) {
  EXPECT_TRUE(set.insert(10));
  EXPECT_FALSE(set.insert(10));
  EXPECT_EQ(set.size(), 1u);
}

TEST_F(HashSetTest, EraseOperation) {
  set.insert(10);
  set.insert(20);
  set.insert(30);

  EXPECT_TRUE(set.erase(20));
  EXPECT_FALSE(set.contains(20));
  EXPECT_EQ(set.size(), 2u);

  EXPECT_FALSE(set.erase(40));
  EXPECT_EQ(set.size(), 2u);
}

TEST_F(HashSetTest, ClearOperation) {
  set.insert(10);
  set.insert(20);
  set.insert(30);

  set.clear();
  EXPECT_TRUE(set.is_empty());
  EXPECT_EQ(set.size(), 0u);
}

TEST_F(HashSetTest, InitializerListConstruction) {
  HashSet<int> s{1, 2, 3, 4, 5};
  EXPECT_EQ(s.size(), 5u);
  EXPECT_TRUE(s.contains(1));
  EXPECT_TRUE(s.contains(5));
}

TEST_F(HashSetTest, MoveSemantics) {
  set.insert(10);
  set.insert(20);

  HashSet<int> moved = std::move(set);
  EXPECT_TRUE(set.is_empty());
  EXPECT_EQ(moved.size(), 2u);

  set = std::move(moved);
  EXPECT_TRUE(moved.is_empty());
  EXPECT_EQ(set.size(), 2u);
}

TEST_F(HashSetTest, RangeBasedIteration) {
  set.insert(10);
  set.insert(20);
  set.insert(30);

  std::vector<int> values;
  for (const auto& val : set) {
    values.push_back(val);
  }

  EXPECT_EQ(values.size(), 3u);
}

TEST_F(HashSetTest, RehashOnLoadFactor) {
  HashSet<int> small_set(4, 0.75);

  for (int i = 0; i < 100; ++i) {
    small_set.insert(i);
  }

  EXPECT_EQ(small_set.size(), 100u);
  for (int i = 0; i < 100; ++i) {
    EXPECT_TRUE(small_set.contains(i));
  }
}

TEST_F(HashSetTest, EmplaceOperation) {
  HashSet<std::string> strings;
  EXPECT_TRUE(strings.emplace("Hello"));
  EXPECT_TRUE(strings.emplace("World"));
  EXPECT_FALSE(strings.emplace("Hello"));

  EXPECT_EQ(strings.size(), 2u);
}

TEST_F(HashSetTest, LoadFactor) {
  set.insert(1);
  set.insert(2);
  set.insert(3);
  set.insert(4);

  EXPECT_GT(set.load_factor(), 0.0);
  EXPECT_LT(set.load_factor(), 1.0);
}

TEST_F(HashSetTest, StringSet) {
  HashSet<std::string> strings;
  strings.insert("apple");
  strings.insert("banana");
  strings.insert("cherry");

  EXPECT_TRUE(strings.contains("apple"));
  EXPECT_TRUE(strings.contains("banana"));
  EXPECT_TRUE(strings.contains("cherry"));
  EXPECT_FALSE(strings.contains("date"));
}

//===---------------------------------------------------------------------------===//
