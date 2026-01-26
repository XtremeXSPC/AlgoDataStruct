//===---------------------------------------------------------------------------===//
/**
 * @file main_Disjoint_Set_Union.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for DisjointSetUnion implementation.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the DisjointSetUnion data structure,
 * showcasing union-find operations, connectivity checks, and dynamic growth.
 */
//===---------------------------------------------------------------------------===//

#include <iostream>
#include <string>

#include "../include/ads/graphs/Disjoint_Set_Union.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using namespace ads::graphs;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Prints the current status of the DisjointSetUnion.
void print_status(const DisjointSetUnion& dsu, const string& label) {
  cout << label << " (elements: " << dsu.size() << ", sets: " << dsu.set_count() << ")\n";
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Demonstrates basic union and connectivity operations.
void demo_basic_operations() {
  ads::demo::print_section("Demo: Basic Operations");

  DisjointSetUnion dsu(7);
  print_status(dsu, "Initial structure");

  cout << "\nUnion 0-1, 1-2, 3-4\n";
  dsu.union_sets(0, 1);
  dsu.union_sets(1, 2);
  dsu.union_sets(3, 4);
  print_status(dsu, "After unions");

  cout << "Connected(0,2): " << (dsu.connected(0, 2) ? "yes" : "no") << "\n";
  cout << "Connected(2,3): " << (dsu.connected(2, 3) ? "yes" : "no") << "\n";
}

//===-------------------------- FIND OPERATIONS DEMO ---------------------------===//

// Demonstrates find operations with path compression.
void demo_find_operations() {
  ads::demo::print_section("Demo: Find Operations");

  DisjointSetUnion dsu(5);
  dsu.union_sets(0, 1);
  dsu.union_sets(1, 2);

  cout << "Representative of 0: " << dsu.find(0) << "\n";
  cout << "Representative of 2: " << dsu.find(2) << "\n";
  cout << "Representative of 4: " << dsu.find(4) << "\n";
}

//===-------------------------- DYNAMIC GROWTH DEMO ----------------------------===//

// Demonstrates adding new elements dynamically.
void demo_dynamic_growth() {
  ads::demo::print_section("Demo: Dynamic Growth");

  DisjointSetUnion dsu(3);
  print_status(dsu, "Initial structure");

  size_t new_index = dsu.add_element();
  cout << "Added element at index " << new_index << "\n";

  dsu.union_sets(2, new_index);
  print_status(dsu, "After adding and union");
}

//===-------------------------- EXCEPTION HANDLING DEMO -------------------------===//

// Demonstrates exception handling for invalid operations.
void demo_exceptions() {
  ads::demo::print_section("Demo: Exception Handling");

  DisjointSetUnion dsu(2);
  try {
    [[maybe_unused]] auto result = dsu.find(5);
  } catch (const DisjointSetException& e) {
    cout << "Caught exception: " << e.what() << "\n";
  }
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("DISJOINT SET UNION - COMPREHENSIVE DEMO");

    demo_basic_operations();
    demo_find_operations();
    demo_dynamic_growth();
    demo_exceptions();

    ads::demo::print_footer();
  } catch (const exception& e) {
    ads::demo::print_error(string("Unhandled exception: ") + e.what());
    return 1;
  }

  return 0;
}

//===---------------------------------------------------------------------------===//
