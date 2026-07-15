//===---------------------------------------------------------------------------===//
/**
 * @file main_KD_Tree.cc
 * @author Costantino Lombardi
 * @brief Demo for the KDTree structure.
 * @version 0.1
 * @date 2026-07-13
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the k-d tree: a balanced build from a point set and
 * the three spatial queries it accelerates -- nearest neighbour, k-nearest
 * neighbours, and orthogonal range search.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/trees/search/KD_Tree.hpp"
#include "support/Demo_Utilities.hpp"

#include <array>
#include <iostream>

using std::cerr;
using std::cout;
using std::exception;

using ads::arrays::DynamicArray;
using ads::trees::KDTree;

namespace {

using Point = std::array<int, 2>;

auto print_point(const Point& p) -> void {
  cout << '(' << p[0] << ", " << p[1] << ')';
}

auto sample_points() -> DynamicArray<Point> {
  DynamicArray<Point> points;
  for (const Point& p : {Point{2, 3}, {5, 4}, {9, 6}, {4, 7}, {8, 1}, {7, 2}, {6, 8}, {1, 5}}) {
    points.push_back(p);
  }
  return points;
}

} // namespace

//===----- BUILD & NEAREST -----------------------------------------------------===//

auto demo_nearest() -> void {
  ads::demo::print_section("k-d Tree - Nearest Neighbour");

  KDTree<int, 2> tree(sample_points());
  cout << "points loaded = " << tree.size() << ", height = " << tree.height() << "\n";

  const Point query = {6, 5};
  cout << "query = ";
  print_point(query);
  cout << " -> nearest = ";
  print_point(tree.nearest(query));
  cout << "\n";
  ads::demo::print_success("Nearest-neighbour search prunes whole branches via the split planes.");
}

//===----- K-NEAREST -----------------------------------------------------------===//

auto demo_k_nearest() -> void {
  ads::demo::print_section("k-d Tree - k Nearest Neighbours");

  KDTree<int, 2> tree(sample_points());
  const Point    query = {6, 5};

  auto neighbours = tree.k_nearest(query, 3);
  cout << "3 nearest to ";
  print_point(query);
  cout << ": ";
  for (const Point& neighbour : neighbours) {
    print_point(neighbour);
    cout << ' ';
  }
  cout << "\n";
  ads::demo::print_success("Neighbours come back sorted by ascending distance.");
}

//===----- RANGE SEARCH --------------------------------------------------------===//

auto demo_range() -> void {
  ads::demo::print_section("k-d Tree - Orthogonal Range Search");

  KDTree<int, 2> tree(sample_points());
  const Point    lower = {4, 1};
  const Point    upper = {8, 6};

  auto inside = tree.range_search(lower, upper);
  cout << "points in box [";
  print_point(lower);
  cout << " .. ";
  print_point(upper);
  cout << "]: ";
  for (const Point& point : inside) {
    print_point(point);
    cout << ' ';
  }
  cout << "\n";
  ads::demo::print_success("Range search visits only the subtrees that overlap the box.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("K-D TREE - COMPREHENSIVE DEMO");
    demo_nearest();
    demo_k_nearest();
    demo_range();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
