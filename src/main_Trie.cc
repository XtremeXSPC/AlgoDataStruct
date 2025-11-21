//===--------------------------------------------------------------------------===//
/**
 * @file main_Trie.cc
 * @author Costantino Lombardi
 * @brief Comprehensive test suite for Trie (Prefix Tree) implementation
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include "ads/trees/Trie.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>

using namespace ads::trees;
using namespace std;

// ANSI color codes for output
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
  cout << CYAN << "[TEST] " << RESET << name << "..." << endl;

#define ASSERT(condition, message) \
  do { \
    if (!(condition)) { \
      cout << RED << "  ✗ FAILED: " << RESET << message << endl; \
      tests_failed++; \
      return; \
    } \
  } while (0)

#define PASS() \
  do { \
    cout << GREEN << "  ✓ PASSED" << RESET << endl; \
    tests_passed++; \
  } while (0)

//===--------------------------------------------------------------------------===//
// Test Functions
//===--------------------------------------------------------------------------===//

/**
 * @brief Test basic insert and search operations
 */
void test_basic_insert_search() {
  TEST("Basic Insert and Search");

  TrieMap trie;

  // Test empty trie
  ASSERT(trie.is_empty(), "New trie should be empty");
  ASSERT(trie.size() == 0, "New trie should have size 0");

  // Insert words
  trie.insert("hello");
  ASSERT(!trie.is_empty(), "Trie should not be empty after insert");
  ASSERT(trie.size() == 1, "Size should be 1 after one insert");
  ASSERT(trie.search("hello"), "Should find inserted word 'hello'");

  trie.insert("world");
  ASSERT(trie.size() == 2, "Size should be 2 after two inserts");
  ASSERT(trie.search("world"), "Should find inserted word 'world'");

  // Search for non-existent words
  ASSERT(!trie.search("hel"), "'hel' is prefix, not word");
  ASSERT(!trie.search("worlds"), "'worlds' was not inserted");
  ASSERT(!trie.search(""), "Empty string should not be found");

  // Insert duplicate
  trie.insert("hello");
  ASSERT(trie.size() == 2, "Size should remain 2 after duplicate insert");

  PASS();
}

/**
 * @brief Test prefix matching functionality
 */
void test_starts_with() {
  TEST("Starts With (Prefix Matching)");

  TrieMap trie;
  trie.insert("apple");
  trie.insert("application");
  trie.insert("apply");
  trie.insert("banana");

  // Valid prefixes
  ASSERT(trie.starts_with("app"), "Should find prefix 'app'");
  ASSERT(trie.starts_with("appl"), "Should find prefix 'appl'");
  ASSERT(trie.starts_with("apple"), "Complete word is valid prefix");
  ASSERT(trie.starts_with("ban"), "Should find prefix 'ban'");
  ASSERT(trie.starts_with(""), "Empty prefix should match everything");

  // Invalid prefixes
  ASSERT(!trie.starts_with("orange"), "Should not find 'orange' prefix");
  ASSERT(!trie.starts_with("applz"), "Should not find 'applz' prefix");

  PASS();
}

/**
 * @brief Test word removal
 */
void test_remove() {
  TEST("Remove Word");

  TrieMap trie;
  trie.insert("car");
  trie.insert("card");
  trie.insert("care");
  trie.insert("careful");
  trie.insert("cat");

  ASSERT(trie.size() == 5, "Should have 5 words initially");

  // Remove word that's not prefix of others
  ASSERT(trie.remove("cat"), "Should successfully remove 'cat'");
  ASSERT(!trie.search("cat"), "'cat' should be gone");
  ASSERT(trie.size() == 4, "Size should be 4 after removal");

  // Remove word that's prefix of others
  ASSERT(trie.remove("care"), "Should successfully remove 'care'");
  ASSERT(!trie.search("care"), "'care' should be gone");
  ASSERT(trie.search("careful"), "'careful' should still exist");
  ASSERT(trie.size() == 3, "Size should be 3 after removal");

  // Remove word that has prefixes
  ASSERT(trie.remove("card"), "Should successfully remove 'card'");
  ASSERT(trie.search("car"), "'car' should still exist");
  ASSERT(trie.size() == 2, "Size should be 2 after removal");

  // Try removing non-existent word
  ASSERT(!trie.remove("cart"), "Should return false for non-existent word");
  ASSERT(trie.size() == 2, "Size should remain 2");

  // Remove all remaining words
  trie.remove("car");
  trie.remove("careful");
  ASSERT(trie.is_empty(), "Trie should be empty after removing all");

  PASS();
}

/**
 * @brief Test getting all words with prefix
 */
void test_get_all_words_with_prefix() {
  TEST("Get All Words With Prefix");

  TrieMap trie;
  trie.insert("car");
  trie.insert("card");
  trie.insert("care");
  trie.insert("careful");
  trie.insert("cat");
  trie.insert("dog");

  // Get words with prefix "car"
  auto car_words = trie.get_all_words_with_prefix("car");
  ASSERT(car_words.size() == 4, "Should find 4 words with prefix 'car'");

  // Verify all expected words are present
  vector<string> expected_car = {"car", "card", "care", "careful"};
  sort(car_words.begin(), car_words.end());
  sort(expected_car.begin(), expected_car.end());
  ASSERT(car_words == expected_car, "Should match expected words");

  // Get words with prefix "ca"
  auto ca_words = trie.get_all_words_with_prefix("ca");
  ASSERT(ca_words.size() == 5, "Should find 5 words with prefix 'ca'");

  // Get words with prefix that doesn't exist
  auto xyz_words = trie.get_all_words_with_prefix("xyz");
  ASSERT(xyz_words.empty(), "Should find no words with prefix 'xyz'");

  // Get all words (empty prefix)
  auto all_words = trie.get_all_words_with_prefix("");
  ASSERT(all_words.size() == 6, "Should find all 6 words with empty prefix");

  PASS();
}

/**
 * @brief Test counting words with prefix
 */
void test_count_words_with_prefix() {
  TEST("Count Words With Prefix");

  TrieMap trie;
  trie.insert("test");
  trie.insert("testing");
  trie.insert("tester");
  trie.insert("tested");
  trie.insert("team");
  trie.insert("tea");

  ASSERT(trie.count_words_with_prefix("test") == 4, "Should count 4 'test*' words");
  ASSERT(trie.count_words_with_prefix("tea") == 2, "Should count 2 'tea*' words");
  ASSERT(trie.count_words_with_prefix("te") == 6, "Should count all 6 'te*' words");
  ASSERT(trie.count_words_with_prefix("xyz") == 0, "Should count 0 'xyz*' words");
  ASSERT(trie.count_words_with_prefix("") == 6, "Should count all words");

  PASS();
}

/**
 * @brief Test longest common prefix
 */
void test_longest_common_prefix() {
  TEST("Longest Common Prefix");

  TrieMap trie;

  // Empty trie
  ASSERT(trie.longest_common_prefix() == "", "Empty trie should return empty prefix");

  // Single word
  trie.insert("flower");
  ASSERT(trie.longest_common_prefix() == "flower", "Single word should be LCP");

  // Multiple words with common prefix
  trie.insert("flow");
  trie.insert("flight");
  string lcp = trie.longest_common_prefix();
  ASSERT(lcp == "fl", "LCP of flower, flow, flight should be 'fl'");

  // No common prefix
  TrieMap trie2;
  trie2.insert("dog");
  trie2.insert("racecar");
  trie2.insert("car");
  ASSERT(trie2.longest_common_prefix() == "", "No LCP for different starting letters");

  PASS();
}

/**
 * @brief Test clear operation
 */
void test_clear() {
  TEST("Clear Trie");

  TrieMap trie;
  trie.insert("hello");
  trie.insert("world");
  trie.insert("test");

  ASSERT(trie.size() == 3, "Should have 3 words");

  trie.clear();
  ASSERT(trie.is_empty(), "Trie should be empty after clear");
  ASSERT(trie.size() == 0, "Size should be 0 after clear");
  ASSERT(!trie.search("hello"), "Should not find cleared words");

  // Insert after clear
  trie.insert("new");
  ASSERT(trie.size() == 1, "Should be able to insert after clear");
  ASSERT(trie.search("new"), "Should find newly inserted word");

  PASS();
}

/**
 * @brief Test array-based trie (lowercase a-z only)
 */
void test_array_based_trie() {
  TEST("Array-Based Trie (a-z only)");

  TrieArray trie;

  // Insert lowercase words
  trie.insert("hello");
  trie.insert("world");
  trie.insert("algorithm");

  ASSERT(trie.size() == 3, "Should have 3 words");
  ASSERT(trie.search("hello"), "Should find 'hello'");
  ASSERT(trie.search("world"), "Should find 'world'");
  ASSERT(trie.starts_with("alg"), "Should find prefix 'alg'");

  // Test operations
  auto words = trie.get_all_words_with_prefix("h");
  ASSERT(words.size() == 1, "Should find 1 word with prefix 'h'");

  PASS();
}

/**
 * @brief Test exception handling
 */
void test_exceptions() {
  TEST("Exception Handling");

  TrieMap trie;

  // Empty word insertion
  try {
    trie.insert("");
    ASSERT(false, "Should throw on empty word insert");
  } catch (const std::invalid_argument& e) {
    // Expected
  }

  // For array-based trie, test invalid characters
  TrieArray array_trie;
  try {
    array_trie.insert("Hello");  // Uppercase not allowed
    ASSERT(false, "Should throw on uppercase in array trie");
  } catch (const std::invalid_argument& e) {
    // Expected
  }

  try {
    array_trie.insert("hello!");  // Special char not allowed
    ASSERT(false, "Should throw on special char in array trie");
  } catch (const std::invalid_argument& e) {
    // Expected
  }

  PASS();
}

/**
 * @brief Test with dictionary words (autocomplete scenario)
 */
void test_autocomplete_scenario() {
  TEST("Autocomplete Scenario");

  TrieMap dictionary;

  // Build small dictionary
  vector<string> words = {
    "program", "programming", "programmer", "programs",
    "progress", "project", "projector",
    "provide", "provider", "provision"
  };

  for (const auto& word : words) {
    dictionary.insert(word);
  }

  // User types "prog"
  auto suggestions = dictionary.get_all_words_with_prefix("prog");
  ASSERT(suggestions.size() == 5, "Should suggest 5 words for 'prog'");

  // User types "proje"
  suggestions = dictionary.get_all_words_with_prefix("proje");
  ASSERT(suggestions.size() == 2, "Should suggest 2 words for 'proje'");

  // User types "provi"
  suggestions = dictionary.get_all_words_with_prefix("provi");
  ASSERT(suggestions.size() == 3, "Should suggest 3 words for 'provi'");

  PASS();
}

/**
 * @brief Performance test with large dataset
 */
void test_performance() {
  TEST("Performance Test (Large Dataset)");

  TrieMap trie;
  const int num_words = 10000;

  // Measure insert time
  auto start = chrono::high_resolution_clock::now();

  for (int i = 0; i < num_words; ++i) {
    string word = "word" + to_string(i);
    trie.insert(word);
  }

  auto end = chrono::high_resolution_clock::now();
  auto insert_duration = chrono::duration_cast<chrono::milliseconds>(end - start);

  cout << "  Insert " << num_words << " words: " << insert_duration.count() << " ms" << endl;
  ASSERT(trie.size() == num_words, "Should have inserted all words");

  // Measure search time
  start = chrono::high_resolution_clock::now();

  for (int i = 0; i < num_words; ++i) {
    string word = "word" + to_string(i);
    ASSERT(trie.search(word), "Should find all inserted words");
  }

  end = chrono::high_resolution_clock::now();
  auto search_duration = chrono::duration_cast<chrono::milliseconds>(end - start);

  cout << "  Search " << num_words << " words: " << search_duration.count() << " ms" << endl;

  // Measure prefix search
  start = chrono::high_resolution_clock::now();

  auto results = trie.get_all_words_with_prefix("word1");

  end = chrono::high_resolution_clock::now();
  auto prefix_duration = chrono::duration_cast<chrono::milliseconds>(end - start);

  cout << "  Prefix search 'word1*': " << prefix_duration.count()
       << " ms (found " << results.size() << " matches)" << endl;

  PASS();
}

/**
 * @brief Test move semantics
 */
void test_move_semantics() {
  TEST("Move Semantics");

  TrieMap trie1;
  trie1.insert("hello");
  trie1.insert("world");
  trie1.insert("test");

  size_t original_size = trie1.size();

  // Move constructor
  TrieMap trie2(std::move(trie1));
  ASSERT(trie2.size() == original_size, "Moved trie should have same size");
  ASSERT(trie2.search("hello"), "Moved trie should contain words");

  // Move assignment
  TrieMap trie3;
  trie3.insert("temp");
  trie3 = std::move(trie2);
  ASSERT(trie3.size() == original_size, "Move assigned trie should have same size");
  ASSERT(trie3.search("world"), "Move assigned trie should contain words");

  PASS();
}

/**
 * @brief Stress test with complex scenarios
 */
void test_stress() {
  TEST("Stress Test");

  TrieMap trie;

  // Insert many words with shared prefixes
  vector<string> prefixes = {"cat", "car", "dog", "dot", "test", "tea"};
  vector<string> suffixes = {"", "s", "ed", "ing", "er", "est"};

  int word_count = 0;
  for (const auto& prefix : prefixes) {
    for (const auto& suffix : suffixes) {
      string word = prefix + suffix;
      if (!word.empty()) {
        trie.insert(word);
        word_count++;
      }
    }
  }

  ASSERT(trie.size() == word_count, "Should have inserted all combinations");

  // Verify all words can be found
  for (const auto& prefix : prefixes) {
    for (const auto& suffix : suffixes) {
      string word = prefix + suffix;
      if (!word.empty()) {
        ASSERT(trie.search(word), "Should find word: " + word);
      }
    }
  }

  // Remove half the words
  for (size_t i = 0; i < prefixes.size() / 2; ++i) {
    for (const auto& suffix : suffixes) {
      string word = prefixes[i] + suffix;
      if (!word.empty()) {
        trie.remove(word);
      }
    }
  }

  // Verify removed words are gone
  for (size_t i = 0; i < prefixes.size() / 2; ++i) {
    for (const auto& suffix : suffixes) {
      string word = prefixes[i] + suffix;
      if (!word.empty()) {
        ASSERT(!trie.search(word), "Should not find removed word: " + word);
      }
    }
  }

  PASS();
}

//===--------------------------------------------------------------------------===//
// Main Test Runner
//===--------------------------------------------------------------------------===//

int main() {
  cout << BOLD << BLUE << "\n=================================\n" << RESET;
  cout << BOLD << "  Trie Test Suite\n" << RESET;
  cout << BOLD << BLUE << "=================================\n" << RESET << endl;

  // Run all tests
  test_basic_insert_search();
  test_starts_with();
  test_remove();
  test_get_all_words_with_prefix();
  test_count_words_with_prefix();
  test_longest_common_prefix();
  test_clear();
  test_array_based_trie();
  test_exceptions();
  test_autocomplete_scenario();
  test_performance();
  test_move_semantics();
  test_stress();

  // Print summary
  cout << BOLD << BLUE << "\n=================================\n" << RESET;
  cout << BOLD << "  Test Summary\n" << RESET;
  cout << BOLD << BLUE << "=================================\n" << RESET;
  cout << GREEN << "  Passed: " << tests_passed << RESET << endl;
  cout << RED   << "  Failed: " << tests_failed << RESET << endl;
  cout << BOLD << BLUE << "=================================\n" << RESET << endl;

  return tests_failed > 0 ? 1 : 0;
}

//===--------------------------------------------------------------------------===//
