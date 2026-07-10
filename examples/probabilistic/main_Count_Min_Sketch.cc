//===---------------------------------------------------------------------------===//
/**
 * @file main_Count_Min_Sketch.cc
 * @author Costantino Lombardi
 * @brief Demo program for the CountMinSketch frequency estimator.
 * @version 0.1
 * @date 2026-07-10
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the Count-Min Sketch: estimating item frequencies in
 * sub-linear space, the one-sided (never-underestimate) guarantee, sizing via a
 * target error/confidence, and the linear mergeability of partial streams.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/probabilistic/Count_Min_Sketch.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using ads::arrays::DynamicArray;
using ads::probabilistic::CountMinSketch;

//===----- BASIC ESTIMATION ----------------------------------------------------===//

auto demo_basic() -> void {
  ads::demo::print_section("Count-Min Sketch - Frequency Estimation");

  CountMinSketch<string> sketch(512, 4);

  // A tiny stream with a clear heavy hitter.
  DynamicArray<string> stream;
  for (int i = 0; i < 50; ++i) {
    stream.push_back("apple");
  }
  for (int i = 0; i < 10; ++i) {
    stream.push_back("banana");
  }
  stream.push_back("cherry");

  for (const string& item : stream) {
    sketch.add(item);
  }

  ads::demo::print_info("Stream: 50x apple, 10x banana, 1x cherry.");
  cout << "estimate(apple)  = " << sketch.estimate("apple") << "\n";
  cout << "estimate(banana) = " << sketch.estimate("banana") << "\n";
  cout << "estimate(cherry) = " << sketch.estimate("cherry") << "\n";
  cout << "estimate(durian) = " << sketch.estimate("durian") << " (never added)\n";
  ads::demo::print_success("Estimates never fall below the true frequency.");
}

//===----- SIZING FOR A TARGET ERROR -------------------------------------------===//

auto demo_from_estimates() -> void {
  ads::demo::print_section("Count-Min Sketch - Sizing by Error / Confidence");

  auto sketch = CountMinSketch<int>::from_estimates(/*epsilon=*/0.001, /*delta=*/0.01);
  ads::demo::print_info("from_estimates(epsilon=0.001, delta=0.01).");
  cout << "width = " << sketch.width() << ", depth = " << sketch.depth() << "\n";

  for (int i = 0; i < 100'000; ++i) {
    sketch.add(i % 500); // 500 distinct keys, 200 each
  }
  cout << "After 100000 adds, total = " << sketch.total_count() << "\n";
  cout << "estimate(key 0) = " << sketch.estimate(0) << " (true 200)\n";
  cout << "additive error bound e*N/width = " << sketch.error_bound() << "\n";
  ads::demo::print_success("Table sized so the overestimate stays within epsilon*N.");
}

//===----- MERGE ---------------------------------------------------------------===//

auto demo_merge() -> void {
  ads::demo::print_section("Count-Min Sketch - Merging Partial Streams");

  CountMinSketch<string> shard_a(512, 4);
  CountMinSketch<string> shard_b(512, 4);

  shard_a.add("x", 30);
  shard_a.add("y", 5);
  shard_b.add("x", 12);
  shard_b.add("z", 7);
  ads::demo::print_info("Two shards counted independently, then merged.");

  shard_a.merge(shard_b);
  cout << "merged estimate(x) = " << shard_a.estimate("x") << " (30 + 12)\n";
  cout << "merged estimate(y) = " << shard_a.estimate("y") << "\n";
  cout << "merged estimate(z) = " << shard_a.estimate("z") << "\n";
  cout << "merged total       = " << shard_a.total_count() << "\n";
  ads::demo::print_success("Sketches sum element-wise: map-reduce friendly.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  ads::demo::print_header("COUNT-MIN SKETCH - COMPREHENSIVE DEMO");

  try {
    demo_basic();
    demo_from_estimates();
    demo_merge();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
