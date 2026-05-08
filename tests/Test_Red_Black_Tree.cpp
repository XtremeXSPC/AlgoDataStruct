//===--------------------------------------------------------------------------===//
/**
 * @file Test_RedBlackTree.cpp
 * @brief Google Test unit tests for Red-Black Tree implementation
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include "../include/ads/trees/Red_Black_Tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>

using namespace ads::trees;

template <typename T>
using RedBlackTreeType = Red_Black_Tree<T>;

namespace {

struct RbtMoveOnlyOrdered {
  int value;

  explicit RbtMoveOnlyOrdered(int v) : value(v) {}

  RbtMoveOnlyOrdered(const RbtMoveOnlyOrdered&)                        = delete;
  auto operator=(const RbtMoveOnlyOrdered&) -> RbtMoveOnlyOrdered&     = delete;
  RbtMoveOnlyOrdered(RbtMoveOnlyOrdered&&) noexcept                    = default;
  auto operator=(RbtMoveOnlyOrdered&&) noexcept -> RbtMoveOnlyOrdered& = default;

  auto operator<(const RbtMoveOnlyOrdered& other) const -> bool { return value < other.value; }

  auto operator==(const RbtMoveOnlyOrdered& other) const -> bool { return value == other.value; }
};

auto expect_matches_set(const RedBlackTreeType<int>& tree, const std::set<int>& oracle) -> void {
  EXPECT_EQ(tree.size(), oracle.size());
  EXPECT_EQ(tree.is_empty(), oracle.empty());
  EXPECT_TRUE(tree.validate_properties());

  const std::vector<int> actual(tree.begin(), tree.end());
  const std::vector<int> expected(oracle.begin(), oracle.end());
  EXPECT_EQ(actual, expected);
}

} // namespace

// Test fixture for RedBlackTree.
class RedBlackTreeTest : public ::testing::Test {
protected:
  RedBlackTreeType<int> tree;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(RedBlackTreeTest, IsEmptyOnConstruction) {
  EXPECT_EQ(tree.size(), 0);
  EXPECT_TRUE(tree.is_empty());
}

TEST_F(RedBlackTreeTest, Clear) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  EXPECT_FALSE(tree.is_empty());

  tree.clear();
  EXPECT_EQ(tree.size(), 0);
  EXPECT_TRUE(tree.is_empty());
}

//===----------------------------- INSERTION TESTS -----------------------------===//

TEST_F(RedBlackTreeTest, InsertSingleElement) {
  tree.insert(50);
  EXPECT_EQ(tree.size(), 1);
  EXPECT_TRUE(tree.contains(50));
}

TEST_F(RedBlackTreeTest, InsertMultipleElements) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);
  tree.insert(60);
  tree.insert(80);

  EXPECT_EQ(tree.size(), 7);
  EXPECT_TRUE(tree.contains(50));
  EXPECT_TRUE(tree.contains(30));
  EXPECT_TRUE(tree.contains(70));
}

TEST_F(RedBlackTreeTest, InsertDuplicateRejected) {
  tree.insert(50);
  EXPECT_FALSE(tree.insert(50));
  EXPECT_EQ(tree.size(), 1);
}

//===----------------------------- ACCESSOR TESTS ------------------------------===//

TEST_F(RedBlackTreeTest, ContainsElement) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  EXPECT_TRUE(tree.contains(50));
  EXPECT_TRUE(tree.contains(30));
  EXPECT_TRUE(tree.contains(70));
  EXPECT_FALSE(tree.contains(100));
}

TEST_F(RedBlackTreeTest, FindMinMax) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(80);

  EXPECT_EQ(tree.find_min(), 20);
  EXPECT_EQ(tree.find_max(), 80);
}

TEST_F(RedBlackTreeTest, FindMinMaxOnEmptyThrows) {
  EXPECT_THROW([[maybe_unused]] auto val = tree.find_min(), EmptyTreeException);
  EXPECT_THROW([[maybe_unused]] auto val = tree.find_max(), EmptyTreeException);
}

//===------------------------------ REMOVAL TESTS ------------------------------===//

TEST_F(RedBlackTreeTest, RemoveLeafNode) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);

  EXPECT_TRUE(tree.remove(20));
  EXPECT_EQ(tree.size(), 3);
  EXPECT_FALSE(tree.contains(20));
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(RedBlackTreeTest, RemoveNodeWithChildren) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);

  EXPECT_TRUE(tree.remove(30));
  EXPECT_EQ(tree.size(), 4);
  EXPECT_FALSE(tree.contains(30));
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(RedBlackTreeTest, RemoveRoot) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);
  tree.insert(60);
  tree.insert(80);

  EXPECT_TRUE(tree.remove(50));
  EXPECT_EQ(tree.size(), 6);
  EXPECT_FALSE(tree.contains(50));
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(RedBlackTreeTest, RemoveNonExistent) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  EXPECT_FALSE(tree.remove(999));
  EXPECT_EQ(tree.size(), 3);
  EXPECT_TRUE(tree.validate_properties());
}

//===----------------------------- TRAVERSAL TESTS -----------------------------===//

TEST_F(RedBlackTreeTest, InOrderTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);

  std::vector<int> result;
  tree.in_order_traversal([&result](const int& val) { result.push_back(val); });

  std::vector<int> expected = {20, 30, 40, 50, 70};
  EXPECT_EQ(result, expected);
}

TEST_F(RedBlackTreeTest, PreOrderTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);

  std::vector<int> result;
  tree.pre_order_traversal([&result](const int& val) { result.push_back(val); });

  std::vector<int> sorted = result;
  std::sort(sorted.begin(), sorted.end());
  std::vector<int> expected = {20, 30, 40, 50, 70};
  EXPECT_EQ(sorted, expected);
  EXPECT_EQ(result.size(), expected.size());
}

TEST_F(RedBlackTreeTest, PostOrderTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);

  std::vector<int> result;
  tree.post_order_traversal([&result](const int& val) { result.push_back(val); });

  std::vector<int> sorted = result;
  std::sort(sorted.begin(), sorted.end());
  std::vector<int> expected = {20, 30, 40, 50, 70};
  EXPECT_EQ(sorted, expected);
  EXPECT_EQ(result.size(), expected.size());
}

TEST_F(RedBlackTreeTest, LevelOrderTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);

  std::vector<int> result;
  tree.level_order_traversal([&result](const int& val) { result.push_back(val); });

  std::vector<int> sorted = result;
  std::sort(sorted.begin(), sorted.end());
  std::vector<int> expected = {20, 30, 40, 50, 70};
  EXPECT_EQ(sorted, expected);
  EXPECT_EQ(result.size(), expected.size());
}

//===----------------------------- ITERATOR TESTS ------------------------------===//

TEST_F(RedBlackTreeTest, IteratorTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  std::vector<int> actual;
  for (const auto& value : tree) {
    actual.push_back(value);
  }

  std::vector<int> expected = {30, 50, 70};
  EXPECT_EQ(actual, expected);
}

TEST_F(RedBlackTreeTest, ConstIteratorTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  const auto&      tree_ref = tree;
  std::vector<int> actual(tree_ref.cbegin(), tree_ref.cend());

  std::vector<int> expected = {30, 50, 70};
  EXPECT_EQ(actual, expected);
}

TEST_F(RedBlackTreeTest, ExplicitIterator) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  auto it = tree.begin();
  EXPECT_EQ(*it, 30);
  ++it;
  EXPECT_EQ(*it, 50);
}

TEST_F(RedBlackTreeTest, PostfixIteratorReturnsPreviousValue) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  auto it       = tree.begin();
  auto previous = it++;

  EXPECT_EQ(*previous, 30);
  EXPECT_EQ(*it, 50);
}

TEST_F(RedBlackTreeTest, IteratorCopiesAreIndependent) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  auto first = tree.begin();
  auto copy  = first;

  ++first;

  EXPECT_EQ(*copy, 30);
  EXPECT_EQ(*first, 50);
}

TEST_F(RedBlackTreeTest, EmptyIteratorsCompareEqual) {
  EXPECT_EQ(tree.begin(), tree.end());
  EXPECT_EQ(tree.cbegin(), tree.cend());
}

TEST_F(RedBlackTreeTest, STLAlgorithms) {
  for (int value : {40, 20, 60, 10, 30, 50, 70}) {
    tree.insert(value);
  }

  std::vector<int> actual;
  std::copy(tree.begin(), tree.end(), std::back_inserter(actual));

  std::vector<int> expected = {10, 20, 30, 40, 50, 60, 70};
  EXPECT_EQ(actual, expected);
  EXPECT_EQ(std::accumulate(tree.begin(), tree.end(), 0), 280);
  EXPECT_EQ(std::distance(tree.begin(), tree.end()), static_cast<std::ptrdiff_t>(7));
  EXPECT_NE(std::find(tree.begin(), tree.end(), 50), tree.end());
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(RedBlackTreeTest, MoveConstructor) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  RedBlackTreeType<int> moved_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(moved_tree.size(), 3);
  EXPECT_TRUE(moved_tree.contains(50));
}

TEST_F(RedBlackTreeTest, MoveAssignment) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  RedBlackTreeType<int> other_tree;
  other_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(other_tree.size(), 3);
}

//===------------------------------ BALANCE TESTS ------------------------------===//

TEST_F(RedBlackTreeTest, BalanceAfterAscendingInsertions) {
  // Insert in ascending order.
  for (int i = 1; i <= 50; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 50);
  // Red-Black tree should be balanced.
  EXPECT_LE(tree.height(), 12); // 2*log2(50+1) ~ 12.
}

TEST_F(RedBlackTreeTest, BalanceAfterDescendingInsertions) {
  // Insert in descending order.
  for (int i = 50; i >= 1; --i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 50);
  EXPECT_LE(tree.height(), 12);
}

TEST_F(RedBlackTreeTest, BalanceAfterMultipleOperations) {
  for (int i = 1; i <= 100; ++i) {
    tree.insert(i);
  }

  for (int i = 2; i <= 100; i += 2) {
    EXPECT_TRUE(tree.remove(i));
  }

  EXPECT_EQ(tree.size(), 50);
  EXPECT_TRUE(tree.validate_properties());

  for (int i = 1; i <= 100; i += 2) {
    EXPECT_TRUE(tree.contains(i));
  }
  for (int i = 2; i <= 100; i += 2) {
    EXPECT_FALSE(tree.contains(i));
  }
}

TEST_F(RedBlackTreeTest, MixedDeletionOrderMaintainsProperties) {
  constexpr int kElementCount = 200;
  for (int i = 1; i <= kElementCount; ++i) {
    ASSERT_TRUE(tree.insert(i));
  }

  std::vector<int> removal_order;
  for (int i = 1; i <= kElementCount; i += 3) {
    removal_order.push_back(i);
  }
  for (int i = kElementCount; i >= 1; --i) {
    if (i % 3 == 2) {
      removal_order.push_back(i);
    }
  }
  for (int i = 1; i <= kElementCount; ++i) {
    if (i % 3 == 0) {
      removal_order.push_back(i);
    }
  }

  size_t expected_size = kElementCount;
  for (int value : removal_order) {
    ASSERT_TRUE(tree.remove(value)) << "value: " << value;
    --expected_size;

    EXPECT_EQ(tree.size(), expected_size);
    EXPECT_FALSE(tree.contains(value));
    EXPECT_TRUE(tree.validate_properties()) << "after removing: " << value;
  }

  EXPECT_TRUE(tree.is_empty());
}

TEST_F(RedBlackTreeTest, ReinsertAfterDeletionsMaintainsSortedOrder) {
  for (int value = 1; value <= 80; ++value) {
    ASSERT_TRUE(tree.insert(value));
  }

  for (int value = 10; value <= 70; value += 10) {
    ASSERT_TRUE(tree.remove(value)) << "removed value: " << value;
    ASSERT_TRUE(tree.validate_properties());
  }

  for (int value = 100; value <= 140; value += 10) {
    ASSERT_TRUE(tree.insert(value)) << "inserted value: " << value;
    ASSERT_TRUE(tree.validate_properties());
  }

  std::vector<int> actual(tree.begin(), tree.end());

  std::vector<int> expected;
  for (int value = 1; value <= 80; ++value) {
    if (value % 10 != 0 || value > 70) {
      expected.push_back(value);
    }
  }
  for (int value = 100; value <= 140; value += 10) {
    expected.push_back(value);
  }

  EXPECT_EQ(actual, expected);
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(RedBlackTreeTest, RandomizedOperationsMatchStdSet) {
  std::mt19937                    rng(0xB1AC);
  std::uniform_int_distribution<> value_dist(0, 99);
  std::uniform_int_distribution<> op_dist(0, 2);
  std::set<int>                   oracle;

  for (int step = 0; step < 500; ++step) {
    const int value = value_dist(rng);
    switch (op_dist(rng)) {
    case 0:
      EXPECT_EQ(tree.insert(value), oracle.insert(value).second) << "insert " << value;
      break;
    case 1:
      EXPECT_EQ(tree.remove(value), oracle.erase(value) == 1U) << "remove " << value;
      break;
    default:
      EXPECT_EQ(tree.contains(value), oracle.contains(value)) << "contains " << value;
      break;
    }

    expect_matches_set(tree, oracle);
  }
}

//===----------------------------- EDGE CASE TESTS -----------------------------===//

TEST_F(RedBlackTreeTest, SingleElementTree) {
  tree.insert(42);

  EXPECT_EQ(tree.size(), 1);
  EXPECT_EQ(tree.find_min(), 42);
  EXPECT_EQ(tree.find_max(), 42);
}

TEST_F(RedBlackTreeTest, ClearAndReuse) {
  tree.insert(50);
  tree.insert(30);
  tree.clear();

  tree.insert(100);
  EXPECT_EQ(tree.size(), 1);
  EXPECT_TRUE(tree.contains(100));
}

//===---------------------------- CUSTOM TYPE TESTS ----------------------------===//

TEST(RedBlackTreeCustomTypeTest, StringKeys) {
  RedBlackTreeType<std::string> str_tree;

  str_tree.insert("banana");
  str_tree.insert("apple");
  str_tree.insert("cherry");

  EXPECT_EQ(str_tree.size(), 3);
  EXPECT_EQ(str_tree.find_min(), "apple");
  EXPECT_EQ(str_tree.find_max(), "cherry");
}

TEST(RedBlackTreeMoveOnlyTest, SupportsMoveOnlyValuesAndNativeRemoval) {
  RedBlackTreeType<RbtMoveOnlyOrdered> tree;

  EXPECT_TRUE(tree.insert(RbtMoveOnlyOrdered{40}));
  EXPECT_TRUE(tree.emplace(20));
  EXPECT_TRUE(tree.insert(RbtMoveOnlyOrdered{60}));
  EXPECT_TRUE(tree.emplace(10));
  EXPECT_TRUE(tree.emplace(30));
  EXPECT_TRUE(tree.emplace(50));
  EXPECT_TRUE(tree.emplace(70));
  EXPECT_FALSE(tree.insert(RbtMoveOnlyOrdered{40}));

  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.find_min().value, 10);
  EXPECT_EQ(tree.find_max().value, 70);

  EXPECT_TRUE(tree.remove(RbtMoveOnlyOrdered{40}));
  EXPECT_TRUE(tree.remove(RbtMoveOnlyOrdered{10}));
  EXPECT_FALSE(tree.contains(RbtMoveOnlyOrdered{40}));
  EXPECT_TRUE(tree.validate_properties());

  std::vector<int> values;
  for (const RbtMoveOnlyOrdered& value : tree) {
    values.push_back(value.value);
  }
  std::vector<int> expected{20, 30, 50, 60, 70};
  EXPECT_EQ(values, expected);
}

//===---------------------------------------------------------------------------===//
