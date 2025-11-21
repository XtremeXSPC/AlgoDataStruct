//===--------------------------------------------------------------------------===//
/**
 * @file Test_BTree.cpp
 * @brief Google Test unit tests for B-Tree implementation
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/trees/B_Tree.hpp"

using namespace ads::trees;

// Test fixture for BTree
class BTreeTest : public ::testing::Test {
protected:
  BTree<int> tree{3}; // B-Tree with minimum degree 3 (max 5 keys per node)
};

// ----- Basic State Tests ----- //

TEST_F(BTreeTest, IsEmptyOnConstruction) {
  EXPECT_EQ(tree.size(), 0);
  EXPECT_TRUE(tree.is_empty());
}

TEST_F(BTreeTest, Clear) {
  tree.insert(10);
  tree.insert(20);
  tree.insert(30);
  EXPECT_FALSE(tree.is_empty());

  tree.clear();
  EXPECT_EQ(tree.size(), 0);
  EXPECT_TRUE(tree.is_empty());
}

// ----- Insertion Tests ----- //

TEST_F(BTreeTest, InsertSingleElement) {
  tree.insert(50);
  EXPECT_EQ(tree.size(), 1);
  EXPECT_TRUE(tree.contains(50));
}

TEST_F(BTreeTest, InsertMultipleElements) {
  for (int i = 1; i <= 10; ++i) {
    tree.insert(i * 10);
  }

  EXPECT_EQ(tree.size(), 10);
  for (int i = 1; i <= 10; ++i) {
    EXPECT_TRUE(tree.contains(i * 10));
  }
}

TEST_F(BTreeTest, InsertWithSplit) {
  // Insert enough elements to cause node splits
  for (int i = 1; i <= 20; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 20);
  for (int i = 1; i <= 20; ++i) {
    EXPECT_TRUE(tree.contains(i));
  }
}

TEST_F(BTreeTest, InsertDuplicateRejected) {
  tree.insert(50);
  EXPECT_FALSE(tree.insert(50));
  EXPECT_EQ(tree.size(), 1);
}

// ----- Search Tests ----- //

TEST_F(BTreeTest, ContainsElement) {
  tree.insert(10);
  tree.insert(20);
  tree.insert(30);
  tree.insert(40);
  tree.insert(50);

  EXPECT_TRUE(tree.contains(10));
  EXPECT_TRUE(tree.contains(30));
  EXPECT_TRUE(tree.contains(50));
  EXPECT_FALSE(tree.contains(25));
  EXPECT_FALSE(tree.contains(100));
}

TEST_F(BTreeTest, FindMinMax) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(80);
  tree.insert(10);
  tree.insert(90);

  EXPECT_EQ(tree.find_min(), 10);
  EXPECT_EQ(tree.find_max(), 90);
}

TEST_F(BTreeTest, FindMinMaxOnEmptyThrows) {
  EXPECT_THROW(tree.find_min(), EmptyTreeException);
  EXPECT_THROW(tree.find_max(), EmptyTreeException);
}

// ----- Removal Tests ----- //

TEST_F(BTreeTest, RemoveFromLeaf) {
  tree.insert(10);
  tree.insert(20);
  tree.insert(30);
  tree.insert(40);
  tree.insert(50);

  EXPECT_TRUE(tree.remove(30));
  EXPECT_EQ(tree.size(), 4);
  EXPECT_FALSE(tree.contains(30));
}

TEST_F(BTreeTest, RemoveFromInternalNode) {
  for (int i = 1; i <= 15; ++i) {
    tree.insert(i * 10);
  }

  // Remove an element that might be in an internal node
  EXPECT_TRUE(tree.remove(50));
  EXPECT_FALSE(tree.contains(50));
}

TEST_F(BTreeTest, RemoveNonExistent) {
  tree.insert(10);
  tree.insert(20);

  EXPECT_FALSE(tree.remove(30));
  EXPECT_EQ(tree.size(), 2);
}

TEST_F(BTreeTest, RemoveWithMerge) {
  // Insert and remove elements to trigger merge operations
  for (int i = 1; i <= 10; ++i) {
    tree.insert(i);
  }

  for (int i = 1; i <= 5; ++i) {
    tree.remove(i);
  }

  EXPECT_EQ(tree.size(), 5);
  for (int i = 6; i <= 10; ++i) {
    EXPECT_TRUE(tree.contains(i));
  }
}

TEST_F(BTreeTest, RemoveAll) {
  for (int i = 1; i <= 10; ++i) {
    tree.insert(i);
  }

  for (int i = 1; i <= 10; ++i) {
    tree.remove(i);
  }

  EXPECT_TRUE(tree.is_empty());
}

// ----- Traversal Tests ----- //

TEST_F(BTreeTest, InOrderTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);
  tree.insert(20);
  tree.insert(40);
  tree.insert(60);
  tree.insert(80);

  std::vector<int> result;
  tree.in_order_traversal([&result](const int& val) { result.push_back(val); });

  std::vector<int> expected = {20, 30, 40, 50, 60, 70, 80};
  EXPECT_EQ(result, expected);
}

// ----- Iterator Tests ----- //

TEST_F(BTreeTest, IteratorTraversal) {
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

TEST_F(BTreeTest, MoveConstructor) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  BTree<int> moved_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(moved_tree.size(), 3);
  EXPECT_TRUE(moved_tree.contains(50));
}

TEST_F(BTreeTest, MoveAssignment) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  BTree<int> other_tree{3};
  other_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(other_tree.size(), 3);
}

// ----- Large Dataset Tests ----- //

TEST_F(BTreeTest, LargeDatasetInsert) {
  const int N = 1000;
  for (int i = 0; i < N; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), N);
  EXPECT_EQ(tree.find_min(), 0);
  EXPECT_EQ(tree.find_max(), N - 1);
}

TEST_F(BTreeTest, LargeDatasetInsertAndRemove) {
  const int N = 500;

  // Insert
  for (int i = 0; i < N; ++i) {
    tree.insert(i);
  }

  // Remove half
  for (int i = 0; i < N; i += 2) {
    tree.remove(i);
  }

  EXPECT_EQ(tree.size(), N / 2);

  // Verify remaining elements
  for (int i = 0; i < N; ++i) {
    if (i % 2 == 0) {
      EXPECT_FALSE(tree.contains(i));
    } else {
      EXPECT_TRUE(tree.contains(i));
    }
  }
}

// ----- Different Degree Tests ----- //

TEST(BTreeDegreeTest, MinimumDegree2) {
  BTree<int> tree{2}; // Minimum degree 2 (2-3-4 tree behavior)

  for (int i = 1; i <= 20; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 20);
  for (int i = 1; i <= 20; ++i) {
    EXPECT_TRUE(tree.contains(i));
  }
}

TEST(BTreeDegreeTest, LargerDegree) {
  BTree<int> tree{5}; // Minimum degree 5

  for (int i = 1; i <= 100; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 100);
  // Height should be very small due to high branching factor
  EXPECT_LE(tree.height(), 3);
}

// ----- String Type Tests ----- //

TEST(BTreeStringTest, StringKeys) {
  BTree<std::string> tree{3};

  tree.insert("banana");
  tree.insert("apple");
  tree.insert("cherry");
  tree.insert("date");
  tree.insert("elderberry");

  EXPECT_EQ(tree.size(), 5);
  EXPECT_EQ(tree.find_min(), "apple");
  EXPECT_EQ(tree.find_max(), "elderberry");
}
