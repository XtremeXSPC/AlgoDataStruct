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

using namespace std;

// Helper function to print the list and iterators properties
void print_list(const ads::list::DoublyLinkedList<int>& list, const string& name) {
  cout << "Contents of '" << name << "' (size: " << list.size() << "):\n  ";
  // Use cbegin() and cend() to iteratorerate over a const list
  for (auto iterator = list.cbegin(); iterator != list.cend(); ++iterator) {
    cout << *iterator << " <-> ";
  }
  cout << "nullptr\n";

  // Also print in reverse order to check `prev` pointers
  cout << "  (Reverse): nullptr";
  if (!list.is_empty()) {
    auto iterator = list.cend();
    do {
      --iterator;
      cout << " <-> " << *iterator;
    } while (iterator != list.cbegin());
  }
  cout << '\n';
}

auto main() -> int {
  try {
    ads::list::DoublyLinkedList<int> myList;

    cout << "----------- Adding elements ------------" << '\n';
    myList.push_back(10);
    myList.push_back(20);
    myList.push_front(5);
    myList.emplace_back(30); // Use emplace
    print_list(myList, "myList");

    cout << "\n------ Iteration and manipulation ------" << '\n';
    auto iterator = myList.begin();
    ++iterator;     // iterator points to 10
    *iterator = 15; // Modify the value
    print_list(myList, "myList after modification");

    cout << "\n-------- Insertion and deletion --------" << '\n';
    iterator = myList.insert(iterator, 7); // Inserts 7 before 15, iterator now points to 7
    print_list(myList, "myList after insert");

    ++iterator;                        // iterator now points to 15
    iterator = myList.erase(iterator); // Removes 15, iterator now points to 20
    cout << "Element after the one removed: " << *iterator << '\n';
    print_list(myList, "myList after erase");

    cout << "\n------------ List reversal -------------" << '\n';
    myList.reverse();
    print_list(myList, "myList reversed");

    cout << "\n-------------- Move test ---------------" << '\n';
    ads::list::DoublyLinkedList<int> anotherList = std::move(myList);
    print_list(anotherList, "anotherList (moved)");
    print_list(myList, "myList (empty after move)");

    // ----- Exception Handling Test ----- //
    cout << "\n------- Exception Handling Test --------" << '\n';
    cout << "Trying to call front() on an empty list..." << '\n';
    // myList is empty after the move
    // This call will throw an exception
    myList.front();

  } catch (const ads::list::ListException& e) {
    cerr << "ERROR CORRECTLY CAUGHT: " << e.what() << '\n';
  } catch (const exception& e) {
    cerr << "Unexpected generic error: " << e.what() << '\n';
  }

  return 0;
}