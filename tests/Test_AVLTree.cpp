//===--------------------------------------------------------------------------===//
/**
 * @file Test_AVLTree.cpp
 * @brief Google Test unit tests for AVL Tree implementation
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/trees/AVL_Tree.hpp"

using namespace ads::trees;

// Test fixture for AVLTree
class AVLTreeTest : public ::testing::Test {
protected:
  AVLTree<int> tree;
};

// ----- Basic State Tests ----- //

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

// ----- Insertion Tests ----- //

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

// ----- Rotation Tests (Balance) ----- //

TEST_F(AVLTreeTest, LeftLeftRotation) {
  // Insert in descending order to trigger LL rotation
  tree.insert(30);
  tree.insert(20);
  tree.insert(10); // Should trigger rotation

  // Tree should be balanced
  EXPECT_EQ(tree.size(), 3);
  EXPECT_LE(tree.height(), 1); // Height should be at most 1 for balanced tree
}

TEST_F(AVLTreeTest, RightRightRotation) {
  // Insert in ascending order to trigger RR rotation
  tree.insert(10);
  tree.insert(20);
  tree.insert(30); // Should trigger rotation

  EXPECT_EQ(tree.size(), 3);
  EXPECT_LE(tree.height(), 1);
}

TEST_F(AVLTreeTest, LeftRightRotation) {
  // LR case
  tree.insert(30);
  tree.insert(10);
  tree.insert(20); // Should trigger LR rotation

  EXPECT_EQ(tree.size(), 3);
  EXPECT_LE(tree.height(), 1);
}

TEST_F(AVLTreeTest, RightLeftRotation) {
  // RL case
  tree.insert(10);
  tree.insert(30);
  tree.insert(20); // Should trigger RL rotation

  EXPECT_EQ(tree.size(), 3);
  EXPECT_LE(tree.height(), 1);
}

// ----- Search Tests ----- //

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
  EXPECT_THROW(tree.find_min(), EmptyTreeException);
  EXPECT_THROW(tree.find_max(), EmptyTreeException);
}

// ----- Removal Tests ----- //

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
  // Build a tree that will need rebalancing after removal
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);
  tree.insert(60);
  tree.insert(80);

  tree.remove(20);
  tree.remove(40);
  // Tree should still be balanced

  EXPECT_LE(tree.height(), 2);
}

// ----- Traversal Tests ----- //

TEST_F(AVLTreeTest, InOrderTraversal) {
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

// ----- Iterator Tests ----- //

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

// ----- Move Semantics Tests ----- //

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

// ----- Balance Tests ----- //

TEST_F(AVLTreeTest, BalanceAfterMultipleInsertions) {
  // Insert many elements
  for (int i = 1; i <= 100; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 100);
  // For a balanced AVL tree with 100 nodes, height should be around log2(100) ~ 7
  EXPECT_LE(tree.height(), 10);
}

TEST_F(AVLTreeTest, BalanceAfterRandomInsertions) {
  std::vector<int> values = {50, 25, 75, 10, 30, 60, 90, 5, 15, 27, 35, 55, 65, 85, 95};
  for (int val : values) {
    tree.insert(val);
  }

  EXPECT_EQ(tree.size(), 15);
  // Height should be reasonable for balanced tree
  EXPECT_LE(tree.height(), 5);
}
