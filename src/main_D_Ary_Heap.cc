//===---------------------------------------------------------------------------===//
/**
 * @file main_D_Ary_Heap.cc
 * @author Costantino Lombardi
 * @brief Demo for the DAryHeap structure.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/heaps/D_Ary_Heap.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cout;
using std::string;

using namespace ads::heaps;

namespace {

//===------------------------------- HEAP DEMOS --------------------------------===//

auto demo_max_d_ary_heap() -> void {
  ads::demo::print_section("4-ary heap (max semantics)");

  DAryHeap<int> heap(4);
  for (int value : {12, 7, 25, 3, 18, 30, 1, 19}) {
    heap.insert(value);
  }

  cout << "Top: " << heap.top() << '\n';
  cout << "Extraction order: ";
  while (!heap.is_empty()) {
    cout << heap.extract_top();
    if (!heap.is_empty()) {
      cout << ", ";
    }
  }
  cout << '\n';
}

auto demo_min_d_ary_heap() -> void {
  ads::demo::print_section("5-ary heap (min semantics)");

  MinDAryHeap<string> heap(5);
  heap.insert("mango");
  heap.insert("apple");
  heap.insert("pear");
  heap.insert("banana");

  cout << "Top: " << heap.top() << '\n';
  heap.update_key(0, string("zebra"));
  cout << "Top after demoting root: " << heap.top() << '\n';
}

} // namespace

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  ads::demo::print_header({"D-ARY HEAP DEMO", "Configurable heap arity with max/min semantics"});

  demo_max_d_ary_heap();
  demo_min_d_ary_heap();

  ads::demo::print_footer("D-ARY HEAP DEMO COMPLETED!");
  return 0;
}

//===---------------------------------------------------------------------------===//
