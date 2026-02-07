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

#include "../include/ads/trees/AVL_Tree.hpp"
#include "../include/ads/trees/B_Tree.hpp"
#include "../include/ads/trees/Binary_Search_Tree.hpp"
#include "../include/ads/trees/Complete_Binary_Tree.hpp"
#include "../include/ads/trees/Fenwick_Tree.hpp"
#include "../include/ads/trees/Red_Black_Tree.hpp"
#include "../include/ads/trees/Tree_Concepts.hpp"

#include <gtest/gtest.h>

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

template <typename T>
concept CanInstantiateBinarySearchTree = requires { typename BinarySearchTree<T>; };

template <typename T>
concept CanInstantiateAVLTree = requires { typename AVLTree<T>; };

template <typename T>
concept CanInstantiateRedBlackTree = requires { typename Red_Black_Tree<T>; };

template <typename T, int Degree>
concept CanInstantiateBTree = requires { typename B_Tree<T, Degree>; };

template <typename T>
concept CanInstantiateCompleteBinaryTree = requires { typename CompleteBinaryTree<T>; };

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

static_assert(requires(Red_Black_Tree<LessAndEqualComparable>& tree, const LessAndEqualComparable& value) {
  tree.insert(value);
  tree.contains(value);
});

static_assert(requires(B_Tree<LessAndEqualComparable, 3>& tree, const LessAndEqualComparable& value) {
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
