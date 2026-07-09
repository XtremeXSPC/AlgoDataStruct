//===---------------------------------------------------------------------------===//
/**
 * @file Test_Fibonacci_Heap.cpp
 * @brief Google Test unit tests for the FibonacciHeap meldable heap.
 * @version 0.1
 * @date 2026-07-09
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "Meldable_Heap_Test_Support.hpp"
#include "ads/heaps/Fibonacci_Heap.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <queue>
#include <random>
#include <string>
#include <vector>

using namespace ads::heaps;

namespace ads::heaps::detail {

// White-box inspector (friend of FibonacciHeap): verifies heap order, the circular
// doubly-linked wiring of every sibling ring, parent pointers, the per-node degree
// bookkeeping, that 'top_' is an un-outranked root, and that the arena size matches
// the element count.
template <typename Heap>
struct FibonacciHeapInspector {
  using Node = typename Heap::Node;

  static auto check_children(const Heap& heap, Node* first, Node* parent, std::size_t& total) -> bool {
    if (first == nullptr) {
      return true;
    }
    int   direct = 0;
    Node* cur    = first;
    do {
      if (cur->left->right != cur || cur->right->left != cur) {
        return false; // broken circular list
      }
      if (cur->parent != parent) {
        return false;
      }
      if (parent != nullptr && heap.comp_(parent->value, cur->value)) {
        return false; // parent must not rank below child
      }
      ++direct;
      total += 1;
      if (!check_children(heap, cur->child, cur, total)) {
        return false;
      }
      cur = cur->right;
    } while (cur != first);
    return parent == nullptr || direct == parent->degree;
  }

  static auto valid(const Heap& heap) -> bool {
    if (heap.top_ == nullptr) {
      return heap.size_ == 0 && heap.pool_.empty();
    }
    if (heap.top_->parent != nullptr) {
      return false;
    }
    std::size_t total = 0;
    Node*       cur   = heap.top_;
    do {
      if (cur->left->right != cur || cur->right->left != cur) {
        return false;
      }
      if (cur->parent != nullptr) {
        return false;
      }
      if (heap.comp_(heap.top_->value, cur->value)) {
        return false; // some root outranks top_
      }
      total += 1;
      if (!check_children(heap, cur->child, cur, total)) {
        return false;
      }
      cur = cur->right;
    } while (cur != heap.top_);
    return total == heap.size_ && heap.pool_.size() == heap.size_;
  }
};

} // namespace ads::heaps::detail

namespace fibonacci_test = ads::heaps::meldable_test;

//===----- CONCEPT & TYPE-TRAIT CHECKS -----------------------------------------===//

static_assert(MeldableHeap<MaxFibonacciHeap<int>>);
static_assert(MeldableHeap<MinFibonacciHeap<int>>);
static_assert(MeldableHeap<FibonacciHeap<std::string>>);
static_assert(MeldableHeap<FibonacciHeap<fibonacci_test::MoveOnlyOrdered>>);

static_assert(std::move_constructible<MaxFibonacciHeap<int>>);
static_assert(!std::copy_constructible<MaxFibonacciHeap<int>>);
static_assert(!std::is_copy_assignable_v<MaxFibonacciHeap<int>>);

//===----- BASIC STATE ---------------------------------------------------------===//

TEST(FibonacciHeapBasic, EmptyOnConstruction) {
  MaxFibonacciHeap<int> heap;
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
}

TEST(FibonacciHeapBasic, InsertUpdatesTopAndSize) {
  MaxFibonacciHeap<int> heap;
  heap.insert(30);
  EXPECT_EQ(heap.top(), 30);
  heap.insert(50);
  EXPECT_EQ(heap.top(), 50);
  heap.insert(40);
  EXPECT_EQ(heap.top(), 50);
  EXPECT_EQ(heap.size(), 3U);
  EXPECT_FALSE(heap.is_empty());
}

TEST(FibonacciHeapBasic, MinHeapTopIsMinimum) {
  MinFibonacciHeap<int> heap;
  heap.insert(30);
  heap.insert(10);
  heap.insert(20);
  EXPECT_EQ(heap.top(), 10);
}

TEST(FibonacciHeapBasic, ClearEmptiesHeap) {
  MaxFibonacciHeap<int> heap{1, 2, 3, 4, 5};
  heap.clear();
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
  heap.insert(7);
  EXPECT_EQ(heap.top(), 7);
}

//===----- CONSTRUCTION --------------------------------------------------------===//

TEST(FibonacciHeapConstruction, InitializerList) {
  fibonacci_test::expect_drains_in_order(MaxFibonacciHeap<int>{3, 1, 4, 1, 5, 9, 2, 6}, {3, 1, 4, 1, 5, 9, 2, 6}, std::greater<int>{});
}

TEST(FibonacciHeapConstruction, FromVector) {
  std::vector<int> src{7, 2, 9, 4, 4, 1};
  fibonacci_test::expect_drains_in_order(MinFibonacciHeap<int>(src), src, std::less<int>{});
}

TEST(FibonacciHeapConstruction, FromIteratorRange) {
  std::vector<int> src{7, 2, 9, 4, 4, 1};
  fibonacci_test::expect_drains_in_order(MaxFibonacciHeap<int>(src.begin(), src.end()), src, std::greater<int>{});
}

TEST(FibonacciHeapConstruction, MoveConstructorTransfersAndEmptiesSource) {
  MaxFibonacciHeap<int> src{5, 1, 8, 3};
  MaxFibonacciHeap<int> dst(std::move(src));
  EXPECT_TRUE(src.is_empty());
  EXPECT_EQ(src.size(), 0U);
  fibonacci_test::expect_drains_in_order(std::move(dst), {5, 1, 8, 3}, std::greater<int>{});
}

TEST(FibonacciHeapConstruction, MoveAssignmentReplacesTarget) {
  MaxFibonacciHeap<int> src{5, 1, 8, 3};
  MaxFibonacciHeap<int> dst{100, 200};
  dst = std::move(src);
  EXPECT_TRUE(src.is_empty());
  fibonacci_test::expect_drains_in_order(std::move(dst), {5, 1, 8, 3}, std::greater<int>{});
}

//===----- MELD ----------------------------------------------------------------===//

TEST(FibonacciHeapMerge, CombinesBothHeaps) {
  MaxFibonacciHeap<int> a{5, 1, 8};
  MaxFibonacciHeap<int> b{3, 9, 2};
  a.merge(std::move(b));
  EXPECT_TRUE(b.is_empty());
  EXPECT_EQ(b.size(), 0U);
  fibonacci_test::expect_drains_in_order(std::move(a), {5, 1, 8, 3, 9, 2}, std::greater<int>{});
}

TEST(FibonacciHeapMerge, WithEmptyHeapsBothDirections) {
  MaxFibonacciHeap<int> filled{4, 2};
  MaxFibonacciHeap<int> empty1;
  filled.merge(std::move(empty1));
  EXPECT_EQ(filled.size(), 2U);

  MaxFibonacciHeap<int> empty2;
  empty2.merge(std::move(filled));
  EXPECT_TRUE(filled.is_empty());
  fibonacci_test::expect_drains_in_order(std::move(empty2), {4, 2}, std::greater<int>{});
}

TEST(FibonacciHeapMerge, SelfMergeIsNoOp) {
  MaxFibonacciHeap<int> h{4, 2, 7};
  h.merge(std::move(h)); // NOLINT(bugprone-use-after-move): self-merge guard
  EXPECT_EQ(h.size(), 3U);
  EXPECT_EQ(h.top(), 7);
}

TEST(FibonacciHeapMerge, IncompatibleStatefulComparatorsThrowWithoutMutation) {
  FibonacciHeap<int, fibonacci_test::StatefulOrder> max_heap(fibonacci_test::StatefulOrder{false});
  FibonacciHeap<int, fibonacci_test::StatefulOrder> min_heap(fibonacci_test::StatefulOrder{true});
  max_heap.insert(10);
  min_heap.insert(1);
  min_heap.insert(2);

  EXPECT_THROW(max_heap.merge(std::move(min_heap)), HeapException);
  EXPECT_EQ(max_heap.size(), 1U);
  EXPECT_EQ(max_heap.top(), 10);
  EXPECT_EQ(min_heap.size(), 2U);
  EXPECT_EQ(min_heap.top(), 1);
}

TEST(FibonacciHeapMerge, EqualStatefulComparatorsMerge) {
  using Heap = FibonacciHeap<int, fibonacci_test::StatefulOrder>;
  fibonacci_test::expect_equal_stateful_comparators_merge<Heap>();
}

//===----- HANDLES: DECREASE_KEY & ERASE ---------------------------------------===//

TEST(FibonacciHeapHandle, EmplaceReturnsValidHandle) {
  MaxFibonacciHeap<int>         heap;
  MaxFibonacciHeap<int>::Handle h = heap.emplace(10);
  EXPECT_TRUE(h.valid());
  MaxFibonacciHeap<int>::Handle empty;
  EXPECT_FALSE(empty.valid());
}

TEST(FibonacciHeapHandle, DecreaseKeyRaisesToTopMaxHeap) {
  MaxFibonacciHeap<int> heap;
  heap.emplace(10);
  auto h = heap.emplace(20);
  heap.emplace(30);
  EXPECT_EQ(heap.top(), 30);

  heap.decrease_key(h, 99); // max-heap: "raise priority" means larger
  EXPECT_EQ(heap.top(), 99);
  EXPECT_EQ(heap.extract_top(), 99);
  EXPECT_EQ(heap.top(), 30);
}

TEST(FibonacciHeapHandle, DecreaseKeyRaisesToTopMinHeap) {
  MinFibonacciHeap<int> heap;
  heap.emplace(10);
  auto h = heap.emplace(20);
  heap.emplace(5);
  EXPECT_EQ(heap.top(), 5);

  heap.decrease_key(h, 1); // min-heap: "raise priority" means smaller
  EXPECT_EQ(heap.top(), 1);
  EXPECT_EQ(heap.extract_top(), 1);
  EXPECT_EQ(heap.top(), 5);
}

TEST(FibonacciHeapHandle, DecreaseKeyRejectsPriorityDrop) {
  MaxFibonacciHeap<int> heap;
  auto                  h = heap.emplace(50);
  EXPECT_THROW(heap.decrease_key(h, 40), HeapException);
  EXPECT_EQ(heap.top(), 50);
  EXPECT_EQ(heap.size(), 1U);
}

TEST(FibonacciHeapHandle, DecreaseKeyEqualValueIsAllowed) {
  MaxFibonacciHeap<int> heap;
  auto                  h = heap.emplace(50);
  heap.emplace(70);
  EXPECT_NO_THROW(heap.decrease_key(h, 50));
  EXPECT_EQ(heap.top(), 70);
}

TEST(FibonacciHeapHandle, EraseRemovesArbitraryElement) {
  MaxFibonacciHeap<int> heap;
  heap.emplace(10);
  auto h20 = heap.emplace(20);
  heap.emplace(30);
  heap.emplace(5);
  EXPECT_EQ(heap.size(), 4U);

  heap.erase(h20);
  EXPECT_EQ(heap.size(), 3U);
  fibonacci_test::expect_drains_in_order(std::move(heap), {10, 30, 5}, std::greater<int>{});
}

TEST(FibonacciHeapHandle, EraseTopElement) {
  MinFibonacciHeap<int> heap;
  auto                  h = heap.emplace(1);
  heap.emplace(2);
  heap.emplace(3);
  EXPECT_EQ(heap.top(), 1);
  heap.erase(h);
  EXPECT_EQ(heap.top(), 2);
  EXPECT_EQ(heap.size(), 2U);
}

TEST(FibonacciHeapHandle, InvalidHandleThrows) {
  MaxFibonacciHeap<int>         heap;
  MaxFibonacciHeap<int>::Handle empty;
  EXPECT_THROW(heap.erase(empty), HeapException);
  EXPECT_THROW(heap.decrease_key(empty, 5), HeapException);
}

TEST(FibonacciHeapHandle, HandlesStayValidAcrossMerge) {
  MaxFibonacciHeap<int> a;
  auto                  ha = a.emplace(10);
  MaxFibonacciHeap<int> b{100, 200, 300};
  a.merge(std::move(b));
  a.decrease_key(ha, 999);
  EXPECT_EQ(a.top(), 999);
}

// Drives extract_top (which triggers consolidation) interleaved with decrease_key
// so cascading cuts and marks are exercised together.
TEST(FibonacciHeapHandle, DecreaseKeyAfterConsolidation) {
  MinFibonacciHeap<int>                      heap;
  std::vector<MinFibonacciHeap<int>::Handle> handles;
  handles.reserve(32);
  for (int v = 0; v < 32; ++v) {
    handles.push_back(heap.emplace(v * 10));
  }
  // Force a consolidation so trees of higher degree (with real children) form.
  EXPECT_EQ(heap.extract_top(), 0);

  // Now decrease several deep nodes below the current top to trigger cuts.
  heap.decrease_key(handles[31], -5);
  EXPECT_EQ(heap.top(), -5);
  heap.decrease_key(handles[20], -10);
  EXPECT_EQ(heap.top(), -10);
  heap.decrease_key(handles[15], -20);
  EXPECT_EQ(heap.top(), -20);

  EXPECT_EQ(heap.extract_top(), -20);
  EXPECT_EQ(heap.extract_top(), -10);
  EXPECT_EQ(heap.extract_top(), -5);
}

//===----- EDGE CASES ----------------------------------------------------------===//

TEST(FibonacciHeapEdge, EmptyAccessThrows) {
  MaxFibonacciHeap<int> heap;
  EXPECT_THROW((void)heap.top(), HeapException);
  EXPECT_THROW((void)heap.extract_top(), HeapException);
}

TEST(FibonacciHeapEdge, SingleElement) {
  MinFibonacciHeap<int> heap;
  heap.insert(42);
  EXPECT_EQ(heap.top(), 42);
  EXPECT_EQ(heap.extract_top(), 42);
  EXPECT_TRUE(heap.is_empty());
}

TEST(FibonacciHeapEdge, AllDuplicates) {
  MaxFibonacciHeap<int> heap;
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

TEST(FibonacciHeapRandomized, MaxMatchesPriorityQueue) {
  fibonacci_test::expect_matches_oracle<MaxFibonacciHeap<int>, fibonacci_test::MaxOracle>(/*seed=*/1'234, /*ops=*/20'000);
}

TEST(FibonacciHeapRandomized, MinMatchesPriorityQueue) {
  fibonacci_test::expect_matches_oracle<MinFibonacciHeap<int>, fibonacci_test::MinOracle>(/*seed=*/9'876, /*ops=*/20'000);
}

//===----- STRUCTURAL INVARIANT (WHITE-BOX) ------------------------------------===//

TEST(FibonacciHeapInvariant, HoldsUnderRandomizedInsertExtract) {
  using Heap      = MaxFibonacciHeap<int>;
  using Inspector = ads::heaps::detail::FibonacciHeapInspector<Heap>;

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
    ASSERT_TRUE(Inspector::valid(heap)) << "Fibonacci invariant broken at op " << i;
  }
}

TEST(FibonacciHeapInvariant, HoldsUnderRandomizedHandleOps) {
  using Heap      = MinFibonacciHeap<int>;
  using Inspector = ads::heaps::detail::FibonacciHeapInspector<Heap>;

  // Every operation acts through a handle we track precisely, so heap size stays
  // in lock-step with our handle vector. Periodic extract_top is included to
  // force consolidation between decrease_key / erase bursts.
  Heap                               heap;
  std::vector<Heap::Handle>          handles;
  std::vector<int>                   values;
  std::mt19937                       rng(4'321);
  std::uniform_int_distribution<int> value(0, 100'000);
  std::uniform_int_distribution<int> action(0, 3);

  auto drop = [&](std::size_t idx) -> void {
    handles.erase(handles.begin() + static_cast<std::ptrdiff_t>(idx));
    values.erase(values.begin() + static_cast<std::ptrdiff_t>(idx));
  };

  for (int i = 0; i < 3'000; ++i) {
    int op = handles.size() < 8 ? 0 : action(rng);
    if (op == 0) { // insert
      int v = value(rng);
      handles.push_back(heap.emplace(v));
      values.push_back(v);
    } else if (op == 1) { // decrease_key (min-heap: lower the value)
      std::size_t idx = rng() % handles.size();
      int         nv  = values[idx] == 0 ? 0 : static_cast<int>(rng() % static_cast<unsigned>(values[idx]));
      heap.decrease_key(handles[idx], nv);
      values[idx] = nv;
    } else if (op == 2) { // erase
      std::size_t idx = rng() % handles.size();
      heap.erase(handles[idx]);
      drop(idx);
    } else { // extract_top: remove the current minimum, dropping its tracked handle
      int  top = heap.extract_top();
      auto it  = std::ranges::find(values, top);
      ASSERT_NE(it, values.end());
      drop(static_cast<std::size_t>(it - values.begin()));
    }
    ASSERT_TRUE(Inspector::valid(heap)) << "invariant broken at op " << i;
    ASSERT_EQ(heap.size(), handles.size());
  }

  std::ranges::sort(values);
  for (int expected : values) {
    EXPECT_EQ(heap.extract_top(), expected);
  }
  EXPECT_TRUE(heap.is_empty());
}

//===----- MOVE-ONLY PAYLOAD ---------------------------------------------------===//

TEST(FibonacciHeapPayload, MoveOnlyValues) {
  using fibonacci_test::MoveOnlyOrdered;
  FibonacciHeap<MoveOnlyOrdered> heap;

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

TEST(FibonacciHeapComparator, OrdersByAbsoluteValue) {
  FibonacciHeap<int, fibonacci_test::AbsLess> heap;
  for (int v : {-5, 3, -1, 4, -9}) {
    heap.insert(v);
  }
  EXPECT_EQ(heap.top(), -9);
  EXPECT_EQ(heap.extract_top(), -9);
  EXPECT_EQ(heap.extract_top(), -5);
  EXPECT_EQ(heap.extract_top(), 4);
}

//===---------------------------------------------------------------------------===//
