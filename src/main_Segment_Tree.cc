//===---------------------------------------------------------------------------===//
/**
 * @file main_Segment_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for SegmentTree implementation.
 * @version 0.2
 * @date 2026-02-04
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the SegmentTree data structure,
 * showcasing range queries and point updates.
 */
//===---------------------------------------------------------------------------===//

#include <algorithm>
#include <iostream>
#include <limits>
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

struct MaxCombine {
  auto operator()(int left, int right) const -> int { return std::max(left, right); }
};

struct MaxIdentity {
  auto operator()() const -> int { return std::numeric_limits<int>::min(); }
};

struct SumCount {
  int sum   = 0;
  int count = 0;
};

struct SumCountCombine {
  auto operator()(const SumCount& left, const SumCount& right) const -> SumCount {
    return SumCount{.sum = left.sum + right.sum, .count = left.count + right.count};
  }
};

struct SumCountIdentity {
  auto operator()() const -> SumCount { return SumCount{}; }
};

struct SumCountLeaf {
  auto operator()(int value) const -> SumCount { return SumCount{.sum = value, .count = 1}; }
};

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

//===--------------------------- CUSTOM FUNCTOR DEMO ---------------------------===//

// Demonstrates custom combine and identity functors (max segment tree).
void demo_custom_functors() {
  ads::demo::print_section("Demo: Custom Functors (Max)");

  vector<int>                                    values = {1, 3, -2, 8, -7};
  SegmentTree<int, int, MaxCombine, MaxIdentity> tree(values);

  cout << "Max [0..4]: " << tree.range_query(0, 4) << "\n";
  cout << "Max [1..3]: " << tree.range_query(1, 3) << "\n";

  cout << "Setting index 3 to 0\n";
  tree.set(3, 0);
  cout << "Max [0..4] after update: " << tree.range_query(0, 4) << "\n";
}

//===---------------------------- CUSTOM NODE DEMO -----------------------------===//

// Demonstrates custom node aggregation using a leaf builder.
void demo_custom_nodes() {
  ads::demo::print_section("Demo: Custom Nodes (Sum + Count)");

  vector<int> values = {2, 4, 6, 8};

  SegmentTree<int, SumCount, SumCountCombine, SumCountIdentity, SumCountLeaf> tree(values);

  SumCount result = tree.range_query(1, 3);
  cout << "Range [1..3] sum: " << result.sum << "\n";
  cout << "Range [1..3] count: " << result.count << "\n";
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
    demo_custom_functors();
    demo_custom_nodes();
    demo_exceptions();

    ads::demo::print_footer();
  } catch (const exception& e) {
    ads::demo::print_error(string("Unhandled exception: ") + e.what());
    return 1;
  }

  return 0;
}

//===---------------------------------------------------------------------------===//
