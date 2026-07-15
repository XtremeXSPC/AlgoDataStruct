//===---------------------------------------------------------------------------===//
/**
 * @file Interval_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the IntervalTree class.
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../../include/ads/trees/search/Interval_Tree.hpp"

#include <cmath>

namespace ads::trees {

//===----- ITERATOR IMPLEMENTATION ---------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
IntervalTree<Endpoint, Value>::iterator::iterator(const iterator& other) :
    stack_(other.stack_.begin(), other.stack_.end()),
    current_(other.current_) {
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::iterator::operator=(const iterator& other) -> iterator& {
  if (this != &other) {
    // Assign after building replacement storage so a failed copy keeps this iterator usable.
    stack_.assign(other.stack_.begin(), other.stack_.end());
    current_ = other.current_;
  }
  return *this;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
IntervalTree<Endpoint, Value>::iterator::iterator(Node* root) : current_(nullptr) {
  push_left(root);
}

template <IntervalEndpoint Endpoint, TreeElement Value>
void IntervalTree<Endpoint, Value>::iterator::push_left(Node* node) {
  while (node) {
    stack_.push_back(node);
    node = node->left.get();
  }
  if (!stack_.is_empty()) {
    current_ = stack_.back();
  }
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::iterator::operator*() const -> reference {
  return current_->entry;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::iterator::operator->() const -> pointer {
  return &(current_->entry);
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::iterator::operator++() -> iterator& {
  if (stack_.is_empty()) {
    current_ = nullptr;
    return *this;
  }

  Node* node = stack_.back();
  stack_.pop_back();

  if (node->right) {
    push_left(node->right.get());
  }

  if (!stack_.is_empty()) {
    current_ = stack_.back();
  } else {
    current_ = nullptr;
  }

  return *this;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::iterator::operator++(int) -> iterator {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::iterator::operator==(const iterator& other) const -> bool {
  return current_ == other.current_;
}

//===----- CONSTRUCTORS AND ASSIGNMENT -----------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
IntervalTree<Endpoint, Value>::IntervalTree() noexcept : root_(nullptr), size_(0) {
}

template <IntervalEndpoint Endpoint, TreeElement Value>
IntervalTree<Endpoint, Value>::IntervalTree(const DynamicArray<Entry>& entries) requires std::copy_constructible<Value>
    : root_(nullptr), size_(0) {
  try {
    for (const Entry& entry : entries) {
      insert(entry.interval, entry.value);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <IntervalEndpoint Endpoint, TreeElement Value>
template <std::input_iterator InputIt>
requires std::constructible_from<typename IntervalTree<Endpoint, Value>::Entry, std::iter_reference_t<InputIt>>
IntervalTree<Endpoint, Value>::IntervalTree(InputIt first, InputIt last) : root_(nullptr), size_(0) {
  try {
    for (; first != last; ++first) {
      Entry entry(*first);
      insert(entry.interval, std::move(entry.value));
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <IntervalEndpoint Endpoint, TreeElement Value>
IntervalTree<Endpoint, Value>::IntervalTree(IntervalTree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) {
  other.size_ = 0;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::operator=(IntervalTree&& other) noexcept -> IntervalTree& {
  if (this != &other) {
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===----- MUTATION OPERATIONS -------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::insert(const Interval& interval, const Value& value) -> void requires std::copy_constructible<Value>
{
  validate_interval(interval);
  insert_helper(root_, interval, value);
  ++size_;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::insert(const Interval& interval, Value&& value) -> void requires std::move_constructible<Value>
{
  validate_interval(interval);
  insert_helper(root_, interval, std::move(value));
  ++size_;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::remove(const Interval& interval) -> bool {
  validate_interval(interval);
  bool removed = false;
  remove_helper(root_, interval, removed);
  if (removed) {
    --size_;
  }
  return removed;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::clear() noexcept -> void {
  root_.reset();
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::size() const noexcept -> size_type {
  return size_;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::height() const noexcept -> int {
  return get_height(root_.get());
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::contains(const Interval& interval) const -> bool {
  return find(interval) != nullptr;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::find(const Interval& interval) const -> const Entry* {
  validate_interval(interval);
  const Node* node = root_.get();
  while (node) {
    if (interval_less(interval, node->entry.interval)) {
      node = node->left.get();
    } else if (interval_less(node->entry.interval, interval)) {
      node = node->right.get();
    } else {
      return &node->entry;
    }
  }
  return nullptr;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::overlaps(const Interval& query) const -> bool {
  return find_any_overlap(query) != nullptr;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::find_any_overlap(const Interval& query) const -> const Entry* {
  validate_interval(query);
  const Node* node = root_.get();
  while (node) {
    if (intervals_overlap(node->entry.interval, query)) {
      return &node->entry;
    }
    // Descend left only if some interval there can reach up to query.low; otherwise
    // the whole left subtree ends strictly before the query begins.
    if (node->left && !(*node->left->max_end < query.low)) {
      node = node->left.get();
    } else {
      node = node->right.get();
    }
  }
  return nullptr;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::find_overlapping(const Interval& query, const visitor_type& visit) const -> void {
  validate_interval(query);
  report_overlapping(root_.get(), query, visit);
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::find_containing(const Endpoint& point, const visitor_type& visit) const -> void {
  validate_endpoint(point);
  report_overlapping(root_.get(), Interval{point, point}, visit);
}

//===----- TRAVERSAL OPERATIONS ------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::in_order_traversal(const visitor_type& visit) const -> void {
  in_order_helper(root_.get(), visit);
}

//===----- VALIDATION ----------------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::validate_properties() const -> bool {
  const Node*     prev    = nullptr;
  int             height  = 0;
  size_type       count   = 0;
  const Endpoint* max_end = nullptr;
  return validate_helper(root_.get(), prev, height, count, max_end) && count == size_;
}

//===----- ITERATOR OPERATIONS -------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::begin() -> iterator {
  return iterator(root_.get());
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::begin() const -> iterator {
  return iterator(root_.get());
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::end() -> iterator {
  return iterator();
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::end() const -> iterator {
  return iterator();
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::cbegin() const -> iterator {
  return iterator(root_.get());
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::cend() const -> iterator {
  return iterator();
}

//=================================================================================//
//===----- INTERVAL PREDICATES -------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::interval_less(const Interval& lhs, const Interval& rhs) noexcept -> bool {
  if (lhs.low < rhs.low) {
    return true;
  }
  if (rhs.low < lhs.low) {
    return false;
  }
  return lhs.high < rhs.high;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::intervals_overlap(const Interval& lhs, const Interval& rhs) noexcept -> bool {
  // Closed intervals overlap iff lhs.low <= rhs.high and rhs.low <= lhs.high.
  return !(rhs.high < lhs.low) && !(lhs.high < rhs.low);
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::validate_interval(const Interval& interval) -> void {
  validate_endpoint(interval.low);
  validate_endpoint(interval.high);
  if (interval.high < interval.low) {
    throw BinaryTreeException("Interval Tree: interval low endpoint must not exceed high endpoint");
  }
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::is_valid_endpoint(const Endpoint& endpoint) noexcept -> bool {
  if constexpr (std::floating_point<Endpoint>) {
    return !std::isnan(endpoint);
  }
  return true;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::validate_endpoint(const Endpoint& endpoint) -> void {
  if (!is_valid_endpoint(endpoint)) {
    throw BinaryTreeException("Interval Tree: endpoints must be ordered values, not NaN");
  }
}

//===----- AUGMENTATION HELPERS ------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::get_height(const Node* node) const noexcept -> int {
  return node ? node->height : 0;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::get_balance_factor(const Node* node) const noexcept -> int {
  return node ? get_height(node->left.get()) - get_height(node->right.get()) : 0;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
void IntervalTree<Endpoint, Value>::update_metrics(Node* node) noexcept {
  if (!node) {
    return;
  }
  node->height = 1 + std::max(get_height(node->left.get()), get_height(node->right.get()));

  const Endpoint* largest = &node->entry.interval.high;
  if (node->left && *largest < *node->left->max_end) {
    largest = node->left->max_end;
  }
  if (node->right && *largest < *node->right->max_end) {
    largest = node->right->max_end;
  }
  node->max_end = largest;
}

//===----- ROTATION OPERATIONS -------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::rotate_right(std::unique_ptr<Node> y) noexcept -> std::unique_ptr<Node> {
  auto x   = std::move(y->left);
  y->left  = std::move(x->right);
  x->right = std::move(y);

  // Metrics must be recomputed bottom-up, after pointers are fully rewired.
  update_metrics(x->right.get());
  update_metrics(x.get());

  return x;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::rotate_left(std::unique_ptr<Node> x) noexcept -> std::unique_ptr<Node> {
  auto y   = std::move(x->right);
  x->right = std::move(y->left);
  y->left  = std::move(x);

  // Metrics must be recomputed bottom-up, after pointers are fully rewired.
  update_metrics(y->left.get());
  update_metrics(y.get());

  return y;
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::rotate_left_right(std::unique_ptr<Node> node) noexcept -> std::unique_ptr<Node> {
  node->left = rotate_left(std::move(node->left));
  return rotate_right(std::move(node));
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::rotate_right_left(std::unique_ptr<Node> node) noexcept -> std::unique_ptr<Node> {
  node->right = rotate_right(std::move(node->right));
  return rotate_left(std::move(node));
}

//===----- BALANCING -----------------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::balance(std::unique_ptr<Node> node) noexcept -> std::unique_ptr<Node> {
  if (!node) {
    return node;
  }

  update_metrics(node.get());
  const int factor = get_balance_factor(node.get());

  // Left-Left case (LL).
  if (factor > 1 && get_balance_factor(node->left.get()) >= 0) {
    return rotate_right(std::move(node));
  }
  // Left-Right case (LR).
  if (factor > 1 && get_balance_factor(node->left.get()) < 0) {
    return rotate_left_right(std::move(node));
  }
  // Right-Right case (RR).
  if (factor < -1 && get_balance_factor(node->right.get()) <= 0) {
    return rotate_left(std::move(node));
  }
  // Right-Left case (RL).
  if (factor < -1 && get_balance_factor(node->right.get()) > 0) {
    return rotate_right_left(std::move(node));
  }

  return node;
}

//===----- MODIFICATION HELPERS ------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
template <typename V>
auto IntervalTree<Endpoint, Value>::insert_helper(std::unique_ptr<Node>& node, const Interval& interval, V&& value) -> void {
  if (!node) {
    node = std::make_unique<Node>(interval, std::forward<V>(value));
    return;
  }

  // Equal keys descend to the right, giving multiset semantics.
  if (interval_less(interval, node->entry.interval)) {
    insert_helper(node->left, interval, std::forward<V>(value));
  } else {
    insert_helper(node->right, interval, std::forward<V>(value));
  }

  node = balance(std::move(node));
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::remove_helper(std::unique_ptr<Node>& node, const Interval& interval, bool& removed) noexcept -> void {
  if (!node) {
    removed = false;
    return;
  }

  if (interval_less(interval, node->entry.interval)) {
    remove_helper(node->left, interval, removed);
  } else if (interval_less(node->entry.interval, interval)) {
    remove_helper(node->right, interval, removed);
  } else {
    // Found a node whose interval matches; remove this single occurrence.
    removed = true;

    if (!node->left) {
      node = std::move(node->right);
      return;
    }
    if (!node->right) {
      node = std::move(node->left);
      return;
    }

    // Two children: replace with the in-order successor (min of the right subtree).
    auto successor   = detach_min(node->right);
    successor->left  = std::move(node->left);
    successor->right = std::move(node->right);
    node             = std::move(successor);
  }

  node = balance(std::move(node));
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::detach_min(std::unique_ptr<Node>& node) noexcept -> std::unique_ptr<Node> {
  if (!node->left) {
    auto min_node = std::move(node);
    node          = std::move(min_node->right);
    return min_node;
  }

  auto min_node = detach_min(node->left);
  node          = balance(std::move(node));
  return min_node;
}

//===----- QUERY HELPERS -------------------------------------------------------===//

template <IntervalEndpoint Endpoint, TreeElement Value>
void IntervalTree<Endpoint, Value>::report_overlapping(const Node* node, const Interval& query, const visitor_type& visit) const {
  if (!node || *node->max_end < query.low) {
    return;
  }

  // Left subtree may hold an overlap only if it reaches up to the query's start.
  if (node->left && !(*node->left->max_end < query.low)) {
    report_overlapping(node->left.get(), query, visit);
  }

  if (intervals_overlap(node->entry.interval, query)) {
    visit(node->entry.interval, node->entry.value);
  }

  // The right subtree starts at keys >= node.low; skip it once node.low passes query.high.
  if (!(query.high < node->entry.interval.low)) {
    report_overlapping(node->right.get(), query, visit);
  }
}

template <IntervalEndpoint Endpoint, TreeElement Value>
void IntervalTree<Endpoint, Value>::in_order_helper(const Node* node, const visitor_type& visit) const {
  if (!node) {
    return;
  }
  in_order_helper(node->left.get(), visit);
  visit(node->entry.interval, node->entry.value);
  in_order_helper(node->right.get(), visit);
}

template <IntervalEndpoint Endpoint, TreeElement Value>
auto IntervalTree<Endpoint, Value>::validate_helper(
    const Node* node, const Node*& prev, int& height, size_type& count, const Endpoint*& out_max) const noexcept -> bool {
  if (!node) {
    height  = 0;
    out_max = nullptr;
    return true;
  }

  int             left_height  = 0;
  int             right_height = 0;
  const Endpoint* left_max     = nullptr;
  const Endpoint* right_max    = nullptr;

  if (!validate_helper(node->left.get(), prev, left_height, count, left_max)) {
    return false;
  }

  if (!is_valid_endpoint(node->entry.interval.low) || !is_valid_endpoint(node->entry.interval.high)
      || node->entry.interval.high < node->entry.interval.low) {
    return false;
  }

  // In-order keys must be non-decreasing (duplicates allowed).
  if (prev != nullptr && interval_less(node->entry.interval, prev->entry.interval)) {
    return false;
  }
  prev = node;
  ++count;

  if (!validate_helper(node->right.get(), prev, right_height, count, right_max)) {
    return false;
  }

  // Augmentation: max_end == max(high, left.max_end, right.max_end).
  const Endpoint* expected = &node->entry.interval.high;
  if (left_max && *expected < *left_max) {
    expected = left_max;
  }
  if (right_max && *expected < *right_max) {
    expected = right_max;
  }
  if (*node->max_end < *expected || *expected < *node->max_end) {
    return false;
  }
  out_max = expected;

  // AVL height and balance bookkeeping.
  height = 1 + std::max(left_height, right_height);
  if (node->height != height) {
    return false;
  }
  const int factor = left_height - right_height;
  return factor >= -1 && factor <= 1;
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
