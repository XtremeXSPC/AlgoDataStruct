//===---------------------------------------------------------------------------===//
/**
 * @file main_Circular_Linked_List.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for CircularLinkedList implementation.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the CircularLinkedList data structure,
 * showcasing circular behavior, rotation, and round-robin patterns.
 */
//===---------------------------------------------------------------------------===//

#include <iostream>
#include <string>

#include "../include/ads/lists/Circular_Linked_List.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using namespace ads::lists;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Helper function to print list contents.
template <typename T>
void print_list(const CircularLinkedList<T>& list, const string& label) {
  cout << label << " (size: " << list.size() << ")\n";
  if (list.is_empty()) {
    cout << "  (empty)\n";
    return;
  }
  cout << "  [ ";
  for (const auto& value : list) {
    cout << value << " ";
  }
  cout << "] (circular: last -> first)\n";
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Demonstrate basic push_back, push_front, front, back operations.
void demo_basic_operations() {
  ads::demo::print_section("Demo: Basic Operations");

  CircularLinkedList<int> list;
  cout << "Created empty circular linked list.\n";
  print_list(list, "Initial state");

  cout << "\nPushing to back: 10, 20, 30\n";
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);
  print_list(list, "After push_back");

  cout << "\nPushing to front: 5, 0\n";
  list.push_front(5);
  list.push_front(0);
  print_list(list, "After push_front");

  cout << "\nFront element: " << list.front() << "\n";
  cout << "Back element: " << list.back() << "\n";

  ads::demo::print_success("Basic operations work correctly.");
}

//===------------------------- CIRCULAR BEHAVIOR DEMO --------------------------===//

// Demonstrate circular behavior and rotation.
void demo_circular_behavior() {
  ads::demo::print_section("Demo: Circular Behavior");

  CircularLinkedList<int> list;
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i);
  }
  print_list(list, "Initial list");

  cout << "\nThe list forms a ring: 1 -> 2 -> 3 -> 4 -> 5 -> (back to 1)\n";

  cout << "\nRotating the list (moving head to next element):\n";
  for (int i = 0; i < 5; ++i) {
    cout << "  Rotation " << i << ": front=" << list.front() << ", back=" << list.back() << "\n";
    list.rotate();
  }
  cout << "After 5 rotations, we're back at the start.\n";
  print_list(list, "After full cycle");

  ads::demo::print_success("Circular behavior works correctly.");
}

//===---------------------------- ROUND-ROBIN DEMO -----------------------------===//

// Demonstrate round-robin pattern using CircularLinkedList.
void demo_round_robin() {
  ads::demo::print_section("Demo: Round-Robin Pattern");

  cout << "Simulating round-robin task scheduling:\n\n";

  CircularLinkedList<string> tasks;
  tasks.push_back("Task A");
  tasks.push_back("Task B");
  tasks.push_back("Task C");
  tasks.push_back("Task D");

  print_list(tasks, "Task queue");

  cout << "\nExecuting 10 time slices (round-robin):\n";
  for (int slice = 1; slice <= 10; ++slice) {
    cout << "  Slice " << slice << ": Executing [" << tasks.front() << "]\n";
    tasks.rotate();
  }

  cout << "\nRemoving completed Task B:\n";
  tasks.rotate();
  tasks.pop_front();
  print_list(tasks, "After removing Task B");

  cout << "\nAdding new Task E:\n";
  tasks.push_back("Task E");
  print_list(tasks, "After adding Task E");

  ads::demo::print_success("Round-robin pattern works correctly.");
}

//===------------------------ CONTAINS AND SEARCH DEMO -------------------------===//

// Demonstrate contains() method.
void demo_search() {
  ads::demo::print_section("Demo: Search Operations");

  CircularLinkedList<int> list;
  for (int i = 10; i <= 50; i += 10) {
    list.push_back(i);
  }
  print_list(list, "List");

  cout << "\nSearching for elements:\n";
  cout << "  contains(10): " << (list.contains(10) ? "true" : "false") << "\n";
  cout << "  contains(30): " << (list.contains(30) ? "true" : "false") << "\n";
  cout << "  contains(50): " << (list.contains(50) ? "true" : "false") << "\n";
  cout << "  contains(25): " << (list.contains(25) ? "true" : "false") << "\n";
  cout << "  contains(100): " << (list.contains(100) ? "true" : "false") << "\n";

  ads::demo::print_success("Search operations work correctly.");
}

//===--------------------------- REMOVAL OPERATIONS DEMO -----------------------===//

void demo_removal() {
  ads::demo::print_section("Demo: Removal Operations");

  CircularLinkedList<int> list;
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i);
  }
  print_list(list, "Initial list");

  cout << "\nRemoving from front:\n";
  cout << "  pop_front() removes: " << list.front() << "\n";
  list.pop_front();
  print_list(list, "After pop_front");

  cout << "\nRemoving from back:\n";
  cout << "  pop_back() removes: " << list.back() << "\n";
  list.pop_back();
  print_list(list, "After pop_back");

  cout << "\nClearing the list:\n";
  list.clear();
  print_list(list, "After clear");

  ads::demo::print_success("Removal operations work correctly.");
}

//===--------------------------- EXCEPTION HANDLING DEMO -----------------------===//

void demo_exception_handling() {
  ads::demo::print_section("Demo: Exception Handling");

  CircularLinkedList<int> list;

  cout << "Testing pop_front on empty list:\n";
  try {
    list.pop_front();
    ads::demo::print_error("  Exception not thrown (unexpected).");
  } catch (const ListException& e) {
    ads::demo::print_info("  Caught expected exception: ");
    cout << e.what() << "\n";
  }

  cout << "\nTesting pop_back on empty list:\n";
  try {
    list.pop_back();
    ads::demo::print_error("  Exception not thrown (unexpected).");
  } catch (const ListException& e) {
    ads::demo::print_info("  Caught expected exception: ");
    cout << e.what() << "\n";
  }

  cout << "\nTesting front() on empty list:\n";
  try {
    [[maybe_unused]] auto val = list.front();
    ads::demo::print_error("  Exception not thrown (unexpected).");
  } catch (const ListException& e) {
    ads::demo::print_info("  Caught expected exception: ");
    cout << e.what() << "\n";
  }

  ads::demo::print_success("Exception handling works correctly.");
}

//===--------------------------- STRING TYPE DEMO ------------------------------===//

void demo_string_type() {
  ads::demo::print_section("Demo: String Type Support");

  CircularLinkedList<string> players;
  players.emplace_back("Alice");
  players.emplace_back("Bob");
  players.emplace_back("Charlie");
  players.emplace_back("Diana");

  print_list(players, "Players in game");

  cout << "\nSimulating turns (each player takes 2 turns):\n";
  for (int round = 1; round <= 8; ++round) {
    cout << "  Turn " << round << ": " << players.front() << "'s turn\n";
    players.rotate();
  }

  ads::demo::print_success("String type support works correctly.");
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  ads::demo::print_header("CircularLinkedList Demo");

  try {
    demo_basic_operations();
    demo_circular_behavior();
    demo_round_robin();
    demo_search();
    demo_removal();
    demo_exception_handling();
    demo_string_type();

    ads::demo::print_footer("All demos completed successfully!");
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected exception: " << e.what() << "\n";
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
