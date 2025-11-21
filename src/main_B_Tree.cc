//===--------------------------------------------------------------------------===//
/**
 * @file main_B_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive test suite for B-Tree implementation
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include "ads/trees/B_Tree.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using namespace ads::trees;
using namespace std;

// ANSI color codes
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define BOLD "\033[1m"

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

  B_Tree<int, 3> btree;

  ASSERT(btree.is_empty(), "New tree should be empty");
  ASSERT(btree.size() == 0, "New tree should have size 0");
  ASSERT(btree.get_min_degree() == 3, "Min degree should be 3");
  ASSERT(btree.get_max_keys() == 5, "Max keys should be 2*3-1 = 5");

  // Insert elements
  ASSERT(btree.insert(10), "Should insert 10");
  ASSERT(btree.size() == 1, "Size should be 1");
  ASSERT(btree.search(10), "Should find 10");

  ASSERT(btree.insert(20), "Should insert 20");
  ASSERT(btree.insert(5), "Should insert 5");
  ASSERT(btree.size() == 3, "Size should be 3");

  // Search existing
  ASSERT(btree.contains(5), "Should contain 5");
  ASSERT(btree.contains(10), "Should contain 10");
  ASSERT(btree.contains(20), "Should contain 20");

  // Search non-existing
  ASSERT(!btree.search(15), "Should not find 15");
  ASSERT(!btree.search(100), "Should not find 100");

  // Duplicate insert
  ASSERT(!btree.insert(10), "Should not insert duplicate 10");
  ASSERT(btree.size() == 3, "Size should remain 3");

  PASS();
}

/**
 * @brief Test sequential insertions and splits
 */
void test_sequential_insert() {
  TEST("Sequential Insert with Splits");

  B_Tree<int, 2> btree; // t=2: 2-3-4 tree (1-3 keys, 2-4 children)

  cout << "  Inserting 1-10 sequentially..." << '\n';

  // Insert 1-10 and watch splits happen
  for (int i = 1; i <= 10; ++i) {
    ASSERT(btree.insert(i), "Should insert " + to_string(i));
    ASSERT(btree.validate_properties(), "Tree should maintain B-Tree properties after insert " + to_string(i));
  }

  ASSERT(btree.size() == 10, "Should have 10 elements");

  // Verify all elements present
  for (int i = 1; i <= 10; ++i) {
    ASSERT(btree.search(i), "Should find " + to_string(i));
  }

  int h = btree.height();
  cout << "  Height: " << h << " nodes: " << btree.count_nodes() << '\n';

  PASS();
}

/**
 * @brief Test in-order traversal
 */
void test_traversal() {
  TEST("In-Order Traversal");

  B_Tree<int, 3> btree;

  // Insert in random order
  vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
  for (int val : values) {
    btree.insert(val);
  }

  // Collect via traversal
  vector<int> result;
  btree.in_order_traversal([&result](const int& val) -> void { result.push_back(val); });

  // Should be sorted
  vector<int> expected = {10, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80};
  ASSERT(result == expected, "In-order traversal should produce sorted output");

  PASS();
}

/**
 * @brief Test B-Tree properties validation
 */
void test_properties() {
  TEST("B-Tree Properties Validation");

  B_Tree<int, 3> btree;

  // Empty tree is valid
  ASSERT(btree.validate_properties(), "Empty tree should be valid");

  // Insert many elements
  for (int i = 1; i <= 100; ++i) {
    btree.insert(i);
  }

  ASSERT(btree.validate_properties(), "Large tree should maintain properties");
  ASSERT(btree.size() == 100, "Should have 100 elements");

  // All leaves should be at same level
  // Height should be low: log_t(n)
  int h = btree.height();
  cout << "  Height for 100 elements (t=3): " << h << '\n';
  // For t=3, log_3(100) ≈ 4.19
  ASSERT(h <= 5, "Height should be ≤ 5 for 100 elements with t=3");

  PASS();
}

/**
 * @brief Test with different minimum degrees
 */
void test_different_degrees() {
  TEST("Different Minimum Degrees");

  // t=2: 2-3-4 tree
  {
    B_Tree<int, 2> btree2;
    for (int i = 1; i <= 50; ++i) {
      btree2.insert(i);
    }
    ASSERT(btree2.size() == 50, "t=2 tree should have 50 elements");
    ASSERT(btree2.validate_properties(), "t=2 tree should be valid");
    int h2 = btree2.height();
    cout << "  t=2: height=" << h2 << " nodes=" << btree2.count_nodes() << '\n';
  }

  // t=5: larger nodes
  {
    B_Tree<int, 5> btree5;
    for (int i = 1; i <= 50; ++i) {
      btree5.insert(i);
    }
    ASSERT(btree5.size() == 50, "t=5 tree should have 50 elements");
    ASSERT(btree5.validate_properties(), "t=5 tree should be valid");
    int h5 = btree5.height();
    cout << "  t=5: height=" << h5 << " nodes=" << btree5.count_nodes() << '\n';
    // t=5 should have lower height than t=2
  }

  PASS();
}

/**
 * @brief Test random insertions
 */
void test_random_insert() {
  TEST("Random Insertions");

  B_Tree<int, 3> btree;

  // Generate and shuffle values
  vector<int> values;
  for (int i = 1; i <= 100; ++i) {
    values.push_back(i);
  }

  random_device rd;
  mt19937       g(rd());
  shuffle(values.begin(), values.end(), g);

  // Insert in random order
  for (int val : values) {
    btree.insert(val);
    ASSERT(btree.validate_properties(), "Tree should maintain properties");
  }

  ASSERT(btree.size() == 100, "Should have 100 elements");

  // Verify all elements
  for (int i = 1; i <= 100; ++i) {
    ASSERT(btree.search(i), "Should find " + to_string(i));
  }

  PASS();
}

/**
 * @brief Test clear operation
 */
void test_clear() {
  TEST("Clear Tree");

  B_Tree<int, 3> btree;

  // Insert elements
  for (int i = 1; i <= 20; ++i) {
    btree.insert(i);
  }

  ASSERT(btree.size() == 20, "Should have 20 elements");

  // Clear
  btree.clear();
  ASSERT(btree.is_empty(), "Tree should be empty after clear");
  ASSERT(btree.size() == 0, "Size should be 0 after clear");
  ASSERT(!btree.search(10), "Should not find elements after clear");

  // Insert after clear
  btree.insert(42);
  ASSERT(btree.size() == 1, "Should be able to insert after clear");
  ASSERT(btree.validate_properties(), "Tree should be valid after clear and insert");

  PASS();
}

/**
 * @brief Test move semantics
 */
void test_move_semantics() {
  TEST("Move Semantics");

  B_Tree<int, 3> btree1;
  for (int i = 1; i <= 20; ++i) {
    btree1.insert(i);
  }

  size_t original_size = btree1.size();

  // Move constructor
  B_Tree<int, 3> btree2(std::move(btree1));
  ASSERT(btree2.size() == original_size, "Moved tree should have same size");
  ASSERT(btree2.search(10), "Moved tree should contain elements");
  ASSERT(btree2.validate_properties(), "Moved tree should be valid");

  // Move assignment
  B_Tree<int, 3> btree3;
  btree3.insert(100);
  btree3 = std::move(btree2);
  ASSERT(btree3.size() == original_size, "Move assigned tree should have same size");
  ASSERT(btree3.search(15), "Move assigned tree should contain elements");

  PASS();
}

/**
 * @brief Test stress with large dataset
 */
void test_stress() {
  TEST("Stress Test (Large Dataset)");

  B_Tree<int, 5> btree;
  const int      N = 10000;

  // Insert
  auto start = chrono::high_resolution_clock::now();
  for (int i = 1; i <= N; ++i) {
    btree.insert(i);
  }
  auto end             = chrono::high_resolution_clock::now();
  auto insert_duration = chrono::duration_cast<chrono::milliseconds>(end - start);

  cout << "  Insert " << N << " elements: " << insert_duration.count() << " ms" << '\n';

  ASSERT(btree.size() == N, "Should have " + to_string(N) + " elements");
  ASSERT(btree.validate_properties(), "Large tree should maintain properties");

  int    h     = btree.height();
  size_t nodes = btree.count_nodes();
  cout << "  Height: " << h << " Nodes: " << nodes << '\n';

  // Height should be log_5(10000) ≈ 5.71
  ASSERT(h <= 7, "Height should be ≤ 7 for 10000 elements with t=5");

  // Search
  start = chrono::high_resolution_clock::now();
  for (int i = 1; i <= N; ++i) {
    ASSERT(btree.search(i), "Should find all elements");
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

  B_Tree<string, 3> btree;

  vector<string> words = {"apple", "banana", "cherry", "date", "elderberry", "fig", "grape", "honeydew", "kiwi", "lemon"};

  for (const auto& word : words) {
    btree.insert(word);
  }

  ASSERT(btree.size() == 10, "Should have 10 words");
  ASSERT(btree.validate_properties(), "String tree should be valid");
  ASSERT(btree.search("cherry"), "Should find 'cherry'");
  ASSERT(!btree.search("orange"), "Should not find 'orange'");

  // Check sorted order
  vector<string> result;
  btree.in_order_traversal([&result](const string& s) -> void { result.push_back(s); });

  vector<string> expected = words;
  sort(expected.begin(), expected.end());
  ASSERT(result == expected, "String traversal should be sorted");

  PASS();
}

/**
 * @brief Test height comparison with different t values
 */
void test_height_comparison() {
  TEST("Height Comparison (Different t)");

  const int N = 1000;

  B_Tree<int, 2>  btree2;
  B_Tree<int, 5>  btree5;
  B_Tree<int, 10> btree10;

  for (int i = 1; i <= N; ++i) {
    btree2.insert(i);
    btree5.insert(i);
    btree10.insert(i);
  }

  int h2  = btree2.height();
  int h5  = btree5.height();
  int h10 = btree10.height();

  cout << "  For " << N << " elements:" << '\n';
  cout << "    t=2:  height=" << h2 << " nodes=" << btree2.count_nodes() << '\n';
  cout << "    t=5:  height=" << h5 << " nodes=" << btree5.count_nodes() << '\n';
  cout << "    t=10: height=" << h10 << " nodes=" << btree10.count_nodes() << '\n';

  // Larger t should have smaller height
  ASSERT(h10 < h5, "t=10 should have lower height than t=5");
  ASSERT(h5 < h2, "t=5 should have lower height than t=2");

  PASS();
}

//===--------------------------------------------------------------------------===//
// Main Test Runner
//===--------------------------------------------------------------------------===//

auto main() -> int {
  cout << BOLD << BLUE << "\n=================================\n" << RESET;
  cout << BOLD << "  B-Tree Test Suite\n" << RESET;
  cout << BOLD << BLUE << "=================================\n" << RESET << '\n';

  // Run all tests
  test_basic_insert_search();
  test_sequential_insert();
  test_traversal();
  test_properties();
  test_different_degrees();
  test_random_insert();
  test_clear();
  test_move_semantics();
  test_stress();
  test_string_type();
  test_height_comparison();

  // Print summary
  cout << BOLD << BLUE << "\n=================================\n" << RESET;
  cout << BOLD << "  Test Summary\n" << RESET;
  cout << BOLD << BLUE << "=================================\n" << RESET;
  cout << GREEN << "  Passed: " << tests_passed << RESET << '\n';
  cout << RED << "  Failed: " << tests_failed << RESET << '\n';
  cout << BOLD << BLUE << "=================================\n" << RESET << '\n';

  if (tests_failed == 0) {
    cout << GREEN << BOLD << "\n  ✓ All tests passed!\n" << RESET << '\n';
  }

  return tests_failed > 0 ? 1 : 0;
}

//===--------------------------------------------------------------------------===//
