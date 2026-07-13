//===---------------------------------------------------------------------------===//
/**
 * @file main_Cartesian_Tree.cc
 * @author Costantino Lombardi
 * @brief Demo for the CartesianTree structure.
 * @version 0.1
 * @date 2026-07-13
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the Cartesian tree: an O(n) build from a sequence,
 * the twin in-order and heap-order invariants, and the range-minimum query that
 * follows from a single root-to-node descent.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/trees/search/Cartesian_Tree.hpp"
#include "support/Demo_Utilities.hpp"

#include <functional>
#include <iostream>

using std::cerr;
using std::cout;
using std::exception;

using ads::arrays::DynamicArray;
using ads::trees::CartesianTree;

//===----- BUILD & INVARIANTS --------------------------------------------------===//

auto demo_build() -> void {
  ads::demo::print_section("Cartesian Tree - Build and Invariants");

  DynamicArray<int> sequence;
  for (int value : {9, 3, 7, 1, 8, 12, 10}) {
    sequence.push_back(value);
  }

  CartesianTree<int> tree(sequence);

  cout << "sequence : ";
  for (size_t i = 0; i < sequence.size(); ++i) {
    cout << sequence[i] << ' ';
  }
  cout << "\nin-order : ";
  tree.in_order_traversal([](const int& value) { cout << value << ' '; });
  cout << "\npre-order: ";
  tree.pre_order_traversal([](const int& value) { cout << value << ' '; });
  cout << "\nroot (min) = " << tree.extremum() << ", height = " << tree.height() << "\n";
  ads::demo::print_success("In-order reproduces the sequence; the root is the global minimum.");
}

//===----- RANGE MINIMUM -------------------------------------------------------===//

auto demo_range_minimum() -> void {
  ads::demo::print_section("Cartesian Tree - Range Minimum Query");

  DynamicArray<int> sequence;
  for (int value : {5, 2, 8, 6, 1, 9, 3, 7}) {
    sequence.push_back(value);
  }
  CartesianTree<int> tree(sequence);

  cout << "sequence : ";
  for (size_t i = 0; i < sequence.size(); ++i) {
    cout << sequence[i] << ' ';
  }
  cout << "\n";

  auto report_range = [&](size_t left, size_t right) -> void {
    const size_t index = tree.range_extremum_index(left, right);
    cout << "min over [" << left << ", " << right << "] = " << tree.range_extremum(left, right) << " at position " << index << "\n";
  };
  report_range(0, 3);
  report_range(2, 6);
  report_range(4, 7);
  ads::demo::print_success("The range minimum is the ancestor reached by descending toward the range.");
}

//===----- MAX-TREE VARIANT ----------------------------------------------------===//

auto demo_max_tree() -> void {
  ads::demo::print_section("Cartesian Tree - Max Variant");

  DynamicArray<int> sequence;
  for (int value : {3, 1, 4, 1, 5, 9, 2, 6}) {
    sequence.push_back(value);
  }
  CartesianTree<int, std::greater<int>> tree(sequence);

  cout << "sequence : ";
  for (size_t i = 0; i < sequence.size(); ++i) {
    cout << sequence[i] << ' ';
  }
  cout << "\nroot (max) = " << tree.extremum() << "\n";
  cout << "max over [2, 5] = " << tree.range_extremum(2, 5) << "\n";
  ads::demo::print_success("Passing std::greater turns the structure into a range-maximum index.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  ads::demo::print_header("CARTESIAN TREE - COMPREHENSIVE DEMO");

  try {
    demo_build();
    demo_range_minimum();
    demo_max_tree();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
