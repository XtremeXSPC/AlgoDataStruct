//===---------------------------------------------------------------------------===//
/**
 * @file main_HyperLogLog.cc
 * @author Costantino Lombardi
 * @brief Demo program for the HyperLogLog cardinality estimator.
 * @version 0.1
 * @date 2026-07-10
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the HyperLogLog: estimating the number of distinct
 * items in a stream using only a few kilobytes of registers, the fact that
 * duplicates do not inflate the count, precision-driven accuracy, and the
 * register-wise mergeability of partial streams.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/probabilistic/HyperLogLog.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using ads::arrays::DynamicArray;
using ads::probabilistic::HyperLogLog;

//===----- BASIC CARDINALITY ---------------------------------------------------===//

auto demo_basic() -> void {
  ads::demo::print_section("HyperLogLog - Distinct Count Estimation");

  HyperLogLog<int> sketch(14);

  // 100000 distinct keys, each observed several times.
  for (int repeat = 0; repeat < 5; ++repeat) {
    for (int i = 0; i < 100'000; ++i) {
      sketch.add(i);
    }
  }

  ads::demo::print_info("Stream: 100000 distinct keys, each added 5 times.");
  cout << "registers      = " << sketch.register_count() << "\n";
  cout << "true distinct  = 100000\n";
  cout << "estimate()     = " << sketch.estimate() << "\n";
  cout << "std. error     = " << sketch.relative_error() << "\n";
  ads::demo::print_success("Duplicates do not inflate the distinct count.");
}

//===----- SIZING FOR A TARGET ERROR -------------------------------------------===//

auto demo_from_error() -> void {
  ads::demo::print_section("HyperLogLog - Sizing by Target Error");

  auto sketch = HyperLogLog<string>::from_error(/*relative_error=*/0.02);
  ads::demo::print_info("from_error(relative_error=0.02).");
  cout << "precision      = " << sketch.precision() << "\n";
  cout << "registers      = " << sketch.register_count() << "\n";

  DynamicArray<string> stream;
  for (int i = 0; i < 25'000; ++i) {
    stream.push_back("user-" + std::to_string(i));
  }
  for (const string& item : stream) {
    sketch.add(item);
  }
  cout << "true distinct  = 25000\n";
  cout << "estimate()     = " << sketch.estimate() << "\n";
  ads::demo::print_success("Register count chosen to keep error near the target.");
}

//===----- MERGE ---------------------------------------------------------------===//

auto demo_merge() -> void {
  ads::demo::print_section("HyperLogLog - Merging Partial Streams");

  HyperLogLog<int> shard_a(14);
  HyperLogLog<int> shard_b(14);

  for (int i = 0; i < 40'000; ++i) {
    shard_a.add(i); // keys 0..39999
  }
  for (int i = 20'000; i < 60'000; ++i) {
    shard_b.add(i); // keys 20000..59999 (20000 overlap)
  }
  ads::demo::print_info("Two shards with overlapping key ranges, then merged.");

  shard_a.merge(shard_b);
  cout << "union true distinct = 60000\n";
  cout << "merged estimate()   = " << shard_a.estimate() << "\n";
  ads::demo::print_success("Sketches take the register-wise maximum: map-reduce friendly.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  ads::demo::print_header("HYPERLOGLOG - COMPREHENSIVE DEMO");

  try {
    demo_basic();
    demo_from_error();
    demo_merge();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
