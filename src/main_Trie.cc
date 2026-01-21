//===--------------------------------------------------------------------------===//
/**
 * @file main_Trie.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for Trie (Prefix Tree) implementation.
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the Trie data structure, showcasing
 * prefix-based operations, autocomplete, and string manipulation.
 */
//===--------------------------------------------------------------------------===//

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../include/ads/trees/Trie.hpp"

using std::cerr;
using std::cout;
using std::string;
using std::vector;

using TrieType = ads::trees::Trie<>;

// Helper function to print trie statistics
void print_trie_stats(const TrieType& trie, const string& name) {
  cout << "Trie '" << name << "' (size: " << trie.size() << "):\n";

  if (trie.is_empty()) {
    cout << "  (empty)\n";
    return;
  }

  auto words = trie.get_all_words();
  cout << "  Words: ";
  for (size_t i = 0; i < words.size() && i < 10; ++i) {
    cout << words[i];
    if (i < words.size() - 1 && i < 9) {
      cout << ", ";
    }
  }
  if (words.size() > 10) {
    cout << "... (+" << (words.size() - 10) << " more)";
  }
  cout << '\n';
}

// Demo: Basic operations
void demo_basic_operations() {
  cout << "\n========== Demo: Basic Operations ==========\n";

  TrieType trie;

  cout << "Creating empty Trie...\n";
  cout << "  Size: " << trie.size() << ", Empty: " << (trie.is_empty() ? "yes" : "no") << "\n";

  // Insert words
  cout << "\nInserting words: hello, world, help, heap, hero\n";
  trie.insert("hello");
  trie.insert("world");
  trie.insert("help");
  trie.insert("heap");
  trie.insert("hero");

  print_trie_stats(trie, "trie");

  // Search for words
  cout << "\nSearching for words:\n";
  cout << "  Contains 'hello'? " << (trie.search("hello") ? "Yes" : "No") << '\n';
  cout << "  Contains 'help'? " << (trie.search("help") ? "Yes" : "No") << '\n';
  cout << "  Contains 'hel'? " << (trie.search("hel") ? "Yes" : "No") << " (prefix only)\n";
  cout << "  Contains 'helper'? " << (trie.search("helper") ? "Yes" : "No") << '\n';

  // Duplicate insertion
  cout << "\nTrying to insert duplicate 'hello': ";
  size_t before_size = trie.size();
  trie.insert("hello");
  bool inserted = trie.size() > before_size;
  cout << (inserted ? "inserted" : "not inserted (correct behavior)") << '\n';
}

// Demo: Prefix operations
void demo_prefix_operations() {
  cout << "\n========== Demo: Prefix Operations ==========\n";

  TrieType trie;

  // Insert words with common prefixes
  vector<string> words = {"apple", "application", "apply", "apt", "banana", "band", "bandana"};
  for (const auto& word : words) {
    trie.insert(word);
  }

  print_trie_stats(trie, "trie");

  // Test starts_with
  cout << "\nTesting starts_with():\n";
  cout << "  Starts with 'app'? " << (trie.starts_with("app") ? "Yes" : "No") << '\n';
  cout << "  Starts with 'ban'? " << (trie.starts_with("ban") ? "Yes" : "No") << '\n';
  cout << "  Starts with 'cat'? " << (trie.starts_with("cat") ? "Yes" : "No") << '\n';
  cout << "  Starts with 'apple'? " << (trie.starts_with("apple") ? "Yes" : "No") << '\n';

  // Get words with prefix
  cout << "\nWords starting with 'app':\n  ";
  auto app_words = trie.get_all_words_with_prefix("app");
  for (const auto& word : app_words) {
    cout << word << " ";
  }
  cout << '\n';

  cout << "\nWords starting with 'ban':\n  ";
  auto ban_words = trie.get_all_words_with_prefix("ban");
  for (const auto& word : ban_words) {
    cout << word << " ";
  }
  cout << '\n';
}

// Demo: Autocomplete functionality
void demo_autocomplete() {
  cout << "\n========== Demo: Autocomplete ==========\n";

  TrieType trie;

  // Insert a vocabulary
  vector<string> vocabulary = {"car",   "card",     "care",   "careful",     "carefully", "careless",  "carpet", "carpool", "cat",
                               "catch", "category", "cattle", "celebration", "cell",      "cellphone", "center", "central"};

  for (const auto& word : vocabulary) {
    trie.insert(word);
  }

  cout << "Vocabulary loaded: " << trie.size() << " words\n";

  // Simulate autocomplete
  auto autocomplete = [&trie](const string& prefix) {
    cout << "\nAutocomplete for '" << prefix << "':\n";
    auto suggestions = trie.get_all_words_with_prefix(prefix);
    if (suggestions.empty()) {
      cout << "  (no suggestions)\n";
    } else {
      for (const auto& word : suggestions) {
        cout << "  - " << word << '\n';
      }
    }
    cout << "  (" << suggestions.size() << " suggestions)\n";
  };

  autocomplete("car");
  autocomplete("cat");
  autocomplete("cel");
  autocomplete("xyz");
}

// Demo: Word counting
void demo_word_counting() {
  cout << "\n========== Demo: Word Counting ==========\n";

  TrieType trie;

  vector<string> words = {"test", "testing", "tested", "tester", "tests", "the", "them", "their", "there", "these"};

  for (const auto& word : words) {
    trie.insert(word);
  }

  print_trie_stats(trie, "trie");

  // Count words with prefix
  cout << "\nCounting words with prefix:\n";
  cout << "  Words starting with 'test': " << trie.count_words_with_prefix("test") << '\n';
  cout << "  Words starting with 'the': " << trie.count_words_with_prefix("the") << '\n';
  cout << "  Words starting with 't': " << trie.count_words_with_prefix("t") << '\n';
  cout << "  Words starting with 'xyz': " << trie.count_words_with_prefix("xyz") << '\n';
}

// Demo: Remove operations
void demo_remove_operations() {
  cout << "\n========== Demo: Remove Operations ==========\n";

  TrieType trie;

  trie.insert("car");
  trie.insert("card");
  trie.insert("care");
  trie.insert("careful");

  print_trie_stats(trie, "original trie");

  // Remove a word that's a prefix of another
  cout << "\nRemoving 'car' (prefix of 'card', 'care', 'careful'):\n";
  trie.remove("car");
  cout << "  Contains 'car'? " << (trie.search("car") ? "Yes" : "No") << '\n';
  cout << "  Contains 'card'? " << (trie.search("card") ? "Yes" : "No") << '\n';
  cout << "  Starts with 'car'? " << (trie.starts_with("car") ? "Yes" : "No") << '\n';

  // Remove a longer word
  cout << "\nRemoving 'careful':\n";
  trie.remove("careful");
  cout << "  Contains 'careful'? " << (trie.search("careful") ? "Yes" : "No") << '\n';
  cout << "  Contains 'care'? " << (trie.search("care") ? "Yes" : "No") << '\n';

  print_trie_stats(trie, "after removals");
}

// Demo: Move semantics
void demo_move_semantics() {
  cout << "\n========== Demo: Move Semantics ==========\n";

  TrieType trie1;
  trie1.insert("hello");
  trie1.insert("world");
  trie1.insert("help");

  cout << "Original trie:\n";
  print_trie_stats(trie1, "trie1");

  // Move constructor
  TrieType trie2 = std::move(trie1);

  cout << "\nAfter move construction:\n";
  print_trie_stats(trie1, "trie1 (should be empty)");
  print_trie_stats(trie2, "trie2 (should have the data)");

  // Move assignment
  TrieType trie3;
  trie3.insert("test");

  cout << "\nBefore move assignment:\n";
  print_trie_stats(trie3, "trie3");

  trie3 = std::move(trie2);

  cout << "\nAfter move assignment:\n";
  print_trie_stats(trie2, "trie2 (should be empty)");
  print_trie_stats(trie3, "trie3 (should have trie2's data)");
}

// Demo: Performance with large dataset
void demo_performance() {
  cout << "\n========== Demo: Performance ==========\n";

  TrieType trie;

  const int N = 10000;

  cout << "Inserting " << N << " words...\n";

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < N; ++i) {
    trie.insert("word" + std::to_string(i));
  }

  auto end      = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "  Time: " << duration.count() << " ms\n";
  cout << "  Size: " << trie.size() << "\n";

  // Search performance
  cout << "\nSearching for all " << N << " words...\n";
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; ++i) {
    [[maybe_unused]] bool found = trie.search("word" + std::to_string(i));
  }
  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "  Time: " << duration.count() << " ms\n";

  // Prefix search performance
  cout << "\nSearching for words with prefix 'word1'...\n";
  start          = std::chrono::high_resolution_clock::now();
  auto word1_set = trie.get_all_words_with_prefix("word1");
  end            = std::chrono::high_resolution_clock::now();
  duration       = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "  Time: " << duration.count() << " ms\n";
  cout << "  Found: " << word1_set.size() << " words\n";
}

// Demo: Case sensitivity
void demo_case_sensitivity() {
  cout << "\n========== Demo: Case Sensitivity ==========\n";

  TrieType trie;

  trie.insert("Hello");
  trie.insert("hello");
  trie.insert("HELLO");

  cout << "Inserted: 'Hello', 'hello', 'HELLO'\n";
  cout << "  Size: " << trie.size() << " (each is stored separately)\n\n";

  cout << "Searching:\n";
  cout << "  Contains 'Hello'? " << (trie.search("Hello") ? "Yes" : "No") << '\n';
  cout << "  Contains 'hello'? " << (trie.search("hello") ? "Yes" : "No") << '\n';
  cout << "  Contains 'HELLO'? " << (trie.search("HELLO") ? "Yes" : "No") << '\n';
  cout << "  Contains 'HeLLo'? " << (trie.search("HeLLo") ? "Yes" : "No") << '\n';
}

// Demo: Edge cases
void demo_edge_cases() {
  cout << "\n========== Demo: Edge Cases ==========\n";

  TrieType trie;

  // Empty string
  cout << "Testing empty string:\n";
  try {
    trie.insert("");
    cout << "  Inserted empty string, size: " << trie.size() << '\n';
  } catch (const std::invalid_argument& e) {
    cout << "  Insertion rejected: " << e.what() << '\n';
  }
  cout << "  Contains ''? " << (trie.search("") ? "Yes" : "No") << '\n';

  // Single character words
  cout << "\nSingle character words:\n";
  trie.insert("a");
  trie.insert("b");
  trie.insert("c");
  cout << "  Inserted 'a', 'b', 'c', size: " << trie.size() << '\n';

  // Long word
  string long_word = "supercalifragilisticexpialidocious";
  cout << "\nLong word:\n";
  trie.insert(long_word);
  cout << "  Inserted '" << long_word << "'\n";
  cout << "  Contains it? " << (trie.search(long_word) ? "Yes" : "No") << '\n';

  // Clear and reuse
  cout << "\nClearing and reusing trie:\n";
  trie.clear();
  cout << "  After clear - Size: " << trie.size() << ", Empty: " << (trie.is_empty() ? "yes" : "no") << '\n';

  trie.insert("new");
  trie.insert("words");
  print_trie_stats(trie, "trie after reuse");
}

auto main() -> int {
  try {
    cout << "╔═══----------------------------------------------------═══╗\n";
    cout << "          TRIE (PREFIX TREE) - COMPREHENSIVE DEMO           \n";
    cout << "╚═══----------------------------------------------------═══╝\n";

    demo_basic_operations();
    demo_prefix_operations();
    demo_autocomplete();
    demo_word_counting();
    demo_remove_operations();
    demo_move_semantics();
    demo_performance();
    demo_case_sensitivity();
    demo_edge_cases();

    cout << "\n";
    cout << "╔═══----------------------------------------------------═══╗\n";
    cout << "             ALL DEMOS COMPLETED SUCCESSFULLY!              \n";
    cout << "╚═══----------------------------------------------------═══╝\n";

  } catch (const std::exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
