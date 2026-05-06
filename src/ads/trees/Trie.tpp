//===---------------------------------------------------------------------------===//
/**
 * @file Trie.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the Trie (Prefix Tree) class.
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/trees/Trie.hpp"

namespace ads::trees {

// DynamicArray stores flexible children without std::unordered_map; vector results remain public API.

//===--------------------------- NODE IMPLEMENTATION ---------------------------===//

template <bool UseMap>
Trie<UseMap>::TrieNode::TrieNode() : is_end_of_word(false) {
  // Array initialization is automatic (all nullptrs).
  // No need for explicit fill since unique_ptr defaults to nullptr.
}

template <bool UseMap>
auto Trie<UseMap>::TrieNode::has_children() const -> bool {
  if constexpr (UseMap) {
    return !children.is_empty();
  } else {
    return std::ranges::any_of(children, [](const auto& child) { return child != nullptr; });
  }
}

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

template <bool UseMap>
Trie<UseMap>::Trie() : root_(std::make_unique<TrieNode>()), word_count_(0) {
}

template <bool UseMap>
Trie<UseMap>::Trie(Trie&& other) noexcept : root_(std::move(other.root_)), word_count_(other.word_count_) {
  other.word_count_ = 0;
}

template <bool UseMap>
auto Trie<UseMap>::operator=(Trie&& other) noexcept -> Trie& {
  if (this != &other) {
    root_             = std::move(other.root_);
    word_count_       = other.word_count_;
    other.word_count_ = 0;
  }
  return *this;
}

//===-------------------------- INSERTION OPERATIONS ---------------------------===//

template <bool UseMap>
void Trie<UseMap>::insert(const std::string& word) {
  if (word.empty()) {
    throw std::invalid_argument("Cannot insert empty word");
  }

  if (!root_) {
    // Moved-from tries avoid allocation in noexcept moves; first mutation rebuilds the sentinel root.
    root_ = std::make_unique<TrieNode>();
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

//===--------------------------- REMOVAL OPERATIONS ----------------------------===//

template <bool UseMap>
void Trie<UseMap>::clear() {
  root_       = std::make_unique<TrieNode>();
  word_count_ = 0;
}

template <bool UseMap>
auto Trie<UseMap>::remove(const std::string& word) -> bool {
  if (word.empty() || !root_) {
    return false;
  }

  bool found = false;
  remove_helper(root_.get(), word, 0, found);
  return found;
}

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <bool UseMap>
auto Trie<UseMap>::is_empty() const -> bool {
  return word_count_ == 0;
}

template <bool UseMap>
auto Trie<UseMap>::size() const -> size_t {
  return word_count_;
}

template <bool UseMap>
auto Trie<UseMap>::starts_with(const std::string& prefix) const -> bool {
  if (prefix.empty()) {
    return true; // Empty prefix matches everything.
  }
  return find_prefix_node(prefix) != nullptr;
}

template <bool UseMap>
auto Trie<UseMap>::get_all_words() const -> std::vector<std::string> {
  return get_all_words_with_prefix("");
}

template <bool UseMap>
auto Trie<UseMap>::get_all_words_with_prefix(const std::string& prefix) const -> std::vector<std::string> {
  std::vector<std::string> results;

  const TrieNode* node = find_prefix_node(prefix);
  if (!node) {
    return results; // Prefix not found.
  }

  // DFS from this node to collect all words.
  dfs_collect_words(node, prefix, results);
  return results;
}

template <bool UseMap>
auto Trie<UseMap>::count_words_with_prefix(const std::string& prefix) const -> size_t {
  const TrieNode* node = find_prefix_node(prefix);
  if (!node) {
    return 0;
  }
  return count_words_helper(node);
}

template <bool UseMap>
auto Trie<UseMap>::longest_common_prefix() const -> std::string {
  if (!root_ || word_count_ == 0) {
    return "";
  }

  std::string     prefix;
  const TrieNode* node = root_.get();

  // Traverse while there's only one child and not end of word.
  while (node) {
    // Count children.
    int             child_count = 0;
    char            next_char   = '\0';
    const TrieNode* next_node   = nullptr;

    if constexpr (UseMap) {
      child_count = static_cast<int>(node->children.size());
      if (child_count == 1) {
        const auto& only_child = node->children[0];
        next_char              = only_child.character;
        next_node              = only_child.child.get();
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

    // Stop if more than one branch or at end of word.
    if (child_count != 1 || node->is_end_of_word) {
      break;
    }

    prefix += next_char;
    node = next_node;
  }

  return prefix;
}

template <bool UseMap>
auto Trie<UseMap>::search(const std::string& word) const -> bool {
  if (word.empty()) {
    return false;
  }

  const TrieNode* node = find_prefix_node(word);
  return node != nullptr && node->is_end_of_word;
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <bool UseMap>
auto Trie<UseMap>::char_to_index(char c) -> int {
  if (c < 'a' || c > 'z') {
    throw std::invalid_argument("Trie with array storage only supports lowercase a-z");
  }
  return c - 'a';
}

template <bool UseMap>
auto Trie<UseMap>::get_child(const TrieNode* node, char c) const -> const TrieNode* {
  if (node == nullptr) {
    return nullptr;
  }

  if constexpr (UseMap) {
    for (const auto& entry : node->children) {
      if (entry.character == c) {
        return entry.child.get();
      }
    }
    return nullptr;
  } else {
    int idx = char_to_index(c);
    return node->children[idx].get();
  }
}

template <bool UseMap>
auto Trie<UseMap>::get_child(TrieNode* node, char c) -> TrieNode* {
  if (node == nullptr) {
    return nullptr;
  }

  if constexpr (UseMap) {
    for (auto& entry : node->children) {
      if (entry.character == c) {
        return entry.child.get();
      }
    }
    return nullptr;
  } else {
    int idx = char_to_index(c);
    return node->children[idx].get();
  }
}

template <bool UseMap>
auto Trie<UseMap>::get_or_create_child(TrieNode* node, char c) -> TrieNode* {
  if constexpr (UseMap) {
    if (TrieNode* child = get_child(node, c); child != nullptr) {
      return child;
    }
    auto& entry = node->children.emplace_back(c, std::make_unique<TrieNode>());
    return entry.child.get();
  } else {
    int idx = char_to_index(c);
    if (!node->children[idx]) {
      node->children[idx] = std::make_unique<TrieNode>();
    }
    return node->children[idx].get();
  }
}

template <bool UseMap>
auto Trie<UseMap>::find_prefix_node(const std::string& prefix) const -> const TrieNode* {
  const TrieNode* node = root_.get();
  for (char c : prefix) {
    node = get_child(node, c);
    if (!node) {
      return nullptr;
    }
  }
  return node;
}

template <bool UseMap>
auto Trie<UseMap>::dfs_collect_words(
    const TrieNode* node, const std::string& current_word, std::vector<std::string>& results) const -> void {
  if (!node) {
    return;
  }

  // If this node marks end of word, add to results.
  if (node->is_end_of_word) {
    results.push_back(current_word);
  }

  // Recurse to all children.
  if constexpr (UseMap) {
    for (const auto& entry : node->children) {
      dfs_collect_words(entry.child.get(), current_word + entry.character, results);
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

template <bool UseMap>
auto Trie<UseMap>::count_words_helper(const TrieNode* node) const -> size_t {
  if (!node) {
    return 0;
  }

  size_t count = node->is_end_of_word ? 1 : 0;

  if constexpr (UseMap) {
    for (const auto& entry : node->children) {
      count += count_words_helper(entry.child.get());
    }
  } else {
    for (const auto& child : node->children) {
      count += count_words_helper(child.get());
    }
  }

  return count;
}

template <bool UseMap>
auto Trie<UseMap>::remove_helper(TrieNode* node, const std::string& word, size_t depth, bool& found) -> bool {
  if (!node) {
    return false;
  }

  // Reached end of word.
  if (depth == word.length()) {
    // Word found - unmark it.
    if (node->is_end_of_word) {
      node->is_end_of_word = false;
      word_count_--;
      found = true;
      // Delete node if it has no children (not prefix of other words).
      return !node->has_children();
    }
    return false;
  }

  // Recurse to child.
  char      c     = word[depth];
  TrieNode* child = get_child(node, c);

  // If character path doesn't exist, word not in trie.
  if (!child) {
    return false;
  }

  if (remove_helper(child, word, depth + 1, found)) {
    // Child should be deleted.
    if constexpr (UseMap) {
      for (size_t i = 0; i < node->children.size(); ++i) {
        if (node->children[i].character == c) {
          // Erase by index so the move-only child entry is removed without copying.
          node->children.erase(i);
          break;
        }
      }
    } else {
      node->children[char_to_index(c)].reset();
    }

    // Delete current node if:
    // - Not end of another word.
    // - Has no other children.
    return !node->is_end_of_word && !node->has_children();
  }

  return false;
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
