//===---------------------------------------------------------------------------===//
/**
 * @file Dynamic_Bitset.tpp
 * @author Costantino Lombardi
 * @brief Inline definitions for the DynamicBitset class.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 *
 * @details DynamicBitset is a non-template class, so every out-of-line definition
 *          here is marked @c inline to remain ODR-safe when this file is included
 *          (through the header) into multiple translation units.
 */
//===---------------------------------------------------------------------------===//

#pragma once

#include "../../../include/ads/arrays/Dynamic_Bitset.hpp"

#include <algorithm>

namespace ads::arrays {

//===----- BIT REFERENCE PROXY -------------------------------------------------===//

inline auto DynamicBitset::reference::operator=(bool value) noexcept -> reference& {
  if (value) {
    *word_ |= (std::uint64_t{1} << bit_);
  } else {
    *word_ &= ~(std::uint64_t{1} << bit_);
  }
  return *this;
}

inline auto DynamicBitset::reference::operator=(const reference& other) noexcept -> reference& {
  return (*this = static_cast<bool>(other));
}

inline DynamicBitset::reference::operator bool() const noexcept {
  return ((*word_ >> bit_) & std::uint64_t{1}) != 0;
}

inline auto DynamicBitset::reference::operator~() const noexcept -> bool {
  return !static_cast<bool>(*this);
}

inline auto DynamicBitset::reference::flip() noexcept -> reference& {
  *word_ ^= (std::uint64_t{1} << bit_);
  return *this;
}

//===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT --------------------------------===//

inline DynamicBitset::DynamicBitset(size_t nbits, bool value) :
    words_(words_for(nbits), value ? ~std::uint64_t{0} : std::uint64_t{0}),
    bit_count_(nbits) {
  clear_unused_bits();
}

inline DynamicBitset::DynamicBitset(std::initializer_list<bool> bits) :
    words_(words_for(bits.size()), std::uint64_t{0}),
    bit_count_(bits.size()) {
  size_t i = 0;
  for (bool b : bits) {
    if (b) {
      words_[word_of(i)] |= (std::uint64_t{1} << offset_of(i));
    }
    ++i;
  }
}

template <std::input_iterator InputIt>
requires std::convertible_to<std::iter_value_t<InputIt>, bool>
DynamicBitset::DynamicBitset(InputIt first, InputIt last) {
  for (; first != last; ++first) {
    push_back(static_cast<bool>(*first));
  }
}

inline DynamicBitset::DynamicBitset(const DynamicBitset& other) :
    words_(other.words_.begin(), other.words_.end()),
    bit_count_(other.bit_count_) {
}

inline DynamicBitset::DynamicBitset(DynamicBitset&& other) noexcept : words_(std::move(other.words_)), bit_count_(other.bit_count_) {
  other.bit_count_ = 0;
}

inline auto DynamicBitset::operator=(const DynamicBitset& other) -> DynamicBitset& {
  if (this != &other) {
    DynamicBitset tmp(other);
    swap(tmp);
  }
  return *this;
}

inline auto DynamicBitset::operator=(DynamicBitset&& other) noexcept -> DynamicBitset& {
  if (this != &other) {
    words_           = std::move(other.words_);
    bit_count_       = other.bit_count_;
    other.bit_count_ = 0;
  }
  return *this;
}

//===----- SINGLE-BIT ACCESS ---------------------------------------------------===//

inline auto DynamicBitset::test(size_t pos) const -> bool {
  if (pos >= bit_count_) {
    throw ArrayOutOfRangeException();
  }
  return get_bit(pos);
}

inline auto DynamicBitset::operator[](size_t pos) const noexcept -> bool {
  return get_bit(pos);
}

inline auto DynamicBitset::operator[](size_t pos) noexcept -> reference {
  return reference(&words_[word_of(pos)], offset_of(pos));
}

//===----- SINGLE-BIT MUTATORS -------------------------------------------------===//

inline auto DynamicBitset::set(size_t pos, bool value) -> DynamicBitset& {
  if (pos >= bit_count_) {
    throw ArrayOutOfRangeException();
  }
  if (value) {
    words_[word_of(pos)] |= (std::uint64_t{1} << offset_of(pos));
  } else {
    words_[word_of(pos)] &= ~(std::uint64_t{1} << offset_of(pos));
  }
  return *this;
}

inline auto DynamicBitset::reset(size_t pos) -> DynamicBitset& {
  if (pos >= bit_count_) {
    throw ArrayOutOfRangeException();
  }
  words_[word_of(pos)] &= ~(std::uint64_t{1} << offset_of(pos));
  return *this;
}

inline auto DynamicBitset::flip(size_t pos) -> DynamicBitset& {
  if (pos >= bit_count_) {
    throw ArrayOutOfRangeException();
  }
  words_[word_of(pos)] ^= (std::uint64_t{1} << offset_of(pos));
  return *this;
}

//===----- WHOLE-BITSET MUTATORS -----------------------------------------------===//

inline auto DynamicBitset::set() noexcept -> DynamicBitset& {
  for (size_t i = 0; i < words_.size(); ++i) {
    words_[i] = ~std::uint64_t{0};
  }
  clear_unused_bits();
  return *this;
}

inline auto DynamicBitset::reset() noexcept -> DynamicBitset& {
  for (size_t i = 0; i < words_.size(); ++i) {
    words_[i] = std::uint64_t{0};
  }
  return *this;
}

inline auto DynamicBitset::flip() noexcept -> DynamicBitset& {
  for (size_t i = 0; i < words_.size(); ++i) {
    words_[i] = ~words_[i];
  }
  clear_unused_bits();
  return *this;
}

//===----- QUERIES -------------------------------------------------------------===//

inline auto DynamicBitset::count() const noexcept -> size_type {
  size_type total = 0;
  for (size_t i = 0; i < words_.size(); ++i) {
    total += static_cast<size_type>(std::popcount(words_[i]));
  }
  return total;
}

inline auto DynamicBitset::any() const noexcept -> bool {
  for (size_t i = 0; i < words_.size(); ++i) {
    if (words_[i] != 0) {
      return true;
    }
  }
  return false;
}

inline auto DynamicBitset::none() const noexcept -> bool {
  return !any();
}

inline auto DynamicBitset::all() const noexcept -> bool {
  if (bit_count_ == 0) {
    return true;
  }
  const size_t full = bit_count_ / kBitsPerWord;
  for (size_t i = 0; i < full; ++i) {
    if (words_[i] != ~std::uint64_t{0}) {
      return false;
    }
  }
  const size_t r = offset_of(bit_count_);
  if (r != 0) {
    const std::uint64_t mask = (std::uint64_t{1} << r) - 1;
    if ((words_[full] & mask) != mask) {
      return false;
    }
  }
  return true;
}

inline auto DynamicBitset::size() const noexcept -> size_type {
  return bit_count_;
}

inline auto DynamicBitset::is_empty() const noexcept -> bool {
  return bit_count_ == 0;
}

//===----- BIT SCANNING --------------------------------------------------------===//

inline auto DynamicBitset::find_first() const noexcept -> size_type {
  for (size_t i = 0; i < words_.size(); ++i) {
    if (words_[i] != 0) {
      return i * kBitsPerWord + static_cast<size_type>(std::countr_zero(words_[i]));
    }
  }
  return npos;
}

inline auto DynamicBitset::find_next(size_t pos) const noexcept -> size_type {
  if (bit_count_ == 0 || pos >= bit_count_ - 1) {
    return npos;
  }
  const size_t  start = pos + 1;
  size_t        wi    = word_of(start);
  std::uint64_t word  = words_[wi] & (~std::uint64_t{0} << offset_of(start));
  if (word != 0) {
    return wi * kBitsPerWord + static_cast<size_type>(std::countr_zero(word));
  }
  for (++wi; wi < words_.size(); ++wi) {
    if (words_[wi] != 0) {
      return wi * kBitsPerWord + static_cast<size_type>(std::countr_zero(words_[wi]));
    }
  }
  return npos;
}

//===----- SIZING --------------------------------------------------------------===//

inline auto DynamicBitset::resize(size_t nbits, bool value) -> void {
  if (nbits == bit_count_) {
    return;
  }
  const size_t old_bits = bit_count_;
  words_.resize(words_for(nbits), value ? ~std::uint64_t{0} : std::uint64_t{0});
  bit_count_ = nbits;

  if (nbits > old_bits && value) {
    // resize() only fills newly appended words; the previously-last partial word
    // keeps its high bits zero, so set them explicitly when growing with ones.
    const size_t r = offset_of(old_bits);
    if (r != 0) {
      words_[word_of(old_bits)] |= (~std::uint64_t{0} << r);
    }
  }
  clear_unused_bits();
}

inline auto DynamicBitset::push_back(bool value) -> void {
  if (bit_count_ == npos) {
    throw ArrayOverflowException("DynamicBitset size overflow");
  }
  if (offset_of(bit_count_) == 0) {
    words_.push_back(std::uint64_t{0}); // may throw; bit_count_ untouched on failure
  }
  if (value) {
    words_[word_of(bit_count_)] |= (std::uint64_t{1} << offset_of(bit_count_));
  }
  ++bit_count_;
}

inline auto DynamicBitset::clear() noexcept -> void {
  words_.clear();
  bit_count_ = 0;
}

//===----- BITWISE COMPOUND OPERATORS ------------------------------------------===//

inline auto DynamicBitset::operator&=(const DynamicBitset& rhs) -> DynamicBitset& {
  resize(std::max(bit_count_, rhs.bit_count_), false);
  const size_t rw = rhs.words_.size();
  for (size_t i = 0; i < words_.size(); ++i) {
    words_[i] &= (i < rw) ? rhs.words_[i] : std::uint64_t{0};
  }
  clear_unused_bits();
  return *this;
}

inline auto DynamicBitset::operator|=(const DynamicBitset& rhs) -> DynamicBitset& {
  resize(std::max(bit_count_, rhs.bit_count_), false);
  const size_t rw = rhs.words_.size();
  for (size_t i = 0; i < rw; ++i) {
    words_[i] |= rhs.words_[i];
  }
  clear_unused_bits();
  return *this;
}

inline auto DynamicBitset::operator^=(const DynamicBitset& rhs) -> DynamicBitset& {
  resize(std::max(bit_count_, rhs.bit_count_), false);
  const size_t rw = rhs.words_.size();
  for (size_t i = 0; i < rw; ++i) {
    words_[i] ^= rhs.words_[i];
  }
  clear_unused_bits();
  return *this;
}

inline auto DynamicBitset::operator<<=(size_t n) noexcept -> DynamicBitset& {
  if (n == 0 || bit_count_ == 0) {
    return *this;
  }
  if (n >= bit_count_) {
    return reset();
  }
  const size_t word_shift = n / kBitsPerWord;
  const size_t bit_shift  = n % kBitsPerWord;
  const size_t nw         = words_.size();

  if (bit_shift == 0) {
    for (size_t i = nw; i-- > word_shift;) {
      words_[i] = words_[i - word_shift];
    }
  } else {
    for (size_t i = nw; i-- > word_shift;) {
      const std::uint64_t high = words_[i - word_shift] << bit_shift;
      const std::uint64_t low  = (i - word_shift >= 1) ? (words_[i - word_shift - 1] >> (kBitsPerWord - bit_shift)) : std::uint64_t{0};
      words_[i]                = high | low;
    }
  }
  for (size_t i = 0; i < word_shift; ++i) {
    words_[i] = std::uint64_t{0};
  }
  clear_unused_bits();
  return *this;
}

inline auto DynamicBitset::operator>>=(size_t n) noexcept -> DynamicBitset& {
  if (n == 0 || bit_count_ == 0) {
    return *this;
  }
  if (n >= bit_count_) {
    return reset();
  }
  const size_t word_shift = n / kBitsPerWord;
  const size_t bit_shift  = n % kBitsPerWord;
  const size_t nw         = words_.size();

  if (bit_shift == 0) {
    for (size_t i = 0; i + word_shift < nw; ++i) {
      words_[i] = words_[i + word_shift];
    }
  } else {
    for (size_t i = 0; i < nw; ++i) {
      const std::uint64_t low  = (i + word_shift < nw) ? (words_[i + word_shift] >> bit_shift) : std::uint64_t{0};
      const std::uint64_t high = (i + word_shift + 1 < nw) ? (words_[i + word_shift + 1] << (kBitsPerWord - bit_shift)) : std::uint64_t{0};
      words_[i]                = low | high;
    }
  }
  for (size_t i = nw - word_shift; i < nw; ++i) {
    words_[i] = std::uint64_t{0};
  }
  clear_unused_bits();
  return *this;
}

inline auto DynamicBitset::operator~() const -> DynamicBitset {
  DynamicBitset result(*this);
  result.flip();
  return result;
}

//===----- CONVERSION ----------------------------------------------------------===//

inline auto DynamicBitset::to_string(char zero, char one) const -> std::string {
  std::string out;
  out.reserve(bit_count_);
  for (size_t i = bit_count_; i-- > 0;) {
    out.push_back(get_bit(i) ? one : zero);
  }
  return out;
}

//===----- UTILITY -------------------------------------------------------------===//

inline auto DynamicBitset::swap(DynamicBitset& other) noexcept -> void {
  using std::swap;
  swap(words_, other.words_);
  swap(bit_count_, other.bit_count_);
}

//===----- FREE BITWISE OPERATORS ----------------------------------------------===//

inline auto operator&(const DynamicBitset& a, const DynamicBitset& b) -> DynamicBitset {
  const DynamicBitset& longer  = (a.size() >= b.size()) ? a : b;
  const DynamicBitset& shorter = (a.size() >= b.size()) ? b : a;
  DynamicBitset        result(longer);
  result &= shorter;
  return result;
}

inline auto operator|(const DynamicBitset& a, const DynamicBitset& b) -> DynamicBitset {
  const DynamicBitset& longer  = (a.size() >= b.size()) ? a : b;
  const DynamicBitset& shorter = (a.size() >= b.size()) ? b : a;
  DynamicBitset        result(longer);
  result |= shorter;
  return result;
}

inline auto operator^(const DynamicBitset& a, const DynamicBitset& b) -> DynamicBitset {
  const DynamicBitset& longer  = (a.size() >= b.size()) ? a : b;
  const DynamicBitset& shorter = (a.size() >= b.size()) ? b : a;
  DynamicBitset        result(longer);
  result ^= shorter;
  return result;
}

inline auto operator<<(const DynamicBitset& bs, size_t n) -> DynamicBitset {
  DynamicBitset result(bs);
  result <<= n;
  return result;
}

inline auto operator>>(const DynamicBitset& bs, size_t n) -> DynamicBitset {
  DynamicBitset result(bs);
  result >>= n;
  return result;
}

} // namespace ads::arrays

//===---------------------------------------------------------------------------===//
