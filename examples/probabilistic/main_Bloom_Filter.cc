//===---------------------------------------------------------------------------===//
/**
 * @file main_Bloom_Filter.cc
 * @author Costantino Lombardi
 * @brief Demo for the Bloom filter probabilistic set.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#include "ads/probabilistic/Bloom_Filter.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using namespace ads::probabilistic;

//===----- DEMO FILTER OPERATIONS ----------------------------------------------===//

auto demo_filter_operations() -> void {
  ads::demo::print_section("Bloom Filter Operations");

  auto filter = BloomFilter<string>::from_estimates(1'000, 0.01);

  filter.insert("graph");
  filter.insert("algorithm");
  filter.insert("trie");

  cout << "Bit count: " << filter.bit_count() << '\n';
  cout << "Hash count: " << filter.hash_count() << '\n';
  cout << "Load factor: " << filter.load_factor() << '\n';
  cout << "Estimated false-positive rate: " << filter.estimated_false_positive_rate() << '\n';
  cout << '\n';

  for (const string& word : {"graph", "algorithm", "heap", "suffix"}) {
    cout << "might_contain(\"" << word << "\") = " << std::boolalpha << filter.might_contain(word) << '\n';
  }
}

//===----- MAIN FUNCTION -------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("BLOOM FILTER - COMPREHENSIVE DEMO");

    demo_filter_operations();

    ads::demo::print_footer();
  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}

//===---------------------------------------------------------------------------===//
