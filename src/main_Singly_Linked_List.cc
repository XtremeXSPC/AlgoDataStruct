//===---------------------------------------------------------------------------===//
/**
 * @file main_Singly_Linked_List.cc
 * @author Costantino Lombardi
 * @brief Demonstration of SinglyLinkedList usage.
 * @version 0.2
 * @date 2025-01-20
 *
 * This file is a quick visual demo, not a test suite. Unit tests live in /tests.
 */
//===---------------------------------------------------------------------------===//

#include <exception>
#include <iostream>
#include <string>
#include <utility>

#include "../include/ads/lists/Singly_Linked_List.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::to_string;

using namespace ads::lists;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Helper function to print the list contents.
template <typename T>
void print_list(const SinglyLinkedList<T>& list, const string& label) {
  cout << ANSI_CYAN << label << ANSI_RESET << " (size " << list.size() << "): ";
  for (const auto& v : list) {
    cout << v << ' ';
  }
  cout << '\n';
}

//===----------------------------- DEMO FUNCTIONS ------------------------------===//

// Basic push/pop demo.
void demo_basics() {
  ads::demo::print_section("Basic push/pop");

  SinglyLinkedList<int> list;
  list.push_front(3);
  list.push_front(2);
  list.push_front(1);
  list.push_back(4);
  list.push_back(5);

  print_list(list, "After pushes");
  cout << "front=" << list.front() << " back=" << list.back() << '\n';

  list.pop_front();
  list.pop_back();
  print_list(list, "After pop_front + pop_back");
}

//===------------------------ EMPLACE AND REVERSE DEMO -------------------------===//

// Emplace and reverse demo.
void demo_emplace_and_reverse() {
  ads::demo::print_section("Emplace and reverse");

  SinglyLinkedList<string> words;
  words.emplace_front("World");
  words.emplace_front("Hello");
  words.emplace_back("!");

  print_list(words, "Before reverse");
  words.reverse();
  print_list(words, "After reverse");
}

//===--------------------------- MOVE SEMANTICS DEMO ---------------------------===//

// Move semantics demo.
void demo_move_semantics() {
  ads::demo::print_section("Move semantics");

  SinglyLinkedList<int> original;
  for (int i = 1; i <= 5; ++i) {
    original.push_back(i);
  }
  print_list(original, "Original");

  SinglyLinkedList<int> moved_ctor(std::move(original));
  print_list(moved_ctor, "Moved (ctor)");
  print_list(original, "Original (after move)");

  SinglyLinkedList<int> assigned;
  assigned.push_back(42);
  assigned = std::move(moved_ctor);
  print_list(assigned, "Moved (assignment)");
}

//===------------------------- EXCEPTION HANDLING DEMO -------------------------===//

// Test exception handling.
void demo_exceptions() {
  ads::demo::print_section("Exception handling");

  SinglyLinkedList<int> empty;
  try {
    empty.front();
  } catch (const ListException& e) {
    cout << ANSI_YELLOW << "front() threw as expected: " << e.what() << ANSI_RESET << '\n';
  }
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  ads::demo::print_header("SINGLY LINKED LIST - COMPREHENSIVE DEMO");

  demo_basics();
  demo_emplace_and_reverse();
  demo_move_semantics();
  demo_exceptions();

  cout << "\n";
  ads::demo::print_info("Done. Run ENABLE_TESTING=ON to execute the full unit tests.");
  return 0;
}

//===---------------------------------------------------------------------------===//
