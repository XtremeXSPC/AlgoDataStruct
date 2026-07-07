//===---------------------------------------------------------------------------===//
/**
 * @file Test_Binomial_Heap.cpp
 * @brief Google Test unit tests for the BinomialHeap meldable heap.
 * @version 0.1
 * @date 2026-07-07
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "Meldable_Heap_Test_Support.hpp"
#include "ads/heaps/Binomial_Heap.hpp"

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

// White-box inspector (friend of BinomialHeap): verifies the structural binomial
// invariants (root list sorted by strictly increasing degree; every tree is a
// well-formed binomial tree of its stated degree), heap order, the element count,
// and that every handle record points back at the node holding it.
template <typename Heap>
struct BinomialHeapInspector {
  using Node = typename Heap::Node;

  // A binomial tree of degree k has children of degree k-1, k-2, ..., 0 in
  // child/sibling order, each a valid binomial tree, all obeying heap order.
  static auto valid_tree(const Heap& heap, const Node* node) -> bool {
    int         expected_child_degree = node->degree - 1;
    std::size_t child_count           = 0;
    for (const Node* c = node->child.get(); c != nullptr; c = c->sibling.get()) {
      if (c->parent != node) {
        return false;
      }
      if (c->degree != expected_child_degree) {
        return false;
      }
      if (heap.comp_(node->value, c->value)) { // parent must not rank below child
        return false;
      }
      if (!valid_tree(heap, c)) {
        return false;
      }
      --expected_child_degree;
      ++child_count;
    }
    return child_count == static_cast<std::size_t>(node->degree);
  }

  static auto count(const Node* node) -> std::size_t {
    std::size_t total = 0;
    for (const Node* c = node ? node->child.get() : nullptr; c != nullptr; c = c->sibling.get()) {
      total += count(c);
    }
    return node ? total + 1 : 0;
  }

  // Confirms every handle record in the subtree points back at its owner node.
  static auto handles_consistent(const Node* node) -> bool {
    if (!node) {
      return true;
    }
    if (!node->handle || node->handle->node != node) {
      return false;
    }
    for (const Node* c = node->child.get(); c != nullptr; c = c->sibling.get()) {
      if (!handles_consistent(c)) {
        return false;
      }
    }
    return true;
  }

  static auto valid(const Heap& heap) -> bool {
    int         previous_degree = -1;
    std::size_t total           = 0;
    for (const Node* root = heap.head_.get(); root != nullptr; root = root->sibling.get()) {
      if (root->parent != nullptr) {
        return false;
      }
      if (root->degree <= previous_degree) { // strictly increasing degrees
        return false;
      }
      previous_degree = root->degree;
      if (!valid_tree(heap, root) || !handles_consistent(root)) {
        return false;
      }
      total += count(root);
    }
    return total == heap.size_;
  }
};

} // namespace ads::heaps::detail

namespace binomial_test = ads::heaps::meldable_test;

//===----- CONCEPT & TYPE-TRAIT CHECKS -----------------------------------------===//

static_assert(MeldableHeap<MaxBinomialHeap<int>>);
static_assert(MeldableHeap<MinBinomialHeap<int>>);
static_assert(MeldableHeap<BinomialHeap<std::string>>);
static_assert(MeldableHeap<BinomialHeap<binomial_test::MoveOnlyOrdered>>);

static_assert(std::move_constructible<MaxBinomialHeap<int>>);
static_assert(!std::copy_constructible<MaxBinomialHeap<int>>);
static_assert(!std::is_copy_assignable_v<MaxBinomialHeap<int>>);

//===----- BASIC STATE ---------------------------------------------------------===//

TEST(BinomialHeapBasic, EmptyOnConstruction) {
  MaxBinomialHeap<int> heap;
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
}

TEST(BinomialHeapBasic, InsertUpdatesTopAndSize) {
  MaxBinomialHeap<int> heap;
  heap.insert(30);
  EXPECT_EQ(heap.top(), 30);
  heap.insert(50);
  EXPECT_EQ(heap.top(), 50);
  heap.insert(40);
  EXPECT_EQ(heap.top(), 50);
  EXPECT_EQ(heap.size(), 3U);
  EXPECT_FALSE(heap.is_empty());
}

TEST(BinomialHeapBasic, MinHeapTopIsMinimum) {
  MinBinomialHeap<int> heap;
  heap.insert(30);
  heap.insert(10);
  heap.insert(20);
  EXPECT_EQ(heap.top(), 10);
}

TEST(BinomialHeapBasic, ClearEmptiesHeap) {
  MaxBinomialHeap<int> heap{1, 2, 3, 4, 5};
  heap.clear();
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
  heap.insert(7);
  EXPECT_EQ(heap.top(), 7);
}

//===----- CONSTRUCTION --------------------------------------------------------===//

TEST(BinomialHeapConstruction, InitializerList) {
  binomial_test::expect_drains_in_order(MaxBinomialHeap<int>{3, 1, 4, 1, 5, 9, 2, 6}, {3, 1, 4, 1, 5, 9, 2, 6}, std::greater<int>{});
}

TEST(BinomialHeapConstruction, FromVector) {
  std::vector<int> src{7, 2, 9, 4, 4, 1};
  binomial_test::expect_drains_in_order(MinBinomialHeap<int>(src), src, std::less<int>{});
}

TEST(BinomialHeapConstruction, FromIteratorRange) {
  std::vector<int> src{7, 2, 9, 4, 4, 1};
  binomial_test::expect_drains_in_order(MaxBinomialHeap<int>(src.begin(), src.end()), src, std::greater<int>{});
}

TEST(BinomialHeapConstruction, MoveConstructorTransfersAndEmptiesSource) {
  MaxBinomialHeap<int> src{5, 1, 8, 3};
  MaxBinomialHeap<int> dst(std::move(src));
  EXPECT_TRUE(src.is_empty());
  EXPECT_EQ(src.size(), 0U);
  binomial_test::expect_drains_in_order(std::move(dst), {5, 1, 8, 3}, std::greater<int>{});
}

TEST(BinomialHeapConstruction, MoveAssignmentReplacesTarget) {
  MaxBinomialHeap<int> src{5, 1, 8, 3};
  MaxBinomialHeap<int> dst{100, 200};
  dst = std::move(src);
  EXPECT_TRUE(src.is_empty());
  binomial_test::expect_drains_in_order(std::move(dst), {5, 1, 8, 3}, std::greater<int>{});
}

//===----- MELD ----------------------------------------------------------------===//

TEST(BinomialHeapMerge, CombinesBothHeaps) {
  MaxBinomialHeap<int> a{5, 1, 8};
  MaxBinomialHeap<int> b{3, 9, 2};
  a.merge(std::move(b));
  EXPECT_TRUE(b.is_empty());
  EXPECT_EQ(b.size(), 0U);
  binomial_test::expect_drains_in_order(std::move(a), {5, 1, 8, 3, 9, 2}, std::greater<int>{});
}

TEST(BinomialHeapMerge, WithEmptyHeapsBothDirections) {
  MaxBinomialHeap<int> filled{4, 2};
  MaxBinomialHeap<int> empty1;
  filled.merge(std::move(empty1));
  EXPECT_EQ(filled.size(), 2U);

  MaxBinomialHeap<int> empty2;
  empty2.merge(std::move(filled));
  EXPECT_TRUE(filled.is_empty());
  binomial_test::expect_drains_in_order(std::move(empty2), {4, 2}, std::greater<int>{});
}

TEST(BinomialHeapMerge, SelfMergeIsNoOp) {
  MaxBinomialHeap<int> h{4, 2, 7};
  h.merge(std::move(h)); // NOLINT(bugprone-use-after-move): self-merge guard
  EXPECT_EQ(h.size(), 3U);
  EXPECT_EQ(h.top(), 7);
}

TEST(BinomialHeapMerge, IncompatibleStatefulComparatorsThrowWithoutMutation) {
  BinomialHeap<int, binomial_test::StatefulOrder> max_heap(binomial_test::StatefulOrder{false});
  BinomialHeap<int, binomial_test::StatefulOrder> min_heap(binomial_test::StatefulOrder{true});
  max_heap.insert(10);
  min_heap.insert(1);
  min_heap.insert(2);

  EXPECT_THROW(max_heap.merge(std::move(min_heap)), HeapException);
  EXPECT_EQ(max_heap.size(), 1U);
  EXPECT_EQ(max_heap.top(), 10);
  EXPECT_EQ(min_heap.size(), 2U);
  EXPECT_EQ(min_heap.top(), 1);
}

TEST(BinomialHeapMerge, EqualStatefulComparatorsMerge) {
  using Heap = BinomialHeap<int, binomial_test::StatefulOrder>;
  binomial_test::expect_equal_stateful_comparators_merge<Heap>();
}

//===----- HANDLES: DECREASE_KEY & ERASE ---------------------------------------===//

TEST(BinomialHeapHandle, EmplaceReturnsValidHandle) {
  MaxBinomialHeap<int>         heap;
  MaxBinomialHeap<int>::Handle h = heap.emplace(10);
  EXPECT_TRUE(h.valid());
  MaxBinomialHeap<int>::Handle empty;
  EXPECT_FALSE(empty.valid());
}

TEST(BinomialHeapHandle, DecreaseKeyRaisesToTopMaxHeap) {
  MaxBinomialHeap<int> heap;
  heap.emplace(10);
  auto h = heap.emplace(20);
  heap.emplace(30); // current top
  EXPECT_EQ(heap.top(), 30);

  heap.decrease_key(h, 99); // for a max-heap, "raise priority" means larger
  EXPECT_EQ(heap.top(), 99);
  EXPECT_EQ(heap.extract_top(), 99);
  EXPECT_EQ(heap.top(), 30);
}

TEST(BinomialHeapHandle, DecreaseKeyRaisesToTopMinHeap) {
  MinBinomialHeap<int> heap;
  heap.emplace(10);
  auto h = heap.emplace(20);
  heap.emplace(5); // current top
  EXPECT_EQ(heap.top(), 5);

  heap.decrease_key(h, 1); // for a min-heap, "raise priority" means smaller
  EXPECT_EQ(heap.top(), 1);
  EXPECT_EQ(heap.extract_top(), 1);
  EXPECT_EQ(heap.top(), 5);
}

TEST(BinomialHeapHandle, DecreaseKeyRejectsPriorityDrop) {
  MaxBinomialHeap<int> heap;
  auto                 h = heap.emplace(50);
  EXPECT_THROW(heap.decrease_key(h, 40), HeapException); // 40 < 50 lowers priority
  EXPECT_EQ(heap.top(), 50);
  EXPECT_EQ(heap.size(), 1U);
}

TEST(BinomialHeapHandle, DecreaseKeyEqualValueIsAllowed) {
  MaxBinomialHeap<int> heap;
  auto                 h = heap.emplace(50);
  heap.emplace(70);
  EXPECT_NO_THROW(heap.decrease_key(h, 50)); // equal priority is permitted
  EXPECT_EQ(heap.top(), 70);
}

TEST(BinomialHeapHandle, EraseRemovesArbitraryElement) {
  MaxBinomialHeap<int> heap;
  heap.emplace(10);
  auto h20 = heap.emplace(20);
  heap.emplace(30);
  heap.emplace(5);
  EXPECT_EQ(heap.size(), 4U);

  heap.erase(h20);
  EXPECT_EQ(heap.size(), 3U);
  binomial_test::expect_drains_in_order(std::move(heap), {10, 30, 5}, std::greater<int>{});
}

TEST(BinomialHeapHandle, EraseTopElement) {
  MinBinomialHeap<int> heap;
  auto                 h = heap.emplace(1);
  heap.emplace(2);
  heap.emplace(3);
  EXPECT_EQ(heap.top(), 1);
  heap.erase(h);
  EXPECT_EQ(heap.top(), 2);
  EXPECT_EQ(heap.size(), 2U);
}

TEST(BinomialHeapHandle, InvalidHandleThrows) {
  MaxBinomialHeap<int>         heap;
  MaxBinomialHeap<int>::Handle empty;
  EXPECT_THROW(heap.erase(empty), HeapException);
  EXPECT_THROW(heap.decrease_key(empty, 5), HeapException);
}

TEST(BinomialHeapHandle, HandlesStayValidAcrossMerge) {
  MaxBinomialHeap<int> a;
  auto                 ha = a.emplace(10);
  MaxBinomialHeap<int> b{100, 200, 300};
  a.merge(std::move(b));
  // 'ha' must still refer to the element 10 after the meld relinked nodes.
  a.decrease_key(ha, 999);
  EXPECT_EQ(a.top(), 999);
}

//===----- EDGE CASES ----------------------------------------------------------===//

TEST(BinomialHeapEdge, EmptyAccessThrows) {
  MaxBinomialHeap<int> heap;
  EXPECT_THROW((void)heap.top(), HeapException);
  EXPECT_THROW((void)heap.extract_top(), HeapException);
}

TEST(BinomialHeapEdge, SingleElement) {
  MinBinomialHeap<int> heap;
  heap.insert(42);
  EXPECT_EQ(heap.top(), 42);
  EXPECT_EQ(heap.extract_top(), 42);
  EXPECT_TRUE(heap.is_empty());
}

TEST(BinomialHeapEdge, AllDuplicates) {
  MaxBinomialHeap<int> heap;
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

TEST(BinomialHeapRandomized, MaxMatchesPriorityQueue) {
  binomial_test::expect_matches_oracle<MaxBinomialHeap<int>, binomial_test::MaxOracle>(/*seed=*/1'234, /*ops=*/20'000);
}

TEST(BinomialHeapRandomized, MinMatchesPriorityQueue) {
  binomial_test::expect_matches_oracle<MinBinomialHeap<int>, binomial_test::MinOracle>(/*seed=*/9'876, /*ops=*/20'000);
}

//===----- STRUCTURAL INVARIANT (WHITE-BOX) ------------------------------------===//

TEST(BinomialHeapInvariant, HoldsUnderRandomizedInsertExtract) {
  using Heap      = MaxBinomialHeap<int>;
  using Inspector = ads::heaps::detail::BinomialHeapInspector<Heap>;

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
    ASSERT_TRUE(Inspector::valid(heap)) << "binomial invariant broken at op " << i;
  }
}

TEST(BinomialHeapInvariant, HoldsUnderRandomizedHandleOps) {
  using Heap      = MinBinomialHeap<int>;
  using Inspector = ads::heaps::detail::BinomialHeapInspector<Heap>;

  // Every operation here acts through a handle we track precisely, so the heap's
  // size stays in lock-step with our handle vector. (extract_top removes the top,
  // whose handle we could not identify here; it is covered structurally by the
  // insert/extract invariant test above.)
  Heap                               heap;
  std::vector<Heap::Handle>          handles;
  std::vector<int>                   values;
  std::mt19937                       rng(4'321);
  std::uniform_int_distribution<int> value(0, 100'000);
  std::uniform_int_distribution<int> action(0, 2);

  for (int i = 0; i < 3'000; ++i) {
    // Force an insert whenever we have too few elements to bias toward growth.
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

  // Drain what remains against a sorted oracle to confirm end-to-end order.
  std::ranges::sort(values);
  for (int expected : values) {
    EXPECT_EQ(heap.extract_top(), expected);
  }
  EXPECT_TRUE(heap.is_empty());
}

//===----- MOVE-ONLY PAYLOAD ---------------------------------------------------===//

TEST(BinomialHeapPayload, MoveOnlyValues) {
  using binomial_test::MoveOnlyOrdered;
  BinomialHeap<MoveOnlyOrdered> heap;

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

TEST(BinomialHeapComparator, OrdersByAbsoluteValue) {
  BinomialHeap<int, binomial_test::AbsLess> heap;
  for (int v : {-5, 3, -1, 4, -9}) {
    heap.insert(v);
  }
  EXPECT_EQ(heap.top(), -9);
  EXPECT_EQ(heap.extract_top(), -9);
  EXPECT_EQ(heap.extract_top(), -5);
  EXPECT_EQ(heap.extract_top(), 4);
}

//===---------------------------------------------------------------------------===//
