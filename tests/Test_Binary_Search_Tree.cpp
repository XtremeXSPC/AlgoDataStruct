//===---------------------------------------------------------------------------===//
/**
 * @file Test_BinarySearchTree.cpp
 * @brief Google Test unit tests for BinarySearchTree.
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/trees/Binary_Search_Tree.hpp"

using namespace ads::trees;

// Test fixture for BinarySearchTree.
class BinarySearchTreeTest : public ::testing::Test {
protected:
  BinarySearchTree<int> tree;

  void SetUp() override {
    // Common setup: create a balanced tree.
    //        50
    //       /  \
    //      30   70
    //     / \   / \
    //    20 40 60 80
  }

  void buildBalancedTree() {
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);
    tree.insert(60);
    tree.insert(80);
  }
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(BinarySearchTreeTest, IsEmptyOnConstruction) {
  EXPECT_EQ(tree.size(), 0);
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.height(), -1);
}

TEST_F(BinarySearchTreeTest, Clear) {
  buildBalancedTree();
  EXPECT_FALSE(tree.is_empty());

  tree.clear();
  EXPECT_EQ(tree.size(), 0);
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.height(), -1);
}

//===----------------------------- INSERTION TESTS -----------------------------===//

TEST_F(BinarySearchTreeTest, InsertSingleElement) {
  EXPECT_TRUE(tree.insert(50));
  EXPECT_EQ(tree.size(), 1);
  EXPECT_EQ(tree.height(), 0);
  EXPECT_TRUE(tree.contains(50));
}

TEST_F(BinarySearchTreeTest, InsertMultipleElements) {
  buildBalancedTree();
  EXPECT_EQ(tree.size(), 7);
  EXPECT_EQ(tree.height(), 2);
}

TEST_F(BinarySearchTreeTest, InsertDuplicateRejected) {
  tree.insert(50);
  EXPECT_FALSE(tree.insert(50));
  EXPECT_EQ(tree.size(), 1);
}

TEST_F(BinarySearchTreeTest, EmplaceElement) {
  tree.emplace(42);
  EXPECT_EQ(tree.size(), 1);
  EXPECT_TRUE(tree.contains(42));
}

//===------------------------------ SEARCH TESTS -------------------------------===//

TEST_F(BinarySearchTreeTest, ContainsElement) {
  buildBalancedTree();

  EXPECT_TRUE(tree.contains(50));
  EXPECT_TRUE(tree.contains(20));
  EXPECT_TRUE(tree.contains(80));
  EXPECT_FALSE(tree.contains(15));
  EXPECT_FALSE(tree.contains(100));
}

TEST_F(BinarySearchTreeTest, FindMinMax) {
  buildBalancedTree();

  EXPECT_EQ(tree.find_min(), 20);
  EXPECT_EQ(tree.find_max(), 80);
}

TEST_F(BinarySearchTreeTest, FindMinMaxOnEmptyThrows) {
  EXPECT_THROW({ [[maybe_unused]] auto val = tree.find_min(); }, EmptyTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto val = tree.find_max(); }, EmptyTreeException);
}

TEST_F(BinarySearchTreeTest, SuccessorPredecessor) {
  buildBalancedTree();

  const int* succ = tree.successor(40);
  ASSERT_NE(succ, nullptr);
  EXPECT_EQ(*succ, 50);

  const int* pred = tree.predecessor(40);
  ASSERT_NE(pred, nullptr);
  EXPECT_EQ(*pred, 30);

  // Test edge cases
  EXPECT_EQ(tree.successor(80), nullptr);   // No successor for max
  EXPECT_EQ(tree.predecessor(20), nullptr); // No predecessor for min
}

//===------------------------------ REMOVAL TESTS ------------------------------===//

TEST_F(BinarySearchTreeTest, RemoveLeafNode) {
  buildBalancedTree();
  EXPECT_TRUE(tree.remove(20));
  EXPECT_EQ(tree.size(), 6);
  EXPECT_FALSE(tree.contains(20));
}

TEST_F(BinarySearchTreeTest, RemoveNodeWithOneChild) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(20);

  EXPECT_TRUE(tree.remove(30));
  EXPECT_EQ(tree.size(), 2);
  EXPECT_FALSE(tree.contains(30));
  EXPECT_TRUE(tree.contains(20));
}

TEST_F(BinarySearchTreeTest, RemoveNodeWithTwoChildren) {
  buildBalancedTree();
  EXPECT_TRUE(tree.remove(30));
  EXPECT_EQ(tree.size(), 6);
  EXPECT_FALSE(tree.contains(30));
  EXPECT_TRUE(tree.contains(20));
  EXPECT_TRUE(tree.contains(40));
}

TEST_F(BinarySearchTreeTest, RemoveRoot) {
  buildBalancedTree();
  EXPECT_TRUE(tree.remove(50));
  EXPECT_EQ(tree.size(), 6);
  EXPECT_FALSE(tree.contains(50));
}

TEST_F(BinarySearchTreeTest, RemoveNonExistent) {
  buildBalancedTree();
  EXPECT_FALSE(tree.remove(100));
  EXPECT_EQ(tree.size(), 7);
}

//===----------------------------- TRAVERSAL TESTS -----------------------------===//

TEST_F(BinarySearchTreeTest, InOrderTraversal) {
  buildBalancedTree();

  std::vector<int> result;
  tree.in_order_traversal([&result](const int& val) { result.push_back(val); });

  std::vector<int> expected = {20, 30, 40, 50, 60, 70, 80};
  EXPECT_EQ(result, expected);
}

TEST_F(BinarySearchTreeTest, PreOrderTraversal) {
  buildBalancedTree();

  std::vector<int> result;
  tree.pre_order_traversal([&result](const int& val) { result.push_back(val); });

  std::vector<int> expected = {50, 30, 20, 40, 70, 60, 80};
  EXPECT_EQ(result, expected);
}

TEST_F(BinarySearchTreeTest, PostOrderTraversal) {
  buildBalancedTree();

  std::vector<int> result;
  tree.post_order_traversal([&result](const int& val) { result.push_back(val); });

  std::vector<int> expected = {20, 40, 30, 60, 80, 70, 50};
  EXPECT_EQ(result, expected);
}

TEST_F(BinarySearchTreeTest, LevelOrderTraversal) {
  buildBalancedTree();

  std::vector<int> result;
  tree.level_order_traversal([&result](const int& val) { result.push_back(val); });

  std::vector<int> expected = {50, 30, 70, 20, 40, 60, 80};
  EXPECT_EQ(result, expected);
}

//===----------------------------- ITERATOR TESTS ------------------------------===//

TEST_F(BinarySearchTreeTest, IteratorTraversal) {
  buildBalancedTree();

  std::vector<int> actual;
  for (const auto& val : tree) {
    actual.push_back(val);
  }

  std::vector<int> expected = {20, 30, 40, 50, 60, 70, 80};
  EXPECT_EQ(actual, expected);
}

TEST_F(BinarySearchTreeTest, ExplicitIterator) {
  buildBalancedTree();

  auto it = tree.begin();
  EXPECT_EQ(*it, 20);
  ++it;
  EXPECT_EQ(*it, 30);
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(BinarySearchTreeTest, MoveConstructor) {
  buildBalancedTree();

  BinarySearchTree<int> moved_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(moved_tree.size(), 7);
  EXPECT_TRUE(moved_tree.contains(50));
}

TEST_F(BinarySearchTreeTest, MoveAssignment) {
  buildBalancedTree();

  BinarySearchTree<int> other_tree;
  other_tree.insert(100);

  other_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(other_tree.size(), 7);
  EXPECT_FALSE(other_tree.contains(100));
  EXPECT_TRUE(other_tree.contains(50));
}

//===----------------------------- EDGE CASE TESTS -----------------------------===//

TEST_F(BinarySearchTreeTest, SingleElementTree) {
  tree.insert(42);

  EXPECT_EQ(tree.size(), 1);
  EXPECT_EQ(tree.height(), 0);
  EXPECT_EQ(tree.find_min(), 42);
  EXPECT_EQ(tree.find_max(), 42);
}

TEST_F(BinarySearchTreeTest, DegenerateTree) {
  // Create a degenerate tree (linked list).
  for (int i = 1; i <= 5; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 5);
  EXPECT_EQ(tree.height(), 4); // Height equals size - 1.
}

//===---------------------------- CUSTOM TYPE TESTS ----------------------------===//

TEST(BinarySearchTreeCustomTypeTest, CustomComparison) {
  struct Person {
    std::string name;
    int         age;

    bool operator<(const Person& other) const { return age < other.age; }
    bool operator==(const Person& other) const { return age == other.age; }
  };

  BinarySearchTree<Person> people_tree;
  people_tree.emplace("Alice", 30);
  people_tree.emplace("Bob", 25);
  people_tree.emplace("Charlie", 35);

  EXPECT_EQ(people_tree.size(), 3);
  EXPECT_EQ(people_tree.find_min().name, "Bob");
  EXPECT_EQ(people_tree.find_max().name, "Charlie");
}

//===---------------------------------------------------------------------------===//
