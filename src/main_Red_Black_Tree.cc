//===--------------------------------------------------------------------------===//
/**
 * @file main_Red_Black_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for Red-Black Tree implementation.
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the Red-Black Tree data structure,
 * showcasing its self-balancing properties and various operations.
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

using ads::trees::Red_Black_Tree;

// Helper function to print tree contents
template <typename T>
void print_rbt(const Red_Black_Tree<T>& tree, const string& name) {
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

  Red_Black_Tree<int> rbt;

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

  // Test in-order traversal
  cout << "\nIn-order traversal (should be sorted):\n  ";
  rbt.in_order_traversal([](const int& value) -> void { cout << value << " "; });
  cout << '\n';

  // Test duplicates
  cout << "\nTrying to insert duplicate (40): ";
  bool inserted = rbt.insert(40);
  cout << (inserted ? "inserted" : "not inserted (correct behavior)") << '\n';
}

// Demo: Sequential insertions
void demo_sequential_insertions() {
  cout << "\n========== Demo: Sequential Insertions ==========\n";

  cout << "Red-Black Trees maintain balance even with sequential insertions.\n\n";

  Red_Black_Tree<int> rbt_asc;

  cout << "Inserting 1 to 15 in ascending order:\n";
  for (int i = 1; i <= 15; ++i) {
    rbt_asc.insert(i);
  }
  cout << "  Size: " << rbt_asc.size() << ", Height: " << rbt_asc.height() << "\n";
  cout << "  (A regular BST would have height 15, RBT keeps it balanced)\n";

  Red_Black_Tree<int> rbt_desc;

  cout << "\nInserting 15 to 1 in descending order:\n";
  for (int i = 15; i >= 1; --i) {
    rbt_desc.insert(i);
  }
  cout << "  Size: " << rbt_desc.size() << ", Height: " << rbt_desc.height() << "\n";
}

// Demo: Search operations
void demo_search_operations() {
  cout << "\n========== Demo: Search Operations ==========\n";

  Red_Black_Tree<int> rbt;

  // Build a tree
  vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
  for (int val : values) {
    rbt.insert(val);
  }

  print_rbt(rbt, "rbt");

  // Test contains/search
  cout << "\nSearch operations:\n";
  cout << "  contains(40): " << (rbt.contains(40) ? "found" : "not found") << '\n';
  cout << "  contains(55): " << (rbt.contains(55) ? "found" : "not found") << '\n';
  cout << "  search(70): " << (rbt.search(70) ? "found" : "not found") << '\n';
  cout << "  search(100): " << (rbt.search(100) ? "found" : "not found") << '\n';
}

// Demo: RB-Tree properties validation
void demo_property_validation() {
  cout << "\n========== Demo: RB-Tree Properties Validation ==========\n";

  Red_Black_Tree<int> rbt;

  // Insert random values
  vector<int> values = {41, 38, 31, 12, 19, 8, 50, 45, 60, 55, 70};
  cout << "Inserting values: ";
  for (int val : values) {
    cout << val << " ";
    rbt.insert(val);
  }
  cout << '\n';

  print_rbt(rbt, "rbt");

  cout << "\nRed-Black Tree properties:\n";
  cout << "  Height: " << rbt.height() << '\n';
  cout << "  Black height: " << rbt.black_height() << '\n';
  cout << "  Properties valid: " << (rbt.validate_properties() ? "yes" : "no") << '\n';
}

// Demo: Random insertions
void demo_random_insertions() {
  cout << "\n========== Demo: Random Insertions ==========\n";

  Red_Black_Tree<int> rbt;

  // Generate and shuffle values
  vector<int> values;
  for (int i = 1; i <= 100; ++i) {
    values.push_back(i);
  }

  std::random_device rd;
  std::mt19937       g(rd());
  std::shuffle(values.begin(), values.end(), g);

  cout << "Inserting 100 values in random order...\n";
  for (int val : values) {
    rbt.insert(val);
  }

  cout << "  Size: " << rbt.size() << '\n';
  cout << "  Height: " << rbt.height() << " (theoretical max ~2*log2(n) = ~14)\n";
  cout << "  Black height: " << rbt.black_height() << '\n';
  cout << "  Properties valid: " << (rbt.validate_properties() ? "yes" : "no") << '\n';

  // Verify all elements present
  int found_count = 0;
  for (int i = 1; i <= 100; ++i) {
    if (rbt.search(i)) {
      ++found_count;
    }
  }
  cout << "  All elements found: " << (found_count == 100 ? "yes" : "no") << '\n';
}

// Demo: Move semantics
void demo_move_semantics() {
  cout << "\n========== Demo: Move Semantics ==========\n";

  Red_Black_Tree<int> rbt1;
  for (int i = 1; i <= 10; ++i) {
    rbt1.insert(i);
  }

  cout << "Original tree:\n";
  print_rbt(rbt1, "rbt1");

  // Move constructor
  Red_Black_Tree<int> rbt2 = std::move(rbt1);

  cout << "\nAfter move construction:\n";
  print_rbt(rbt1, "rbt1 (should be empty)");
  print_rbt(rbt2, "rbt2 (should have the data)");

  // Move assignment
  Red_Black_Tree<int> rbt3;
  rbt3.insert(100);
  rbt3.insert(200);

  cout << "\nBefore move assignment:\n";
  print_rbt(rbt3, "rbt3");

  rbt3 = std::move(rbt2);

  cout << "\nAfter move assignment:\n";
  print_rbt(rbt2, "rbt2 (should be empty)");
  print_rbt(rbt3, "rbt3 (should have rbt2's data)");
}

// Demo: Performance
void demo_performance() {
  cout << "\n========== Demo: Performance ==========\n";

  const int N = 10000;

  cout << "Inserting " << N << " elements sequentially...\n";

  auto start = std::chrono::high_resolution_clock::now();

  Red_Black_Tree<int> rbt;
  for (int i = 1; i <= N; ++i) {
    rbt.insert(i);
  }

  auto end             = std::chrono::high_resolution_clock::now();
  auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "  Insert time: " << insert_duration.count() << " ms\n";
  cout << "  Size: " << rbt.size() << '\n';
  cout << "  Height: " << rbt.height() << " (theoretical O(log n) ~ " << (int)(2 * std::log2(N)) << ")\n";

  // Search performance
  cout << "\nSearching for all " << N << " elements...\n";
  start = std::chrono::high_resolution_clock::now();
  for (int i = 1; i <= N; ++i) {
    [[maybe_unused]] bool found = rbt.search(i);
  }
  end                  = std::chrono::high_resolution_clock::now();
  auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "  Search time: " << search_duration.count() << " ms\n";
}

// Demo: Clear and reuse
void demo_clear_reuse() {
  cout << "\n========== Demo: Clear and Reuse ==========\n";

  Red_Black_Tree<int> rbt;

  for (int i = 1; i <= 20; ++i) {
    rbt.insert(i);
  }
  cout << "After inserting 20 elements:\n";
  cout << "  Size: " << rbt.size() << ", Height: " << rbt.height() << '\n';

  rbt.clear();
  cout << "\nAfter clear():\n";
  cout << "  Size: " << rbt.size() << ", Empty: " << (rbt.is_empty() ? "yes" : "no") << '\n';

  rbt.insert(42);
  rbt.insert(17);
  rbt.insert(99);
  cout << "\nAfter inserting new values (42, 17, 99):\n";
  print_rbt(rbt, "rbt");
  cout << "  Properties valid: " << (rbt.validate_properties() ? "yes" : "no") << '\n';
}

// Demo: Edge cases
void demo_edge_cases() {
  cout << "\n========== Demo: Edge Cases ==========\n";

  Red_Black_Tree<int> rbt;

  // Test operations on empty tree
  cout << "Testing operations on empty tree:\n";
  cout << "  is_empty(): " << (rbt.is_empty() ? "true" : "false") << '\n';
  cout << "  size(): " << rbt.size() << '\n';
  cout << "  height(): " << rbt.height() << '\n';
  cout << "  contains(42): " << (rbt.contains(42) ? "true" : "false") << '\n';
  cout << "  validate_properties(): " << (rbt.validate_properties() ? "true" : "false") << '\n';

  // Test single element
  cout << "\nTesting single element:\n";
  rbt.insert(42);
  print_rbt(rbt, "rbt with single element");
  cout << "  Properties valid: " << (rbt.validate_properties() ? "yes" : "no") << '\n';

  // Test clear
  cout << "\nTesting clear():\n";
  rbt.insert(10);
  rbt.insert(20);
  rbt.insert(30);
  cout << "Before clear: size=" << rbt.size() << '\n';
  rbt.clear();
  cout << "After clear: size=" << rbt.size() << ", is_empty=" << rbt.is_empty() << '\n';
}

auto main() -> int {
  try {
    cout << "╔═══----------------------------------------------------═══╗\n";
    cout << "            RED-BLACK TREE - COMPREHENSIVE DEMO             \n";
    cout << "╚═══----------------------------------------------------═══╝\n";

    demo_basic_operations();
    demo_sequential_insertions();
    demo_search_operations();
    demo_property_validation();
    demo_random_insertions();
    demo_move_semantics();
    demo_performance();
    demo_clear_reuse();
    demo_edge_cases();

    cout << "\n";
    cout << "╔═══----------------------------------------------------═══╗\n";
    cout << "             ALL DEMOS COMPLETED SUCCESSFULLY!              \n";
    cout << "╚═══----------------------------------------------------═══╝\n";

  } catch (const std::exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
