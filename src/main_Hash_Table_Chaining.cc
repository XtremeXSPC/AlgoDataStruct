//===---------------------------------------------------------------------------===//
/**
 * @file main_Hash_Table_Chaining.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for Hash Table Chaining implementation.
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the Hash Table with Chaining data structure,
 * showcasing its insertion, access, updates, and deletion operations.
 */
//===---------------------------------------------------------------------------===//

#include <chrono>
#include <format>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "../include/ads/hash/Hash_Table_Chaining.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::to_string;
using std::unordered_map;
using std::vector;

using namespace ads::hash;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Helper function to print hash table statistics.
template <typename Key, typename Value>
void print_stats(const HashTableChaining<Key, Value>& table, const string& name) {
  cout << "Hash Table '" << name << "':\n";
  cout << "  Size:        " << table.size() << '\n';
  cout << "  Capacity:    " << table.capacity() << '\n';
  cout << "  Load Factor: " << table.load_factor() << '\n';
  cout << "  Max LF:      " << table.max_load_factor() << '\n';
  cout << "  Empty:       " << (table.is_empty() ? "Yes" : "No") << '\n';
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Demonstrates basic insertion and access.
void demo_basic_operations() {
  ads::demo::print_section("Demo: Basic Operations");

  HashTableChaining<int, string> table;

  cout << "Inserting key-value pairs:\n";
  table.insert(1, "one");
  table.insert(2, "two");
  table.insert(3, "three");
  table.insert(4, "four");
  table.insert(5, "five");

  print_stats(table, "after insertions");

  // Test access.
  cout << "\nAccessing values:\n";
  cout << "  table[1] = " << table.at(1) << '\n';
  cout << "  table[3] = " << table.at(3) << '\n';
  cout << "  table[5] = " << table.at(5) << '\n';

  // Test contains.
  cout << "\nTesting contains():\n";
  cout << "  Contains 3? " << (table.contains(3) ? "Yes" : "No") << '\n';
  cout << "  Contains 10? " << (table.contains(10) ? "Yes" : "No") << '\n';
}

//===------------------------------ UPDATES DEMO -------------------------------===//

// Demonstrates updating existing keys.
void demo_updates() {
  ads::demo::print_section("Demo: Updates");

  HashTableChaining<string, int> table;

  table.insert("apple", 5);
  table.insert("banana", 3);
  table.insert("orange", 7);

  cout << "Initial values:\n";
  cout << "  apple: " << table["apple"] << '\n';
  cout << "  banana: " << table["banana"] << '\n';

  // Update existing keys.
  cout << "\nUpdating values:\n";
  table.insert("apple", 10);
  table["banana"] = 15;

  cout << "After updates:\n";
  cout << "  apple: " << table["apple"] << '\n';
  cout << "  banana: " << table["banana"] << '\n';

  print_stats(table, "after updates");
}

//===----------------------------- OPERATOR[] DEMO -----------------------------===//

// Demonstrates operator[].
void demo_bracket_operator() {
  ads::demo::print_section("Demo: Operator[]");

  HashTableChaining<string, int> table;

  cout << "Using operator[] to insert and access:\n";
  table["first"]  = 1;
  table["second"] = 2;
  table["third"]  = 3;

  cout << "  first: " << table["first"] << '\n';
  cout << "  second: " << table["second"] << '\n';
  cout << "  third: " << table["third"] << '\n';

  // Access non-existent key (should create with default value).
  cout << "\nAccessing non-existent key 'fourth':\n";
  int value = table["fourth"];
  cout << "  Value: " << value << " (default-constructed)\n";
  cout << "  Table size is now: " << table.size() << '\n';
}

//===-------------------------------- FIND DEMO --------------------------------===//

// Demonstrates find method.
void demo_find() {
  ads::demo::print_section("Demo: Find");

  HashTableChaining<int, string> table;
  table.insert(10, "ten");
  table.insert(20, "twenty");
  table.insert(30, "thirty");

  cout << "Testing find():\n";

  const auto* result = table.find(20);
  if (result) {
    cout << "  Found key 20: " << *result << '\n';
  } else {
    cout << "  Key 20 not found\n";
  }

  result = table.find(100);
  if (result) {
    cout << "  Found key 100: " << *result << '\n';
  } else {
    cout << "  Key 100 not found (correct)\n";
  }
}

//===------------------------------- ERASE DEMO --------------------------------===//

// Demonstrates erase method.
void demo_erase() {
  ads::demo::print_section("Demo: Erase");

  HashTableChaining<int, string> table;

  for (int i = 1; i <= 10; ++i) {
    table.insert(i, std::format("value_{}", i));
  }

  print_stats(table, "before erasure");

  cout << "\nErasing keys 3, 5, 7:\n";
  cout << "  Erase 3: " << (table.erase(3) ? "success" : "failed") << '\n';
  cout << "  Erase 5: " << (table.erase(5) ? "success" : "failed") << '\n';
  cout << "  Erase 7: " << (table.erase(7) ? "success" : "failed") << '\n';

  cout << "\nErasing non-existent key 100:\n";
  cout << "  Erase 100: " << (table.erase(100) ? "success" : "failed (correct)") << '\n';

  print_stats(table, "after erasure");

  // Verify remaining keys.
  cout << "\nVerifying remaining keys:\n";
  for (int i = 1; i <= 10; ++i) {
    cout << "  Contains " << i << "? " << (table.contains(i) ? "Yes" : "No") << '\n';
  }
}

//===----------------------------- REHASHING DEMO ------------------------------===//

// Demonstrates rehashing.
void demo_rehashing() {
  ads::demo::print_section("Demo: Rehashing");

  // Small initial capacity.
  HashTableChaining<int, int> table(4, 0.75f);

  cout << "Initial state:\n";
  print_stats(table, "empty table");

  cout << "\nInserting 20 elements to trigger rehashing:\n";
  for (int i = 1; i <= 20; ++i) {
    table.insert(i, i * 10);
    if (i % 5 == 0) {
      cout << "After " << i << " insertions: capacity=" << table.capacity() << ", load_factor=" << table.load_factor() << '\n';
    }
  }

  print_stats(table, "after insertions with auto-rehash");

  // Verify all values are still accessible after rehashing.
  cout << "\nVerifying all values after rehashing:\n";
  bool all_found = true;
  for (int i = 1; i <= 20; ++i) {
    if (!table.contains(i) || table[i] != i * 10) {
      all_found = false;
      cout << "  ERROR: Key " << i << " not found or has wrong value!\n";
    }
  }
  if (all_found) {
    cout << "  ✓ All 20 values correctly preserved after rehashing\n";
  }
}

//===------------------------------ RESERVE DEMO -------------------------------===//

// Demonstrates reserve method.
void demo_reserve() {
  ads::demo::print_section("Demo: Reserve");

  HashTableChaining<int, string> table;

  cout << "Initial capacity: " << table.capacity() << '\n';

  cout << "\nReserving capacity for 100 elements:\n";
  table.reserve(100);
  cout << "New capacity: " << table.capacity() << '\n';

  // Insert many elements without triggering rehash.
  cout << "\nInserting 50 elements:\n";
  for (int i = 0; i < 50; ++i) {
    table.insert(i, "value");
  }

  cout << "Capacity after insertions: " << table.capacity() << " (should still be 100)\n";
  print_stats(table, "after reserve and insertions");
}

// Test with custom types.
struct Person {
  string name;
  int    age;

  Person() : name(""), age(0) {}
  Person(const string& n, int a) : name(n), age(a) {}

  bool operator==(const Person& other) const { return name == other.name && age == other.age; }
};

//===---------------------------- CUSTOM TYPES DEMO ----------------------------===//

// Demonstrates usage with custom types.
void demo_custom_types() {
  ads::demo::print_section("Demo: Custom Types");

  HashTableChaining<string, Person> table;

  table.insert("alice", Person("Alice", 30));
  table.insert("bob", Person("Bob", 25));
  table.insert("charlie", Person("Charlie", 35));

  cout << "Accessing custom types:\n";
  cout << "  alice: " << table["alice"].name << ", age " << table["alice"].age << '\n';
  cout << "  bob: " << table["bob"].name << ", age " << table["bob"].age << '\n';
  cout << "  charlie: " << table["charlie"].name << ", age " << table["charlie"].age << '\n';

  print_stats(table, "custom types table");
}

//===------------------------- COLLISION HANDLING DEMO -------------------------===//

// Demonstrates collision handling.
void demo_collisions() {
  ads::demo::print_section("Demo: Collision Handling");

  // Use small capacity to force collisions
  HashTableChaining<int, string> table(4);

  cout << "Inserting values that will collide (capacity=4):\n";
  // Keys 0, 4, 8, 12 will all hash to the same bucket (0),
  table.insert(0, "zero");
  table.insert(4, "four");
  table.insert(8, "eight");
  table.insert(12, "twelve");

  print_stats(table, "with collisions");

  cout << "\nVerifying all colliding values are accessible:\n";
  cout << "  table[0] = " << table[0] << '\n';
  cout << "  table[4] = " << table[4] << '\n';
  cout << "  table[8] = " << table[8] << '\n';
  cout << "  table[12] = " << table[12] << '\n';
}

//===------------------------- EXCEPTION HANDLING DEMO -------------------------===//

// Demonstrates exception handling.
void demo_exceptions() {
  ads::demo::print_section("Demo: Exception Handling");

  HashTableChaining<int, string> table;
  table.insert(1, "one");
  table.insert(2, "two");

  // Test at() with non-existent key.
  cout << "Testing at() with non-existent key:\n";
  try {
    table.at(100);
    cout << "  ERROR: No exception thrown!\n";
  } catch (const exception& e) {
    cout << "  Caught exception: " << e.what() << '\n';
  }

  // Test invalid max_load_factor.
  cout << "\nTesting set_max_load_factor with invalid value:\n";
  try {
    table.set_max_load_factor(-0.5f);
    cout << "  ERROR: No exception thrown!\n";
  } catch (const exception& e) {
    cout << "  Caught exception: " << e.what() << '\n';
  }
}

//===--------------------------- MOVE SEMANTICS DEMO ---------------------------===//

// Demonstrates move semantics.
void demo_move_semantics() {
  ads::demo::print_section("Demo: Move Semantics");

  HashTableChaining<int, string> table1;
  table1.insert(1, "one");
  table1.insert(2, "two");
  table1.insert(3, "three");

  cout << "Original table:\n";
  print_stats(table1, "table1");

  // Move constructor.
  HashTableChaining<int, string> table2(std::move(table1));
  cout << "\nAfter move construction:\n";
  print_stats(table2, "table2");
  cout << "table1 size: " << table1.size() << " (should be 0)\n";

  // Move assignment.
  HashTableChaining<int, string> table3;
  table3 = std::move(table2);
  cout << "\nAfter move assignment:\n";
  print_stats(table3, "table3");
  cout << "table2 size: " << table2.size() << " (should be 0)\n";
}

//===------------------------------- CLEAR DEMO --------------------------------===//

// Demonstrates clear method.
void demo_clear() {
  ads::demo::print_section("Demo: Clear");

  HashTableChaining<int, int> table;
  for (int i = 0; i < 100; ++i) {
    table.insert(i, i * i);
  }

  cout << "Before clear:\n";
  print_stats(table, "table");

  table.clear();

  cout << "\nAfter clear:\n";
  print_stats(table, "table");

  cout << "\nInserting after clear:\n";
  table.insert(42, 1764);
  cout << "  Contains 42? " << (table.contains(42) ? "Yes" : "No") << '\n';
  cout << "  table[42] = " << table[42] << '\n';
}

//===---------------------------- PERFORMANCE DEMO -----------------------------===//

// Performance comparison with unordered_map.
void demo_performance() {
  ads::demo::print_section("Demo: Performance Comparison");

  const int N = 100000;

  // Test our hash table.
  auto start = std::chrono::high_resolution_clock::now();

  HashTableChaining<int, int> our_table;
  for (int i = 0; i < N; ++i) {
    our_table.insert(i, i * 2);
  }

  auto end      = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "Our HashTableChaining:\n";
  cout << "  Insertions: " << duration.count() << " ms\n";
  cout << "  Final size: " << our_table.size() << '\n';
  cout << "  Capacity:   " << our_table.capacity() << '\n';
  cout << "  Load factor: " << our_table.load_factor() << '\n';

  // Test unordered_map.
  start = std::chrono::high_resolution_clock::now();

  unordered_map<int, int> std_table;
  for (int i = 0; i < N; ++i) {
    std_table[i] = i * 2;
  }

  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "\nunordered_map:\n";
  cout << "  Insertions: " << duration.count() << " ms\n";
  cout << "  Final size: " << std_table.size() << '\n';
  cout << "  Load factor: " << std_table.load_factor() << '\n';
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  ads::demo::print_header("HASH TABLE CHAINING - COMPREHENSIVE DEMO");

  try {
    demo_basic_operations();
    demo_updates();
    demo_bracket_operator();
    demo_find();
    demo_erase();
    demo_rehashing();
    demo_reserve();
    demo_custom_types();
    demo_collisions();
    demo_exceptions();
    demo_move_semantics();
    demo_clear();
    demo_performance();

    ads::demo::print_footer();

    return 0;

  } catch (const exception& e) {
    cerr << "\n[FATAL ERROR] Unhandled exception: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
