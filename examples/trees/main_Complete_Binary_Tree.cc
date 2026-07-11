//===---------------------------------------------------------------------------===//
/**
 * @file main_Complete_Binary_Tree.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for CompleteBinaryTree implementation.
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the CompleteBinaryTree data structure,
 * showcasing level-order insertion, various traversals, and tree properties.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/trees/Complete_Binary_Tree.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using ads::arrays::DynamicArray;
using namespace ads::trees;

//===----- HELPER FUNCTIONS ----------------------------------------------------===//

// Helper function to print tree information.
template <typename T>
auto print_tree_info(const CompleteBinaryTree<T>& tree, const string& label) -> void {
  cout << label << "\n";
  cout << "  Size: " << tree.size() << "\n";
  cout << "  Height: " << tree.height() << "\n";
  cout << "  Empty: " << (tree.is_empty() ? "yes" : "no") << "\n";
  if (!tree.is_empty()) {
    cout << "  Root: " << tree.root() << "\n";
  }
}

// Helper function to print traversal results.
auto print_traversal(const DynamicArray<int>& elements, const string& name) -> void {
  cout << "  " << name << ": [ ";
  for (int val : elements) {
    cout << val << " ";
  }
  cout << "]\n";
}

//===----- BASIC OPERATIONS DEMO -----------------------------------------------===//

// Demonstrate basic insertion and properties.
auto demo_basic_operations() -> void {
  ads::demo::print_section("Basic Operations");

  CompleteBinaryTree<int> tree;
  cout << "Created empty tree.\n";
  print_tree_info(tree, "Initial state");

  cout << "\nInserting elements: 1, 2, 3, 4, 5, 6, 7\n";
  for (int i = 1; i <= 7; ++i) {
    tree.insert(i);
  }
  print_tree_info(tree, "After insertions");

  cout << "\nTree structure (level-order insertion creates complete tree):\n";
  cout << "         1\n";
  cout << "       /   \\\n";
  cout << "      2     3\n";
  cout << "     / \\   / \\\n";
  cout << "    4   5 6   7\n";

  ads::demo::print_success("Basic operations work correctly.");
}

//===----- TRAVERSAL DEMO ------------------------------------------------------===//

// Demonstrate various tree traversals.
auto demo_traversals() -> void {
  ads::demo::print_section("Tree Traversals");

  CompleteBinaryTree<int> tree{1, 2, 3, 4, 5, 6, 7};

  cout << "Tree with 7 nodes (complete binary tree):\n";
  cout << "         1\n";
  cout << "       /   \\\n";
  cout << "      2     3\n";
  cout << "     / \\   / \\\n";
  cout << "    4   5 6   7\n\n";

  DynamicArray<int> elements;

  cout << "1. Level-Order (Breadth-First):\n";
  elements.clear();
  tree.level_order_traversal([&elements](int val) -> void { elements.push_back(val); });
  print_traversal(elements, "Level-order");
  cout << "   Visits nodes level by level, left to right.\n";

  cout << "\n2. Pre-Order (Root, Left, Right):\n";
  elements.clear();
  tree.pre_order_traversal([&elements](int val) -> void { elements.push_back(val); });
  print_traversal(elements, "Pre-order ");
  cout << "   Root first, then left subtree, then right subtree.\n";

  cout << "\n3. In-Order (Left, Root, Right):\n";
  elements.clear();
  tree.in_order_traversal([&elements](int val) -> void { elements.push_back(val); });
  print_traversal(elements, "In-order  ");
  cout << "   Left subtree, then root, then right subtree.\n";

  cout << "\n4. Post-Order (Left, Right, Root):\n";
  elements.clear();
  tree.post_order_traversal([&elements](int val) -> void { elements.push_back(val); });
  print_traversal(elements, "Post-order");
  cout << "   Left subtree, then right subtree, then root.\n";

  ads::demo::print_success("All traversals work correctly.");
}

//===----- HEIGHT CALCULATION DEMO ---------------------------------------------===//

// Demonstrate height calculation.
auto demo_height() -> void {
  ads::demo::print_section("Height Calculation");

  CompleteBinaryTree<int> tree;

  cout << "Height at different sizes:\n";
  cout << "  Empty tree: height = " << tree.height() << "\n";

  tree.insert(1);
  cout << "  1 node:     height = " << tree.height() << "\n";

  tree.insert(2);
  tree.insert(3);
  cout << "  3 nodes:    height = " << tree.height() << "\n";

  tree.insert(4);
  tree.insert(5);
  tree.insert(6);
  tree.insert(7);
  cout << "  7 nodes:    height = " << tree.height() << " (perfect binary tree)\n";

  tree.insert(8);
  cout << "  8 nodes:    height = " << tree.height() << " (new level started)\n";

  ads::demo::print_success("Height calculation works correctly.");
}

//===----- SEARCH DEMO ---------------------------------------------------------===//

// Demonstrate search operations.
auto demo_search() -> void {
  ads::demo::print_section("Search Operations");

  CompleteBinaryTree<int> tree{10, 20, 30, 40, 50, 60, 70};

  cout << "Tree contents (level-order): ";
  for (int val : tree.to_vector()) {
    cout << val << " ";
  }
  cout << "\n\n";

  cout << "Searching for elements:\n";
  DynamicArray<int> search_values = {10, 30, 50, 70, 25, 100};
  for (int val : search_values) {
    cout << "  contains(" << val << "): " << (tree.contains(val) ? "true" : "false") << "\n";
  }

  ads::demo::print_success("Search operations work correctly.");
}

//===----- NODE ACCESS DEMO ----------------------------------------------------===//

// Demonstrate accessing nodes directly.
auto demo_node_access() -> void {
  ads::demo::print_section("Node Access");

  CompleteBinaryTree<int> tree{1, 2, 3, 4, 5};

  auto* root = tree.root_node();
  cout << "Root node:\n";
  cout << "  Data: " << root->data << "\n";
  cout << "  Left child: " << (root->left ? std::to_string(root->left->data) : "nullptr") << "\n";
  cout << "  Right child: " << (root->right ? std::to_string(root->right->data) : "nullptr") << "\n";

  cout << "\nNavigating the tree manually:\n";
  cout << "  root->left->left->data = " << root->left->left->data << "\n";
  cout << "  root->left->right->data = " << root->left->right->data << "\n";

  ads::demo::print_success("Node access works correctly.");
}

//===----- EXCEPTION HANDLING DEMO ---------------------------------------------===//

// Demonstrate exception handling for invalid operations.
auto demo_exception_handling() -> void {
  ads::demo::print_section("Exception Handling");

  CompleteBinaryTree<int> tree;

  cout << "Testing root() on empty tree:\n";
  try {
    [[maybe_unused]] auto val = tree.root();
    ads::demo::print_error("  Exception not thrown (unexpected).");
  } catch (const EmptyTreeException& e) {
    ads::demo::print_info("  Caught expected exception: ");
    cout << e.what() << "\n";
  }

  ads::demo::print_success("Exception handling works correctly.");
}

//===----- STRING TYPE DEMO ----------------------------------------------------===//

// Demonstrate CompleteBinaryTree with string type.
auto demo_string_type() -> void {
  ads::demo::print_section("String Type Support");

  CompleteBinaryTree<string> tree;
  tree.emplace("Root");
  tree.emplace("Left");
  tree.emplace("Right");
  tree.emplace("LL");
  tree.emplace("LR");

  cout << "String tree (level-order): ";
  tree.level_order_traversal([](const string& s) -> void { cout << "\"" << s << "\" "; });
  cout << "\n\n";

  cout << "Tree structure:\n";
  cout << "       \"Root\"\n";
  cout << "       /    \\\n";
  cout << "   \"Left\"  \"Right\"\n";
  cout << "    / \\\n";
  cout << " \"LL\" \"LR\"\n";

  ads::demo::print_success("String type support works correctly.");
}

//===----- MAIN FUNCTION -------------------------------------------------------===//

auto main() -> int {
  ads::demo::print_header("COMPLETE BINARY TREE - COMPREHENSIVE DEMO");

  try {
    demo_basic_operations();
    demo_traversals();
    demo_height();
    demo_search();
    demo_node_access();
    demo_exception_handling();
    demo_string_type();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected exception: " << e.what() << "\n";
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
