//===---------------------------------------------------------------------------===//
/**
 * @file main_Pairing_Heap.cc
 * @author Costantino Lombardi
 * @brief Demo program for the PairingHeap meldable priority queue.
 * @version 0.1
 * @date 2026-07-08
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the PairingHeap: basic priority-queue usage, the
 * O(1) merge (meld), and the fast handle-based decrease_key / erase. Because a
 * pairing heap relinks nodes rather than moving values, its handles are just
 * node pointers that stay valid across every operation.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/heaps/Pairing_Heap.hpp"
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
  ads::demo::print_section("Pairing Heap - Basic Operations");

  MaxPairingHeap<int> heap{3, 1, 4, 1, 5, 9, 2, 6};
  ads::demo::print_info("Built a max-heap from {3, 1, 4, 1, 5, 9, 2, 6}.");
  cout << "Size: " << heap.size() << ", top: " << heap.top() << "\n";

  DynamicArray<int> drained;
  while (!heap.is_empty()) {
    drained.push_back(heap.extract_top());
  }
  ads::demo::print_sequence(drained, "Extraction order (descending): ");
  ads::demo::print_success("Two-pass combine restored priority order.");
}

//===----- MELD ----------------------------------------------------------------===//

auto demo_merge() -> void {
  ads::demo::print_section("Pairing Heap - Merge (Meld)");

  MaxPairingHeap<int> left{10, 30, 20};
  MaxPairingHeap<int> right{25, 5, 35};
  ads::demo::print_info("Merging {10, 30, 20} with {25, 5, 35}.");

  left.merge(std::move(right));
  cout << "After merge - size: " << left.size() << ", top: " << left.top() << "\n";
  cout << "Source heap is now empty: " << std::boolalpha << right.is_empty() << "\n";
  ads::demo::print_success("Meld is a single O(1) link of two roots.");
}

//===----- HANDLES -------------------------------------------------------------===//

auto demo_handles() -> void {
  ads::demo::print_section("Pairing Heap - Handles (decrease_key / erase)");

  MinPairingHeap<int> heap;
  heap.emplace(50);
  auto task = heap.emplace(80);
  heap.emplace(20);
  auto victim = heap.emplace(65);
  ads::demo::print_info("Inserted {50, 80, 20, 65}; holding handles to 80 and 65.");
  cout << "Top (min): " << heap.top() << "\n";

  heap.decrease_key(task, 5); // cut 80's node and meld it back as the new minimum
  cout << "After decrease_key(80 -> 5), top: " << heap.top() << "\n";

  heap.erase(victim); // remove 65 regardless of its position
  ads::demo::print_info("Erased the element 65 via its handle.");

  DynamicArray<int> drained;
  while (!heap.is_empty()) {
    drained.push_back(heap.extract_top());
  }
  ads::demo::print_sequence(drained, "Remaining, ascending: ");
  ads::demo::print_success("Node-pointer handles stayed valid throughout.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("PAIRING HEAP - COMPREHENSIVE DEMO");
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
