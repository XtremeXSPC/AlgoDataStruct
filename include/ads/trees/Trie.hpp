//===---------------------------------------------------------------------------===//
/**
 * @file Trie.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the Trie (Prefix Tree) class.
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef TRIE_HPP
#define TRIE_HPP

#include <algorithm>
#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace ads::trees {

/**
 * @brief Trie (Prefix Tree) data structure.
 *
 * @details A Trie (pronounced "try") is a tree-based data structure optimized for storing
 *          and searching strings. Each path from root to a node represents a prefix, and
 *          complete words are marked with a flag. Trie excels at prefix-based operations
 *          like autocomplete and spell checking.
 *
 *          Key Properties:
 *          - Search time: O(m) where m = string length (independent of # words)
 *          - Prefix sharing: Common prefixes stored once.
 *          - Space-efficient for large dictionaries with common prefixes.
 *
 *          Applications:
 *          - Autocomplete systems.
 *          - Spell checkers.
 *          - IP routing (longest prefix matching).
 *          - Text editors (word completion).
 *
 * @tparam UseMap If true, uses unordered_map for children (supports any char).
 *                If false, uses array for a-z lowercase only (faster but limited).
 *
 * @complexity
 * - Insert: O(m) time, O(m) space worst case (m = word length)
 * - Search: O(m) time, O(1) space
 * - StartsWith: O(m) time, O(1) space (m = prefix length)
 * - Remove: O(m) time, O(1) space
 * - GetAllWithPrefix: O(p + n*k) where p = prefix length, n = # results, k = avg word length
 */
template <bool UseMap = true>
class Trie {
public:
  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Construct an empty Trie.
   * @complexity Time O(1), Space O(1)
   */
  Trie();

  /**
   * @brief Move constructor.
   * @param other The trie from which to move resources.
   * @complexity Time O(1), Space O(1)
   */
  Trie(Trie&& other) noexcept;

  /**
   * @brief Destructor (automatic cleanup via unique_ptr).
   * @complexity Time O(n), Space O(1)
   */
  ~Trie() = default;

  /**
   * @brief Move assignment operator.
   * @param other The trie from which to move resources.
   * @return A reference to this instance.
   * @complexity Time O(n), Space O(1)
   */
  auto operator=(Trie&& other) noexcept -> Trie&;

  // Copy constructor and assignment are disabled (move-only type).
  Trie(const Trie&)                    = delete;
  auto operator=(const Trie&) -> Trie& = delete;

  //===------------------------- INSERTION OPERATIONS --------------------------===//

  /**
   * @brief Insert a word into the trie.
   *
   * @param word Word to insert (non-empty).
   * @complexity O(m) time, O(m) space worst case (m = word length)
   *
   * @throws std::invalid_argument if word is empty.
   *
   */
  void insert(const std::string& word);

  //===-------------------------- REMOVAL OPERATIONS ---------------------------===//

  /**
   * @brief Remove a word from the trie.
   *
   * Removes the word while preserving any other words that share prefixes.
   * Nodes are deleted only if they're not part of other words.
   *
   * @param word Word to remove.
   * @return true if word was found and removed.
   * @complexity O(m) time, O(1) space (m = word length)
   *
   */
  auto remove(const std::string& word) -> bool;

  /**
   * @brief Remove all words from trie.
   * @complexity O(n), Space O(1) where n = total nodes
   */
  void clear();

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

  /**
   * @brief Check if trie is empty.
   * @return true if no words stored.
   * @complexity O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const -> bool;

  /**
   * @brief Get number of words in trie.
   * @return Word count.
   * @complexity O(1), Space O(1)
   */
  [[nodiscard]] auto size() const -> size_t;

  /**
   * @brief Check if any word starts with given prefix.
   *
   * @param prefix Prefix to check.
   * @return true if at least one word has this prefix.
   * @complexity O(m) time, O(1) space (m = prefix length)
   *
   */
  [[nodiscard]] auto starts_with(const std::string& prefix) const -> bool;

  /**
   * @brief Get all words in trie (in lexicographic order for array version).
   * @return Vector of all words.
   * @complexity O(n*k) where n=word count, k=avg word length
   */
  [[nodiscard]] auto get_all_words() const -> std::vector<std::string>;

  /**
   * @brief Get all words that start with given prefix.
   *
   * @param prefix Prefix to match (empty prefix returns all words).
   * @return Vector of all words with the given prefix.
   * @complexity O(p + n*k) where p=prefix length, n=results, k=avg word length
   *
   */
  [[nodiscard]] auto get_all_words_with_prefix(const std::string& prefix) const -> std::vector<std::string>;

  /**
   * @brief Count words that start with given prefix.
   *
   * @param prefix Prefix to match.
   * @return Number of words with this prefix.
   * @complexity O(n) worst case (n = nodes in subtree)
   *
   */
  [[nodiscard]] auto count_words_with_prefix(const std::string& prefix) const -> size_t;

  /**
   * @brief Get longest common prefix of all words.
   *
   * @return Longest prefix shared by all words, or empty if trie is empty.
   * @complexity O(h) where h = height of common prefix path
   */
  [[nodiscard]] auto longest_common_prefix() const -> std::string;

  /**
   * @brief Search for an exact word.
   *
   * @param word Word to search for.
   * @return true if word exists in trie.
   * @complexity O(m) time, O(1) space (m = word length)
   *
   */
  [[nodiscard]] auto search(const std::string& word) const -> bool;

private:
  //===------------------------ INTERNAL NODE STRUCTURE ------------------------===//
  /**
   * @brief Internal node structure for Trie.
   *
   * Each node represents a character in the trie. The path from root to any
   * node forms a prefix. Nodes with is_end_of_word=true mark complete words.
   */
  struct TrieNode {
    // Children storage: map for flexibility, array for speed.
    std::conditional_t<UseMap, std::unordered_map<char, std::unique_ptr<TrieNode>>,
                       std::array<std::unique_ptr<TrieNode>, 26>>
        children;

    bool is_end_of_word; ///< True if this node marks end of a word.

    /** @brief Construct a new Trie Node. */
    TrieNode();

    /**
     * @brief Check if node has any children.
     * @return true if node has at least one child.
     * @complexity Time O(1) average for map version, O(AlphabetSize) for array version.
     */
    [[nodiscard]] auto has_children() const -> bool;
  };

  //===============================================================================//
  //===------------------------- PRIVATE HELPER METHODS ------------------------===//

  /**
   * @brief Get child index for array-based implementation.
   * @param c Character to convert.
   * @return Index 0-25 for 'a'-'z'.
   * @throws std::invalid_argument if character not in [a-z].
   */
  static auto char_to_index(char c) -> int;

  /**
   * @brief Get child node for a character.
   * @param node Current node.
   * @param c Character key.
   * @return Pointer to child node, or nullptr if not exists.
   */
  auto get_child(const TrieNode* node, char c) const -> TrieNode*;

  /**
   * @brief Create or get child node for a character.
   * @param node Current node.
   * @param c Character key.
   * @return Pointer to child node (creates if doesn't exist).
   */
  auto get_or_create_child(TrieNode* node, char c) -> TrieNode*;

  /**
   * @brief Navigate to node representing given prefix.
   * @param prefix Prefix string to find.
   * @return Pointer to node representing prefix, or nullptr if not found.
   */
  auto find_prefix_node(const std::string& prefix) const -> TrieNode*;

  /**
   * @brief DFS to collect all words from a node.
   * @param node Starting node.
   * @param current_word Prefix accumulated so far.
   * @param results Vector to store results.
   */
  void dfs_collect_words(const TrieNode* node, const std::string& current_word,
                         std::vector<std::string>& results) const;

  /**
   * @brief Recursive helper to count words with prefix.
   * @param node Current node.
   * @return Number of words in subtree.
   */
  [[nodiscard]] auto count_words_helper(const TrieNode* node) const -> size_t;

  /**
   * @brief Recursive helper to remove a word.
   * @param node Current node.
   * @param word Word to remove.
   * @param depth Current depth (character index in word).
   * @param found Output parameter: set to true if word was found and removed.
   * @return true if current node should be deleted.
   */
  auto remove_helper(TrieNode* node, const std::string& word, size_t depth, bool& found) -> bool;

  //===----------------------------- DATA MEMBERS ------------------------------===//

  std::unique_ptr<TrieNode> root_;       ///< Root node (empty prefix).
  size_t                    word_count_; ///< Number of words stored.
};

// Common type aliases.
using TrieMap   = Trie<true>;  ///< Trie with map storage (supports any char).
using TrieArray = Trie<false>; ///< Trie with array storage (lowercase a-z only).

} // namespace ads::trees

// Include the implementation file for templates.
#include "../../../src/ads/trees/Trie.tpp"

#endif // TRIE_HPP

//===---------------------------------------------------------------------------===//
