//===---------------------------------------------------------------------------===//
/**
 * @file main_BPlus_Tree.cc
 * @author Costantino Lombardi
 * @brief Demo for the BPlusTree structure.
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the B+ tree: an ordered key-value index whose linked
 * leaves make range scans a single descent followed by a sequential walk. Here it
 * indexes a small price list by product code.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/trees/search/BPlus_Tree.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using ads::arrays::DynamicArray;
using ads::trees::BPlusTree;

namespace {

using PriceIndex = BPlusTree<int, string>;

auto sample_index() -> PriceIndex {
  const DynamicArray<PriceIndex::Entry> catalogue = []() -> DynamicArray<PriceIndex::Entry> {
    DynamicArray<PriceIndex::Entry> data;
    data.push_back({.key = 1'004, .value = "keyboard"});
    data.push_back({.key = 1'001, .value = "monitor"});
    data.push_back({.key = 1'007, .value = "webcam"});
    data.push_back({.key = 1'002, .value = "mouse"});
    data.push_back({.key = 1'005, .value = "speakers"});
    data.push_back({.key = 1'003, .value = "headset"});
    data.push_back({.key = 1'006, .value = "microphone"});
    return data;
  }();

  return PriceIndex(catalogue);
}

} // namespace

//===----- POINT LOOKUP --------------------------------------------------------===//

auto demo_lookup() -> void {
  ads::demo::print_section("B+ Tree - Point Lookup");

  PriceIndex index = sample_index();
  cout << "records = " << index.size() << ", height = " << index.height() << "\n";

  const int code = 1'003;
  cout << "lookup code " << code << " -> ";
  if (const string* name = index.find(code)) {
    cout << *name << "\n";
  } else {
    cout << "(not found)\n";
  }
  ads::demo::print_success("A lookup descends the routers to a single leaf.");
}

//===----- RANGE SCAN ----------------------------------------------------------===//

auto demo_range() -> void {
  ads::demo::print_section("B+ Tree - Ordered Range Scan");

  PriceIndex index = sample_index();
  const int  low   = 1'002;
  const int  high  = 1'005;

  cout << "codes in [" << low << ", " << high << "]:\n";
  index.range(low, high, [](const int& code, const string& name) -> void { cout << "  - " << code << ": " << name << "\n"; });
  ads::demo::print_success("Range scan walks the linked leaves after one descent.");
}

//===----- FULL ORDERED WALK ---------------------------------------------------===//

auto demo_walk() -> void {
  ads::demo::print_section("B+ Tree - Full Ordered Walk");

  PriceIndex index = sample_index();
  cout << "all records in key order: ";
  for (const auto& entry : index) {
    cout << entry.key << ' ';
  }
  cout << "\n";
  cout << "smallest = " << index.find_min().key << ", largest = " << index.find_max().key << "\n";
  ads::demo::print_success("Iterating the leaf chain yields every record in ascending order.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("B+ TREE - COMPREHENSIVE DEMO");
    demo_lookup();
    demo_range();
    demo_walk();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
