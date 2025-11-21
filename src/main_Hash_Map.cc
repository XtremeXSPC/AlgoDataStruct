//===--------------------------------------------------------------------------===//
/**
 * @file main_Hash_Map.cc
 * @author Costantino Lombardi
 * @brief Test file for HashMap class with iterators
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include <iostream>
#include <sstream>
#include <string>

#include "../include/ads/associative/Hash_Map.hpp"

using namespace ads::associative;

void print_separator(const std::string& title) {
  std::cout << "\n========== " << title << " ==========\n";
}

void test_hashmap_basic() {
  print_separator("HashMap - Basic Operations");

  HashMap<std::string, int> scores;

  scores["Alice"]   = 95;
  scores["Bob"]     = 87;
  scores["Charlie"] = 92;

  std::cout << "Created map with 3 entries\n";
  std::cout << "Size: " << scores.size() << '\n';
  std::cout << "Alice's score: " << scores["Alice"] << '\n';
  std::cout << "Bob's score: " << scores.at("Bob") << '\n';

  std::cout << "\nContains checks:\n";
  std::cout << "Contains 'Alice': " << std::boolalpha << scores.contains("Alice") << '\n';
  std::cout << "Contains 'David': " << scores.contains("David") << '\n';
}

void test_hashmap_iteration() {
  print_separator("HashMap - Range-Based For Loop");

  HashMap<std::string, int> ages = {{"Alice", 25}, {"Bob", 30}, {"Charlie", 28}};

  std::cout << "Iterating with range-based for:\n";
  for (const auto& [name, age] : ages) {
    std::cout << "  " << name << ": " << age << " years old\n";
  }

  std::cout << "\nIterating with explicit iterators:\n";
  for (auto it = ages.begin(); it != ages.end(); ++it) {
    std::cout << "  " << it->first << " -> " << it->second << '\n';
  }
}

void test_hashmap_insert_emplace() {
  print_separator("HashMap - Insert and Emplace");

  HashMap<int, std::string> map;

  auto [it1, inserted1] = map.insert({1, "one"});
  auto [it2, inserted2] = map.insert({2, "two"});
  auto [it3, inserted3] = map.insert({1, "ONE"}); // Duplicate key

  std::cout << "Insert {1, \"one\"}: " << (inserted1 ? "inserted" : "not inserted") << '\n';
  std::cout << "Insert {2, \"two\"}: " << (inserted2 ? "inserted" : "not inserted") << '\n';
  std::cout << "Insert {1, \"ONE\"}: " << (inserted3 ? "inserted" : "not inserted (duplicate)") << '\n';

  map.emplace(3, "three");
  std::cout << "\nAfter emplace(3, \"three\"), size: " << map.size() << '\n';

  std::cout << "\nAll entries:\n";
  for (const auto& [key, value] : map) {
    std::cout << "  " << key << " -> " << value << '\n';
  }
}

void test_hashmap_erase() {
  print_separator("HashMap - Erase Operations");

  HashMap<std::string, double> prices = {{"apple", 1.20}, {"banana", 0.50}, {"orange", 0.80}, {"grape", 2.50}};

  std::cout << "Initial size: " << prices.size() << '\n';

  size_t erased = prices.erase("banana");
  std::cout << "Erased 'banana': " << erased << " elements removed\n";
  std::cout << "Size after erase: " << prices.size() << '\n';

  auto it = prices.find("apple");
  if (it != prices.end()) {
    prices.erase(it);
    std::cout << "Erased 'apple' via iterator\n";
  }

  std::cout << "Final size: " << prices.size() << '\n';
  std::cout << "Remaining entries:\n";
  for (const auto& [item, price] : prices) {
    std::cout << "  " << item << ": $" << price << '\n';
  }
}

void test_hashmap_utility_methods() {
  print_separator("HashMap - Utility Methods (keys, values, entries)");

  HashMap<std::string, int> inventory = {{"apples", 50}, {"oranges", 30}, {"bananas", 45}, {"grapes", 25}};

  std::cout << "Inventory map:\n";
  for (const auto& [item, count] : inventory) {
    std::cout << "  " << item << ": " << count << '\n';
  }

  auto keys = inventory.keys();
  std::cout << "\nAll keys: ";
  for (const auto& key : keys) {
    std::cout << key << " ";
  }
  std::cout << '\n';

  auto values = inventory.values();
  std::cout << "All values: ";
  for (const auto& val : values) {
    std::cout << val << " ";
  }
  std::cout << '\n';

  auto entries = inventory.entries();
  std::cout << "All entries (as pairs):\n";
  for (const auto& [k, v] : entries) {
    std::cout << "  {" << k << ", " << v << "}\n";
  }
}

void test_word_frequency() {
  print_separator("Application - Word Frequency Counter");

  std::string text = "the quick brown fox jumps over the lazy dog the fox was quick";

  HashMap<std::string, int> frequency;

  std::istringstream iss(text);
  std::string        word;
  while (iss >> word) {
    frequency[word]++;
  }

  std::cout << "Text: \"" << text << "\"\n\n";
  std::cout << "Word frequencies:\n";
  for (const auto& [w, count] : frequency) {
    std::cout << "  '" << w << "': " << count << " time(s)\n";
  }

  std::cout << "\nMost frequent word(s):\n";
  int         max_count = 0;
  std::string most_frequent;

  for (const auto& [w, count] : frequency) {
    if (count > max_count) {
      max_count     = count;
      most_frequent = w;
    }
  }

  std::cout << "  '" << most_frequent << "' appears " << max_count << " times\n";
}

void test_hashmap_move_semantics() {
  print_separator("HashMap - Move Semantics");

  HashMap<int, std::string> map1;
  map1[1] = "one";
  map1[2] = "two";
  map1[3] = "three";

  std::cout << "Map1 size: " << map1.size() << '\n';

  HashMap<int, std::string> map2(std::move(map1));
  std::cout << "After move to map2:\n";
  std::cout << "  Map2 size: " << map2.size() << '\n';
  std::cout << "  Map1 size: " << map1.size() << '\n';

  HashMap<int, std::string> map3;
  map3 = std::move(map2);
  std::cout << "After move assign to map3:\n";
  std::cout << "  Map3 size: " << map3.size() << '\n';
}

int main() {
  std::cout << "╔═══════════════════════════════════════════════════════╗\n";
  std::cout << "║          HASH MAP - COMPREHENSIVE TEST SUITE          ║\n";
  std::cout << "╚═══════════════════════════════════════════════════════╝\n";

  try {
    test_hashmap_basic();
    test_hashmap_iteration();
    test_hashmap_insert_emplace();
    test_hashmap_erase();
    test_hashmap_utility_methods();
    test_word_frequency();
    test_hashmap_move_semantics();

    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║           ALL TESTS COMPLETED SUCCESSFULLY!           ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n";

  } catch (const std::exception& e) {
    std::cerr << "\nTest failed with exception: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
//===--------------------------------------------------------------------------===//
