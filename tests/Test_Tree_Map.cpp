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

#include "../include/ads/associative/Tree_Map.hpp"

#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

using namespace ads::associative;

namespace {

struct LessOnlyKey {
  int value;

  auto operator<(const LessOnlyKey& other) const -> bool { return value < other.value; }

  auto operator==(const LessOnlyKey& other) const -> bool { return value == other.value; }
};

auto expect_matches_map(const TreeMap<int, int>& map, const std::map<int, int>& oracle) -> void {
  EXPECT_EQ(map.size(), oracle.size());
  EXPECT_EQ(map.empty(), oracle.empty());

  std::vector<int>                 keys;
  std::vector<int>                 values;
  std::vector<std::pair<int, int>> entries;
  keys.reserve(oracle.size());
  values.reserve(oracle.size());
  entries.reserve(oracle.size());

  for (const auto& [key, value] : oracle) {
    keys.push_back(key);
    values.push_back(value);
    entries.emplace_back(key, value);

    ASSERT_NE(map.find(key), nullptr);
    EXPECT_EQ(map.at(key), value);
  }

  EXPECT_EQ(map.keys(), keys);
  EXPECT_EQ(map.values(), values);
  EXPECT_EQ(map.entries(), entries);
}

} // namespace

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
  EXPECT_THROW(map.at(42), ads::associative::KeyNotFoundException);
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

TEST(TreeMapModelTest, RandomizedOperationsMatchStdMap) {
  TreeMap<int, int>               map;
  std::map<int, int>              oracle;
  std::mt19937                    rng(0xA11);
  std::uniform_int_distribution<> key_dist(0, 49);
  std::uniform_int_distribution<> value_dist(-100, 100);
  std::uniform_int_distribution<> op_dist(0, 4);

  for (int step = 0; step < 500; ++step) {
    const int key   = key_dist(rng);
    const int value = value_dist(rng);

    switch (op_dist(rng)) {
    case 0:
      map.put(key, value);
      oracle[key] = value;
      break;
    case 1: {
      const bool inserted = !oracle.contains(key);
      EXPECT_EQ(map.insert(key, value), inserted) << "insert " << key;
      oracle[key] = value;
      break;
    }
    case 2:
      EXPECT_EQ(map.remove(key), oracle.erase(key) == 1U) << "remove " << key;
      break;
    case 3:
      map[key] += value;
      oracle[key] += value;
      break;
    default:
      EXPECT_EQ(map.contains(key), oracle.contains(key)) << "contains " << key;
      break;
    }

    expect_matches_map(map, oracle);
  }
}

TEST(TreeMapMoveOnlyTest, SupportsMoveOnlyMappedValues) {
  TreeMap<int, std::unique_ptr<int>> map;
  int                                copied_key = 2;

  map.put(1, std::make_unique<int>(10));
  map.put(copied_key, std::make_unique<int>(20));
  EXPECT_FALSE(map.insert(copied_key, std::make_unique<int>(25)));
  EXPECT_TRUE(map.emplace(3, std::make_unique<int>(30)));
  map[4] = std::make_unique<int>(40);

  ASSERT_NE(map.find(1), nullptr);
  ASSERT_NE(map.find(2), nullptr);
  EXPECT_EQ(*map.at(1), 10);
  EXPECT_EQ(*map.at(2), 25);
  EXPECT_EQ(*map.at(3), 30);
  EXPECT_EQ(*map.at(4), 40);
  EXPECT_EQ(map.size(), 4U);

  EXPECT_TRUE(map.erase(1));
  EXPECT_FALSE(map.contains(1));
}

TEST(TreeMapMoveOnlyTest, CopyPutThrowsForMoveOnlyMappedValues) {
  TreeMap<int, std::unique_ptr<int>> map;
  auto                               value = std::make_unique<int>(10);

  EXPECT_THROW(map.put(1, value), TreeMapException);
}

TEST(TreeMapKeyComparisonTest, SupportsKeysWithLessOnlyOrdering) {
  TreeMap<LessOnlyKey, std::string> map;

  map.put(LessOnlyKey{3}, std::string("three"));
  map.put(LessOnlyKey{1}, std::string("one"));
  map.put(LessOnlyKey{2}, std::string("two"));

  std::vector<LessOnlyKey> expected{{1}, {2}, {3}};
  EXPECT_EQ(map.keys(), expected);
  EXPECT_EQ(map.at(LessOnlyKey{2}), "two");
}

//===---------------------------------------------------------------------------===//
