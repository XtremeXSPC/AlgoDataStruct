//===---------------------------------------------------------------------------===//
/**
 * @file main_Hash_Map.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for HashMap class with iterators.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the HashMap data structure, showcasing
 * its insertion, access, iteration, and utility methods.
 */
//===---------------------------------------------------------------------------===//

#include <format>
#include <iostream>
#include <sstream>
#include <string>

#include "../include/ads/associative/Hash_Map.hpp"
using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::to_string;
using std::vector;

using namespace ads::associative;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Print a separator with title.
void print_separator(const string& title) {
  cout << "\n=====---------- " << title << " ----------=====\n";
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Test basic insertion and access.
void demo_hashmap_basic() {
  print_separator("HashMap - Basic Operations");

  HashMap<string, int> scores;

  scores["Alice"]   = 95;
  scores["Bob"]     = 87;
  scores["Charlie"] = 92;

  cout << "Created map with 3 entries\n";
  cout << "Size: " << scores.size() << '\n';
  cout << "Alice's score: " << scores["Alice"] << '\n';
  cout << "Bob's score: " << scores.at("Bob") << '\n';

  cout << "\nContains checks:\n";
  cout << std::format("Contains 'Alice': {}\n", scores.contains("Alice"));
  cout << std::format("Contains 'David': {}\n", scores.contains("David"));
}

//===----------------------------- ITERATION DEMO ------------------------------===//

// Test iteration over entries.
void demo_hashmap_iteration() {
  print_separator("HashMap - Range-Based For Loop");

  HashMap<string, int> ages = {{"Alice", 25}, {"Bob", 30}, {"Charlie", 28}};

  cout << "Iterating with range-based for:\n";
  for (const auto& [name, age] : ages) {
    cout << "  " << name << ": " << age << " years old\n";
  }

  cout << "\nIterating with explicit iterators:\n";
  for (auto it = ages.begin(); it != ages.end(); ++it) {
    cout << "  " << it->first << " -> " << it->second << '\n';
  }
}

//===----------------------- INSERTION AND EMPLACE DEMO ------------------------===//

// Test insert and emplace methods.
void demo_hashmap_insert_emplace() {
  print_separator("HashMap - Insert and Emplace");

  HashMap<int, string> map;

  auto [it1, inserted1] = map.insert({1, "one"});
  auto [it2, inserted2] = map.insert({2, "two"});
  auto [it3, inserted3] = map.insert({1, "ONE"}); // Duplicate key

  cout << "Insert {1, \"one\"}: " << (inserted1 ? "inserted" : "not inserted") << '\n';
  cout << "Insert {2, \"two\"}: " << (inserted2 ? "inserted" : "not inserted") << '\n';
  cout << "Insert {1, \"ONE\"}: " << (inserted3 ? "inserted" : "not inserted (duplicate)") << '\n';

  map.emplace(3, "three");
  cout << "\nAfter emplace(3, \"three\"), size: " << map.size() << '\n';

  cout << "\nAll entries:\n";
  for (const auto& [key, value] : map) {
    cout << "  " << key << " -> " << value << '\n';
  }
}

//===-------------------------- ERASE OPERATIONS DEMO --------------------------===//

// Test erase method.
void demo_hashmap_erase() {
  print_separator("HashMap - Erase Operations");

  HashMap<string, double> prices = {{"apple", 1.20}, {"banana", 0.50}, {"orange", 0.80}, {"grape", 2.50}};

  cout << "Initial size: " << prices.size() << '\n';

  size_t erased = prices.erase("banana");
  cout << "Erased 'banana': " << erased << " elements removed\n";
  cout << "Size after erase: " << prices.size() << '\n';

  auto it = prices.find("apple");
  if (it != prices.end()) {
    prices.erase(it);
    cout << "Erased 'apple' via iterator\n";
  }

  cout << "Final size: " << prices.size() << '\n';
  cout << "Remaining entries:\n";
  for (const auto& [item, price] : prices) {
    cout << "  " << item << ": $" << price << '\n';
  }
}

//===-------------------------- UTILITY METHODS DEMO ---------------------------===//

// Test keys, values, and entries methods.
void demo_hashmap_utility_methods() {
  print_separator("HashMap - Utility Methods (keys, values, entries)");

  HashMap<string, int> inventory = {{"apples", 50}, {"oranges", 30}, {"bananas", 45}, {"grapes", 25}};

  cout << "Inventory map:\n";
  for (const auto& [item, count] : inventory) {
    cout << "  " << item << ": " << count << '\n';
  }

  auto keys = inventory.keys();
  cout << "\nAll keys: ";
  for (const auto& key : keys) {
    cout << key << " ";
  }
  cout << '\n';

  auto values = inventory.values();
  cout << "All values: ";
  for (const auto& val : values) {
    cout << val << " ";
  }
  cout << '\n';

  auto entries = inventory.entries();
  cout << "All entries (as pairs):\n";
  for (const auto& [k, v] : entries) {
    cout << "  {" << k << ", " << v << "}\n";
  }
}

//===------------------------ APPLICATION EXAMPLE DEMO -------------------------===//

// Application: Word frequency counter.
void demo_word_frequency() {
  print_separator("Application - Word Frequency Counter");

  string text = "the quick brown fox jumps over the lazy dog the fox was quick";

  HashMap<string, int> frequency;

  std::istringstream iss(text);
  string             word;
  while (iss >> word) {
    frequency[word]++;
  }

  cout << "Text: \"" << text << "\"\n\n";
  cout << "Word frequencies:\n";
  for (const auto& [w, count] : frequency) {
    cout << "  '" << w << "': " << count << " time(s)\n";
  }

  cout << "\nMost frequent word(s):\n";
  int    max_count = 0;
  string most_frequent;

  for (const auto& [w, count] : frequency) {
    if (count > max_count) {
      max_count     = count;
      most_frequent = w;
    }
  }

  cout << "  '" << most_frequent << "' appears " << max_count << " times\n";
}

//===--------------------------- MOVE SEMANTICS DEMO ---------------------------===//

// Test move semantics.
void demo_hashmap_move_semantics() {
  print_separator("HashMap - Move Semantics");

  HashMap<int, string> map1;
  map1[1] = "one";
  map1[2] = "two";
  map1[3] = "three";

  cout << "Map1 size: " << map1.size() << '\n';

  HashMap<int, string> map2(std::move(map1));
  cout << "After move to map2:\n";
  cout << "  Map2 size: " << map2.size() << '\n';

  HashMap<int, string> map3;
  map3 = std::move(map2);
  cout << "After move assign to map3:\n";
  cout << "  Map3 size: " << map3.size() << '\n';
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  cout << "╔═══----------------------------------------------------═══╗\n";
  cout << "           HASH MAP - COMPREHENSIVE DEMO PROGRAM            \n";
  cout << "╚═══----------------------------------------------------═══╝\n";

  try {
    demo_hashmap_basic();
    demo_hashmap_iteration();
    demo_hashmap_insert_emplace();
    demo_hashmap_erase();
    demo_hashmap_utility_methods();
    demo_word_frequency();
    demo_hashmap_move_semantics();

    cout << "\n";
    cout << "╔═══----------------------------------------------------═══╗\n";
    cout << "             ALL DEMOS COMPLETED SUCCESSFULLY!              \n";
    cout << "╚═══----------------------------------------------------═══╝\n";

  } catch (const exception& e) {
    cerr << "\nTest failed with exception: " << e.what() << '\n';
    return 1;
  }

  return 0;
}

//===--------------------------------------------------------------------------===//
