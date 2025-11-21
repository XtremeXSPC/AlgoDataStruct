//===--------------------------------------------------------------------------===//
/**
 * @file main_Red_Black_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for Red-Black Tree implementation
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the Red-Black Tree data structure,
 * showcasing its self-balancing properties, rotations, and various operations.
 */
//===--------------------------------------------------------------------------===//

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../include/ads/trees/Red_Black_Tree.hpp"

using std::cerr;
using std::cout;
using std::string;
using std::to_string;
using std::vector;

using ads::trees::RedBlackTree;

// Helper function to print tree contents
template <typename T>
void print_rbt(const RedBlackTree<T>& tree, const string& name) {
  cout << "Red-Black Tree '" << name << "' (size: " << tree.size() << ", height: " << tree.height() << "):\n";

  if (tree.is_empty()) {
    cout << "  (empty)\n";
    return;
  }

  cout << "  In-order: ";
  tree.in_order_traversal([](const T& value) -> void { cout << value << " "; });
  cout << '\n';
}

// Demo: Basic operations
void demo_basic_operations() {
  cout << "\n========== Demo: Basic Operations ==========\n";

  RedBlackTree<int> rbt;

  cout << "Creating empty Red-Black Tree...\n";
  cout << "  Size: " << rbt.size() << ", Empty: " << (rbt.is_empty() ? "yes" : "no") << "\n";

  // Insert elements
  cout << "\nInserting values: 50, 30, 70, 20, 40, 60, 80\n";
  rbt.insert(50);
  rbt.insert(30);
  rbt.insert(70);
  rbt.insert(20);
  rbt.insert(40);
  rbt.insert(60);
  rbt.insert(80);

  print_rbt(rbt, "rbt");

  // Test different traversals
  cout << "\nDifferent traversals:\n";

  cout << "  Pre-order:   ";
  rbt.pre_order_traversal([](const int& value) -> void { cout << value << " "; });
  cout << '\n';

  cout << "  Post-order:  ";
  rbt.post_order_traversal([](const int& value) -> void { cout << value << " "; });
  cout << '\n';

  cout << "  Level-order: ";
  rbt.level_order_traversal([](const int& value) -> void { cout << value << " "; });
  cout << '\n';

  // Test duplicates
  cout << "\nTrying to insert duplicate (40): ";
  bool inserted = rbt.insert(40);
  cout << (inserted ? "inserted" : "not inserted (correct behavior)") << '\n';
}

// Demo: Self-balancing with sequential insertions
void demo_sequential_insertions() {
  cout << "\n========== Demo: Sequential Insertions (Self-Balancing) ==========\n";

  cout << "Red-Black trees maintain balance even with sequential (worst-case) insertions.\n\n";

  // Ascending order
  RedBlackTree<int> rbt_asc;
  cout << "Inserting 1 to 15 in ascending order:\n";
  for (int i = 1; i <= 15; ++i) {
    rbt_asc.insert(i);
  }
  cout << "  Size: " << rbt_asc.size() << ", Height: " << rbt_asc.height() << "\n";
  cout << "  (Unbalanced BST would have height " << 14 << ")\n";

  // Descending order
  RedBlackTree<int> rbt_desc;
  cout << "\nInserting 15 to 1 in descending order:\n";
  for (int i = 15; i >= 1; --i) {
    rbt_desc.insert(i);
  }
  cout << "  Size: " << rbt_desc.size() << ", Height: " << rbt_desc.height() << "\n";

  cout << "\nBoth trees maintain O(log n) height thanks to self-balancing!\n";
}

// Demo: Search operations
void demo_search_operations() {
  cout << "\n========== Demo: Search Operations ==========\n";

  RedBlackTree<int> rbt;

  vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
  for (int val : values) {
    rbt.insert(val);
  }

  print_rbt(rbt, "rbt");

  // Test contains
  cout << "\nTesting contains():\n";
  cout << "  Contains 40? " << (rbt.contains(40) ? "Yes" : "No") << '\n';
  cout << "  Contains 55? " << (rbt.contains(55) ? "Yes" : "No") << '\n';
  cout << "  Contains 10? " << (rbt.contains(10) ? "Yes" : "No") << '\n';

  // Test find_min and find_max
  cout << "\nMinimum value: " << rbt.find_min() << '\n';
  cout << "Maximum value: " << rbt.find_max() << '\n';
}

// Demo: Remove operations
void demo_remove_operations() {
  cout << "\n========== Demo: Remove Operations ==========\n";

  RedBlackTree<int> rbt;

  vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
  for (int val : values) {
    rbt.insert(val);
  }

  print_rbt(rbt, "original tree");

  // Remove leaf node
  cout << "\nRemoving leaf node (10):\n";
  rbt.remove(10);
  print_rbt(rbt, "after removing 10");

  // Remove node with one child
  cout << "\nRemoving node with one child (20):\n";
  rbt.remove(20);
  print_rbt(rbt, "after removing 20");

  // Remove node with two children
  cout << "\nRemoving node with two children (30):\n";
  rbt.remove(30);
  print_rbt(rbt, "after removing 30");

  // Remove root
  cout << "\nRemoving root node (50):\n";
  rbt.remove(50);
  print_rbt(rbt, "after removing root");

  cout << "\nTree remains balanced after all removals (height: " << rbt.height() << ")\n";
}

// Demo: Iterator functionality
void demo_iterators() {
  cout << "\n========== Demo: Iterators ==========\n";

  RedBlackTree<int> rbt;

  vector<int> values = {50, 30, 70, 20, 40, 60, 80};
  for (int val : values) {
    rbt.insert(val);
  }

  // Range-based for loop
  cout << "Using range-based for loop (in-order traversal):\n  ";
  for (const int& value : rbt) {
    cout << value << " ";
  }
  cout << '\n';

  // Explicit iterator usage
  cout << "\nUsing explicit iterators:\n  ";
  for (auto it = rbt.begin(); it != rbt.end(); ++it) {
    cout << *it << " ";
  }
  cout << '\n';

  // Collecting values into a vector
  cout << "\nCollecting values into a vector:\n  ";
  vector<int> collected;
  for (const auto& val : rbt) {
    collected.push_back(val);
  }
  cout << "Vector contents: ";
  for (int val : collected) {
    cout << val << " ";
  }
  cout << '\n';
}

// Demo: Custom types
void demo_custom_types() {
  cout << "\n========== Demo: Custom Types ==========\n";

  struct Person {
    string name;
    int    age;

    Person(string n, int a) : name(std::move(n)), age(a) {}

    auto operator<(const Person& other) const -> bool { return age < other.age; }
    auto operator==(const Person& other) const -> bool { return age == other.age; }
  };

  RedBlackTree<Person> people;

  cout << "Inserting people by age:\n";
  people.emplace("Alice", 30);
  people.emplace("Bob", 25);
  people.emplace("Charlie", 35);
  people.emplace("Diana", 28);
  people.emplace("Eve", 32);

  cout << "\nPeople sorted by age (in-order traversal):\n";
  people.in_order_traversal([](const Person& p) -> void { cout << "  " << p.name << " (age " << p.age << ")\n"; });

  cout << "\nYoungest: " << people.find_min().name << " (age " << people.find_min().age << ")\n";
  cout << "Oldest: " << people.find_max().name << " (age " << people.find_max().age << ")\n";
}

// Demo: Move semantics
void demo_move_semantics() {
  cout << "\n========== Demo: Move Semantics ==========\n";

  RedBlackTree<int> rbt1;
  rbt1.insert(50);
  rbt1.insert(30);
  rbt1.insert(70);

  cout << "Original tree:\n";
  print_rbt(rbt1, "rbt1");

  // Move constructor
  RedBlackTree<int> rbt2 = std::move(rbt1);

  cout << "\nAfter move construction:\n";
  print_rbt(rbt1, "rbt1 (should be empty)");
  print_rbt(rbt2, "rbt2 (should have the data)");

  // Move assignment
  RedBlackTree<int> rbt3;
  rbt3.insert(10);
  rbt3.insert(20);

  cout << "\nBefore move assignment:\n";
  print_rbt(rbt3, "rbt3");

  rbt3 = std::move(rbt2);

  cout << "\nAfter move assignment:\n";
  print_rbt(rbt2, "rbt2 (should be empty)");
  print_rbt(rbt3, "rbt3 (should have rbt2's data)");
}

// Demo: Performance comparison with large dataset
void demo_performance() {
  cout << "\n========== Demo: Performance ==========\n";

  const int N = 10000;

  cout << "Inserting " << N << " elements in sequential order...\n";

  auto start = std::chrono::high_resolution_clock::now();

  RedBlackTree<int> rbt;
  for (int i = 1; i <= N; ++i) {
    rbt.insert(i);
  }

  auto end      = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "  Time: " << duration.count() << " ms\n";
  cout << "  Size: " << rbt.size() << "\n";
  cout << "  Height: " << rbt.height() << " (theoretical max: ~" << (int)(2 * std::log2(N + 1)) << ")\n";

  // Search performance
  cout << "\nSearching for all " << N << " elements...\n";
  start = std::chrono::high_resolution_clock::now();
  for (int i = 1; i <= N; ++i) {
    [[maybe_unused]] bool found = rbt.contains(i);
  }
  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "  Time: " << duration.count() << " ms\n";
}

// Demo: Exception handling
void demo_exceptions() {
  cout << "\n========== Demo: Exception Handling ==========\n";

  RedBlackTree<int> empty_tree;

  cout << "Testing exceptions on empty tree:\n";

  try {
    cout << "  Trying to find_min() on empty tree...\n";
    [[maybe_unused]] auto min_val = empty_tree.find_min();
    cout << "    ERROR: No exception thrown!\n";
  } catch (const ads::trees::EmptyTreeException& e) {
    cout << "    Correctly caught: " << e.what() << '\n';
  }

  try {
    cout << "  Trying to find_max() on empty tree...\n";
    [[maybe_unused]] auto max_val = empty_tree.find_max();
    cout << "    ERROR: No exception thrown!\n";
  } catch (const ads::trees::EmptyTreeException& e) {
    cout << "    Correctly caught: " << e.what() << '\n';
  }
}

// Demo: Edge cases
void demo_edge_cases() {
  cout << "\n========== Demo: Edge Cases ==========\n";

  // Single element tree
  RedBlackTree<int> single;
  single.insert(42);

  cout << "Single-element tree:\n";
  print_rbt(single, "single");
  cout << "  Height: " << single.height() << " (should be 0)\n";
  cout << "  Min: " << single.find_min() << ", Max: " << single.find_max() << '\n';

  // Clear and reuse
  cout << "\nClearing and reusing tree:\n";
  single.clear();
  cout << "  After clear - Size: " << single.size() << ", Empty: " << (single.is_empty() ? "yes" : "no") << '\n';

  single.insert(10);
  single.insert(5);
  single.insert(15);
  print_rbt(single, "single after reuse");
}

auto main() -> int {
  try {
    cout << "========================================\n";
    cout << "  Red-Black Tree - Comprehensive Demo\n";
    cout << "========================================\n";

    demo_basic_operations();
    demo_sequential_insertions();
    demo_search_operations();
    demo_remove_operations();
    demo_iterators();
    demo_custom_types();
    demo_move_semantics();
    demo_performance();
    demo_exceptions();
    demo_edge_cases();

    cout << "\n========================================\n";
    cout << "  All Demos Completed Successfully!\n";
    cout << "========================================\n";

  } catch (const std::exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
