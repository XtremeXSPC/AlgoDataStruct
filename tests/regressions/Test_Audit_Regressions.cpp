//===---------------------------------------------------------------------------===//
/**
 * @file Test_Audit_Regressions.cpp
 * @brief Regression tests for defects found by the 2026-07 full-library audit.
 * @version 0.1
 * @date 2026-07-16
 *
 * @details Covers the three critical defect groups fixed after the audit
 *          (docs/Audits/AUDIT_Full_Library_2026-07.md):
 *          - F1: use-after-free on self-aliased insertion with reallocation.
 *            The value checks catch the resulting corruption even without
 *            AddressSanitizer; sanitizer builds catch the invalid access too.
 *          - AVL tree loss: a throwing comparator mid insert/remove must leave
 *            the tree intact (strong guarantee).
 *          - F5: operations on moved-from hash structures must be well defined
 *            (the pre-fix code divided by a zero bucket capacity).
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Circular_Array.hpp"
#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/arrays/Gap_Buffer.hpp"
#include "ads/arrays/Static_Vector.hpp"
#include "ads/associative/Hash_Map.hpp"
#include "ads/hash/Hash_Table_Chaining.hpp"
#include "ads/hash/Hash_Table_Open_Addressing.hpp"
#include "ads/heaps/Leftist_Heap.hpp"
#include "ads/heaps/Min_Heap.hpp"
#include "ads/heaps/Pairing_Heap.hpp"
#include "ads/matrices/Sparse_Matrix.hpp"
#include "ads/probabilistic/Bloom_Filter.hpp"
#include "ads/queues/Circular_Array_Deque.hpp"
#include "ads/queues/Circular_Array_Queue.hpp"
#include "ads/queues/Priority_Queue.hpp"
#include "ads/stacks/Array_Stack.hpp"
#include "ads/stacks/Linked_Stack.hpp"
#include "ads/stacks/Static_Stack.hpp"
#include "ads/trees/Nary_Tree.hpp"
#include "ads/trees/search/AVL_Tree.hpp"
#include "ads/trees/trie/Trie.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

// Long enough to defeat SSO so a stale reference dereferences freed heap.
const std::string kSeed = "payload-string-long-enough-to-defeat-small-string-optimization";

//===----- F1: SELF-ALIASED INSERTION ACROSS REALLOCATION -----------------------===//

TEST(AuditAliasingRegression, DynamicArrayPushBackOwnElementSurvivesGrowth) {
  ads::arrays::DynamicArray<std::string> v;
  v.push_back(kSeed);
  for (int i = 0; i < 100; ++i) {
    v.push_back(v[0]); // aliases element 0 across every growth boundary
  }
  for (int i = 0; i < 20; ++i) {
    v.insert(0, v.back()); // positional insert aliasing the last element
  }
  ASSERT_EQ(v.size(), 121U);
  for (const auto& i : v) {
    EXPECT_EQ(i, kSeed);
  }
}

TEST(AuditAliasingRegression, CircularArrayPushesOwnElementSurvivesGrowth) {
  ads::arrays::CircularArray<std::string> arr;
  arr.push_back(kSeed);
  for (int i = 0; i < 60; ++i) {
    arr.push_front(arr.back());
    arr.push_back(arr.front());
  }
  ASSERT_EQ(arr.size(), 121U);
  EXPECT_EQ(arr.front(), kSeed);
  EXPECT_EQ(arr.back(), kSeed);
}

TEST(AuditAliasingRegression, GapBufferInsertOwnElementSurvivesGrowth) {
  ads::arrays::GapBuffer<std::string> gb;
  gb.insert(kSeed);
  for (int i = 0; i < 100; ++i) {
    gb.insert(gb[0]); // gap fills up and the buffer regrows mid-loop
  }
  ASSERT_EQ(gb.size(), 101U);
  EXPECT_EQ(gb[0], kSeed);
  EXPECT_EQ(gb[gb.size() - 1], kSeed);
}

TEST(AuditAliasingRegression, ArrayStackPushOwnTopSurvivesGrowth) {
  ads::stacks::ArrayStack<std::string> st;
  st.push(kSeed);
  for (int i = 0; i < 100; ++i) {
    st.push(st.top());
  }
  ASSERT_EQ(st.size(), 101U);
  EXPECT_EQ(st.top(), kSeed);
}

TEST(AuditAliasingRegression, CircularArrayQueueEnqueueOwnFrontSurvivesGrowth) {
  ads::queues::CircularArrayQueue<std::string> q;
  q.enqueue(kSeed);
  for (int i = 0; i < 100; ++i) {
    q.enqueue(q.front());
  }
  ASSERT_EQ(q.size(), 101U);
  EXPECT_EQ(q.rear(), kSeed);
}

TEST(AuditAliasingRegression, CircularArrayDequePushesOwnElementSurvivesGrowth) {
  ads::queues::CircularArrayDeque<std::string> dq;
  dq.push_back(kSeed);
  for (int i = 0; i < 60; ++i) {
    dq.push_front(dq.back());
    dq.push_back(dq.front());
  }
  ASSERT_EQ(dq.size(), 121U);
  EXPECT_EQ(dq.front(), kSeed);
  EXPECT_EQ(dq.back(), kSeed);
}

TEST(AuditAliasingRegression, HeapsInsertOwnTopSurvivesGrowth) {
  ads::heaps::MinHeap<std::string> heap;
  heap.insert(kSeed);
  for (int i = 0; i < 100; ++i) {
    heap.insert(heap.top()); // propagates through DynamicArray::push_back
  }
  ASSERT_EQ(heap.size(), 101U);
  EXPECT_EQ(heap.top(), kSeed);

  ads::queues::PriorityQueue<std::string> pq;
  pq.push(kSeed);
  for (int i = 0; i < 100; ++i) {
    pq.push(pq.top());
  }
  ASSERT_EQ(pq.size(), 101U);
  EXPECT_EQ(pq.top(), kSeed);
}

TEST(AuditAliasingRegression, HashTableChainingInsertOwnValueSurvivesRehash) {
  ads::hash::HashTableChaining<int, std::string> table;
  table.insert(0, kSeed);
  for (int i = 1; i < 100; ++i) {
    table.insert(i, table.at(0)); // value aliases the table across rehashes
  }
  ASSERT_EQ(table.size(), 100U);
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(table.at(i), kSeed);
  }
}

TEST(AuditAliasingRegression, HashTableOpenAddressingInsertOwnValueSurvivesRehash) {
  ads::hash::HashTableOpenAddressing<int, std::string> table;
  table.insert(0, kSeed);
  for (int i = 1; i < 100; ++i) {
    table.insert(i, table.at(0)); // value aliases the table across rehashes
  }
  ASSERT_EQ(table.size(), 100U);
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(table.at(i), kSeed);
  }
}

//===----- AVL: STRONG GUARANTEE UNDER A THROWING COMPARATOR --------------------===//

int g_comparisons_until_throw = -1; // -1 = disarmed

auto tick() -> void {
  if (g_comparisons_until_throw > 0 && --g_comparisons_until_throw == 0) {
    throw std::runtime_error("comparator failure injection");
  }
}

struct Fragile {
  int v = 0;

  friend auto operator<(const Fragile& a, const Fragile& b) -> bool {
    tick();
    return a.v < b.v;
  }

  friend auto operator==(const Fragile& a, const Fragile& b) -> bool { return a.v == b.v; }

  friend auto operator>(const Fragile& a, const Fragile& b) -> bool { return a.v > b.v; }

  friend auto operator<=(const Fragile& a, const Fragile& b) -> bool { return a.v <= b.v; }

  friend auto operator>=(const Fragile& a, const Fragile& b) -> bool { return a.v >= b.v; }
};

constexpr int kAvlCount = 64;

auto build_fragile_tree() -> ads::trees::AVLTree<Fragile> {
  ads::trees::AVLTree<Fragile> tree;
  for (int i = 0; i < kAvlCount; ++i) {
    tree.insert(Fragile{i});
  }
  return tree;
}

auto expect_intact(const ads::trees::AVLTree<Fragile>& tree) -> void {
  ASSERT_EQ(tree.size(), static_cast<size_t>(kAvlCount));
  EXPECT_TRUE(tree.validate_properties());
  for (int i = 0; i < kAvlCount; ++i) {
    EXPECT_TRUE(tree.contains(Fragile{i}));
  }
}

TEST(AuditAvlExceptionSafety, ThrowingComparatorDuringInsertLeavesTreeIntact) {
  // Inject a throw at every possible comparison depth of the descent.
  for (int fuse = 1; fuse <= 16; ++fuse) {
    auto tree                 = build_fragile_tree();
    g_comparisons_until_throw = fuse;
    bool threw                = false;
    try {
      tree.insert(Fragile{kAvlCount + 1});
    } catch (const std::runtime_error&) {
      threw = true;
    }
    g_comparisons_until_throw = -1;
    if (threw) {
      expect_intact(tree); // strong guarantee: nothing changed
    }
  }
}

TEST(AuditAvlExceptionSafety, ThrowingComparatorDuringRemoveLeavesTreeIntact) {
  for (int fuse = 1; fuse <= 16; ++fuse) {
    auto tree                 = build_fragile_tree();
    g_comparisons_until_throw = fuse;
    bool threw                = false;
    try {
      (void)tree.remove(Fragile{kAvlCount - 1});
    } catch (const std::runtime_error&) {
      threw = true;
    }
    g_comparisons_until_throw = -1;
    if (threw) {
      expect_intact(tree);
    }
  }
}

//===----- F5: MOVED-FROM HASH STRUCTURES ---------------------------------------===//

TEST(AuditMovedFromRegression, ChainingTableIsUsableAfterMove) {
  ads::hash::HashTableChaining<int, std::string> source;
  source.insert(1, "one");
  auto sink(std::move(source));

  // Reads on the husk must report "not found" instead of dividing by zero.
  EXPECT_FALSE(source.contains(1));
  EXPECT_EQ(source.find(1), nullptr);
  EXPECT_EQ(source.count(1), 0U);
  EXPECT_FALSE(source.erase(1));
  EXPECT_THROW((void)source.at(1), ads::hash::KeyNotFoundException);

  // Writes must re-initialize the husk into a working table.
  EXPECT_TRUE(source.insert(7, "seven"));
  EXPECT_EQ(source.at(7), "seven");
  source[8] = "eight";
  EXPECT_EQ(source.at(8), "eight");
  EXPECT_EQ(sink.at(1), "one");
}

TEST(AuditMovedFromRegression, HashMapIsUsableAfterMove) {
  ads::associative::HashMap<int, std::string> source;
  source.insert({1, "one"});
  auto sink(std::move(source));

  EXPECT_EQ(source.find(1), source.end());
  EXPECT_FALSE(source.contains(1));
  EXPECT_EQ(source.count(1), 0U);
  EXPECT_EQ(source.begin(), source.end());
  EXPECT_FALSE(source.erase(1));

  source[2] = "two";
  EXPECT_EQ(source.at(2), "two");
  EXPECT_NE(source.find(2), source.end());
  EXPECT_EQ(sink.at(1), "one");
}

TEST(AuditMovedFromRegression, BloomFilterIsSafeAfterMove) {
  ads::probabilistic::BloomFilter<std::string> source(256, 3);
  source.insert("hello");
  auto sink(std::move(source));

  EXPECT_FALSE(source.might_contain("hello"));
  EXPECT_THROW(source.insert("world"), ads::probabilistic::BloomFilterException);
  EXPECT_TRUE(sink.might_contain("hello"));
}

//===----- F2: OVER-ALIGNED ELEMENT TYPES ----------------------------------------===//

struct alignas(64) OverAligned {
  double lanes[8] = {};
};

TEST(AuditAlignmentRegression, RawStorageContainersHonorOverAlignment) {
  ads::arrays::DynamicArray<OverAligned> v;
  for (int i = 0; i < 20; ++i) {
    v.push_back(OverAligned{});
  }
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(v.data()) % alignof(OverAligned), 0U);

  ads::stacks::ArrayStack<OverAligned> st;
  for (int i = 0; i < 20; ++i) {
    st.push(OverAligned{});
  }
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(&st.top()) % alignof(OverAligned), 0U);
}

//===----- F3: ROLLBACK IN Static* CONSTRUCTORS ----------------------------------===//

int g_leak_live         = 0;  // constructed minus destroyed instances
int g_moves_until_throw = -1; // -1 = disarmed

struct LeakProbe {
  int payload = 0;

  LeakProbe() { ++g_leak_live; }

  explicit LeakProbe(int p) : payload(p) { ++g_leak_live; }

  LeakProbe(const LeakProbe& other) : payload(other.payload) { ++g_leak_live; }

  LeakProbe(LeakProbe&& other) noexcept : payload(other.payload) {
    if (g_moves_until_throw > 0 && --g_moves_until_throw == 0) {
      throw std::runtime_error("move failure injection");
    }
    ++g_leak_live;
  }

  auto operator=(const LeakProbe&) -> LeakProbe& = default;
  auto operator=(LeakProbe&&) -> LeakProbe&      = default;

  ~LeakProbe() { --g_leak_live; }
};

TEST(AuditStaticRollbackRegression, ThrowingMoveDoesNotLeakConstructedElements) {
  using ProbeVector = ads::arrays::StaticVector<LeakProbe, 64>;
  g_leak_live       = 0;
  {
    ProbeVector source;
    for (int i = 0; i < 32; ++i) {
      source.emplace_back(i);
    }
    g_moves_until_throw = 10; // fail on the 10th element move
    EXPECT_THROW({ ProbeVector sink(std::move(source)); }, std::runtime_error);
    g_moves_until_throw = -1;
  }
  // Pre-fix, the elements built before the throw were never destroyed.
  EXPECT_EQ(g_leak_live, 0);
}

//===----- F4: DEEP TEARDOWN WITHOUT RECURSION OR ALLOCATION ---------------------===//

TEST(AuditTeardownRegression, PairingHeapClearsLongChildChain) {
  ads::heaps::PairingHeap<int> heap;
  for (int i = 0; i < 200'000; ++i) {
    heap.insert(i); // the root accumulates one child per insert
  }
  heap.clear(); // must not recurse through the 200k-long chain
  EXPECT_TRUE(heap.is_empty());
}

TEST(AuditTeardownRegression, NaryTreeClearsDeepChain) {
  ads::trees::NaryTree<int> tree;
  auto*                     node = tree.set_root(0);
  for (int i = 1; i < 150'000; ++i) {
    node = tree.add_child(node, i);
  }
  tree.clear(); // parent-pointer teardown: no recursion, no allocation
  EXPECT_TRUE(tree.is_empty());
}

TEST(AuditTeardownRegression, TrieHandlesVeryLongWords) {
  ads::trees::Trie<false> trie;
  const std::string       deep(100'000, 'a');
  trie.insert(deep);
  EXPECT_TRUE(trie.search(deep));
  trie.clear(); // iterative teardown of a 100k-deep chain
  EXPECT_FALSE(trie.search(deep));
}

//===----- ASSORTED MINOR-FIX REGRESSIONS ----------------------------------------===//

TEST(AuditMinorRegression, TrieLookupOfNonLowercaseCharsIsNotFound) {
  ads::trees::Trie<false> trie;
  trie.insert("hello");
  EXPECT_FALSE(trie.search("HELLO"));                        // pre-fix: threw std::invalid_argument
  EXPECT_FALSE(trie.starts_with("HE"));                      // lookups never throw on foreign chars
  EXPECT_FALSE(trie.remove("HEL-LO"));                       // remove is a lookup too
  EXPECT_THROW(trie.insert("Hello"), std::invalid_argument); // inserts still reject
}

TEST(AuditMinorRegression, LeftistHeapMergeRejectsIncompatibleComparators) {
  using Cmp = std::function<bool(const int&, const int&)>;
  ads::heaps::LeftistHeap<int, Cmp> a(Cmp{[](const int& x, const int& y) -> bool { return x < y; }});
  ads::heaps::LeftistHeap<int, Cmp> b(Cmp{[](const int& x, const int& y) -> bool { return x > y; }});
  a.insert(1);
  b.insert(2);
  EXPECT_THROW(a.merge(std::move(b)), ads::heaps::HeapException);
}

TEST(AuditMinorRegression, StacksSupportMoveOnlyValues) {
  // Pre-fix: the virtual push(const T&) override failed to instantiate for
  // move-only T, making every Stack<T> implementation unusable with unique_ptr.
  ads::stacks::ArrayStack<std::unique_ptr<int>>     array_stack;
  ads::stacks::LinkedStack<std::unique_ptr<int>>    linked_stack;
  ads::stacks::StaticStack<std::unique_ptr<int>, 4> static_stack;

  array_stack.push(std::make_unique<int>(1));
  linked_stack.push(std::make_unique<int>(2));
  static_stack.push(std::make_unique<int>(3));
  EXPECT_EQ(*array_stack.top(), 1);
  EXPECT_EQ(*linked_stack.top(), 2);
  EXPECT_EQ(*static_stack.top(), 3);

  // The copy overload stays callable through the interface but must throw.
  const std::unique_ptr<int> lvalue;
  EXPECT_THROW(array_stack.push(lvalue), ads::stacks::StackException);
  EXPECT_THROW(linked_stack.push(lvalue), ads::stacks::StackException);
  EXPECT_THROW(static_stack.push(lvalue), ads::stacks::StackException);
}

TEST(AuditMinorRegression, SparseMatrixIsConsistentAfterMove) {
  ads::matrices::SparseMatrix<double> source(3, 3);
  source.set(1, 1, 2.5);
  auto sink(std::move(source));

  // The husk must be a coherent empty 0x0 matrix, and clear() must stay safe.
  EXPECT_EQ(source.row_count(), 0U);
  EXPECT_EQ(source.column_count(), 0U);
  EXPECT_EQ(source.non_zero_count(), 0U);
  source.clear();
  EXPECT_EQ(sink.value_at(1, 1), 2.5);
}

} // namespace

//===---------------------------------------------------------------------------===//
