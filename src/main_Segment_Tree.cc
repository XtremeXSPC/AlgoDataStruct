//===---------------------------------------------------------------------------===//
/**
 * @file main_Segment_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for SegmentTree implementation.
 * @version 0.1
 * @date 2026-02-03
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the SegmentTree data structure,
 * showcasing range queries and point updates.
 */
//===---------------------------------------------------------------------------===//

#include <iostream>
#include <string>
#include <vector>

#include "../include/ads/trees/Segment_Tree.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::vector;

using ads::trees::SegmentTree;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Print the current state of the Segment Tree.
template <typename T>
void print_tree_state(const SegmentTree<T>& tree, const string& label) {
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

  cout << "  Total sum: " << tree.total_sum() << "\n";
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Demonstrates building the tree and performing queries.
void demo_build_and_queries() {
  ads::demo::print_section("Demo: Build and Queries");

  vector<int>      values = {1, 3, -2, 8, -7};
  SegmentTree<int> tree(values);

  print_tree_state(tree, "Initial tree");
  cout << "Sum [0..2]: " << tree.range_sum(0, 2) << "\n";
  cout << "Sum [2..4]: " << tree.range_sum(2, 4) << "\n";
}

//===------------------------- UPDATE OPERATIONS DEMO --------------------------===//

// Demonstrates point updates (set and add).
void demo_updates() {
  ads::demo::print_section("Demo: Point Updates");

  SegmentTree<int> tree({5, 1, 4, 2, 7});
  print_tree_state(tree, "Before updates");

  cout << "Setting index 2 to 10\n";
  tree.set(2, 10);
  cout << "Adding +3 at index 4\n";
  tree.add(4, 3);

  print_tree_state(tree, "After updates");
  cout << "Sum [1..3]: " << tree.range_sum(1, 3) << "\n";
}

//===------------------------ RESET & CLEAR OPERATIONS -------------------------===//

// Demonstrates resetting and clearing the tree.
void demo_reset_clear() {
  ads::demo::print_section("Demo: Reset and Clear");

  SegmentTree<int> tree(4, 2);
  print_tree_state(tree, "After size/value constructor");

  cout << "Resetting to size 3\n";
  tree.reset(3);
  print_tree_state(tree, "After reset");

  cout << "Clearing tree\n";
  tree.clear();
  print_tree_state(tree, "After clear");
}

//===------------------------- EXCEPTION HANDLING DEMO -------------------------===//

// Demonstrates exception handling for invalid operations.
void demo_exceptions() {
  ads::demo::print_section("Demo: Exception Handling");

  SegmentTree<int> tree({1, 2, 3});

  try {
    tree.set(5, 1);
  } catch (const ads::trees::SegmentTreeException& e) {
    cout << "Caught exception: " << e.what() << "\n";
  }

  try {
    [[maybe_unused]] auto sum = tree.range_sum(2, 1);
  } catch (const ads::trees::SegmentTreeException& e) {
    cout << "Caught exception: " << e.what() << "\n";
  }
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("SEGMENT TREE - COMPREHENSIVE DEMO");

    // Run all demo sections.
    demo_build_and_queries();
    demo_updates();
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
