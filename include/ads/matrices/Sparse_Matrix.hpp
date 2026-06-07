//===---------------------------------------------------------------------------===//
/**
 * @file Sparse_Matrix.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the SparseMatrix CSR container.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include "../algorithms/Sorting.hpp"
#include "../arrays/Dynamic_Array.hpp"
#include "Matrix_Concepts.hpp"
#include "Sparse_Matrix_Exception.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace ads::matrices {

/**
 * @brief A sparse matrix stored in compressed sparse row (CSR) form.
 *
 * @details This class stores only non-zero values and treats `Value{}` as the
 *          implicit zero element. It is intended as a compact, reusable
 *          foundation for row-oriented sparse workloads.
 *
 *          Representation:
 *          - `row_offsets_[r] .. row_offsets_[r + 1)` is the slice for row `r`.
 *          - `column_indices_[i]` stores the column of a non-zero.
 *          - `values_[i]` stores the corresponding value.
 *
 * @tparam Value Matrix value type. Must be default-constructible and comparable
 *               against `Value{}` to detect structural zeros.
 */
template <MatrixValue Value>
class SparseMatrix {
public:
  //===------------------------------ ENTRY TYPE -------------------------------===//

  using value_type        = Value;
  using size_type         = size_t;
  using dense_matrix_type = ads::arrays::DynamicArray<ads::arrays::DynamicArray<Value>>;
  using position_result   = std::pair<size_type, bool>;

  /**
   * @brief One non-zero entry of the matrix.
   */
  struct Entry {
    size_type row;
    size_type column;
    Value     value;

    auto operator==(const Entry& other) const -> bool = default;
  };

  using entry_list_type = ads::arrays::DynamicArray<Entry>;

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty sparse matrix with a fixed shape.
   * @param row_count Number of rows.
   * @param column_count Number of columns.
   * @complexity Time O(rows), Space O(rows)
   */
  explicit SparseMatrix(size_type row_count = 0, size_type column_count = 0);

  /**
   * @brief Constructs a sparse matrix from an initializer list of triplets.
   * @param row_count Number of rows.
   * @param column_count Number of columns.
   * @param entries Non-zero triplets.
   * @throws SparseMatrixException if an entry is out of bounds or duplicated.
   * @complexity Time O(k log k + rows), Space O(k + rows)
   */
  SparseMatrix(size_type row_count, size_type column_count, std::initializer_list<Entry> entries) requires CopyMatrixValue<Value>;

  /**
   * @brief Constructs a sparse matrix from an input range of triplets.
   * @tparam InputIt Input iterator type.
   * @param row_count Number of rows.
   * @param column_count Number of columns.
   * @param first Iterator to the first entry.
   * @param last Iterator past the last entry.
   * @throws SparseMatrixException if an entry is out of bounds or duplicated.
   * @complexity Time O(k log k + rows), Space O(k + rows)
   */
  template <std::input_iterator InputIt>
  SparseMatrix(size_type row_count, size_type column_count, InputIt first, InputIt last) requires MatrixEntryRange<InputIt, Entry>;

  /**
   * @brief Move constructor.
   * @param other Matrix to move from.
   * @complexity Time O(1), Space O(1)
   */
  SparseMatrix(SparseMatrix&& other) noexcept = default;

  /**
   * @brief Move assignment operator.
   * @param other Matrix to move from.
   * @return Reference to this matrix.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(SparseMatrix&& other) noexcept -> SparseMatrix& = default;

  /**
   * @brief Destructor.
   * @complexity Time O(rows + nnz), Space O(1)
   */
  ~SparseMatrix() = default;

  SparseMatrix(const SparseMatrix&)                    = delete;
  auto operator=(const SparseMatrix&) -> SparseMatrix& = delete;

  //===-------------------------- FACTORY OPERATIONS ---------------------------===//

  /**
   * @brief Builds a sparse matrix from a dense `DynamicArray` matrix.
   * @param dense Dense row-major matrix.
   * @return Sparse matrix containing every non-zero value.
   * @throws SparseMatrixException if the dense rows are not rectangular.
   * @complexity Time O(rows * cols), Space O(nnz + rows)
   */
  [[nodiscard]] static auto from_dense(const dense_matrix_type& dense) -> SparseMatrix requires CopyMatrixValue<Value>;

  /**
   * @brief Builds a sparse matrix from a dense initializer-list matrix.
   * @param dense Dense row-major matrix.
   * @return Sparse matrix containing every non-zero value.
   * @throws SparseMatrixException if the dense rows are not rectangular.
   * @complexity Time O(rows * cols), Space O(nnz + rows)
   */
  [[nodiscard]] static auto from_dense(std::initializer_list<std::initializer_list<Value>> dense) -> SparseMatrix
      requires CopyMatrixValue<Value>;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Returns the number of rows.
   * @return Row count.
   */
  [[nodiscard]] auto row_count() const noexcept -> size_type;

  /**
   * @brief Returns the number of columns.
   * @return Column count.
   */
  [[nodiscard]] auto column_count() const noexcept -> size_type;

  /**
   * @brief Returns the number of stored non-zero entries.
   * @return Non-zero count.
   */
  [[nodiscard]] auto non_zero_count() const noexcept -> size_type;

  /**
   * @brief Checks whether the matrix currently stores no non-zero entries.
   * @return true if the matrix is structurally empty.
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Checks whether the matrix is square.
   * @return true if rows == columns.
   */
  [[nodiscard]] auto is_square() const noexcept -> bool;

  //===-------------------------- MUTATION OPERATIONS --------------------------===//

  /**
   * @brief Clears all stored non-zero entries but preserves the shape.
   * @complexity Time O(rows + nnz), Space O(1)
   */
  auto clear() noexcept -> void;

  /**
   * @brief Reserves storage for at least the requested number of non-zero entries.
   * @param new_capacity Requested minimum capacity.
   * @complexity Time O(nnz) if reallocation occurs, Space O(nnz)
   */
  auto reserve_non_zero(size_type new_capacity) -> void;

  /**
   * @brief Checks whether a coordinate stores an explicit non-zero value.
   * @param row Row index.
   * @param column Column index.
   * @return true if the coordinate is stored explicitly.
   * @throws SparseMatrixException if the coordinate is out of bounds.
   */
  [[nodiscard]] auto contains(size_type row, size_type column) const -> bool;

  /**
   * @brief Returns the value at a coordinate, or the implicit zero if absent.
   * @param row Row index.
   * @param column Column index.
   * @return Stored value or `Value{}`.
   * @throws SparseMatrixException if the coordinate is out of bounds.
   */
  [[nodiscard]] auto value_at(size_type row, size_type column) const -> Value;

  /**
   * @brief Returns a const reference to a stored non-zero value.
   * @param row Row index.
   * @param column Column index.
   * @return Const reference to the stored non-zero value.
   * @throws SparseMatrixException if the coordinate is out of bounds or structurally zero.
   */
  [[nodiscard]] auto at(size_type row, size_type column) const -> const Value&;

  /**
   * @brief Inserts or updates one coordinate.
   * @param row Row index.
   * @param column Column index.
   * @param value New value. `Value{}` removes the entry.
   * @throws SparseMatrixException if the coordinate is out of bounds.
   * @complexity Time O(nnz_row + rows), Space O(1) amortized.
   */
  auto set(size_type row, size_type column, const Value& value) -> void;

  /**
   * @brief Inserts or updates one coordinate using move semantics.
   * @param row Row index.
   * @param column Column index.
   * @param value New value. `Value{}` removes the entry.
   * @throws SparseMatrixException if the coordinate is out of bounds.
   * @complexity Time O(nnz_row + rows), Space O(1) amortized.
   */
  auto set(size_type row, size_type column, Value&& value) -> void;

  /**
   * @brief Removes a stored non-zero coordinate if present.
   * @param row Row index.
   * @param column Column index.
   * @return true if an entry was removed, false if the coordinate was already zero.
   * @throws SparseMatrixException if the coordinate is out of bounds.
   * @complexity Time O(nnz_row + rows), Space O(1)
   */
  auto erase(size_type row, size_type column) -> bool;

  //===---------------------------- EXTRACTION API -----------------------------===//

  /**
   * @brief Returns the number of non-zero entries in one row.
   * @param row Row index.
   * @return Row non-zero count.
   * @throws SparseMatrixException if row is out of bounds.
   */
  [[nodiscard]] auto row_non_zero_count(size_type row) const -> size_type;

  /**
   * @brief Materializes one row as a collection of non-zero triplets.
   * @param row Row index.
   * @return Entries of the requested row ordered by column.
   * @throws SparseMatrixException if row is out of bounds.
   * @complexity Time O(nnz_row), Space O(nnz_row)
   */
  [[nodiscard]] auto entries_in_row(size_type row) const -> entry_list_type requires CopyMatrixValue<Value>;

  /**
   * @brief Materializes all stored entries as triplets ordered by row and column.
   * @return All non-zero entries.
   * @complexity Time O(nnz), Space O(nnz)
   */
  [[nodiscard]] auto entries() const -> entry_list_type requires CopyMatrixValue<Value>;

  /**
   * @brief Reconstructs a dense row-major matrix.
   * @return Dense matrix containing explicit zeros where needed.
   * @complexity Time O(rows * cols + nnz), Space O(rows * cols)
   */
  [[nodiscard]] auto to_dense() const -> dense_matrix_type requires CopyMatrixValue<Value>;

  /**
   * @brief Visits all non-zero entries of one row in column order.
   * @tparam Visitor Callable invocable as `visitor(size_type column, const Value& value)`.
   * @param row Row index.
   * @param visitor Callback executed for each stored row entry.
   * @throws SparseMatrixException if row is out of bounds.
   */
  template <typename Visitor>
  auto for_each_in_row(size_type row, Visitor&& visitor) const -> void;

  /**
   * @brief Visits all non-zero entries in row-major order.
   * @tparam Visitor Callable invocable as `visitor(size_type row, size_type column, const Value& value)`.
   * @param visitor Callback executed for each stored entry.
   */
  template <typename Visitor>
  auto for_each_non_zero(Visitor&& visitor) const -> void;

private:
  //===--------------------------- INTERNAL STORAGE ----------------------------===//

  size_type                            row_count_;
  size_type                            column_count_;
  ads::arrays::DynamicArray<size_type> row_offsets_;
  ads::arrays::DynamicArray<size_type> column_indices_;
  ads::arrays::DynamicArray<Value>     values_;

  //===------------------------------ HELPER API -------------------------------===//

  [[nodiscard]] static auto is_zero_value(const Value& value) -> bool;
  [[nodiscard]] static auto entry_less(const Entry& lhs, const Entry& rhs) -> bool;

  auto validate_row(size_type row) const -> void;
  auto validate_coordinate(size_type row, size_type column) const -> void;
  auto set_empty_shape(size_type row_count, size_type column_count) -> void;
  auto increment_row_offsets_after(size_type row) -> void;
  auto decrement_row_offsets_after(size_type row) -> void;
  auto locate_position_in_row(size_type row, size_type column) const -> position_result;

  template <std::input_iterator InputIt>
  auto rebuild_from_entries(size_type row_count, size_type column_count, InputIt first, InputIt last) -> void
      requires MatrixEntryRange<InputIt, Entry>;
};

} // namespace ads::matrices

#include "../../../src/ads/matrices/Sparse_Matrix.tpp"

#endif // SPARSE_MATRIX_HPP

//===---------------------------------------------------------------------------===//
