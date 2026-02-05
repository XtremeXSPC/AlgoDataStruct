//===---------------------------------------------------------------------------===//
/**
 * @file main_AVL_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for AVL Tree implementation.
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the AVL Tree data structure,showcasing its
 * self-balancing properties through various rotations.
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/trees/AVL_Tree.hpp"
#include "../include/ads/trees/Binary_Search_Tree.hpp"
#include "support/Demo_Utilities.hpp"

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::to_string;
using std::vector;

using namespace ads::trees;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Helper function to print tree contents via in-order traversal.
template <typename T>
void print_avl_tree(const AVLTree<T>& tree, const string& name) {
  cout << "AVL Tree '" << name << "' (size: " << tree.size() << ", height: " << tree.height()
       << ", balanced: " << (tree.is_balanced() ? "Yes" : "No") << "):\n";

  if (tree.is_empty()) {
    cout << "  (empty)\n";
    return;
  }

  cout << "  In-order: ";
  tree.in_order_traversal([](const T& value) -> void { cout << value << " "; });
  cout << '\n';
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Demonstrates basic insertion and traversal operations.
void demo_basic_operations() {
  ads::demo::print_section("Demo: Basic Operations");

  AVLTree<int> avl;

  // Test insertion.
  cout << "Inserting values: 50, 30, 70, 20, 40, 60, 80\n";
  avl.insert(50);
  avl.insert(30);
  avl.insert(70);
  avl.insert(20);
  avl.insert(40);
  avl.insert(60);
  avl.insert(80);

  print_avl_tree(avl, "avl");

  // Test different traversals.
  cout << "\nDifferent traversals:\n";

  cout << "  Pre-order:  ";
  avl.pre_order_traversal([](const int& value) -> void { cout << value << " "; });
  cout << '\n';

  cout << "  Post-order: ";
  avl.post_order_traversal([](const int& value) -> void { cout << value << " "; });
  cout << '\n';

  cout << "  Level-order: ";
  avl.level_order_traversal([](const int& value) -> void { cout << value << " "; });
  cout << '\n';

  // Test duplicates.
  cout << "\nTrying to insert duplicate (40): ";
  bool inserted = avl.insert(40);
  cout << (inserted ? "inserted" : "not inserted (correct behavior)") << '\n';
  print_avl_tree(avl, "avl after duplicate attempt");
}

//===----------------------------- ROTATION DEMOS ------------------------------===//

// Test Left-Left (LL) rotation.
void demo_ll_rotation() {
  ads::demo::print_section("Demo: Left-Left (LL) Rotation");

  AVLTree<int> avl;

  cout << "Inserting descending sequence: 30, 20, 10\n";
  cout << "This should trigger a Right rotation (LL case)\n\n";

  avl.insert(30);
  cout << "After inserting 30: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  avl.insert(20);
  cout << "After inserting 20: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  avl.insert(10);
  cout << "After inserting 10: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  print_avl_tree(avl, "avl after LL rotation");

  cout << "\nExpected structure (20 at root):\n";
  cout << "    20\n";
  cout << "   /  \\\n";
  cout << "  10  30\n\n";
}

// Test Right-Right (RR) rotation.
void demo_rr_rotation() {
  ads::demo::print_section("Demo: Right-Right (RR) Rotation");

  AVLTree<int> avl;

  cout << "Inserting ascending sequence: 10, 20, 30\n";
  cout << "This should trigger a Left rotation (RR case)\n\n";

  avl.insert(10);
  cout << "After inserting 10: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  avl.insert(20);
  cout << "After inserting 20: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  avl.insert(30);
  cout << "After inserting 30: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  print_avl_tree(avl, "avl after RR rotation");

  cout << "\nExpected structure (20 at root):\n";
  cout << "    20\n";
  cout << "   /  \\\n";
  cout << "  10  30\n\n";
}

// Test Left-Right (LR) rotation.
void demo_lr_rotation() {
  ads::demo::print_section("Demo: Left-Right (LR) Rotation");

  AVLTree<int> avl;

  cout << "Inserting sequence: 30, 10, 20\n";
  cout << "This should trigger a Left-Right rotation (LR case)\n\n";

  avl.insert(30);
  cout << "After inserting 30: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  avl.insert(10);
  cout << "After inserting 10: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  avl.insert(20);
  cout << "After inserting 20: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  print_avl_tree(avl, "avl after LR rotation");

  cout << "\nExpected structure (20 at root):\n";
  cout << "    20\n";
  cout << "   /  \\\n";
  cout << "  10  30\n\n";
}

// Test Right-Left (RL) rotation.
void demo_rl_rotation() {
  ads::demo::print_section("Demo: Right-Left (RL) Rotation");

  AVLTree<int> avl;

  cout << "Inserting sequence: 10, 30, 20\n";
  cout << "This should trigger a Right-Left rotation (RL case)\n\n";

  avl.insert(10);
  cout << "After inserting 10: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  avl.insert(30);
  cout << "After inserting 30: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  avl.insert(20);
  cout << "After inserting 20: height=" << avl.height() << ", balanced=" << avl.is_balanced() << '\n';

  print_avl_tree(avl, "avl after RL rotation");

  cout << "\nExpected structure (20 at root):\n";
  cout << "    20\n";
  cout << "   /  \\\n";
  cout << "  10  30\n\n";
}

//===------------------------- SEARCH OPERATIONS DEMO --------------------------===//

// Demonstrates search operations.
void demo_search_operations() {
  ads::demo::print_section("Demo: Search Operations");

  AVLTree<int> avl;

  // Build a tree.
  vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
  for (int val : values) {
    avl.insert(val);
  }

  print_avl_tree(avl, "avl");

  // Test contains.
  cout << "\nTesting contains():\n";
  cout << "  Contains 40? " << (avl.contains(40) ? "Yes" : "No") << '\n';
  cout << "  Contains 100? " << (avl.contains(100) ? "Yes" : "No") << '\n';

  // Test min/max.
  cout << "\nTesting find_min() and find_max():\n";
  cout << "  Minimum: " << avl.find_min() << '\n';
  cout << "  Maximum: " << avl.find_max() << '\n';
}

//===---------------------------- REMOVE OPERATIONS ----------------------------===//

// Demonstrates remove operations.
void demo_remove_operations() {
  ads::demo::print_section("Demo: Remove Operations");

  AVLTree<int> avl;

  // Build a tree.
  vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45, 65, 75, 90};
  for (int val : values) {
    avl.insert(val);
  }

  print_avl_tree(avl, "avl before removals");

  // Remove leaf node.
  cout << "\nRemoving leaf node (10):\n";
  avl.remove(10);
  print_avl_tree(avl, "avl after removing 10");
  cout << "Still balanced: " << (avl.is_balanced() ? "Yes" : "No") << '\n';

  // Remove node with one child.
  cout << "\nRemoving node with one child (90):\n";
  avl.remove(90);
  print_avl_tree(avl, "avl after removing 90");
  cout << "Still balanced: " << (avl.is_balanced() ? "Yes" : "No") << '\n';

  // Remove node with two children.
  cout << "\nRemoving node with two children (30):\n";
  avl.remove(30);
  print_avl_tree(avl, "avl after removing 30");
  cout << "Still balanced: " << (avl.is_balanced() ? "Yes" : "No") << '\n';

  // Remove root.
  cout << "\nRemoving root node (50):\n";
  avl.remove(50);
  print_avl_tree(avl, "avl after removing root");
  cout << "Still balanced: " << (avl.is_balanced() ? "Yes" : "No") << '\n';
}

//===------------------------------ ITERATOR DEMO ------------------------------===//

// Demonstrates iterator functionality.
void demo_iterator() {
  ads::demo::print_section("Demo: Iterator");

  AVLTree<int> avl;

  vector<int> values = {50, 30, 70, 20, 40, 60, 80};
  for (int val : values) {
    avl.insert(val);
  }

  cout << "Iterating through AVL tree (should be in sorted order):\n  ";
  for (const auto& value : avl) {
    cout << value << " ";
  }
  cout << '\n';
}

//===--------------------------- MOVE SEMANTICS DEMO ---------------------------===//

// Demonstrates move semantics.
void demo_move_semantics() {
  ads::demo::print_section("Demo: Move Semantics");

  AVLTree<int> avl1;
  avl1.insert(50);
  avl1.insert(30);
  avl1.insert(70);

  cout << "Original tree:\n";
  print_avl_tree(avl1, "avl1");

  // Move constructor.
  AVLTree<int> avl2(std::move(avl1));
  cout << "\nAfter move construction:\n";
  print_avl_tree(avl2, "avl2");
  cout << "avl1 size after move: " << avl1.size() << " (should be 0)\n";

  // Move assignment.
  AVLTree<int> avl3;
  avl3 = std::move(avl2);
  cout << "\nAfter move assignment:\n";
  print_avl_tree(avl3, "avl3");
  cout << "avl2 size after move: " << avl2.size() << " (should be 0)\n";
}

//===-------------------------- SORTED SEQUENCE DEMO ---------------------------===//

// Test with large sorted sequence (worst case for unbalanced BST).
void demo_sorted_sequence() {
  ads::demo::print_section("Demo: Sorted Sequence (AVL vs BST)");

  AVLTree<int>          avl;
  BinarySearchTree<int> bst;

  const int N = 1'000;
  cout << "Inserting sorted sequence from 1 to " << N << "\n\n";

  // Insert into both trees.
  for (int i = 1; i <= N; ++i) {
    avl.insert(i);
    bst.insert(i);
  }

  cout << "AVL Tree:\n";
  cout << "  Size:   " << avl.size() << '\n';
  cout << "  Height: " << avl.height() << " (should be ~" << (int)(1.44 * std::log2(N)) << ")\n";
  cout << "  Balanced: " << (avl.is_balanced() ? "Yes" : "No") << '\n';

  cout << "\nBST (unbalanced):\n";
  cout << "  Size:   " << bst.size() << '\n';
  cout << "  Height: " << bst.height() << " (degrades to " << N << ")\n";

  cout << "\nHeight ratio (BST/AVL): " << (double)bst.height() / avl.height() << "x\n";
  cout << "This demonstrates why balancing is crucial!\n";
}

//===------------------------- PERFORMANCE COMPARISON --------------------------===//

// Performance test: AVL vs BST.
void demo_performance() {
  ads::demo::print_section("Demo: Performance Comparison");

  const int N = 10'000;

  // Random number generator.
  std::random_device              rd;
  std::mt19937                    gen(rd());
  std::uniform_int_distribution<> dis(1, N * 10);

  vector<int> random_values;
  random_values.reserve(N);
  for (int i = 0; i < N; ++i) {
    random_values.push_back(dis(gen));
  }

  // Test AVL Tree.
  auto start = std::chrono::high_resolution_clock::now();

  AVLTree<int> avl;
  for (int val : random_values) {
    avl.insert(val);
  }

  auto end      = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "AVL Tree insertion of " << N << " random values:\n";
  cout << "  Time:    " << duration.count() << " ms\n";
  cout << "  Height:  " << avl.height() << '\n';
  cout << "  Balanced: " << (avl.is_balanced() ? "Yes" : "No") << '\n';

  // Test BST.
  start = std::chrono::high_resolution_clock::now();

  BinarySearchTree<int> bst;
  for (int val : random_values) {
    bst.insert(val);
  }

  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "\nBST insertion of " << N << " random values:\n";
  cout << "  Time:   " << duration.count() << " ms\n";
  cout << "  Height: " << bst.height() << '\n';

  cout << "\nFor random insertions, both trees have similar heights,\n";
  cout << "but AVL guarantees O(log n) even in the worst case.\n";
}

//===----------------------------- EDGE CASES DEMO -----------------------------===//

// Test edge cases.
void demo_edge_cases() {
  ads::demo::print_section("Demo: Edge Cases");

  AVLTree<int> avl;

  // Test operations on empty tree.
  cout << "Testing operations on empty tree:\n";
  cout << "  is_empty(): " << (avl.is_empty() ? "true" : "false") << '\n';
  cout << "  size(): " << avl.size() << '\n';
  cout << "  height(): " << avl.height() << '\n';
  cout << "  is_balanced(): " << (avl.is_balanced() ? "true" : "false") << '\n';
  cout << "  contains(42): " << (avl.contains(42) ? "true" : "false") << '\n';
  cout << "  remove(42): " << (avl.remove(42) ? "true" : "false") << '\n';

  // Test find_min/max on empty tree.
  cout << "\nTesting find_min() on empty tree (should throw):\n";
  try {
    [[maybe_unused]] auto min_val = avl.find_min();
    cout << "  ERROR: No exception thrown!\n";
  } catch (const exception& e) {
    cout << "  Caught exception: " << e.what() << '\n';
  }

  // Test single element.
  cout << "\nTesting single element:\n";
  avl.insert(42);
  print_avl_tree(avl, "avl with single element");

  // Test clear.
  cout << "\nTesting clear():\n";
  [[maybe_unused]] bool inserted1 = avl.insert(10);
  [[maybe_unused]] bool inserted2 = avl.insert(20);
  [[maybe_unused]] bool inserted3 = avl.insert(30);
  cout << "Before clear: size=" << avl.size() << '\n';
  avl.clear();
  cout << "After clear: size=" << avl.size() << ", is_empty=" << avl.is_empty() << '\n';
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  ads::demo::print_header("AVL TREE USAGE EXAMPLES");

  try {
    demo_basic_operations();
    demo_ll_rotation();
    demo_rr_rotation();
    demo_lr_rotation();
    demo_rl_rotation();
    demo_search_operations();
    demo_remove_operations();
    demo_iterator();
    demo_move_semantics();
    demo_sorted_sequence();
    demo_performance();
    demo_edge_cases();

    ads::demo::print_footer();

    return 0;

  } catch (const exception& e) {
    cerr << "\n[FATAL ERROR] Unhandled exception: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
