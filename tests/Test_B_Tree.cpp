//===---------------------------------------------------------------------------===//
/**
 * @file Test_BTree.cpp
 * @brief Google Test unit tests for B-Tree implementation.
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/trees/B_Tree.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ads::trees;

template <typename T, int Degree = 3>
using BTreeType = B_Tree<T, Degree>;

// Test fixture for BTree.
class BTreeTest : public ::testing::Test {
protected:
  BTreeType<int> tree; // B-Tree with minimum degree 3 (max 5 keys per node).
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

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

//===----------------------------- INSERTION TESTS -----------------------------===//

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
  // Insert enough elements to cause node splits.
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

//===------------------------------ SEARCH TESTS -------------------------------===//

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
  EXPECT_THROW([[maybe_unused]] auto val = tree.find_min(), EmptyTreeException);
  EXPECT_THROW([[maybe_unused]] auto val = tree.find_max(), EmptyTreeException);
}

//===------------------------------ REMOVAL TESTS ------------------------------===//

TEST_F(BTreeTest, RemoveFromLeaf) {
  tree.insert(10);
  tree.insert(20);
  tree.insert(30);
  tree.insert(40);

  EXPECT_TRUE(tree.remove(40));
  EXPECT_EQ(tree.size(), 3);
  EXPECT_FALSE(tree.contains(40));
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(BTreeTest, RemoveFromInternalNode) {
  for (int i = 1; i <= 30; ++i) {
    tree.insert(i);
  }

  EXPECT_TRUE(tree.remove(15));
  EXPECT_EQ(tree.size(), 29);
  EXPECT_FALSE(tree.contains(15));
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(BTreeTest, RemoveNonExistent) {
  for (int i = 1; i <= 20; ++i) {
    tree.insert(i);
  }

  EXPECT_FALSE(tree.remove(999));
  EXPECT_EQ(tree.size(), 20);
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(BTreeTest, RemoveWithMerge) {
  for (int i = 1; i <= 40; ++i) {
    tree.insert(i);
  }

  EXPECT_TRUE(tree.remove(10));
  EXPECT_TRUE(tree.remove(11));
  EXPECT_TRUE(tree.remove(12));
  EXPECT_TRUE(tree.remove(13));
  EXPECT_TRUE(tree.remove(14));

  EXPECT_EQ(tree.size(), 35);
  EXPECT_FALSE(tree.contains(10));
  EXPECT_FALSE(tree.contains(14));
  EXPECT_TRUE(tree.validate_properties());
}

TEST_F(BTreeTest, RemoveAll) {
  for (int i = 1; i <= 20; ++i) {
    tree.insert(i);
  }

  for (int i = 1; i <= 20; ++i) {
    EXPECT_TRUE(tree.remove(i));
  }

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0);
  EXPECT_TRUE(tree.validate_properties());
}

//===----------------------------- TRAVERSAL TESTS -----------------------------===//

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

//===------------------------------ ITERATOR TESTS ------------------------------===//

TEST_F(BTreeTest, IteratorTraversal) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  std::vector<int> actual;
  tree.in_order_traversal([&actual](const int& val) { actual.push_back(val); });

  std::vector<int> expected = {30, 50, 70};
  EXPECT_EQ(actual, expected);
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(BTreeTest, MoveConstructor) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  BTreeType<int> moved_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(moved_tree.size(), 3);
  EXPECT_TRUE(moved_tree.contains(50));
}

TEST_F(BTreeTest, MoveAssignment) {
  tree.insert(50);
  tree.insert(30);
  tree.insert(70);

  BTreeType<int> other_tree;
  other_tree = std::move(tree);

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(other_tree.size(), 3);
}

//===--------------------------- LARGE DATASET TESTS ---------------------------===//

TEST_F(BTreeTest, LargeDatasetInsert) {
  const int N = 1'000;
  for (int i = 0; i < N; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), N);
  EXPECT_EQ(tree.find_min(), 0);
  EXPECT_EQ(tree.find_max(), N - 1);
}

TEST_F(BTreeTest, LargeDatasetInsertAndRemove) {
  const int N = 1'000;
  for (int i = 0; i < N; ++i) {
    tree.insert(i);
  }

  for (int i = 0; i < N; i += 2) {
    EXPECT_TRUE(tree.remove(i));
  }

  EXPECT_EQ(tree.size(), N / 2);
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.find_min(), 1);
  EXPECT_EQ(tree.find_max(), N - 1);

  for (int i = 1; i < N; i += 2) {
    EXPECT_TRUE(tree.contains(i));
  }
}

TEST(BTreeDeletionTest, DegreeTwoMixedDeletionMaintainsInvariants) {
  BTreeType<int, 2> tree;
  constexpr int     kElementCount = 90;

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

TEST(BTreeDeletionTest, LargerDegreeMixedDeletionMaintainsInvariants) {
  BTreeType<int, 5> tree;
  constexpr int     kElementCount = 150;

  for (int i = 1; i <= kElementCount; ++i) {
    ASSERT_TRUE(tree.insert(i));
  }

  for (int value = 75; value >= 1; value -= 2) {
    ASSERT_TRUE(tree.remove(value)) << "value: " << value;
    EXPECT_TRUE(tree.validate_properties()) << "after removing: " << value;
  }
  for (int value = 76; value <= kElementCount; value += 2) {
    ASSERT_TRUE(tree.remove(value)) << "value: " << value;
    EXPECT_TRUE(tree.validate_properties()) << "after removing: " << value;
  }

  EXPECT_EQ(tree.size(), 74U);
  for (int value = 2; value <= 74; value += 2) {
    EXPECT_TRUE(tree.contains(value));
  }
  for (int value = 77; value <= kElementCount; value += 2) {
    EXPECT_TRUE(tree.contains(value));
  }
}

TEST(BTreeDeletionTest, ReinsertAfterMixedDeletionMaintainsTraversalOrder) {
  BTreeType<int, 3> tree;

  for (int value = 1; value <= 120; ++value) {
    ASSERT_TRUE(tree.insert(value));
  }

  for (int value = 15; value <= 90; value += 15) {
    ASSERT_TRUE(tree.remove(value)) << "removed value: " << value;
    ASSERT_TRUE(tree.validate_properties());
  }

  for (int value = 200; value <= 240; value += 10) {
    ASSERT_TRUE(tree.insert(value)) << "inserted value: " << value;
    ASSERT_TRUE(tree.validate_properties());
  }

  std::vector<int> actual;
  tree.in_order_traversal([&actual](const int& value) { actual.push_back(value); });

  std::vector<int> expected;
  for (int value = 1; value <= 120; ++value) {
    if (value % 15 != 0 || value > 90) {
      expected.push_back(value);
    }
  }
  for (int value = 200; value <= 240; value += 10) {
    expected.push_back(value);
  }

  EXPECT_EQ(actual, expected);
  EXPECT_TRUE(tree.validate_properties());
}

//===------------------------- DEGREE VARIATION TESTS --------------------------===//

TEST(BTreeDegreeTest, MinimumDegree2) {
  BTreeType<int, 2> tree; // Minimum degree 2 (2-3-4 tree behavior).

  for (int i = 1; i <= 20; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 20);
  for (int i = 1; i <= 20; ++i) {
    EXPECT_TRUE(tree.contains(i));
  }
}

TEST(BTreeDegreeTest, LargerDegree) {
  BTreeType<int, 5> tree; // Minimum degree 5.

  for (int i = 1; i <= 100; ++i) {
    tree.insert(i);
  }

  EXPECT_EQ(tree.size(), 100);
  // Height should be very small due to high branching factor.
  EXPECT_LE(tree.height(), 3);
}

//===---------------------------- CUSTOM TYPE TESTS ----------------------------===//

TEST(BTreeStringTest, StringKeys) {
  BTreeType<std::string> tree;

  tree.insert("banana");
  tree.insert("apple");
  tree.insert("cherry");
  tree.insert("date");
  tree.insert("elderberry");

  EXPECT_EQ(tree.size(), 5);
  EXPECT_EQ(tree.find_min(), "apple");
  EXPECT_EQ(tree.find_max(), "elderberry");
}

//===---------------------------------------------------------------------------===//
