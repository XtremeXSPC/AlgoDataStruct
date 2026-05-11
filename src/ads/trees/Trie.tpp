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

  struct RemoveFrame {
    TrieNode* parent    = nullptr;
    char      character = '\0';
    TrieNode* child     = nullptr;

    RemoveFrame(TrieNode* frame_parent, char frame_character, TrieNode* frame_child) :
        parent(frame_parent),
        character(frame_character),
        child(frame_child) {}
  };

  ads::stacks::ArrayStack<RemoveFrame> path(word.size());
  TrieNode*                            node = root_.get();

  // Keep the search path explicitly so very long words do not consume call stack.
  for (char c : word) {
    TrieNode* child = get_child(node, c);
    if (!child) {
      return false;
    }
    path.emplace(node, c, child);
    node = child;
  }

  if (!node->is_end_of_word) {
    return false;
  }

  node->is_end_of_word = false;
  --word_count_;

  while (!path.is_empty() && !node->is_end_of_word && !node->has_children()) {
    const RemoveFrame frame  = path.top();
    TrieNode*         parent = frame.parent;

    if constexpr (UseMap) {
      for (size_t i = 0; i < parent->children.size(); ++i) {
        if (parent->children[i].character == frame.character) {
          // Erase by index so the move-only child entry is removed without copying.
          parent->children.erase(i);
          break;
        }
      }
    } else {
      parent->children[char_to_index(frame.character)].reset();
    }

    path.pop();
    node = parent;
  }

  return true;
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

  size_t                                   count = 0;
  ads::stacks::ArrayStack<const TrieNode*> stack(std::max<size_t>(word_count_, 1));
  stack.push(node);

  while (!stack.is_empty()) {
    const TrieNode* current = stack.top();
    stack.pop();

    if (current->is_end_of_word) {
      ++count;
    }

    if constexpr (UseMap) {
      for (const auto& entry : current->children) {
        stack.push(entry.child.get());
      }
    } else {
      for (const auto& child : current->children) {
        if (child) {
          stack.push(child.get());
        }
      }
    }
  }

  return count;
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

  struct TraversalFrame {
    const TrieNode* node          = nullptr;
    char            character     = '\0';
    bool            exit          = false;
    bool            has_character = false;

    TraversalFrame(const TrieNode* frame_node, char frame_character, bool is_exit, bool frame_has_character) :
        node(frame_node),
        character(frame_character),
        exit(is_exit),
        has_character(frame_has_character) {}
  };

  ads::stacks::ArrayStack<TraversalFrame> stack(std::max<size_t>(word_count_, 1));
  std::string                             path = current_word;
  stack.emplace(node, '\0', false, false);

  while (!stack.is_empty()) {
    const TraversalFrame frame = stack.top();
    stack.pop();

    if (frame.exit) {
      if (frame.has_character) {
        path.pop_back();
      }
      continue;
    }

    if (frame.has_character) {
      path.push_back(frame.character);
    }

    if (frame.node->is_end_of_word) {
      results.push_back(path);
    }

    stack.emplace(frame.node, frame.character, true, frame.has_character);

    if constexpr (UseMap) {
      const auto& children = frame.node->children;
      for (size_t i = children.size(); i > 0; --i) {
        const auto& entry = children[i - 1];
        // Reverse push order preserves the old recursive traversal order on a LIFO stack.
        stack.emplace(entry.child.get(), entry.character, false, true);
      }
    } else {
      for (size_t i = 26; i > 0; --i) {
        const size_t index = i - 1;
        if (frame.node->children[index]) {
          const char ch = static_cast<char>('a' + index);
          // Reverse push order preserves lexicographic traversal for array-backed tries.
          stack.emplace(frame.node->children[index].get(), ch, false, true);
        }
      }
    }
  }
}

} // namespace ads::trees

//===---------------------------------------------------------------------------===//
