//===---------------------------------------------------------------------------===//
/**
 * @file main_Static_Array.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for StaticArray implementation.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the StaticArray data structure,
 * showcasing construction, access, iteration, comparison, and utility operations.
 */
//===---------------------------------------------------------------------------===//

#include <iostream>
#include <string>

#include "../include/ads/arrays/Static_Array.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using namespace ads::arrays;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

template <typename T, size_t N>
void print_array(const StaticArray<T, N>& array, const string& label) {
  cout << label << " (size: " << array.size() << ")\n";
  cout << "  ";
  for (const auto& value : array) {
    cout << value << " ";
  }
  cout << "\n";
}

//===--------------------------- CONSTRUCTION DEMO -----------------------------===//

void demo_construction() {
  ads::demo::print_section("Demo: Construction");

  cout << "1. Default construction (value-initialized):\n";
  StaticArray<int, 5> default_arr;
  print_array(default_arr, "   Default array");

  cout << "\n2. Initializer list construction:\n";
  StaticArray<int, 5> init_arr{1, 2, 3, 4, 5};
  print_array(init_arr, "   Initialized array");

  cout << "\n3. Fill construction:\n";
  StaticArray<int, 5> fill_arr(42);
  print_array(fill_arr, "   Filled array");

  cout << "\n4. Copy construction:\n";
  StaticArray<int, 5> copy_arr(init_arr);
  print_array(copy_arr, "   Copied array");

  ads::demo::print_success("All construction methods work correctly.");
}

//===--------------------------- ACCESS OPERATIONS DEMO ------------------------===//

void demo_access_operations() {
  ads::demo::print_section("Demo: Access Operations");

  StaticArray<int, 5> array{10, 20, 30, 40, 50};
  print_array(array, "Array");

  cout << "\n1. Subscript operator access:\n";
  cout << "   array[0] = " << array[0] << "\n";
  cout << "   array[2] = " << array[2] << "\n";
  cout << "   array[4] = " << array[4] << "\n";

  cout << "\n2. at() with bounds checking:\n";
  cout << "   array.at(1) = " << array.at(1) << "\n";
  cout << "   array.at(3) = " << array.at(3) << "\n";

  cout << "\n3. front() and back():\n";
  cout << "   array.front() = " << array.front() << "\n";
  cout << "   array.back() = " << array.back() << "\n";

  cout << "\n4. data() pointer access:\n";
  int* ptr = array.data();
  cout << "   *data() = " << *ptr << "\n";
  cout << "   *(data() + 2) = " << *(ptr + 2) << "\n";

  cout << "\n5. Modifying elements:\n";
  array[0] = 100;
  array.at(4) = 500;
  print_array(array, "   Modified array");

  ads::demo::print_success("All access operations work correctly.");
}

//===------------------------------ ITERATION DEMO -----------------------------===//

void demo_iteration() {
  ads::demo::print_section("Demo: Iteration");

  StaticArray<int, 5> array{1, 2, 3, 4, 5};
  print_array(array, "Array");

  cout << "\n1. Range-based for loop:\n   ";
  for (const auto& val : array) {
    cout << val << " ";
  }
  cout << "\n";

  cout << "\n2. Iterator-based loop:\n   ";
  for (auto it = array.begin(); it != array.end(); ++it) {
    cout << *it << " ";
  }
  cout << "\n";

  cout << "\n3. Reverse iteration:\n   ";
  for (auto it = array.rbegin(); it != array.rend(); ++it) {
    cout << *it << " ";
  }
  cout << "\n";

  cout << "\n4. Const iteration:\n   ";
  for (auto it = array.cbegin(); it != array.cend(); ++it) {
    cout << *it << " ";
  }
  cout << "\n";

  ads::demo::print_success("All iteration methods work correctly.");
}

//===--------------------------- COMPARISON DEMO -------------------------------===//

void demo_comparison() {
  ads::demo::print_section("Demo: Comparison");

  StaticArray<int, 5> arr1{1, 2, 3, 4, 5};
  StaticArray<int, 5> arr2{1, 2, 3, 4, 5};
  StaticArray<int, 5> arr3{1, 2, 3, 4, 6};
  StaticArray<int, 5> arr4{1, 2, 3, 4, 4};

  print_array(arr1, "arr1");
  print_array(arr2, "arr2");
  print_array(arr3, "arr3");
  print_array(arr4, "arr4");

  cout << "\n1. Equality comparison:\n";
  cout << "   arr1 == arr2: " << (arr1 == arr2 ? "true" : "false") << "\n";
  cout << "   arr1 == arr3: " << (arr1 == arr3 ? "true" : "false") << "\n";

  cout << "\n2. Three-way comparison (spaceship operator):\n";
  auto cmp1 = arr1 <=> arr2;
  auto cmp2 = arr1 <=> arr3;
  auto cmp3 = arr1 <=> arr4;
  cout << "   arr1 <=> arr2: " << (cmp1 == 0 ? "equal" : (cmp1 < 0 ? "less" : "greater")) << "\n";
  cout << "   arr1 <=> arr3: " << (cmp2 == 0 ? "equal" : (cmp2 < 0 ? "less" : "greater")) << "\n";
  cout << "   arr1 <=> arr4: " << (cmp3 == 0 ? "equal" : (cmp3 < 0 ? "less" : "greater")) << "\n";

  ads::demo::print_success("All comparison operations work correctly.");
}

//===--------------------------- UTILITY OPERATIONS DEMO -----------------------===//

void demo_utility_operations() {
  ads::demo::print_section("Demo: Utility Operations");

  StaticArray<int, 5> arr1{1, 2, 3, 4, 5};
  StaticArray<int, 5> arr2{10, 20, 30, 40, 50};

  cout << "Before swap:\n";
  print_array(arr1, "  arr1");
  print_array(arr2, "  arr2");

  cout << "\nSwapping arrays...\n";
  arr1.swap(arr2);

  cout << "\nAfter swap:\n";
  print_array(arr1, "  arr1");
  print_array(arr2, "  arr2");

  cout << "\nFilling arr1 with 99:\n";
  arr1.fill(99);
  print_array(arr1, "  arr1");

  ads::demo::print_success("All utility operations work correctly.");
}

//===--------------------------- EXCEPTION HANDLING DEMO -----------------------===//

void demo_exception_handling() {
  ads::demo::print_section("Demo: Exception Handling");

  StaticArray<int, 5> array{1, 2, 3, 4, 5};

  cout << "Testing out-of-bounds access with at():\n";
  try {
    [[maybe_unused]] auto val = array.at(10);
    ads::demo::print_error("  Exception not thrown (unexpected).");
  } catch (const ArrayOutOfRangeException& e) {
    ads::demo::print_info("  Caught expected exception: ");
    cout << e.what() << "\n";
  }

  ads::demo::print_success("Exception handling works correctly.");
}

//===--------------------------- STRING TYPE DEMO ------------------------------===//

void demo_string_type() {
  ads::demo::print_section("Demo: String Type Support");

  StaticArray<string, 3> strings{"Hello", "World", "C++20"};
  print_array(strings, "String array");

  cout << "\nAccessing elements:\n";
  cout << "  strings[0] = \"" << strings[0] << "\"\n";
  cout << "  strings[1] = \"" << strings[1] << "\"\n";
  cout << "  strings[2] = \"" << strings[2] << "\"\n";

  cout << "\nModifying strings:\n";
  strings[1] = "StaticArray";
  print_array(strings, "Modified array");

  ads::demo::print_success("String type support works correctly.");
}

//===----------------------------------- MAIN ----------------------------------===//

auto main() -> int {
  ads::demo::print_header("StaticArray Demo");

  try {
    demo_construction();
    demo_access_operations();
    demo_iteration();
    demo_comparison();
    demo_utility_operations();
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
