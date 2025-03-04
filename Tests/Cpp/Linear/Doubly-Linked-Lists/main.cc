#include "DoublyLinekdList.hpp"
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

// Helper function to print the list state
template <typename T>
void printListState(const ads::DoublyLinkedList<T>& list, const std::string& label) {
  std::cout << "\n===== " << label << " =====\n";
  std::cout << "List content: " << list << std::endl;
  std::cout << "Size: " << list.size() << std::endl;
  std::cout << "Empty: " << (list.empty() ? "Yes" : "No") << std::endl;

  // Try to access front and back elements with error handling
  if (!list.empty()) {
    try {
      std::cout << "Front: " << list.front() << std::endl;
      std::cout << "Back: " << list.back() << std::endl;
    } catch (const std::exception& e) {
      std::cerr << "Error accessing elements: " << e.what() << std::endl;
    }
  }
}

// Function to test invalid operations
template <typename T>
void testInvalidOperations(ads::DoublyLinkedList<T>& list) {
  std::cout << "\n===== Testing Invalid Operations =====\n";

  // Testing pop_front on empty list
  try {
    std::cout << "Attempting to pop_front from empty list... ";
    list.pop_front();
    std::cout << "Succeeded (shouldn't happen)\n";
  } catch (const std::exception& e) {
    std::cout << "Caught exception as expected: " << e.what() << std::endl;
  }

  // Testing pop_back on empty list
  try {
    std::cout << "Attempting to pop_back from empty list... ";
    list.pop_back();
    std::cout << "Succeeded (shouldn't happen)\n";
  } catch (const std::exception& e) {
    std::cout << "Caught exception as expected: " << e.what() << std::endl;
  }

  // Testing front on empty list
  try {
    std::cout << "Attempting to access front of empty list... ";
    T value = list.front();
    std::cout << "Succeeded with value: " << value << " (shouldn't happen)\n";
  } catch (const std::exception& e) {
    std::cout << "Caught exception as expected: " << e.what() << std::endl;
  }

  // Testing back on empty list
  try {
    std::cout << "Attempting to access back of empty list... ";
    T value = list.back();
    std::cout << "Succeeded with value: " << value << " (shouldn't happen)\n";
  } catch (const std::exception& e) {
    std::cout << "Caught exception as expected: " << e.what() << std::endl;
  }

  // Testing erase with invalid iterator
  try {
    std::cout << "Attempting to erase with end iterator... ";
    list.erase(list.end());
    std::cout << "Succeeded (shouldn't happen)\n";
  } catch (const std::exception& e) {
    std::cout << "Caught exception as expected: " << e.what() << std::endl;
  }
}

// Function to test the list with numeric types (int, double)
template <typename T>
void testDoublyLinkedList(const std::string& typeName) {
  std::cout << "\n\n***** TESTING DOUBLY LINKED LIST WITH " << typeName << " *****\n";

  try {
    // Create an empty list
    ads::DoublyLinkedList<T> list;
    printListState(list, "Empty List");

    // Test invalid operations on empty list
    testInvalidOperations(list);

    // Add elements to the list
    std::cout << "\n===== Adding Elements =====\n";
    for (T i = 1; i <= 5; ++i) {
      list.push_back(i);
      std::cout << "After push_back(" << i << "): " << list << std::endl;
    }

    for (T i = 0; i >= -4; --i) {
      list.push_front(i);
      std::cout << "After push_front(" << i << "): " << list << std::endl;
    }

    printListState(list, "After Adding Elements");

    // Test iterator-based loop
    std::cout << "\n===== Iterator-based Loop =====\n";
    std::cout << "Forward iteration: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
      std::cout << *it << " ";
    }
    std::cout << std::endl;

    // Test range-based for loop
    std::cout << "\n===== Range-based For Loop =====\n";
    std::cout << "Elements: ";
    for (const auto& item : list) {
      std::cout << item << " ";
    }
    std::cout << std::endl;

    // Test find operation
    std::cout << "\n===== Testing Find =====\n";
    T    searchValue = 3;
    auto foundIt     = list.find(searchValue);
    if (foundIt != list.end()) {
      std::cout << "Found value " << searchValue << " in the list\n";
      std::cout << "Modifying found value to " << searchValue * 10 << std::endl;
      *foundIt = searchValue * 10;
    } else {
      std::cout << "Value " << searchValue << " not found in the list\n";
    }
    std::cout << "List after modification: " << list << std::endl;

    // Test for_each
    std::cout << "\n===== Testing for_each =====\n";
    std::cout << "Doubling all positive values:\n";
    list.for_each([](T& value) {
      if (value > 0) {
        value *= 2;
      }
    });
    std::cout << "List after for_each: " << list << std::endl;

    // Test insert operation
    std::cout << "\n===== Testing Insert =====\n";
    auto insertPos = list.find(0); // Insert after 0
    if (insertPos != list.end()) {
      std::cout << "Inserting value 100 after 0\n";
      list.insert(++insertPos, T(100));
    }
    std::cout << "List after insert: " << list << std::endl;

    // Test erase operation
    std::cout << "\n===== Testing Erase =====\n";
    auto erasePos = list.find(-2);
    if (erasePos != list.end()) {
      std::cout << "Erasing value -2\n";
      erasePos = list.erase(erasePos);
      std::cout << "Next element after erase: " << (erasePos != list.end() ? *erasePos : T{}) << std::endl;
    }
    std::cout << "List after erase: " << list << std::endl;

    // Test pop operations
    std::cout << "\n===== Testing Pop Operations =====\n";
    std::cout << "Original list: " << list << std::endl;
    list.pop_front();
    std::cout << "After pop_front: " << list << std::endl;
    list.pop_back();
    std::cout << "After pop_back: " << list << std::endl;

    // Test list with initializer_list
    std::cout << "\n===== Testing initializer_list Constructor =====\n";
    ads::DoublyLinkedList<T> list2 = {10, 20, 30, 40, 50};
    printListState(list2, "List created with initializer_list");

    // Test comparison operators
    std::cout << "\n===== Testing Comparison Operators =====\n";
    ads::DoublyLinkedList<T> list3 = list;
    ads::DoublyLinkedList<T> list4 = {1, 2, 3};

    std::cout << "list: " << list << std::endl;
    std::cout << "list3 (copy of list): " << list3 << std::endl;
    std::cout << "list4: " << list4 << std::endl;

    std::cout << "list == list3: " << (list == list3 ? "true" : "false") << std::endl;
    std::cout << "list != list3: " << (list != list3 ? "true" : "false") << std::endl;
    std::cout << "list == list4: " << (list == list4 ? "true" : "false") << std::endl;
    std::cout << "list != list4: " << (list != list4 ? "true" : "false") << std::endl;

    // Test clear operation
    std::cout << "\n===== Testing Clear =====\n";
    std::cout << "Before clear: " << list << std::endl;
    list.clear();
    std::cout << "After clear: " << list << std::endl;
    std::cout << "Size after clear: " << list.size() << std::endl;
    std::cout << "Is empty after clear: " << (list.empty() ? "Yes" : "No") << std::endl;

    // Retest invalid operations after clearing
    testInvalidOperations(list);

  } catch (const std::exception& e) {
    std::cerr << "*** Unhandled exception: " << e.what() << " ***\n";
  }
}

// Specializzazione per std::string
template <>
void testDoublyLinkedList<std::string>(const std::string& typeName) {
  std::cout << "\n\n***** TESTING DOUBLY LINKED LIST WITH " << typeName << " *****\n";

  try {
    // Create an empty list
    ads::DoublyLinkedList<std::string> list;
    printListState(list, "Empty List");

    // Test invalid operations on empty list
    testInvalidOperations(list);

    // Add elements to the list
    std::cout << "\n===== Adding Elements =====\n";

    const std::string values[] = {"uno", "due", "tre", "quattro", "cinque"};
    for (const auto& value : values) {
      list.push_back(value);
      std::cout << "After push_back(\"" << value << "\"): " << list << std::endl;
    }

    const std::string prefixValues[] = {"zero", "meno uno", "meno due", "meno tre", "meno quattro"};
    for (const auto& value : prefixValues) {
      list.push_front(value);
      std::cout << "After push_front(\"" << value << "\"): " << list << std::endl;
    }

    printListState(list, "After Adding Elements");

    // Test iterator-based loop
    std::cout << "\n===== Iterator-based Loop =====\n";
    std::cout << "Forward iteration: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
      std::cout << "\"" << *it << "\" ";
    }
    std::cout << std::endl;

    // Test range-based for loop
    std::cout << "\n===== Range-based For Loop =====\n";
    std::cout << "Elements: ";
    for (const auto& item : list) {
      std::cout << "\"" << item << "\" ";
    }
    std::cout << std::endl;

    // Test find operation
    std::cout << "\n===== Testing Find =====\n";
    std::string searchValue = "tre";
    auto        foundIt     = list.find(searchValue);
    if (foundIt != list.end()) {
      std::cout << "Found value \"" << searchValue << "\" in the list\n";
      std::cout << "Modifying found value to \"" << searchValue << "_modificato\"" << std::endl;
      *foundIt = searchValue + "_modificato";
    } else {
      std::cout << "Value \"" << searchValue << "\" not found in the list\n";
    }
    std::cout << "List after modification: " << list << std::endl;

    // Test for_each
    std::cout << "\n===== Testing for_each =====\n";
    std::cout << "Adding suffix to all values:\n";
    list.for_each([](std::string& value) {
      if (!value.empty() && value[0] != 'm') { // Solo se non inizia con 'm'
        value += "_suffisso";
      }
    });
    std::cout << "List after for_each: " << list << std::endl;

    // Test insert operation
    std::cout << "\n===== Testing Insert =====\n";
    auto insertPos = list.find("zero"); // Insert after "zero"
    if (insertPos != list.end()) {
      std::cout << "Inserting value \"cento\" after \"zero\"\n";
      list.insert(++insertPos, std::string("cento"));
    }
    std::cout << "List after insert: " << list << std::endl;

    // Test erase operation
    std::cout << "\n===== Testing Erase =====\n";
    auto erasePos = list.find("meno due");
    if (erasePos != list.end()) {
      std::cout << "Erasing value \"meno due\"\n";
      erasePos = list.erase(erasePos);
      std::cout << "Next element after erase: " << (erasePos != list.end() ? "\"" + *erasePos + "\"" : "end()") << std::endl;
    }
    std::cout << "List after erase: " << list << std::endl;

    // Test pop operations
    std::cout << "\n===== Testing Pop Operations =====\n";
    std::cout << "Original list: " << list << std::endl;
    list.pop_front();
    std::cout << "After pop_front: " << list << std::endl;
    list.pop_back();
    std::cout << "After pop_back: " << list << std::endl;

    // Test list with initializer_list
    std::cout << "\n===== Testing initializer_list Constructor =====\n";
    ads::DoublyLinkedList<std::string> list2 = {"dieci", "venti", "trenta", "quaranta", "cinquanta"};
    printListState(list2, "List created with initializer_list");

    // Test comparison operators
    std::cout << "\n===== Testing Comparison Operators =====\n";
    ads::DoublyLinkedList<std::string> list3 = list;
    ads::DoublyLinkedList<std::string> list4 = {"uno", "due", "tre"};

    std::cout << "list: " << list << std::endl;
    std::cout << "list3 (copy of list): " << list3 << std::endl;
    std::cout << "list4: " << list4 << std::endl;

    std::cout << "list == list3: " << (list == list3 ? "true" : "false") << std::endl;
    std::cout << "list != list3: " << (list != list3 ? "true" : "false") << std::endl;
    std::cout << "list == list4: " << (list == list4 ? "true" : "false") << std::endl;
    std::cout << "list != list4: " << (list != list4 ? "true" : "false") << std::endl;

    // Test clear operation
    std::cout << "\n===== Testing Clear =====\n";
    std::cout << "Before clear: " << list << std::endl;
    list.clear();
    std::cout << "After clear: " << list << std::endl;
    std::cout << "Size after clear: " << list.size() << std::endl;
    std::cout << "Is empty after clear: " << (list.empty() ? "Yes" : "No") << std::endl;

    // Retest invalid operations after clearing
    testInvalidOperations(list);

  } catch (const std::exception& e) {
    std::cerr << "*** Unhandled exception: " << e.what() << " ***\n";
  }
}

// Class to test the list with user-defined types
class TestObject {
private:
  int         id;
  std::string name;

public:
  TestObject(int id = 0, std::string name = "Default") : id(id), name(std::move(name)) {}

  bool operator==(const TestObject& other) const { return id == other.id && name == other.name; }

  bool operator!=(const TestObject& other) const { return !(*this == other); }

  friend std::ostream& operator<<(std::ostream& os, const TestObject& obj) {
    os << "{id:" << obj.id << ", name:\"" << obj.name << "\"}";
    return os;
  }
};

int main() {
  try {
    // Test with int
    testDoublyLinkedList<int>("INT");

    // Test with double
    testDoublyLinkedList<double>("DOUBLE");

    // Test with string
    testDoublyLinkedList<std::string>("STRING");

    // Test with user-defined type
    std::cout << "\n\n***** TESTING DOUBLY LINKED LIST WITH CUSTOM CLASS *****\n";
    ads::DoublyLinkedList<TestObject> objectList;

    objectList.push_back(TestObject(1, "First"));
    objectList.push_back(TestObject(2, "Second"));
    objectList.push_back(TestObject(3, "Third"));

    std::cout << "Object list: " << objectList << std::endl;

    // Search for an object
    TestObject searchObj(2, "Second");
    auto       foundIt = objectList.find(searchObj);
    if (foundIt != objectList.end()) {
      std::cout << "Found object: " << *foundIt << std::endl;
    } else {
      std::cout << "Object not found" << std::endl;
    }

    std::cout << "\nAll tests completed successfully!" << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "*** Fatal error: " << e.what() << " ***" << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "*** Unknown fatal error occurred ***" << std::endl;
    return 2;
  }

  return 0;
}