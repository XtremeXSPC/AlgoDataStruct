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

#include "../include/ads/probabilistic/Bloom_Filter.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cout;
using std::string;

using namespace ads::probabilistic;

//===----- MAIN FUNCTION -------------------------------------------------------===//

auto main() -> int {
  ads::demo::print_header({"BLOOM FILTER DEMO", "Probabilistic membership queries"});

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

  ads::demo::print_footer("BLOOM FILTER DEMO COMPLETED!");
  return 0;
}

//===---------------------------------------------------------------------------===//
