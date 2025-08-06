//===--------------------------------------------------------------------------===//
/**
 * @file main.cc
 * @author Costantino Lombardi
 * @brief Tester for list functions
 * @version 0.2
 * @date 2025-06-30
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//

#include "../include/ads/lists/Doubly_Linked_List.hpp"
#include <iostream>
#include <string>

// Helper function to print the list and iterators properties
void print_list(const ads::list::DoublyLinkedList<int>& list, const std::string& name) {
  std::cout << "Contents of '" << name << "' (size: " << list.size() << "):\n  ";
  // Use cbegin() and cend() to iteratorerate over a const list
  for (auto iterator = list.cbegin(); iterator != list.cend(); ++iterator) {
    std::cout << *iterator << " <-> ";
  }
  std::cout << "nullptr\n";

  // Also print in reverse order to check `prev` pointers
  std::cout << "  (Reverse): nullptr";
  if (!list.is_empty()) {
    auto iterator = list.cend();
    do {
      --iterator;
      std::cout << " <-> " << *iterator;
    } while (iterator != list.cbegin());
  }
  std::cout << '\n';
}

auto main() -> int {
  try {
    ads::list::DoublyLinkedList<int> myList;

    std::cout << "----------- Adding elements ------------" << '\n';
    myList.push_back(10);
    myList.push_back(20);
    myList.push_front(5);
    myList.emplace_back(30); // Use emplace
    print_list(myList, "myList");

    std::cout << "\n------ Iteration and manipulation ------" << '\n';
    auto iterator = myList.begin();
    ++iterator;     // iterator points to 10
    *iterator = 15; // Modify the value
    print_list(myList, "myList after modification");

    std::cout << "\n-------- Insertion and deletion --------" << '\n';
    iterator = myList.insert(iterator, 7); // Inserts 7 before 15, iterator now points to 7
    print_list(myList, "myList after insert");

    ++iterator;                        // iterator now points to 15
    iterator = myList.erase(iterator); // Removes 15, iterator now points to 20
    std::cout << "Element after the one removed: " << *iterator << '\n';
    print_list(myList, "myList after erase");

    std::cout << "\n------------ List reversal -------------" << '\n';
    myList.reverse();
    print_list(myList, "myList reversed");

    std::cout << "\n-------------- Move test ---------------" << '\n';
    ads::list::DoublyLinkedList<int> anotherList = std::move(myList);
    print_list(anotherList, "anotherList (moved)");
    print_list(myList, "myList (empty after move)");

    // ----- Exception Handling Test ----- //
    std::cout << "\n------- Exception Handling Test --------" << '\n';
    std::cout << "Trying to call front() on an empty list..." << '\n';
    // myList is empty after the move
    // This call will throw an exception
    myList.front();

  } catch (const ads::list::ListException& e) {
    std::cerr << "ERROR CORRECTLY CAUGHT: " << e.what() << '\n';
  } catch (const std::exception& e) {
    std::cerr << "Unexpected generic error: " << e.what() << '\n';
  }

  return 0;
}