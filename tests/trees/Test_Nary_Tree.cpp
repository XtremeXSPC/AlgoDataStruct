//===---------------------------------------------------------------------------===//
/**
 * @file Test_Nary_Tree.cpp
 * @brief Google Test unit tests for NaryTree.
 * @version 0.1
 * @date 2026-06-19
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/trees/Nary_Tree.hpp"

#include <gtest/gtest.h>

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace ads::trees;

namespace {

struct MoveOnlyEq {
  int value = 0;

  MoveOnlyEq() = default;

  explicit MoveOnlyEq(int v) : value(v) {}

  MoveOnlyEq(MoveOnlyEq&&) noexcept                    = default;
  auto operator=(MoveOnlyEq&&) noexcept -> MoveOnlyEq& = default;
  MoveOnlyEq(const MoveOnlyEq&)                        = delete;
  auto operator=(const MoveOnlyEq&) -> MoveOnlyEq&     = delete;

  auto operator==(const MoveOnlyEq& other) const -> bool { return value == other.value; }
};

// Builds:
//          1
//        / | \
//       2  3  4
//      /|     |
//     5 6     7
auto build_sample(NaryTree<int>& tree) {
  auto* n1 = tree.set_root(1);
  auto* n2 = tree.add_child(n1, 2);
  tree.add_child(n1, 3);
  auto* n4 = tree.add_child(n1, 4);
  tree.add_child(n2, 5);
  tree.add_child(n2, 6);
  tree.add_child(n4, 7);
}

template <typename Fn>
auto collect(Fn traversal) -> std::vector<int> {
  std::vector<int> out;
  traversal([&out](const int& v) -> auto { out.push_back(v); });
  return out;
}

} // namespace

//===----- MOVE-ONLY CONFORMANCE -----------------------------------------------===//

static_assert(!std::is_copy_constructible_v<NaryTree<int>>);
static_assert(!std::is_copy_assignable_v<NaryTree<int>>);
static_assert(std::is_move_constructible_v<NaryTree<int>>);
static_assert(std::is_move_assignable_v<NaryTree<int>>);

//===----- TEST FIXTURE --------------------------------------------------------===//

class NaryTreeTest : public ::testing::Test {
protected:
  NaryTree<int> tree;
};

//===----- BASIC STATE TESTS ---------------------------------------------------===//

TEST_F(NaryTreeTest, EmptyOnConstruction) {
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0u);
  EXPECT_EQ(tree.height(), -1);
  EXPECT_EQ(tree.root(), nullptr);
  EXPECT_FALSE(tree.contains(1));
}

TEST_F(NaryTreeTest, SetRootCreatesSingleNode) {
  auto* r = tree.set_root(42);
  ASSERT_NE(r, nullptr);
  EXPECT_EQ(tree.root(), r);
  EXPECT_EQ(r->value(), 42);
  EXPECT_EQ(r->parent(), nullptr);
  EXPECT_TRUE(r->is_leaf());
  EXPECT_FALSE(tree.is_empty());
  EXPECT_EQ(tree.size(), 1u);
  EXPECT_EQ(tree.height(), 0);
}

TEST_F(NaryTreeTest, SetRootOnNonEmptyThrows) {
  tree.set_root(1);
  EXPECT_THROW((void)tree.set_root(2), ads::trees::InvalidOperationException);
  EXPECT_EQ(tree.size(), 1u);
}

TEST_F(NaryTreeTest, AddChildNullParentThrows) {
  EXPECT_THROW((void)tree.add_child(nullptr, 1), ads::trees::InvalidOperationException);
}

//===----- STRUCTURE TESTS -----------------------------------------------------===//

TEST_F(NaryTreeTest, ChildLinksAndCounts) {
  auto* root  = tree.set_root(1);
  auto* a     = tree.add_child(root, 2);
  auto* b     = tree.add_child(root, 3);
  auto* grand = tree.add_child(a, 4);

  EXPECT_EQ(tree.size(), 4u);
  EXPECT_EQ(root->child_count(), 2u);
  EXPECT_EQ(root->child(0), a);
  EXPECT_EQ(root->child(1), b);
  EXPECT_EQ(a->child_count(), 1u);
  EXPECT_EQ(a->child(0), grand);
  EXPECT_EQ(grand->parent(), a);
  EXPECT_EQ(a->parent(), root);
  EXPECT_TRUE(b->is_leaf());
  EXPECT_EQ(tree.height(), 2);
}

TEST_F(NaryTreeTest, ChildOutOfRangeThrows) {
  auto* root = tree.set_root(1);
  tree.add_child(root, 2);
  EXPECT_THROW((void)root->child(1), ads::trees::InvalidOperationException);
}

TEST_F(NaryTreeTest, MutateValueThroughHandle) {
  auto* root    = tree.set_root(1);
  root->value() = 99;
  EXPECT_EQ(tree.root()->value(), 99);
}

//===----- QUERY TESTS ---------------------------------------------------------===//

TEST_F(NaryTreeTest, ContainsSearchesWholeTree) {
  build_sample(tree);
  for (int v : {1, 2, 3, 4, 5, 6, 7}) {
    EXPECT_TRUE(tree.contains(v));
  }
  EXPECT_FALSE(tree.contains(8));
  EXPECT_FALSE(tree.contains(0));
}

//===----- TRAVERSAL TESTS -----------------------------------------------------===//

TEST_F(NaryTreeTest, PreOrderTraversal) {
  build_sample(tree);
  auto seq = collect([&](const auto& v) -> auto { tree.pre_order_traversal(v); });
  EXPECT_EQ(seq, (std::vector<int>{1, 2, 5, 6, 3, 4, 7}));
}

TEST_F(NaryTreeTest, PostOrderTraversal) {
  build_sample(tree);
  auto seq = collect([&](const auto& v) -> auto { tree.post_order_traversal(v); });
  EXPECT_EQ(seq, (std::vector<int>{5, 6, 2, 3, 7, 4, 1}));
}

TEST_F(NaryTreeTest, LevelOrderTraversal) {
  build_sample(tree);
  auto seq = collect([&](const auto& v) -> auto { tree.level_order_traversal(v); });
  EXPECT_EQ(seq, (std::vector<int>{1, 2, 3, 4, 5, 6, 7}));
}

//===----- EMPLACE TESTS -------------------------------------------------------===//

TEST(NaryTreeEmplace, EmplaceRootAndChildConstructInPlace) {
  NaryTree<std::string> tree;
  auto*                 root = tree.emplace_root(3, 'x');        // "xxx"
  auto*                 kid  = tree.emplace_child(root, 2, 'y'); // "yy"
  EXPECT_EQ(root->value(), "xxx");
  EXPECT_EQ(kid->value(), "yy");
  EXPECT_EQ(tree.size(), 2u);
  EXPECT_TRUE(tree.contains(std::string("yy")));
}

//===----- REMOVAL / MOVE TESTS ------------------------------------------------===//

TEST_F(NaryTreeTest, Clear) {
  build_sample(tree);
  tree.clear();
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0u);
  EXPECT_EQ(tree.height(), -1);
}

TEST_F(NaryTreeTest, MoveConstructionTransfersAndEmptiesSource) {
  build_sample(tree);
  NaryTree<int> moved(std::move(tree));
  EXPECT_EQ(moved.size(), 7u);
  EXPECT_TRUE(moved.contains(5));
  EXPECT_TRUE(tree.is_empty());
}

TEST_F(NaryTreeTest, MoveAssignmentTransfers) {
  build_sample(tree);
  NaryTree<int> other;
  other.set_root(100);
  other = std::move(tree);
  EXPECT_EQ(other.size(), 7u);
  EXPECT_TRUE(other.contains(7));
  EXPECT_FALSE(other.contains(100));
}

TEST(NaryTreeMoveOnly, HoldsMoveOnlyValues) {
  NaryTree<MoveOnlyEq> tree;
  auto*                root = tree.set_root(MoveOnlyEq(1));
  tree.add_child(root, MoveOnlyEq(2));
  tree.emplace_child(root, 3);
  EXPECT_EQ(tree.size(), 3u);
  EXPECT_TRUE(tree.contains(MoveOnlyEq(2)));
  EXPECT_TRUE(tree.contains(MoveOnlyEq(3)));
  EXPECT_FALSE(tree.contains(MoveOnlyEq(9)));
}

//===----- STRESS TEST ---------------------------------------------------------===//

TEST_F(NaryTreeTest, DeepChainTeardownAndHeight) {
  constexpr int kDepth = 5'000;
  auto*         node   = tree.set_root(0);
  for (int i = 1; i < kDepth; ++i) {
    node = tree.add_child(node, i);
  }
  EXPECT_EQ(tree.size(), static_cast<size_t>(kDepth));
  EXPECT_EQ(tree.height(), kDepth - 1);
  EXPECT_TRUE(tree.contains(kDepth - 1));

  size_t visited = 0;
  tree.pre_order_traversal([&visited](const int&) -> void { ++visited; });
  EXPECT_EQ(visited, static_cast<size_t>(kDepth));
  // Destructor must dismantle the deep chain iteratively (no stack overflow).
}

//===---------------------------------------------------------------------------===//
