//===--------------------------------------------------------------------------===//
/**
 * @file Test_Trie.cpp
 * @brief Google Test unit tests for Trie implementation
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/trees/Trie.hpp"

using namespace ads::trees;

// Test fixture for Trie
class TrieTest : public ::testing::Test {
protected:
  Trie trie;
};

// ----- Basic State Tests ----- //

TEST_F(TrieTest, IsEmptyOnConstruction) {
  EXPECT_EQ(trie.size(), 0);
  EXPECT_TRUE(trie.is_empty());
}

TEST_F(TrieTest, Clear) {
  trie.insert("hello");
  trie.insert("world");
  EXPECT_FALSE(trie.is_empty());

  trie.clear();
  EXPECT_EQ(trie.size(), 0);
  EXPECT_TRUE(trie.is_empty());
}

// ----- Insertion Tests ----- //

TEST_F(TrieTest, InsertSingleWord) {
  EXPECT_TRUE(trie.insert("hello"));
  EXPECT_EQ(trie.size(), 1);
  EXPECT_TRUE(trie.contains("hello"));
}

TEST_F(TrieTest, InsertMultipleWords) {
  trie.insert("hello");
  trie.insert("world");
  trie.insert("help");

  EXPECT_EQ(trie.size(), 3);
  EXPECT_TRUE(trie.contains("hello"));
  EXPECT_TRUE(trie.contains("world"));
  EXPECT_TRUE(trie.contains("help"));
}

TEST_F(TrieTest, InsertDuplicateRejected) {
  trie.insert("hello");
  EXPECT_FALSE(trie.insert("hello"));
  EXPECT_EQ(trie.size(), 1);
}

TEST_F(TrieTest, InsertEmptyString) {
  EXPECT_TRUE(trie.insert(""));
  EXPECT_EQ(trie.size(), 1);
  EXPECT_TRUE(trie.contains(""));
}

TEST_F(TrieTest, InsertPrefixWords) {
  trie.insert("car");
  trie.insert("card");
  trie.insert("care");
  trie.insert("careful");

  EXPECT_EQ(trie.size(), 4);
  EXPECT_TRUE(trie.contains("car"));
  EXPECT_TRUE(trie.contains("card"));
  EXPECT_TRUE(trie.contains("care"));
  EXPECT_TRUE(trie.contains("careful"));
}

// ----- Search Tests ----- //

TEST_F(TrieTest, ContainsWord) {
  trie.insert("hello");
  trie.insert("help");
  trie.insert("heap");

  EXPECT_TRUE(trie.contains("hello"));
  EXPECT_TRUE(trie.contains("help"));
  EXPECT_TRUE(trie.contains("heap"));
  EXPECT_FALSE(trie.contains("hel")); // Prefix but not a word
  EXPECT_FALSE(trie.contains("helper")); // Not inserted
}

TEST_F(TrieTest, StartsWithPrefix) {
  trie.insert("hello");
  trie.insert("help");
  trie.insert("heap");
  trie.insert("world");

  EXPECT_TRUE(trie.starts_with("hel"));
  EXPECT_TRUE(trie.starts_with("he"));
  EXPECT_TRUE(trie.starts_with("h"));
  EXPECT_TRUE(trie.starts_with("hello"));
  EXPECT_TRUE(trie.starts_with("wor"));
  EXPECT_FALSE(trie.starts_with("hi"));
  EXPECT_FALSE(trie.starts_with("x"));
}

TEST_F(TrieTest, GetWordsWithPrefix) {
  trie.insert("car");
  trie.insert("card");
  trie.insert("care");
  trie.insert("careful");
  trie.insert("cat");
  trie.insert("dog");

  auto words = trie.get_words_with_prefix("car");
  EXPECT_EQ(words.size(), 4);

  // Check that all expected words are present
  std::vector<std::string> expected = {"car", "card", "care", "careful"};
  for (const auto& word : expected) {
    EXPECT_NE(std::find(words.begin(), words.end(), word), words.end());
  }
}

TEST_F(TrieTest, GetAllWords) {
  trie.insert("apple");
  trie.insert("banana");
  trie.insert("cherry");

  auto words = trie.get_all_words();
  EXPECT_EQ(words.size(), 3);
}

// ----- Removal Tests ----- //

TEST_F(TrieTest, RemoveWord) {
  trie.insert("hello");
  trie.insert("help");

  EXPECT_TRUE(trie.remove("hello"));
  EXPECT_EQ(trie.size(), 1);
  EXPECT_FALSE(trie.contains("hello"));
  EXPECT_TRUE(trie.contains("help"));
}

TEST_F(TrieTest, RemoveNonExistent) {
  trie.insert("hello");

  EXPECT_FALSE(trie.remove("world"));
  EXPECT_EQ(trie.size(), 1);
}

TEST_F(TrieTest, RemovePrefixWord) {
  trie.insert("car");
  trie.insert("card");

  // Remove the shorter word, longer should remain
  EXPECT_TRUE(trie.remove("car"));
  EXPECT_FALSE(trie.contains("car"));
  EXPECT_TRUE(trie.contains("card"));
  EXPECT_TRUE(trie.starts_with("car")); // Prefix still exists
}

TEST_F(TrieTest, RemoveLongerWord) {
  trie.insert("car");
  trie.insert("card");

  // Remove the longer word, shorter should remain
  EXPECT_TRUE(trie.remove("card"));
  EXPECT_TRUE(trie.contains("car"));
  EXPECT_FALSE(trie.contains("card"));
}

TEST_F(TrieTest, RemoveAll) {
  trie.insert("hello");
  trie.insert("world");
  trie.insert("help");

  trie.remove("hello");
  trie.remove("world");
  trie.remove("help");

  EXPECT_TRUE(trie.is_empty());
}

// ----- Move Semantics Tests ----- //

TEST_F(TrieTest, MoveConstructor) {
  trie.insert("hello");
  trie.insert("world");

  Trie moved_trie = std::move(trie);

  EXPECT_TRUE(trie.is_empty());
  EXPECT_EQ(moved_trie.size(), 2);
  EXPECT_TRUE(moved_trie.contains("hello"));
}

TEST_F(TrieTest, MoveAssignment) {
  trie.insert("hello");
  trie.insert("world");

  Trie other_trie;
  other_trie = std::move(trie);

  EXPECT_TRUE(trie.is_empty());
  EXPECT_EQ(other_trie.size(), 2);
}

// ----- Edge Cases ----- //

TEST_F(TrieTest, SingleCharacterWords) {
  trie.insert("a");
  trie.insert("b");
  trie.insert("c");

  EXPECT_EQ(trie.size(), 3);
  EXPECT_TRUE(trie.contains("a"));
  EXPECT_TRUE(trie.contains("b"));
  EXPECT_TRUE(trie.contains("c"));
}

TEST_F(TrieTest, LongWord) {
  std::string long_word = "supercalifragilisticexpialidocious";
  trie.insert(long_word);

  EXPECT_TRUE(trie.contains(long_word));
  EXPECT_TRUE(trie.starts_with("super"));
  EXPECT_TRUE(trie.starts_with("supercal"));
}

TEST_F(TrieTest, CaseSensitivity) {
  trie.insert("Hello");
  trie.insert("hello");

  EXPECT_EQ(trie.size(), 2);
  EXPECT_TRUE(trie.contains("Hello"));
  EXPECT_TRUE(trie.contains("hello"));
  EXPECT_FALSE(trie.contains("HELLO"));
}

// ----- Autocomplete Scenario Tests ----- //

TEST_F(TrieTest, AutocompleteSuggestions) {
  trie.insert("app");
  trie.insert("apple");
  trie.insert("application");
  trie.insert("apply");
  trie.insert("banana");

  auto suggestions = trie.get_words_with_prefix("app");
  EXPECT_EQ(suggestions.size(), 4);

  // Verify "banana" is not in suggestions
  EXPECT_EQ(std::find(suggestions.begin(), suggestions.end(), "banana"), suggestions.end());
}

TEST_F(TrieTest, NoSuggestionsForUnknownPrefix) {
  trie.insert("hello");
  trie.insert("help");

  auto suggestions = trie.get_words_with_prefix("xyz");
  EXPECT_TRUE(suggestions.empty());
}

// ----- Large Dataset Tests ----- //

TEST_F(TrieTest, LargeDataset) {
  const int N = 1000;
  for (int i = 0; i < N; ++i) {
    trie.insert("word" + std::to_string(i));
  }

  EXPECT_EQ(trie.size(), N);

  // Verify some random words
  EXPECT_TRUE(trie.contains("word0"));
  EXPECT_TRUE(trie.contains("word500"));
  EXPECT_TRUE(trie.contains("word999"));
}

TEST_F(TrieTest, PrefixSearchPerformance) {
  // Insert many words with common prefix
  for (int i = 0; i < 100; ++i) {
    trie.insert("prefix" + std::to_string(i));
  }

  auto words = trie.get_words_with_prefix("prefix");
  EXPECT_EQ(words.size(), 100);
}

// ----- Count Words With Prefix ----- //

TEST_F(TrieTest, CountWordsWithPrefix) {
  trie.insert("car");
  trie.insert("card");
  trie.insert("care");
  trie.insert("careful");
  trie.insert("cat");

  EXPECT_EQ(trie.count_words_with_prefix("car"), 4);
  EXPECT_EQ(trie.count_words_with_prefix("ca"), 5);
  EXPECT_EQ(trie.count_words_with_prefix("cat"), 1);
  EXPECT_EQ(trie.count_words_with_prefix("xyz"), 0);
}
