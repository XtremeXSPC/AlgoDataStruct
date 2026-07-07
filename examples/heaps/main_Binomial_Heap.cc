//===---------------------------------------------------------------------------===//
/**
 * @file main_Binomial_Heap.cc
 * @author Costantino Lombardi
 * @brief Demo program for the BinomialHeap meldable priority queue.
 * @version 0.1
 * @date 2026-07-07
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the BinomialHeap: basic priority-queue usage, the
 * O(log n) merge (meld), and the handle-based decrease_key / erase operations
 * that the forest heaps introduce on top of the meldable-heap contract.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/heaps/Binomial_Heap.hpp"
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
  ads::demo::print_section("Binomial Heap - Basic Operations");

  MaxBinomialHeap<int> heap{3, 1, 4, 1, 5, 9, 2, 6};
  ads::demo::print_info("Built a max-heap from {3, 1, 4, 1, 5, 9, 2, 6}.");
  cout << "Size: " << heap.size() << ", top: " << heap.top() << "\n";

  DynamicArray<int> drained;
  while (!heap.is_empty()) {
    drained.push_back(heap.extract_top());
  }
  ads::demo::print_sequence(drained, "Extraction order (descending): ");
  ads::demo::print_success("Elements left the heap in priority order.");
}

//===----- MELD ----------------------------------------------------------------===//

auto demo_merge() -> void {
  ads::demo::print_section("Binomial Heap - Merge (Meld)");

  MaxBinomialHeap<int> left{10, 30, 20};
  MaxBinomialHeap<int> right{25, 5, 35};
  ads::demo::print_info("Merging {10, 30, 20} with {25, 5, 35}.");

  left.merge(std::move(right));
  cout << "After merge - size: " << left.size() << ", top: " << left.top() << "\n";
  cout << "Source heap is now empty: " << std::boolalpha << right.is_empty() << "\n";
  ads::demo::print_success("Two forests added like binary numbers in O(log n).");
}

//===----- HANDLES -------------------------------------------------------------===//

auto demo_handles() -> void {
  ads::demo::print_section("Binomial Heap - Handles (decrease_key / erase)");

  MinBinomialHeap<int> heap;
  heap.emplace(50);
  auto task = heap.emplace(80); // keep a handle to this element
  heap.emplace(20);
  auto victim = heap.emplace(65);
  ads::demo::print_info("Inserted {50, 80, 20, 65}; holding handles to 80 and 65.");
  cout << "Top (min): " << heap.top() << "\n";

  heap.decrease_key(task, 5); // raise priority: 80 -> 5 becomes the new minimum
  cout << "After decrease_key(80 -> 5), top: " << heap.top() << "\n";

  heap.erase(victim); // remove 65 regardless of its position
  ads::demo::print_info("Erased the element 65 via its handle.");

  DynamicArray<int> drained;
  while (!heap.is_empty()) {
    drained.push_back(heap.extract_top());
  }
  ads::demo::print_sequence(drained, "Remaining, ascending: ");
  ads::demo::print_success("Handles stayed valid across every meld and sift.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("BINOMIAL HEAP - COMPREHENSIVE DEMO");
    demo_basic();
    demo_merge();
    demo_handles();
    ads::demo::print_footer();
  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}

//===---------------------------------------------------------------------------===//
