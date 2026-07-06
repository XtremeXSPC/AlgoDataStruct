//===---------------------------------------------------------------------------===//
/**
 * @file main_Skew_Heap.cc
 * @author Costantino Lombardi
 * @brief Demo program for the SkewHeap meldable priority queue.
 * @version 0.1
 * @date 2026-07-06
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the SkewHeap: basic priority-queue usage, the
 * difference between the Max and Min comparator aliases, and the defining
 * O(log n) amortized merge (meld) operation. Unlike the leftist heap it stores
 * no balance metadata; the self-adjusting child swap keeps it balanced.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/heaps/Skew_Heap.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <vector>

using std::cerr;
using std::cout;
using std::exception;
using std::vector;

using namespace ads::heaps;
using ads::arrays::DynamicArray;

//===----- BASIC OPERATIONS ----------------------------------------------------===//

auto demo_basic() -> void {
  ads::demo::print_section("Skew Heap - Basic Operations");

  MaxSkewHeap<int> heap{3, 1, 4, 1, 5, 9, 2, 6};
  ads::demo::print_info("Built a max-heap from {3, 1, 4, 1, 5, 9, 2, 6}.");
  cout << "Size: " << heap.size() << ", top: " << heap.top() << "\n";

  DynamicArray<int> drained;
  while (!heap.is_empty()) {
    drained.push_back(heap.extract_top());
  }
  ads::demo::print_sequence(drained, "Extraction order (descending): ");
  ads::demo::print_success("Elements left the heap in priority order.");
}

//===----- MIN VS MAX ----------------------------------------------------------===//

auto demo_min_vs_max() -> void {
  ads::demo::print_section("Skew Heap - Min vs Max Aliases");

  // SkewHeap's vector constructor takes a const std::vector<T>& (API bridge).
  const vector<int> data{42, 7, 19, 88, 3};
  MaxSkewHeap<int>  max_heap(data);
  MinSkewHeap<int>  min_heap(data);

  ads::demo::print_sequence(data, "Input: ");
  cout << "MaxSkewHeap top: " << max_heap.top() << "\n";
  cout << "MinSkewHeap top: " << min_heap.top() << "\n";
  ads::demo::print_success("The comparator alias selects which end is the top.");
}

//===----- MELD ----------------------------------------------------------------===//

auto demo_merge() -> void {
  ads::demo::print_section("Skew Heap - Merge (Meld)");

  MaxSkewHeap<int> left{10, 30, 20};
  MaxSkewHeap<int> right{25, 5, 35};
  ads::demo::print_info("Merging {10, 30, 20} with {25, 5, 35}.");

  left.merge(std::move(right));
  cout << "After merge - size: " << left.size() << ", top: " << left.top() << "\n";
  cout << "Source heap is now empty: " << std::boolalpha << right.is_empty() << "\n";

  DynamicArray<int> drained;
  while (!left.is_empty()) {
    drained.push_back(left.extract_top());
  }
  ads::demo::print_sequence(drained, "Merged extraction order: ");
  ads::demo::print_success("Two heaps melded into one in O(log n) amortized.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  ads::demo::print_header("SKEW HEAP - COMPREHENSIVE DEMO");

  try {
    demo_basic();
    demo_min_vs_max();
    demo_merge();

    ads::demo::print_footer();

    return 0;

  } catch (const exception& e) {
    cerr << "\n[FATAL ERROR] Unhandled exception: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
