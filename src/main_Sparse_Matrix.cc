//===---------------------------------------------------------------------------===//
/**
 * @file main_Sparse_Matrix.cc
 * @author Costantino Lombardi
 * @brief Demo for the SparseMatrix CSR container.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/matrices/Sparse_Matrix.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>

using std::cout;

using namespace ads::matrices;

namespace {

//===----- SPARSE MATRIX DEMOS -------------------------------------------------===//

auto demo_triplet_build() -> void {
  ads::demo::print_section("Build from triplets");

  SparseMatrix<int> matrix(
      4, 5,
      {
          {0, 1, 7},
          {2, 3, 5},
          {3, 0, 9},
      });

  cout << "Shape: " << matrix.row_count() << " x " << matrix.column_count() << '\n';
  cout << "Non-zero entries:\n";
  matrix.for_each_non_zero(
      [&](size_t row, size_t column, const int& value) { cout << "  (" << row << ", " << column << ") = " << value << '\n'; });
}

auto demo_mutation_and_rows() -> void {
  ads::demo::print_section("Row-wise updates");

  SparseMatrix<int> matrix(3, 4);
  matrix.set(1, 0, 4);
  matrix.set(1, 3, 8);
  matrix.set(2, 2, 6);
  matrix.set(1, 0, 5);

  cout << "Row 1 entries: ";
  bool first = true;
  matrix.for_each_in_row(1, [&](size_t column, const int& value) {
    if (!first) {
      cout << ", ";
    }
    first = false;
    cout << "[" << column << "]=" << value;
  });
  cout << '\n';
}

} // namespace

//===----- MAIN FUNCTION -------------------------------------------------------===//

auto main() -> int {
  ads::demo::print_header({"SPARSE MATRIX DEMO", "Compressed Sparse Row (CSR) container"});

  demo_triplet_build();
  demo_mutation_and_rows();

  ads::demo::print_footer("SPARSE MATRIX DEMO COMPLETED!");
  return 0;
}

//===---------------------------------------------------------------------------===//
