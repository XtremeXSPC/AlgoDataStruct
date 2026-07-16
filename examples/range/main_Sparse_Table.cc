//===---------------------------------------------------------------------------===//
/**
 * @file main_Sparse_Table.cc
 * @author Costantino Lombardi
 * @brief Demonstrates idempotent-semigroup Sparse Table queries.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the Sparse Table: a static index that answers
 * idempotent range queries (min, max, gcd) in O(1) after an O(n log n) build. It
 * queries a fixed terrain-height profile and a divisor sequence.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/range/Sparse_Table.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <utility>

using std::cerr;
using std::cout;
using std::exception;

using ads::arrays::DynamicArray;
using ads::range::SparseTable;

namespace {

// Fixed terrain profile, dogfooding DynamicArray as the source sequence.
auto heights() -> DynamicArray<int> {
  DynamicArray<int> data;
  for (int h : {4, 7, 2, 9, 6, 1, 8, 3, 5, 10, 2, 7}) {
    data.push_back(h);
  }
  return data;
}

} // namespace

//===----- RANGE MINIMUM QUERY -------------------------------------------------===//

auto demo_range_min() -> void {
  ads::demo::print_section("Sparse Table - Range Minimum Query");

  const DynamicArray<int> data = heights();
  SparseTable<int>        rmq(data.begin(), data.end());

  cout << "profile (" << rmq.size() << " points): ";
  for (const int& h : rmq) {
    cout << h << ' ';
  }
  cout << "\n";

  for (auto [l, r] : {std::pair{0, 11}, std::pair{2, 5}, std::pair{6, 9}}) {
    cout << "min over [" << l << ", " << r << "] = " << rmq.range_query(static_cast<size_t>(l), static_cast<size_t>(r)) << "\n";
  }
  ads::demo::print_success("Each query combines two overlapping power-of-two windows in O(1).");
}

//===----- RANGE MAXIMUM QUERY -------------------------------------------------===//

auto demo_range_max() -> void {
  ads::demo::print_section("Sparse Table - Range Maximum Query");

  const DynamicArray<int>                           data = heights();
  SparseTable<int, ads::algebra::MaxSemigroup<int>> rmax(data.begin(), data.end());

  for (auto [l, r] : {std::pair{0, 11}, std::pair{2, 5}, std::pair{6, 9}}) {
    cout << "max over [" << l << ", " << r << "] = " << rmax.range_query(static_cast<size_t>(l), static_cast<size_t>(r)) << "\n";
  }
  ads::demo::print_success("The same layout serves any idempotent operation via the algebra policy.");
}

//===----- RANGE GCD QUERY -----------------------------------------------------===//

auto demo_range_gcd() -> void {
  ads::demo::print_section("Sparse Table - Range GCD Query");

  DynamicArray<int> data;
  for (int v : {12, 18, 24, 6, 9, 15, 100, 40}) {
    data.push_back(v);
  }
  SparseTable<int, ads::algebra::GcdMonoid<int>> rgcd(data.begin(), data.end());

  cout << "gcd over [0, 3] = " << rgcd.range_query(0, 3) << "\n"; // gcd(12,18,24,6)
  cout << "gcd over [4, 5] = " << rgcd.range_query(4, 5) << "\n"; // gcd(9,15)
  cout << "gcd over [0, 7] = " << rgcd.range_query(0, 7) << "\n";
  ads::demo::print_success("GCD is idempotent, so the Sparse Table applies unchanged.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("SPARSE TABLE - COMPREHENSIVE DEMO");
    demo_range_min();
    demo_range_max();
    demo_range_gcd();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
