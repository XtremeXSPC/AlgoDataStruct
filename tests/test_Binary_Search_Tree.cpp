//===--------------------------------------------------------------------------===//
/**
 * @file test_Binary_Search_Tree.cpp
 * @brief Unit tests for BinarySearchTree (GoogleTest)
 */
//===--------------------------------------------------------------------------===//

#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <vector>

#include "../include/ads/trees/Binary_Search_Tree.hpp"

using ads::trees::BinarySearchTree;
using ads::trees::EmptyTreeException;
using std::string;
using std::vector;

template <typename T>
vector<T> collect_in_order(const BinarySearchTree<T>& tree) {
  vector<T> values;
  tree.in_order_traversal([&values](const T& value) -> void { values.push_back(value); });
  return values;
}

TEST(BinarySearchTreeTest, BasicOperations) {
  BinarySearchTree<int> bst;
  EXPECT_TRUE(bst.is_empty());
  EXPECT_EQ(bst.size(), 0);

  vector<int> values = {50, 30, 70, 20, 40, 60, 80};
  for (int v : values) {
    EXPECT_TRUE(bst.insert(v));
  }

  EXPECT_EQ(bst.size(), values.size());
  EXPECT_EQ(collect_in_order(bst), (vector<int>{20, 30, 40, 50, 60, 70, 80}));

  EXPECT_FALSE(bst.insert(40));
  EXPECT_EQ(bst.size(), values.size());
}

TEST(BinarySearchTreeTest, SearchOperations) {
  BinarySearchTree<int> bst;
  vector<int>           values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
  for (int v : values) {
    bst.insert(v);
  }

  EXPECT_TRUE(bst.contains(40));
  EXPECT_FALSE(bst.contains(55));
  EXPECT_TRUE(bst.contains(10));

  EXPECT_EQ(bst.find_min(), 10);
  EXPECT_EQ(bst.find_max(), 80);

  const int* succ = bst.successor(40);
  const int* pred = bst.predecessor(40);
  ASSERT_NE(succ, nullptr);
  ASSERT_NE(pred, nullptr);
  EXPECT_EQ(*succ, 45);
  EXPECT_EQ(*pred, 35);

  succ = bst.successor(10);
  pred = bst.predecessor(10);
  ASSERT_NE(succ, nullptr);
  EXPECT_EQ(*succ, 20);
  EXPECT_EQ(pred, nullptr);
}

TEST(BinarySearchTreeTest, RemovalOperations) {
  BinarySearchTree<int> bst;
  vector<int>           values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
  for (int v : values) {
    bst.insert(v);
  }

  EXPECT_TRUE(bst.remove(10));
  EXPECT_FALSE(bst.contains(10));

  EXPECT_TRUE(bst.remove(20));
  EXPECT_FALSE(bst.contains(20));

  EXPECT_TRUE(bst.remove(30));
  EXPECT_FALSE(bst.contains(30));

  EXPECT_TRUE(bst.remove(50));
  EXPECT_FALSE(bst.contains(50));

  EXPECT_EQ(collect_in_order(bst), (vector<int>{25, 35, 40, 45, 60, 70, 80}));
}

TEST(BinarySearchTreeTest, Iterators) {
  BinarySearchTree<int> bst;
  vector<int>           values = {50, 30, 70, 20, 40, 60, 80};
  for (int v : values) {
    bst.insert(v);
  }

  vector<int> from_range;
  for (int v : bst) {
    from_range.push_back(v);
  }

  vector<int> collected_explicit;
  for (auto it = bst.begin(); it != bst.end(); ++it) {
    collected_explicit.push_back(*it);
  }

  EXPECT_EQ(from_range, (vector<int>{20, 30, 40, 50, 60, 70, 80}));
  EXPECT_EQ(from_range, collected_explicit);
}

TEST(BinarySearchTreeTest, CustomTypes) {
  struct Person {
    string name;
    int    age;

    Person(string n, int a) : name(std::move(n)), age(a) {}
    auto operator<(const Person& other) const -> bool { return age < other.age; }
    auto operator==(const Person& other) const -> bool { return age == other.age; }
  };

  BinarySearchTree<Person> people;

  people.emplace("Alice", 30);
  people.emplace("Bob", 25);
  people.emplace("Charlie", 35);
  people.emplace("Diana", 28);
  people.emplace("Eve", 32);

  EXPECT_EQ(people.find_min().age, 25);
  EXPECT_EQ(people.find_max().age, 35);

  vector<string> names;
  people.in_order_traversal([&names](const Person& p) -> void { names.push_back(p.name); });

  EXPECT_EQ(names, (vector<string>{"Bob", "Diana", "Alice", "Eve", "Charlie"}));
}

TEST(BinarySearchTreeTest, MoveSemantics) {
  BinarySearchTree<int> bst1;
  bst1.insert(50);
  bst1.insert(30);
  bst1.insert(70);

  BinarySearchTree<int> bst2 = std::move(bst1);
  EXPECT_EQ(bst1.size(), 0);
  EXPECT_EQ(bst2.size(), 3);
  EXPECT_TRUE(bst2.contains(50));

  BinarySearchTree<int> bst3;
  bst3.insert(10);
  bst3.insert(20);
  bst3 = std::move(bst2);

  EXPECT_EQ(bst2.size(), 0);
  EXPECT_EQ(bst3.size(), 3);
  EXPECT_TRUE(bst3.contains(70));
}

TEST(BinarySearchTreeTest, Exceptions) {
  BinarySearchTree<int> empty_tree;

  EXPECT_THROW({ [[maybe_unused]] auto x = empty_tree.find_min(); }, EmptyTreeException);
  EXPECT_THROW({ [[maybe_unused]] auto x = empty_tree.find_max(); }, EmptyTreeException);
}

TEST(BinarySearchTreeTest, EdgeCases) {
  BinarySearchTree<int> single;
  single.insert(42);
  EXPECT_EQ(single.size(), 1);
  EXPECT_EQ(single.height(), 0);
  EXPECT_EQ(single.find_min(), 42);
  EXPECT_EQ(single.find_max(), 42);

  BinarySearchTree<int> degenerate;
  for (int i = 1; i <= 5; ++i) {
    degenerate.insert(i);
  }
  EXPECT_EQ(degenerate.height(), static_cast<int>(degenerate.size()) - 1);

  degenerate.clear();
  EXPECT_TRUE(degenerate.is_empty());

  degenerate.insert(10);
  degenerate.insert(5);
  degenerate.insert(15);
  EXPECT_EQ(degenerate.size(), 3);
  EXPECT_EQ(degenerate.height(), 1);
}

