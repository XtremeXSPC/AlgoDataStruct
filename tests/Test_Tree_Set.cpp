//===---------------------------------------------------------------------------===//
/**
 * @file Test_Tree_Set.cpp
 * @brief Google Test unit tests for TreeSet.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/associative/Tree_Set.hpp"

#include <gtest/gtest.h>

#include <random>
#include <set>
#include <string>
#include <vector>

using namespace ads::associative;

namespace {

struct MoveOnlyOrdered {
  int value;

  explicit MoveOnlyOrdered(int v) : value(v) {}

  MoveOnlyOrdered(const MoveOnlyOrdered&)                        = delete;
  auto operator=(const MoveOnlyOrdered&) -> MoveOnlyOrdered&     = delete;
  MoveOnlyOrdered(MoveOnlyOrdered&&) noexcept                    = default;
  auto operator=(MoveOnlyOrdered&&) noexcept -> MoveOnlyOrdered& = default;

  auto operator<(const MoveOnlyOrdered& other) const -> bool { return value < other.value; }

  auto operator==(const MoveOnlyOrdered& other) const -> bool { return value == other.value; }
};

auto expect_matches_set(const TreeSet<int>& set, const std::set<int>& oracle) -> void {
  EXPECT_EQ(set.size(), oracle.size());
  EXPECT_EQ(set.is_empty(), oracle.empty());
  const std::vector<int> expected(oracle.begin(), oracle.end());
  EXPECT_EQ(set.to_vector(), expected);
}

} // namespace

// Test fixture for TreeSet.
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

  auto             vec = set.to_vector();
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

  auto                     vec = strings.to_vector();
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

TEST_F(TreeSetTest, RandomizedOperationsMatchStdSet) {
  std::mt19937                    rng(0x5E7);
  std::uniform_int_distribution<> value_dist(0, 99);
  std::uniform_int_distribution<> op_dist(0, 2);
  std::set<int>                   oracle;

  for (int step = 0; step < 500; ++step) {
    const int value = value_dist(rng);
    switch (op_dist(rng)) {
    case 0:
      EXPECT_EQ(set.insert(value), oracle.insert(value).second) << "insert " << value;
      break;
    case 1:
      EXPECT_EQ(set.erase(value), oracle.erase(value) == 1U) << "erase " << value;
      break;
    default:
      EXPECT_EQ(set.contains(value), oracle.contains(value)) << "contains " << value;
      break;
    }

    expect_matches_set(set, oracle);
  }
}

TEST(TreeSetMoveOnlyTest, SupportsMoveOnlyValues) {
  TreeSet<MoveOnlyOrdered> set;

  EXPECT_TRUE(set.insert(MoveOnlyOrdered{20}));
  EXPECT_TRUE(set.insert(MoveOnlyOrdered{10}));
  EXPECT_TRUE(set.insert(MoveOnlyOrdered{30}));
  EXPECT_FALSE(set.insert(MoveOnlyOrdered{20}));

  EXPECT_EQ(set.size(), 3U);
  EXPECT_TRUE(set.contains(MoveOnlyOrdered{10}));
  EXPECT_EQ(set.min().value, 10);
  EXPECT_EQ(set.max().value, 30);

  std::vector<int> values;
  set.for_each([&values](const MoveOnlyOrdered& value) { values.push_back(value.value); });
  std::vector<int> expected{10, 20, 30};
  EXPECT_EQ(values, expected);

  EXPECT_TRUE(set.erase(MoveOnlyOrdered{20}));
  EXPECT_FALSE(set.contains(MoveOnlyOrdered{20}));
}

//===---------------------------------------------------------------------------===//
