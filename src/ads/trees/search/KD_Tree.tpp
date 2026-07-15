//===---------------------------------------------------------------------------===//
/**
 * @file KD_Tree.tpp
 * @author Costantino Lombardi
 * @brief Implementation of template methods for KDTree.
 * @version 0.1
 * @date 2026-07-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../../include/ads/trees/search/KD_Tree.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

namespace ads::trees {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
KDTree<Coordinate, Dimensions>::KDTree() noexcept : root_(nullptr), size_(0) {
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
KDTree<Coordinate, Dimensions>::KDTree(const DynamicArray<point_type>& points) : root_(nullptr), size_(0) {
  DynamicArray<point_type> scratch(points.begin(), points.end());
  for (const point_type& point : scratch) {
    validate_point(point);
  }
  try {
    build_balanced(scratch);
  } catch (...) {
    clear();
    throw;
  }
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
template <std::input_iterator InputIt>
requires std::constructible_from<typename KDTree<Coordinate, Dimensions>::point_type, std::iter_reference_t<InputIt>>
KDTree<Coordinate, Dimensions>::KDTree(InputIt first, InputIt last) : root_(nullptr), size_(0) {
  DynamicArray<point_type> scratch;
  for (; first != last; ++first) {
    point_type point(*first);
    validate_point(point);
    scratch.push_back(std::move(point));
  }
  try {
    build_balanced(scratch);
  } catch (...) {
    clear();
    throw;
  }
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
KDTree<Coordinate, Dimensions>::KDTree(KDTree&& other) noexcept : root_(std::move(other.root_)), size_(other.size_) {
  other.size_ = 0;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
KDTree<Coordinate, Dimensions>::~KDTree() {
  clear();
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::operator=(KDTree&& other) noexcept -> KDTree& {
  if (this != &other) {
    clear();
    root_       = std::move(other.root_);
    size_       = other.size_;
    other.size_ = 0;
  }
  return *this;
}

//===----- MUTATION OPERATIONS -------------------------------------------------===//

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::insert(const point_type& point) -> void {
  validate_point(point);
  if (!root_) {
    root_ = std::make_unique<Node>(point);
    size_ = 1;
    return;
  }

  Node*     node  = root_.get();
  size_type depth = 0;
  while (true) {
    const size_type axis = depth % Dimensions;
    if (point_less(point, node->point, axis)) {
      if (node->left) {
        node = node->left.get();
      } else {
        node->left = std::make_unique<Node>(point);
        ++size_;
        return;
      }
    } else {
      if (node->right) {
        node = node->right.get();
      } else {
        node->right = std::make_unique<Node>(point);
        ++size_;
        return;
      }
    }
    ++depth;
  }
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::clear() noexcept -> void {
  // Iteratively dismantle the tree to avoid deep recursive destruction paths: an
  // unbalanced insertion order can grow the height to size_.
  while (root_) {
    if (root_->left) {
      auto old_root  = std::move(root_);
      root_          = std::move(old_root->left);
      old_root->left = std::move(root_->right);
      root_->right   = std::move(old_root);
    } else {
      auto next = std::move(root_->right);
      root_.reset();
      root_ = std::move(next);
    }
  }
  size_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::is_empty() const noexcept -> bool {
  return root_ == nullptr;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::size() const noexcept -> size_type {
  return size_;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::height() const -> int {
  // Iterative depth-first walk so an unbalanced tree cannot overflow the call stack.
  if (!root_) {
    return -1;
  }

  int                       best = 0;
  DynamicArray<const Node*> nodes;
  DynamicArray<int>         depths;
  nodes.push_back(root_.get());
  depths.push_back(0);

  while (!nodes.is_empty()) {
    const Node* node  = nodes.back();
    const int   depth = depths.back();
    nodes.pop_back();
    depths.pop_back();

    if (depth > best) {
      best = depth;
    }
    if (node->left) {
      nodes.push_back(node->left.get());
      depths.push_back(depth + 1);
    }
    if (node->right) {
      nodes.push_back(node->right.get());
      depths.push_back(depth + 1);
    }
  }
  return best;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::contains(const point_type& point) const -> bool {
  validate_point(point);
  const Node* node  = root_.get();
  size_type   depth = 0;
  while (node) {
    if (points_equal(node->point, point)) {
      return true;
    }
    const size_type axis = depth % Dimensions;

    node = point_less(point, node->point, axis) ? node->left.get() : node->right.get();
    ++depth;
  }
  return false;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::nearest(const point_type& query) const -> point_type {
  if (!root_) {
    throw EmptyTreeException("Cannot query nearest in empty k-d tree");
  }
  validate_point(query);

  struct Frame {
    const Node*   node;
    size_type     depth;
    distance_type bound; // lower bound on any distance in this branch (-inf = always visit)
  };

  constexpr distance_type never_prune = -std::numeric_limits<distance_type>::infinity();

  point_type    best      = root_->point;
  distance_type best_dist = std::numeric_limits<distance_type>::infinity();

  DynamicArray<Frame> stack;
  stack.push_back(Frame{root_.get(), 0, never_prune});

  while (!stack.is_empty()) {
    const Frame frame = stack.back();
    stack.pop_back();

    if (frame.node == nullptr || frame.bound > best_dist) {
      continue; // empty child, or this branch cannot beat the current best
    }

    const distance_type dist = distance_squared(query, frame.node->point);
    if (dist < best_dist) {
      best_dist = dist;
      best      = frame.node->point;
    }

    const size_type     axis        = frame.depth % Dimensions;
    const distance_type plane_bound = axis_distance_squared(query[axis], frame.node->point[axis]);

    const bool  query_is_less = point_less(query, frame.node->point, axis);
    const Node* near          = query_is_less ? frame.node->left.get() : frame.node->right.get();
    const Node* far           = query_is_less ? frame.node->right.get() : frame.node->left.get();

    // Push far first so it is revisited only after the near subtree has refined
    // best_dist; its bound is re-checked against that refined value on pop.
    stack.push_back(Frame{far, frame.depth + 1, plane_bound});
    stack.push_back(Frame{near, frame.depth + 1, never_prune});
  }
  return best;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::k_nearest(const point_type& query, size_type k) const -> DynamicArray<point_type> {
  validate_point(query);
  DynamicArray<point_type> result;
  if (k == 0 || !root_) {
    return result;
  }

  struct Frame {
    const Node*   node;
    size_type     depth;
    distance_type bound;
  };

  constexpr distance_type never_prune = -std::numeric_limits<distance_type>::infinity();
  const auto by_distance = [](const Neighbor& lhs, const Neighbor& rhs) noexcept -> auto { return lhs.distance < rhs.distance; };

  DynamicArray<Neighbor> heap; // max-heap on distance: heap.front() is the current worst kept
  DynamicArray<Frame>    stack;
  stack.push_back(Frame{root_.get(), 0, never_prune});

  while (!stack.is_empty()) {
    const Frame frame = stack.back();
    stack.pop_back();
    if (frame.node == nullptr) {
      continue;
    }

    const distance_type worst = heap.size() < k ? std::numeric_limits<distance_type>::infinity() : heap.front().distance;
    if (frame.bound > worst) {
      continue; // this branch cannot hold a point among the k closest
    }

    const distance_type dist = distance_squared(query, frame.node->point);
    if (heap.size() < k) {
      heap.push_back(Neighbor{dist, frame.node->point});
      std::push_heap(heap.begin(), heap.end(), by_distance);
    } else if (dist < heap.front().distance) {
      std::pop_heap(heap.begin(), heap.end(), by_distance);
      heap.back() = Neighbor{dist, frame.node->point};
      std::push_heap(heap.begin(), heap.end(), by_distance);
    }

    const size_type     axis        = frame.depth % Dimensions;
    const distance_type plane_bound = axis_distance_squared(query[axis], frame.node->point[axis]);

    const bool query_is_less = point_less(query, frame.node->point, axis);

    const Node* near = query_is_less ? frame.node->left.get() : frame.node->right.get();
    const Node* far  = query_is_less ? frame.node->right.get() : frame.node->left.get();

    stack.push_back(Frame{far, frame.depth + 1, plane_bound});
    stack.push_back(Frame{near, frame.depth + 1, never_prune});
  }

  std::sort(heap.begin(), heap.end(), by_distance);
  result.reserve(heap.size());
  for (size_type i = 0; i < heap.size(); ++i) {
    result.push_back(heap[i].point);
  }
  return result;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::range_search(const point_type& lower, const point_type& upper) const -> DynamicArray<point_type> {
  validate_point(lower);
  validate_point(upper);
  for (size_type axis = 0; axis < Dimensions; ++axis) {
    if (lower[axis] > upper[axis]) {
      throw BinaryTreeException("KDTree range box has lower > upper on some axis");
    }
  }

  DynamicArray<point_type> result;
  if (!root_) {
    return result;
  }

  struct Frame {
    const Node* node;
    size_type   depth;
  };

  DynamicArray<Frame> stack;
  stack.push_back(Frame{root_.get(), 0});
  while (!stack.is_empty()) {
    const Frame frame = stack.back();
    stack.pop_back();
    const Node* node = frame.node;

    if (in_box(node->point, lower, upper)) {
      result.push_back(node->point);
    }

    const size_type axis = frame.depth % Dimensions;
    // Descend left only if some point below the split plane can still reach lower;
    // descend right only if some point at/above it can still reach upper.
    if (node->left && lower[axis] <= node->point[axis]) {
      stack.push_back(Frame{node->left.get(), frame.depth + 1});
    }
    if (node->right && node->point[axis] <= upper[axis]) {
      stack.push_back(Frame{node->right.get(), frame.depth + 1});
    }
  }
  return result;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::for_each(const visitor_type& visit) const -> void {
  if (!root_) {
    return;
  }

  DynamicArray<const Node*> stack;
  stack.push_back(root_.get());
  while (!stack.is_empty()) {
    const Node* node = stack.back();
    stack.pop_back();
    visit(node->point);
    if (node->right) {
      stack.push_back(node->right.get());
    }
    if (node->left) {
      stack.push_back(node->left.get());
    }
  }
}

//===----- PRIVATE HELPERS -----------------------------------------------------===//

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::distance_squared(const point_type& lhs, const point_type& rhs) -> distance_type {
  distance_type sum = 0.0L;
  for (size_type axis = 0; axis < Dimensions; ++axis) {
    const distance_type squared = axis_distance_squared(lhs[axis], rhs[axis]);
    if (squared > std::numeric_limits<distance_type>::max() - sum) {
      throw BinaryTreeException("KDTree squared distance is not representable");
    }
    sum += squared;
  }
  return sum;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::axis_distance_squared(const coordinate_type& lhs, const coordinate_type& rhs) -> distance_type {
  distance_type magnitude;
  if constexpr (std::integral<coordinate_type>) {
    using unsigned_type     = std::make_unsigned_t<coordinate_type>;
    const auto unsigned_lhs = static_cast<unsigned_type>(lhs);
    const auto unsigned_rhs = static_cast<unsigned_type>(rhs);
    const auto difference   = lhs < rhs ? unsigned_rhs - unsigned_lhs : unsigned_lhs - unsigned_rhs;
    magnitude               = static_cast<distance_type>(difference);
  } else {
    magnitude = std::fabs(static_cast<distance_type>(lhs) - static_cast<distance_type>(rhs));
  }

  const distance_type squared = magnitude * magnitude;
  if (!std::isfinite(squared)) {
    throw BinaryTreeException("KDTree squared distance is not representable");
  }
  return squared;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::points_equal(const point_type& lhs, const point_type& rhs) noexcept -> bool {
  for (size_type axis = 0; axis < Dimensions; ++axis) {
    if (!(lhs[axis] == rhs[axis])) {
      return false;
    }
  }
  return true;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::point_less(const point_type& lhs, const point_type& rhs, size_type axis) noexcept -> bool {
  for (size_type offset = 0; offset < Dimensions; ++offset) {
    const size_type current_axis = (axis + offset) % Dimensions;
    if (lhs[current_axis] < rhs[current_axis]) {
      return true;
    }
    if (rhs[current_axis] < lhs[current_axis]) {
      return false;
    }
  }
  return false;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::validate_point(const point_type& point) -> void {
  if constexpr (std::floating_point<coordinate_type>) {
    for (const coordinate_type coordinate : point) {
      if (!std::isfinite(coordinate)) {
        throw BinaryTreeException("KDTree coordinates must be finite");
      }
    }
  }
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::in_box(const point_type& point, const point_type& lower, const point_type& upper) noexcept -> bool {
  for (size_type axis = 0; axis < Dimensions; ++axis) {
    if (point[axis] < lower[axis] || upper[axis] < point[axis]) {
      return false;
    }
  }
  return true;
}

template <KDCoordinate Coordinate, std::size_t Dimensions>
requires(Dimensions > 0)
auto KDTree<Coordinate, Dimensions>::build_balanced(DynamicArray<point_type>& scratch) -> void {
  if (scratch.is_empty()) {
    return;
  }

  struct Work {
    std::unique_ptr<Node>* slot; // where the built subtree root is stored

    size_type lo; // scratch range [lo, hi)
    size_type hi;
    size_type depth;
  };

  DynamicArray<Work> stack;
  stack.push_back(Work{&root_, 0, scratch.size(), 0});

  while (!stack.is_empty()) {
    const Work work = stack.back();
    stack.pop_back();
    if (work.lo >= work.hi) {
      continue;
    }

    const size_type axis  = work.depth % Dimensions;
    const size_type mid   = work.lo + (work.hi - work.lo) / 2;
    const auto      first = scratch.begin();
    std::nth_element(first + work.lo, first + mid, first + work.hi, [axis](const point_type& lhs, const point_type& rhs) -> auto {
      return point_less(lhs, rhs, axis);
    });

    *work.slot  = std::make_unique<Node>(scratch[mid]);
    Node* built = work.slot->get();
    ++size_;

    stack.push_back(Work{&built->left, work.lo, mid, work.depth + 1});
    stack.push_back(Work{&built->right, mid + 1, work.hi, work.depth + 1});
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
