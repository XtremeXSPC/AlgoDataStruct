//===--------------------------------------------------------------------------===//
/**
 * @file Red_Black_Tree.hpp
 * @author Costantino Lombardi
 * @brief Red-Black Tree - Self-balancing Binary Search Tree with color properties
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * A Red-Black Tree is a self-balancing binary search tree where each node stores
 * an extra bit representing "color" (red or black). These colors ensure the tree
 * remains approximately balanced during insertions and deletions.
 *
 * Red-Black Properties:
 * 1. Every node is either red or black
 * 2. The root is always black
 * 3. All leaves (NIL) are black
 * 4. If a node is red, both children are black (no two reds in a row)
 * 5. Every path from root to leaf contains the same number of black nodes
 *
 * Performance Advantages:
 * - O(log n) worst-case for insert/delete/search
 * - Fewer rotations than AVL (better for insert-heavy workloads)
 * - Maximum height: 2 * log(n + 1)
 * - Used in std::map, std::set, Linux kernel's CFS scheduler
 *
 * Comparison with AVL:
 * - AVL: More strictly balanced (faster search)
 * - RB:  Fewer rotations (faster insert/delete)
 * - AVL: Max 1 rotation on insert, O(log n) on delete
 * - RB:  Max 2 rotations on insert, max 3 on delete
 */
//===--------------------------------------------------------------------------===//
#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

#include "Binary_Tree_Exception.hpp"

namespace ads::trees {

/**
 * @brief Node color enum for Red-Black Tree
 */
enum class Color { Red, Black };

/**
 * @brief Red-Black Tree implementation
 *
 * @tparam T Type of elements stored (must be comparable with <)
 *
 * @complexity
 * - Insert: O(log n) time, at most 2 rotations
 * - Delete: O(log n) time, at most 3 rotations
 * - Search: O(log n) time
 * - Space:  O(n) with 1 bit per node for color
 */
template <typename T>
class Red_Black_Tree {
private:
  /**
   * @brief Internal node structure
   */
  struct Node {
    T                     data;
    Color                 color;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    Node*                 parent; // Non-owning pointer for upward traversal

    Node(const T& val, Color col = Color::Red, Node* par = nullptr) : data(val), color(col), left(nullptr), right(nullptr), parent(par) {}

    Node(T&& val, Color col = Color::Red, Node* par = nullptr) :
        data(std::move(val)), color(col), left(nullptr), right(nullptr), parent(par) {}
  };

  std::unique_ptr<Node> root_;
  size_t                size_{};

  /**
   * @brief Get color of node (nullptr is considered black)
   */
  static auto get_color(const Node* node) -> Color { return (node == nullptr) ? Color::Black : node->color; }

  /**
   * @brief Set color of node (safe for nullptr)
   */
  static void set_color(Node* node, Color color) {
    if (node != nullptr) {
      node->color = color;
    }
  }

  /**
   * @brief Left rotation around node x
   *
   *     x                y
   *    / \              / \
   *   α   y     =>     x   γ
   *      / \          / \
   *     β   γ        α   β
   *
   * @param x_ptr Unique pointer to node x (will be modified)
   * @return Unique pointer to new subtree root (y)
   */
  auto rotate_left(std::unique_ptr<Node> x_ptr) -> std::unique_ptr<Node> {
    auto  y_ptr = std::move(x_ptr->right);
    Node* x     = x_ptr.get();
    Node* y     = y_ptr.get();

    // Turn y's left subtree into x's right subtree
    x_ptr->right = std::move(y_ptr->left);
    if (x_ptr->right) {
      x_ptr->right->parent = x;
    }

    // Link x's parent to y
    y->parent = x->parent;

    // y's left child becomes x
    y->left         = std::move(x_ptr);
    y->left->parent = y;

    return y_ptr;
  }

  /**
   * @brief Right rotation around node y
   *
   *       y            x
   *      / \          / \
   *     x   γ   =>   α   y
   *    / \              / \
   *   α   β            β   γ
   *
   * @param y_ptr Unique pointer to node y (will be modified)
   * @return Unique pointer to new subtree root (x)
   */
  auto rotate_right(std::unique_ptr<Node> y_ptr) -> std::unique_ptr<Node> {
    auto  x_ptr = std::move(y_ptr->left);
    Node* y     = y_ptr.get();
    Node* x     = x_ptr.get();

    // Turn x's right subtree into y's left subtree
    y_ptr->left = std::move(x_ptr->right);
    if (y_ptr->left) {
      y_ptr->left->parent = y;
    }

    // Link y's parent to x
    x->parent = y->parent;

    // x's right child becomes y
    x->right         = std::move(y_ptr);
    x->right->parent = x;

    return x_ptr;
  }

  /**
   * @brief Fix Red-Black properties after insertion
   *
   * After standard BST insert of a RED node, this fixes violations.
   * Three cases based on uncle's color and node position.
   *
   * @param node Newly inserted node (initially red)
   */
  void insert_fixup(Node* node) {
    // Continue until we reach root or parent is black
    while (node != root_.get() && get_color(node->parent) == Color::Red) {
      Node* parent      = node->parent;
      Node* grandparent = parent->parent;

      if (parent == grandparent->left.get()) {
        // Parent is left child of grandparent
        Node* uncle = grandparent->right.get();

        if (get_color(uncle) == Color::Red) {
          // Case 1: Uncle is red - Recolor
          set_color(parent, Color::Black);
          set_color(uncle, Color::Black);
          set_color(grandparent, Color::Red);
          node = grandparent; // Continue from grandparent
        } else {
          // Uncle is black
          if (node == parent->right.get()) {
            // Case 2: Node is right child - Left rotate parent
            node = parent;
            // Rotate at parent level
            if (grandparent->left.get() == parent) {
              grandparent->left = rotate_left(std::move(grandparent->left));
              parent            = node->parent;
            }
          }
          // Case 3: Node is left child - Recolor and right rotate grandparent
          set_color(parent, Color::Black);
          set_color(grandparent, Color::Red);

          Node* great_grandparent = grandparent->parent;
          if (great_grandparent == nullptr) {
            // Rotating root
            root_ = rotate_right(std::move(root_));
          } else if (great_grandparent->left.get() == grandparent) {
            great_grandparent->left = rotate_right(std::move(great_grandparent->left));
          } else {
            great_grandparent->right = rotate_right(std::move(great_grandparent->right));
          }
          break;
        }
      } else {
        // Parent is right child of grandparent (symmetric cases)
        Node* uncle = grandparent->left.get();

        if (get_color(uncle) == Color::Red) {
          // Case 1: Uncle is red - Recolor
          set_color(parent, Color::Black);
          set_color(uncle, Color::Black);
          set_color(grandparent, Color::Red);
          node = grandparent;
        } else {
          // Uncle is black
          if (node == parent->left.get()) {
            // Case 2: Node is left child - Right rotate parent
            node = parent;
            if (grandparent->right.get() == parent) {
              grandparent->right = rotate_right(std::move(grandparent->right));
              parent             = node->parent;
            }
          }
          // Case 3: Node is right child - Recolor and left rotate grandparent
          set_color(parent, Color::Black);
          set_color(grandparent, Color::Red);

          Node* great_grandparent = grandparent->parent;
          if (great_grandparent == nullptr) {
            root_ = rotate_left(std::move(root_));
          } else if (great_grandparent->left.get() == grandparent) {
            great_grandparent->left = rotate_left(std::move(great_grandparent->left));
          } else {
            great_grandparent->right = rotate_left(std::move(great_grandparent->right));
          }
          break;
        }
      }
    }

    // Ensure root is always black
    root_->color = Color::Black;
  }

  /**
   * @brief Recursive insert helper
   */
  auto insert_helper(std::unique_ptr<Node>& node, const T& value, Node* parent) -> std::pair<Node*, bool> {
    if (!node) {
      node = std::make_unique<Node>(value, Color::Red, parent);
      size_++;
      return {node.get(), true};
    }

    if (value < node->data) {
      return insert_helper(node->left, value, node.get());
    } else if (value > node->data) {
      return insert_helper(node->right, value, node.get());
    } else {
      // Duplicate - do not insert
      return {nullptr, false};
    }
  }

  /**
   * @brief Find minimum node in subtree
   */
  static auto find_min(Node* node) -> Node* {
    while (node && node->left) {
      node = node->left.get();
    }
    return node;
  }

  /**
   * @brief Recursive search helper
   */
  auto search_helper(const Node* node, const T& value) const -> bool {
    if (!node) {
      return false;
    }
    if (value == node->data) {
      return true;
    } else if (value < node->data) {
      return search_helper(node->left.get(), value);
    } else {
      return search_helper(node->right.get(), value);
    }
  }

  /**
   * @brief In-order traversal helper
   */
  void in_order_helper(const Node* node, std::function<void(const T&)> visit) const {
    if (node) {
      in_order_helper(node->left.get(), visit);
      visit(node->data);
      in_order_helper(node->right.get(), visit);
    }
  }

  /**
   * @brief Calculate height of subtree
   */
  auto height_helper(const Node* node) const -> int {
    if (!node) {
      return -1;
    }
    return 1 + std::max(height_helper(node->left.get()), height_helper(node->right.get()));
  }

  /**
   * @brief Calculate black height (number of black nodes from node to leaf)
   */
  auto black_height_helper(const Node* node) const -> int {
    if (!node) {
      return 0; // NIL is black
    }
    int left_bh = black_height_helper(node->left.get());
    return left_bh + (node->color == Color::Black ? 1 : 0);
  }

  /**
   * @brief Validate Red-Black properties recursively
   * @return Black height of subtree, or -1 if invalid
   */
  auto validate_helper(const Node* node) const -> int {
    if (!node) {
      return 0; // NIL leaves are black
    }

    // Property 4: If node is red, children must be black
    if (node->color == Color::Red) {
      if (get_color(node->left.get()) == Color::Red || get_color(node->right.get()) == Color::Red) {
        return -1; // Violation: red node has red child
      }
    }

    // Property 5: Black heights must be equal for all paths
    int left_bh  = validate_helper(node->left.get());
    int right_bh = validate_helper(node->right.get());

    if (left_bh == -1 || right_bh == -1 || left_bh != right_bh) {
      return -1; // Violation: unequal black heights
    }

    // Return black height of this subtree
    return left_bh + (node->color == Color::Black ? 1 : 0);
  }

public:
  /**
   * @brief Construct empty Red-Black Tree
   */
  Red_Black_Tree() : root_(nullptr), size_(0) {}

  /**
   * @brief Destructor (automatic via unique_ptr)
   */
  ~Red_Black_Tree() = default;

  // Disable copy (would be expensive)
  Red_Black_Tree(const Red_Black_Tree&)                    = delete;
  auto operator=(const Red_Black_Tree&) -> Red_Black_Tree& = delete;

  // Enable move
  Red_Black_Tree(Red_Black_Tree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) { other.size_ = 0; }
  auto operator=(Red_Black_Tree&& other) noexcept -> Red_Black_Tree& {
    if (this != &other) {
      root_ = std::move(other.root_);
      size_ = other.size_;
      other.size_ = 0;
    }
    return *this;
  }

  /**
   * @brief Insert a value into the tree
   *
   * Performs standard BST insert, then fixes Red-Black properties.
   * New nodes are initially red.
   *
   * @param value Value to insert
   * @return true if inserted, false if duplicate
   * @complexity O(log n) time, at most 2 rotations
   */
  auto insert(const T& value) -> bool {
    auto [new_node, inserted] = insert_helper(root_, value, nullptr);

    if (inserted) {
      insert_fixup(new_node);
      return true;
    }
    return false;
  }

  /**
   * @brief Search for a value
   * @param value Value to search
   * @return true if found
   * @complexity O(log n)
   */
  auto search(const T& value) const -> bool { return search_helper(root_.get(), value); }

  /**
   * @brief Alias for search
   */
  auto contains(const T& value) const -> bool { return search(value); }

  /**
   * @brief Get number of elements
   * @complexity O(1)
   */
  [[nodiscard]] auto size() const -> size_t { return size_; }

  /**
   * @brief Check if tree is empty
   * @complexity O(1)
   */
  [[nodiscard]] auto is_empty() const -> bool { return size_ == 0; }

  /**
   * @brief Clear all elements
   * @complexity O(n)
   */
  void clear() {
    root_.reset();
    size_ = 0;
  }

  /**
   * @brief Get height of tree
   * @return Height (empty tree = -1, single node = 0)
   * @complexity O(n)
   */
  [[nodiscard]] auto height() const -> int { return height_helper(root_.get()); }

  /**
   * @brief Get smallest value in the tree
   * @throws EmptyTreeException if the tree is empty
   */
  [[nodiscard]] auto find_min() const -> const T& {
    if (is_empty()) {
      throw EmptyTreeException("Red-Black Tree is empty");
    }

    const Node* node = root_.get();
    while (node->left) {
      node = node->left.get();
    }
    return node->data;
  }

  /**
   * @brief Get largest value in the tree
   * @throws EmptyTreeException if the tree is empty
   */
  [[nodiscard]] auto find_max() const -> const T& {
    if (is_empty()) {
      throw EmptyTreeException("Red-Black Tree is empty");
    }

    const Node* node = root_.get();
    while (node->right) {
      node = node->right.get();
    }
    return node->data;
  }

  /**
   * @brief Get black height of tree
   * @return Number of black nodes from root to leaf
   * @complexity O(log n)
   */
  [[nodiscard]] auto black_height() const -> int { return black_height_helper(root_.get()); }

  /**
   * @brief Validate Red-Black Tree properties
   *
   * Checks:
   * 1. Root is black
   * 2. No red node has red child
   * 3. All paths have equal black height
   *
   * @return true if all properties satisfied
   * @complexity O(n)
   */
  [[nodiscard]] auto validate_properties() const -> bool {
    if (!root_) {
      return true; // Empty tree is valid
    }

    // Property 2: Root must be black
    if (root_->color != Color::Black) {
      return false;
    }

    // Validate remaining properties recursively
    return validate_helper(root_.get()) != -1;
  }

  /**
   * @brief In-order traversal
   * @param visit Function to call for each element
   * @complexity O(n)
   */
  void in_order_traversal(std::function<void(const T&)> visit) const { in_order_helper(root_.get(), visit); }
};

} // namespace ads::trees

//===--------------------------------------------------------------------------===//
