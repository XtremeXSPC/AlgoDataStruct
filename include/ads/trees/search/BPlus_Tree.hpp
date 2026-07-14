//===---------------------------------------------------------------------------===//
/**
 * @file BPlus_Tree.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the BPlusTree class.
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef BPLUS_TREE_HPP
#define BPLUS_TREE_HPP

#include "../../arrays/Dynamic_Array.hpp"
#include "../Tree_Concepts.hpp"
#include "../exceptions/Binary_Tree_Exception.hpp"

#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>

namespace ads::trees {

using ads::arrays::DynamicArray;

/**
 * @brief A B+ Tree: a block-oriented ordered map with linked leaves.
 *
 * @details A B+ Tree keeps every key-value record in its leaves and stores only
 *          separator keys in internal nodes, which act purely as routers. All
 *          leaves lie at the same depth and are chained left to right through a
 *          @c next pointer, so an ordered range scan is a single descent followed
 *          by a linear walk of the leaf chain.
 *
 *          With minimum degree @c t, every node holds between @c t-1 and @c 2t-1
 *          keys (the root is exempt from the lower bound). Insertion splits full
 *          nodes proactively on the way down: a full leaf yields a copied-up
 *          separator, while a full internal node pushes its median up as in a
 *          classic B-Tree. Deletion is likewise top-down, refilling any child that
 *          sits at the minimum before descending into it, so a leaf never
 *          underflows when its record is finally erased.
 *
 *          Separators satisfy the loose invariant
 *          @c max(child[i]) < keys[i] <= min(child[i+1]); a separator therefore
 *          need not be a live key, which lets a leaf lose its minimum without
 *          rewriting ancestor separators.
 *
 *          Keys are unique according to ordering equivalence. Comparisons and
 *          relocations are non-throwing so structural commits cannot leave a
 *          partially updated node; copies may throw and are prepared before a
 *          split mutates the tree. Floating-point NaN keys are rejected.
 *
 *          The tree itself is move-only. Values must support non-throwing moves
 *          because records shift between leaf blocks during splits and merges.
 *
 * @tparam Key Ordered key type.
 * @tparam Value Mapped value stored alongside each key in the leaves.
 * @tparam MinDegree Minimum degree @c t (>= 2); a node holds up to @c 2t-1 keys.
 */
template <BPlusKey Key, BPlusValue Value, int MinDegree = 3>
requires ValidBTreeDegree<MinDegree>
class BPlusTree {
private:
  struct Node;

public:
  //===----- PUBLIC VALUE TYPES ------------------------------------------------===//

  /// @brief A stored key together with its mapped value (the leaf payload).
  struct Entry {
    Key   key;
    Value value;
  };

  ///@brief Type aliases for convenience.
  using key_type     = Key;
  using mapped_type  = Value;
  using value_type   = Entry;
  using size_type    = std::size_t;
  using visitor_type = std::function<void(const Key&, const Value&)>;

  //===----- ITERATOR CLASS ----------------------------------------------------===//
  /**
   * @brief Forward iterator over the leaf chain, in ascending key order.
   *
   * @details Dereferences to @c const @c Entry&. Advancing steps within a leaf's
   *          records and then follows the @c next pointer to the successor leaf.
   */
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Entry;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const Entry*;
    using reference         = const Entry&;

    ///@brief Default constructor for iterator (past-the-end).
    iterator() = default;

    ///@brief Returns a const reference to the current entry.
    auto operator*() const -> reference;

    ///@brief Returns a const pointer to the current entry.
    auto operator->() const -> pointer;

    ///@brief Advances to the next entry in ascending key order (pre-increment).
    auto operator++() -> iterator&;

    ///@brief Advances to the next entry in ascending key order (post-increment).
    auto operator++(int) -> iterator;

    ///@brief Returns true if both iterators reference the same position.
    auto operator==(const iterator& other) const -> bool;

  private:
    friend class BPlusTree;

    iterator(const Node* leaf, size_type index) : leaf_(leaf), index_(index) {}

    const Node* leaf_  = nullptr;
    size_type   index_ = 0;
  };

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty B+ tree.
   * @complexity Time O(1), Space O(1)
   */
  BPlusTree() noexcept;

  /**
   * @brief Builds a B+ tree from a set of entries.
   * @param entries Entries to insert; duplicate keys after the first are ignored.
   * @complexity Time O(nt log_t n), Space O(n)
   */
  explicit BPlusTree(const DynamicArray<Entry>& entries) requires std::copy_constructible<Value>;

  /**
   * @brief Builds a B+ tree from an iterator range of entries.
   * @tparam InputIt Input iterator whose reference type constructs an @c Entry.
   * @param first Beginning of the range.
   * @param last End of the range.
   * @complexity Time O(nt log_t n), Space O(n)
   */
  template <std::input_iterator InputIt>
  requires std::constructible_from<Entry, std::iter_reference_t<InputIt>>
  BPlusTree(InputIt first, InputIt last);

  /**
   * @brief Move constructor.
   * @param other Tree to move from; left empty.
   * @complexity Time O(1), Space O(1)
   */
  BPlusTree(BPlusTree&& other) noexcept;

  /**
   * @brief Destructor. Removes all nodes.
   * @complexity Time O(n), Space O(log n)
   */
  ~BPlusTree() = default;

  /**
   * @brief Move assignment operator.
   * @param other Tree to move from; left empty.
   * @return Reference to this tree.
   * @complexity Time O(n), Space O(log n)
   */
  auto operator=(BPlusTree&& other) noexcept -> BPlusTree&;

  BPlusTree(const BPlusTree&)                    = delete;
  auto operator=(const BPlusTree&) -> BPlusTree& = delete;

  //===----- MUTATION OPERATIONS -----------------------------------------------===//

  /**
   * @brief Inserts a key-value record (copy).
   * @param key Key to insert.
   * @param value Value copied into the new record.
   * @return true if inserted, false if the key already exists.
   * @throws BinaryTreeException if @p key is NaN.
   * @complexity Time O(t log_t n), Space O(log_t n)
   */
  auto insert(const Key& key, const Value& value) -> bool requires std::copy_constructible<Value>;

  /**
   * @brief Inserts a key-value record (move).
   * @param key Key to insert.
   * @param value Value moved into the new record.
   * @return true if inserted, false if the key already exists.
   * @throws BinaryTreeException if @p key is NaN.
   * @complexity Time O(t log_t n), Space O(log_t n)
   */
  auto insert(const Key& key, Value&& value) -> bool requires std::move_constructible<Value>;

  /**
   * @brief Removes the record with the given key.
   * @param key Key to remove.
   * @return true if a record was found and removed.
   * @throws BinaryTreeException if @p key is NaN.
   * @complexity Time O(t log_t n), Space O(log_t n)
   */
  auto remove(const Key& key) -> bool;

  /**
   * @brief Removes all nodes from the tree.
   * @complexity Time O(n), Space O(log n)
   */
  auto clear() noexcept -> void;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Checks whether the tree is empty.
   * @return true if empty.
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Returns the number of stored records.
   * @return Record count.
   */
  [[nodiscard]] auto size() const noexcept -> size_type;

  /**
   * @brief Returns the height of the tree (leaf-only tree = 0).
   * @return Number of edges from the root to any leaf.
   * @complexity Time O(log_t n), Space O(1)
   */
  [[nodiscard]] auto height() const noexcept -> int;

  /**
   * @brief Tests whether a key is stored.
   * @param key Key to search for.
   * @return true if a matching record exists.
   * @throws BinaryTreeException if @p key is NaN.
   * @complexity Time O(t log_t n), Space O(1)
   */
  [[nodiscard]] auto contains(const Key& key) const -> bool;

  /**
   * @brief Finds the value mapped to a key.
   * @param key Key to search for.
   * @return Pointer to the mapped value, or nullptr if the key is absent.
   * @throws BinaryTreeException if @p key is NaN.
   * @complexity Time O(t log_t n), Space O(1)
   */
  [[nodiscard]] auto find(const Key& key) const -> const Value*;

  /**
   * @brief Returns the record with the smallest key.
   * @return Const reference to the minimum entry.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity Time O(log_t n), Space O(1)
   */
  [[nodiscard]] auto find_min() const -> const Entry&;

  /**
   * @brief Returns the record with the largest key.
   * @return Const reference to the maximum entry.
   * @throws EmptyTreeException if the tree is empty.
   * @complexity Time O(log_t n), Space O(1)
   */
  [[nodiscard]] auto find_max() const -> const Entry&;

  /**
   * @brief Visits every record whose key lies in the inclusive range [low, high].
   * @param low Inclusive lower bound.
   * @param high Inclusive upper bound.
   * @param visit Callback invoked with each key and value, in ascending order.
   * @throws BinaryTreeException if either bound is NaN.
   * @complexity Time O(t log_t n + t + m), where m is the number of reported records;
   *             Space O(1)
   */
  auto range(const Key& low, const Key& high, const visitor_type& visit) const -> void;

  //===----- TRAVERSAL OPERATIONS ----------------------------------------------===//

  /**
   * @brief Visits every record in ascending key order via the leaf chain.
   * @param visit Callback invoked with each key and value.
   * @complexity Time O(n), Space O(1)
   */
  auto in_order_traversal(const visitor_type& visit) const -> void;

  //===----- VALIDATION --------------------------------------------------------===//

  /**
   * @brief Checks the B+ tree structural, ordering, and leaf-chain invariants.
   * @details Primarily for testing: verifies key-count bounds, sorted keys, equal
   *          leaf depth, child/key arity, separator bounds, and that the leaf chain
   *          yields all @ref size records in strictly ascending order.
   * @return true if every invariant holds.
   * @complexity Time O(n), Space O(log_t n)
   */
  [[nodiscard]] auto validate_properties() const -> bool;

  //===----- B+ TREE SPECIFIC OPERATIONS ---------------------------------------===//

  ///@brief Returns the minimum degree t. Complexity: O(1)
  static constexpr auto get_min_degree() -> int;

  ///@brief Returns the maximum number of keys per node (2t-1). Complexity: O(1)
  static constexpr auto get_max_keys() -> int;

  ///@brief Returns the minimum number of keys per non-root node (t-1). Complexity: O(1)
  static constexpr auto get_min_keys() -> int;

  //===----- ITERATOR OPERATIONS -----------------------------------------------===//

  ///@brief Returns an iterator to the smallest record. Complexity: O(log_t n)
  auto begin() const -> iterator;

  ///@brief Returns a past-the-end iterator. Complexity: O(1)
  auto end() const -> iterator;

  ///@brief Returns an iterator to the smallest record. Complexity: O(log_t n)
  auto cbegin() const -> iterator;

  ///@brief Returns a past-the-end iterator. Complexity: O(1)
  auto cend() const -> iterator;

private:
  //===----- CONSTANTS ---------------------------------------------------------===//

  static_assert(MinDegree >= 2, "Minimum degree must be at least 2");

  static constexpr int t        = MinDegree;
  static constexpr int MAX_KEYS = 2 * t - 1;
  static constexpr int MIN_KEYS = t - 1;

  //===----- INTERNAL NODE STRUCTURE -------------------------------------------===//
  /**
   * @brief A node that is either an internal router or a leaf record block.
   *
   * @details Internal nodes use @c keys (separators) and @c children; leaves use
   *          @c entries and the @c next chain pointer. @c next is non-owning: node
   *          ownership always flows through @c children and the tree root.
   */
  struct Node {
    bool                                is_leaf;
    DynamicArray<Key>                   keys;           ///< Separators (internal nodes).
    DynamicArray<std::unique_ptr<Node>> children;       ///< Child pointers (internal nodes).
    DynamicArray<Entry>                 entries;        ///< Records (leaf nodes).
    Node*                               next = nullptr; ///< Next leaf in ascending order.

    explicit Node(bool leaf);
  };

  ///@brief Prepared result of splitting one full node.
  struct SplitResult {
    Key                   separator;
    std::unique_ptr<Node> right;
  };

  std::unique_ptr<Node> root_; ///< Root node.
  size_type             size_; ///< Number of stored records.

  //===----- NAVIGATION HELPERS ------------------------------------------------===//

  ///@brief Returns the number of keys/records held by @p node.
  [[nodiscard]] static auto key_count(const Node* node) noexcept -> int;

  ///@brief Returns the child index to descend into for @p key.
  [[nodiscard]] static auto child_index(const Node* node, const Key& key) noexcept -> int;

  ///@brief Returns true when two keys are equivalent under the ordering relation.
  [[nodiscard]] static auto keys_equal(const Key& lhs, const Key& rhs) noexcept -> bool;

  ///@brief Returns whether @p key belongs to the supported ordered domain.
  [[nodiscard]] static auto is_valid_key(const Key& key) noexcept -> bool;

  ///@brief Throws when @p key does not belong to the supported ordered domain.
  static auto validate_key(const Key& key) -> void;

  ///@brief Returns the leftmost leaf, or nullptr if the tree is empty.
  [[nodiscard]] auto leftmost_leaf() const noexcept -> const Node*;

  //===----- INSERTION HELPERS -------------------------------------------------===//

  ///@brief Shared entry point for copy and move insertion.
  template <typename V>
  auto insert_impl(const Key& key, V&& value) -> bool;

  ///@brief Splits the full child at @p index of @p parent (leaf or internal).
  void split_child(Node* parent, int index);

  ///@brief Prepares and commits a split while @p node remains owned by its caller.
  [[nodiscard]] auto split_node(Node* node) -> SplitResult;

  ///@brief Inserts into a subtree guaranteed to be non-full.
  template <typename V>
  auto insert_non_full(Node* node, const Key& key, V&& value) -> bool;

  //===----- REMOVAL HELPERS ---------------------------------------------------===//

  ///@brief Removes @p key from the subtree rooted at @p node.
  auto remove_from_node(Node* node, const Key& key) -> bool;

  ///@brief Ensures child @p index can spare a key, by borrowing or merging.
  void fill_child(Node* node, int index);

  ///@brief Borrows one key from the left sibling of child @p index.
  void borrow_from_previous(Node* node, int index);

  ///@brief Borrows one key from the right sibling of child @p index.
  void borrow_from_next(Node* node, int index);

  ///@brief Merges child @p index with its right sibling.
  void merge_children(Node* node, int index);

  //===----- VALIDATION HELPER -------------------------------------------------===//

  ///@brief Recursive structural validation helper.
  [[nodiscard]] auto
  validate_helper(const Node* node, bool is_root, int level, const Key* lower_bound, const Key* upper_bound, int& leaf_level) const -> bool;
};

} // namespace ads::trees

#include "../../../../src/ads/trees/search/BPlus_Tree.tpp"

#endif // BPLUS_TREE_HPP

//===---------------------------------------------------------------------------===//
