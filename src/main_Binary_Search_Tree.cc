//===--------------------------------------------------------------------------===//
/**
 * @file main_Binary_Search_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive test program for Binary Search Tree implementation
 * @version 0.1
 * @date 2025-10-15
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//

#include <iostream>
#include <string>
#include <vector>

#include "../include/ads/trees/Binary_Search_Tree.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::to_string;
using std::vector;

// Helper function to print tree contents via in-order traversal
template <typename T>
void print_tree(const ads::tree::BinarySearchTree<T>& tree, const string& name) {
  cout << "Tree '" << name << "' (size: " << tree.size() << ", height: " << tree.height() << "):\n";

  if (tree.is_empty()) {
    cout << "  (empty)\n";
    return;
  }

  cout << "  In-order: ";
  tree.in_order_traversal([](const T& value) { cout << value << " "; });
  cout << '\n';
}

// Test basic insertion and traversals
void test_basic_operations() {
  cout << "\n========== Test: Basic Operations ==========\n";

  ads::tree::BinarySearchTree<int> bst;

  // Test insertion
  cout << "Inserting values: 50, 30, 70, 20, 40, 60, 80\n";
  bst.insert(50);
  bst.insert(30);
  bst.insert(70);
  bst.insert(20);
  bst.insert(40);
  bst.insert(60);
  bst.insert(80);

  print_tree(bst, "bst");

  // Test different traversals
  cout << "\nDifferent traversals:\n";

  cout << "  Pre-order:  ";
  bst.pre_order_traversal([](const int& value) { cout << value << " "; });
  cout << '\n';

  cout << "  Post-order: ";
  bst.post_order_traversal([](const int& value) { cout << value << " "; });
  cout << '\n';

  cout << "  Level-order: ";
  bst.level_order_traversal([](const int& value) { cout << value << " "; });
  cout << '\n';

  // Test duplicates
  cout << "\nTrying to insert duplicate (40): ";
  bool inserted = bst.insert(40);
  cout << (inserted ? "inserted" : "not inserted (correct behavior)") << '\n';
  print_tree(bst, "bst after duplicate attempt");
}

// Test search operations
void test_search_operations() {
  cout << "\n========== Test: Search Operations ==========\n";

  ads::tree::BinarySearchTree<int> bst;

  // Build a tree
  vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
  for (int val : values) {
    bst.insert(val);
  }

  print_tree(bst, "bst");

  // Test contains
  cout << "\nTesting contains():\n";
  cout << "  Contains 40? " << (bst.contains(40) ? "Yes" : "No") << '\n';
  cout << "  Contains 55? " << (bst.contains(55) ? "Yes" : "No") << '\n';
  cout << "  Contains 10? " << (bst.contains(10) ? "Yes" : "No") << '\n';

  // Test find_min and find_max
  cout << "\nMinimum value: " << bst.find_min() << '\n';
  cout << "Maximum value: " << bst.find_max() << '\n';

  // Test successor and predecessor
  cout << "\nTesting successor and predecessor:\n";
  int        test_val = 40;
  const int* succ     = bst.successor(test_val);
  const int* pred     = bst.predecessor(test_val);

  cout << "  Successor of " << test_val << ": " << (succ ? to_string(*succ) : "none") << '\n';
  cout << "  Predecessor of " << test_val << ": " << (pred ? to_string(*pred) : "none") << '\n';

  test_val = 10;
  succ     = bst.successor(test_val);
  pred     = bst.predecessor(test_val);
  cout << "  Successor of " << test_val << ": " << (succ ? to_string(*succ) : "none") << '\n';
  cout << "  Predecessor of " << test_val << ": " << (pred ? to_string(*pred) : "none") << '\n';
}

// Test removal operations
void test_removal() {
  cout << "\n========== Test: Removal Operations ==========\n";

  ads::tree::BinarySearchTree<int> bst;

  // Build a tree
  vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
  for (int val : values) {
    bst.insert(val);
  }

  print_tree(bst, "original tree");

  // Remove a leaf node
  cout << "\nRemoving leaf node (10):\n";
  bst.remove(10);
  print_tree(bst, "after removing 10");

  // Remove a node with one child
  cout << "\nRemoving node with one child (20):\n";
  bst.remove(20);
  print_tree(bst, "after removing 20");

  // Remove a node with two children
  cout << "\nRemoving node with two children (30):\n";
  bst.remove(30);
  print_tree(bst, "after removing 30");

  // Remove the root
  cout << "\nRemoving root node (50):\n";
  bst.remove(50);
  print_tree(bst, "after removing root");

  // Try to remove non-existent value
  cout << "\nTrying to remove non-existent value (100): ";
  bool removed = bst.remove(100);
  cout << (removed ? "removed" : "not found (correct behavior)") << '\n';
}

// Test iterator functionality
void test_iterators() {
  cout << "\n========== Test: Iterators ==========\n";

  ads::tree::BinarySearchTree<int> bst;

  // Build a tree
  vector<int> values = {50, 30, 70, 20, 40, 60, 80};
  for (int val : values) {
    bst.insert(val);
  }

  // Test range-based for loop
  cout << "Using range-based for loop (in-order traversal):\n  ";
  for (const int& value : bst) {
    cout << value << " ";
  }
  cout << '\n';

  // Test explicit iterator usage
  cout << "\nUsing explicit iterators:\n  ";
  for (auto it = bst.begin(); it != bst.end(); ++it) {
    cout << *it << " ";
  }
  cout << '\n';

  // Test collecting values into a vector
  cout << "\nCollecting values into a vector:\n  ";
  vector<int> collected;
  for (const auto& val : bst) {
    collected.push_back(val);
  }
  cout << "Vector contents: ";
  for (int val : collected) {
    cout << val << " ";
  }
  cout << '\n';
}

// Test with custom types
void test_custom_types() {
  cout << "\n========== Test: Custom Types ==========\n";

  struct Person {
    string name;
    int    age;

    Person(string n, int a) : name(std::move(n)), age(a) {}

    // BST requires comparison operators
    bool operator<(const Person& other) const { return age < other.age; }

    bool operator==(const Person& other) const { return age == other.age; }
  };

  ads::tree::BinarySearchTree<Person> people;

  // Insert people
  cout << "Inserting people by age:\n";
  people.emplace("Alice", 30);
  people.emplace("Bob", 25);
  people.emplace("Charlie", 35);
  people.emplace("Diana", 28);
  people.emplace("Eve", 32);

  cout << "\nPeople sorted by age (in-order traversal):\n";
  people.in_order_traversal([](const Person& p) { cout << "  " << p.name << " (age " << p.age << ")\n"; });

  cout << "\nYoungest person: " << people.find_min().name << " (age " << people.find_min().age << ")\n";
  cout << "Oldest person: " << people.find_max().name << " (age " << people.find_max().age << ")\n";
}

// Test move semantics
void test_move_semantics() {
  cout << "\n========== Test: Move Semantics ==========\n";

  ads::tree::BinarySearchTree<int> bst1;
  bst1.insert(50);
  bst1.insert(30);
  bst1.insert(70);

  cout << "Original tree:\n";
  print_tree(bst1, "bst1");

  // Test move constructor
  ads::tree::BinarySearchTree<int> bst2 = std::move(bst1);

  cout << "\nAfter move construction:\n";
  print_tree(bst1, "bst1 (should be empty)");
  print_tree(bst2, "bst2 (should have the data)");

  // Test move assignment
  ads::tree::BinarySearchTree<int> bst3;
  bst3.insert(10);
  bst3.insert(20);

  cout << "\nBefore move assignment:\n";
  print_tree(bst3, "bst3");

  bst3 = std::move(bst2);

  cout << "\nAfter move assignment:\n";
  print_tree(bst2, "bst2 (should be empty)");
  print_tree(bst3, "bst3 (should have bst2's data)");
}

// Test exception handling
void test_exceptions() {
  cout << "\n========== Test: Exception Handling ==========\n";

  ads::tree::BinarySearchTree<int> empty_tree;

  cout << "Testing exceptions on empty tree:\n";

  try {
    cout << "  Trying to find_min() on empty tree...\n";
    [[maybe_unused]] auto min_val = empty_tree.find_min();
    cout << "    ERROR: No exception thrown!\n";
  } catch (const ads::tree::EmptyTreeException& e) {
    cout << "    Correctly caught: " << e.what() << '\n';
  }

  try {
    cout << "  Trying to find_max() on empty tree...\n";
    [[maybe_unused]] auto max_val = empty_tree.find_max();
    cout << "    ERROR: No exception thrown!\n";
  } catch (const ads::tree::EmptyTreeException& e) {
    cout << "    Correctly caught: " << e.what() << '\n';
  }
}

// Test edge cases
void test_edge_cases() {
  cout << "\n========== Test: Edge Cases ==========\n";

  // Single element tree
  ads::tree::BinarySearchTree<int> single;
  single.insert(42);

  cout << "Single-element tree:\n";
  print_tree(single, "single");
  cout << "  Height: " << single.height() << " (should be 0)\n";
  cout << "  Min: " << single.find_min() << ", Max: " << single.find_max() << '\n';

  // Degenerate tree (essentially a linked list)
  ads::tree::BinarySearchTree<int> degenerate;
  for (int i = 1; i <= 5; ++i) {
    degenerate.insert(i);
  }

  cout << "\nDegenerate tree (sequential insertions):\n";
  print_tree(degenerate, "degenerate");
  cout << "  Height: " << degenerate.height() << " (equals size-1 for degenerate tree)\n";

  // Clear and reuse
  cout << "\nClearing and reusing tree:\n";
  degenerate.clear();
  cout << "  After clear - Size: " << degenerate.size() << ", Empty: " << (degenerate.is_empty() ? "yes" : "no") << '\n';

  degenerate.insert(10);
  degenerate.insert(5);
  degenerate.insert(15);
  print_tree(degenerate, "degenerate after reuse");
}

auto main() -> int {
  try {
    cout << "========================================\n";
    cout << "Binary Search Tree - Comprehensive Tests\n";
    cout << "========================================\n";

    test_basic_operations();
    test_search_operations();
    test_removal();
    test_iterators();
    test_custom_types();
    test_move_semantics();
    test_exceptions();
    test_edge_cases();

    cout << "\n========================================\n";
    cout << "All Tests Completed Successfully!\n";
    cout << "========================================\n";

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}