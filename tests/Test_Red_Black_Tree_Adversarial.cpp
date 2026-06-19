//===---------------------------------------------------------------------------===//
/**
 * @file Test_RedBlackTree.cpp
 * @brief Google Test unit tests for Red-Black Tree implementation
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * @details Adversarial tests for Red-Black Tree fixup logic.
 *
 * These tests target the insert_fixup case structure in
 * src/ads/trees/Red_Black_Tree.tpp (lines 287-368). Each test asserts the
 * full Red-Black invariant via validate_properties() plus shape-specific
 * expectations that would fail if any single rotation were applied in the
 * wrong order or omitted.
 *
 * @NOTE on deletion: RedBlackTree<T>::remove now executes an actual RB delete
 * path with fixup logic. The final adversarial case below therefore checks the
 * post-delete structure produced by the current deterministic implementation,
 * rather than assuming a rebuild from the retained values.
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/trees/search/Red_Black_Tree.hpp"

#include <gtest/gtest.h>

#include <initializer_list>
#include <vector>

using namespace ads::trees;

namespace {

// Collects in-order values for structural verification.
auto in_order(const RedBlackTree<int>& tree) -> std::vector<int> {
  std::vector<int> out;
  tree.in_order_traversal([&](const int& v) { out.push_back(v); });
  return out;
}

// Collects pre-order values so tests can pin the root / rotation shape.
auto pre_order(const RedBlackTree<int>& tree) -> std::vector<int> {
  std::vector<int> out;
  tree.pre_order_traversal([&](const int& v) { out.push_back(v); });
  return out;
}

// Inserts each value and asserts every RB invariant after every insert.
// Catches implementations that produce a valid tree only at the final step.
void insert_and_validate_incrementally(RedBlackTree<int>& tree, std::initializer_list<int> values) {
  size_t expected_size = tree.size();
  for (int v : values) {
    ASSERT_TRUE(tree.insert(v)) << "duplicate insert of " << v;
    ++expected_size;
    ASSERT_EQ(tree.size(), expected_size) << "size mismatch after inserting " << v;
    ASSERT_TRUE(tree.validate_properties()) << "RB invariant broken after inserting " << v;
  }
}

} // namespace

//===---------------------------------------------------------------------------===//
// Adv1: Case-1 recolor chain that propagates upward and forces a
// Case-3 rotation at the root. Tests the termination condition of
// the fixup loop when 'node' becomes root_.get() after recoloring.
//
// Sequence: 30, 20, 40, 10, 5, 3, 2, 1
//
// Predicted final shape:
//
//              10(B)
//             /    \
//           3(R)   30(R)
//          /  \    /    \
//        2(B) 5(B) 20(B) 40(B)
//        /
//       1(R)
//
// Would fail under: a fixup that stops propagation after one recolor,
// or one that forgets to re-blacken the root.
//===---------------------------------------------------------------------------===//
TEST(RedBlackAdversarial, Adv1_Case1PropagatesToRootRotation) {
  RedBlackTree<int> tree;
  insert_and_validate_incrementally(tree, {30, 20, 40, 10, 5, 3, 2, 1});

  EXPECT_EQ(tree.size(), 8u);
  EXPECT_TRUE(tree.validate_properties());

  EXPECT_EQ(in_order(tree), (std::vector<int>{1, 2, 3, 5, 10, 20, 30, 40}));

  // After the cascading fixup, 10 must be the root (verifies the root
  // replacement path at Red_Black_Tree.tpp:319-321).
  const auto preorder = pre_order(tree);
  ASSERT_FALSE(preorder.empty());
  EXPECT_EQ(preorder.front(), 10);

  // Black-height of the root is 2 on every path: root(B) -> {one black
  // grandchild} -> NIL.
  EXPECT_EQ(tree.black_height(), 2);

  // Height bound: at most 2 * floor(log2(n + 1)) = 2 * 3 = 6.
  EXPECT_LE(tree.height(), 6);
}

//===---------------------------------------------------------------------------===//
// Adv2: Left-Right double rotation at the root.
//
// Sequence: 20, 10, 15
// Predicted: 15(B), L=10(R), R=20(R).
//
// A naive implementation that performs only Case 2 (left-rotate parent)
// leaves 15(R) parent of 10(R) with 20(B) as the root -- a valid BST
// but an RB violation (red-red edge). validate_properties() catches it.
//===---------------------------------------------------------------------------===//
TEST(RedBlackAdversarial, Adv2_LR_DoubleRotation_AtRoot) {
  RedBlackTree<int> tree;
  insert_and_validate_incrementally(tree, {20, 10, 15});

  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.size(), 3u);
  EXPECT_EQ(in_order(tree), (std::vector<int>{10, 15, 20}));

  // After LR, the originally inserted 15 must have become the root.
  const auto preorder = pre_order(tree);
  ASSERT_EQ(preorder.size(), 3u);
  EXPECT_EQ(preorder[0], 15);
  EXPECT_EQ(preorder[1], 10);
  EXPECT_EQ(preorder[2], 20);

  EXPECT_EQ(tree.height(), 1);
}

//===---------------------------------------------------------------------------===//
// Adv3: Right-Left double rotation at the root (mirror of Adv2).
//
// Sequence: 10, 20, 15
// Predicted: 15(B), L=10(R), R=20(R).
//
// Exercises the symmetric branch at Red_Black_Tree.tpp:329-362.
//===---------------------------------------------------------------------------===//
TEST(RedBlackAdversarial, Adv3_RL_DoubleRotation_AtRoot) {
  RedBlackTree<int> tree;
  insert_and_validate_incrementally(tree, {10, 20, 15});

  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.size(), 3u);
  EXPECT_EQ(in_order(tree), (std::vector<int>{10, 15, 20}));

  const auto preorder = pre_order(tree);
  ASSERT_EQ(preorder.size(), 3u);
  EXPECT_EQ(preorder[0], 15);
  EXPECT_EQ(preorder[1], 10);
  EXPECT_EQ(preorder[2], 20);

  EXPECT_EQ(tree.height(), 1);
}

//===---------------------------------------------------------------------------===//
// Adv4: Monotone ascending insertion of length 7.
//
// Exercises repeated single-left (RR) rotations interleaved with Case-1
// recoloring. Adversarial because a naive BST would produce a linear
// chain of height 6; any rebalance bug that suppresses either the
// rotation or the recolor breaks either the height bound or the
// black-height invariant.
//
// Predicted final shape (verified by trace):
//
//           2(B)
//         /      \
//       1(B)    4(R)
//              /     \
//            3(B)    6(B)
//                   /   \
//                 5(R)  7(R)
//===---------------------------------------------------------------------------===//
TEST(RedBlackAdversarial, Adv4_MonotoneAscendingMixedFixups) {
  RedBlackTree<int> tree;
  insert_and_validate_incrementally(tree, {1, 2, 3, 4, 5, 6, 7});

  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.size(), 7u);
  EXPECT_EQ(in_order(tree), (std::vector<int>{1, 2, 3, 4, 5, 6, 7}));

  // 2 must be the root after the sequence of RR-style rotations.
  const auto preorder = pre_order(tree);
  ASSERT_FALSE(preorder.empty());
  EXPECT_EQ(preorder.front(), 2);

  // Height bound: 2 * floor(log2(8)) = 6. Actual expected height is 3.
  EXPECT_LE(tree.height(), 6);
  EXPECT_EQ(tree.height(), 3);

  EXPECT_EQ(tree.black_height(), 2);
}

//===---------------------------------------------------------------------------===//
// Adv5: LR double rotation where the grandparent is NOT the root.
//
// Sequence: 50, 30, 70, 25, 27
//
// Exercises the "great_grandparent->left.get() == grandparent" branch
// at Red_Black_Tree.tpp:322-326 for BOTH rotations (Case 2's rotate_left
// and Case 3's rotate_right), which requires that parent pointers be
// correctly rewired by rotate_left BEFORE Case 3 reads node->parent.
//
// A subtle failure mode: if Case 2's rotation is accidentally performed
// via `root_ = rotate_left(root_)` (because the implementer forgot that
// 30 is not the root), the ownership pointer on 50 becomes dangling.
//
// Predicted final shape:
//
//        50(B)
//       /    \
//     27(B)  70(B)
//     /  \
//   25(R) 30(R)
//===---------------------------------------------------------------------------===//
TEST(RedBlackAdversarial, Adv5_LR_AtNonRootGrandparent) {
  RedBlackTree<int> tree;
  insert_and_validate_incrementally(tree, {50, 30, 70, 25, 27});

  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.size(), 5u);
  EXPECT_EQ(in_order(tree), (std::vector<int>{25, 27, 30, 50, 70}));

  // Pre-order pins the exact shape 50, 27, 25, 30, 70.
  const auto preorder = pre_order(tree);
  ASSERT_EQ(preorder.size(), 5u);
  EXPECT_EQ(preorder[0], 50);
  EXPECT_EQ(preorder[1], 27);
  EXPECT_EQ(preorder[2], 25);
  EXPECT_EQ(preorder[3], 30);
  EXPECT_EQ(preorder[4], 70);

  EXPECT_EQ(tree.height(), 2);
  EXPECT_EQ(tree.black_height(), 2);
}

//===---------------------------------------------------------------------------===//
// Adv6: Delete of a root node with two non-empty subtrees.
//
// Seed: 10, 5, 15, 3, 7, 12, 20. Then remove(10).
//
// This case verifies that deleting a root with two non-empty subtrees leaves
// the tree in a valid RB state and that the deterministic delete-fixup path
// produces the expected replacement root and child ordering for this exact
// sequence.
//
// Predicted final shape:
//
//        12(B)
//       /   \
//     5(B)   15(B)
//    /   \       \
//  3(R)  7(R)    20(R)
//===---------------------------------------------------------------------------===//
TEST(RedBlackAdversarial, Adv6_DeleteRootTwoChildren_RebuildCorrectness) {
  RedBlackTree<int> tree;
  insert_and_validate_incrementally(tree, {10, 5, 15, 3, 7, 12, 20});

  ASSERT_EQ(tree.size(), 7u);
  ASSERT_TRUE(tree.validate_properties());

  ASSERT_TRUE(tree.remove(10));

  // Post-delete invariants.
  EXPECT_TRUE(tree.validate_properties());
  EXPECT_EQ(tree.size(), 6u);
  EXPECT_FALSE(tree.contains(10));
  EXPECT_EQ(in_order(tree), (std::vector<int>{3, 5, 7, 12, 15, 20}));

  // Pre-order pins the deterministic delete-fixup shape for this sequence.
  const auto preorder = pre_order(tree);
  ASSERT_EQ(preorder.size(), 6u);
  EXPECT_EQ(preorder[0], 12);
  EXPECT_EQ(preorder[1], 5);
  EXPECT_EQ(preorder[2], 3);
  EXPECT_EQ(preorder[3], 7);
  EXPECT_EQ(preorder[4], 15);
  EXPECT_EQ(preorder[5], 20);

  // Idempotency: second remove of the same key must report false.
  EXPECT_FALSE(tree.remove(10));
  EXPECT_EQ(tree.size(), 6u);
  EXPECT_TRUE(tree.validate_properties());
}

//===---------------------------------------------------------------------------===//
