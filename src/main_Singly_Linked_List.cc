//===--------------------------------------------------------------------------===//
/**
 * @file main_Singly_Linked_List.cc
 * @author Costantino Lombardi
 * @brief Test file for SinglyLinkedList class
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include <iostream>
#include <string>

#include "../include/ads/lists/Singly_Linked_List.hpp"

using namespace ads::list;

void print_separator(const std::string& title) {
  std::cout << "\n========== " << title << " ==========\n";
}

void test_basic_operations() {
  print_separator("Test Basic Operations");

  SinglyLinkedList<int> list;

  std::cout << "Empty list created. is_empty(): " << std::boolalpha << list.is_empty() << '\n';
  std::cout << "Size: " << list.size() << '\n';

  // Test push_front
  std::cout << "\nPushing to front: 3, 2, 1\n";
  list.push_front(3);
  list.push_front(2);
  list.push_front(1);

  std::cout << "List contents (should be 1 2 3): ";
  for (const auto& val : list) {
    std::cout << val << ' ';
  }
  std::cout << "\nSize: " << list.size() << '\n';
  std::cout << "Front: " << list.front() << ", Back: " << list.back() << '\n';

  // Test push_back
  std::cout << "\nPushing to back: 4, 5, 6\n";
  list.push_back(4);
  list.push_back(5);
  list.push_back(6);

  std::cout << "List contents (should be 1 2 3 4 5 6): ";
  for (const auto& val : list) {
    std::cout << val << ' ';
  }
  std::cout << "\nSize: " << list.size() << '\n';
  std::cout << "Front: " << list.front() << ", Back: " << list.back() << '\n';

  // Test pop_front
  std::cout << "\nPopping from front twice...\n";
  list.pop_front();
  list.pop_front();

  std::cout << "List contents (should be 3 4 5 6): ";
  for (const auto& val : list) {
    std::cout << val << ' ';
  }
  std::cout << "\nSize: " << list.size() << '\n';

  // Test pop_back
  std::cout << "\nPopping from back once...\n";
  list.pop_back();

  std::cout << "List contents (should be 3 4 5): ";
  for (const auto& val : list) {
    std::cout << val << ' ';
  }
  std::cout << "\nSize: " << list.size() << '\n';
}

void test_emplace_operations() {
  print_separator("Test Emplace Operations");

  SinglyLinkedList<std::string> list;

  std::cout << "Emplacing strings...\n";
  list.emplace_front("Hello");
  list.emplace_back("World");
  list.emplace_front("Say");

  std::cout << "List contents: ";
  for (const auto& str : list) {
    std::cout << str << ' ';
  }
  std::cout << "\nSize: " << list.size() << '\n';
}

void test_reverse() {
  print_separator("Test Reverse Operation");

  SinglyLinkedList<int> list;
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i);
  }

  std::cout << "Original list: ";
  for (const auto& val : list) {
    std::cout << val << ' ';
  }
  std::cout << '\n';

  list.reverse();

  std::cout << "Reversed list: ";
  for (const auto& val : list) {
    std::cout << val << ' ';
  }
  std::cout << '\n';
  std::cout << "Front: " << list.front() << ", Back: " << list.back() << '\n';

  // Test reverse with single element
  SinglyLinkedList<int> single;
  single.push_back(42);
  single.reverse();
  std::cout << "Single element reversed: " << single.front() << '\n';

  // Test reverse with empty list
  SinglyLinkedList<int> empty;
  empty.reverse();
  std::cout << "Empty list reversed (size should be 0): " << empty.size() << '\n';
}

void test_clear() {
  print_separator("Test Clear Operation");

  SinglyLinkedList<int> list;
  for (int i = 0; i < 10; ++i) {
    list.push_back(i);
  }

  std::cout << "List size before clear: " << list.size() << '\n';
  list.clear();
  std::cout << "List size after clear: " << list.size() << '\n';
  std::cout << "is_empty(): " << std::boolalpha << list.is_empty() << '\n';
}

void test_move_semantics() {
  print_separator("Test Move Semantics");

  SinglyLinkedList<int> list1;
  for (int i = 1; i <= 5; ++i) {
    list1.push_back(i);
  }

  std::cout << "List1 contents: ";
  for (const auto& val : list1) {
    std::cout << val << ' ';
  }
  std::cout << "\nList1 size: " << list1.size() << '\n';

  // Test move constructor
  std::cout << "\nMoving list1 to list2 (move constructor)...\n";
  SinglyLinkedList<int> list2(std::move(list1));

  std::cout << "List2 contents: ";
  for (const auto& val : list2) {
    std::cout << val << ' ';
  }
  std::cout << "\nList2 size: " << list2.size() << '\n';
  std::cout << "List1 size after move: " << list1.size() << '\n';

  // Test move assignment
  SinglyLinkedList<int> list3;
  list3.push_back(99);
  std::cout << "\nMoving list2 to list3 (move assignment)...\n";
  list3 = std::move(list2);

  std::cout << "List3 contents: ";
  for (const auto& val : list3) {
    std::cout << val << ' ';
  }
  std::cout << "\nList3 size: " << list3.size() << '\n';
  std::cout << "List2 size after move: " << list2.size() << '\n';
}

void test_exception_handling() {
  print_separator("Test Exception Handling");

  SinglyLinkedList<int> list;

  std::cout << "Testing operations on empty list...\n";

  try {
    list.front();
    std::cout << "ERROR: front() should throw on empty list\n";
  } catch (const ListException& e) {
    std::cout << "Caught expected exception for front(): " << e.what() << '\n';
  }

  try {
    list.back();
    std::cout << "ERROR: back() should throw on empty list\n";
  } catch (const ListException& e) {
    std::cout << "Caught expected exception for back(): " << e.what() << '\n';
  }

  try {
    list.pop_front();
    std::cout << "ERROR: pop_front() should throw on empty list\n";
  } catch (const ListException& e) {
    std::cout << "Caught expected exception for pop_front(): " << e.what() << '\n';
  }

  try {
    list.pop_back();
    std::cout << "ERROR: pop_back() should throw on empty list\n";
  } catch (const ListException& e) {
    std::cout << "Caught expected exception for pop_back(): " << e.what() << '\n';
  }
}

void test_large_list() {
  print_separator("Test Large List");

  SinglyLinkedList<int> list;
  const int             N = 10000;

  std::cout << "Inserting " << N << " elements...\n";
  for (int i = 0; i < N; ++i) {
    list.push_back(i);
  }

  std::cout << "List size: " << list.size() << '\n';
  std::cout << "Front: " << list.front() << ", Back: " << list.back() << '\n';

  std::cout << "Reversing list...\n";
  list.reverse();
  std::cout << "After reverse - Front: " << list.front() << ", Back: " << list.back() << '\n';

  std::cout << "Clearing list...\n";
  list.clear();
  std::cout << "Size after clear: " << list.size() << '\n';

  std::cout << "Large list test completed successfully (no stack overflow)\n";
}

void test_const_iterator() {
  print_separator("Test Const Iterator");

  SinglyLinkedList<int> list;
  for (int i = 1; i <= 5; ++i) {
    list.push_back(i);
  }

  const SinglyLinkedList<int>& const_list = list;

  std::cout << "Using const_iterator: ";
  for (auto it = const_list.cbegin(); it != const_list.cend(); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << '\n';

  std::cout << "Using const range-based for: ";
  for (const auto& val : const_list) {
    std::cout << val << ' ';
  }
  std::cout << '\n';
}

void test_rvalue_insertion() {
  print_separator("Test RValue Insertion");

  SinglyLinkedList<std::string> list;

  std::string s1 = "Move1";
  std::string s2 = "Move2";

  std::cout << "Before move - s1: " << s1 << ", s2: " << s2 << '\n';

  list.push_front(std::move(s1));
  list.push_back(std::move(s2));

  std::cout << "After move - s1: '" << s1 << "', s2: '" << s2 << "'\n";
  std::cout << "List contents: ";
  for (const auto& str : list) {
    std::cout << str << ' ';
  }
  std::cout << '\n';
}

int main() {
  std::cout << "==============================================\n";
  std::cout << "  SINGLY LINKED LIST - COMPREHENSIVE TESTS\n";
  std::cout << "==============================================\n";

  try {
    test_basic_operations();
    test_emplace_operations();
    test_reverse();
    test_clear();
    test_move_semantics();
    test_exception_handling();
    test_large_list();
    test_const_iterator();
    test_rvalue_insertion();

    print_separator("ALL TESTS COMPLETED SUCCESSFULLY");
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "\n!!! UNEXPECTED EXCEPTION !!!\n";
    std::cerr << "What: " << e.what() << '\n';
    return 1;
  }
}

//===--------------------------------------------------------------------------===//
