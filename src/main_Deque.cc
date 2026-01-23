//===---------------------------------------------------------------------------===//
/**
 * @file main_Deque.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for CircularArrayDeque implementation.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the CircularArrayDeque data structure,
 * showcasing insertion, deletion, iteration, resizing, and move semantics.
 */
//===---------------------------------------------------------------------------===//

#include <iostream>
#include <string>
#include <utility>

#include "../include/ads/queues/Circular_Array_Deque.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using namespace ads::queues;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Helper function to print deque contents.
template <typename T>
void print_deque(const CircularArrayDeque<T>& deque, const string& label) {
  cout << label << " (size: " << deque.size() << ", capacity: " << deque.capacity() << ")\n";
  if (deque.is_empty()) {
    cout << "  (empty)\n";
    return;
  }

  cout << "  ";
  for (const auto& value : deque) {
    cout << value << " ";
  }
  cout << "\n";
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Demonstrate basic push_front, push_back, front, back operations.
void demo_basic_operations() {
  ads::demo::print_section("Demo: Basic Operations");

  CircularArrayDeque<int> deque;
  cout << "Created empty deque.\n";
  print_deque(deque, "Initial state");

  cout << "\nPushing elements at both ends: front(5), back(10), back(20), front(0)\n";
  deque.push_front(5);
  deque.push_back(10);
  deque.push_back(20);
  deque.push_front(0);
  print_deque(deque, "After push operations");

  cout << "Front element: " << deque.front() << "\n";
  cout << "Back element: " << deque.back() << "\n";
}

//===-------------------------- POP OPERATIONS DEMO ----------------------------===//

// Demonstrate pop_front and pop_back operations.
void demo_pop_operations() {
  ads::demo::print_section("Demo: Pop Operations");

  CircularArrayDeque<int> deque;
  for (int i = 1; i <= 5; ++i) {
    deque.push_back(i * 10);
  }
  print_deque(deque, "Original deque");

  cout << "\nPopping from front...\n";
  deque.pop_front();
  print_deque(deque, "After pop_front");

  cout << "\nPopping from back...\n";
  deque.pop_back();
  print_deque(deque, "After pop_back");
}

//===------------------------- RESERVE & INDEX DEMO ----------------------------===//

// Demonstrate reserve and index access operations.
void demo_reserve_and_index() {
  ads::demo::print_section("Demo: Reserve and Indexing");

  CircularArrayDeque<int> deque;
  for (int i = 0; i < 8; ++i) {
    deque.push_back(i);
  }

  print_deque(deque, "Before reserve");
  cout << "\nReserving capacity for 32 elements\n";
  deque.reserve(32);
  print_deque(deque, "After reserve");

  cout << "\nAccess by index: deque[0] = " << deque[0] << ", deque[3] = " << deque[3] << "\n";
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

  CircularArrayDeque<Person> people;
  people.emplace_back("Alice", 28);
  people.emplace_front("Bob", 35);
  people.emplace_back("Charlie", 40);

  cout << "People in deque:\n";
  for (const auto& person : people) {
    cout << "  " << person.name << " (age " << person.age << ")\n";
  }
}

//===--------------------------- MOVE SEMANTICS DEMO ---------------------------===//

// Demonstrate move semantics.
void demo_move_semantics() {
  ads::demo::print_section("Demo: Move Semantics");

  CircularArrayDeque<int> source;
  for (int i = 1; i <= 4; ++i) {
    source.push_back(i * 5);
  }
  print_deque(source, "Source before move");

  CircularArrayDeque<int> moved = std::move(source);
  print_deque(moved, "Moved deque");
  print_deque(source, "Source after move");
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("DEQUE - COMPREHENSIVE DEMO");

    // Test Deque implementations.
    demo_basic_operations();
    demo_pop_operations();
    demo_reserve_and_index();
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
