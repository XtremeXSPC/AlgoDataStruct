//===---------------------------------------------------------------------------===//
/**
 * @file Test_BPlus_Tree.cpp
 * @brief Google Test unit tests for BPlusTree.
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/trees/search/BPlus_Tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace ads::trees;

namespace {

using Tree = BPlusTree<int, int>;

struct BucketKey {
  int value;

  friend auto operator<(const BucketKey& lhs, const BucketKey& rhs) noexcept -> bool { return lhs.value / 10 < rhs.value / 10; }
};

struct ThrowingCompareKey {
  int value;

  friend auto operator<(const ThrowingCompareKey& lhs, const ThrowingCompareKey& rhs) -> bool { return lhs.value < rhs.value; }
};

struct ThrowingCopyKey {
  int                value;
  static inline bool fail_copy = false;

  explicit ThrowingCopyKey(int key) : value(key) {}

  ThrowingCopyKey(const ThrowingCopyKey& other) : value(other.value) {
    if (fail_copy) {
      throw std::runtime_error("key copy failed");
    }
  }

  ThrowingCopyKey(ThrowingCopyKey&&) noexcept                    = default;
  auto operator=(const ThrowingCopyKey&) -> ThrowingCopyKey&     = default;
  auto operator=(ThrowingCopyKey&&) noexcept -> ThrowingCopyKey& = default;

  friend auto operator<(const ThrowingCopyKey& lhs, const ThrowingCopyKey& rhs) noexcept -> bool { return lhs.value < rhs.value; }
};

struct ThrowingCopyValue {
  int                value;
  static inline bool fail_copy = false;

  explicit ThrowingCopyValue(int payload) : value(payload) {}

  ThrowingCopyValue(const ThrowingCopyValue& other) : value(other.value) {
    if (fail_copy) {
      throw std::runtime_error("value copy failed");
    }
  }

  ThrowingCopyValue(ThrowingCopyValue&&) noexcept                    = default;
  auto operator=(const ThrowingCopyValue&) -> ThrowingCopyValue&     = default;
  auto operator=(ThrowingCopyValue&&) noexcept -> ThrowingCopyValue& = default;
};

struct NonAssignableValue {
  NonAssignableValue()                                        = default;
  NonAssignableValue(NonAssignableValue&&) noexcept           = default;
  auto operator=(NonAssignableValue&&) -> NonAssignableValue& = delete;
};

static_assert(BPlusKey<int>);
static_assert(BPlusKey<BucketKey>);
static_assert(BPlusKey<ThrowingCopyKey>);
static_assert(!BPlusKey<ThrowingCompareKey>);
static_assert(BPlusValue<int>);
static_assert(BPlusValue<std::unique_ptr<int>>);
static_assert(!BPlusValue<NonAssignableValue>);

//===----- TEST UTILITIES ------------------------------------------------------===//

///@brief Collects the whole tree, in leaf-chain order, as (key, value) pairs.
template <int Degree>
auto dump(const BPlusTree<int, int, Degree>& tree) -> std::vector<std::pair<int, int>> {
  std::vector<std::pair<int, int>> out;
  for (const auto& entry : tree) {
    out.emplace_back(entry.key, entry.value);
  }
  return out;
}

///@brief Mirrors an ordered map as a sorted vector of pairs.
auto dump(const std::map<int, int>& ref) -> std::vector<std::pair<int, int>> {
  return {ref.begin(), ref.end()};
}

//===----- BASIC OPERATIONS ----------------------------------------------------===//

TEST(BPlusTreeBasic, EmptyState) {
  Tree tree;
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0U);
  EXPECT_EQ(tree.height(), 0);
  EXPECT_FALSE(tree.contains(5));
  EXPECT_EQ(tree.find(5), nullptr);
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.begin(), tree.end());
}

TEST(BPlusTreeBasic, InsertFindContains) {
  Tree tree;
  EXPECT_TRUE(tree.insert(10, 100));
  EXPECT_TRUE(tree.insert(20, 200));
  EXPECT_TRUE(tree.insert(5, 50));

  EXPECT_EQ(tree.size(), 3U);
  ASSERT_NE(tree.find(10), nullptr);
  EXPECT_EQ(*tree.find(10), 100);
  EXPECT_EQ(*tree.find(5), 50);
  EXPECT_TRUE(tree.contains(20));
  EXPECT_FALSE(tree.contains(15));
  EXPECT_TRUE(tree.validate_properties());
}

TEST(BPlusTreeBasic, DuplicateKeyRejected) {
  Tree tree;
  EXPECT_TRUE(tree.insert(7, 1));
  EXPECT_FALSE(tree.insert(7, 999));
  EXPECT_EQ(tree.size(), 1U);
  ASSERT_NE(tree.find(7), nullptr);
  EXPECT_EQ(*tree.find(7), 1); // Value not overwritten.
}

TEST(BPlusTreeBasic, OrderingEquivalentKeyIsDuplicate) {
  BPlusTree<BucketKey, int> tree;
  EXPECT_TRUE(tree.insert(BucketKey{11}, 1));
  EXPECT_FALSE(tree.insert(BucketKey{12}, 2));
  EXPECT_EQ(tree.size(), 1U);

  const int* found = tree.find(BucketKey{12});
  ASSERT_NE(found, nullptr);
  EXPECT_EQ(*found, 1);
  EXPECT_TRUE(tree.validate_properties());
}

TEST(BPlusTreeBasic, MinMax) {
  Tree tree;
  for (int k : {30, 10, 50, 20, 40}) {
    tree.insert(k, k * 10);
  }
  EXPECT_EQ(tree.find_min().key, 10);
  EXPECT_EQ(tree.find_min().value, 100);
  EXPECT_EQ(tree.find_max().key, 50);
  EXPECT_EQ(tree.find_max().value, 500);
}

TEST(BPlusTreeBasic, MinMaxThrowWhenEmpty) {
  Tree tree;
  EXPECT_THROW((void)tree.find_min(), EmptyTreeException);
  EXPECT_THROW((void)tree.find_max(), EmptyTreeException);
}

//===----- ORDERED SCAN & SPLITTING --------------------------------------------===//

TEST(BPlusTreeScan, LeafChainStaysSortedAcrossSplits) {
  Tree tree;
  for (int k = 0; k < 200; ++k) {
    tree.insert(k, k);
  }
  ASSERT_TRUE(tree.validate_properties());
  EXPECT_GT(tree.height(), 0); // Splits produced internal levels.

  std::vector<std::pair<int, int>> expected;
  for (int k = 0; k < 200; ++k) {
    expected.emplace_back(k, k);
  }
  EXPECT_EQ(dump(tree), expected);
}

TEST(BPlusTreeScan, ReverseInsertionStillSorted) {
  Tree tree;
  for (int k = 199; k >= 0; --k) {
    tree.insert(k, k * 2);
  }
  ASSERT_TRUE(tree.validate_properties());
  auto d = dump(tree);
  ASSERT_EQ(d.size(), 200U);
  for (int k = 0; k < 200; ++k) {
    EXPECT_EQ(d[static_cast<std::size_t>(k)], std::make_pair(k, k * 2));
  }
}

//===----- RANGE QUERIES -------------------------------------------------------===//

TEST(BPlusTreeRange, InclusiveRangeMatchesOracle) {
  Tree                            tree;
  std::map<int, int>              ref;
  std::mt19937                    rng(2'024);
  std::uniform_int_distribution<> dist(0, 999);
  for (int i = 0; i < 500; ++i) {
    const int k = dist(rng);
    if (ref.emplace(k, i).second) {
      tree.insert(k, i);
    }
  }
  ASSERT_TRUE(tree.validate_properties());

  for (int q = 0; q < 200; ++q) {
    int lo = dist(rng);
    int hi = dist(rng);
    if (lo > hi) {
      std::swap(lo, hi);
    }
    std::vector<std::pair<int, int>> got;
    tree.range(lo, hi, [&](const int& k, const int& v) { got.emplace_back(k, v); });

    std::vector<std::pair<int, int>> expected;
    for (auto it = ref.lower_bound(lo); it != ref.end() && it->first <= hi; ++it) {
      expected.emplace_back(it->first, it->second);
    }
    EXPECT_EQ(got, expected) << "range [" << lo << ", " << hi << "]";
  }
}

//===----- RANDOMIZED CHURN (INSERT + REMOVE) ----------------------------------===//

template <int Degree>
void run_churn(unsigned seed, int operations, int key_space) {
  BPlusTree<int, int, Degree>     tree;
  std::map<int, int>              ref;
  std::mt19937                    rng(seed);
  std::uniform_int_distribution<> key_dist(0, key_space);
  std::uniform_int_distribution<> op_dist(0, 2); // Bias toward insertion.

  for (int i = 0; i < operations; ++i) {
    const int key = key_dist(rng);
    if (op_dist(rng) == 0) {
      const bool tree_removed = tree.remove(key);
      const bool ref_removed  = ref.erase(key) != 0;
      ASSERT_EQ(tree_removed, ref_removed) << "remove " << key << " at op " << i;
    } else {
      const bool tree_ins = tree.insert(key, i);
      const bool ref_ins  = ref.emplace(key, i).second;
      ASSERT_EQ(tree_ins, ref_ins) << "insert " << key << " at op " << i;
    }

    ASSERT_EQ(tree.size(), ref.size()) << "size mismatch at op " << i;
    ASSERT_TRUE(tree.validate_properties()) << "invariant broken at op " << i;
  }
  EXPECT_EQ(dump(tree), dump(ref));
}

TEST(BPlusTreeChurn, DegreeTwo) {
  run_churn<2>(11, 4'000, 60);
}

TEST(BPlusTreeChurn, DegreeThree) {
  run_churn<3>(22, 4'000, 80);
}

TEST(BPlusTreeChurn, DegreeSix) {
  run_churn<6>(33, 6'000, 150);
}

TEST(BPlusTreeChurn, WideKeySpaceLargeDegree) {
  run_churn<16>(44, 8'000, 5'000);
}

TEST(BPlusTreeChurn, HighDegreeTreeRegrowsAfterRootStorageShrinks) {
  constexpr int           kRecordCount = 4'096;
  constexpr int           kSurvivors   = 64;
  BPlusTree<int, int, 16> tree;

  for (int key = 0; key < kRecordCount; ++key) {
    ASSERT_TRUE(tree.insert(key, key * 2));
  }
  ASSERT_TRUE(tree.validate_properties());

  for (int key = 0; key < kRecordCount - kSurvivors; ++key) {
    ASSERT_TRUE(tree.remove(key));
  }
  ASSERT_EQ(tree.size(), static_cast<std::size_t>(kSurvivors));
  ASSERT_TRUE(tree.validate_properties());

  for (int key = 0; key < kRecordCount - kSurvivors; ++key) {
    ASSERT_TRUE(tree.insert(key, key * 2));
  }
  ASSERT_EQ(tree.size(), static_cast<std::size_t>(kRecordCount));
  ASSERT_TRUE(tree.validate_properties());

  for (int key = 0; key < kRecordCount; ++key) {
    const int* value = tree.find(key);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, key * 2);
  }
}

//===----- REMOVAL EDGE CASES --------------------------------------------------===//

TEST(BPlusTreeRemoval, RemoveMissingReturnsFalse) {
  Tree tree;
  tree.insert(1, 1);
  EXPECT_FALSE(tree.remove(2));
  EXPECT_EQ(tree.size(), 1U);
}

TEST(BPlusTreeRemoval, RemoveEverythingEmptiesTree) {
  Tree             tree;
  std::vector<int> keys;
  for (int k = 0; k < 100; ++k) {
    tree.insert(k, k);
    keys.push_back(k);
  }
  std::mt19937 rng(7);
  std::shuffle(keys.begin(), keys.end(), rng);
  for (int k : keys) {
    EXPECT_TRUE(tree.remove(k));
    EXPECT_TRUE(tree.validate_properties());
  }
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0U);
  EXPECT_EQ(tree.height(), 0);
  EXPECT_EQ(tree.begin(), tree.end());
}

//===----- CONSTRUCTION VARIANTS -----------------------------------------------===//

TEST(BPlusTreeConstruction, FromEntryArray) {
  DynamicArray<Tree::Entry> entries;
  entries.push_back({3, 30});
  entries.push_back({1, 10});
  entries.push_back({2, 20});
  entries.push_back({1, 99}); // Duplicate key ignored.

  Tree tree(entries);
  EXPECT_EQ(tree.size(), 3U);
  ASSERT_NE(tree.find(1), nullptr);
  EXPECT_EQ(*tree.find(1), 10);
  EXPECT_TRUE(tree.validate_properties());
}

TEST(BPlusTreeConstruction, FromIteratorRange) {
  std::vector<Tree::Entry> entries = {{5, 50}, {1, 10}, {3, 30}};
  Tree                     tree(entries.begin(), entries.end());
  EXPECT_EQ(tree.size(), 3U);
  EXPECT_EQ(dump(tree), (std::vector<std::pair<int, int>>{{1, 10}, {3, 30}, {5, 50}}));
}

//===----- MOVE SEMANTICS ------------------------------------------------------===//

TEST(BPlusTreeMove, MoveConstructTransfersOwnership) {
  Tree source;
  for (int k = 0; k < 50; ++k) {
    source.insert(k, k);
  }
  Tree moved(std::move(source));
  EXPECT_EQ(moved.size(), 50U);
  EXPECT_TRUE(moved.validate_properties());
  EXPECT_TRUE(source.is_empty());
}

TEST(BPlusTreeMove, MoveAssignReplacesTarget) {
  Tree source;
  source.insert(1, 1);
  Tree target;
  target.insert(9, 9);
  target = std::move(source);
  EXPECT_EQ(target.size(), 1U);
  EXPECT_TRUE(target.contains(1));
  EXPECT_FALSE(target.contains(9));
}

//===----- ALTERNATE TYPES -----------------------------------------------------===//

TEST(BPlusTreeTypes, StringValues) {
  BPlusTree<int, std::string> tree;
  tree.insert(2, "two");
  tree.insert(1, "one");
  tree.insert(3, "three");
  ASSERT_NE(tree.find(2), nullptr);
  EXPECT_EQ(*tree.find(2), "two");

  std::vector<std::string> ordered;
  tree.in_order_traversal([&](const int&, const std::string& v) { ordered.push_back(v); });
  EXPECT_EQ(ordered, (std::vector<std::string>{"one", "two", "three"}));
}

TEST(BPlusTreeTypes, MoveOnlyValuesSurviveSplits) {
  BPlusTree<int, std::unique_ptr<int>> tree;
  for (int key = 0; key < 30; ++key) {
    EXPECT_TRUE(tree.insert(key, std::make_unique<int>(key * 2)));
  }

  const std::unique_ptr<int>* found = tree.find(17);
  ASSERT_NE(found, nullptr);
  ASSERT_NE(*found, nullptr);
  EXPECT_EQ(**found, 34);
  EXPECT_TRUE(tree.validate_properties());
}

//===----- INVALID KEYS AND EXCEPTION SAFETY ----------------------------------===//

TEST(BPlusTreeErrors, NaNKeysThrowForEveryKeyBasedOperation) {
  using DoubleTree = BPlusTree<double, int>;
  const double nan = std::numeric_limits<double>::quiet_NaN();

  DoubleTree tree;
  EXPECT_THROW(tree.insert(nan, 1), BinaryTreeException);
  EXPECT_THROW(tree.remove(nan), BinaryTreeException);
  EXPECT_THROW((void)tree.find(nan), BinaryTreeException);
  EXPECT_THROW((void)tree.contains(nan), BinaryTreeException);
  EXPECT_THROW(tree.range(nan, 10.0, [](const double&, const int&) -> void {}), BinaryTreeException);
  EXPECT_THROW(tree.range(0.0, nan, [](const double&, const int&) -> void {}), BinaryTreeException);
  EXPECT_TRUE(tree.is_empty());
  EXPECT_TRUE(tree.validate_properties());
}

TEST(BPlusTreeExceptionSafety, FailedFirstKeyCopyLeavesTreeEmpty) {
  BPlusTree<ThrowingCopyKey, int> tree;
  const ThrowingCopyKey           key{1};

  ThrowingCopyKey::fail_copy = true;
  EXPECT_THROW(tree.insert(key, 1), std::runtime_error);
  ThrowingCopyKey::fail_copy = false;

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0U);
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_THROW((void)tree.find_min(), EmptyTreeException);
}

TEST(BPlusTreeExceptionSafety, FailedSeparatorCopyPreservesFullRoot) {
  BPlusTree<ThrowingCopyKey, int> tree;
  for (int key = 0; key < 5; ++key) {
    EXPECT_TRUE(tree.insert(ThrowingCopyKey{key}, key));
  }
  const ThrowingCopyKey candidate{5};

  ThrowingCopyKey::fail_copy = true;
  EXPECT_THROW(tree.insert(candidate, 5), std::runtime_error);
  ThrowingCopyKey::fail_copy = false;

  EXPECT_EQ(tree.size(), 5U);
  for (int key = 0; key < 5; ++key) {
    EXPECT_TRUE(tree.contains(ThrowingCopyKey{key}));
  }
  EXPECT_FALSE(tree.contains(candidate));
  EXPECT_TRUE(tree.validate_properties());
}

TEST(BPlusTreeExceptionSafety, FailedValueCopyPreservesRecordsAfterSplit) {
  BPlusTree<int, ThrowingCopyValue> tree;
  for (int key = 0; key < 5; ++key) {
    EXPECT_TRUE(tree.insert(key, ThrowingCopyValue{key}));
  }
  const ThrowingCopyValue candidate{5};

  ThrowingCopyValue::fail_copy = true;
  EXPECT_THROW(tree.insert(5, candidate), std::runtime_error);
  ThrowingCopyValue::fail_copy = false;

  EXPECT_EQ(tree.size(), 5U);
  for (int key = 0; key < 5; ++key) {
    const ThrowingCopyValue* found = tree.find(key);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->value, key);
  }
  EXPECT_EQ(tree.find(5), nullptr);
  EXPECT_TRUE(tree.validate_properties());
}

TEST(BPlusTreeExceptionSafety, FailedSeparatorCopyDuringRemovalPreservesRecords) {
  BPlusTree<ThrowingCopyKey, int> tree;
  for (int key = 0; key < 6; ++key) {
    tree.insert(ThrowingCopyKey{key}, key);
  }
  ASSERT_TRUE(tree.remove(ThrowingCopyKey{0}));
  ASSERT_EQ(tree.size(), 5U);

  ThrowingCopyKey::fail_copy = true;
  EXPECT_THROW(tree.remove(ThrowingCopyKey{1}), std::runtime_error);
  ThrowingCopyKey::fail_copy = false;

  EXPECT_EQ(tree.size(), 5U);
  for (int key = 1; key < 6; ++key) {
    EXPECT_TRUE(tree.contains(ThrowingCopyKey{key}));
  }
  EXPECT_TRUE(tree.validate_properties());
}

} // namespace

//===---------------------------------------------------------------------------===//
