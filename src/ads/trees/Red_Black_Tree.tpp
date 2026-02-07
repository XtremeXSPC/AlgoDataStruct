//===---------------------------------------------------------------------------===//
/**
 * @file Red_Black_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the Red-Black Tree class.
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/trees/Red_Black_Tree.hpp"

namespace ads::trees {

//===--------------------------- NODE IMPLEMENTATION ---------------------------===//

template <OrderedTreeElement T>
Red_Black_Tree<T>::Node::Node(const T& val, Color col, Node* par) :
    data(val),
    color(col),
    left(nullptr),
    right(nullptr),
    parent(par) {
}

template <OrderedTreeElement T>
Red_Black_Tree<T>::Node::Node(T&& val, Color col, Node* par) :
    data(std::move(val)),
    color(col),
    left(nullptr),
    right(nullptr),
    parent(par) {
}

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

template <OrderedTreeElement T>
Red_Black_Tree<T>::Red_Black_Tree() : root_(nullptr), size_(0) {
}

template <OrderedTreeElement T>
Red_Black_Tree<T>::Red_Black_Tree(Red_Black_Tree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) {
  other.size_ = 0;
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::operator=(Red_Black_Tree&& other) noexcept -> Red_Black_Tree& {
  if (this != &other) {
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::insert(const T& value) -> bool {
  auto [new_node, inserted] = insert_helper(root_, value, nullptr);

  if (inserted) {
    insert_fixup(new_node);
    return true;
  }
  return false;
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::remove(const T& value) -> bool {
  if (is_empty()) {
    return false;
  }

  std::vector<T> retained_values;
  retained_values.reserve(size_ > 0 ? size_ - 1 : 0);

  bool removed = false;
  in_order_traversal([&](const T& current_value) {
    if (!removed && current_value == value) {
      removed = true;
      return;
    }
    retained_values.push_back(current_value);
  });

  if (!removed) {
    return false;
  }

  Red_Black_Tree rebuilt_tree;
  for (const auto& current_value : retained_values) {
    rebuilt_tree.insert(current_value);
  }

  *this = std::move(rebuilt_tree);
  return true;
}

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <OrderedTreeElement T>
void Red_Black_Tree<T>::clear() {
  root_.reset();
  size_ = 0;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::is_empty() const -> bool {
  return size_ == 0;
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::size() const -> size_t {
  return size_;
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::height() const -> int {
  return height_helper(root_.get());
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::contains(const T& value) const -> bool {
  return search(value);
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::find_min() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("Red-Black Tree is empty");
  }

  const Node* node = root_.get();
  while (node->left) {
    node = node->left.get();
  }
  return node->data;
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::find_max() const -> const T& {
  if (is_empty()) {
    throw EmptyTreeException("Red-Black Tree is empty");
  }

  const Node* node = root_.get();
  while (node->right) {
    node = node->right.get();
  }
  return node->data;
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::search(const T& value) const -> bool {
  return search_helper(root_.get(), value);
}

//===------------------- RED-BLACK TREE SPECIFIC OPERATIONS -------------------===//

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::black_height() const -> int {
  return black_height_helper(root_.get());
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::validate_properties() const -> bool {
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

//===-------------------------- TRAVERSAL OPERATIONS ---------------------------===//

template <OrderedTreeElement T>
void Red_Black_Tree<T>::in_order_traversal(std::function<void(const T&)> visit) const {
  in_order_helper(root_.get(), visit);
}

template <OrderedTreeElement T>
void Red_Black_Tree<T>::pre_order_traversal(std::function<void(const T&)> visit) const {
  pre_order_helper(root_.get(), visit);
}

template <OrderedTreeElement T>
void Red_Black_Tree<T>::level_order_traversal(std::function<void(const T&)> visit) const {
  if (!root_) {
    return;
  }

  std::queue<const Node*> node_queue;
  node_queue.push(root_.get());

  while (!node_queue.empty()) {
    const Node* current = node_queue.front();
    node_queue.pop();

    visit(current->data);

    if (current->left) {
      node_queue.push(current->left.get());
    }
    if (current->right) {
      node_queue.push(current->right.get());
    }
  }
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::get_color(const Node* node) -> Color {
  return (node == nullptr) ? Color::Black : node->color;
}

template <OrderedTreeElement T>
void Red_Black_Tree<T>::set_color(Node* node, Color color) {
  if (node != nullptr) {
    node->color = color;
  }
}

//===--------------------------- ROTATION OPERATIONS ---------------------------===//

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::rotate_left(std::unique_ptr<Node> x_ptr) -> std::unique_ptr<Node> {
  auto  y_ptr = std::move(x_ptr->right);
  Node* x     = x_ptr.get();
  Node* y     = y_ptr.get();

  // Turn y's left subtree into x's right subtree.
  x_ptr->right = std::move(y_ptr->left);
  if (x_ptr->right) {
    x_ptr->right->parent = x;
  }

  // Link x's parent to y.
  y->parent = x->parent;

  // y's left child becomes x.
  y->left         = std::move(x_ptr);
  y->left->parent = y;

  return y_ptr;
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::rotate_right(std::unique_ptr<Node> y_ptr) -> std::unique_ptr<Node> {
  auto  x_ptr = std::move(y_ptr->left);
  Node* y     = y_ptr.get();
  Node* x     = x_ptr.get();

  // Turn x's right subtree into y's left subtree.
  y_ptr->left = std::move(x_ptr->right);
  if (y_ptr->left) {
    y_ptr->left->parent = y;
  }

  // Link y's parent to x.
  x->parent = y->parent;

  // x's right child becomes y.
  x->right         = std::move(y_ptr);
  x->right->parent = x;

  return x_ptr;
}

//===---------------------------- INSERTION HELPERS ----------------------------===//

template <OrderedTreeElement T>
void Red_Black_Tree<T>::insert_fixup(Node* node) {
  // Continue until we reach root or parent is black.
  while (node != root_.get() && get_color(node->parent) == Color::Red) {
    Node* parent      = node->parent;
    Node* grandparent = parent->parent;

    if (parent == grandparent->left.get()) {
      // Parent is left child of grandparent.
      Node* uncle = grandparent->right.get();

      if (get_color(uncle) == Color::Red) {
        // Case 1: Uncle is red - Recolor.
        set_color(parent, Color::Black);
        set_color(uncle, Color::Black);
        set_color(grandparent, Color::Red);
        node = grandparent; // Continue from grandparent.
      } else {
        // Uncle is black.
        if (node == parent->right.get()) {
          // Case 2: Node is right child - Left rotate parent.
          node = parent;
          // Rotate at parent level.
          if (grandparent->left.get() == parent) {
            grandparent->left = rotate_left(std::move(grandparent->left));
            parent            = node->parent;
          }
        }
        // Case 3: Node is left child - Recolor and right rotate grandparent.
        set_color(parent, Color::Black);
        set_color(grandparent, Color::Red);

        Node* great_grandparent = grandparent->parent;
        if (great_grandparent == nullptr) {
          // Rotating root.
          root_ = rotate_right(std::move(root_));
        } else if (great_grandparent->left.get() == grandparent) {
          great_grandparent->left = rotate_right(std::move(great_grandparent->left));
        } else {
          great_grandparent->right = rotate_right(std::move(great_grandparent->right));
        }
        break;
      }
    } else {
      // Parent is right child of grandparent (symmetric cases).
      Node* uncle = grandparent->left.get();

      if (get_color(uncle) == Color::Red) {
        // Case 1: Uncle is red - Recolor.
        set_color(parent, Color::Black);
        set_color(uncle, Color::Black);
        set_color(grandparent, Color::Red);
        node = grandparent;
      } else {
        // Uncle is black.
        if (node == parent->left.get()) {
          // Case 2: Node is left child - Right rotate parent.
          node = parent;
          if (grandparent->right.get() == parent) {
            grandparent->right = rotate_right(std::move(grandparent->right));
            parent             = node->parent;
          }
        }
        // Case 3: Node is right child - Recolor and left rotate grandparent.
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

  // Ensure root is always black.
  root_->color = Color::Black;
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::insert_helper(std::unique_ptr<Node>& node, const T& value, Node* parent)
    -> std::pair<Node*, bool> {
  if (!node) {
    node = std::make_unique<Node>(value, Color::Red, parent);
    size_++;
    return {node.get(), true};
  }

  if (value < node->data) {
    return insert_helper(node->left, value, node.get());
  } else if (node->data < value) {
    return insert_helper(node->right, value, node.get());
  } else {
    // Duplicate - do not insert
    return {nullptr, false};
  }
}

//===----------------------------- SEARCH HELPERS ------------------------------===//

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::find_min_node(Node* node) -> Node* {
  while (node && node->left) {
    node = node->left.get();
  }
  return node;
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::search_helper(const Node* node, const T& value) const -> bool {
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

//===---------------------------- TRAVERSAL HELPERS ----------------------------===//

template <OrderedTreeElement T>
void Red_Black_Tree<T>::in_order_helper(const Node* node, std::function<void(const T&)> visit) const {
  if (node) {
    in_order_helper(node->left.get(), visit);
    visit(node->data);
    in_order_helper(node->right.get(), visit);
  }
}

template <OrderedTreeElement T>
void Red_Black_Tree<T>::pre_order_helper(const Node* node, std::function<void(const T&)> visit) const {
  if (node) {
    visit(node->data);
    pre_order_helper(node->left.get(), visit);
    pre_order_helper(node->right.get(), visit);
  }
}

//===--------------------------- HEIGHT/VALIDATION -----------------------------===//

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::height_helper(const Node* node) const -> int {
  if (!node) {
    return -1;
  }
  return 1 + std::max(height_helper(node->left.get()), height_helper(node->right.get()));
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::black_height_helper(const Node* node) const -> int {
  if (!node) {
    return 0; // NIL is black.
  }
  int left_bh = black_height_helper(node->left.get());
  return left_bh + (node->color == Color::Black ? 1 : 0);
}

template <OrderedTreeElement T>
auto Red_Black_Tree<T>::validate_helper(const Node* node) const -> int {
  if (!node) {
    return 0; // NIL leaves are black.
  }

  // Property 4: If node is red, children must be black.
  if (node->color == Color::Red) {
    if (get_color(node->left.get()) == Color::Red || get_color(node->right.get()) == Color::Red) {
      return -1; // Violation: red node has red child.
    }
  }

  // Property 5: Black heights must be equal for all paths.
  int left_bh  = validate_helper(node->left.get());
  int right_bh = validate_helper(node->right.get());

  if (left_bh == -1 || right_bh == -1 || left_bh != right_bh) {
    return -1; // Violation: unequal black heights.
  }

  // Return black height of this subtree.
  return left_bh + (node->color == Color::Black ? 1 : 0);
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
