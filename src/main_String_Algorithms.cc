//===---------------------------------------------------------------------------===//
/**
 * @file main_String_Algorithms.cc
 * @author Costantino Lombardi
 * @brief Demo for KMP and Rabin-Karp pattern matching.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/algorithms/String_Algorithms.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string_view>

using std::cout;

using namespace ads::algorithms;

namespace {

//===------------------------------ DEMO HELPERS -------------------------------===//

auto print_matches(std::string_view label, const ads::arrays::DynamicArray<size_t>& matches) -> void {
  cout << label;
  if (matches.is_empty()) {
    cout << "none\n";
    return;
  }

  for (size_t index = 0; index < matches.size(); ++index) {
    cout << matches[index];
    if (index + 1 < matches.size()) {
      cout << ", ";
    }
  }
  cout << '\n';
}

} // namespace

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  ads::demo::print_header({"STRING ALGORITHMS DEMO", "KMP and Rabin-Karp"});

  constexpr std::string_view text    = "abracadabra abracadabra";
  constexpr std::string_view pattern = "abra";

  const auto kmp_matches   = kmp_search(text, pattern);
  const auto rabin_matches = rabin_karp_search(text, pattern);

  cout << "Text:    " << text << '\n';
  cout << "Pattern: " << pattern << "\n\n";

  print_matches("KMP matches:         ", kmp_matches);
  print_matches("Rabin-Karp matches:  ", rabin_matches);

  ads::demo::print_footer("STRING ALGORITHMS DEMO COMPLETED!");
  return 0;
}

//===---------------------------------------------------------------------------===//
