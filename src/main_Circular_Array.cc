//===---------------------------------------------------------------------------===//
/**
 * @file main_Circular_Array.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for CircularArray implementation.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the CircularArray data structure,
 * showcasing circular buffer behavior, dual-ended operations, and wrap-around.
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/arrays/Circular_Array.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using namespace ads::arrays;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Helper function to print array contents.
template <typename T>
void print_array(const CircularArray<T>& array, const string& label) {
  cout << label << " (size: " << array.size() << ", capacity: " << array.capacity() << ")\n";
  if (array.is_empty()) {
    cout << "  (empty)\n";
    return;
  }
  cout << "  [ ";
  for (const auto& value : array) {
    cout << value << " ";
  }
  cout << "]\n";
}

//===-------------------------- BASIC OPERATIONS DEMO ---------------------------===//

// Demonstrate basic push_back, push_front, front, back operations.
void demo_basic_operations() {
  ads::demo::print_section("Demo: Basic Operations");

  CircularArray<int> array;
  cout << "Created empty circular array.\n";
  print_array(array, "Initial state");

  cout << "\nPushing to back: 10, 20, 30\n";
  array.push_back(10);
  array.push_back(20);
  array.push_back(30);
  print_array(array, "After push_back");

  cout << "\nPushing to front: 5, 0\n";
  array.push_front(5);
  array.push_front(0);
  print_array(array, "After push_front");

  cout << "\nFront element: " << array.front() << "\n";
  cout << "Back element: " << array.back() << "\n";

  ads::demo::print_success("Basic operations work correctly.");
}

//===----------------------- DUAL-ENDED OPERATIONS DEMO ------------------------===//

// Demonstrate push/pop at both ends.
void demo_dual_ended() {
  ads::demo::print_section("Demo: Dual-Ended Operations");

  CircularArray<int> array;

  cout << "Building array with alternating push_front/push_back:\n";
  cout << "  push_back(0)\n";
  array.push_back(0);
  cout << "  push_front(-1)\n";
  array.push_front(-1);
  cout << "  push_back(1)\n";
  array.push_back(1);
  cout << "  push_front(-2)\n";
  array.push_front(-2);
  cout << "  push_back(2)\n";
  array.push_back(2);

  print_array(array, "Result");
  cout << "Expected: [ -2 -1 0 1 2 ]\n";

  cout << "\nRemoving from both ends:\n";
  cout << "  pop_front() removes: " << array.front() << "\n";
  array.pop_front();
  cout << "  pop_back() removes: " << array.back() << "\n";
  array.pop_back();
  print_array(array, "After removals");

  ads::demo::print_success("Dual-ended operations work correctly.");
}

//===------------------------ WRAP-AROUND BEHAVIOR DEMO ------------------------===//

// Demonstrate wrap-around behavior.
void demo_wrap_around() {
  ads::demo::print_section("Demo: Wrap-Around Behavior");

  CircularArray<int> array(8);
  cout << "Created array with capacity 8.\n";

  cout << "\n1. Fill array halfway:\n";
  for (int i = 0; i < 4; ++i) {
    array.push_back(i);
  }
  print_array(array, "   After adding 0-3");

  cout << "\n2. Remove first 2 elements (creates space at front):\n";
  array.pop_front();
  array.pop_front();
  print_array(array, "   After removing 2 elements");
  cout << "   Internal head has moved forward.\n";

  cout << "\n3. Add more elements (will wrap around):\n";
  for (int i = 4; i < 10; ++i) {
    array.push_back(i);
  }
  print_array(array, "   After adding 4-9");
  cout << "   Elements wrap around in internal buffer.\n";

  cout << "\n4. Verify logical order is preserved:\n";
  cout << "   Logical indices: ";
  for (size_t i = 0; i < array.size(); ++i) {
    cout << "[" << i << "]=" << array[i] << " ";
  }
  cout << "\n";

  ads::demo::print_success("Wrap-around behavior works correctly.");
}

//===----------------------------- ITERATION DEMO ------------------------------===//

// Demonstrate iteration over circular array.
void demo_iteration() {
  ads::demo::print_section("Demo: Iteration");

  CircularArray<int> array;
  for (int i = 1; i <= 5; ++i) {
    array.push_back(i);
  }
  array.pop_front();
  array.pop_front();
  for (int i = 6; i <= 8; ++i) {
    array.push_back(i);
  }

  print_array(array, "Array (with internal wrap-around)");

  cout << "\n1. Range-based for loop (forward):\n   ";
  for (const auto& val : array) {
    cout << val << " ";
  }
  cout << "\n";

  cout << "\n2. Reverse iteration:\n   ";
  for (auto it = array.rbegin(); it != array.rend(); ++it) {
    cout << *it << " ";
  }
  cout << "\n";

  cout << "\n3. Index-based access:\n   ";
  for (size_t i = 0; i < array.size(); ++i) {
    cout << array[i] << " ";
  }
  cout << "\n";

  ads::demo::print_success("Iteration works correctly with wrap-around.");
}

//===------------------------ CAPACITY MANAGEMENT DEMO -------------------------===//

// Demonstrate capacity management functions.
void demo_capacity() {
  ads::demo::print_section("Demo: Capacity Management");

  CircularArray<int> array;
  cout << "Initial capacity: " << array.capacity() << "\n";

  cout << "\n1. Reserve capacity for 50 elements:\n";
  array.reserve(50);
  cout << "   Capacity after reserve: " << array.capacity() << "\n";

  cout << "\n2. Add 40 elements:\n";
  for (int i = 0; i < 40; ++i) {
    array.push_back(i);
  }
  cout << "   Size: " << array.size() << ", Capacity: " << array.capacity() << "\n";

  cout << "\n3. Remove 30 elements:\n";
  for (int i = 0; i < 30; ++i) {
    array.pop_front();
  }
  cout << "   Size: " << array.size() << ", Capacity: " << array.capacity() << "\n";

  cout << "\n4. Shrink to fit:\n";
  array.shrink_to_fit();
  cout << "   Size: " << array.size() << ", Capacity: " << array.capacity() << "\n";

  ads::demo::print_success("Capacity management works correctly.");
}

//===------------------------- EXCEPTION HANDLING DEMO -------------------------===//

// Demonstrate exception handling for out-of-bounds and underflow.
void demo_exception_handling() {
  ads::demo::print_section("Demo: Exception Handling");

  CircularArray<int> array;

  cout << "Testing pop_front on empty array:\n";
  try {
    array.pop_front();
    ads::demo::print_error("  Exception not thrown (unexpected).");
  } catch (const ArrayUnderflowException& e) {
    ads::demo::print_info("  Caught expected exception: ");
    cout << e.what() << "\n";
  }

  cout << "\nTesting pop_back on empty array:\n";
  try {
    array.pop_back();
    ads::demo::print_error("  Exception not thrown (unexpected).");
  } catch (const ArrayUnderflowException& e) {
    ads::demo::print_info("  Caught expected exception: ");
    cout << e.what() << "\n";
  }

  array.push_back(42);
  cout << "\nTesting at() with out-of-bounds index:\n";
  try {
    [[maybe_unused]] auto val = array.at(10);
    ads::demo::print_error("  Exception not thrown (unexpected).");
  } catch (const ArrayOutOfRangeException& e) {
    ads::demo::print_info("  Caught expected exception: ");
    cout << e.what() << "\n";
  }

  ads::demo::print_success("Exception handling works correctly.");
}

//===---------------------------- STRING TYPE DEMO -----------------------------===//

// Demonstrate CircularArray with string type.
void demo_string_type() {
  ads::demo::print_section("Demo: String Type Support");

  CircularArray<string> strings;
  strings.emplace_back("World");
  strings.emplace_front("Hello");
  strings.emplace_back("!");

  print_array(strings, "String array");

  cout << "\nBuilding sentence: ";
  for (const auto& s : strings) {
    cout << s << " ";
  }
  cout << "\n";

  ads::demo::print_success("String type support works correctly.");
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  ads::demo::print_header("CircularArray Demo");

  try {
    demo_basic_operations();
    demo_dual_ended();
    demo_wrap_around();
    demo_iteration();
    demo_capacity();
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
