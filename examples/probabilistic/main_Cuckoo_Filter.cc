//===---------------------------------------------------------------------------===//
/**
 * @file main_Cuckoo_Filter.cc
 * @author Costantino Lombardi
 * @brief Demo program for the CuckooFilter probabilistic set.
 * @version 0.1
 * @date 2026-07-12
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the Cuckoo filter: approximate membership in
 * sub-linear space, the deletion support that a Bloom filter lacks, and the way
 * the structure fills densely before an insertion can fail near saturation.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/probabilistic/Cuckoo_Filter.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using ads::arrays::DynamicArray;
using ads::probabilistic::CuckooFilter;

//===----- BASIC MEMBERSHIP ----------------------------------------------------===//

auto demo_membership() -> void {
  ads::demo::print_section("Cuckoo Filter - Approximate Membership");

  CuckooFilter<string> filter(1'024);

  DynamicArray<string> words;
  words.push_back("alpha");
  words.push_back("bravo");
  words.push_back("charlie");
  for (const string& word : words) {
    (void)filter.insert(word);
  }

  ads::demo::print_info("Inserted: alpha, bravo, charlie.");
  cout << "might_contain(alpha)  = " << filter.might_contain("alpha") << "\n";
  cout << "might_contain(bravo)  = " << filter.might_contain("bravo") << "\n";
  cout << "might_contain(delta)  = " << filter.might_contain("delta") << " (never inserted)\n";
  ads::demo::print_success("Inserted keys are always found; absent keys almost never are.");
}

//===----- DELETION ------------------------------------------------------------===//

auto demo_deletion() -> void {
  ads::demo::print_section("Cuckoo Filter - Deletion Support");

  CuckooFilter<int> filter(1'024);
  (void)filter.insert(1'234);
  (void)filter.insert(5'678);
  ads::demo::print_info("Inserted 1234 and 5678, then erase 1234.");

  cout << "before: contains(1234) = " << filter.might_contain(1'234) << "\n";
  const bool erased = filter.erase(1'234);
  cout << "erase(1234) returned     " << erased << "\n";
  cout << "after:  contains(1234) = " << filter.might_contain(1'234) << "\n";
  cout << "after:  contains(5678) = " << filter.might_contain(5'678) << " (untouched)\n";
  ads::demo::print_success("Unlike a Bloom filter, a Cuckoo filter can delete keys.");
}

//===----- CAPACITY & SATURATION -----------------------------------------------===//

auto demo_capacity() -> void {
  ads::demo::print_section("Cuckoo Filter - Load and Saturation");

  CuckooFilter<int> filter(256);
  ads::demo::print_info("Small filter sized for 256 keys, then filled until an insert fails.");
  cout << "bucket_count = " << filter.bucket_count() << ", capacity = " << filter.capacity() << "\n";

  int inserted = 0;
  for (int i = 0; i < 100'000; ++i) {
    if (!filter.insert(i)) {
      break;
    }
    ++inserted;
  }

  cout << "stored fingerprints = " << inserted << "\n";
  cout << "load factor         = " << filter.load_factor() << "\n";
  cout << "false-positive rate ~ " << filter.false_positive_rate() << "\n";
  ads::demo::print_success("The filter packs densely before an insertion fails.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  ads::demo::print_header("CUCKOO FILTER - COMPREHENSIVE DEMO");

  try {
    demo_membership();
    demo_deletion();
    demo_capacity();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
