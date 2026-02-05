//===---------------------------------------------------------------------------===//
/**
 * @file main_Tree_Map.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for TreeMap implementation.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the usage of the TreeMap data structure,
 * showcasing ordered insertion, lookup, and update operations.
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/associative/Tree_Map.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using namespace ads::associative;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Helper function to print map entries.
template <typename K, typename V>
void print_entries(const TreeMap<K, V>& map, const string& label) {
  cout << label << " (size: " << map.size() << ")\n";
  auto entries = map.entries();
  if (entries.empty()) {
    cout << "  (empty)\n";
    return;
  }

  for (const auto& [key, value] : entries) {
    cout << "  " << key << " => " << value << "\n";
  }
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Demonstrates basic insertion and extraction.
void demo_basic_operations() {
  ads::demo::print_section("Demo: Basic Operations");

  TreeMap<string, int> scores;
  scores.put("Alice", 92);
  scores.put("Bob", 85);
  scores.put("Charlie", 78);

  print_entries(scores, "Initial entries (ordered)");

  cout << "\nUpdating Bob's score to 90\n";
  scores.put("Bob", 90);
  print_entries(scores, "After update");

  cout << "\nContains Alice? " << (scores.contains("Alice") ? "yes" : "no") << "\n";
  cout << "Contains Diana? " << (scores.contains("Diana") ? "yes" : "no") << "\n";
}

//===-------------------------- ACCESS OPERATIONS DEMO -------------------------===//

// Demonstrates access operations.
void demo_access_operations() {
  ads::demo::print_section("Demo: Access Operations");

  TreeMap<string, int> scores;
  scores.put("Elena", 88);
  scores.put("Frank", 75);

  cout << "Elena's score (get): " << scores.get("Elena") << "\n";

  cout << "\nUsing operator[] for Diana (default insert)\n";
  scores["Diana"] = 95;
  print_entries(scores, "After operator[] insert");

  cout << "\nTrying to access missing key...\n";
  try {
    scores.at("Ghost");
  } catch (const KeyNotFoundException& e) {
    cout << "  Exception caught: " << e.what() << "\n";
  }
}

//===-------------------------- EMPLACE OPERATIONS DEMO -------------------------===//

// Demonstrates emplace operations.
void demo_emplace_operations() {
  ads::demo::print_section("Demo: Emplace Operations");

  struct Person {
    string name;
    int    age;

    Person(string n, int a) : name(std::move(n)), age(a) {}
  };

  TreeMap<int, Person> people;
  people.emplace(10, "Alice", 28);
  people.emplace(5, "Bob", 34);
  people.emplace(15, "Charlie", 40);

  cout << "People ordered by ID:\n";
  for (const auto& [id, person] : people.entries()) {
    cout << "  " << id << ": " << person.name << " (" << person.age << ")\n";
  }
}

//===-------------------------- REMOVAL OPERATIONS DEMO -------------------------===//

// Demonstrates removal operations.
void demo_removal_operations() {
  ads::demo::print_section("Demo: Removal Operations");

  TreeMap<int, string> items;
  items.put(3, "Gamma");
  items.put(1, "Alpha");
  items.put(2, "Beta");

  print_entries(items, "Before removal");

  cout << "\nRemoving key 2\n";
  items.remove(2);
  print_entries(items, "After removal");
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("TREE MAP - COMPREHENSIVE DEMO");

    demo_basic_operations();
    demo_access_operations();
    demo_emplace_operations();
    demo_removal_operations();

    ads::demo::print_footer();
  } catch (const exception& e) {
    ads::demo::print_error(string("Unhandled exception: ") + e.what());
    return 1;
  }

  return 0;
}

//===---------------------------------------------------------------------------===//
