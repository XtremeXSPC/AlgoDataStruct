//===---------------------------------------------------------------------------===//
/**
 * @file Sqrt_Decomposition.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the acted-monoid square-root decomposition.
 * @version 1.0
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/range/Sqrt_Decomposition.hpp"

#include <algorithm>
#include <cmath>

namespace ads::range {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
SqrtDecomposition<Value, Acted>::SqrtDecomposition(Acted acted) noexcept(std::is_nothrow_move_constructible_v<Acted>) :
    acted_(std::move(acted)) {
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
SqrtDecomposition<Value, Acted>::SqrtDecomposition(const std::vector<Value>& values, Acted acted) : acted_(std::move(acted)) {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
SqrtDecomposition<Value, Acted>::SqrtDecomposition(std::vector<Value>&& values, Acted acted) : acted_(std::move(acted)) {
  rebuild(DynamicArray<Value>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
  values.clear();
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
SqrtDecomposition<Value, Acted>::SqrtDecomposition(std::initializer_list<Value> values, Acted acted) : acted_(std::move(acted)) {
  rebuild(DynamicArray<Value>(values));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
template <std::input_iterator InputIt>
SqrtDecomposition<Value, Acted>::SqrtDecomposition(InputIt first, InputIt last, Acted acted) : acted_(std::move(acted)) {
  rebuild(DynamicArray<Value>(first, last));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
SqrtDecomposition<Value, Acted>::SqrtDecomposition(SqrtDecomposition&& other) noexcept(std::is_nothrow_move_constructible_v<Acted>) :
    acted_(std::move(other.acted_)),
    data_(std::move(other.data_)),
    block_aggregate_(std::move(other.block_aggregate_)),
    block_lazy_(std::move(other.block_lazy_)),
    block_size_(other.block_size_),
    num_blocks_(other.num_blocks_),
    size_(other.size_) {
  other.block_size_ = 0;
  other.num_blocks_ = 0;
  other.size_       = 0;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::operator=(SqrtDecomposition&& other) noexcept(std::is_nothrow_move_assignable_v<Acted>)
    -> SqrtDecomposition& {
  if (this != &other) {
    acted_            = std::move(other.acted_);
    data_             = std::move(other.data_);
    block_aggregate_  = std::move(other.block_aggregate_);
    block_lazy_       = std::move(other.block_lazy_);
    block_size_       = other.block_size_;
    num_blocks_       = other.num_blocks_;
    size_             = other.size_;
    other.block_size_ = 0;
    other.num_blocks_ = 0;
    other.size_       = 0;
  }
  return *this;
}

//===----- MODIFICATION OPERATIONS ---------------------------------------------===//

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::build(const std::vector<Value>& values) -> void {
  rebuild(DynamicArray<Value>(values.begin(), values.end()));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::build(std::vector<Value>&& values) -> void {
  rebuild(DynamicArray<Value>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
  values.clear();
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::build(std::initializer_list<Value> values) -> void {
  rebuild(DynamicArray<Value>(values));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
template <std::input_iterator InputIt>
auto SqrtDecomposition<Value, Acted>::build(InputIt first, InputIt last) -> void
    requires std::constructible_from<Value, std::iter_reference_t<InputIt>>
{
  rebuild(DynamicArray<Value>(first, last));
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::clear() noexcept -> void {
  data_.clear();
  block_aggregate_.clear();
  block_lazy_.clear();
  block_size_ = 0;
  num_blocks_ = 0;
  size_       = 0;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::set(size_type index, const Value& value) -> void {
  validate_index(index);

  DynamicArray<PartialBlockUpdate> partial_updates;
  DynamicArray<LazyBlockUpdate>    lazy_updates;
  partial_updates.reserve(1);
  partial_updates.push_back(make_set_update(index, value));
  commit_updates(partial_updates, lazy_updates);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::apply(size_type index, const tag_type& action) -> void {
  validate_index(index);
  range_apply(index, index, action);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::range_apply(size_type left, size_type right, const tag_type& action) -> void {
  validate_range(left, right);
  const size_type left_block  = block_of(left);
  const size_type right_block = block_of(right);

  DynamicArray<PartialBlockUpdate> partial_updates;
  DynamicArray<LazyBlockUpdate>    lazy_updates;
  partial_updates.reserve(left_block == right_block ? 1 : 2);
  lazy_updates.reserve(right_block - left_block);

  if (left_block == right_block) {
    partial_updates.push_back(make_partial_update(left_block, left, right, action));
  } else {
    partial_updates.push_back(make_partial_update(left_block, left, block_end(left_block) - 1, action));
    for (size_type block = left_block + 1; block < right_block; ++block) {
      std::optional<tag_type> next = block_lazy_[block].has_value() ? std::optional<tag_type>(acted_.compose(*block_lazy_[block], action))
                                                                    : std::optional<tag_type>(action);
      lazy_updates.push_back(LazyBlockUpdate{block, std::move(next)});
    }
    partial_updates.push_back(make_partial_update(right_block, block_start(right_block), right, action));
  }

  commit_updates(partial_updates, lazy_updates);
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::range_query(size_type left, size_type right) const -> Value {
  validate_range(left, right);
  const size_type left_block  = block_of(left);
  const size_type right_block = block_of(right);
  Value           result      = acted_.value_identity();

  if (left_block == right_block) {
    for (size_type index = left; index <= right; ++index) {
      result = acted_.combine(result, true_value(index));
    }
    return result;
  }

  for (size_type index = left; index < block_end(left_block); ++index) {
    result = acted_.combine(result, true_value(index));
  }
  for (size_type block = left_block + 1; block < right_block; ++block) {
    result = acted_.combine(result, true_block_aggregate(block));
  }
  for (size_type index = block_start(right_block); index <= right; ++index) {
    result = acted_.combine(result, true_value(index));
  }
  return result;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::value_at(size_type index) const -> Value {
  validate_index(index);
  return true_value(index);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::to_vector() const -> std::vector<Value> {
  std::vector<Value> result;
  result.reserve(size_);
  for (size_type index = 0; index < size_; ++index) {
    result.push_back(true_value(index));
  }
  return result;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::total() const -> Value {
  if (is_empty()) {
    return acted_.value_identity();
  }
  Value result = acted_.value_identity();
  for (size_type block = 0; block < num_blocks_; ++block) {
    result = acted_.combine(result, true_block_aggregate(block));
  }
  return result;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::size() const noexcept -> size_type {
  return size_;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::block_size() const noexcept -> size_type {
  return block_size_;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::get_acted_monoid() const noexcept -> const Acted& {
  return acted_;
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::block_end(size_type block) const noexcept -> size_type {
  const size_type start     = block_start(block);
  const size_type remaining = size_ - start;
  return start + ((block_size_ < remaining) ? block_size_ : remaining);
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::true_value(size_type index) const -> Value {
  const size_type block = block_of(index);
  return block_lazy_[block].has_value() ? acted_.apply(data_[index], *block_lazy_[block], 1) : data_[index];
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::true_block_aggregate(size_type block) const -> Value {
  return block_lazy_[block].has_value() ? acted_.apply(block_aggregate_[block], *block_lazy_[block], block_length(block))
                                        : block_aggregate_[block];
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::make_partial_update(size_type block, size_type left, size_type right, const tag_type& action) const
    -> PartialBlockUpdate {
  const size_type start = block_start(block);
  const size_type end   = block_end(block);

  DynamicArray<Value> values;
  values.reserve(end - start);
  for (size_type index = start; index < end; ++index) {
    Value value = true_value(index);
    if (left <= index && index <= right) {
      value = acted_.apply(value, action, 1);
    }
    values.push_back(std::move(value));
  }

  Value aggregate = acted_.value_identity();
  for (const Value& value : values) {
    aggregate = acted_.combine(aggregate, value);
  }
  return PartialBlockUpdate{block, std::move(values), std::move(aggregate)};
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::make_set_update(size_type index, const Value& value) const -> PartialBlockUpdate {
  const size_type block = block_of(index);
  const size_type start = block_start(block);
  const size_type end   = block_end(block);

  DynamicArray<Value> values;
  values.reserve(end - start);
  for (size_type position = start; position < end; ++position) {
    values.push_back(position == index ? Value(value) : true_value(position));
  }

  Value aggregate = acted_.value_identity();
  for (const Value& staged : values) {
    aggregate = acted_.combine(aggregate, staged);
  }
  return PartialBlockUpdate{block, std::move(values), std::move(aggregate)};
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::commit_updates(
    DynamicArray<PartialBlockUpdate>& partial, DynamicArray<LazyBlockUpdate>& lazy) noexcept -> void {
  using std::swap;
  for (PartialBlockUpdate& update : partial) {
    const size_type start = block_start(update.block);
    for (size_type offset = 0; offset < update.values.size(); ++offset) {
      swap(data_[start + offset], update.values[offset]);
    }
    swap(block_aggregate_[update.block], update.aggregate);
    block_lazy_[update.block].reset();
  }
  for (LazyBlockUpdate& update : lazy) {
    block_lazy_[update.block].swap(update.lazy);
  }
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::rebuild(DynamicArray<Value>&& values) -> void {
  const size_type new_size = values.size();

  size_type new_block_size = 0;
  size_type new_num_blocks = 0;

  DynamicArray<Value>                   new_aggregate;
  DynamicArray<std::optional<tag_type>> new_lazy;

  if (new_size != 0) {
    new_block_size = static_cast<size_type>(std::sqrt(static_cast<double>(new_size)));
    if (new_block_size == 0) {
      new_block_size = 1;
    }
    new_num_blocks = 1 + ((new_size - 1) / new_block_size);
    new_aggregate.reserve(new_num_blocks);
    new_lazy.assign(new_num_blocks, std::nullopt);

    for (size_type block = 0; block < new_num_blocks; ++block) {
      const size_type start     = block * new_block_size;
      const size_type end       = std::min(start + new_block_size, new_size);
      Value           aggregate = acted_.value_identity();
      for (size_type index = start; index < end; ++index) {
        aggregate = acted_.combine(aggregate, values[index]);
      }
      new_aggregate.push_back(std::move(aggregate));
    }
  }

  data_            = std::move(values);
  block_aggregate_ = std::move(new_aggregate);
  block_lazy_      = std::move(new_lazy);
  block_size_      = new_block_size;
  num_blocks_      = new_num_blocks;
  size_            = new_size;
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::validate_index(size_type index) const -> void {
  if (index >= size_) {
    throw RangeIndexException("SqrtDecomposition index out of range");
  }
}

template <typename Value, typename Acted>
requires RangeActionPolicy<Value, Acted>
auto SqrtDecomposition<Value, Acted>::validate_range(size_type left, size_type right) const -> void {
  if (size_ == 0 || left > right || right >= size_) {
    throw InvalidRangeException("SqrtDecomposition invalid or out-of-bounds range");
  }
}

} // namespace ads::range

//===---------------------------------------------------------------------------===//
