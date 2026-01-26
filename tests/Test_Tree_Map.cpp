//===---------------------------------------------------------------------------===//
/**
 * @file Test_Tree_Map.cpp
 * @brief Google Test unit tests for TreeMap.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/associative/Tree_Map.hpp"

using namespace ads::associative;

// Test fixture for TreeMap.
class TreeMapTest : public ::testing::Test {
protected:
  TreeMap<int, std::string> map;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(TreeMapTest, IsEmptyOnConstruction) {
  EXPECT_EQ(map.size(), 0);
  EXPECT_TRUE(map.empty());
}

TEST_F(TreeMapTest, PutAndGet) {
  map.put(10, "ten");
  map.put(5, "five");

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.get(10), "ten");
  EXPECT_EQ(map.get(5), "five");
}

TEST_F(TreeMapTest, PutUpdatesExisting) {
  map.put(1, "one");
  map.put(1, "ONE");

  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map.get(1), "ONE");
}

TEST_F(TreeMapTest, AtThrowsOnMissingKey) {
  EXPECT_THROW(map.at(42), KeyNotFoundException);
}

//===----------------------- INSERTION OPERATIONS TESTS ------------------------===//

TEST_F(TreeMapTest, InsertReturnsInsertionState) {
  EXPECT_TRUE(map.insert(3, "three"));
  EXPECT_FALSE(map.insert(3, "THREE"));
  EXPECT_EQ(map.get(3), "THREE");
}

//===------------------------ CONTAINS AND REMOVE TESTS ------------------------===//

TEST_F(TreeMapTest, ContainsAndRemove) {
  map.put(2, "two");
  map.put(4, "four");

  EXPECT_TRUE(map.contains(2));
  EXPECT_TRUE(map.remove(2));
  EXPECT_FALSE(map.contains(2));
  EXPECT_FALSE(map.remove(2));
}

TEST_F(TreeMapTest, OperatorBracketInsertsDefault) {
  map[7] = "seven";
  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map.get(7), "seven");
}

TEST_F(TreeMapTest, KeysAreOrdered) {
  map.put(3, "c");
  map.put(1, "a");
  map.put(2, "b");

  std::vector<int> expected = {1, 2, 3};
  EXPECT_EQ(map.keys(), expected);
}

TEST_F(TreeMapTest, EmplaceInsertsAndUpdates) {
  struct Payload {
    std::string label;
    int         value;

    Payload(std::string l, int v) : label(std::move(l)), value(v) {}
  };

  TreeMap<int, Payload> payloads;
  EXPECT_TRUE(payloads.emplace(1, "A", 10));
  EXPECT_FALSE(payloads.emplace(1, "B", 20));

  const auto& entry = payloads.get(1);
  EXPECT_EQ(entry.label, "B");
  EXPECT_EQ(entry.value, 20);
}

//===---------------------------------------------------------------------------===//
