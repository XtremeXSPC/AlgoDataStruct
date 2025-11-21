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

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/trees/Red_Black_Tree.hpp"

using namespace ads::trees;

// Test fixture for RedBlackTree
class RedBlackTreeTest : public ::testing::Test {
protected:
  RedBlackTree<int> tree;
};

// ----- Basic State Tests ----- //

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

// ----- Insertion Tests ----- //

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

// ----- Search Tests ----- //

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
  EXPECT_THROW(tree.find_min(), EmptyTreeException);
  EXPECT_THROW(tree.find_max(), EmptyTreeException);
}

// ----- Removal Tests ----- //

TEST_F(RedBlackTreeTest, RemoveLeafNode) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  EXPECT_TRUE(tree.remove(30));
  EXPECT_EQ(tree.size(), 2);
  EXPECT_FALSE(tree.contains(30));
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
  EXPECT_TRUE(tree.contains(20));
  EXPECT_TRUE(tree.contains(40));
}

TEST_F(RedBlackTreeTest, RemoveRoot) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  EXPECT_TRUE(tree.remove(50));
  EXPECT_EQ(tree.size(), 2);
  EXPECT_FALSE(tree.contains(50));
}

TEST_F(RedBlackTreeTest, RemoveNonExistent) {
  tree.insert(50);
  EXPECT_FALSE(tree.remove(100));
  EXPECT_EQ(tree.size(), 1);
}

// ----- Traversal Tests ----- //

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

  std::vector<int> result;
  tree.pre_order_traversal([&result](const int& val) { result.push_back(val); });

  EXPECT_EQ(result.size(), 3);
}

TEST_F(RedBlackTreeTest, LevelOrderTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  std::vector<int> result;
  tree.level_order_traversal([&result](const int& val) { result.push_back(val); });

  EXPECT_EQ(result.size(), 3);
}

// ----- Iterator Tests ----- //

TEST_F(RedBlackTreeTest, IteratorTraversal) {
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

TEST_F(RedBlackTreeTest, MoveConstructor) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  RedBlackTree<int> moved_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(moved_tree.size(), 3);
  EXPECT_TRUE(moved_tree.contains(50));
}

TEST_F(RedBlackTreeTest, MoveAssignment) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  RedBlackTree<int> other_tree;
  other_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(other_tree.size(), 3);
}

// ----- Balance Tests ----- //

TEST_F(RedBlackTreeTest, BalanceAfterAscendingInsertions) {
  // Insert in ascending order
  for (int i = 1; i <= 50; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 50);
  // Red-Black tree should be balanced
  EXPECT_LE(tree.height(), 12); // 2*log2(50+1) ~ 12
}

TEST_F(RedBlackTreeTest, BalanceAfterDescendingInsertions) {
  // Insert in descending order
  for (int i = 50; i >= 1; --i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 50);
  EXPECT_LE(tree.height(), 12);
}

TEST_F(RedBlackTreeTest, BalanceAfterMultipleOperations) {
  // Insert elements
  for (int i = 1; i <= 100; ++i) {
    tree.insert(i);
  }

  // Remove some elements
  for (int i = 1; i <= 50; i += 2) {
    tree.remove(i);
  }

  EXPECT_EQ(tree.size(), 75);
  // Tree should still be balanced
  EXPECT_LE(tree.height(), 15);
}

// ----- Edge Cases ----- //

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

// ----- Custom Type Tests ----- //

TEST(RedBlackTreeCustomTypeTest, StringKeys) {
  RedBlackTree<std::string> str_tree;

  str_tree.insert("banana");
  str_tree.insert("apple");
  str_tree.insert("cherry");

  EXPECT_EQ(str_tree.size(), 3);
  EXPECT_EQ(str_tree.find_min(), "apple");
  EXPECT_EQ(str_tree.find_max(), "cherry");
}
