#include "DoublyLinekdList.hpp"
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

// Helper function to print the list state
template <typename T>
void printListState(const ads::DoublyLinkedList<T>& list, const string& label) {
  cout << "\n===== " << label << " =====\n";
  cout << "List content: " << list << endl;
  cout << "Size: " << list.size() << endl;
  cout << "Empty: " << (list.empty() ? "Yes" : "No") << endl;

  // Try to access front and back elements with error handling
  if (!list.empty()) {
    try {
      cout << "Front: " << list.front() << endl;
      cout << "Back: " << list.back() << endl;
    } catch (const exception& e) {
      cerr << "Error accessing elements: " << e.what() << endl;
    }
  }
}

// Function to test invalid operations
template <typename T>
void testInvalidOperations(ads::DoublyLinkedList<T>& list) {
  cout << "\n===== Testing Invalid Operations =====\n";

  // Testing pop_front on empty list
  try {
    cout << "Attempting to pop_front from empty list... ";
    list.pop_front();
    cout << "Succeeded (shouldn't happen)\n";
  } catch (const exception& e) {
    cout << "Caught exception as expected: " << e.what() << endl;
  }

  // Testing pop_back on empty list
  try {
    cout << "Attempting to pop_back from empty list... ";
    list.pop_back();
    cout << "Succeeded (shouldn't happen)\n";
  } catch (const exception& e) {
    cout << "Caught exception as expected: " << e.what() << endl;
  }

  // Testing front on empty list
  try {
    cout << "Attempting to access front of empty list... ";
    T value = list.front();
    cout << "Succeeded with value: " << value << " (shouldn't happen)\n";
  } catch (const exception& e) {
    cout << "Caught exception as expected: " << e.what() << endl;
  }

  // Testing back on empty list
  try {
    cout << "Attempting to access back of empty list... ";
    T value = list.back();
    cout << "Succeeded with value: " << value << " (shouldn't happen)\n";
  } catch (const exception& e) {
    cout << "Caught exception as expected: " << e.what() << endl;
  }

  // Testing erase with invalid iterator
  try {
    cout << "Attempting to erase with end iterator... ";
    list.erase(list.end());
    cout << "Succeeded (shouldn't happen)\n";
  } catch (const exception& e) {
    cout << "Caught exception as expected: " << e.what() << endl;
  }
}

// Function to test the list with numeric types (int, double)
template <typename T>
void testDoublyLinkedList(const string& typeName) {
  cout << "\n\n***** TESTING DOUBLY LINKED LIST WITH " << typeName << " *****\n";

  try {
    // Create an empty list
    ads::DoublyLinkedList<T> list;
    printListState(list, "Empty List");

    // Test invalid operations on empty list
    testInvalidOperations(list);

    // Add elements to the list
    cout << "\n===== Adding Elements =====\n";
    for (T i = 1; i <= 5; ++i) {
      list.push_back(i);
      cout << "After push_back(" << i << "): " << list << endl;
    }

    for (T i = 0; i >= -4; --i) {
      list.push_front(i);
      cout << "After push_front(" << i << "): " << list << endl;
    }

    printListState(list, "After Adding Elements");

    // Test iterator-based loop
    cout << "\n===== Iterator-based Loop =====\n";
    cout << "Forward iteration: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
      cout << *it << " ";
    }
    cout << endl;

    // Test range-based for loop
    cout << "\n===== Range-based For Loop =====\n";
    cout << "Elements: ";
    for (const auto& item : list) {
      cout << item << " ";
    }
    cout << endl;

    // Test find operation
    cout << "\n===== Testing Find =====\n";
    T    searchValue = 3;
    auto foundIt     = list.find(searchValue);
    if (foundIt != list.end()) {
      cout << "Found value " << searchValue << " in the list\n";
      cout << "Modifying found value to " << searchValue * 10 << endl;
      *foundIt = searchValue * 10;
    } else {
      cout << "Value " << searchValue << " not found in the list\n";
    }
    cout << "List after modification: " << list << endl;

    // Test for_each
    cout << "\n===== Testing for_each =====\n";
    cout << "Doubling all positive values:\n";
    list.for_each([](T& value) {
      if (value > 0) {
        value *= 2;
      }
    });
    cout << "List after for_each: " << list << endl;

    // Test insert operation
    cout << "\n===== Testing Insert =====\n";
    auto insertPos = list.find(0); // Insert after 0
    if (insertPos != list.end()) {
      cout << "Inserting value 100 after 0\n";
      list.insert(++insertPos, T(100));
    }
    cout << "List after insert: " << list << endl;

    // Test erase operation
    cout << "\n===== Testing Erase =====\n";
    auto erasePos = list.find(-2);
    if (erasePos != list.end()) {
      cout << "Erasing value -2\n";
      erasePos = list.erase(erasePos);
      cout << "Next element after erase: " << (erasePos != list.end() ? *erasePos : T{}) << endl;
    }
    cout << "List after erase: " << list << endl;

    // Test pop operations
    cout << "\n===== Testing Pop Operations =====\n";
    cout << "Original list: " << list << endl;
    list.pop_front();
    cout << "After pop_front: " << list << endl;
    list.pop_back();
    cout << "After pop_back: " << list << endl;

    // Test list with initializer_list
    cout << "\n===== Testing initializer_list Constructor =====\n";
    ads::DoublyLinkedList<T> list2 = {10, 20, 30, 40, 50};
    printListState(list2, "List created with initializer_list");

    // Test comparison operators
    cout << "\n===== Testing Comparison Operators =====\n";
    ads::DoublyLinkedList<T> list3 = list;
    ads::DoublyLinkedList<T> list4 = {1, 2, 3};

    cout << "list: " << list << endl;
    cout << "list3 (copy of list): " << list3 << endl;
    cout << "list4: " << list4 << endl;

    cout << "list == list3: " << (list == list3 ? "true" : "false") << endl;
    cout << "list != list3: " << (list != list3 ? "true" : "false") << endl;
    cout << "list == list4: " << (list == list4 ? "true" : "false") << endl;
    cout << "list != list4: " << (list != list4 ? "true" : "false") << endl;

    // Test clear operation
    cout << "\n===== Testing Clear =====\n";
    cout << "Before clear: " << list << endl;
    list.clear();
    cout << "After clear: " << list << endl;
    cout << "Size after clear: " << list.size() << endl;
    cout << "Is empty after clear: " << (list.empty() ? "Yes" : "No") << endl;

    // Retest invalid operations after clearing
    testInvalidOperations(list);

  } catch (const exception& e) {
    cerr << "*** Unhandled exception: " << e.what() << " ***\n";
  }
}

// Specializzazione per  string
template <>
void testDoublyLinkedList<string>(const string& typeName) {
  cout << "\n\n***** TESTING DOUBLY LINKED LIST WITH " << typeName << " *****\n";

  try {
    // Create an empty list
    ads::DoublyLinkedList<string> list;
    printListState(list, "Empty List");

    // Test invalid operations on empty list
    testInvalidOperations(list);

    // Add elements to the list
    cout << "\n===== Adding Elements =====\n";

    const string values[] = {"uno", "due", "tre", "quattro", "cinque"};
    for (const auto& value : values) {
      list.push_back(value);
      cout << "After push_back(\"" << value << "\"): " << list << endl;
    }

    const string prefixValues[] = {"zero", "meno uno", "meno due", "meno tre", "meno quattro"};
    for (const auto& value : prefixValues) {
      list.push_front(value);
      cout << "After push_front(\"" << value << "\"): " << list << endl;
    }

    printListState(list, "After Adding Elements");

    // Test iterator-based loop
    cout << "\n===== Iterator-based Loop =====\n";
    cout << "Forward iteration: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
      cout << "\"" << *it << "\" ";
    }
    cout << endl;

    // Test range-based for loop
    cout << "\n===== Range-based For Loop =====\n";
    cout << "Elements: ";
    for (const auto& item : list) {
      cout << "\"" << item << "\" ";
    }
    cout << endl;

    // Test find operation
    cout << "\n===== Testing Find =====\n";
    string searchValue = "tre";
    auto   foundIt     = list.find(searchValue);
    if (foundIt != list.end()) {
      cout << "Found value \"" << searchValue << "\" in the list\n";
      cout << "Modifying found value to \"" << searchValue << "_modificato\"" << endl;
      *foundIt = searchValue + "_modificato";
    } else {
      cout << "Value \"" << searchValue << "\" not found in the list\n";
    }
    cout << "List after modification: " << list << endl;

    // Test for_each
    cout << "\n===== Testing for_each =====\n";
    cout << "Adding suffix to all values:\n";
    list.for_each([](string& value) {
      if (!value.empty() && value[0] != 'm') { // Solo se non inizia con 'm'
        value += "_suffisso";
      }
    });
    cout << "List after for_each: " << list << endl;

    // Test insert operation
    cout << "\n===== Testing Insert =====\n";
    auto insertPos = list.find("zero"); // Insert after "zero"
    if (insertPos != list.end()) {
      cout << "Inserting value \"cento\" after \"zero\"\n";
      list.insert(++insertPos, string("cento"));
    }
    cout << "List after insert: " << list << endl;

    // Test erase operation
    cout << "\n===== Testing Erase =====\n";
    auto erasePos = list.find("meno due");
    if (erasePos != list.end()) {
      cout << "Erasing value \"meno due\"\n";
      erasePos = list.erase(erasePos);
      cout << "Next element after erase: " << (erasePos != list.end() ? "\"" + *erasePos + "\"" : "end()") << endl;
    }
    cout << "List after erase: " << list << endl;

    // Test pop operations
    cout << "\n===== Testing Pop Operations =====\n";
    cout << "Original list: " << list << endl;
    list.pop_front();
    cout << "After pop_front: " << list << endl;
    list.pop_back();
    cout << "After pop_back: " << list << endl;

    // Test list with initializer_list
    cout << "\n===== Testing initializer_list Constructor =====\n";
    ads::DoublyLinkedList<string> list2 = {"dieci", "venti", "trenta", "quaranta", "cinquanta"};
    printListState(list2, "List created with initializer_list");

    // Test comparison operators
    cout << "\n===== Testing Comparison Operators =====\n";
    ads::DoublyLinkedList<string> list3 = list;
    ads::DoublyLinkedList<string> list4 = {"uno", "due", "tre"};

    cout << "list: " << list << endl;
    cout << "list3 (copy of list): " << list3 << endl;
    cout << "list4: " << list4 << endl;

    cout << "list == list3: " << (list == list3 ? "true" : "false") << endl;
    cout << "list != list3: " << (list != list3 ? "true" : "false") << endl;
    cout << "list == list4: " << (list == list4 ? "true" : "false") << endl;
    cout << "list != list4: " << (list != list4 ? "true" : "false") << endl;

    // Test clear operation
    cout << "\n===== Testing Clear =====\n";
    cout << "Before clear: " << list << endl;
    list.clear();
    cout << "After clear: " << list << endl;
    cout << "Size after clear: " << list.size() << endl;
    cout << "Is empty after clear: " << (list.empty() ? "Yes" : "No") << endl;

    // Retest invalid operations after clearing
    testInvalidOperations(list);

  } catch (const exception& e) {
    cerr << "*** Unhandled exception: " << e.what() << " ***\n";
  }
}

// Class to test the list with user-defined types
class TestObject {
private:
  int    id;
  string name;

public:
  TestObject(int id = 0, string name = "Default") : id(id), name(std::move(name)) {}

  bool operator==(const TestObject& other) const { return id == other.id && name == other.name; }

  bool operator!=(const TestObject& other) const { return !(*this == other); }

  friend ostream& operator<<(ostream& os, const TestObject& obj) {
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
    testDoublyLinkedList<string>("STRING");

    // Test with user-defined type
    cout << "\n\n***** TESTING DOUBLY LINKED LIST WITH CUSTOM CLASS *****\n";
    ads::DoublyLinkedList<TestObject> objectList;

    objectList.push_back(TestObject(1, "First"));
    objectList.push_back(TestObject(2, "Second"));
    objectList.push_back(TestObject(3, "Third"));

    cout << "Object list: " << objectList << endl;

    // Search for an object
    TestObject searchObj(2, "Second");
    auto       foundIt = objectList.find(searchObj);
    if (foundIt != objectList.end()) {
      cout << "Found object: " << *foundIt << endl;
    } else {
      cout << "Object not found" << endl;
    }

    cout << "\nAll tests completed successfully!" << endl;

  } catch (const exception& e) {
    cerr << "*** Fatal error: " << e.what() << " ***" << endl;
    return 1;
  } catch (...) {
    cerr << "*** Unknown fatal error occurred ***" << endl;
    return 2;
  }

  return 0;
}