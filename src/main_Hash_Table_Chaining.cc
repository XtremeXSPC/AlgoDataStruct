//===--------------------------------------------------------------------------===//
/**
 * @file main_Hash_Table_Chaining.cc
 * @author Costantino Lombardi
 * @brief Comprehensive test program for Hash Table Chaining implementation
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "../include/ads/hash/Hash_Table_Chaining.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::vector;

using ads::hash::HashTableChaining;

// Helper function to print hash table statistics
template <typename Key, typename Value>
void print_stats(const HashTableChaining<Key, Value>& table, const string& name) {
  cout << "Hash Table '" << name << "':\n";
  cout << "  Size:        " << table.size() << '\n';
  cout << "  Capacity:    " << table.capacity() << '\n';
  cout << "  Load Factor: " << table.load_factor() << '\n';
  cout << "  Max LF:      " << table.max_load_factor() << '\n';
  cout << "  Empty:       " << (table.is_empty() ? "Yes" : "No") << '\n';
}

// Test basic insertion and access
void test_basic_operations() {
  cout << "\n========== Test: Basic Operations ==========\n";

  HashTableChaining<int, string> table;

  cout << "Inserting key-value pairs:\n";
  table.insert(1, "one");
  table.insert(2, "two");
  table.insert(3, "three");
  table.insert(4, "four");
  table.insert(5, "five");

  print_stats(table, "after insertions");

  // Test access
  cout << "\nAccessing values:\n";
  cout << "  table[1] = " << table.at(1) << '\n';
  cout << "  table[3] = " << table.at(3) << '\n';
  cout << "  table[5] = " << table.at(5) << '\n';

  // Test contains
  cout << "\nTesting contains():\n";
  cout << "  Contains 3? " << (table.contains(3) ? "Yes" : "No") << '\n';
  cout << "  Contains 10? " << (table.contains(10) ? "Yes" : "No") << '\n';
}

// Test update existing keys
void test_updates() {
  cout << "\n========== Test: Updates ==========\n";

  HashTableChaining<string, int> table;

  table.insert("apple", 5);
  table.insert("banana", 3);
  table.insert("orange", 7);

  cout << "Initial values:\n";
  cout << "  apple: " << table["apple"] << '\n';
  cout << "  banana: " << table["banana"] << '\n';

  // Update existing keys
  cout << "\nUpdating values:\n";
  table.insert("apple", 10);
  table["banana"] = 15;

  cout << "After updates:\n";
  cout << "  apple: " << table["apple"] << '\n';
  cout << "  banana: " << table["banana"] << '\n';

  print_stats(table, "after updates");
}

// Test operator[]
void test_bracket_operator() {
  cout << "\n========== Test: Operator[] ==========\n";

  HashTableChaining<string, int> table;

  cout << "Using operator[] to insert and access:\n";
  table["first"] = 1;
  table["second"] = 2;
  table["third"] = 3;

  cout << "  first: " << table["first"] << '\n';
  cout << "  second: " << table["second"] << '\n';
  cout << "  third: " << table["third"] << '\n';

  // Access non-existent key (should create with default value)
  cout << "\nAccessing non-existent key 'fourth':\n";
  int value = table["fourth"];
  cout << "  Value: " << value << " (default-constructed)\n";
  cout << "  Table size is now: " << table.size() << '\n';
}

// Test find()
void test_find() {
  cout << "\n========== Test: Find ==========\n";

  HashTableChaining<int, string> table;
  table.insert(10, "ten");
  table.insert(20, "twenty");
  table.insert(30, "thirty");

  cout << "Testing find():\n";

  auto* result = table.find(20);
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

// Test erase
void test_erase() {
  cout << "\n========== Test: Erase ==========\n";

  HashTableChaining<int, string> table;

  for (int i = 1; i <= 10; ++i) {
    table.insert(i, "value_" + std::to_string(i));
  }

  print_stats(table, "before erasure");

  cout << "\nErasing keys 3, 5, 7:\n";
  cout << "  Erase 3: " << (table.erase(3) ? "success" : "failed") << '\n';
  cout << "  Erase 5: " << (table.erase(5) ? "success" : "failed") << '\n';
  cout << "  Erase 7: " << (table.erase(7) ? "success" : "failed") << '\n';

  cout << "\nErasing non-existent key 100:\n";
  cout << "  Erase 100: " << (table.erase(100) ? "success" : "failed (correct)") << '\n';

  print_stats(table, "after erasure");

  // Verify remaining keys
  cout << "\nVerifying remaining keys:\n";
  for (int i = 1; i <= 10; ++i) {
    cout << "  Contains " << i << "? " << (table.contains(i) ? "Yes" : "No") << '\n';
  }
}

// Test rehashing
void test_rehashing() {
  cout << "\n========== Test: Rehashing ==========\n";

  HashTableChaining<int, int> table(4, 0.75f); // Small initial capacity

  cout << "Initial state:\n";
  print_stats(table, "empty table");

  cout << "\nInserting 20 elements to trigger rehashing:\n";
  for (int i = 1; i <= 20; ++i) {
    table.insert(i, i * 10);
    if (i % 5 == 0) {
      cout << "After " << i << " insertions: capacity=" << table.capacity()
           << ", load_factor=" << table.load_factor() << '\n';
    }
  }

  print_stats(table, "after insertions with auto-rehash");

  // Verify all values are still accessible after rehashing
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

// Test reserve()
void test_reserve() {
  cout << "\n========== Test: Reserve ==========\n";

  HashTableChaining<int, string> table;

  cout << "Initial capacity: " << table.capacity() << '\n';

  cout << "\nReserving capacity for 100 elements:\n";
  table.reserve(100);
  cout << "New capacity: " << table.capacity() << '\n';

  // Insert many elements without triggering rehash
  cout << "\nInserting 50 elements:\n";
  for (int i = 0; i < 50; ++i) {
    table.insert(i, "value");
  }

  cout << "Capacity after insertions: " << table.capacity() << " (should still be 100)\n";
  print_stats(table, "after reserve and insertions");
}

// Test with custom types
struct Person {
  string name;
  int    age;

  Person() : name(""), age(0) {}
  Person(const string& n, int a) : name(n), age(a) {}

  bool operator==(const Person& other) const {
    return name == other.name && age == other.age;
  }
};

void test_custom_types() {
  cout << "\n========== Test: Custom Types ==========\n";

  HashTableChaining<string, Person> table;

  table.insert("alice", Person("Alice", 30));
  table.insert("bob", Person("Bob", 25));
  table.emplace("charlie", "Charlie", 35);

  cout << "Accessing custom types:\n";
  cout << "  alice: " << table["alice"].name << ", age " << table["alice"].age << '\n';
  cout << "  bob: " << table["bob"].name << ", age " << table["bob"].age << '\n';
  cout << "  charlie: " << table["charlie"].name << ", age " << table["charlie"].age << '\n';

  print_stats(table, "custom types table");
}

// Test collisions (intentional)
void test_collisions() {
  cout << "\n========== Test: Collision Handling ==========\n";

  // Use small capacity to force collisions
  HashTableChaining<int, string> table(4);

  cout << "Inserting values that will collide (capacity=4):\n";
  // Keys 0, 4, 8, 12 will all hash to the same bucket (0)
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

// Test exception handling
void test_exceptions() {
  cout << "\n========== Test: Exception Handling ==========\n";

  HashTableChaining<int, string> table;
  table.insert(1, "one");
  table.insert(2, "two");

  // Test at() with non-existent key
  cout << "Testing at() with non-existent key:\n";
  try {
    string value = table.at(100);
    cout << "  ERROR: No exception thrown!\n";
  } catch (const exception& e) {
    cout << "  Caught exception: " << e.what() << '\n';
  }

  // Test invalid max_load_factor
  cout << "\nTesting set_max_load_factor with invalid value:\n";
  try {
    table.set_max_load_factor(-0.5f);
    cout << "  ERROR: No exception thrown!\n";
  } catch (const exception& e) {
    cout << "  Caught exception: " << e.what() << '\n';
  }
}

// Test move semantics
void test_move_semantics() {
  cout << "\n========== Test: Move Semantics ==========\n";

  HashTableChaining<int, string> table1;
  table1.insert(1, "one");
  table1.insert(2, "two");
  table1.insert(3, "three");

  cout << "Original table:\n";
  print_stats(table1, "table1");

  // Move constructor
  HashTableChaining<int, string> table2(std::move(table1));
  cout << "\nAfter move construction:\n";
  print_stats(table2, "table2");
  cout << "table1 size: " << table1.size() << " (should be 0)\n";

  // Move assignment
  HashTableChaining<int, string> table3;
  table3 = std::move(table2);
  cout << "\nAfter move assignment:\n";
  print_stats(table3, "table3");
  cout << "table2 size: " << table2.size() << " (should be 0)\n";
}

// Test clear()
void test_clear() {
  cout << "\n========== Test: Clear ==========\n";

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

// Performance comparison with std::unordered_map
void test_performance() {
  cout << "\n========== Test: Performance Comparison ==========\n";

  const int N = 100000;

  // Test our hash table
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

  // Test std::unordered_map
  start = std::chrono::high_resolution_clock::now();

  std::unordered_map<int, int> std_table;
  for (int i = 0; i < N; ++i) {
    std_table[i] = i * 2;
  }

  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "\nstd::unordered_map:\n";
  cout << "  Insertions: " << duration.count() << " ms\n";
  cout << "  Final size: " << std_table.size() << '\n';
  cout << "  Load factor: " << std_table.load_factor() << '\n';
}

int main() {
  cout << "╔════════════════════════════════════════════════════════╗\n";
  cout << "║   HASH TABLE CHAINING COMPREHENSIVE TEST SUITE        ║\n";
  cout << "╚════════════════════════════════════════════════════════╝\n";

  try {
    test_basic_operations();
    test_updates();
    test_bracket_operator();
    test_find();
    test_erase();
    test_rehashing();
    test_reserve();
    test_custom_types();
    test_collisions();
    test_exceptions();
    test_move_semantics();
    test_clear();
    test_performance();

    cout << "\n╔════════════════════════════════════════════════════════╗\n";
    cout << "║  ALL TESTS COMPLETED SUCCESSFULLY!                    ║\n";
    cout << "╚════════════════════════════════════════════════════════╝\n";

    return 0;

  } catch (const exception& e) {
    cerr << "\n[FATAL ERROR] Unhandled exception: " << e.what() << '\n';
    return 1;
  }
}

//===--------------------------------------------------------------------------===//
