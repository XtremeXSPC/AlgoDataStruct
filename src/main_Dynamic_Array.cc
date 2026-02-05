//===---------------------------------------------------------------------------===//
/**
 * @file main_Dynamic_Array.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for DynamicArray implementation.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the DynamicArray data structure,
 * showcasing insertion, deletion, iteration, resizing, and move semantics.
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/arrays/Dynamic_Array.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>
#include <utility>

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using namespace ads::arrays;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Helper function to print array contents.
template <typename T>
void print_array(const DynamicArray<T>& array, const string& label) {
  cout << label << " (size: " << array.size() << ", capacity: " << array.capacity() << ")\n";
  if (array.is_empty()) {
    cout << "  (empty)\n";
    return;
  }
  cout << "  ";
  for (const auto& value : array) {
    cout << value << " ";
  }
  cout << "\n";
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Demonstrate basic push_back, front, back operations.
void demo_basic_operations() {
  ads::demo::print_section("Demo: Basic Operations");

  DynamicArray<int> array;
  cout << "Created empty array.\n";
  print_array(array, "Initial state");

  cout << "\nPushing elements: 10, 20, 30\n";
  array.push_back(10);
  array.push_back(20);
  array.push_back(30);
  print_array(array, "After push_back");

  cout << "Front element: " << array.front() << "\n";
  cout << "Back element: " << array.back() << "\n";
}

//===-------------------------- INSERT & ERASE DEMO ----------------------------===//

// Demonstrate insert and erase operations.
void demo_insert_erase() {
  ads::demo::print_section("Demo: Insert and Erase");

  DynamicArray<int> array;
  array.push_back(10);
  array.push_back(30);
  array.push_back(40);
  print_array(array, "Original array");

  cout << "\nInserting 20 at index 1\n";
  array.insert(1, 20);
  print_array(array, "After insert");

  cout << "\nErasing element at index 2\n";
  array.erase(2);
  print_array(array, "After erase");
}

//===------------------------- RESIZE & RESERVE DEMO ---------------------------===//

// Demonstrate resize and reserve operations.
void demo_resize_reserve() {
  ads::demo::print_section("Demo: Resize and Reserve");

  DynamicArray<int> array(3, 7);
  print_array(array, "Filled array");

  cout << "\nReserving capacity for 32 elements\n";
  array.reserve(32);
  print_array(array, "After reserve");

  cout << "\nResizing to 6 elements (default-initialized)\n";
  array.resize(6);
  print_array(array, "After resize");

  cout << "\nShrinking to fit\n";
  array.shrink_to_fit();
  print_array(array, "After shrink_to_fit");
}

//===------------------------- EMPLACE & ITERATOR DEMO -------------------------===//

// Demonstrate emplace and iterator functionality.
void demo_emplace_iterators() {
  ads::demo::print_section("Demo: Emplace and Iterators");

  struct Person {
    string name;
    int    age;

    Person(string n, int a) : name(std::move(n)), age(a) {}
  };

  DynamicArray<Person> people;
  people.emplace_back("Alice", 28);
  people.emplace_back("Bob", 32);
  people.emplace(1, "Charlie", 40);

  cout << "People in array:\n";
  for (const auto& person : people) {
    cout << "  " << person.name << " (age " << person.age << ")\n";
  }
}

//===--------------------------- MOVE SEMANTICS DEMO ---------------------------===//

// Demonstrate move semantics.
void demo_move_semantics() {
  ads::demo::print_section("Demo: Move Semantics");

  DynamicArray<int> source;
  for (int i = 1; i <= 5; ++i) {
    source.push_back(i * 10);
  }
  print_array(source, "Source before move");

  DynamicArray<int> moved = std::move(source);
  print_array(moved, "Moved array");
  print_array(source, "Source after move");
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("DYNAMIC ARRAY - COMPREHENSIVE DEMO");

    // Run all demo functions.
    demo_basic_operations();
    demo_insert_erase();
    demo_resize_reserve();
    demo_emplace_iterators();
    demo_move_semantics();

    ads::demo::print_footer();
  } catch (const exception& e) {
    ads::demo::print_error(string("Unhandled exception: ") + e.what());
    return 1;
  }

  return 0;
}

//===---------------------------------------------------------------------------===//
