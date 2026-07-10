//===---------------------------------------------------------------------------===//
/**
 * @file HyperLogLog.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the HyperLogLog class.
 * @version 0.1
 * @date 2026-07-10
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/probabilistic/HyperLogLog.hpp"

#include <algorithm>
#include <bit>
#include <limits>

namespace ads::probabilistic {

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

template <typename Key, typename Hash>
HyperLogLog<Key, Hash>::HyperLogLog(size_type precision, Hash hasher) :
    precision_(validate_precision(precision)),
    register_m_(size_type{1} << precision_),
    hasher_(std::move(hasher)),
    registers_(register_m_, std::uint8_t{0}) {
}

template <typename Key, typename Hash>
HyperLogLog<Key, Hash>::HyperLogLog(HyperLogLog&& other) noexcept(std::is_nothrow_move_constructible_v<Hash>) :
    precision_(other.precision_),
    register_m_(other.register_m_),
    hasher_(std::move(other.hasher_)),
    registers_(std::move(other.registers_)) {
  other.precision_  = 0;
  other.register_m_ = 0;
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::operator=(HyperLogLog&& other) noexcept(std::is_nothrow_move_assignable_v<Hash>) -> HyperLogLog& {
  if (this != &other) {
    precision_        = other.precision_;
    register_m_       = other.register_m_;
    hasher_           = std::move(other.hasher_);
    registers_        = std::move(other.registers_);
    other.precision_  = 0;
    other.register_m_ = 0;
  }
  return *this;
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::from_error(double relative_error, Hash hasher) -> HyperLogLog<Key, Hash> {
  if (!(relative_error > 0.0 && relative_error < 1.0)) {
    throw HyperLogLogException("HyperLogLog relative_error must be in (0, 1)");
  }

  // Standard error is 1.04/sqrt(m), so m = (1.04/error)^2 and precision = ceil(log2(m)).
  const double exact_precision = 2.0 * std::log2(kErrorConstant / relative_error);
  const double rounded         = std::ceil(exact_precision);

  size_type precision = kMinPrecision;
  if (rounded > static_cast<double>(kMaxPrecision)) {
    precision = kMaxPrecision;
  } else if (rounded > static_cast<double>(kMinPrecision)) {
    precision = static_cast<size_type>(rounded);
  }
  return HyperLogLog(precision, std::move(hasher));
}

//===----- CORE OPERATIONS -----------------------------------------------------===//

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::add(const Key& key) -> void {
  if (register_m_ == 0) {
    throw HyperLogLogException("HyperLogLog cannot add to a moved-from sketch");
  }
  const auto [index, rho] = bucket_and_rho(key);
  registers_[index]       = std::max(registers_[index], rho);
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::estimate() const -> size_type {
  if (register_m_ == 0) {
    return 0;
  }
  double estimate_value = raw_estimate();

  // Small-range correction: with many empty registers, linear counting is sharper.
  if (estimate_value <= 2.5 * static_cast<double>(register_m_)) {
    size_type zeros = 0;
    for (size_type i = 0; i < register_m_; ++i) {
      if (registers_[i] == 0) {
        ++zeros;
      }
    }
    if (zeros != 0) {
      const double m = static_cast<double>(register_m_);
      estimate_value = m * std::log(m / static_cast<double>(zeros));
    }
  }
  // No large-range correction: the 64-bit hash makes 2^32 collisions negligible.

  const double max_result = static_cast<double>(std::numeric_limits<size_type>::max());
  if (!std::isfinite(estimate_value) || estimate_value >= max_result) {
    return std::numeric_limits<size_type>::max();
  }
  return static_cast<size_type>(estimate_value + 0.5);
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::merge(const HyperLogLog& other) -> void {
  if (precision_ != other.precision_ || !has_compatible_hasher(other)) {
    throw HyperLogLogException("HyperLogLogs must have the same precision and hash family to be merged");
  }

  for (size_type i = 0; i < register_m_; ++i) {
    registers_[i] = std::max(registers_[i], other.registers_[i]);
  }
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::clear() noexcept -> void {
  for (size_type i = 0; i < register_m_; ++i) {
    registers_[i] = std::uint8_t{0};
  }
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::precision() const noexcept -> size_type {
  return precision_;
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::register_count() const noexcept -> size_type {
  return register_m_;
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::relative_error() const noexcept -> double {
  return register_m_ == 0 ? 0.0 : kErrorConstant / std::sqrt(static_cast<double>(register_m_));
}

//===----- PRIVATE HELPERS -----------------------------------------------------===//

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::mix_hash(std::uint64_t value) noexcept -> std::uint64_t {
  value += 0x9e37'79b9'7f4a'7c15ULL;
  value = (value ^ (value >> 30U)) * 0xbf58'476d'1ce4'e5b9ULL;
  value = (value ^ (value >> 27U)) * 0x94d0'49bb'1331'11ebULL;
  return value ^ (value >> 31U);
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::bucket_and_rho(const Key& key) const -> std::pair<size_type, std::uint8_t> {
  const std::uint64_t hash = mix_hash(static_cast<std::uint64_t>(hasher_(key)));

  // Top 'precision_' bits select the register; the rest carry the rho value.
  const auto index = static_cast<size_type>(hash >> (64U - precision_));

  // Shift the index bits out; the remaining bits now occupy the high end.
  const std::uint64_t tail    = hash << precision_;
  const auto          max_rho = static_cast<std::uint8_t>(64U - precision_ + 1U);
  const std::uint8_t  rho     = tail == 0 ? max_rho : static_cast<std::uint8_t>(std::countl_zero(tail) + 1);

  return {index, rho};
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::raw_estimate() const -> double {
  double sum = 0.0;
  for (size_type i = 0; i < register_m_; ++i) {
    sum += std::ldexp(1.0, -static_cast<int>(registers_[i])); // 2^-register
  }
  const double m = static_cast<double>(register_m_);
  return alpha() * m * m / sum;
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::alpha() const noexcept -> double {
  switch (register_m_) {
  case 16:
    return 0.673;
  case 32:
    return 0.697;
  case 64:
    return 0.709;
  default:
    return 0.7213 / (1.0 + 1.079 / static_cast<double>(register_m_));
  }
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::validate_precision(size_type precision) -> size_type {
  if (precision < kMinPrecision || precision > kMaxPrecision) {
    throw HyperLogLogException("HyperLogLog precision must be in [4, 18]");
  }
  return precision;
}

template <typename Key, typename Hash>
auto HyperLogLog<Key, Hash>::has_compatible_hasher(const HyperLogLog& other) const -> bool {
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
