//===---------------------------------------------------------------------------===//
/**
 * @file Count_Min_Sketch.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the CountMinSketch class.
 * @version 0.1
 * @date 2026-07-10
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/probabilistic/Count_Min_Sketch.hpp"

#include <algorithm>
#include <limits>

namespace ads::probabilistic {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <typename Key, typename Hash, typename Count>
CountMinSketch<Key, Hash, Count>::CountMinSketch(size_type width, size_type depth, Hash hasher) :
    width_(width),
    depth_(depth),
    total_count_(0),
    hasher_(std::move(hasher)),
    counters_(table_size(width, depth), Count{0}) {
}

template <typename Key, typename Hash, typename Count>
CountMinSketch<Key, Hash, Count>::CountMinSketch(CountMinSketch&& other) noexcept(std::is_nothrow_move_constructible_v<Hash>) :
    width_(other.width_),
    depth_(other.depth_),
    total_count_(other.total_count_),
    hasher_(std::move(other.hasher_)),
    counters_(std::move(other.counters_)) {
  other.width_       = 0;
  other.depth_       = 0;
  other.total_count_ = 0;
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::operator=(CountMinSketch&& other) noexcept(std::is_nothrow_move_assignable_v<Hash>)
    -> CountMinSketch& {
  if (this != &other) {
    width_             = other.width_;
    depth_             = other.depth_;
    total_count_       = other.total_count_;
    hasher_            = std::move(other.hasher_);
    counters_          = std::move(other.counters_);
    other.width_       = 0;
    other.depth_       = 0;
    other.total_count_ = 0;
  }
  return *this;
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::from_estimates(double epsilon, double delta, Hash hasher) -> CountMinSketch<Key, Hash, Count> {
  if (!(epsilon > 0.0 && epsilon < 1.0)) {
    throw CountMinSketchException("Count-Min Sketch epsilon must be in (0, 1)");
  }
  if (!(delta > 0.0 && delta < 1.0)) {
    throw CountMinSketchException("Count-Min Sketch delta must be in (0, 1)");
  }

  const auto width_estimate = static_cast<long double>(kEuler) / static_cast<long double>(epsilon);
  const auto depth_estimate = std::log(1.0L / static_cast<long double>(delta));
  const auto width_ceiling  = std::ceil(width_estimate);
  const auto depth_ceiling  = std::ceil(depth_estimate);
  const auto max_size       = static_cast<long double>(std::numeric_limits<size_type>::max());
  if (!std::isfinite(width_ceiling) || !std::isfinite(depth_ceiling) || width_ceiling > max_size || depth_ceiling > max_size) {
    throw CountMinSketchException("Count-Min Sketch estimates require a table that is too large");
  }

  const size_type width = std::max<size_type>(1, static_cast<size_type>(width_ceiling));
  const size_type depth = std::max<size_type>(1, static_cast<size_type>(depth_ceiling));
  return CountMinSketch(width, depth, std::move(hasher));
}

//===----- CORE OPERATIONS -----------------------------------------------------===//

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::add(const Key& key, Count count) -> void {
  if (width_ == 0 || depth_ == 0) {
    throw CountMinSketchException("Count-Min Sketch cannot add to a moved-from sketch");
  }

  const Count max_count = std::numeric_limits<Count>::max();
  if (count > max_count - total_count_) {
    throw CountMinSketchException("Count-Min Sketch total count overflow");
  }
  for (size_type row = 0; row < depth_; ++row) {
    const Count& counter = counters_[row * width_ + column_for(key, row)];
    if (count > max_count - counter) {
      throw CountMinSketchException("Count-Min Sketch counter overflow");
    }
  }

  for (size_type row = 0; row < depth_; ++row) {
    counters_[row * width_ + column_for(key, row)] += count;
  }
  total_count_ += count;
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::estimate(const Key& key) const -> Count {
  if (depth_ == 0) {
    return Count{0};
  }
  Count result = counters_[column_for(key, 0)]; // row 0 offset is zero
  for (size_type row = 1; row < depth_; ++row) {
    result = std::min(result, counters_[row * width_ + column_for(key, row)]);
  }
  return result;
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::merge(const CountMinSketch& other) -> void {
  if (width_ != other.width_ || depth_ != other.depth_ || !has_compatible_hasher(other)) {
    throw CountMinSketchException("Count-Min Sketches must have the same layout and hash family to be merged");
  }

  const Count max_count = std::numeric_limits<Count>::max();
  if (other.total_count_ > max_count - total_count_) {
    throw CountMinSketchException("Count-Min Sketch total count overflow during merge");
  }
  for (size_type index = 0; index < counters_.size(); ++index) {
    if (other.counters_[index] > max_count - counters_[index]) {
      throw CountMinSketchException("Count-Min Sketch counter overflow during merge");
    }
  }

  for (size_type index = 0; index < counters_.size(); ++index) {
    counters_[index] += other.counters_[index];
  }
  total_count_ += other.total_count_;
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::clear() noexcept -> void {
  for (size_type index = 0; index < counters_.size(); ++index) {
    counters_[index] = Count{0};
  }
  total_count_ = 0;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::width() const noexcept -> size_type {
  return width_;
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::depth() const noexcept -> size_type {
  return depth_;
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::total_count() const noexcept -> Count {
  return total_count_;
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::error_bound() const noexcept -> double {
  return width_ == 0 ? 0.0 : kEuler * static_cast<double>(total_count_) / static_cast<double>(width_);
}

//===----- PRIVATE HELPERS -----------------------------------------------------===//

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::mix_hash(std::uint64_t value) noexcept -> std::uint64_t {
  value += 0x9e37'79b9'7f4a'7c15ULL;
  value = (value ^ (value >> 30U)) * 0xbf58'476d'1ce4'e5b9ULL;
  value = (value ^ (value >> 27U)) * 0x94d0'49bb'1331'11ebULL;
  return value ^ (value >> 31U);
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::column_for(const Key& key, size_type row) const -> size_type {
  const auto seed  = static_cast<std::uint64_t>(hasher_(key));
  const auto mixed = mix_hash(seed + 0x9e37'79b9'7f4a'7c15ULL * static_cast<std::uint64_t>(row + 1));
  return static_cast<size_type>(mixed % width_);
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::table_size(size_type width, size_type depth) -> size_type {
  if (width == 0) {
    throw CountMinSketchException("Count-Min Sketch width must be greater than zero");
  }
  if (depth == 0) {
    throw CountMinSketchException("Count-Min Sketch depth must be greater than zero");
  }
  if (width > std::numeric_limits<size_type>::max() / depth) {
    throw CountMinSketchException("Count-Min Sketch table size is too large");
  }
  return width * depth;
}

template <typename Key, typename Hash, typename Count>
auto CountMinSketch<Key, Hash, Count>::has_compatible_hasher(const CountMinSketch& other) const -> bool {
  if (this == &other) {
    return true;
  }
  if constexpr (std::is_empty_v<Hash>) {
    return true;
  } else if constexpr (std::equality_comparable<Hash>) {
    return hasher_ == other.hasher_;
  } else {
    return false;
  }
}

} // namespace ads::probabilistic

//===---------------------------------------------------------------------------===//
