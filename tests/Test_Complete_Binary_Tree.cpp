//===---------------------------------------------------------------------------===//
/**
 * @file Test_Complete_Binary_Tree.cpp
 * @brief Google Test unit tests for CompleteBinaryTree.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/trees/Complete_Binary_Tree.hpp"

using namespace ads::trees;

// Test fixture for CompleteBinaryTree.
class CompleteBinaryTreeTest : public ::testing::Test {
protected:
  CompleteBinaryTree<int> tree;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(CompleteBinaryTreeTest, IsEmptyOnConstruction) {
  EXPECT_EQ(tree.size(), 0u);
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.height(), -1);
}

TEST_F(CompleteBinaryTreeTest, InsertAndRoot) {
  tree.insert(10);
  EXPECT_EQ(tree.size(), 1u);
  EXPECT_EQ(tree.root(), 10);
  EXPECT_EQ(tree.height(), 0);
}

TEST_F(CompleteBinaryTreeTest, InsertMultiple) {
  tree.insert(1);
  tree.insert(2);
  tree.insert(3);
  tree.insert(4);
  tree.insert(5);

  EXPECT_EQ(tree.size(), 5u);
  EXPECT_EQ(tree.root(), 1);
  EXPECT_EQ(tree.height(), 2);
}

TEST_F(CompleteBinaryTreeTest, InitializerListConstruction) {
  CompleteBinaryTree<int> t{1, 2, 3, 4, 5, 6, 7};

  EXPECT_EQ(t.size(), 7u);
  EXPECT_EQ(t.root(), 1);
  EXPECT_EQ(t.height(), 2);
}

TEST_F(CompleteBinaryTreeTest, ContainsOperation) {
  CompleteBinaryTree<int> t{10, 20, 30, 40, 50};

  EXPECT_TRUE(t.contains(10));
  EXPECT_TRUE(t.contains(30));
  EXPECT_TRUE(t.contains(50));
  EXPECT_FALSE(t.contains(25));
  EXPECT_FALSE(t.contains(100));
}

TEST_F(CompleteBinaryTreeTest, ContainsOnEmpty) {
  EXPECT_FALSE(tree.contains(42));
}

TEST_F(CompleteBinaryTreeTest, RootOnEmptyThrows) {
  EXPECT_THROW(tree.root(), EmptyTreeException);
}

TEST_F(CompleteBinaryTreeTest, LevelOrderTraversal) {
  CompleteBinaryTree<int> t{1, 2, 3, 4, 5, 6, 7};

  std::vector<int> result;
  t.level_order_traversal([&result](int val) { result.push_back(val); });

  std::vector<int> expected{1, 2, 3, 4, 5, 6, 7};
  EXPECT_EQ(result, expected);
}

TEST_F(CompleteBinaryTreeTest, PreOrderTraversal) {
  CompleteBinaryTree<int> t{1, 2, 3, 4, 5, 6, 7};

  std::vector<int> result;
  t.pre_order_traversal([&result](int val) { result.push_back(val); });

  // Pre-order: root, left, right.
  // Tree:     1
  //         /   \
  //        2     3
  //       / \   / \
  //      4   5 6   7
  std::vector<int> expected{1, 2, 4, 5, 3, 6, 7};
  EXPECT_EQ(result, expected);
}

TEST_F(CompleteBinaryTreeTest, InOrderTraversal) {
  CompleteBinaryTree<int> t{1, 2, 3, 4, 5, 6, 7};

  std::vector<int> result;
  t.in_order_traversal([&result](int val) { result.push_back(val); });

  // In-order: left, root, right.
  std::vector<int> expected{4, 2, 5, 1, 6, 3, 7};
  EXPECT_EQ(result, expected);
}

TEST_F(CompleteBinaryTreeTest, PostOrderTraversal) {
  CompleteBinaryTree<int> t{1, 2, 3, 4, 5, 6, 7};

  std::vector<int> result;
  t.post_order_traversal([&result](int val) { result.push_back(val); });

  // Post-order: left, right, root.
  std::vector<int> expected{4, 5, 2, 6, 7, 3, 1};
  EXPECT_EQ(result, expected);
}

TEST_F(CompleteBinaryTreeTest, ToVector) {
  CompleteBinaryTree<int> t{1, 2, 3, 4, 5};

  auto             vec = t.to_vector();
  std::vector<int> expected{1, 2, 3, 4, 5};
  EXPECT_EQ(vec, expected);
}

TEST_F(CompleteBinaryTreeTest, MoveSemantics) {
  tree.insert(10);
  tree.insert(20);

  CompleteBinaryTree<int> moved = std::move(tree);
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(moved.size(), 2u);

  tree = std::move(moved);
  EXPECT_TRUE(moved.is_empty());
  EXPECT_EQ(tree.size(), 2u);
}

TEST_F(CompleteBinaryTreeTest, ClearOperation) {
  tree.insert(1);
  tree.insert(2);
  tree.insert(3);

  tree.clear();
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0u);
}

TEST_F(CompleteBinaryTreeTest, EmplaceOperation) {
  CompleteBinaryTree<std::string> strings;
  strings.emplace("Hello");
  strings.emplace("World");

  EXPECT_EQ(strings.size(), 2u);
  EXPECT_EQ(strings.root(), "Hello");
}

TEST_F(CompleteBinaryTreeTest, HeightCalculation) {
  EXPECT_EQ(tree.height(), -1); // Empty.

  tree.insert(1);
  EXPECT_EQ(tree.height(), 0); // Root only.

  tree.insert(2);
  tree.insert(3);
  EXPECT_EQ(tree.height(), 1); // Two levels.

  tree.insert(4);
  tree.insert(5);
  tree.insert(6);
  tree.insert(7);
  EXPECT_EQ(tree.height(), 2); // Three levels (complete).

  tree.insert(8);
  EXPECT_EQ(tree.height(), 3); // Four levels.
}

TEST_F(CompleteBinaryTreeTest, RootNodeAccess) {
  tree.insert(42);
  auto* node = tree.root_node();
  EXPECT_NE(node, nullptr);
  EXPECT_EQ(node->data, 42);
  EXPECT_EQ(node->left, nullptr);
  EXPECT_EQ(node->right, nullptr);
}

//===---------------------------------------------------------------------------===//
