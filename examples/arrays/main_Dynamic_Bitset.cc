//===---------------------------------------------------------------------------===//
/**
 * @file main_Dynamic_Bitset.cc
 * @author Costantino Lombardi
 * @brief Demo for the DynamicBitset structure.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the DynamicBitset: a resizable, word-packed bit
 * sequence. It runs a Sieve of Eratosthenes, treats bitsets as integer sets with
 * union / intersection / difference, and scans set bits with find_first/find_next.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/arrays/Dynamic_Bitset.hpp"
#include "support/Demo_Utilities.hpp"

#include <cstddef>
#include <iostream>

using std::cerr;
using std::cout;
using std::exception;

using ads::arrays::DynamicArray;
using ads::arrays::DynamicBitset;

namespace {

// Collects every set bit index into a DynamicArray using the scanning interface.
auto set_bits(const DynamicBitset& bs) -> DynamicArray<size_t> {
  DynamicArray<size_t> members;
  for (size_t p = bs.find_first(); p != DynamicBitset::npos; p = bs.find_next(p)) {
    members.push_back(p);
  }
  return members;
}

auto print_bits(const DynamicArray<size_t>& members) -> void {
  for (const size_t& m : members) {
    cout << m << ' ';
  }
  cout << "\n";
}

} // namespace

//===----- SIEVE OF ERATOSTHENES -----------------------------------------------===//

auto demo_sieve() -> void {
  ads::demo::print_section("Dynamic Bitset - Sieve of Eratosthenes");

  const size_t  limit = 100;
  DynamicBitset is_prime(limit + 1, true); // index i marks whether i is prime
  is_prime.reset(0);
  is_prime.reset(1);

  for (size_t i = 2; i * i <= limit; ++i) {
    if (is_prime.test(i)) {
      for (size_t multiple = i * i; multiple <= limit; multiple += i) {
        is_prime.reset(multiple);
      }
    }
  }

  cout << "primes up to " << limit << " (" << is_prime.count() << " found): ";
  print_bits(set_bits(is_prime));
  ads::demo::print_success("Each composite is cleared with an O(1) reset; count is a popcount.");
}

//===----- BITSETS AS INTEGER SETS ---------------------------------------------===//

auto demo_set_algebra() -> void {
  ads::demo::print_section("Dynamic Bitset - Set Algebra");

  const size_t  universe = 32;
  DynamicBitset multiples_of_3(universe, false);
  DynamicBitset multiples_of_4(universe, false);

  const DynamicArray<size_t> seeds = []() -> DynamicArray<size_t> {
    DynamicArray<size_t> data;
    for (size_t i = 0; i < 32; ++i) {
      data.push_back(i);
    }
    return data;
  }();

  for (const size_t& i : seeds) {
    if (i % 3 == 0) {
      multiples_of_3.set(i);
    }
    if (i % 4 == 0) {
      multiples_of_4.set(i);
    }
  }

  cout << "multiples of 3: ";
  print_bits(set_bits(multiples_of_3));
  cout << "multiples of 4: ";
  print_bits(set_bits(multiples_of_4));

  cout << "union (| ):     ";
  print_bits(set_bits(multiples_of_3 | multiples_of_4));
  cout << "intersect (& ): ";
  print_bits(set_bits(multiples_of_3 & multiples_of_4));
  cout << "sym.diff (^ ):  ";
  print_bits(set_bits(multiples_of_3 ^ multiples_of_4));

  ads::demo::print_success("Union, intersection, and difference are single word-parallel passes.");
}

//===----- RESIZE AND SHIFT ----------------------------------------------------===//

auto demo_resize_shift() -> void {
  ads::demo::print_section("Dynamic Bitset - Resize and Shift");

  DynamicBitset flags(4, true); // 1111
  cout << "start (4 bits): " << flags.to_string() << "\n";

  flags.resize(12, false);
  cout << "grown to 12:    " << flags.to_string() << "\n";

  flags <<= 4;
  cout << "shifted << 4:   " << flags.to_string() << "\n";

  flags.push_back(true);
  cout << "push_back(1):   " << flags.to_string() << " (size " << flags.size() << ")\n";

  ads::demo::print_success("Growth preserves low bits; shifts keep the length fixed.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("DYNAMIC BITSET - COMPREHENSIVE DEMO");
    demo_sieve();
    demo_set_algebra();
    demo_resize_shift();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
