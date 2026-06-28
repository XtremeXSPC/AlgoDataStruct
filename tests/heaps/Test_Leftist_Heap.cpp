//===---------------------------------------------------------------------------===//
/**
 * @file Test_Leftist_Heap.cpp
 * @brief Google Test unit tests for the LeftistHeap meldable heap.
 * @version 0.1
 * @date 2026-06-28
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/heaps/Leftist_Heap.hpp"

#include <gtest/gtest.h>

#include <cstdlib>
#include <functional>
#include <queue>
#include <random>
#include <string>
#include <vector>

using namespace ads::heaps;

namespace ads::heaps::detail {

// White-box inspector (friend of LeftistHeap): verifies heap order, the leftist
// npl property, and the npl bookkeeping over the entire internal tree. This is
// the structural counterpart to the behavioral oracle tests.
template <typename Heap>
struct LeftistHeapInspector {
  using NodePtr = typename Heap::NodePtr;

  static auto npl(const NodePtr& node) -> int { return node ? node->npl : -1; }

  static auto count(const NodePtr& node) -> std::size_t {
    return node ? 1 + count(node->left) + count(node->right) : 0;
  }

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
    // Leftist property and npl bookkeeping.
    if (npl(node->left) < npl(node->right)) {
      return false;
    }
    if (node->npl != npl(node->right) + 1) {
      return false;
    }
    return check(heap, node->left) && check(heap, node->right);
  }

  static auto valid(const Heap& heap) -> bool { return check(heap, heap.root_); }

  static auto size_matches(const Heap& heap) -> bool { return count(heap.root_) == heap.size_; }
};

} // namespace ads::heaps::detail

namespace leftist_test {

// Move-only, totally-ordered payload to exercise the move-only insert/emplace
// and extract paths. Named uniquely to stay clear of other tests under the
// unity build.
struct MoveOnlyOrdered {
  int value = 0;

  MoveOnlyOrdered() = default;
  explicit MoveOnlyOrdered(int v) : value(v) {}

  MoveOnlyOrdered(MoveOnlyOrdered&&) noexcept            = default;
  auto operator=(MoveOnlyOrdered&&) noexcept -> MoveOnlyOrdered& = default;
  MoveOnlyOrdered(const MoveOnlyOrdered&)                = delete;
  auto operator=(const MoveOnlyOrdered&) -> MoveOnlyOrdered&     = delete;

  auto operator<=>(const MoveOnlyOrdered&) const = default;
};

// Orders by absolute value; used to prove the comparator is fully generic.
struct AbsLess {
  auto operator()(int a, int b) const -> bool { return std::abs(a) < std::abs(b); }
};

// Drains `heap` and asserts it yields `input` in the order produced by sorting
// with `pop_order` (the comparator describing extraction order). Reusable by
// every MeldableHeap, so the deferred heaps can share it.
template <typename Heap, typename PopOrder>
void expect_drains_in_order(Heap heap, std::vector<int> input, PopOrder pop_order) {
  std::sort(input.begin(), input.end(), pop_order);
  ASSERT_EQ(heap.size(), input.size());
  for (int expected : input) {
    ASSERT_FALSE(heap.is_empty());
    EXPECT_EQ(heap.top(), expected);
    EXPECT_EQ(heap.extract_top(), expected);
  }
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
}

// Interleaved random push/pop workload validated step-by-step against a
// std::priority_queue oracle. Heap and Oracle must agree on priority order.
template <typename Heap, typename Oracle>
void expect_matches_oracle(unsigned seed, int ops) {
  Heap                               heap;
  Oracle                             oracle;
  std::mt19937                       rng(seed);
  std::uniform_int_distribution<int> value(-10000, 10000);
  std::bernoulli_distribution        do_pop(0.45);

  for (int i = 0; i < ops; ++i) {
    if (!oracle.empty() && do_pop(rng)) {
      ASSERT_EQ(heap.top(), oracle.top());
      EXPECT_EQ(heap.extract_top(), oracle.top());
      oracle.pop();
    } else {
      int v = value(rng);
      heap.insert(v);
      oracle.push(v);
    }
    ASSERT_EQ(heap.size(), oracle.size());
  }
  while (!oracle.empty()) {
    EXPECT_EQ(heap.extract_top(), oracle.top());
    oracle.pop();
  }
  EXPECT_TRUE(heap.is_empty());
}

using MaxOracle = std::priority_queue<int>;
using MinOracle = std::priority_queue<int, std::vector<int>, std::greater<>>;

} // namespace leftist_test

//===----- CONCEPT & TYPE-TRAIT CHECKS -----------------------------------------===//

static_assert(MeldableHeap<MaxLeftistHeap<int>>);
static_assert(MeldableHeap<MinLeftistHeap<int>>);
static_assert(MeldableHeap<LeftistHeap<std::string>>);
static_assert(MeldableHeap<LeftistHeap<leftist_test::MoveOnlyOrdered>>);

static_assert(std::move_constructible<MaxLeftistHeap<int>>);
static_assert(!std::copy_constructible<MaxLeftistHeap<int>>);
static_assert(!std::is_copy_assignable_v<MaxLeftistHeap<int>>);

//===----- BASIC STATE ---------------------------------------------------------===//

TEST(LeftistHeapBasic, EmptyOnConstruction) {
  MaxLeftistHeap<int> heap;
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
}

TEST(LeftistHeapBasic, InsertUpdatesTopAndSize) {
  MaxLeftistHeap<int> heap;
  heap.insert(30);
  EXPECT_EQ(heap.top(), 30);
  heap.insert(50);
  EXPECT_EQ(heap.top(), 50);
  heap.insert(40);
  EXPECT_EQ(heap.top(), 50);
  EXPECT_EQ(heap.size(), 3U);
  EXPECT_FALSE(heap.is_empty());
}

TEST(LeftistHeapBasic, MinHeapTopIsMinimum) {
  MinLeftistHeap<int> heap;
  heap.insert(30);
  heap.insert(10);
  heap.insert(20);
  EXPECT_EQ(heap.top(), 10);
}

TEST(LeftistHeapBasic, ClearEmptiesHeap) {
  MaxLeftistHeap<int> heap{1, 2, 3, 4, 5};
  heap.clear();
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
  heap.insert(7); // usable after clear
  EXPECT_EQ(heap.top(), 7);
}

//===----- CONSTRUCTION --------------------------------------------------------===//

TEST(LeftistHeapConstruction, InitializerList) {
  leftist_test::expect_drains_in_order(MaxLeftistHeap<int>{3, 1, 4, 1, 5, 9, 2, 6},
                                       {3, 1, 4, 1, 5, 9, 2, 6}, std::greater<int>{});
}

TEST(LeftistHeapConstruction, FromVector) {
  std::vector<int> src{7, 2, 9, 4, 4, 1};
  leftist_test::expect_drains_in_order(MinLeftistHeap<int>(src), src, std::less<int>{});
}

TEST(LeftistHeapConstruction, FromIteratorRange) {
  std::vector<int> src{7, 2, 9, 4, 4, 1};
  leftist_test::expect_drains_in_order(MaxLeftistHeap<int>(src.begin(), src.end()), src, std::greater<int>{});
}

TEST(LeftistHeapConstruction, MoveConstructorTransfersAndEmptiesSource) {
  MaxLeftistHeap<int> src{5, 1, 8, 3};
  MaxLeftistHeap<int> dst(std::move(src));
  EXPECT_TRUE(src.is_empty());
  EXPECT_EQ(src.size(), 0U);
  leftist_test::expect_drains_in_order(std::move(dst), {5, 1, 8, 3}, std::greater<int>{});
}

TEST(LeftistHeapConstruction, MoveAssignmentReplacesTarget) {
  MaxLeftistHeap<int> src{5, 1, 8, 3};
  MaxLeftistHeap<int> dst{100, 200}; // contents must be discarded
  dst = std::move(src);
  EXPECT_TRUE(src.is_empty());
  leftist_test::expect_drains_in_order(std::move(dst), {5, 1, 8, 3}, std::greater<int>{});
}

//===----- EMPLACE -------------------------------------------------------------===//

TEST(LeftistHeapEmplace, ConstructsInPlaceAndReturnsReference) {
  MaxLeftistHeap<std::string> heap;
  std::string&                ref = heap.emplace(3, 'a'); // "aaa"
  EXPECT_EQ(ref, "aaa");
  heap.emplace(5, 'b'); // "bbbbb" sorts above "aaa"
  EXPECT_EQ(heap.top(), "bbbbb");
  EXPECT_EQ(heap.size(), 2U);
}

//===----- MELD ----------------------------------------------------------------===//

TEST(LeftistHeapMerge, CombinesBothHeaps) {
  MaxLeftistHeap<int> a{5, 1, 8};
  MaxLeftistHeap<int> b{3, 9, 2};
  a.merge(std::move(b));
  EXPECT_TRUE(b.is_empty());
  EXPECT_EQ(b.size(), 0U);
  leftist_test::expect_drains_in_order(std::move(a), {5, 1, 8, 3, 9, 2}, std::greater<int>{});
}

TEST(LeftistHeapMerge, WithEmptyHeapsBothDirections) {
  MaxLeftistHeap<int> filled{4, 2};
  MaxLeftistHeap<int> empty1;
  filled.merge(std::move(empty1)); // empty into filled
  EXPECT_EQ(filled.size(), 2U);

  MaxLeftistHeap<int> empty2;
  empty2.merge(std::move(filled)); // filled into empty
  EXPECT_TRUE(filled.is_empty());
  leftist_test::expect_drains_in_order(std::move(empty2), {4, 2}, std::greater<int>{});
}

TEST(LeftistHeapMerge, SelfMergeIsNoOp) {
  MaxLeftistHeap<int> h{4, 2, 7};
  h.merge(std::move(h)); // NOLINT(bugprone-use-after-move): self-merge guard
  EXPECT_EQ(h.size(), 3U);
  EXPECT_EQ(h.top(), 7);
}

//===----- EDGE CASES ----------------------------------------------------------===//

TEST(LeftistHeapEdge, EmptyAccessThrows) {
  MaxLeftistHeap<int> heap;
  EXPECT_THROW((void)heap.top(), HeapException);
  EXPECT_THROW((void)heap.extract_top(), HeapException);
}

TEST(LeftistHeapEdge, SingleElement) {
  MinLeftistHeap<int> heap;
  heap.insert(42);
  EXPECT_EQ(heap.top(), 42);
  EXPECT_EQ(heap.extract_top(), 42);
  EXPECT_TRUE(heap.is_empty());
}

TEST(LeftistHeapEdge, AllDuplicates) {
  MaxLeftistHeap<int> heap;
  for (int i = 0; i < 16; ++i) {
    heap.insert(7);
  }
  EXPECT_EQ(heap.size(), 16U);
  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ(heap.extract_top(), 7);
  }
  EXPECT_TRUE(heap.is_empty());
}

//===----- RANDOMIZED ORACLE ---------------------------------------------------===//

TEST(LeftistHeapRandomized, MaxMatchesPriorityQueue) {
  leftist_test::expect_matches_oracle<MaxLeftistHeap<int>, leftist_test::MaxOracle>(/*seed=*/1234, /*ops=*/20000);
}

TEST(LeftistHeapRandomized, MinMatchesPriorityQueue) {
  leftist_test::expect_matches_oracle<MinLeftistHeap<int>, leftist_test::MinOracle>(/*seed=*/9876, /*ops=*/20000);
}

//===----- STRUCTURAL INVARIANT (WHITE-BOX) ------------------------------------===//

TEST(LeftistHeapInvariant, HoldsUnderRandomizedOps) {
  using Heap      = MaxLeftistHeap<int>;
  using Inspector = ads::heaps::detail::LeftistHeapInspector<Heap>;

  Heap                               heap;
  std::mt19937                       rng(2024);
  std::uniform_int_distribution<int> value(-1000, 1000);
  std::bernoulli_distribution        do_pop(0.4);

  for (int i = 0; i < 4000; ++i) {
    if (!heap.is_empty() && do_pop(rng)) {
      heap.extract_top();
    } else {
      heap.insert(value(rng));
    }
    ASSERT_TRUE(Inspector::valid(heap)) << "leftist invariant broken at op " << i;
    ASSERT_TRUE(Inspector::size_matches(heap)) << "size drift at op " << i;
  }
}

TEST(LeftistHeapInvariant, HoldsAfterMerge) {
  using Heap      = MinLeftistHeap<int>;
  using Inspector = ads::heaps::detail::LeftistHeapInspector<Heap>;

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
  EXPECT_EQ(a.size(), 1200U);
}

//===----- MOVE-ONLY PAYLOAD ---------------------------------------------------===//

TEST(LeftistHeapPayload, MoveOnlyValues) {
  using leftist_test::MoveOnlyOrdered;
  LeftistHeap<MoveOnlyOrdered> heap; // default std::less => top is the maximum

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

TEST(LeftistHeapComparator, OrdersByAbsoluteValue) {
  LeftistHeap<int, leftist_test::AbsLess> heap; // top == largest magnitude
  for (int v : {-5, 3, -1, 4, -9}) {
    heap.insert(v);
  }
  EXPECT_EQ(heap.top(), -9);
  EXPECT_EQ(heap.extract_top(), -9);
  EXPECT_EQ(heap.extract_top(), -5);
  EXPECT_EQ(heap.extract_top(), 4);
}

//===---------------------------------------------------------------------------===//
