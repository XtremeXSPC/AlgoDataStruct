//===--------------------------------------------------------------------------===//
/**
 * @file main_Singly_Linked_List.cc
 * @author Costantino Lombardi
 * @brief Demonstration of SinglyLinkedList usage.
 * @version 0.2
 * @date 2025-01-20
 *
 * This file is a quick visual demo, not a test suite. Unit tests live in /tests.
 */
//===--------------------------------------------------------------------------===//

#include <exception>
#include <iostream>
#include <string>
#include <utility>

#include "../include/ads/lists/Singly_Linked_List.hpp"
#include "ads/support/ConsoleColors.hpp"

using ads::list::ListException;
using ads::list::SinglyLinkedList;

template <typename T>
void print_list(const SinglyLinkedList<T>& list, const std::string& label) {
  std::cout << CYAN << label << RESET << " (size " << list.size() << "): ";
  for (const auto& v : list) {
    std::cout << v << ' ';
  }
  std::cout << '\n';
}

void demo_basics() {
  std::cout << BOLD << BLUE << "\n-- Basic push/pop --" << RESET << '\n';

  SinglyLinkedList<int> list;
  list.push_front(3);
  list.push_front(2);
  list.push_front(1);
  list.push_back(4);
  list.push_back(5);

  print_list(list, "After pushes");
  std::cout << "front=" << list.front() << " back=" << list.back() << '\n';

  list.pop_front();
  list.pop_back();
  print_list(list, "After pop_front + pop_back");
}

void demo_emplace_and_reverse() {
  std::cout << BOLD << BLUE << "\n-- Emplace and reverse --" << RESET << '\n';

  SinglyLinkedList<std::string> words;
  words.emplace_front("World");
  words.emplace_front("Hello");
  words.emplace_back("!");

  print_list(words, "Before reverse");
  words.reverse();
  print_list(words, "After reverse");
}

void demo_move_semantics() {
  std::cout << BOLD << BLUE << "\n-- Move semantics --" << RESET << '\n';

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

void demo_exceptions() {
  std::cout << BOLD << BLUE << "\n-- Exception handling --" << RESET << '\n';

  SinglyLinkedList<int> empty;
  try {
    empty.front();
  } catch (const ListException& e) {
    std::cout << YELLOW << "front() threw as expected: " << e.what() << RESET << '\n';
  }
}

auto main() -> int {
  std::cout << BOLD << BLUE << "=================================\n" << RESET;
  std::cout << BOLD << "  Singly Linked List Demo\n" << RESET;
  std::cout << BOLD << BLUE << "=================================\n" << RESET;

  demo_basics();
  demo_emplace_and_reverse();
  demo_move_semantics();
  demo_exceptions();

  std::cout << BOLD << BLUE << "\nDone. Run ENABLE_TESTING=ON to execute the full unit tests.\n" << RESET;
  return 0;
}

//===--------------------------------------------------------------------------===//
