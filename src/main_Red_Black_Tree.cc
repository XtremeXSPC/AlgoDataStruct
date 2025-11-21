//===--------------------------------------------------------------------------===//
/**
 * @file main_Red_Black_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive test suite for Red-Black Tree implementation
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include "ads/support/ConsoleColors.hpp"
#include "ads/trees/Red_Black_Tree.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using namespace ads::trees;
using namespace std;

// Test tracking
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) cout << CYAN << "[TEST] " << RESET << (name) << "..." << endl;

#define ASSERT(condition, message)                                                                                                         \
  do {                                                                                                                                     \
    if (!(condition)) {                                                                                                                    \
      cout << RED << "  ✗ FAILED: " << RESET << (message) << endl;                                                                         \
      tests_failed++;                                                                                                                      \
      return;                                                                                                                              \
    }                                                                                                                                      \
  } while (0)

#define PASS()                                                                                                                             \
  do {                                                                                                                                     \
    cout << GREEN << "  ✓ PASSED" << RESET << endl;                                                                                        \
    tests_passed++;                                                                                                                        \
  } while (0)

//===--------------------------------------------------------------------------===//
// Test Functions
//===--------------------------------------------------------------------------===//

/**
 * @brief Test basic insert and search
 */
void test_basic_insert_search() {
  TEST("Basic Insert and Search");

  Red_Black_Tree<int> rbt;

  ASSERT(rbt.is_empty(), "New tree should be empty");
  ASSERT(rbt.size() == 0, "New tree should have size 0");

  // Insert elements
  ASSERT(rbt.insert(10), "Should insert 10");
  ASSERT(rbt.size() == 1, "Size should be 1");
  ASSERT(rbt.search(10), "Should find 10");

  ASSERT(rbt.insert(20), "Should insert 20");
  ASSERT(rbt.insert(5), "Should insert 5");
  ASSERT(rbt.size() == 3, "Size should be 3");

  // Search existing
  ASSERT(rbt.contains(5), "Should contain 5");
  ASSERT(rbt.contains(10), "Should contain 10");
  ASSERT(rbt.contains(20), "Should contain 20");

  // Search non-existing
  ASSERT(!rbt.search(15), "Should not find 15");
  ASSERT(!rbt.search(100), "Should not find 100");

  // Duplicate insert
  ASSERT(!rbt.insert(10), "Should not insert duplicate 10");
  ASSERT(rbt.size() == 3, "Size should remain 3");

  PASS();
}

/**
 * @brief Test Red-Black properties are maintained
 */
void test_rb_properties() {
  TEST("Red-Black Tree Properties");

  Red_Black_Tree<int> rbt;

  // Empty tree is valid
  ASSERT(rbt.validate_properties(), "Empty tree should be valid");

  // Single element
  rbt.insert(10);
  ASSERT(rbt.validate_properties(), "Single element tree should be valid");

  // Multiple insertions
  vector<int> values = {20, 5, 15, 25, 1, 7};
  for (int val : values) {
    rbt.insert(val);
    ASSERT(rbt.validate_properties(), "Tree should maintain RB properties after insert " + to_string(val));
  }

  PASS();
}

/**
 * @brief Test sequential insertions (worst case for BST)
 */
void test_sequential_insert() {
  TEST("Sequential Insert (Ascending)");

  Red_Black_Tree<int> rbt;

  // Insert 1, 2, 3, ..., 20
  for (int i = 1; i <= 20; ++i) {
    rbt.insert(i);
  }

  ASSERT(rbt.size() == 20, "Should have 20 elements");
  ASSERT(rbt.validate_properties(), "Tree should maintain RB properties");

  // Verify all elements present
  for (int i = 1; i <= 20; ++i) {
    ASSERT(rbt.search(i), "Should find " + to_string(i));
  }

  // Height should be O(log n) - for n=20, height should be ≤ 2*log2(21) ≈ 9
  int h = rbt.height();
  cout << "  Height for 20 sequential inserts: " << h << '\n';
  ASSERT(h <= 9, "Height should be ≤ 9 for 20 elements");

  PASS();
}

/**
 * @brief Test descending insertions
 */
void test_descending_insert() {
  TEST("Sequential Insert (Descending)");

  Red_Black_Tree<int> rbt;

  // Insert 20, 19, 18, ..., 1
  for (int i = 20; i >= 1; --i) {
    rbt.insert(i);
  }

  ASSERT(rbt.size() == 20, "Should have 20 elements");
  ASSERT(rbt.validate_properties(), "Tree should maintain RB properties");

  // Height check
  int h = rbt.height();
  cout << "  Height for 20 descending inserts: " << h << '\n';
  ASSERT(h <= 9, "Height should be ≤ 9 for 20 elements");

  PASS();
}

/**
 * @brief Test random insertions
 */
void test_random_insert() {
  TEST("Random Insertions");

  Red_Black_Tree<int> rbt;

  // Generate random values
  vector<int> values;
  for (int i = 1; i <= 100; ++i) {
    values.push_back(i);
  }

  random_device rd;
  mt19937       g(rd());
  shuffle(values.begin(), values.end(), g);

  // Insert in random order
  for (int val : values) {
    rbt.insert(val);
    ASSERT(rbt.validate_properties(), "Tree should maintain RB properties");
  }

  ASSERT(rbt.size() == 100, "Should have 100 elements");

  // Verify all elements
  for (int i = 1; i <= 100; ++i) {
    ASSERT(rbt.search(i), "Should find " + to_string(i));
  }

  // Height check: for n=100, height ≤ 2*log2(101) ≈ 13.3
  int h = rbt.height();
  cout << "  Height for 100 random inserts: " << h << '\n';
  ASSERT(h <= 14, "Height should be ≤ 14 for 100 elements");

  PASS();
}

/**
 * @brief Test black height property
 */
void test_black_height() {
  TEST("Black Height Property");

  Red_Black_Tree<int> rbt;

  // Insert elements
  vector<int> values = {10, 5, 15, 2, 7, 12, 20, 1, 3, 6, 8};
  for (int val : values) {
    rbt.insert(val);
  }

  int bh = rbt.black_height();
  cout << "  Black height: " << bh << '\n';
  ASSERT(bh > 0, "Black height should be positive");

  // Black height should be at least log2(n+1) / 2
  // For n=11, min black height is around 2-3
  ASSERT(bh >= 2, "Black height should be at least 2 for 11 elements");

  PASS();
}

/**
 * @brief Test in-order traversal
 */
void test_traversal() {
  TEST("In-Order Traversal");

  Red_Black_Tree<int> rbt;

  // Insert in random order
  vector<int> values = {15, 5, 20, 3, 10, 18, 25};
  for (int val : values) {
    rbt.insert(val);
  }

  // Collect elements via traversal
  vector<int> result;
  rbt.in_order_traversal([&result](const int& val) -> void { result.push_back(val); });

  // Should be sorted
  vector<int> expected = {3, 5, 10, 15, 18, 20, 25};
  ASSERT(result == expected, "In-order traversal should produce sorted output");

  PASS();
}

/**
 * @brief Test clear operation
 */
void test_clear() {
  TEST("Clear Tree");

  Red_Black_Tree<int> rbt;

  // Insert elements
  for (int i = 1; i <= 10; ++i) {
    rbt.insert(i);
  }

  ASSERT(rbt.size() == 10, "Should have 10 elements");

  // Clear
  rbt.clear();
  ASSERT(rbt.is_empty(), "Tree should be empty after clear");
  ASSERT(rbt.size() == 0, "Size should be 0 after clear");
  ASSERT(!rbt.search(5), "Should not find elements after clear");

  // Insert after clear
  rbt.insert(42);
  ASSERT(rbt.size() == 1, "Should be able to insert after clear");
  ASSERT(rbt.validate_properties(), "Tree should be valid after clear and insert");

  PASS();
}

/**
 * @brief Test move semantics
 */
void test_move_semantics() {
  TEST("Move Semantics");

  Red_Black_Tree<int> rbt1;
  for (int i = 1; i <= 10; ++i) {
    rbt1.insert(i);
  }

  size_t original_size = rbt1.size();

  // Move constructor
  Red_Black_Tree<int> rbt2(std::move(rbt1));
  ASSERT(rbt2.size() == original_size, "Moved tree should have same size");
  ASSERT(rbt2.search(5), "Moved tree should contain elements");
  ASSERT(rbt2.validate_properties(), "Moved tree should be valid");

  // Move assignment
  Red_Black_Tree<int> rbt3;
  rbt3.insert(100);
  rbt3 = std::move(rbt2);
  ASSERT(rbt3.size() == original_size, "Move assigned tree should have same size");
  ASSERT(rbt3.search(7), "Move assigned tree should contain elements");

  PASS();
}

/**
 * @brief Stress test with large dataset
 */
void test_stress() {
  TEST("Stress Test (Large Dataset)");

  Red_Black_Tree<int> rbt;
  const int           N = 10000;

  // Insert many elements
  auto start = chrono::high_resolution_clock::now();
  for (int i = 1; i <= N; ++i) {
    rbt.insert(i);
  }
  auto end             = chrono::high_resolution_clock::now();
  auto insert_duration = chrono::duration_cast<chrono::milliseconds>(end - start);

  cout << "  Insert " << N << " elements: " << insert_duration.count() << " ms" << '\n';

  ASSERT(rbt.size() == N, "Should have " + to_string(N) + " elements");
  ASSERT(rbt.validate_properties(), "Large tree should maintain RB properties");

  // Height check: for n=10000, height ≤ 2*log2(10001) ≈ 26.6
  int h = rbt.height();
  cout << "  Height: " << h << '\n';
  ASSERT(h <= 27, "Height should be ≤ 27 for 10000 elements");

  // Search test
  start = chrono::high_resolution_clock::now();
  for (int i = 1; i <= N; ++i) {
    ASSERT(rbt.search(i), "Should find all inserted elements");
  }
  end                  = chrono::high_resolution_clock::now();
  auto search_duration = chrono::duration_cast<chrono::milliseconds>(end - start);

  cout << "  Search " << N << " elements: " << search_duration.count() << " ms" << '\n';

  PASS();
}

/**
 * @brief Test with string type
 */
void test_string_type() {
  TEST("String Type Support");

  Red_Black_Tree<string> rbt;

  vector<string> words = {"apple", "banana", "cherry", "date", "elderberry"};
  for (const auto& word : words) {
    rbt.insert(word);
  }

  ASSERT(rbt.size() == 5, "Should have 5 words");
  ASSERT(rbt.validate_properties(), "String tree should be valid");
  ASSERT(rbt.search("cherry"), "Should find 'cherry'");
  ASSERT(!rbt.search("fig"), "Should not find 'fig'");

  // Check sorted order
  vector<string> result;
  rbt.in_order_traversal([&result](const string& s) -> void { result.push_back(s); });

  vector<string> expected = {"apple", "banana", "cherry", "date", "elderberry"};
  ASSERT(result == expected, "String traversal should be sorted");

  PASS();
}

/**
 * @brief Performance comparison with unbalanced BST (conceptual)
 */
void test_balance_advantage() {
  TEST("Balance Advantage Demonstration");

  Red_Black_Tree<int> rbt;

  // Sequential insert (worst case for unbalanced BST)
  const int N = 1000;
  for (int i = 1; i <= N; ++i) {
    rbt.insert(i);
  }

  int h               = rbt.height();
  int theoretical_max = static_cast<int>(2 * std::log2(N + 1));

  cout << "  Actual height: " << h << '\n';
  cout << "  Theoretical max (2*log2(n+1)): " << theoretical_max << '\n';
  cout << "  Unbalanced BST height would be: " << (N - 1) << '\n';

  ASSERT(h <= theoretical_max, "Height should be ≤ theoretical maximum");
  ASSERT(h < (N - 1), "Height should be much less than unbalanced BST");

  PASS();
}

//===--------------------------------------------------------------------------===//
// Main Test Runner
//===--------------------------------------------------------------------------===//

auto main() -> int {
  cout << BOLD << BLUE << "\n=================================\n" << RESET;
  cout << BOLD << "  Red-Black Tree Test Suite\n" << RESET;
  cout << BOLD << BLUE << "=================================\n" << RESET << '\n';

  // Run all tests
  test_basic_insert_search();
  test_rb_properties();
  test_sequential_insert();
  test_descending_insert();
  test_random_insert();
  test_black_height();
  test_traversal();
  test_clear();
  test_move_semantics();
  test_stress();
  test_string_type();
  test_balance_advantage();

  // Print summary
  cout << BOLD << BLUE << "\n=================================\n" << RESET;
  cout << BOLD << "  Test Summary\n" << RESET;
  cout << BOLD << BLUE << "=================================\n" << RESET;
  cout << GREEN << "  Passed: " << tests_passed << RESET << '\n';
  cout << RED << "  Failed: " << tests_failed << RESET << '\n';
  cout << BOLD << BLUE << "=================================\n" << RESET << '\n';

  if (tests_failed == 0) {
    cout << GREEN << BOLD << "  ✓ All tests passed!\n" << RESET << '\n';
  }

  return tests_failed > 0 ? 1 : 0;
}

//===--------------------------------------------------------------------------===//
