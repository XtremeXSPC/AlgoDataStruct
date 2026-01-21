//===--------------------------------------------------------------------------===//
/**
 * @file main_Linked_Lists.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for DoublyLinkedList implementation.
 * @version 0.2
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the DoublyLinkedList data structure,
 * showcasing various operations such as insertion, deletion, iteration,
 * and move semantics.
 */
//===--------------------------------------------------------------------------===//

#include <iostream>
#include <string>
#include <vector>

#include "../include/ads/lists/Doubly_Linked_List.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::to_string;
using std::vector;

using ads::list::DoublyLinkedList;
using ads::list::ListException;

// Helper function to print the list contents
template <typename T>
void print_list(const DoublyLinkedList<T>& list, const string& name) {
  cout << "List '" << name << "' (size: " << list.size() << "):\n";

  if (list.is_empty()) {
    cout << "  (empty)\n";
    return;
  }

  // Forward traversal
  cout << "  Forward:  ";
  for (auto it = list.cbegin(); it != list.cend(); ++it) {
    cout << *it << " <-> ";
  }
  cout << "nullptr\n";

  // Reverse traversal
  cout << "  Reverse:  nullptr";
  auto it = list.cend();
  while (it != list.cbegin()) {
    --it;
    cout << " <-> " << *it;
  }
  cout << "\n";
}

// Demo: Basic push operations
void demo_basic_operations() {
  cout << "\n========== Demo: Basic Operations ==========\n";

  DoublyLinkedList<int> list;

  cout << "Creating empty list...\n";
  cout << "  Size: " << list.size() << ", Empty: " << (list.is_empty() ? "yes" : "no") << "\n";

  // Push back operations
  cout << "\nPushing elements to back: 10, 20, 30\n";
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);
  print_list(list, "after push_back");

  // Push front operations
  cout << "\nPushing elements to front: 5, 0\n";
  list.push_front(5);
  list.push_front(0);
  print_list(list, "after push_front");

  // Access front and back
  cout << "\nFront element: " << list.front() << "\n";
  cout << "Back element: " << list.back() << "\n";
}

// Demo: Pop operations
void demo_pop_operations() {
  cout << "\n========== Demo: Pop Operations ==========\n";

  DoublyLinkedList<int> list;
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i * 10);
  }
  print_list(list, "original");

  cout << "\nPopping from front...\n";
  list.pop_front();
  cout << "  New front: " << list.front() << "\n";
  print_list(list, "after pop_front");

  cout << "\nPopping from back...\n";
  list.pop_back();
  cout << "  New back: " << list.back() << "\n";
  print_list(list, "after pop_back");
}

// Demo: Insert and erase operations
void demo_insert_erase() {
  cout << "\n========== Demo: Insert and Erase ==========\n";

  DoublyLinkedList<int> list;
  list.push_back(10);
  list.push_back(30);
  list.push_back(40);
  print_list(list, "original");

  // Insert in the middle
  cout << "\nInserting 20 at position 1 (before 30):\n";
  auto it = list.begin();
  ++it; // Point to 30
  list.insert(it, 20);
  print_list(list, "after insert");

  // Erase from the middle
  cout << "\nErasing element at position 2 (value 30):\n";
  it = list.begin();
  ++it;
  ++it; // Point to 30
  it = list.erase(it);
  cout << "  Iterator now points to: " << *it << "\n";
  print_list(list, "after erase");

  // Insert at beginning
  cout << "\nInserting 5 at the beginning:\n";
  list.insert(list.begin(), 5);
  print_list(list, "after insert at begin");

  // Insert at end
  cout << "\nInserting 50 at the end:\n";
  list.insert(list.end(), 50);
  print_list(list, "after insert at end");
}

// Demo: Emplace operations
void demo_emplace() {
  cout << "\n========== Demo: Emplace Operations ==========\n";

  struct Person {
    string name;
    int    age;

    Person(string n, int a) : name(std::move(n)), age(a) {}
  };

  DoublyLinkedList<Person> people;

  cout << "Emplacing Person objects directly:\n";
  people.emplace_back("Alice", 30);
  people.emplace_back("Charlie", 35);
  people.emplace_front("Bob", 25);

  cout << "\nPeople in list:\n";
  for (const auto& person : people) {
    cout << "  " << person.name << " (age " << person.age << ")\n";
  }
}

// Demo: Iterators
void demo_iterators() {
  cout << "\n========== Demo: Iterators ==========\n";

  DoublyLinkedList<int> list;
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i);
  }

  // Range-based for loop
  cout << "Using range-based for loop:\n  ";
  for (const int& value : list) {
    cout << value << " ";
  }
  cout << "\n";

  // Explicit iterator usage
  cout << "\nUsing explicit iterators:\n  ";
  for (auto it = list.begin(); it != list.end(); ++it) {
    cout << *it << " ";
  }
  cout << "\n";

  // Const iterators
  const auto& const_list = list;
  cout << "\nUsing const iterators:\n  ";
  for (auto it = const_list.cbegin(); it != const_list.cend(); ++it) {
    cout << *it << " ";
  }
  cout << "\n";

  // Modifying through iterator
  cout << "\nModifying elements through iterator (doubling values):\n";
  for (int& value : list) {
    value *= 2;
  }
  print_list(list, "after modification");

  // Collecting into vector
  cout << "\nCollecting into vector:\n  ";
  vector<int> vec(list.begin(), list.end());
  cout << "Vector contents: ";
  for (int v : vec) {
    cout << v << " ";
  }
  cout << "\n";
}

// Demo: Reverse operation
void demo_reverse() {
  cout << "\n========== Demo: Reverse ==========\n";

  DoublyLinkedList<int> list;
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i);
  }
  print_list(list, "original");

  list.reverse();
  print_list(list, "reversed");

  // Reverse again
  list.reverse();
  print_list(list, "reversed again (back to original)");

  // Edge case: single element
  DoublyLinkedList<int> single;
  single.push_back(42);
  cout << "\nSingle element before reverse: " << single.front() << "\n";
  single.reverse();
  cout << "Single element after reverse: " << single.front() << "\n";
}

// Demo: Move semantics
void demo_move_semantics() {
  cout << "\n========== Demo: Move Semantics ==========\n";

  DoublyLinkedList<int> list1;
  for (int i = 1; i <= 3; ++i) {
    list1.push_back(i * 10);
  }

  cout << "Original list:\n";
  print_list(list1, "list1");

  // Move constructor
  cout << "\nMove constructing list2 from list1:\n";
  DoublyLinkedList<int> list2 = std::move(list1);
  print_list(list1, "list1 (should be empty)");
  print_list(list2, "list2 (should have data)");

  // Move assignment
  DoublyLinkedList<int> list3;
  list3.push_back(99);
  list3.push_back(100);

  cout << "\nBefore move assignment:\n";
  print_list(list3, "list3");

  cout << "\nMove assigning list2 to list3:\n";
  list3 = std::move(list2);
  print_list(list2, "list2 (should be empty)");
  print_list(list3, "list3 (should have list2's data)");
}

// Demo: Clear and reuse
void demo_clear() {
  cout << "\n========== Demo: Clear and Reuse ==========\n";

  DoublyLinkedList<int> list;
  for (int i = 0; i < 5; ++i) {
    list.push_back(i);
  }
  print_list(list, "before clear");

  list.clear();
  cout << "\nAfter clear:\n";
  cout << "  Size: " << list.size() << ", Empty: " << (list.is_empty() ? "yes" : "no") << "\n";

  // Reuse after clear
  cout << "\nReusing list after clear:\n";
  list.push_back(100);
  list.push_back(200);
  print_list(list, "after reuse");
}

// Demo: Exception handling
void demo_exceptions() {
  cout << "\n========== Demo: Exception Handling ==========\n";

  DoublyLinkedList<int> empty_list;

  cout << "Testing exceptions on empty list:\n";

  try {
    cout << "  Trying front() on empty list...\n";
    [[maybe_unused]] auto val = empty_list.front();
    cout << "    ERROR: No exception thrown!\n";
  } catch (const ListException& e) {
    cout << "    Correctly caught: " << e.what() << "\n";
  }

  try {
    cout << "  Trying back() on empty list...\n";
    [[maybe_unused]] auto val = empty_list.back();
    cout << "    ERROR: No exception thrown!\n";
  } catch (const ListException& e) {
    cout << "    Correctly caught: " << e.what() << "\n";
  }

  try {
    cout << "  Trying pop_front() on empty list...\n";
    empty_list.pop_front();
    cout << "    ERROR: No exception thrown!\n";
  } catch (const ListException& e) {
    cout << "    Correctly caught: " << e.what() << "\n";
  }

  try {
    cout << "  Trying pop_back() on empty list...\n";
    empty_list.pop_back();
    cout << "    ERROR: No exception thrown!\n";
  } catch (const ListException& e) {
    cout << "    Correctly caught: " << e.what() << "\n";
  }
}

// Demo: Edge cases
void demo_edge_cases() {
  cout << "\n========== Demo: Edge Cases ==========\n";

  // Single element list
  DoublyLinkedList<int> single;
  single.push_back(42);
  cout << "Single element list:\n";
  cout << "  Size: " << single.size() << "\n";
  cout << "  Front: " << single.front() << ", Back: " << single.back() << "\n";
  cout << "  Front == Back? " << (single.front() == single.back() ? "yes" : "no") << "\n";

  // Pop from single element list
  single.pop_back();
  cout << "\nAfter popping single element:\n";
  cout << "  Size: " << single.size() << ", Empty: " << (single.is_empty() ? "yes" : "no") << "\n";

  // Large list
  cout << "\nLarge list test (1000 elements):\n";
  DoublyLinkedList<int> large;
  for (int i = 0; i < 1000; ++i) {
    large.push_back(i);
  }
  cout << "  Size: " << large.size() << "\n";
  cout << "  Front: " << large.front() << ", Back: " << large.back() << "\n";

  large.reverse();
  cout << "  After reverse - Front: " << large.front() << ", Back: " << large.back() << "\n";
}

auto main() -> int {
  try {
    cout << "╔═══----------------------------------------------------═══╗\n";
    cout << "          DOUBLY LINKED LIST - COMPREHENSIVE DEMO           \n";
    cout << "╚═══----------------------------------------------------═══╝\n";

    demo_basic_operations();
    demo_pop_operations();
    demo_insert_erase();
    demo_emplace();
    demo_iterators();
    demo_reverse();
    demo_move_semantics();
    demo_clear();
    demo_exceptions();
    demo_edge_cases();

    cout << "\n";
    cout << "╔═══----------------------------------------------------═══╗\n";
    cout << "             ALL DEMOS COMPLETED SUCCESSFULLY!              \n";
    cout << "╚═══----------------------------------------------------═══╝\n";

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
