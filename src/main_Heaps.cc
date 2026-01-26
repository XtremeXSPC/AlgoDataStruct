//===---------------------------------------------------------------------------===//
/**
 * @file main_Heaps.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for MinHeap and MaxHeap classes.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the MinHeap and MaxHeap data structures,
 * showcasing their insertion, extraction, and utility methods.
 */
//===---------------------------------------------------------------------------===//

#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "../include/ads/heaps/Max_Heap.hpp"
#include "../include/ads/heaps/Min_Heap.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::move;
using std::string;
using std::to_string;
using std::vector;

using namespace ads::heaps;

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Min Heap basic operations.
void demo_min_heap_basic() {
  ads::demo::print_section("Min Heap - Basic Operations");

  MinHeap<int> heap;

  cout << std::format("Empty heap created. is_empty(): {}\n", heap.is_empty());
  cout << std::format("Size: {}, Capacity: {}\n", heap.size(), heap.capacity());

  cout << "\nInserting elements: 5, 3, 7, 1, 9, 2\n";
  heap.insert(5);
  heap.insert(3);
  heap.insert(7);
  heap.insert(1);
  heap.insert(9);
  heap.insert(2);

  cout << "Size: " << heap.size() << '\n';
  cout << "Top (min): " << heap.top() << '\n';

  cout << "\nExtracting all elements (should be sorted ascending):\n";
  while (!heap.is_empty()) {
    cout << heap.extract_min() << ' ';
  }
  cout << '\n';
  cout << std::format("Heap is now empty: {}\n", heap.is_empty());
}

//===---------------------- CONSTRUCTION FROM VECTOR DEMO ----------------------===//

// Min Heap construction from vector.
void demo_min_heap_from_vector() {
  ads::demo::print_section("Min Heap - Construction from Vector");

  vector<int> data = {15, 10, 20, 8, 12, 25, 18};
  cout << "Original vector: ";
  for (int val : data) {
    cout << val << ' ';
  }
  cout << '\n';

  MinHeap<int> heap(data);
  cout << "Heap size: " << heap.size() << '\n';
  cout << "Top (min): " << heap.top() << '\n';

  cout << "Extracting all elements:\n";
  while (!heap.is_empty()) {
    cout << heap.extract_min() << ' ';
  }
  cout << '\n';
}

//===--------------------------- MOVE SEMANTICS DEMO ---------------------------===//

// Min Heap move semantics.
void demo_min_heap_move_semantics() {
  ads::demo::print_section("Min Heap - Move Semantics");

  MinHeap<int> heap1;
  for (int i = 10; i > 0; --i) {
    heap1.insert(i);
  }

  cout << "Heap1 size: " << heap1.size() << ", top: " << heap1.top() << '\n';

  cout << "\nMoving heap1 to heap2 (move constructor)...\n";
  MinHeap<int> heap2(std::move(heap1));

  cout << "Heap2 size: " << heap2.size() << ", top: " << heap2.top() << '\n';
  cout << "Heap1 size after move: " << heap1.size() << '\n';

  MinHeap<int> heap3;
  heap3.insert(99);

  cout << "\nMoving heap2 to heap3 (move assignment)...\n";
  heap3 = std::move(heap2);

  cout << "Heap3 size: " << heap3.size() << ", top: " << heap3.top() << '\n';
  cout << "Heap2 size after move: " << heap2.size() << '\n';
}

//===------------------------- EMPLACE OPERATIONS DEMO -------------------------===//

// Min Heap emplace operations.
void demo_min_heap_emplace() {
  ads::demo::print_section("Min Heap - Emplace Operations");

  MinHeap<string> heap;

  heap.emplace("World");
  heap.emplace("Hello");
  heap.emplace("Test");
  heap.emplace("Algorithms");

  cout << "Heap size: " << heap.size() << '\n';
  cout << "Top: " << heap.top() << '\n';

  cout << "Extracting all strings:\n";
  while (!heap.is_empty()) {
    cout << heap.extract_min() << '\n';
  }
}

//===------------------------- EXCEPTION HANDLING DEMO -------------------------===//

// Min Heap exception handling.
void demo_min_heap_exception_handling() {
  ads::demo::print_section("Min Heap - Exception Handling");

  MinHeap<int> heap;

  try {
    heap.top();
    cout << "ERROR: top() should throw on empty heap\n";
  } catch (const HeapException& e) {
    cout << "Caught expected exception for top(): " << e.what() << '\n';
  }

  try {
    heap.extract_min();
    cout << "ERROR: extract_min() should throw on empty heap\n";
  } catch (const HeapException& e) {
    cout << "Caught expected exception for extract_min(): " << e.what() << '\n';
  }
}

//===--------------------------- LARGE DATASET DEMO ----------------------------===//

// Min Heap large dataset test.
void demo_min_heap_large() {
  ads::demo::print_section("Min Heap - Large Dataset");

  const int    N = 10000;
  MinHeap<int> heap;

  cout << "Inserting " << N << " elements...\n";
  for (int i = N; i > 0; --i) {
    heap.insert(i);
  }

  cout << "Heap size: " << heap.size() << '\n';
  cout << "Top (should be 1): " << heap.top() << '\n';

  cout << "Extracting first 10 elements: ";
  for (int i = 0; i < 10; ++i) {
    cout << heap.extract_min() << ' ';
  }
  cout << '\n';

  cout << "Clearing heap...\n";
  heap.clear();
  cout << "Size after clear: " << heap.size() << '\n';
}

//===----------------------------- MAX HEAP DEMOS ------------------------------===//

// Max Heap basic operations.
void demo_max_heap_basic() {
  ads::demo::print_section("Max Heap - Basic Operations");

  MaxHeap<int> heap;

  cout << std::format("Empty heap created. is_empty(): {}\n", heap.is_empty());
  cout << std::format("Size: {}, Capacity: {}\n", heap.size(), heap.capacity());

  cout << "\nInserting elements: 5, 3, 7, 1, 9, 2\n";
  heap.insert(5);
  heap.insert(3);
  heap.insert(7);
  heap.insert(1);
  heap.insert(9);
  heap.insert(2);

  cout << "Size: " << heap.size() << '\n';
  cout << "Top (max): " << heap.top() << '\n';

  cout << "\nExtracting all elements (should be sorted descending):\n";
  while (!heap.is_empty()) {
    cout << heap.extract_max() << ' ';
  }
  cout << '\n';
  cout << std::format("Heap is now empty: {}\n", heap.is_empty());
}

//===---------------------- CONSTRUCTION FROM VECTOR DEMO ----------------------===//

// Max Heap construction from vector.
void demo_max_heap_from_vector() {
  ads::demo::print_section("Max Heap - Construction from Vector");

  vector<int> data = {15, 10, 20, 8, 12, 25, 18};
  cout << "Original vector: ";
  for (int val : data) {
    cout << val << ' ';
  }
  cout << '\n';

  MaxHeap<int> heap(data);
  cout << "Heap size: " << heap.size() << '\n';
  cout << "Top (max): " << heap.top() << '\n';

  cout << "Extracting all elements:\n";
  while (!heap.is_empty()) {
    cout << heap.extract_max() << ' ';
  }
  cout << '\n';
}

//===------------------------ HEAPSORT APPLICATION DEMO ------------------------===//

// Max Heap heapsort application.
void demo_max_heap_heapsort() {
  ads::demo::print_section("Max Heap - Heapsort Application");

  vector<int> data = {64, 34, 25, 12, 22, 11, 90};
  cout << "Unsorted array: ";
  for (int val : data) {
    cout << val << ' ';
  }
  cout << '\n';

  // Build max heap.
  MaxHeap<int> heap(data);

  cout << "Sorted (ascending) using max heap:\n";
  vector<int> sorted;
  while (!heap.is_empty()) {
    sorted.push_back(heap.extract_max());
  }

  // Reverse to get ascending order.
  for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
    cout << *it << ' ';
  }
  cout << '\n';
}

//===-------------------------- HEAP COMPARISON DEMO ---------------------------===//

// Compare Min Heap and Max Heap.
void demo_heap_comparison() {
  ads::demo::print_section("Heap Comparison - Min vs Max");

  vector<int> data = {5, 2, 8, 1, 9, 3, 7};

  MinHeap<int> min_heap(data);
  MaxHeap<int> max_heap(data);

  cout << "Same input data: ";
  for (int val : data) {
    cout << val << ' ';
  }
  cout << '\n';

  cout << "\nMinHeap top (min): " << min_heap.top() << '\n';
  cout << "MaxHeap top (max): " << max_heap.top() << '\n';

  cout << "\nExtracting from MinHeap: ";
  for (int i = 0; i < 3; ++i) {
    cout << min_heap.extract_min() << ' ';
  }
  cout << '\n';

  cout << "Extracting from MaxHeap: ";
  for (int i = 0; i < 3; ++i) {
    cout << max_heap.extract_max() << ' ';
  }
  cout << '\n';
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  ads::demo::print_header("MIN HEAP AND MAX HEAP - COMPREHENSIVE DEMO");

  try {
    // Min Heap tests.
    demo_min_heap_basic();
    demo_min_heap_from_vector();
    demo_min_heap_move_semantics();
    demo_min_heap_emplace();
    demo_min_heap_exception_handling();
    demo_min_heap_large();

    // Max Heap tests.
    demo_max_heap_basic();
    demo_max_heap_from_vector();
    demo_max_heap_heapsort();

    // Comparison.
    demo_heap_comparison();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\n!!! UNEXPECTED EXCEPTION !!!\n";
    cerr << "What: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
