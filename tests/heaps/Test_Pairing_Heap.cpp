//===---------------------------------------------------------------------------===//
/**
 * @file Test_Pairing_Heap.cpp
 * @brief Google Test unit tests for the PairingHeap meldable heap.
 * @version 0.1
 * @date 2026-07-08
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "Meldable_Heap_Test_Support.hpp"
#include "ads/heaps/Pairing_Heap.hpp"

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

// White-box inspector (friend of PairingHeap): verifies heap order, the
// left-child / right-sibling 'prev' wiring (leftmost child points at its parent,
// each later sibling at its left neighbour), and the element count.
template <typename Heap>
struct PairingHeapInspector {
  using Node = typename Heap::Node;

  static auto check_children(const Heap& heap, const Node* parent) -> bool {
    const Node* expected_prev = parent;
    for (const Node* c = parent->child.get(); c != nullptr; c = c->sibling.get()) {
      if (c->prev != expected_prev) {
        return false;
      }
      if (heap.comp_(parent->value, c->value)) { // parent must not rank below child
        return false;
      }
      if (!check_children(heap, c)) {
        return false;
      }
      expected_prev = c;
    }
    return true;
  }

  static auto count(const Node* node) -> std::size_t {
    std::size_t total = 0;
    for (const Node* c = node ? node->child.get() : nullptr; c != nullptr; c = c->sibling.get()) {
      total += count(c);
    }
    return node ? total + 1 : 0;
  }

  static auto valid(const Heap& heap) -> bool {
    if (!heap.root_) {
      return heap.size_ == 0;
    }
    if (heap.root_->prev != nullptr) {
      return false;
    }
    return check_children(heap, heap.root_.get()) && count(heap.root_.get()) == heap.size_;
  }
};

} // namespace ads::heaps::detail

namespace pairing_test = ads::heaps::meldable_test;

//===----- CONCEPT & TYPE-TRAIT CHECKS -----------------------------------------===//

static_assert(MeldableHeap<MaxPairingHeap<int>>);
static_assert(MeldableHeap<MinPairingHeap<int>>);
static_assert(MeldableHeap<PairingHeap<std::string>>);
static_assert(MeldableHeap<PairingHeap<pairing_test::MoveOnlyOrdered>>);

static_assert(std::move_constructible<MaxPairingHeap<int>>);
static_assert(!std::copy_constructible<MaxPairingHeap<int>>);
static_assert(!std::is_copy_assignable_v<MaxPairingHeap<int>>);

//===----- BASIC STATE ---------------------------------------------------------===//

TEST(PairingHeapBasic, EmptyOnConstruction) {
  MaxPairingHeap<int> heap;
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
}

TEST(PairingHeapBasic, InsertUpdatesTopAndSize) {
  MaxPairingHeap<int> heap;
  heap.insert(30);
  EXPECT_EQ(heap.top(), 30);
  heap.insert(50);
  EXPECT_EQ(heap.top(), 50);
  heap.insert(40);
  EXPECT_EQ(heap.top(), 50);
  EXPECT_EQ(heap.size(), 3U);
  EXPECT_FALSE(heap.is_empty());
}

TEST(PairingHeapBasic, MinHeapTopIsMinimum) {
  MinPairingHeap<int> heap;
  heap.insert(30);
  heap.insert(10);
  heap.insert(20);
  EXPECT_EQ(heap.top(), 10);
}

TEST(PairingHeapBasic, ClearEmptiesHeap) {
  MaxPairingHeap<int> heap{1, 2, 3, 4, 5};
  heap.clear();
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
  heap.insert(7);
  EXPECT_EQ(heap.top(), 7);
}

//===----- CONSTRUCTION --------------------------------------------------------===//

TEST(PairingHeapConstruction, InitializerList) {
  pairing_test::expect_drains_in_order(MaxPairingHeap<int>{3, 1, 4, 1, 5, 9, 2, 6}, {3, 1, 4, 1, 5, 9, 2, 6}, std::greater<int>{});
}

TEST(PairingHeapConstruction, FromVector) {
  std::vector<int> src{7, 2, 9, 4, 4, 1};
  pairing_test::expect_drains_in_order(MinPairingHeap<int>(src), src, std::less<int>{});
}

TEST(PairingHeapConstruction, FromIteratorRange) {
  std::vector<int> src{7, 2, 9, 4, 4, 1};
  pairing_test::expect_drains_in_order(MaxPairingHeap<int>(src.begin(), src.end()), src, std::greater<int>{});
}

TEST(PairingHeapConstruction, MoveConstructorTransfersAndEmptiesSource) {
  MaxPairingHeap<int> src{5, 1, 8, 3};
  MaxPairingHeap<int> dst(std::move(src));
  EXPECT_TRUE(src.is_empty());
  EXPECT_EQ(src.size(), 0U);
  pairing_test::expect_drains_in_order(std::move(dst), {5, 1, 8, 3}, std::greater<int>{});
}

TEST(PairingHeapConstruction, MoveAssignmentReplacesTarget) {
  MaxPairingHeap<int> src{5, 1, 8, 3};
  MaxPairingHeap<int> dst{100, 200};
  dst = std::move(src);
  EXPECT_TRUE(src.is_empty());
  pairing_test::expect_drains_in_order(std::move(dst), {5, 1, 8, 3}, std::greater<int>{});
}

//===----- MELD ----------------------------------------------------------------===//

TEST(PairingHeapMerge, CombinesBothHeaps) {
  MaxPairingHeap<int> a{5, 1, 8};
  MaxPairingHeap<int> b{3, 9, 2};
  a.merge(std::move(b));
  EXPECT_TRUE(b.is_empty());
  EXPECT_EQ(b.size(), 0U);
  pairing_test::expect_drains_in_order(std::move(a), {5, 1, 8, 3, 9, 2}, std::greater<int>{});
}

TEST(PairingHeapMerge, WithEmptyHeapsBothDirections) {
  MaxPairingHeap<int> filled{4, 2};
  MaxPairingHeap<int> empty1;
  filled.merge(std::move(empty1));
  EXPECT_EQ(filled.size(), 2U);

  MaxPairingHeap<int> empty2;
  empty2.merge(std::move(filled));
  EXPECT_TRUE(filled.is_empty());
  pairing_test::expect_drains_in_order(std::move(empty2), {4, 2}, std::greater<int>{});
}

TEST(PairingHeapMerge, SelfMergeIsNoOp) {
  MaxPairingHeap<int> h{4, 2, 7};
  h.merge(std::move(h)); // NOLINT(bugprone-use-after-move): self-merge guard
  EXPECT_EQ(h.size(), 3U);
  EXPECT_EQ(h.top(), 7);
}

TEST(PairingHeapMerge, IncompatibleStatefulComparatorsThrowWithoutMutation) {
  PairingHeap<int, pairing_test::StatefulOrder> max_heap(pairing_test::StatefulOrder{false});
  PairingHeap<int, pairing_test::StatefulOrder> min_heap(pairing_test::StatefulOrder{true});
  max_heap.insert(10);
  min_heap.insert(1);
  min_heap.insert(2);

  EXPECT_THROW(max_heap.merge(std::move(min_heap)), HeapException);
  EXPECT_EQ(max_heap.size(), 1U);
  EXPECT_EQ(max_heap.top(), 10);
  EXPECT_EQ(min_heap.size(), 2U);
  EXPECT_EQ(min_heap.top(), 1);
}

TEST(PairingHeapMerge, EqualStatefulComparatorsMerge) {
  using Heap = PairingHeap<int, pairing_test::StatefulOrder>;
  pairing_test::expect_equal_stateful_comparators_merge<Heap>();
}

//===----- HANDLES: DECREASE_KEY & ERASE ---------------------------------------===//

TEST(PairingHeapHandle, EmplaceReturnsValidHandle) {
  MaxPairingHeap<int>         heap;
  MaxPairingHeap<int>::Handle h = heap.emplace(10);
  EXPECT_TRUE(h.valid());
  MaxPairingHeap<int>::Handle empty;
  EXPECT_FALSE(empty.valid());
}

TEST(PairingHeapHandle, DecreaseKeyRaisesToTopMaxHeap) {
  MaxPairingHeap<int> heap;
  heap.emplace(10);
  auto h = heap.emplace(20);
  heap.emplace(30);
  EXPECT_EQ(heap.top(), 30);

  heap.decrease_key(h, 99); // max-heap: "raise priority" means larger
  EXPECT_EQ(heap.top(), 99);
  EXPECT_EQ(heap.extract_top(), 99);
  EXPECT_EQ(heap.top(), 30);
}

TEST(PairingHeapHandle, DecreaseKeyRaisesToTopMinHeap) {
  MinPairingHeap<int> heap;
  heap.emplace(10);
  auto h = heap.emplace(20);
  heap.emplace(5);
  EXPECT_EQ(heap.top(), 5);

  heap.decrease_key(h, 1); // min-heap: "raise priority" means smaller
  EXPECT_EQ(heap.top(), 1);
  EXPECT_EQ(heap.extract_top(), 1);
  EXPECT_EQ(heap.top(), 5);
}

TEST(PairingHeapHandle, DecreaseKeyRejectsPriorityDrop) {
  MaxPairingHeap<int> heap;
  auto                h = heap.emplace(50);
  EXPECT_THROW(heap.decrease_key(h, 40), HeapException);
  EXPECT_EQ(heap.top(), 50);
  EXPECT_EQ(heap.size(), 1U);
}

TEST(PairingHeapHandle, DecreaseKeyEqualValueIsAllowed) {
  MaxPairingHeap<int> heap;
  auto                h = heap.emplace(50);
  heap.emplace(70);
  EXPECT_NO_THROW(heap.decrease_key(h, 50));
  EXPECT_EQ(heap.top(), 70);
}

TEST(PairingHeapHandle, EraseRemovesArbitraryElement) {
  MaxPairingHeap<int> heap;
  heap.emplace(10);
  auto h20 = heap.emplace(20);
  heap.emplace(30);
  heap.emplace(5);
  EXPECT_EQ(heap.size(), 4U);

  heap.erase(h20);
  EXPECT_EQ(heap.size(), 3U);
  pairing_test::expect_drains_in_order(std::move(heap), {10, 30, 5}, std::greater<int>{});
}

TEST(PairingHeapHandle, EraseTopElement) {
  MinPairingHeap<int> heap;
  auto                h = heap.emplace(1);
  heap.emplace(2);
  heap.emplace(3);
  EXPECT_EQ(heap.top(), 1);
  heap.erase(h);
  EXPECT_EQ(heap.top(), 2);
  EXPECT_EQ(heap.size(), 2U);
}

TEST(PairingHeapHandle, InvalidHandleThrows) {
  MaxPairingHeap<int>         heap;
  MaxPairingHeap<int>::Handle empty;
  EXPECT_THROW(heap.erase(empty), HeapException);
  EXPECT_THROW(heap.decrease_key(empty, 5), HeapException);
}

TEST(PairingHeapHandle, HandlesStayValidAcrossMerge) {
  MaxPairingHeap<int> a;
  auto                ha = a.emplace(10);
  MaxPairingHeap<int> b{100, 200, 300};
  a.merge(std::move(b));
  a.decrease_key(ha, 999);
  EXPECT_EQ(a.top(), 999);
}

//===----- EDGE CASES ----------------------------------------------------------===//

TEST(PairingHeapEdge, EmptyAccessThrows) {
  MaxPairingHeap<int> heap;
  EXPECT_THROW((void)heap.top(), HeapException);
  EXPECT_THROW((void)heap.extract_top(), HeapException);
}

TEST(PairingHeapEdge, SingleElement) {
  MinPairingHeap<int> heap;
  heap.insert(42);
  EXPECT_EQ(heap.top(), 42);
  EXPECT_EQ(heap.extract_top(), 42);
  EXPECT_TRUE(heap.is_empty());
}

TEST(PairingHeapEdge, AllDuplicates) {
  MaxPairingHeap<int> heap;
  for (int i = 0; i < 16; ++i) {
    heap.insert(7);
  }
  EXPECT_EQ(heap.size(), 16U);
  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ(heap.extract_top(), 7);
  }
  EXPECT_TRUE(heap.is_empty());
}

// After many inserts with no extraction the root owns a very long child chain;
// this exercises the iterative teardown and combine buffers on that shape.
TEST(PairingHeapEdge, ManyInsertsThenDrainSorted) {
  MinPairingHeap<int> heap;
  std::vector<int>    input;
  for (int i = 5'000; i > 0; --i) {
    heap.insert(i);
    input.push_back(i);
  }
  pairing_test::expect_drains_in_order(std::move(heap), input, std::less<int>{});
}

//===----- RANDOMIZED ORACLE ---------------------------------------------------===//

TEST(PairingHeapRandomized, MaxMatchesPriorityQueue) {
  pairing_test::expect_matches_oracle<MaxPairingHeap<int>, pairing_test::MaxOracle>(/*seed=*/1'234, /*ops=*/20'000);
}

TEST(PairingHeapRandomized, MinMatchesPriorityQueue) {
  pairing_test::expect_matches_oracle<MinPairingHeap<int>, pairing_test::MinOracle>(/*seed=*/9'876, /*ops=*/20'000);
}

//===----- STRUCTURAL INVARIANT (WHITE-BOX) ------------------------------------===//

TEST(PairingHeapInvariant, HoldsUnderRandomizedInsertExtract) {
  using Heap      = MaxPairingHeap<int>;
  using Inspector = ads::heaps::detail::PairingHeapInspector<Heap>;

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
    ASSERT_TRUE(Inspector::valid(heap)) << "pairing invariant broken at op " << i;
  }
}

TEST(PairingHeapInvariant, HoldsUnderRandomizedHandleOps) {
  using Heap      = MinPairingHeap<int>;
  using Inspector = ads::heaps::detail::PairingHeapInspector<Heap>;

  // Every operation acts through a handle we track precisely, so heap size stays
  // in lock-step with our handle vector. (extract_top removes the top, whose
  // handle we could not identify here; it is covered by the test above.)
  Heap                               heap;
  std::vector<Heap::Handle>          handles;
  std::vector<int>                   values;
  std::mt19937                       rng(4'321);
  std::uniform_int_distribution<int> value(0, 100'000);
  std::uniform_int_distribution<int> action(0, 2);

  for (int i = 0; i < 3'000; ++i) {
    int op = handles.size() < 4 ? 0 : action(rng);
    if (op == 0) { // insert
      int v = value(rng);
      handles.push_back(heap.emplace(v));
      values.push_back(v);
    } else if (op == 1) { // decrease_key (min-heap: lower the value)
      std::size_t idx = rng() % handles.size();
      int         nv  = values[idx] == 0 ? 0 : static_cast<int>(rng() % static_cast<unsigned>(values[idx]));
      heap.decrease_key(handles[idx], nv);
      values[idx] = nv;
    } else { // erase
      std::size_t idx = rng() % handles.size();
      heap.erase(handles[idx]);
      handles.erase(handles.begin() + static_cast<std::ptrdiff_t>(idx));
      values.erase(values.begin() + static_cast<std::ptrdiff_t>(idx));
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

TEST(PairingHeapPayload, MoveOnlyValues) {
  using pairing_test::MoveOnlyOrdered;
  PairingHeap<MoveOnlyOrdered> heap;

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

TEST(PairingHeapComparator, OrdersByAbsoluteValue) {
  PairingHeap<int, pairing_test::AbsLess> heap;
  for (int v : {-5, 3, -1, 4, -9}) {
    heap.insert(v);
  }
  EXPECT_EQ(heap.top(), -9);
  EXPECT_EQ(heap.extract_top(), -9);
  EXPECT_EQ(heap.extract_top(), -5);
  EXPECT_EQ(heap.extract_top(), 4);
}

//===---------------------------------------------------------------------------===//
