//===--------------------------------------------------------------------------===//
/**
 * @file Trie.hpp
 * @author Costantino Lombardi
 * @brief Trie (Prefix Tree) implementation for efficient string storage and retrieval
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * A Trie (pronounced "try") is a tree-based data structure optimized for storing
 * and searching strings. Each path from root to a node represents a prefix, and
 * complete words are marked with a flag. Trie excels at prefix-based operations
 * like autocomplete and spell checking.
 *
 * Key Properties:
 * - Search time: O(m) where m = string length (independent of # words)
 * - Prefix sharing: Common prefixes stored once
 * - Space-efficient for large dictionaries with common prefixes
 *
 * Applications:
 * - Autocomplete systems
 * - Spell checkers
 * - IP routing (longest prefix matching)
 * - Text editors (word completion)
 */
//===--------------------------------------------------------------------------===//
#pragma once

#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace ads::trees {

/**
 * @brief Trie (Prefix Tree) data structure
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
private:
  /**
   * @brief Internal node structure for Trie
   *
   * Each node represents a character in the trie. The path from root to any
   * node forms a prefix. Nodes with is_end_of_word=true mark complete words.
   */
  struct TrieNode {
    // Children storage: map for flexibility, array for speed
    std::conditional_t<UseMap, std::unordered_map<char, std::unique_ptr<TrieNode>>, std::array<std::unique_ptr<TrieNode>, 26>> children;

    bool is_end_of_word; ///< True if this node marks end of a word

    /**
     * @brief Construct a new Trie Node
     */
    TrieNode() : is_end_of_word(false) {
      // Array initialization is automatic (all nullptrs)
      // No need for explicit fill since unique_ptr defaults to nullptr
    }

    /**
     * @brief Check if node has any children
     * @return true if node has at least one child
     */
    [[nodiscard]] auto has_children() const -> bool {
      if constexpr (UseMap) {
        return !children.empty();
      } else {
        for (const auto& child : children) {
          if (child != nullptr) {
            return true;
          }
        }
        return false;
      }
    }
  };

  std::unique_ptr<TrieNode> root_;       ///< Root node (empty prefix)
  size_t                    word_count_; ///< Number of words stored

  /**
   * @brief Get child index for array-based implementation
   * @param c Character to convert
   * @return Index 0-25 for 'a'-'z'
   * @throws std::invalid_argument if character not in [a-z]
   */
  static auto char_to_index(char c) -> int {
    if (c < 'a' || c > 'z') {
      throw std::invalid_argument("Trie with array storage only supports lowercase a-z");
    }
    return c - 'a';
  }

  /**
   * @brief Get child node for a character
   * @param node Current node
   * @param c Character key
   * @return Pointer to child node, or nullptr if not exists
   */
  auto get_child(const TrieNode* node, char c) const -> TrieNode* {
    if constexpr (UseMap) {
      auto it = node->children.find(c);
      return (it != node->children.end()) ? it->second.get() : nullptr;
    } else {
      int idx = char_to_index(c);
      return node->children[idx].get();
    }
  }

  /**
   * @brief Create or get child node for a character
   * @param node Current node
   * @param c Character key
   * @return Reference to child node (creates if doesn't exist)
   */
  auto get_or_create_child(TrieNode* node, char c) -> TrieNode*& {
    if constexpr (UseMap) {
      auto& child_ptr = node->children[c];
      if (!child_ptr) {
        child_ptr = std::make_unique<TrieNode>();
      }
      return reinterpret_cast<TrieNode*&>(child_ptr);
    } else {
      int idx = char_to_index(c);
      if (!node->children[idx]) {
        node->children[idx] = std::make_unique<TrieNode>();
      }
      return reinterpret_cast<TrieNode*&>(node->children[idx]);
    }
  }

  /**
   * @brief Navigate to node representing given prefix
   * @param prefix Prefix string to find
   * @return Pointer to node representing prefix, or nullptr if not found
   */
  auto find_prefix_node(const std::string& prefix) const -> TrieNode* {
    TrieNode* node = root_.get();
    for (char c : prefix) {
      node = get_child(node, c);
      if (!node) {
        return nullptr;
      }
    }
    return node;
  }

  /**
   * @brief Recursive helper to remove a word
   * @param node Current node
   * @param word Word to remove
   * @param depth Current depth (character index in word)
   * @param found Output parameter: set to true if word was found and removed
   * @return true if current node should be deleted
   */
  auto remove_helper(TrieNode* node, const std::string& word, size_t depth, bool& found) -> bool {
    if (!node) {
      return false;
    }

    // Reached end of word
    if (depth == word.length()) {
      // Word found - unmark it
      if (node->is_end_of_word) {
        node->is_end_of_word = false;
        word_count_--;
        found = true;
        // Delete node if it has no children (not prefix of other words)
        return !node->has_children();
      }
      return false;
    }

    // Recurse to child
    char      c     = word[depth];
    TrieNode* child = get_child(node, c);

    // If character path doesn't exist, word not in trie
    if (!child) {
      return false;
    }

    if (remove_helper(child, word, depth + 1, found)) {
      // Child should be deleted
      if constexpr (UseMap) {
        node->children.erase(c);
      } else {
        node->children[char_to_index(c)].reset();
      }

      // Delete current node if:
      // - Not end of another word
      // - Has no other children
      return !node->is_end_of_word && !node->has_children();
    }

    return false;
  }

  /**
   * @brief DFS to collect all words from a node
   * @param node Starting node
   * @param current_word Prefix accumulated so far
   * @param results Vector to store results
   */
  void dfs_collect_words(const TrieNode* node, const std::string& current_word, std::vector<std::string>& results) const {
    if (!node) {
      return;
    }

    // If this node marks end of word, add to results
    if (node->is_end_of_word) {
      results.push_back(current_word);
    }

    // Recurse to all children
    if constexpr (UseMap) {
      for (const auto& [ch, child] : node->children) {
        dfs_collect_words(child.get(), current_word + ch, results);
      }
    } else {
      for (int i = 0; i < 26; ++i) {
        if (node->children[i]) {
          char ch = static_cast<char>('a' + i);
          dfs_collect_words(node->children[i].get(), current_word + ch, results);
        }
      }
    }
  }

  /**
   * @brief Recursive helper to count words with prefix
   * @param node Current node
   * @return Number of words in subtree
   */
  auto count_words_helper(const TrieNode* node) const -> size_t {
    if (!node) {
      return 0;
    }

    size_t count = node->is_end_of_word ? 1 : 0;

    if constexpr (UseMap) {
      for (const auto& [ch, child] : node->children) {
        count += count_words_helper(child.get());
      }
    } else {
      for (const auto& child : node->children) {
        count += count_words_helper(child.get());
      }
    }

    return count;
  }

public:
  /**
   * @brief Construct an empty Trie
   */
  Trie() : root_(std::make_unique<TrieNode>()), word_count_(0) {}

  /**
   * @brief Destructor (automatic cleanup via unique_ptr)
   */
  ~Trie() = default;

  // Disable copy (deep copy would be expensive)
  Trie(const Trie&)                    = delete;
  auto operator=(const Trie&) -> Trie& = delete;

  // Enable move
  Trie(Trie&& other) noexcept : root_(std::move(other.root_)), word_count_(other.word_count_) {
    other.root_       = std::make_unique<TrieNode>();
    other.word_count_ = 0;
  }
  auto operator=(Trie&& other) noexcept -> Trie& {
    if (this != &other) {
      root_       = std::move(other.root_);
      word_count_ = other.word_count_;
      other.root_ = std::make_unique<TrieNode>();
      other.word_count_ = 0;
    }
    return *this;
  }

  /**
   * @brief Insert a word into the trie
   *
   * @param word Word to insert (non-empty)
   * @complexity O(m) time, O(m) space worst case (m = word length)
   *
   * @throws std::invalid_argument if word is empty
   *
   * @example
   * ```cpp
   * Trie<> trie;
   * trie.insert("hello");
   * trie.insert("help");
   * ```
   */
  void insert(const std::string& word) {
    if (word.empty()) {
      throw std::invalid_argument("Cannot insert empty word");
    }

    TrieNode* node = root_.get();
    for (char c : word) {
      node = get_or_create_child(node, c);
    }

    // Mark end of word
    if (!node->is_end_of_word) {
      node->is_end_of_word = true;
      word_count_++;
    }
  }

  /**
   * @brief Search for an exact word
   *
   * @param word Word to search for
   * @return true if word exists in trie
   * @complexity O(m) time, O(1) space (m = word length)
   *
   * @example
   * ```cpp
   * trie.insert("hello");
   * trie.search("hello");  // true
   * trie.search("hell");   // false (not marked as word)
   * ```
   */
  [[nodiscard]] auto search(const std::string& word) const -> bool {
    if (word.empty()) {
      return false;
    }

    TrieNode* node = find_prefix_node(word);
    return node != nullptr && node->is_end_of_word;
  }

  /**
   * @brief Check if any word starts with given prefix
   *
   * @param prefix Prefix to check
   * @return true if at least one word has this prefix
   * @complexity O(m) time, O(1) space (m = prefix length)
   *
   * @example
   * ```cpp
   * trie.insert("hello");
   * trie.starts_with("hel");  // true
   * trie.starts_with("world"); // false
   * ```
   */
  [[nodiscard]] auto starts_with(const std::string& prefix) const -> bool {
    if (prefix.empty()) {
      return true; // Empty prefix matches everything
    }
    return find_prefix_node(prefix) != nullptr;
  }

  /**
   * @brief Remove a word from the trie
   *
   * Removes the word while preserving any other words that share prefixes.
   * Nodes are deleted only if they're not part of other words.
   *
   * @param word Word to remove
   * @return true if word was found and removed
   * @complexity O(m) time, O(1) space (m = word length)
   *
   * @example
   * ```cpp
   * trie.insert("car");
   * trie.insert("card");
   * trie.remove("car");    // true - removes "car", keeps "card"
   * trie.search("car");    // false
   * trie.search("card");   // true
   * ```
   */
  auto remove(const std::string& word) -> bool {
    if (word.empty() || !root_) {
      return false;
    }

    bool found = false;
    remove_helper(root_.get(), word, 0, found);
    return found;
  }

  /**
   * @brief Get all words that start with given prefix
   *
   * @param prefix Prefix to match (empty prefix returns all words)
   * @return Vector of all words with the given prefix
   * @complexity O(p + n*k) where p=prefix length, n=results, k=avg word length
   *
   * @example
   * ```cpp
   * trie.insert("car");
   * trie.insert("card");
   * trie.insert("cat");
   * auto words = trie.get_all_words_with_prefix("ca");
   * // words = ["car", "card", "cat"]
   * ```
   */
  [[nodiscard]] auto get_all_words_with_prefix(const std::string& prefix) const -> std::vector<std::string> {
    std::vector<std::string> results;

    TrieNode* node = find_prefix_node(prefix);
    if (!node) {
      return results; // Prefix not found
    }

    // DFS from this node to collect all words
    dfs_collect_words(node, prefix, results);
    return results;
  }

  /**
   * @brief Count words that start with given prefix
   *
   * @param prefix Prefix to match
   * @return Number of words with this prefix
   * @complexity O(n) worst case (n = nodes in subtree)
   *
   * @example
   * ```cpp
   * trie.count_words_with_prefix("ca");  // 3 (car, card, cat)
   * ```
   */
  [[nodiscard]] auto count_words_with_prefix(const std::string& prefix) const -> size_t {
    TrieNode* node = find_prefix_node(prefix);
    if (!node) {
      return 0;
    }
    return count_words_helper(node);
  }

  /**
   * @brief Get longest common prefix of all words
   *
   * @return Longest prefix shared by all words, or empty if trie is empty
   * @complexity O(h) where h = height of common prefix path
   *
   * @example
   * ```cpp
   * trie.insert("flower");
   * trie.insert("flow");
   * trie.insert("flight");
   * trie.longest_common_prefix();  // "fl"
   * ```
   */
  [[nodiscard]] auto longest_common_prefix() const -> std::string {
    if (!root_ || word_count_ == 0) {
      return "";
    }

    std::string prefix;
    TrieNode*   node = root_.get();

    // Traverse while there's only one child and not end of word
    while (node) {
      // Count children
      int       child_count = 0;
      char      next_char   = '\0';
      TrieNode* next_node   = nullptr;

      if constexpr (UseMap) {
        child_count = node->children.size();
        if (child_count == 1) {
          auto it   = node->children.begin();
          next_char = it->first;
          next_node = it->second.get();
        }
      } else {
        for (int i = 0; i < 26; ++i) {
          if (node->children[i]) {
            child_count++;
            next_char = static_cast<char>('a' + i);
            next_node = node->children[i].get();
            if (child_count > 1)
              break;
          }
        }
      }

      // Stop if more than one branch or at end of word
      if (child_count != 1 || node->is_end_of_word) {
        break;
      }

      prefix += next_char;
      node = next_node;
    }

    return prefix;
  }

  /**
   * @brief Get number of words in trie
   * @return Word count
   * @complexity O(1)
   */
  [[nodiscard]] auto size() const -> size_t { return word_count_; }

  /**
   * @brief Check if trie is empty
   * @return true if no words stored
   * @complexity O(1)
   */
  [[nodiscard]] auto is_empty() const -> bool { return word_count_ == 0; }

  /**
   * @brief Remove all words from trie
   * @complexity O(n) where n = total nodes
   */
  void clear() {
    root_       = std::make_unique<TrieNode>();
    word_count_ = 0;
  }

  /**
   * @brief Get all words in trie (in lexicographic order for array version)
   * @return Vector of all words
   * @complexity O(n*k) where n=word count, k=avg word length
   */
  [[nodiscard]] auto get_all_words() const -> std::vector<std::string> { return get_all_words_with_prefix(""); }
};

// Common type aliases
using TrieMap   = Trie<true>;  ///< Trie with map storage (supports any char)
using TrieArray = Trie<false>; ///< Trie with array storage (lowercase a-z only)

} // namespace ads::trees

//===--------------------------------------------------------------------------===//
