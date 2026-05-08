//===---------------------------------------------------------------------------===//
/**
 * @file Test_AVLTree.cpp
 * @brief Google Test unit tests for AVL Tree implementation.
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/trees/AVL_Tree.hpp"

#include <gtest/gtest.h>

#include <random>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace ads::trees;

namespace {

auto expect_matches_set(const AVLTree<int>& tree, const std::set<int>& oracle) -> void {
  EXPECT_EQ(tree.size(), oracle.size());
  EXPECT_EQ(tree.is_empty(), oracle.empty());
  EXPECT_TRUE(tree.validate_properties());

  std::vector<int> actual;
  tree.in_order_traversal([&actual](const int& value) { actual.push_back(value); });
  const std::vector<int> expected(oracle.begin(), oracle.end());
  EXPECT_EQ(actual, expected);
}

} // namespace

// Test fixture for AVLTree.
class AVLTreeTest : public ::testing::Test {
protected:
  AVLTree<int> tree;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(AVLTreeTest, IsEmptyOnConstruction) {
  EXPECT_EQ(tree.size(), 0);
  EXPECT_TRUE(tree.is_empty());
}

TEST_F(AVLTreeTest, Clear) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  EXPECT_FALSE(tree.is_empty());

  tree.clear();
  EXPECT_EQ(tree.size(), 0);
  EXPECT_TRUE(tree.is_empty());
}

//===----------------------------- INSERTION TESTS -----------------------------===//

TEST_F(AVLTreeTest, InsertSingleElement) {
  tree.insert(50);
  EXPECT_EQ(tree.size(), 1);
  EXPECT_TRUE(tree.contains(50));
}

TEST_F(AVLTreeTest, InsertMultipleElements) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);

  EXPECT_EQ(tree.size(), 5);
  EXPECT_TRUE(tree.contains(50));
  EXPECT_TRUE(tree.contains(30));
  EXPECT_TRUE(tree.contains(70));
}

TEST_F(AVLTreeTest, InsertDuplicateRejected) {
  tree.insert(50);
  EXPECT_FALSE(tree.insert(50));
  EXPECT_EQ(tree.size(), 1);
}

//===----------------------------- ROTATION TESTS ------------------------------===//

TEST_F(AVLTreeTest, LeftLeftRotation) {
  // Insert in descending order to trigger LL rotation.
  tree.insert(30);
  tree.insert(20);
  tree.insert(10); // Should trigger rotation.

  // Tree should be balanced
  EXPECT_EQ(tree.size(), 3);
  EXPECT_LE(tree.height(), 2); // Height measured in node levels (leaf = 1).
  EXPECT_TRUE(tree.is_balanced());
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(AVLTreeTest, RightRightRotation) {
  // Insert in ascending order to trigger RR rotation.
  tree.insert(10);
  tree.insert(20);
  tree.insert(30); // Should trigger rotation.

  EXPECT_EQ(tree.size(), 3);
  EXPECT_LE(tree.height(), 2);
  EXPECT_TRUE(tree.is_balanced());
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(AVLTreeTest, LeftRightRotation) {
  // LR case
  tree.insert(30);
  tree.insert(10);
  tree.insert(20); // Should trigger LR rotation.

  EXPECT_EQ(tree.size(), 3);
  EXPECT_LE(tree.height(), 2);
  EXPECT_TRUE(tree.is_balanced());
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(AVLTreeTest, RightLeftRotation) {
  // RL case
  tree.insert(10);
  tree.insert(30);
  tree.insert(20); // Should trigger RL rotation.

  EXPECT_EQ(tree.size(), 3);
  EXPECT_LE(tree.height(), 2);
  EXPECT_TRUE(tree.is_balanced());
  EXPECT_TRUE(tree.validate_properties());
}

//===------------------------------ SEARCH TESTS -------------------------------===//

TEST_F(AVLTreeTest, ContainsElement) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  EXPECT_TRUE(tree.contains(50));
  EXPECT_TRUE(tree.contains(30));
  EXPECT_TRUE(tree.contains(70));
  EXPECT_FALSE(tree.contains(100));
}

TEST_F(AVLTreeTest, FindMinMax) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(80);

  EXPECT_EQ(tree.find_min(), 20);
  EXPECT_EQ(tree.find_max(), 80);
}

TEST_F(AVLTreeTest, FindMinMaxOnEmptyThrows) {
  EXPECT_THROW({ [[maybe_unused]] auto result = tree.find_min(); }, EmptyTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto result = tree.find_max(); }, EmptyTreeException);
}

//===------------------------------ REMOVAL TESTS ------------------------------===//

TEST_F(AVLTreeTest, RemoveLeafNode) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  EXPECT_TRUE(tree.remove(30));
  EXPECT_EQ(tree.size(), 2);
  EXPECT_FALSE(tree.contains(30));
}

TEST_F(AVLTreeTest, RemoveNodeWithChildren) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);

  EXPECT_TRUE(tree.remove(30));
  EXPECT_EQ(tree.size(), 4);
  EXPECT_FALSE(tree.contains(30));
  EXPECT_TRUE(tree.contains(20));
  EXPECT_TRUE(tree.contains(40));
}

TEST_F(AVLTreeTest, RemoveRoot) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  EXPECT_TRUE(tree.remove(50));
  EXPECT_EQ(tree.size(), 2);
  EXPECT_FALSE(tree.contains(50));
}

TEST_F(AVLTreeTest, RemoveNonExistent) {
  tree.insert(50);
  EXPECT_FALSE(tree.remove(100));
  EXPECT_EQ(tree.size(), 1);
}

TEST_F(AVLTreeTest, RemoveWithRebalancing) {
  // Build a tree that will need rebalancing after removal.
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);
  tree.insert(60);
  tree.insert(80);

  tree.remove(20);
  tree.remove(40);
  // Tree should still be balanced.

  EXPECT_LE(tree.height(), 3);
  EXPECT_TRUE(tree.is_balanced());
  EXPECT_TRUE(tree.validate_properties());
}

//===----------------------------- TRAVERSAL TESTS -----------------------------===//

TEST_F(AVLTreeTest, InOrderTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);

  std::vector<int> result;
  tree.in_order_traversal([&result](const int& val) -> void { result.push_back(val); });

  std::vector<int> expected = {20, 30, 40, 50, 70};
  EXPECT_EQ(result, expected);
}

//===----------------------------- ITERATOR TESTS ------------------------------===//

TEST_F(AVLTreeTest, IteratorTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  std::vector<int> actual;
  for (const auto& val : tree) {
    actual.push_back(val);
  }

  std::vector<int> expected = {30, 50, 70};
  EXPECT_EQ(actual, expected);
}

TEST_F(AVLTreeTest, PostfixIteratorReturnsPreviousValue) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  auto it       = tree.begin();
  auto previous = it++;

  EXPECT_EQ(*previous, 30);
  EXPECT_EQ(*it, 50);
}

TEST_F(AVLTreeTest, EmptyIteratorsCompareEqual) {
  EXPECT_EQ(tree.begin(), tree.end());
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(AVLTreeTest, MoveConstructor) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  AVLTree<int> moved_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(moved_tree.size(), 3);
  EXPECT_TRUE(moved_tree.contains(50));
}

TEST_F(AVLTreeTest, MoveAssignment) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  AVLTree<int> other_tree;
  other_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(other_tree.size(), 3);
}

//===------------------------------ BALANCE TESTS ------------------------------===//

TEST_F(AVLTreeTest, BalanceAfterMultipleInsertions) {
  // Insert many elements.
  for (int i = 1; i <= 100; ++i) {
    tree.insert(i);
    ASSERT_TRUE(tree.validate_properties()) << "after inserting " << i;
  }

  EXPECT_EQ(tree.size(), 100);
  // For a balanced AVL tree with 100 nodes, height should be around log2(100) ~ 7.
  EXPECT_LE(tree.height(), 10);
  EXPECT_TRUE(tree.is_balanced());
}

TEST_F(AVLTreeTest, BalanceAfterRandomInsertions) {
  std::vector<int> values = {50, 25, 75, 10, 30, 60, 90, 5, 15, 27, 35, 55, 65, 85, 95};
  for (int val : values) {
    tree.insert(val);
    ASSERT_TRUE(tree.validate_properties()) << "after inserting " << val;
  }

  EXPECT_EQ(tree.size(), 15);
  // Height should be reasonable for balanced tree.
  EXPECT_LE(tree.height(), 5);
  EXPECT_TRUE(tree.is_balanced());
}

TEST_F(AVLTreeTest, ValidatePropertiesAfterEveryRemoval) {
  for (int value = 1; value <= 50; ++value) {
    ASSERT_TRUE(tree.insert(value));
  }

  for (int value = 1; value <= 50; value += 2) {
    ASSERT_TRUE(tree.remove(value));
    ASSERT_TRUE(tree.validate_properties()) << "after removing " << value;
  }
}

TEST_F(AVLTreeTest, RandomizedOperationsMatchStdSet) {
  std::mt19937                    rng(0xA71);
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

TEST(AVLTreeApiTest, FindReturnsConstPointer) {
  static_assert(std::is_same_v<decltype(std::declval<AVLTree<int>&>().find(std::declval<const int&>())), const int*>);
  SUCCEED();
}

//===---------------------------------------------------------------------------===//
