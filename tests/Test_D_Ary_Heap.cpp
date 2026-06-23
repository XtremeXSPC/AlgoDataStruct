//===---------------------------------------------------------------------------===//
/**
 * @file Test_D_Ary_Heap.cpp
 * @brief Google Test unit tests for the DAryHeap implementation.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/heaps/D_Ary_Heap.hpp"

#include <gtest/gtest.h>

#include <array>
#include <functional>
#include <string>
#include <vector>

using namespace ads::heaps;

namespace {

//===----- TEST UTILITIES ------------------------------------------------------===//

struct MoveOnlyItem {
  int         priority;
  std::string label;

  MoveOnlyItem(int priority_value, std::string label_value) : priority(priority_value), label(std::move(label_value)) {}

  MoveOnlyItem(const MoveOnlyItem&)                        = delete;
  auto operator=(const MoveOnlyItem&) -> MoveOnlyItem&     = delete;
  MoveOnlyItem(MoveOnlyItem&&) noexcept                    = default;
  auto operator=(MoveOnlyItem&&) noexcept -> MoveOnlyItem& = default;

  [[nodiscard]] auto operator<(const MoveOnlyItem& other) const -> bool { return priority < other.priority; }

  [[nodiscard]] auto operator>(const MoveOnlyItem& other) const -> bool { return priority > other.priority; }
};

} // namespace

static_assert(MoveHeapValue<MoveOnlyItem>);
static_assert(!CopyHeapValue<MoveOnlyItem>);
static_assert(EmplaceHeapValue<MoveOnlyItem, int, std::string>);

// Test fixture for default-arity heap behavior.
class DAryHeapTest : public ::testing::Test {
protected:
  DAryHeap<int> heap;
};

//===----- CONSTRUCTION TESTS --------------------------------------------------===//

TEST_F(DAryHeapTest, IsEmptyOnConstruction) {
  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
  EXPECT_EQ(heap.arity(), 2U);
}

TEST(DAryHeapConstructionTest, RejectsInvalidArity) {
  EXPECT_THROW(static_cast<void>(DAryHeap<int>(0)), HeapException);
  EXPECT_THROW(static_cast<void>(DAryHeap<int>(1)), HeapException);
}

TEST(DAryHeapConstructionTest, AcceptsCustomArityAndCapacity) {
  DAryHeap<int> heap(4, 64);

  EXPECT_EQ(heap.arity(), 4U);
  EXPECT_GE(heap.capacity(), 64U);
}

//===----- ORDERING / ACCESS TESTS ---------------------------------------------===//

TEST_F(DAryHeapTest, InsertAndTopDefaultToMaxHeapSemantics) {
  heap.insert(10);
  heap.insert(40);
  heap.insert(20);

  EXPECT_EQ(heap.top(), 40);
}

TEST(DAryHeapOrderTest, ExtractTopPreservesMaxOrderForFourAryHeap) {
  DAryHeap<int> heap(4);
  for (int value : {5, 3, 7, 1, 9, 2, 8, 4, 6}) {
    heap.insert(value);
  }

  std::vector<int> extracted;
  while (!heap.is_empty()) {
    extracted.push_back(heap.extract_top());
  }

  EXPECT_EQ(extracted, (std::vector<int>{9, 8, 7, 6, 5, 4, 3, 2, 1}));
}

TEST(DAryHeapOrderTest, MinAliasUsesMinHeapSemantics) {
  MinDAryHeap<int> heap(5);
  for (int value : {9, 4, 7, 1, 3, 8}) {
    heap.insert(value);
  }

  std::vector<int> extracted;
  while (!heap.is_empty()) {
    extracted.push_back(heap.extract_top());
  }

  EXPECT_EQ(extracted, (std::vector<int>{1, 3, 4, 7, 8, 9}));
}

TEST(DAryHeapConstructionTest, BuildFromVectorHeapifiesElements) {
  const std::vector<int> values = {10, 30, 20, 50, 40};
  DAryHeap<int>          heap(values, 3);

  EXPECT_EQ(heap.size(), values.size());
  EXPECT_EQ(heap.arity(), 3U);
  EXPECT_EQ(heap.top(), 50);
}

TEST(DAryHeapConstructionTest, BuildFromRangeHeapifiesElements) {
  const std::array<int, 5> values = {10, 30, 20, 50, 40};
  DAryHeap<int>            heap(values.begin(), values.end(), 4);

  EXPECT_EQ(heap.size(), values.size());
  EXPECT_EQ(heap.arity(), 4U);
  EXPECT_EQ(heap.top(), 50);
}

TEST(DAryHeapConstructionTest, InitializerListConstructorHeapifiesElements) {
  DAryHeap<int> heap({11, 7, 15, 3, 9}, 3);

  EXPECT_EQ(heap.size(), 5U);
  EXPECT_EQ(heap.top(), 15);
}

TEST(DAryHeapValueTest, EmplaceConstructsElementInPlace) {
  DAryHeap<std::string> heap(3);
  heap.emplace("beta");
  heap.emplace("alpha");
  heap.emplace("gamma");

  EXPECT_EQ(heap.top(), "gamma");
}

TEST(DAryHeapValueTest, SupportsMoveOnlyValues) {
  DAryHeap<MoveOnlyItem> heap(4);
  heap.emplace(10, "ten");
  heap.emplace(30, "thirty");
  heap.emplace(20, "twenty");

  EXPECT_EQ(heap.top().priority, 30);
  EXPECT_EQ(heap.extract_top().label, "thirty");
  EXPECT_EQ(heap.extract_top().label, "twenty");
  EXPECT_EQ(heap.extract_top().label, "ten");
}

//===----- MUTATION / UTILITY TESTS --------------------------------------------===//

TEST(DAryHeapMutationTest, UpdateKeyCanPromoteElement) {
  DAryHeap<int> heap(4);
  heap.insert(10);
  heap.insert(20);
  heap.insert(30);
  heap.insert(15);

  heap.update_key(3, 40);

  EXPECT_EQ(heap.top(), 40);
}

TEST(DAryHeapMutationTest, UpdateKeyCanDemoteElement) {
  DAryHeap<int> heap(3);
  heap.insert(50);
  heap.insert(40);
  heap.insert(30);
  heap.insert(20);

  heap.update_key(0, 5);

  EXPECT_EQ(heap.top(), 40);
  EXPECT_EQ(heap.extract_top(), 40);
  EXPECT_EQ(heap.extract_top(), 30);
  EXPECT_EQ(heap.extract_top(), 20);
  EXPECT_EQ(heap.extract_top(), 5);
}

TEST(DAryHeapMutationTest, UpdateKeyRejectsInvalidIndex) {
  DAryHeap<int> heap(3);
  heap.insert(10);

  EXPECT_THROW(heap.update_key(4, 20), HeapException);
}

TEST_F(DAryHeapTest, TopAndExtractOnEmptyThrow) {
  EXPECT_THROW(heap.top(), HeapException);
  EXPECT_THROW(heap.extract_top(), HeapException);
}

TEST(DAryHeapUtilityTest, ReserveAndClearWorkWithHigherArity) {
  DAryHeap<int> heap(8);
  heap.reserve(128);

  for (int value = 0; value < 100; ++value) {
    heap.insert(value);
  }

  EXPECT_EQ(heap.top(), 99);
  EXPECT_EQ(heap.size(), 100U);

  heap.clear();

  EXPECT_TRUE(heap.is_empty());
  EXPECT_EQ(heap.size(), 0U);
}

//===----- STRUCTURE / MOVE TESTS ----------------------------------------------===//

TEST(DAryHeapStructureTest, LargeArityStillWorksForSmallHeap) {
  DAryHeap<int> heap(16);
  heap.insert(3);
  heap.insert(9);
  heap.insert(1);

  EXPECT_EQ(heap.extract_top(), 9);
  EXPECT_EQ(heap.extract_top(), 3);
  EXPECT_EQ(heap.extract_top(), 1);
}

TEST(DAryHeapMoveTest, MoveSemanticsPreserveArityAndContents) {
  DAryHeap<int> source(5);
  source.insert(10);
  source.insert(30);
  source.insert(20);

  DAryHeap<int> moved(std::move(source));
  EXPECT_TRUE(source.is_empty());
  EXPECT_EQ(moved.arity(), 5U);
  EXPECT_EQ(moved.top(), 30);

  DAryHeap<int> assigned(3);
  assigned.insert(1);
  assigned = std::move(moved);

  EXPECT_TRUE(moved.is_empty());
  EXPECT_EQ(assigned.arity(), 5U);
  EXPECT_EQ(assigned.top(), 30);
}

//===---------------------------------------------------------------------------===//
