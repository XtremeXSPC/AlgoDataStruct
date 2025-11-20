//===--------------------------------------------------------------------------===//
/**
 * @file main_Hash_Table_Open_Addressing.cc
 * @author Costantino Lombardi
 * @brief Comprehensive test program for Hash Table Open Addressing implementation
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
#include <vector>

#include "../include/ads/hash/Hash_Table_Chaining.hpp"
#include "../include/ads/hash/Hash_Table_Open_Addressing.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::vector;

using ads::hash::HashTableChaining;
using ads::hash::HashTableOpenAddressing;
using ads::hash::ProbingStrategy;

// Helper function to convert probing strategy to string
string strategy_to_string(ProbingStrategy strategy) {
  switch (strategy) {
  case ProbingStrategy::LINEAR:
    return "Linear Probing";
  case ProbingStrategy::QUADRATIC:
    return "Quadratic Probing";
  case ProbingStrategy::DOUBLE_HASH:
    return "Double Hashing";
  default:
    return "Unknown";
  }
}

// Helper function to print hash table statistics
template <typename Key, typename Value>
void print_stats(const HashTableOpenAddressing<Key, Value>& table, const string& name) {
  cout << "Hash Table '" << name << "':\n";
  cout << "  Size:        " << table.size() << '\n';
  cout << "  Capacity:    " << table.capacity() << '\n';
  cout << "  Load Factor: " << table.load_factor() << '\n';
  cout << "  Max LF:      " << table.max_load_factor() << '\n';
  cout << "  Strategy:    " << strategy_to_string(table.probing_strategy()) << '\n';
  cout << "  Empty:       " << (table.is_empty() ? "Yes" : "No") << '\n';
}

// Test basic operations with linear probing
void test_basic_operations() {
  cout << "\n========== Test: Basic Operations (Linear Probing) ==========\n";

  HashTableOpenAddressing<int, string> table(16, 0.5f, ProbingStrategy::LINEAR);

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

// Test all three probing strategies
void test_probing_strategies() {
  cout << "\n========== Test: Different Probing Strategies ==========\n";

  vector<ProbingStrategy> strategies = {ProbingStrategy::LINEAR, ProbingStrategy::QUADRATIC, ProbingStrategy::DOUBLE_HASH};

  for (auto strategy : strategies) {
    cout << "\nTesting " << strategy_to_string(strategy) << ":\n";

    HashTableOpenAddressing<int, int> table(16, 0.5f, strategy);

    // Insert some values
    for (int i = 0; i < 10; ++i) {
      table.insert(i, i * 10);
    }

    // Verify all values
    cout << "  Inserted 10 values\n";
    bool all_found = true;
    for (int i = 0; i < 10; ++i) {
      if (!table.contains(i) || table[i] != i * 10) {
        all_found = false;
        cout << "    ERROR: Key " << i << " not found or has wrong value!\n";
      }
    }
    if (all_found) {
      cout << "  ✓ All values correctly stored and retrieved\n";
    }

    print_stats(table, strategy_to_string(strategy));
  }
}

// Test tombstone handling (deleted slots)
void test_tombstones() {
  cout << "\n========== Test: Tombstone Handling ==========\n";

  HashTableOpenAddressing<int, string> table(8, 0.5f, ProbingStrategy::LINEAR);

  // Insert values that will cause collisions
  cout << "Inserting colliding keys (0, 8, 16):\n";
  table.insert(0, "zero");
  table.insert(8, "eight");
  table.insert(16, "sixteen");

  cout << "  All 3 keys inserted\n";

  // Delete middle element
  cout << "\nDeleting middle element (key 8):\n";
  bool erased = table.erase(8);
  cout << "  Erase result: " << (erased ? "success" : "failed") << '\n';

  // Verify we can still find the last element
  cout << "\nVerifying key 16 is still accessible:\n";
  cout << "  Contains 16? " << (table.contains(16) ? "Yes (correct)" : "No (ERROR!)") << '\n';
  if (table.contains(16)) {
    cout << "  Value: " << table[16] << '\n';
  }

  // Insert a new value at the tombstone location
  cout << "\nInserting new key 8 (should reuse tombstone slot):\n";
  table.insert(8, "new_eight");
  cout << "  table[8] = " << table[8] << '\n';

  print_stats(table, "after tombstone operations");
}

// Test updates
void test_updates() {
  cout << "\n========== Test: Updates ==========\n";

  HashTableOpenAddressing<string, int> table;

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

// Test rehashing
void test_rehashing() {
  cout << "\n========== Test: Rehashing ==========\n";

  HashTableOpenAddressing<int, int> table(4, 0.5f); // Small capacity, low threshold

  cout << "Initial state:\n";
  print_stats(table, "empty table");

  cout << "\nInserting elements to trigger rehashing:\n";
  for (int i = 1; i <= 20; ++i) {
    table.insert(i, i * 10);
    if (i % 5 == 0) {
      cout << "After " << i << " insertions: capacity=" << table.capacity() << ", load_factor=" << table.load_factor() << '\n';
    }
  }

  print_stats(table, "after auto-rehash");

  // Verify all values preserved
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

// Test with high collision rate
void test_high_collisions() {
  cout << "\n========== Test: High Collision Rate ==========\n";

  // Small capacity forces many collisions
  HashTableOpenAddressing<int, int> table(8, 0.5f, ProbingStrategy::LINEAR);

  cout << "Inserting 4 values into table with capacity 8:\n";
  table.insert(0, 100);
  table.insert(8, 200);  // Will collide with 0
  table.insert(16, 300); // Will collide with 0 and 8
  table.insert(24, 400); // Will collide with all above

  cout << "\nAll values share the same hash:\n";
  for (int key : {0, 8, 16, 24}) {
    cout << "  table[" << key << "] = " << table[key] << '\n';
  }

  print_stats(table, "with high collisions");
}

// Test reserve()
void test_reserve() {
  cout << "\n========== Test: Reserve ==========\n";

  HashTableOpenAddressing<int, string> table;

  cout << "Initial capacity: " << table.capacity() << '\n';

  cout << "\nReserving capacity for 100 elements:\n";
  table.reserve(100);
  cout << "New capacity: " << table.capacity() << '\n';

  // Insert many elements
  for (int i = 0; i < 40; ++i) {
    table.insert(i, "value");
  }

  cout << "Capacity after 40 insertions: " << table.capacity() << " (should still be 100)\n";
  print_stats(table, "after reserve");
}

// Test exception handling
void test_exceptions() {
  cout << "\n========== Test: Exception Handling ==========\n";

  HashTableOpenAddressing<int, string> table;
  table.insert(1, "one");

  // Test at() with non-existent key
  cout << "Testing at() with non-existent key:\n";
  try {
    table.at(100);
    cout << "  ERROR: No exception thrown!\n";
  } catch (const exception& e) {
    cout << "  Caught exception: " << e.what() << '\n';
  }

  // Test invalid max_load_factor
  cout << "\nTesting set_max_load_factor with invalid value (>= 1.0):\n";
  try {
    table.set_max_load_factor(1.5f);
    cout << "  ERROR: No exception thrown!\n";
  } catch (const exception& e) {
    cout << "  Caught exception: " << e.what() << '\n';
  }

  cout << "\nTesting set_max_load_factor with invalid value (<= 0):\n";
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

  HashTableOpenAddressing<int, string> table1;
  table1.insert(1, "one");
  table1.insert(2, "two");
  table1.insert(3, "three");

  cout << "Original table:\n";
  print_stats(table1, "table1");

  // Move constructor
  HashTableOpenAddressing<int, string> table2(std::move(table1));
  cout << "\nAfter move construction:\n";
  print_stats(table2, "table2");
  cout << "table1 size: " << table1.size() << " (should be 0)\n";

  // Move assignment
  HashTableOpenAddressing<int, string> table3;
  table3 = std::move(table2);
  cout << "\nAfter move assignment:\n";
  print_stats(table3, "table3");
  cout << "table2 size: " << table2.size() << " (should be 0)\n";
}

// Test clear()
void test_clear() {
  cout << "\n========== Test: Clear ==========\n";

  HashTableOpenAddressing<int, int> table;
  for (int i = 0; i < 50; ++i) {
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

// Performance comparison: Open Addressing vs Chaining
void test_performance_comparison() {
  cout << "\n========== Test: Performance Comparison ==========\n";

  const int N = 50000;

  // Test Open Addressing with different strategies
  vector<ProbingStrategy> strategies = {ProbingStrategy::LINEAR, ProbingStrategy::QUADRATIC, ProbingStrategy::DOUBLE_HASH};

  for (auto strategy : strategies) {
    auto start = std::chrono::high_resolution_clock::now();

    HashTableOpenAddressing<int, int> table(16, 0.5f, strategy);
    for (int i = 0; i < N; ++i) {
      table.insert(i, i * 2);
    }

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    cout << "\n" << strategy_to_string(strategy) << ":\n";
    cout << "  Insertions: " << duration.count() << " ms\n";
    cout << "  Final size: " << table.size() << '\n';
    cout << "  Capacity:   " << table.capacity() << '\n';
    cout << "  Load factor: " << table.load_factor() << '\n';
  }

  // Compare with Chaining
  auto start = std::chrono::high_resolution_clock::now();

  HashTableChaining<int, int> chaining_table;
  for (int i = 0; i < N; ++i) {
    chaining_table.insert(i, i * 2);
  }

  auto end      = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "\nChaining (for comparison):\n";
  cout << "  Insertions: " << duration.count() << " ms\n";
  cout << "  Final size: " << chaining_table.size() << '\n';
  cout << "  Capacity:   " << chaining_table.capacity() << '\n';
  cout << "  Load factor: " << chaining_table.load_factor() << '\n';
}

// Test edge cases
void test_edge_cases() {
  cout << "\n========== Test: Edge Cases ==========\n";

  HashTableOpenAddressing<int, int> table;

  // Test operations on empty table
  cout << "Testing operations on empty table:\n";
  cout << "  is_empty(): " << (table.is_empty() ? "true" : "false") << '\n';
  cout << "  size(): " << table.size() << '\n';
  cout << "  contains(42): " << (table.contains(42) ? "true" : "false") << '\n';
  cout << "  erase(42): " << (table.erase(42) ? "true" : "false") << '\n';

  // Test single element
  cout << "\nTesting single element:\n";
  table.insert(42, 1764);
  print_stats(table, "single element table");

  // Test operator[] with non-existent key
  cout << "\nTesting operator[] with non-existent key:\n";
  int value = table[99];
  cout << "  table[99] = " << value << " (default value)\n";
  cout << "  Size after operator[]: " << table.size() << '\n';
}

int main() {
  cout << "╔════════════════════════════════════════════════════════╗\n";
  cout << "║   HASH TABLE OPEN ADDRESSING COMPREHENSIVE TEST       ║\n";
  cout << "╚════════════════════════════════════════════════════════╝\n";

  try {
    test_basic_operations();
    test_probing_strategies();
    test_tombstones();
    test_updates();
    test_rehashing();
    test_high_collisions();
    test_reserve();
    test_exceptions();
    test_move_semantics();
    test_clear();
    test_performance_comparison();
    test_edge_cases();

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
