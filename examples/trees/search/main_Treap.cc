//===---------------------------------------------------------------------------===//
/**
 * @file main_Treap.cc
 * @author Costantino Lombardi
 * @brief Demo for the Treap structure.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#include "ads/trees/search/Treap.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>

using std::cerr;
using std::cout;
using std::exception;

using namespace ads::trees;

namespace {

//===----- DEMO HELPERS --------------------------------------------------------===//

auto demo_explicit_priorities() -> void {
  ads::demo::print_section("Deterministic priorities");

  Treap<int> treap;
  treap.insert_with_priority(30, 10);
  treap.insert_with_priority(10, 40);
  treap.insert_with_priority(20, 50);

  cout << "In-order: ";
  treap.in_order_traversal([](const int& value) { cout << value << ' '; });
  cout << "\nPre-order: ";
  treap.pre_order_traversal([](const int& value) { cout << value << ' '; });
  cout << '\n';
}

//===----- TREAP DEMOS ---------------------------------------------------------===//

auto demo_regular_usage() -> void {
  ads::demo::print_section("Regular randomized priorities");

  Treap<int> treap;
  for (int value : {50, 30, 70, 20, 40, 60, 80}) {
    treap.insert(value);
  }

  cout << "Min: " << treap.find_min() << '\n';
  cout << "Max: " << treap.find_max() << '\n';
  cout << "Height: " << treap.height() << '\n';
  cout << "Contains 60: " << std::boolalpha << treap.contains(60) << '\n';
}

} // namespace

//===----- MAIN FUNCTION -------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("TREAP - COMPREHENSIVE DEMO");

    demo_explicit_priorities();
    demo_regular_usage();

    ads::demo::print_footer();
  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}

//===---------------------------------------------------------------------------===//
