//===---------------------------------------------------------------------------===//
/**
 * @file main_Sqrt_Decomposition.cc
 * @author Costantino Lombardi
 * @brief Demonstrates acted-monoid square-root decomposition operations.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the Sqrt Decomposition with lazy range updates: it
 * models per-shelf inventory counts, applies bulk restocks over contiguous shelf
 * ranges in O(sqrt(n)), and answers range-sum stock queries in O(sqrt(n)).
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/range/Sqrt_Decomposition.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>

using std::cerr;
using std::cout;
using std::exception;

using ads::arrays::DynamicArray;
using ads::range::SqrtDecomposition;

namespace {

using Inventory = SqrtDecomposition<long>;

// Initial per-shelf counts, dogfooding DynamicArray as the source sequence.
auto initial_counts() -> DynamicArray<long> {
  DynamicArray<long> data;
  for (long c : {4, 7, 2, 9, 6, 1, 8, 3, 5, 10, 2, 7, 4, 6, 3, 8}) {
    data.push_back(c);
  }
  return data;
}

auto print_section_totals(const Inventory& inv) -> void {
  cout << "  stock [0, 7]  = " << inv.range_query(0, 7) << "\n";
  cout << "  stock [8, 15] = " << inv.range_query(8, 15) << "\n";
  cout << "  stock [0, 15] = " << inv.range_query(0, 15) << "\n";
}

} // namespace

//===----- INITIAL STATE -------------------------------------------------------===//

auto demo_initial() -> void {
  ads::demo::print_section("Sqrt Decomposition - Initial Stock");

  const DynamicArray<long> counts = initial_counts();
  Inventory                inv(counts.begin(), counts.end());

  cout << "shelves = " << inv.size() << ", block size = " << inv.block_size() << "\n";
  print_section_totals(inv);
  ads::demo::print_success("A range-sum query folds partial shelves and whole-block totals.");
}

//===----- BULK RESTOCK (RANGE UPDATE) -----------------------------------------===//

auto demo_restock() -> void {
  ads::demo::print_section("Sqrt Decomposition - Bulk Restock");

  const DynamicArray<long> counts = initial_counts();
  Inventory                inv(counts.begin(), counts.end());

  cout << "restock +5 on shelves [2, 11]\n";
  inv.range_apply(2, 11, 5);
  cout << "restock +10 on shelves [0, 15]\n";
  inv.range_apply(0, 15, 10);
  cout << "correction -3 on shelf 4\n";
  inv.apply(4, -3);

  print_section_totals(inv);
  cout << "  shelf 4 count = " << inv.value_at(4) << "\n";
  ads::demo::print_success("Interior blocks take the delta lazily; only partial blocks are touched.");
}

//===----- MATERIALIZED VIEW ---------------------------------------------------===//

auto demo_materialize() -> void {
  ads::demo::print_section("Sqrt Decomposition - Materialized Counts");

  const DynamicArray<long> counts = initial_counts();
  Inventory                inv(counts.begin(), counts.end());
  inv.range_apply(0, 15, 100);
  inv.range_apply(4, 9, 5);

  cout << "all shelf counts: ";
  for (const long& c : inv.to_vector()) {
    cout << c << ' ';
  }
  cout << "\n";
  ads::demo::print_success("to_vector applies every pending block tag to produce true values.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("SQRT DECOMPOSITION - COMPREHENSIVE DEMO");
    demo_initial();
    demo_restock();
    demo_materialize();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
