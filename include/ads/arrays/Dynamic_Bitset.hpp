//===---------------------------------------------------------------------------===//
/**
 * @file Dynamic_Bitset.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the DynamicBitset class.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef DYNAMIC_BITSET_HPP
#define DYNAMIC_BITSET_HPP

#include "Array_Exception.hpp"
#include "Dynamic_Array.hpp"

#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <string>
#include <utility>

namespace ads::arrays {

/**
 * @brief A dynamically resizable sequence of bits with packed storage.
 *
 * @details DynamicBitset stores its bits packed into 64-bit words, so single-bit
 *          operations are O(1) and bulk operations (population count, bitwise
 *          logic, shifts) run in O(n / 64). Bit @c i lives in word @c i/64 at
 *          offset @c i%64 (least-significant-bit first). The unused high bits of
 *          the last word are kept canonically zero (the "tail-mask" invariant),
 *          which keeps @c count / @c any / @c all and comparisons correct.
 *
 *          Unlike the other array containers in this library, DynamicBitset is a
 *          regular @b value type: it is both copyable and movable, mirroring
 *          @c std::bitset. This is required because the bitwise operators return
 *          fresh bitsets by value.
 *
 * @note Binary operators (@c & @c | @c ^) accept operands of different lengths:
 *       the shorter operand is treated as zero-extended and the result has the
 *       length of the longer operand.
 */
class DynamicBitset {
public:
  using value_type      = bool;
  using size_type       = size_t;
  using const_reference = bool;

  /// @brief Sentinel returned by find operations when no matching bit exists.
  static constexpr size_type npos = static_cast<size_type>(-1);

  //===----- BIT REFERENCE PROXY -----------------------------------------------===//

  /**
   * @brief Proxy object standing in for a single mutable bit.
   *
   * @details Returned by the non-const @c operator[]. Because a bit is not
   *          individually addressable, this proxy holds the owning word and the
   *          bit offset, and forwards assignments and flips to that word. All
   *          operations are @c noexcept.
   *
   * @warning A reference is invalidated by any structural operation on the
   *          owning bitset, including @c resize, @c push_back, @c clear,
   *          @c swap, move operations, and destruction. Using an invalidated
   *          reference has undefined behavior.
   */
  class reference {
  public:
    /// @brief Assigns a boolean value to the referenced bit.
    auto operator=(bool value) noexcept -> reference&;

    /// @brief Copies the value of another referenced bit.
    auto operator=(const reference& other) noexcept -> reference&;

    /// @brief Reads the referenced bit as a boolean.
    operator bool() const noexcept;

    /// @brief Returns the complement of the referenced bit (without modifying it).
    auto operator~() const noexcept -> bool;

    /// @brief Flips the referenced bit in place.
    auto flip() noexcept -> reference&;

  private:
    friend class DynamicBitset;

    reference(std::uint64_t* word, size_t bit) noexcept : word_(word), bit_(bit) {}

    std::uint64_t* word_; ///< The word owning the referenced bit.
    size_t         bit_;  ///< The bit offset within that word (0..63).
  };

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs a bitset of @p nbits bits, each initialized to @p value.
   * @param nbits Number of bits (default: 0).
   * @param value Initial value for every bit (default: false).
   * @complexity Time O(nbits/64), Space O(nbits/64)
   */
  explicit DynamicBitset(size_t nbits = 0, bool value = false);

  /**
   * @brief Constructs a bitset from a boolean initializer list (index 0 first).
   * @param bits The bits, in ascending index order.
   * @complexity Time O(n), Space O(n)
   */
  DynamicBitset(std::initializer_list<bool> bits);

  /**
   * @brief Constructs a bitset from an iterator range of bool-convertible values.
   * @tparam InputIt Input iterator type whose value type converts to bool.
   * @param first Iterator to the first bit.
   * @param last  Iterator past the last bit.
   * @complexity Time O(n), Space O(n)
   */
  template <std::input_iterator InputIt>
  requires std::convertible_to<std::iter_value_t<InputIt>, bool>
  DynamicBitset(InputIt first, InputIt last);

  /// @brief Copy constructor (deep-copies the packed words).
  DynamicBitset(const DynamicBitset& other);

  /// @brief Move constructor. Leaves @p other as a valid empty bitset.
  DynamicBitset(DynamicBitset&& other) noexcept;

  /// @brief Copy assignment (strong exception guarantee via copy-and-swap).
  auto operator=(const DynamicBitset& other) -> DynamicBitset&;

  /// @brief Move assignment. Leaves @p other as a valid empty bitset.
  auto operator=(DynamicBitset&& other) noexcept -> DynamicBitset&;

  /// @brief Destructor.
  ~DynamicBitset() = default;

  //===----- SINGLE-BIT ACCESS -------------------------------------------------===//

  /**
   * @brief Accesses a bit with bounds checking.
   * @param pos The bit index.
   * @return The value of the bit.
   * @throws ArrayOutOfRangeException if @p pos >= size().
   */
  [[nodiscard]] auto test(size_t pos) const -> bool;

  /**
   * @brief Accesses a bit without bounds checking (read-only).
   * @param pos The bit index.
   * @return The value of the bit.
   */
  auto operator[](size_t pos) const noexcept -> bool;

  /**
   * @brief Accesses a bit without bounds checking (mutable proxy).
   * @param pos The bit index.
   * @return A reference proxy for the bit.
   */
  auto operator[](size_t pos) noexcept -> reference;

  //===----- SINGLE-BIT MUTATORS -----------------------------------------------===//

  /**
   * @brief Sets a bit to @p value.
   * @param pos The bit index.
   * @param value The value to assign (default: true).
   * @return Reference to this bitset.
   * @throws ArrayOutOfRangeException if @p pos >= size().
   */
  auto set(size_t pos, bool value = true) -> DynamicBitset&;

  /**
   * @brief Clears a bit (sets it to false).
   * @param pos The bit index.
   * @return Reference to this bitset.
   * @throws ArrayOutOfRangeException if @p pos >= size().
   */
  auto reset(size_t pos) -> DynamicBitset&;

  /**
   * @brief Flips a bit.
   * @param pos The bit index.
   * @return Reference to this bitset.
   * @throws ArrayOutOfRangeException if @p pos >= size().
   */
  auto flip(size_t pos) -> DynamicBitset&;

  //===----- WHOLE-BITSET MUTATORS ---------------------------------------------===//

  /// @brief Sets every bit to true. @complexity O(n/64)
  auto set() noexcept -> DynamicBitset&;

  /// @brief Clears every bit to false. @complexity O(n/64)
  auto reset() noexcept -> DynamicBitset&;

  /// @brief Flips every bit. @complexity O(n/64)
  auto flip() noexcept -> DynamicBitset&;

  //===----- QUERIES -----------------------------------------------------------===//

  /// @brief Returns the number of set bits (population count). @complexity O(n/64)
  [[nodiscard]] auto count() const noexcept -> size_type;

  /// @brief Returns true if any bit is set. @complexity O(n/64)
  [[nodiscard]] auto any() const noexcept -> bool;

  /// @brief Returns true if no bit is set. @complexity O(n/64)
  [[nodiscard]] auto none() const noexcept -> bool;

  /// @brief Returns true if every bit is set (vacuously true when empty). @complexity O(n/64)
  [[nodiscard]] auto all() const noexcept -> bool;

  /// @brief Returns the number of bits.
  [[nodiscard]] auto size() const noexcept -> size_type;

  /// @brief Returns true if the bitset holds no bits.
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  //===----- BIT SCANNING ------------------------------------------------------===//

  /**
   * @brief Finds the index of the lowest set bit.
   * @return The index, or @c npos if no bit is set.
   * @complexity Time O(n/64)
   */
  [[nodiscard]] auto find_first() const noexcept -> size_type;

  /**
   * @brief Finds the index of the lowest set bit strictly greater than @p pos.
   * @param pos The index to search after.
   * @return The index, or @c npos if none exists.
   * @complexity Time O(n/64)
   */
  [[nodiscard]] auto find_next(size_t pos) const noexcept -> size_type;

  //===----- SIZING ------------------------------------------------------------===//

  /**
   * @brief Resizes the bitset to @p nbits bits.
   * @param nbits The new number of bits.
   * @param value Value for bits newly appended when growing (default: false).
   * @details Growing appends bits equal to @p value; shrinking drops the highest
   *          bits. Existing low-order bits are preserved.
   * @complexity Time O(n/64)
   */
  auto resize(size_t nbits, bool value = false) -> void;

  /**
   * @brief Appends a single bit at the highest index.
   * @param value The value of the new bit.
   * @throws ArrayOverflowException if the logical size cannot be increased.
   * @complexity Time O(1) amortized
   */
  auto push_back(bool value) -> void;

  /// @brief Removes all bits (size becomes 0). @complexity O(n/64)
  auto clear() noexcept -> void;

  //===----- BITWISE COMPOUND OPERATORS ----------------------------------------===//

  /// @brief In-place bitwise AND (zero-extends the shorter operand).
  auto operator&=(const DynamicBitset& rhs) -> DynamicBitset&;

  /// @brief In-place bitwise OR (zero-extends the shorter operand).
  auto operator|=(const DynamicBitset& rhs) -> DynamicBitset&;

  /// @brief In-place bitwise XOR (zero-extends the shorter operand).
  auto operator^=(const DynamicBitset& rhs) -> DynamicBitset&;

  /// @brief In-place left shift by @p n bits (size preserved; vacated bits zero).
  auto operator<<=(size_t n) noexcept -> DynamicBitset&;

  /// @brief In-place right shift by @p n bits (size preserved; vacated bits zero).
  auto operator>>=(size_t n) noexcept -> DynamicBitset&;

  /// @brief Returns the bitwise complement.
  [[nodiscard]] auto operator~() const -> DynamicBitset;

  //===----- CONVERSION --------------------------------------------------------===//

  /**
   * @brief Renders the bitset as a string, most-significant bit first.
   * @param zero Character for a cleared bit (default: '0').
   * @param one  Character for a set bit (default: '1').
   * @return The string representation (empty when size() == 0).
   * @note Uses std::string as an output bridge; not internal storage.
   * @complexity Time O(n)
   */
  [[nodiscard]] auto to_string(char zero = '0', char one = '1') const -> std::string;

  //===----- UTILITY -----------------------------------------------------------===//

  /// @brief Swaps the contents of two bitsets. @complexity O(1)
  auto swap(DynamicBitset& other) noexcept -> void;

  /// @brief Non-member swap (ADL). @complexity O(1)
  friend auto swap(DynamicBitset& a, DynamicBitset& b) noexcept -> void { a.swap(b); }

  /**
   * @brief Equality comparison (equal size and identical bits).
   * @complexity Time O(n/64)
   */
  friend auto operator==(const DynamicBitset& a, const DynamicBitset& b) noexcept -> bool {
    if (a.bit_count_ != b.bit_count_) {
      return false;
    }
    for (size_t i = 0; i < a.words_.size(); ++i) {
      if (a.words_[i] != b.words_[i]) {
        return false;
      }
    }
    return true;
  }

private:
  //===----- PRIVATE HELPERS ---------------------------------------------------===//

  static constexpr size_t kBitsPerWord = 64;

  /// @brief Word index holding bit @p pos.
  static constexpr auto word_of(size_t pos) noexcept -> size_t { return pos / kBitsPerWord; }

  /// @brief Bit offset of @p pos within its word.
  static constexpr auto offset_of(size_t pos) noexcept -> size_t { return pos % kBitsPerWord; }

  /// @brief Number of words needed to store @p nbits bits.
  static constexpr auto words_for(size_t nbits) noexcept -> size_t {
    return nbits / kBitsPerWord + static_cast<size_t>(nbits % kBitsPerWord != 0);
  }

  /// @brief Reads bit @p pos without bounds checking.
  auto get_bit(size_t pos) const noexcept -> bool { return ((words_[word_of(pos)] >> offset_of(pos)) & std::uint64_t{1}) != 0; }

  /// @brief Restores the tail-mask invariant on the last word.
  auto clear_unused_bits() noexcept -> void {
    const size_t r = offset_of(bit_count_);
    if (r != 0 && !words_.is_empty()) {
      words_[words_.size() - 1] &= (std::uint64_t{1} << r) - 1;
    }
  }

  //===----- DATA MEMBERS ------------------------------------------------------===//

  DynamicArray<std::uint64_t> words_;         ///< Packed bit storage (LSB-first).
  size_t                      bit_count_ = 0; ///< Number of logical bits.
};

//===----- FREE BITWISE OPERATORS ----------------------------------------------===//

/// @brief Bitwise AND of two bitsets (zero-extends the shorter operand).
auto operator&(const DynamicBitset& a, const DynamicBitset& b) -> DynamicBitset;

/// @brief Bitwise OR of two bitsets (zero-extends the shorter operand).
auto operator|(const DynamicBitset& a, const DynamicBitset& b) -> DynamicBitset;

/// @brief Bitwise XOR of two bitsets (zero-extends the shorter operand).
auto operator^(const DynamicBitset& a, const DynamicBitset& b) -> DynamicBitset;

/// @brief Left shift producing a new bitset (size preserved).
auto operator<<(const DynamicBitset& bs, size_t n) -> DynamicBitset;

/// @brief Right shift producing a new bitset (size preserved).
auto operator>>(const DynamicBitset& bs, size_t n) -> DynamicBitset;

} // namespace ads::arrays

// Include the implementation file for the (inline) member and free definitions.
#include "../../../src/ads/arrays/Dynamic_Bitset.tpp"

#endif // DYNAMIC_BITSET_HPP

//===---------------------------------------------------------------------------===//
