//===---------------------------------------------------------------------------===//
/**
 * @file main_Fibonacci_Heap.cc
 * @author Costantino Lombardi
 * @brief Demo program for the FibonacciHeap meldable priority queue.
 * @version 0.1
 * @date 2026-07-09
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the FibonacciHeap: basic priority-queue usage, the
 * O(1) merge (meld), and the O(1) amortized handle-based decrease_key that makes
 * the Fibonacci heap the classic choice for Dijkstra's and Prim's algorithms.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/heaps/Fibonacci_Heap.hpp"
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
  ads::demo::print_section("Fibonacci Heap - Basic Operations");

  MaxFibonacciHeap<int> heap{3, 1, 4, 1, 5, 9, 2, 6};
  ads::demo::print_info("Built a max-heap from {3, 1, 4, 1, 5, 9, 2, 6}.");
  cout << "Size: " << heap.size() << ", top: " << heap.top() << "\n";

  DynamicArray<int> drained;
  while (!heap.is_empty()) {
    drained.push_back(heap.extract_top());
  }
  ads::demo::print_sequence(drained, "Extraction order (descending): ");
  ads::demo::print_success("Consolidation restored priority order on demand.");
}

//===----- MELD ----------------------------------------------------------------===//

auto demo_merge() -> void {
  ads::demo::print_section("Fibonacci Heap - Merge (Meld)");

  MaxFibonacciHeap<int> left{10, 30, 20};
  MaxFibonacciHeap<int> right{25, 5, 35};
  ads::demo::print_info("Merging {10, 30, 20} with {25, 5, 35}.");

  left.merge(std::move(right));
  cout << "After merge - size: " << left.size() << ", top: " << left.top() << "\n";
  cout << "Source heap is now empty: " << std::boolalpha << right.is_empty() << "\n";
  ads::demo::print_success("Meld is an O(1) splice of two root lists.");
}

//===----- HANDLES -------------------------------------------------------------===//

auto demo_handles() -> void {
  ads::demo::print_section("Fibonacci Heap - Handles (decrease_key / erase)");

  MinFibonacciHeap<int> heap;
  heap.emplace(50);
  auto task = heap.emplace(80);
  heap.emplace(20);
  auto victim = heap.emplace(65);
  ads::demo::print_info("Inserted {50, 80, 20, 65}; holding handles to 80 and 65.");
  cout << "Top (min): " << heap.top() << "\n";

  heap.decrease_key(task, 5); // O(1) amortized: cut the node and cascade
  cout << "After decrease_key(80 -> 5), top: " << heap.top() << "\n";

  heap.erase(victim); // remove 65 regardless of its position
  ads::demo::print_info("Erased the element 65 via its handle.");

  DynamicArray<int> drained;
  while (!heap.is_empty()) {
    drained.push_back(heap.extract_top());
  }
  ads::demo::print_sequence(drained, "Remaining, ascending: ");
  ads::demo::print_success("O(1) amortized decrease_key: the Dijkstra/Prim bound.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  ads::demo::print_header("FIBONACCI HEAP - COMPREHENSIVE DEMO");

  try {
    demo_basic();
    demo_merge();
    demo_handles();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
