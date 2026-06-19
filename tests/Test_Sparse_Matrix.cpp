//===---------------------------------------------------------------------------===//
/**
 * @file Test_Sparse_Matrix.cpp
 * @brief Google Test unit tests for the SparseMatrix CSR container.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/matrices/Sparse_Matrix.hpp"

#include <gtest/gtest.h>

#include <string>
#include <tuple>
#include <vector>

using namespace ads::matrices;

namespace {

//===----- TEST UTILITIES ------------------------------------------------------===//

auto make_dense(std::initializer_list<std::initializer_list<int>> rows) -> ads::arrays::DynamicArray<ads::arrays::DynamicArray<int>> {
  ads::arrays::DynamicArray<ads::arrays::DynamicArray<int>> dense;
  dense.reserve(rows.size());
  for (const auto& row : rows) {
    dense.push_back(ads::arrays::DynamicArray<int>(row.begin(), row.end()));
  }
  return dense;
}

} // namespace

//===----- CONSTRUCTION TESTS --------------------------------------------------===//

TEST(SparseMatrixTest, EmptyMatrixStartsWithZeroShapeAndNoEntries) {
  SparseMatrix<int> matrix;

  EXPECT_EQ(matrix.row_count(), 0U);
  EXPECT_EQ(matrix.column_count(), 0U);
  EXPECT_EQ(matrix.non_zero_count(), 0U);
  EXPECT_TRUE(matrix.is_empty());
  EXPECT_TRUE(matrix.is_square());
}

TEST(SparseMatrixConstructionTest, ShapeConstructorStoresDimensions) {
  SparseMatrix<int> matrix(3, 5);

  EXPECT_EQ(matrix.row_count(), 3U);
  EXPECT_EQ(matrix.column_count(), 5U);
  EXPECT_FALSE(matrix.is_square());
  EXPECT_EQ(matrix.non_zero_count(), 0U);
}

TEST(SparseMatrixConstructionTest, BuildsFromTripletsInRowMajorOrder) {
  SparseMatrix<int> matrix(
      4, 5,
      {
          {2, 4, 9},
          {0, 1, 7},
          {2, 1, 3},
          {3, 0, 5},
      });

  EXPECT_EQ(matrix.non_zero_count(), 4U);
  EXPECT_EQ(matrix.value_at(0, 1), 7);
  EXPECT_EQ(matrix.value_at(2, 1), 3);
  EXPECT_EQ(matrix.value_at(2, 4), 9);
  EXPECT_EQ(matrix.value_at(1, 1), 0);

  const auto entries = matrix.entries();
  ASSERT_EQ(entries.size(), 4U);
  EXPECT_EQ(entries[0], (SparseMatrix<int>::Entry{0, 1, 7}));
  EXPECT_EQ(entries[1], (SparseMatrix<int>::Entry{2, 1, 3}));
  EXPECT_EQ(entries[2], (SparseMatrix<int>::Entry{2, 4, 9}));
  EXPECT_EQ(entries[3], (SparseMatrix<int>::Entry{3, 0, 5}));
}

TEST(SparseMatrixConstructionTest, RejectsDuplicateCoordinates) {
  EXPECT_THROW(
      static_cast<void>(SparseMatrix<int>(
          2, 2,
          {
              {0, 1, 4},
              {0, 1, 9},
          })),
      SparseMatrixException);
}

TEST(SparseMatrixConstructionTest, RejectsOutOfBoundsTriplets) {
  EXPECT_THROW(
      static_cast<void>(SparseMatrix<int>(
          2, 3,
          {
              {1, 1, 7},
              {2, 0, 5},
          })),
      SparseMatrixException);
}

TEST(SparseMatrixConstructionTest, ZeroTripletsAreIgnored) {
  SparseMatrix<int> matrix(
      3, 3,
      {
          {0, 1, 0},
          {1, 2, 4},
      });

  EXPECT_EQ(matrix.non_zero_count(), 1U);
  EXPECT_FALSE(matrix.contains(0, 1));
  EXPECT_TRUE(matrix.contains(1, 2));
}

//===----- DENSE CONVERSION TESTS ----------------------------------------------===//

TEST(SparseMatrixDenseTest, BuildsFromDenseDynamicArray) {
  const auto dense = make_dense({
      {0, 5, 0},
      {2, 0, 0},
      {0, 0, 9},
  });

  const auto matrix = SparseMatrix<int>::from_dense(dense);

  EXPECT_EQ(matrix.row_count(), 3U);
  EXPECT_EQ(matrix.column_count(), 3U);
  EXPECT_EQ(matrix.non_zero_count(), 3U);
  EXPECT_EQ(matrix.value_at(0, 1), 5);
  EXPECT_EQ(matrix.value_at(1, 0), 2);
  EXPECT_EQ(matrix.value_at(2, 2), 9);
}

TEST(SparseMatrixDenseTest, RejectsNonRectangularDenseMatrix) {
  ads::arrays::DynamicArray<ads::arrays::DynamicArray<int>> dense;
  dense.push_back(ads::arrays::DynamicArray<int>({1, 0}));
  dense.push_back(ads::arrays::DynamicArray<int>({0}));

  EXPECT_THROW(static_cast<void>(SparseMatrix<int>::from_dense(dense)), SparseMatrixException);
}

//===----- ACCESS TESTS --------------------------------------------------------===//

TEST(SparseMatrixAccessTest, AtThrowsForStructuralZeros) {
  SparseMatrix<int> matrix(2, 3, {{0, 2, 8}});

  EXPECT_EQ(matrix.at(0, 2), 8);
  EXPECT_THROW(static_cast<void>(matrix.at(1, 1)), SparseMatrixException);
}

TEST(SparseMatrixAccessTest, InvalidCoordinatesThrow) {
  SparseMatrix<int> matrix(2, 3);

  EXPECT_THROW(static_cast<void>(matrix.value_at(2, 0)), SparseMatrixException);
  EXPECT_THROW(static_cast<void>(matrix.contains(0, 3)), SparseMatrixException);
  EXPECT_THROW(matrix.set(3, 0, 1), SparseMatrixException);
  EXPECT_THROW(matrix.erase(0, 7), SparseMatrixException);
}

//===----- MUTATION TESTS ------------------------------------------------------===//

TEST(SparseMatrixMutationTest, SetInsertsUpdatesAndErasesEntries) {
  SparseMatrix<int> matrix(3, 4);

  matrix.set(0, 1, 7);
  matrix.set(2, 3, 9);
  matrix.set(0, 1, 5);

  EXPECT_EQ(matrix.non_zero_count(), 2U);
  EXPECT_EQ(matrix.value_at(0, 1), 5);
  EXPECT_EQ(matrix.value_at(2, 3), 9);

  EXPECT_TRUE(matrix.erase(0, 1));
  EXPECT_FALSE(matrix.erase(0, 1));
  EXPECT_EQ(matrix.value_at(0, 1), 0);
  EXPECT_EQ(matrix.non_zero_count(), 1U);

  matrix.set(2, 3, 0);
  EXPECT_TRUE(matrix.is_empty());
}

TEST(SparseMatrixMutationTest, RowIterationRemainsColumnSortedAfterMutations) {
  SparseMatrix<int> matrix(2, 6);
  matrix.set(1, 4, 8);
  matrix.set(1, 1, 3);
  matrix.set(1, 3, 7);

  std::vector<std::pair<size_t, int>> visited;
  matrix.for_each_in_row(1, [&](size_t column, const int& value) { visited.emplace_back(column, value); });

  EXPECT_EQ(visited, (std::vector<std::pair<size_t, int>>{{1, 3}, {3, 7}, {4, 8}}));
}

//===----- ITERATION / ROW TESTS -----------------------------------------------===//

TEST(SparseMatrixIterationTest, EnumeratesNonZeroEntriesInRowMajorOrder) {
  SparseMatrix<int> matrix(
      3, 4,
      {
          {2, 0, 9},
          {0, 2, 5},
          {1, 1, 4},
      });

  std::vector<std::tuple<size_t, size_t, int>> visited;
  matrix.for_each_non_zero([&](size_t row, size_t column, const int& value) { visited.emplace_back(row, column, value); });

  EXPECT_EQ(
      visited, (std::vector<std::tuple<size_t, size_t, int>>{
                   {0, 2, 5},
                   {1, 1, 4},
                   {2, 0, 9},
               }));
}

TEST(SparseMatrixRowTest, EntriesInRowAndRowCountMatch) {
  SparseMatrix<int> matrix(
      4, 4,
      {
          {0, 0, 1},
          {2, 1, 5},
          {2, 3, 8},
      });

  EXPECT_EQ(matrix.row_non_zero_count(0), 1U);
  EXPECT_EQ(matrix.row_non_zero_count(1), 0U);
  EXPECT_EQ(matrix.row_non_zero_count(2), 2U);

  const auto row_entries = matrix.entries_in_row(2);
  ASSERT_EQ(row_entries.size(), 2U);
  EXPECT_EQ(row_entries[0], (SparseMatrix<int>::Entry{2, 1, 5}));
  EXPECT_EQ(row_entries[1], (SparseMatrix<int>::Entry{2, 3, 8}));
}

//===----- UTILITY TESTS -------------------------------------------------------===//

TEST(SparseMatrixDenseTest, ToDenseReconstructsExplicitZeros) {
  SparseMatrix<int> matrix(
      2, 3,
      {
          {0, 2, 6},
          {1, 0, 4},
      });

  const auto dense = matrix.to_dense();
  ASSERT_EQ(dense.size(), 2U);
  ASSERT_EQ(dense[0].size(), 3U);
  EXPECT_EQ(dense[0][0], 0);
  EXPECT_EQ(dense[0][2], 6);
  EXPECT_EQ(dense[1][0], 4);
  EXPECT_EQ(dense[1][2], 0);
}

TEST(SparseMatrixUtilityTest, ClearPreservesShapeAndRemovesEntries) {
  SparseMatrix<int> matrix(5, 6, {{1, 2, 9}, {4, 5, 7}});

  matrix.clear();

  EXPECT_EQ(matrix.row_count(), 5U);
  EXPECT_EQ(matrix.column_count(), 6U);
  EXPECT_EQ(matrix.non_zero_count(), 0U);
  EXPECT_TRUE(matrix.is_empty());
}

TEST(SparseMatrixValueTest, SupportsStringValuesWithDefaultEmptySentinel) {
  SparseMatrix<std::string> matrix(2, 2);
  matrix.set(0, 1, std::string("north"));

  EXPECT_EQ(matrix.value_at(0, 1), "north");
  EXPECT_EQ(matrix.value_at(1, 1), "");
}

//===---------------------------------------------------------------------------===//
