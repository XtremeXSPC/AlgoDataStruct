//===---------------------------------------------------------------------------===//
/**
 * @file Test_Skew_Heap.cpp
 * @brief Google Test unit tests for the SkewHeap meldable heap.
 * @version 0.1
 * @date 2026-07-06
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "Meldable_Heap_Test_Support.hpp"
#include "ads/heaps/Skew_Heap.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <queue>
#include <random>
#include <string>
#include <vector>

using namespace ads::heaps;

namespace ads::heaps::detail {

// White-box inspector (friend of SkewHeap): a skew heap keeps no balance invariant,
// so the only structural property to verify is heap order over the entire internal
// tree. This complements the behavioral oracle tests.
template <typename Heap>
struct SkewHeapInspector {
  using NodePtr = typename Heap::NodePtr;

  static auto count(const NodePtr& node) -> std::size_t { return node ? 1 + count(node->left) + count(node->right) : 0; }

  static auto check(const Heap& heap, const NodePtr& node) -> bool {
    if (!node) {
      return true;
    }
    // Heap order: a parent must never rank below a child under the comparator.
    if (node->left && heap.comp_(node->value, node->left->value)) {
      return false;
    }
    if (node->right && heap.comp_(node->value, node->right->value)) {
      return false;
    }
    return check(heap, node->left) && check(heap, node->right);
  }

  static auto valid(const Heap& heap) -> bool { return check(heap, heap.root_); }

  static auto size_matches(const Heap& heap) -> bool { return count(heap.root_) == heap.size_; }
};

} // namespace ads::heaps::detail

namespace skew_test = ads::heaps::meldable_test;

//===----- CONCEPT & TYPE-TRAIT CHECKS -----------------------------------------===//

static_assert(MeldableHeap<MaxSkewHeap<int>>);
static_assert(MeldableHeap<MinSkewHeap<int>>);
static_assert(MeldableHeap<SkewHeap<std::string>>);
static_assert(MeldableHeap<SkewHeap<skew_test::MoveOnlyOrdered>>);

static_assert(std::move_constructible<MaxSkewHeap<int>>);
static_assert(!std::copy_constructible<MaxSkewHeap<int>>);
static_assert(!std::is_copy_assignable_v<MaxSkewHeap<int>>);

//===----- BASIC STATE ---------------------------------------------------------===//

TEST(SkewHeapBasic, EmptyOnConstruction) {
  MaxSkewHeap<int> heap;
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
}

TEST(SkewHeapBasic, InsertUpdatesTopAndSize) {
  MaxSkewHeap<int> heap;
  heap.insert(30);
  EXPECT_EQ(heap.top(), 30);
  heap.insert(50);
  EXPECT_EQ(heap.top(), 50);
  heap.insert(40);
  EXPECT_EQ(heap.top(), 50);
  EXPECT_EQ(heap.size(), 3U);
  EXPECT_FALSE(heap.is_empty());
}

TEST(SkewHeapBasic, MinHeapTopIsMinimum) {
  MinSkewHeap<int> heap;
  heap.insert(30);
  heap.insert(10);
  heap.insert(20);
  EXPECT_EQ(heap.top(), 10);
}

TEST(SkewHeapBasic, ClearEmptiesHeap) {
  MaxSkewHeap<int> heap{1, 2, 3, 4, 5};
  heap.clear();
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
  heap.insert(7); // usable after clear
  EXPECT_EQ(heap.top(), 7);
}

//===----- CONSTRUCTION --------------------------------------------------------===//

TEST(SkewHeapConstruction, InitializerList) {
  skew_test::expect_drains_in_order(MaxSkewHeap<int>{3, 1, 4, 1, 5, 9, 2, 6}, {3, 1, 4, 1, 5, 9, 2, 6}, std::greater<int>{});
}

TEST(SkewHeapConstruction, FromVector) {
  std::vector<int> src{7, 2, 9, 4, 4, 1};
  skew_test::expect_drains_in_order(MinSkewHeap<int>(src), src, std::less<int>{});
}

TEST(SkewHeapConstruction, FromIteratorRange) {
  std::vector<int> src{7, 2, 9, 4, 4, 1};
  skew_test::expect_drains_in_order(MaxSkewHeap<int>(src.begin(), src.end()), src, std::greater<int>{});
}

TEST(SkewHeapConstruction, MoveConstructorTransfersAndEmptiesSource) {
  MaxSkewHeap<int> src{5, 1, 8, 3};
  MaxSkewHeap<int> dst(std::move(src));
  EXPECT_TRUE(src.is_empty());
  EXPECT_EQ(src.size(), 0U);
  skew_test::expect_drains_in_order(std::move(dst), {5, 1, 8, 3}, std::greater<int>{});
}

TEST(SkewHeapConstruction, MoveAssignmentReplacesTarget) {
  MaxSkewHeap<int> src{5, 1, 8, 3};
  MaxSkewHeap<int> dst{100, 200}; // contents must be discarded
  dst = std::move(src);
  EXPECT_TRUE(src.is_empty());
  skew_test::expect_drains_in_order(std::move(dst), {5, 1, 8, 3}, std::greater<int>{});
}

//===----- EMPLACE -------------------------------------------------------------===//

TEST(SkewHeapEmplace, ConstructsInPlaceAndReturnsReference) {
  MaxSkewHeap<std::string> heap;
  std::string&             ref = heap.emplace(3, 'a'); // "aaa"
  EXPECT_EQ(ref, "aaa");
  heap.emplace(5, 'b'); // "bbbbb" sorts above "aaa"
  EXPECT_EQ(heap.top(), "bbbbb");
  EXPECT_EQ(heap.size(), 2U);
}

//===----- MELD ----------------------------------------------------------------===//

TEST(SkewHeapMerge, CombinesBothHeaps) {
  MaxSkewHeap<int> a{5, 1, 8};
  MaxSkewHeap<int> b{3, 9, 2};
  a.merge(std::move(b));
  EXPECT_TRUE(b.is_empty());
  EXPECT_EQ(b.size(), 0U);
  skew_test::expect_drains_in_order(std::move(a), {5, 1, 8, 3, 9, 2}, std::greater<int>{});
}

TEST(SkewHeapMerge, WithEmptyHeapsBothDirections) {
  MaxSkewHeap<int> filled{4, 2};
  MaxSkewHeap<int> empty1;
  filled.merge(std::move(empty1)); // empty into filled
  EXPECT_EQ(filled.size(), 2U);

  MaxSkewHeap<int> empty2;
  empty2.merge(std::move(filled)); // filled into empty
  EXPECT_TRUE(filled.is_empty());
  skew_test::expect_drains_in_order(std::move(empty2), {4, 2}, std::greater<int>{});
}

TEST(SkewHeapMerge, SelfMergeIsNoOp) {
  MaxSkewHeap<int> h{4, 2, 7};
  h.merge(std::move(h)); // NOLINT(bugprone-use-after-move): self-merge guard
  EXPECT_EQ(h.size(), 3U);
  EXPECT_EQ(h.top(), 7);
}

TEST(SkewHeapMerge, IncompatibleStatefulComparatorsThrowWithoutMutation) {
  SkewHeap<int, skew_test::StatefulOrder> max_heap(skew_test::StatefulOrder{false});
  SkewHeap<int, skew_test::StatefulOrder> min_heap(skew_test::StatefulOrder{true});
  max_heap.insert(10);
  min_heap.insert(1);
  min_heap.insert(2);

  EXPECT_THROW(max_heap.merge(std::move(min_heap)), HeapException);
  EXPECT_EQ(max_heap.size(), 1U);
  EXPECT_EQ(max_heap.top(), 10);
  EXPECT_EQ(min_heap.size(), 2U);
  EXPECT_EQ(min_heap.top(), 1);
}

TEST(SkewHeapMerge, EqualStatefulComparatorsMerge) {
  using Heap = SkewHeap<int, skew_test::StatefulOrder>;
  skew_test::expect_equal_stateful_comparators_merge<Heap>();
}

//===----- EDGE CASES ----------------------------------------------------------===//

TEST(SkewHeapEdge, EmptyAccessThrows) {
  MaxSkewHeap<int> heap;
  EXPECT_THROW((void)heap.top(), HeapException);
  EXPECT_THROW((void)heap.extract_top(), HeapException);
}

TEST(SkewHeapEdge, SingleElement) {
  MinSkewHeap<int> heap;
  heap.insert(42);
  EXPECT_EQ(heap.top(), 42);
  EXPECT_EQ(heap.extract_top(), 42);
  EXPECT_TRUE(heap.is_empty());
}

TEST(SkewHeapEdge, AllDuplicates) {
  MaxSkewHeap<int> heap;
  for (int i = 0; i < 16; ++i) {
    heap.insert(7);
  }
  EXPECT_EQ(heap.size(), 16U);
  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ(heap.extract_top(), 7);
  }
  EXPECT_TRUE(heap.is_empty());
}

// A strictly ascending insert order builds a long right spine in the naive
// meld; exercises that the iterative meld handles it without stack growth.
TEST(SkewHeapEdge, AscendingInsertsThenDrainSorted) {
  MinSkewHeap<int> heap;
  std::vector<int> input;
  for (int i = 0; i < 5'000; ++i) {
    heap.insert(i);
    input.push_back(i);
  }
  skew_test::expect_drains_in_order(std::move(heap), input, std::less<int>{});
}

//===----- RANDOMIZED ORACLE ---------------------------------------------------===//

TEST(SkewHeapRandomized, MaxMatchesPriorityQueue) {
  skew_test::expect_matches_oracle<MaxSkewHeap<int>, skew_test::MaxOracle>(/*seed=*/1'234, /*ops=*/20'000);
}

TEST(SkewHeapRandomized, MinMatchesPriorityQueue) {
  skew_test::expect_matches_oracle<MinSkewHeap<int>, skew_test::MinOracle>(/*seed=*/9'876, /*ops=*/20'000);
}

//===----- HEAP ORDER (WHITE-BOX) ----------------------------------------------===//

TEST(SkewHeapInvariant, HeapOrderHoldsUnderRandomizedOps) {
  using Heap      = MaxSkewHeap<int>;
  using Inspector = ads::heaps::detail::SkewHeapInspector<Heap>;

  Heap                               heap;
  std::mt19937                       rng(2'024);
  std::uniform_int_distribution<int> value(-1'000, 1'000);
  std::bernoulli_distribution        do_pop(0.4);

  for (int i = 0; i < 4'000; ++i) {
    if (!heap.is_empty() && do_pop(rng)) {
      heap.extract_top();
    } else {
      heap.insert(value(rng));
    }
    ASSERT_TRUE(Inspector::valid(heap)) << "heap order broken at op " << i;
    ASSERT_TRUE(Inspector::size_matches(heap)) << "size drift at op " << i;
  }
}

TEST(SkewHeapInvariant, HeapOrderHoldsAfterMerge) {
  using Heap      = MinSkewHeap<int>;
  using Inspector = ads::heaps::detail::SkewHeapInspector<Heap>;

  std::mt19937                       rng(55);
  std::uniform_int_distribution<int> value(-500, 500);
  Heap                               a;
  Heap                               b;
  for (int i = 0; i < 600; ++i) {
    a.insert(value(rng));
    b.insert(value(rng));
  }
  a.merge(std::move(b));
  EXPECT_TRUE(Inspector::valid(a));
  EXPECT_TRUE(Inspector::size_matches(a));
  EXPECT_EQ(a.size(), 1'200U);
}

//===----- MOVE-ONLY PAYLOAD ---------------------------------------------------===//

TEST(SkewHeapPayload, MoveOnlyValues) {
  using skew_test::MoveOnlyOrdered;
  SkewHeap<MoveOnlyOrdered> heap; // default std::less => top is the maximum

  heap.insert(MoveOnlyOrdered{3});
  heap.emplace(10);
  heap.insert(MoveOnlyOrdered{7});
  EXPECT_EQ(heap.top().value, 10);
  EXPECT_EQ(heap.size(), 3U);

  EXPECT_EQ(heap.extract_top().value, 10);
  EXPECT_EQ(heap.extract_top().value, 7);
  EXPECT_EQ(heap.extract_top().value, 3);
  EXPECT_TRUE(heap.is_empty());
}

//===----- CUSTOM COMPARATOR ---------------------------------------------------===//

TEST(SkewHeapComparator, OrdersByAbsoluteValue) {
  SkewHeap<int, skew_test::AbsLess> heap; // top == largest magnitude
  for (int v : {-5, 3, -1, 4, -9}) {
    heap.insert(v);
  }
  EXPECT_EQ(heap.top(), -9);
  EXPECT_EQ(heap.extract_top(), -9);
  EXPECT_EQ(heap.extract_top(), -5);
  EXPECT_EQ(heap.extract_top(), 4);
}

//===---------------------------------------------------------------------------===//
