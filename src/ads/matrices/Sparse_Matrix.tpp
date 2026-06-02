//===---------------------------------------------------------------------------===//
/**
 * @file Sparse_Matrix.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the SparseMatrix CSR container.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/matrices/Sparse_Matrix.hpp"

namespace ads::matrices {

//===------------------ CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT -------------------===//

// clang-format off
template <typename Value>
  requires std::default_initializable<Value> && std::equality_comparable<Value>
SparseMatrix<Value>::SparseMatrix(size_t row_count, size_t column_count) :
    row_count_(row_count),
    column_count_(column_count),
    row_offsets_(row_count + 1, 0U),
    column_indices_(),
    values_() {
}

template <typename Value>
  requires std::default_initializable<Value> && std::equality_comparable<Value>
SparseMatrix<Value>::SparseMatrix(size_t row_count, size_t column_count, std::initializer_list<Entry> entries)
  requires std::copy_constructible<Value>
    : SparseMatrix(row_count, column_count) {
  rebuild_from_entries(row_count, column_count, entries.begin(), entries.end());
}

template <typename Value>
  requires std::default_initializable<Value> && std::equality_comparable<Value>
template <std::input_iterator InputIt>
SparseMatrix<Value>::SparseMatrix(size_t row_count, size_t column_count, InputIt first, InputIt last)
  requires std::constructible_from<Entry, std::iter_reference_t<InputIt>>
    : SparseMatrix(row_count, column_count) {
  rebuild_from_entries(row_count, column_count, first, last);
}

template <typename Value>
  requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::from_dense(const ads::arrays::DynamicArray<ads::arrays::DynamicArray<Value>>& dense) -> SparseMatrix
  requires std::copy_constructible<Value>
{
  const size_t row_count    = dense.size();
  const size_t column_count = row_count == 0 ? 0 : dense[0].size();

  ads::arrays::DynamicArray<Entry> entries;
  for (size_t row = 0; row < row_count; ++row) {
    if (dense[row].size() != column_count) {
      throw SparseMatrixException("Dense matrix rows must all have the same length");
    }

    for (size_t column = 0; column < column_count; ++column) {
      if (!is_zero_value(dense[row][column])) {
        entries.push_back({row, column, dense[row][column]});
      }
    }
  }

  return SparseMatrix(row_count, column_count, entries.begin(), entries.end());
}

template <typename Value>
  requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::from_dense(std::initializer_list<std::initializer_list<Value>> dense) -> SparseMatrix
  requires std::copy_constructible<Value>
{
  ads::arrays::DynamicArray<ads::arrays::DynamicArray<Value>> rows;
  rows.reserve(dense.size());

  for (const auto& row : dense) {
    rows.push_back(ads::arrays::DynamicArray<Value>(row.begin(), row.end()));
  }

  return from_dense(rows);
}

// clang-format on

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::row_count() const noexcept -> size_t {
  return row_count_;
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::column_count() const noexcept -> size_t {
  return column_count_;
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::non_zero_count() const noexcept -> size_t {
  return values_.size();
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::is_empty() const noexcept -> bool {
  return values_.is_empty();
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::is_square() const noexcept -> bool {
  return row_count_ == column_count_;
}

//===--------------------------- MUTATION OPERATIONS ---------------------------===//

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::clear() noexcept -> void {
  column_indices_.clear();
  values_.clear();
  row_offsets_.assign(row_count_ + 1, 0U);
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::reserve_non_zero(size_t new_capacity) -> void {
  column_indices_.reserve(new_capacity);
  values_.reserve(new_capacity);
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::contains(size_t row, size_t column) const -> bool {
  validate_coordinate(row, column);
  return locate_position_in_row(row, column).second;
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::value_at(size_t row, size_t column) const -> Value {
  validate_coordinate(row, column);
  const auto [position, found] = locate_position_in_row(row, column);
  if (!found) {
    return Value{};
  }
  return values_[position];
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::at(size_t row, size_t column) const -> const Value& {
  validate_coordinate(row, column);
  const auto [position, found] = locate_position_in_row(row, column);
  if (!found) {
    throw SparseMatrixException("Requested coordinate is structurally zero");
  }
  return values_[position];
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::set(size_t row, size_t column, const Value& value) -> void {
  validate_coordinate(row, column);
  if (is_zero_value(value)) {
    static_cast<void>(erase(row, column));
    return;
  }

  const auto [position, found] = locate_position_in_row(row, column);
  if (found) {
    values_[position] = value;
    return;
  }

  column_indices_.insert(position, column);
  values_.insert(position, value);
  increment_row_offsets_after(row);
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::set(size_t row, size_t column, Value&& value) -> void {
  validate_coordinate(row, column);
  if (is_zero_value(value)) {
    static_cast<void>(erase(row, column));
    return;
  }

  const auto [position, found] = locate_position_in_row(row, column);
  if (found) {
    values_[position] = std::move(value);
    return;
  }

  column_indices_.insert(position, column);
  values_.insert(position, std::move(value));
  increment_row_offsets_after(row);
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::erase(size_t row, size_t column) -> bool {
  validate_coordinate(row, column);
  const auto [position, found] = locate_position_in_row(row, column);
  if (!found) {
    return false;
  }

  column_indices_.erase(position);
  values_.erase(position);
  decrement_row_offsets_after(row);
  return true;
}

//===------------------------- EXTRACTION / VISIT API --------------------------===//

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::row_non_zero_count(size_t row) const -> size_t {
  validate_row(row);
  return row_offsets_[row + 1] - row_offsets_[row];
}

template <typename Value>
requires std::default_initializable<Value>
         && std::equality_comparable<Value>
         auto SparseMatrix<Value>::entries_in_row(size_t row) const
         -> ads::arrays::DynamicArray<Entry> requires std::copy_constructible<Value>
{
  validate_row(row);

  ads::arrays::DynamicArray<Entry> row_entries;
  row_entries.reserve(row_non_zero_count(row));
  for (size_t index = row_offsets_[row]; index < row_offsets_[row + 1]; ++index) {
    row_entries.push_back({row, column_indices_[index], values_[index]});
  }
  return row_entries;
}

template <typename Value>
requires std::default_initializable<Value>
         && std::equality_comparable<Value>
         auto SparseMatrix<Value>::entries() const -> ads::arrays::DynamicArray<Entry> requires std::copy_constructible<Value>
{
  ads::arrays::DynamicArray<Entry> all_entries;
  all_entries.reserve(values_.size());

  for_each_non_zero([&](size_t row, size_t column, const Value& value) { all_entries.push_back({row, column, value}); });

  return all_entries;
}

template <typename Value>
requires std::default_initializable<Value>
         && std::equality_comparable<Value>
         auto SparseMatrix<Value>::to_dense() const
         -> ads::arrays::DynamicArray<ads::arrays::DynamicArray<Value>> requires std::copy_constructible<Value>
{
  ads::arrays::DynamicArray<ads::arrays::DynamicArray<Value>> dense;
  dense.reserve(row_count_);

  for (size_t row = 0; row < row_count_; ++row) {
    dense.push_back(ads::arrays::DynamicArray<Value>(column_count_, Value{}));
  }

  for_each_non_zero([&](size_t row, size_t column, const Value& value) { dense[row][column] = value; });

  return dense;
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
template <typename Visitor>
auto SparseMatrix<Value>::for_each_in_row(size_t row, Visitor&& visitor) const -> void {
  validate_row(row);
  for (size_t index = row_offsets_[row]; index < row_offsets_[row + 1]; ++index) {
    visitor(column_indices_[index], values_[index]);
  }
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
template <typename Visitor>
auto SparseMatrix<Value>::for_each_non_zero(Visitor&& visitor) const -> void {
  for (size_t row = 0; row < row_count_; ++row) {
    for_each_in_row(row, [&](size_t column, const Value& value) { visitor(row, column, value); });
  }
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::is_zero_value(const Value& value) -> bool {
  return value == Value{};
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::entry_less(const Entry& lhs, const Entry& rhs) -> bool {
  if (lhs.row != rhs.row) {
    return lhs.row < rhs.row;
  }
  return lhs.column < rhs.column;
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::validate_row(size_t row) const -> void {
  if (row >= row_count_) {
    throw SparseMatrixException("Sparse matrix row index out of range");
  }
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::validate_coordinate(size_t row, size_t column) const -> void {
  if (row >= row_count_ || column >= column_count_) {
    throw SparseMatrixException("Sparse matrix coordinate out of range");
  }
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::set_empty_shape(size_t row_count, size_t column_count) -> void {
  row_count_    = row_count;
  column_count_ = column_count;
  row_offsets_.assign(row_count_ + 1, 0U);
  column_indices_.clear();
  values_.clear();
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::increment_row_offsets_after(size_t row) -> void {
  for (size_t index = row + 1; index < row_offsets_.size(); ++index) {
    ++row_offsets_[index];
  }
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::decrement_row_offsets_after(size_t row) -> void {
  for (size_t index = row + 1; index < row_offsets_.size(); ++index) {
    --row_offsets_[index];
  }
}

template <typename Value>
requires std::default_initializable<Value> && std::equality_comparable<Value>
auto SparseMatrix<Value>::locate_position_in_row(size_t row, size_t column) const -> std::pair<size_t, bool> {
  const size_t row_begin = row_offsets_[row];
  const size_t row_end   = row_offsets_[row + 1];

  size_t position = row_begin;
  while (position < row_end && column_indices_[position] < column) {
    ++position;
  }

  const bool found = position < row_end && column_indices_[position] == column;
  return {position, found};
}

template <typename Value>
requires std::default_initializable<Value>
         && std::equality_comparable<Value>
         template <std::input_iterator InputIt>
         auto SparseMatrix<Value>::rebuild_from_entries(size_t row_count, size_t column_count, InputIt first, InputIt last) -> void
         requires std::constructible_from<Entry, std::iter_reference_t<InputIt>>
{
  set_empty_shape(row_count, column_count);

  ads::arrays::DynamicArray<Entry> collected_entries;
  for (; first != last; ++first) {
    Entry entry = Entry(*first);
    if (entry.row >= row_count_ || entry.column >= column_count_) {
      throw SparseMatrixException("Sparse matrix entry is out of bounds");
    }
    if (!is_zero_value(entry.value)) {
      collected_entries.push_back(std::move(entry));
    }
  }

  ads::algorithms::quick_sort(collected_entries.begin(), collected_entries.end(), entry_less);

  for (size_t index = 1; index < collected_entries.size(); ++index) {
    if (collected_entries[index - 1].row == collected_entries[index].row
        && collected_entries[index - 1].column == collected_entries[index].column) {
      throw SparseMatrixException("Sparse matrix entries contain duplicate coordinates");
    }
  }

  row_offsets_.assign(row_count_ + 1, 0U);
  column_indices_.reserve(collected_entries.size());
  values_.reserve(collected_entries.size());

  for (const auto& entry : collected_entries) {
    ++row_offsets_[entry.row + 1];
  }

  for (size_t row = 1; row < row_offsets_.size(); ++row) {
    row_offsets_[row] += row_offsets_[row - 1];
  }

  for (auto& entry : collected_entries) {
    column_indices_.push_back(entry.column);
    values_.push_back(std::move(entry.value));
  }
}

} // namespace ads::matrices

//===---------------------------------------------------------------------------===//
