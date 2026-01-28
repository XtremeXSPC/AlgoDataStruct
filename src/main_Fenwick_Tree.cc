//===---------------------------------------------------------------------------===//
/**
 * @file main_Fenwick_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for FenwickTree implementation.
 * @version 0.1
 * @date 2026-01-28
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the FenwickTree data structure,
 * showcasing prefix queries, range queries, and point updates.
 */
//===---------------------------------------------------------------------------===//

#include <iostream>
#include <string>
#include <vector>

#include "../include/ads/trees/Fenwick_Tree.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::vector;

using ads::trees::FenwickTree;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Print the current state of the Fenwick tree.
template <typename T>
void print_tree_state(const FenwickTree<T>& tree, const string& label) {
  cout << label << " (size: " << tree.size() << ")\n";
  if (tree.is_empty()) {
    cout << "  (empty)\n";
    return;
  }

  cout << "  Values: ";
  for (size_t i = 0; i < tree.size(); ++i) {
    cout << tree.value_at(i) << " ";
  }
  cout << "\n";

  cout << "  Prefix sums: ";
  for (size_t i = 0; i < tree.size(); ++i) {
    cout << tree.prefix_sum(i) << " ";
  }
  cout << "\n";
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Demonstrates building the tree and performing queries.
void demo_build_and_queries() {
  ads::demo::print_section("Demo: Build and Queries");

  vector<int>      values = {1, 2, 3, 4, 5};
  FenwickTree<int> tree(values);

  print_tree_state(tree, "Initial tree");
  cout << "Total sum: " << tree.total_sum() << "\n";
  cout << "Sum [1..3]: " << tree.range_sum(1, 3) << "\n";
}

//===------------------------- UPDATE OPERATIONS DEMO --------------------------===//

// Demonstrates point updates (additions).
void demo_updates() {
  ads::demo::print_section("Demo: Point Updates");

  FenwickTree<int> tree({5, 1, 4, 2, 7});
  print_tree_state(tree, "Before updates");

  cout << "Adding +3 at index 2\n";
  tree.add(2, 3);
  cout << "Adding -2 at index 4\n";
  tree.add(4, -2);

  print_tree_state(tree, "After updates");
  cout << "Sum [0..2]: " << tree.range_sum(0, 2) << "\n";
}

//===--------------------------- SET OPERATIONS DEMO ---------------------------===//

// Demonstrates setting values at specific indices.
void demo_set_operations() {
  ads::demo::print_section("Demo: Set Operations");

  FenwickTree<int> tree({2, 4, 6, 8});
  print_tree_state(tree, "Original tree");

  cout << "Setting index 1 to 10\n";
  tree.set(1, 10);
  print_tree_state(tree, "After set");
}

//===------------------------ RESET & CLEAR OPERATIONS -------------------------===//

// Demonstrates resetting and clearing the tree.
void demo_reset_clear() {
  ads::demo::print_section("Demo: Reset and Clear");

  FenwickTree<int> tree(3);
  print_tree_state(tree, "After reset to size 3");

  cout << "Clearing tree\n";
  tree.clear();
  print_tree_state(tree, "After clear");
}

//===------------------------- EXCEPTION HANDLING DEMO -------------------------===//

// Demonstrates exception handling for invalid operations.
void demo_exceptions() {
  ads::demo::print_section("Demo: Exception Handling");

  FenwickTree<int> tree({1, 2, 3});

  try {
    tree.add(5, 1);
  } catch (const ads::trees::FenwickTreeException& e) {
    cout << "Caught exception: " << e.what() << "\n";
  }

  try {
    [[maybe_unused]] auto sum = tree.range_sum(2, 1);
  } catch (const ads::trees::FenwickTreeException& e) {
    cout << "Caught exception: " << e.what() << "\n";
  }
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("FENWICK TREE - COMPREHENSIVE DEMO");

    // Run all demo sections.
    demo_build_and_queries();
    demo_updates();
    demo_set_operations();
    demo_reset_clear();
    demo_exceptions();

    ads::demo::print_footer();
  } catch (const exception& e) {
    ads::demo::print_error(string("Unhandled exception: ") + e.what());
    return 1;
  }

  return 0;
}

//===---------------------------------------------------------------------------===//
