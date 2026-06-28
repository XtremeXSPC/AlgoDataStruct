//===---------------------------------------------------------------------------===//
/**
 * @file Test_Tree_Concepts.cpp
 * @brief Compile-time checks for tree concepts and constrained APIs.
 * @version 0.1
 * @date 2026-02-07
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/trees/Complete_Binary_Tree.hpp"
#include "ads/trees/Tree_Concepts.hpp"
#include "ads/trees/range/Fenwick_Tree.hpp"
#include "ads/trees/search/AVL_Tree.hpp"
#include "ads/trees/search/B_Tree.hpp"
#include "ads/trees/search/Binary_Search_Tree.hpp"
#include "ads/trees/search/Red_Black_Tree.hpp"

#include <gtest/gtest.h>

#include <iterator>

using namespace ads::trees;

namespace {

struct LessAndEqualComparable {
  int value = 0;

  auto operator<(const LessAndEqualComparable& other) const -> bool { return value < other.value; }

  auto operator==(const LessAndEqualComparable& other) const -> bool { return value == other.value; }
};

struct OnlyLessComparable {
  int value = 0;

  auto operator<(const OnlyLessComparable& other) const -> bool { return value < other.value; }
};

struct OnlyEqualityComparable {
  int value = 0;

  auto operator==(const OnlyEqualityComparable& other) const -> bool { return value == other.value; }
};

struct NonDestructibleType {
  ~NonDestructibleType() = delete;
};

struct FenwickMissingOps {
  int value = 0;
};

struct MoveOnlyOrdered {
  int value = 0;

  explicit MoveOnlyOrdered(int v) : value(v) {}

  MoveOnlyOrdered(const MoveOnlyOrdered&)                        = delete;
  auto operator=(const MoveOnlyOrdered&) -> MoveOnlyOrdered&     = delete;
  MoveOnlyOrdered(MoveOnlyOrdered&&) noexcept                    = default;
  auto operator=(MoveOnlyOrdered&&) noexcept -> MoveOnlyOrdered& = default;

  auto operator<(const MoveOnlyOrdered& other) const -> bool { return value < other.value; }

  auto operator==(const MoveOnlyOrdered& other) const -> bool { return value == other.value; }
};

template <typename T> concept CanInstantiateBinarySearchTree = requires { typename BinarySearchTree<T>; };

template <typename T> concept CanInstantiateAVLTree = requires { typename AVLTree<T>; };

template <typename T> concept CanInstantiateRedBlackTree = requires { typename RedBlackTree<T>; };

template <typename T, int Degree> concept CanInstantiateBTree = requires { typename BTree<T, Degree>; };

template <typename T> concept CanInstantiateCompleteBinaryTree = requires { typename CompleteBinaryTree<T>; };

static_assert(TreeElement<int>);
static_assert(!TreeElement<NonDestructibleType>);

static_assert(EqualityComparableTreeElement<int>);
static_assert(!EqualityComparableTreeElement<OnlyLessComparable>);

static_assert(OrderedTreeElement<LessAndEqualComparable>);
static_assert(!OrderedTreeElement<OnlyLessComparable>);
static_assert(!OrderedTreeElement<OnlyEqualityComparable>);

static_assert(ValidBTreeDegree<2>);
static_assert(!ValidBTreeDegree<1>);

static_assert(FenwickElement<int>);
static_assert(!FenwickElement<FenwickMissingOps>);

static_assert(requires(BinarySearchTree<LessAndEqualComparable>& tree, const LessAndEqualComparable& value) {
  tree.insert(value);
  tree.remove(value);
});

static_assert(requires(AVLTree<LessAndEqualComparable>& tree, const LessAndEqualComparable& value) {
  tree.insert(value);
  tree.contains(value);
});

static_assert(requires(RedBlackTree<LessAndEqualComparable>& tree, const LessAndEqualComparable& value) {
  tree.insert(value);
  tree.contains(value);
});

static_assert(requires(BTree<LessAndEqualComparable, 3>& tree, const LessAndEqualComparable& value) {
  tree.insert(value);
  tree.contains(value);
});

static_assert(requires(CompleteBinaryTree<LessAndEqualComparable>& tree, const LessAndEqualComparable& value) {
  tree.insert(value);
  tree.contains(value);
});

static_assert(requires(FenwickTree<int>& tree) {
  tree.add(0, 1);
  tree.lower_bound(1);
});

static_assert(OrderedSearchTree<BinarySearchTree<LessAndEqualComparable>, LessAndEqualComparable>);
static_assert(OrderedSearchTree<AVLTree<LessAndEqualComparable>, LessAndEqualComparable>);
static_assert(OrderedSearchTree<RedBlackTree<LessAndEqualComparable>, LessAndEqualComparable>);

static_assert(std::forward_iterator<BinarySearchTree<LessAndEqualComparable>::iterator>);
static_assert(std::forward_iterator<AVLTree<LessAndEqualComparable>::iterator>);
static_assert(std::forward_iterator<RedBlackTree<LessAndEqualComparable>::iterator>);

static_assert(ValidatableOrderedSearchTree<BinarySearchTree<LessAndEqualComparable>, LessAndEqualComparable>);
static_assert(ValidatableOrderedSearchTree<AVLTree<LessAndEqualComparable>, LessAndEqualComparable>);
static_assert(ValidatableOrderedSearchTree<RedBlackTree<LessAndEqualComparable>, LessAndEqualComparable>);

static_assert(CopyInsertableOrderedSearchTree<BinarySearchTree<LessAndEqualComparable>, LessAndEqualComparable>);
static_assert(CopyInsertableOrderedSearchTree<AVLTree<LessAndEqualComparable>, LessAndEqualComparable>);
static_assert(CopyInsertableOrderedSearchTree<RedBlackTree<LessAndEqualComparable>, LessAndEqualComparable>);

static_assert(OrderedSearchTree<BinarySearchTree<MoveOnlyOrdered>, MoveOnlyOrdered>);
static_assert(OrderedSearchTree<AVLTree<MoveOnlyOrdered>, MoveOnlyOrdered>);
static_assert(OrderedSearchTree<RedBlackTree<MoveOnlyOrdered>, MoveOnlyOrdered>);

static_assert(!CopyInsertableOrderedSearchTree<BinarySearchTree<MoveOnlyOrdered>, MoveOnlyOrdered>);
static_assert(!CopyInsertableOrderedSearchTree<AVLTree<MoveOnlyOrdered>, MoveOnlyOrdered>);
static_assert(!CopyInsertableOrderedSearchTree<RedBlackTree<MoveOnlyOrdered>, MoveOnlyOrdered>);

static_assert(!CanInstantiateBinarySearchTree<OnlyEqualityComparable>);
static_assert(!CanInstantiateAVLTree<OnlyEqualityComparable>);
static_assert(!CanInstantiateRedBlackTree<OnlyEqualityComparable>);
static_assert(!CanInstantiateBTree<OnlyEqualityComparable, 3>);
static_assert(!CanInstantiateBTree<LessAndEqualComparable, 1>);
static_assert(!CanInstantiateCompleteBinaryTree<OnlyLessComparable>);

} // namespace

TEST(TreeConceptsTest, ConstraintsCompile) {
  SUCCEED();
}

//===---------------------------------------------------------------------------===//
