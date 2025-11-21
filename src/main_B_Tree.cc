//===--------------------------------------------------------------------------===//
/**
 * @file main_B_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for B-Tree implementation
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the B-Tree data structure,
 * showcasing its multi-way branching, node splitting, and various operations.
 */
//===--------------------------------------------------------------------------===//

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../include/ads/trees/B_Tree.hpp"

using std::cerr;
using std::cout;
using std::string;
using std::to_string;
using std::vector;

using ads::trees::B_Tree;

// Helper function to print tree contents
template <size_t T>
void print_btree(const B_Tree<int, T>& tree, const string& name) {
  cout << "B-Tree '" << name << "' (size: " << tree.size() << ", height: " << tree.height()
       << ", nodes: " << tree.count_nodes() << "):\n";

  if (tree.is_empty()) {
    cout << "  (empty)\n";
    return;
  }

  cout << "  In-order: ";
  tree.in_order_traversal([](const int& value) -> void { cout << value << " "; });
  cout << '\n';
}

// Demo: Basic operations
void demo_basic_operations() {
  cout << "\n========== Demo: Basic Operations ==========\n";

  B_Tree<int, 3> btree; // Minimum degree t=3 (2-5 keys per node)

  cout << "Creating empty B-Tree with minimum degree t=3...\n";
  cout << "  Size: " << btree.size() << ", Empty: " << (btree.is_empty() ? "yes" : "no") << "\n";
  cout << "  Min degree: " << btree.get_min_degree() << ", Max keys per node: " << btree.get_max_keys() << "\n";

  // Insert elements
  cout << "\nInserting values: 10, 20, 5, 6, 12, 30, 7, 17\n";
  btree.insert(10);
  btree.insert(20);
  btree.insert(5);
  btree.insert(6);
  btree.insert(12);
  btree.insert(30);
  btree.insert(7);
  btree.insert(17);

  print_btree(btree, "btree");

  // Test search operations
  cout << "\nSearch operations:\n";
  cout << "  search(10): " << (btree.search(10) ? "found" : "not found") << '\n';
  cout << "  search(15): " << (btree.search(15) ? "found" : "not found") << '\n';
  cout << "  contains(30): " << (btree.contains(30) ? "yes" : "no") << '\n';

  // Test duplicates
  cout << "\nTrying to insert duplicate (10): ";
  bool inserted = btree.insert(10);
  cout << (inserted ? "inserted" : "not inserted (correct behavior)") << '\n';
}

// Demo: Node splitting
void demo_node_splitting() {
  cout << "\n========== Demo: Node Splitting ==========\n";

  cout << "B-Trees split nodes when they become full.\n";
  cout << "With t=2, each node can hold 1-3 keys.\n\n";

  B_Tree<int, 2> btree; // t=2: 2-3-4 tree behavior (1-3 keys per node)

  cout << "Inserting 1 to 10 sequentially (observing height changes):\n";
  for (int i = 1; i <= 10; ++i) {
    int prev_height = btree.height();
    btree.insert(i);
    int new_height = btree.height();

    if (new_height > prev_height) {
      cout << "  After inserting " << i << ": height increased from " << prev_height << " to " << new_height << '\n';
    }
  }

  print_btree(btree, "btree");
  cout << "\nB-Tree properties maintained after all insertions: " << (btree.validate_properties() ? "yes" : "no")
       << '\n';
}

// Demo: Different minimum degrees
void demo_different_degrees() {
  cout << "\n========== Demo: Different Minimum Degrees ==========\n";

  cout << "Higher minimum degree = wider tree = shorter height\n\n";

  const int N = 50;

  // t=2: 2-3-4 tree
  B_Tree<int, 2> btree2;
  for (int i = 1; i <= N; ++i) {
    btree2.insert(i);
  }
  cout << "t=2 (2-3-4 tree): height=" << btree2.height() << ", nodes=" << btree2.count_nodes() << '\n';

  // t=3
  B_Tree<int, 3> btree3;
  for (int i = 1; i <= N; ++i) {
    btree3.insert(i);
  }
  cout << "t=3:             height=" << btree3.height() << ", nodes=" << btree3.count_nodes() << '\n';

  // t=5
  B_Tree<int, 5> btree5;
  for (int i = 1; i <= N; ++i) {
    btree5.insert(i);
  }
  cout << "t=5:             height=" << btree5.height() << ", nodes=" << btree5.count_nodes() << '\n';

  // t=10
  B_Tree<int, 10> btree10;
  for (int i = 1; i <= N; ++i) {
    btree10.insert(i);
  }
  cout << "t=10:            height=" << btree10.height() << ", nodes=" << btree10.count_nodes() << '\n';

  cout << "\nAs t increases, height decreases but node count may vary.\n";
}

// Demo: Search and traversal
void demo_search_traversal() {
  cout << "\n========== Demo: Search and Traversal ==========\n";

  B_Tree<int, 3> btree;

  // Insert in random order
  vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
  cout << "Inserting values in order: ";
  for (int val : values) {
    cout << val << " ";
    btree.insert(val);
  }
  cout << '\n';

  // In-order traversal
  cout << "\nIn-order traversal (sorted): ";
  btree.in_order_traversal([](const int& val) -> void { cout << val << " "; });
  cout << '\n';

  // Using iterators (if available)
  cout << "Using range-based for loop: ";
  for (const auto& val : btree) {
    cout << val << " ";
  }
  cout << '\n';

  // Validate properties
  cout << "\nB-Tree properties valid: " << (btree.validate_properties() ? "yes" : "no") << '\n';
}

// Demo: Random insertions
void demo_random_insertions() {
  cout << "\n========== Demo: Random Insertions ==========\n";

  B_Tree<int, 3> btree;

  // Generate and shuffle values
  vector<int> values;
  for (int i = 1; i <= 100; ++i) {
    values.push_back(i);
  }

  std::random_device              rd;
  std::mt19937                    g(rd());
  std::shuffle(values.begin(), values.end(), g);

  cout << "Inserting 100 values in random order...\n";
  for (int val : values) {
    btree.insert(val);
  }

  cout << "  Size: " << btree.size() << '\n';
  cout << "  Height: " << btree.height() << '\n';
  cout << "  Node count: " << btree.count_nodes() << '\n';
  cout << "  Properties valid: " << (btree.validate_properties() ? "yes" : "no") << '\n';

  // Verify all elements present
  int found_count = 0;
  for (int i = 1; i <= 100; ++i) {
    if (btree.search(i)) {
      ++found_count;
    }
  }
  cout << "  All elements found: " << (found_count == 100 ? "yes" : "no") << '\n';
}

// Demo: Move semantics
void demo_move_semantics() {
  cout << "\n========== Demo: Move Semantics ==========\n";

  B_Tree<int, 3> btree1;
  for (int i = 1; i <= 20; ++i) {
    btree1.insert(i);
  }

  cout << "Original tree:\n";
  print_btree(btree1, "btree1");

  // Move constructor
  B_Tree<int, 3> btree2 = std::move(btree1);

  cout << "\nAfter move construction:\n";
  print_btree(btree1, "btree1 (should be empty)");
  print_btree(btree2, "btree2 (should have the data)");

  // Move assignment
  B_Tree<int, 3> btree3;
  btree3.insert(100);
  btree3.insert(200);

  cout << "\nBefore move assignment:\n";
  print_btree(btree3, "btree3");

  btree3 = std::move(btree2);

  cout << "\nAfter move assignment:\n";
  print_btree(btree2, "btree2 (should be empty)");
  print_btree(btree3, "btree3 (should have btree2's data)");
}

// Demo: String type support
void demo_string_type() {
  cout << "\n========== Demo: String Type Support ==========\n";

  B_Tree<string, 3> btree;

  vector<string> words = {"apple", "banana", "cherry", "date", "elderberry", "fig", "grape", "honeydew", "kiwi", "lemon"};

  cout << "Inserting fruits: ";
  for (const auto& word : words) {
    cout << word << " ";
    btree.insert(word);
  }
  cout << '\n';

  cout << "\nIn-order traversal (alphabetically sorted):\n  ";
  btree.in_order_traversal([](const string& s) -> void { cout << s << " "; });
  cout << '\n';

  cout << "\nSearch operations:\n";
  cout << "  search(\"cherry\"): " << (btree.search("cherry") ? "found" : "not found") << '\n';
  cout << "  search(\"orange\"): " << (btree.search("orange") ? "found" : "not found") << '\n';
}

// Demo: Performance
void demo_performance() {
  cout << "\n========== Demo: Performance ==========\n";

  const int N = 10000;

  cout << "Inserting " << N << " elements sequentially...\n";

  auto start = std::chrono::high_resolution_clock::now();

  B_Tree<int, 5> btree;
  for (int i = 1; i <= N; ++i) {
    btree.insert(i);
  }

  auto end             = std::chrono::high_resolution_clock::now();
  auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "  Insert time: " << insert_duration.count() << " ms\n";
  cout << "  Size: " << btree.size() << '\n';
  cout << "  Height: " << btree.height() << " (theoretical O(log_t(n)) ~ " << (int)(std::log(N) / std::log(5)) << ")\n";

  // Search performance
  cout << "\nSearching for all " << N << " elements...\n";
  start = std::chrono::high_resolution_clock::now();
  for (int i = 1; i <= N; ++i) {
    [[maybe_unused]] bool found = btree.search(i);
  }
  end                  = std::chrono::high_resolution_clock::now();
  auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "  Search time: " << search_duration.count() << " ms\n";
}

// Demo: Clear and reuse
void demo_clear_reuse() {
  cout << "\n========== Demo: Clear and Reuse ==========\n";

  B_Tree<int, 3> btree;

  for (int i = 1; i <= 20; ++i) {
    btree.insert(i);
  }
  cout << "After inserting 20 elements:\n";
  cout << "  Size: " << btree.size() << ", Height: " << btree.height() << '\n';

  btree.clear();
  cout << "\nAfter clear():\n";
  cout << "  Size: " << btree.size() << ", Empty: " << (btree.is_empty() ? "yes" : "no") << '\n';

  btree.insert(42);
  btree.insert(17);
  btree.insert(99);
  cout << "\nAfter inserting new values (42, 17, 99):\n";
  print_btree(btree, "btree");
  cout << "  Properties valid: " << (btree.validate_properties() ? "yes" : "no") << '\n';
}

// Demo: Height comparison for large datasets
void demo_height_comparison() {
  cout << "\n========== Demo: Height Comparison ==========\n";

  const int N = 1000;

  B_Tree<int, 2>  btree2;
  B_Tree<int, 5>  btree5;
  B_Tree<int, 10> btree10;

  for (int i = 1; i <= N; ++i) {
    btree2.insert(i);
    btree5.insert(i);
    btree10.insert(i);
  }

  cout << "For " << N << " elements:\n";
  cout << "  t=2:  height=" << btree2.height() << ", nodes=" << btree2.count_nodes() << '\n';
  cout << "  t=5:  height=" << btree5.height() << ", nodes=" << btree5.count_nodes() << '\n';
  cout << "  t=10: height=" << btree10.height() << ", nodes=" << btree10.count_nodes() << '\n';

  cout << "\nLarger minimum degree results in shorter trees,\n";
  cout << "which means fewer disk accesses in database applications.\n";
}

auto main() -> int {
  try {
    cout << "========================================\n";
    cout << "     B-Tree - Comprehensive Demo\n";
    cout << "========================================\n";

    demo_basic_operations();
    demo_node_splitting();
    demo_different_degrees();
    demo_search_traversal();
    demo_random_insertions();
    demo_move_semantics();
    demo_string_type();
    demo_performance();
    demo_clear_reuse();
    demo_height_comparison();

    cout << "\n========================================\n";
    cout << "  All Demos Completed Successfully!\n";
    cout << "========================================\n";

  } catch (const std::exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
