//===---------------------------------------------------------------------------===//
/**
 * @file Test_Gap_Buffer.cpp
 * @brief Google Test unit tests for GapBuffer.
 * @version 0.1
 * @date 2026-06-08
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/arrays/Gap_Buffer.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ads::arrays;

namespace {

// Reconstructs the logical contents of a char buffer as a string.
std::string contents(const GapBuffer<char>& g) {
  std::string s;
  for (size_t i = 0; i < g.size(); ++i) {
    s += g[i];
  }
  return s;
}

} // namespace

// Test fixture for GapBuffer.
class GapBufferTest : public ::testing::Test {
protected:
  GapBuffer<int> buf{1, 2, 3, 4, 5};
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(GapBufferTest, ConstructionFromInitializerList) {
  EXPECT_EQ(buf.size(), 5u);
  EXPECT_FALSE(buf.is_empty());
  EXPECT_EQ(buf.cursor(), 5u); // cursor starts at the end
  EXPECT_EQ(buf.front(), 1);
  EXPECT_EQ(buf.back(), 5);
}

TEST_F(GapBufferTest, DefaultConstructionIsEmpty) {
  GapBuffer<int> g;
  EXPECT_TRUE(g.is_empty());
  EXPECT_EQ(g.size(), 0u);
  EXPECT_EQ(g.cursor(), 0u);
}

//===----------------------------- INSERTION TESTS -----------------------------===//

TEST_F(GapBufferTest, InsertAtCursorAdvancesCursor) {
  GapBuffer<int> g;
  g.insert(10);
  g.insert(20);
  EXPECT_EQ(g.size(), 2u);
  EXPECT_EQ(g.cursor(), 2u);
  EXPECT_EQ(g[0], 10);
  EXPECT_EQ(g[1], 20);
}

TEST_F(GapBufferTest, InsertInMiddle) {
  buf.move_cursor_to(2);
  buf.insert(99);
  EXPECT_EQ(buf.size(), 6u);
  EXPECT_EQ(buf[1], 2);
  EXPECT_EQ(buf[2], 99);
  EXPECT_EQ(buf[3], 3);
}

TEST_F(GapBufferTest, EmplaceReturnsReference) {
  GapBuffer<std::string> g;
  auto&                  ref = g.emplace(4, 'z');
  EXPECT_EQ(ref, "zzzz");
  EXPECT_EQ(g[0], "zzzz");
}

TEST_F(GapBufferTest, GrowthAcrossManyInsertions) {
  GapBuffer<int> g(2);
  for (int i = 0; i < 100; ++i) {
    g.insert(i);
  }
  g.move_cursor_to(50);
  for (int i = 0; i < 50; ++i) {
    g.insert(-1);
  }
  EXPECT_EQ(g.size(), 150u);
  EXPECT_EQ(g[49], 49);
  EXPECT_EQ(g[50], -1);
  EXPECT_EQ(g[99], -1);
  EXPECT_EQ(g[100], 50);
}

//===------------------------------ REMOVAL TESTS ------------------------------===//

TEST_F(GapBufferTest, EraseIsBackspace) {
  buf.move_cursor_to(3); // cursor between 3 and 4
  buf.erase();           // removes 3
  EXPECT_EQ(buf.size(), 4u);
  EXPECT_EQ(buf[0], 1);
  EXPECT_EQ(buf[1], 2);
  EXPECT_EQ(buf[2], 4);
}

TEST_F(GapBufferTest, EraseForwardIsDelete) {
  buf.move_cursor_to(2); // cursor between 2 and 3
  buf.erase_forward();   // removes 3
  EXPECT_EQ(buf.size(), 4u);
  EXPECT_EQ(buf[2], 4);
}

TEST_F(GapBufferTest, EraseAtBeginningThrows) {
  buf.move_cursor_to(0);
  EXPECT_THROW(buf.erase(), ArrayUnderflowException);
}

TEST_F(GapBufferTest, EraseForwardAtEndThrows) {
  buf.move_cursor_to(buf.size());
  EXPECT_THROW(buf.erase_forward(), ArrayUnderflowException);
}

TEST_F(GapBufferTest, Clear) {
  buf.clear();
  EXPECT_TRUE(buf.is_empty());
  EXPECT_EQ(buf.cursor(), 0u);
}

//===------------------------------ CURSOR TESTS -------------------------------===//

TEST_F(GapBufferTest, MoveCursorToAndBounds) {
  buf.move_cursor_to(0);
  EXPECT_EQ(buf.cursor(), 0u);
  buf.move_cursor_to(5);
  EXPECT_EQ(buf.cursor(), 5u);
  EXPECT_THROW(buf.move_cursor_to(6), ArrayOutOfRangeException);
}

TEST_F(GapBufferTest, AdvanceAndRetreatCursor) {
  buf.move_cursor_to(2);
  buf.advance_cursor();
  EXPECT_EQ(buf.cursor(), 3u);
  buf.retreat_cursor();
  buf.retreat_cursor();
  EXPECT_EQ(buf.cursor(), 1u);
}

TEST_F(GapBufferTest, AdvanceAtEndAndRetreatAtBeginningThrow) {
  buf.move_cursor_to(buf.size());
  EXPECT_THROW(buf.advance_cursor(), ArrayUnderflowException);
  buf.move_cursor_to(0);
  EXPECT_THROW(buf.retreat_cursor(), ArrayUnderflowException);
}

TEST_F(GapBufferTest, ContentsStableUnderCursorMovement) {
  GapBuffer<char> g{'a', 'b', 'c', 'd'};
  for (size_t pos = 0; pos <= g.size(); ++pos) {
    g.move_cursor_to(pos);
    EXPECT_EQ(contents(g), "abcd");
  }
}

//===------------------------------ ACCESS TESTS -------------------------------===//

TEST_F(GapBufferTest, AtWithBoundsChecking) {
  EXPECT_EQ(buf.at(0), 1);
  EXPECT_THROW(buf.at(5), ArrayOutOfRangeException);
}

TEST_F(GapBufferTest, FrontBackOnEmptyThrow) {
  GapBuffer<int> g;
  EXPECT_THROW(g.front(), ArrayUnderflowException);
  EXPECT_THROW(g.back(), ArrayUnderflowException);
}

//===----------------------------- ITERATION TESTS -----------------------------===//

TEST_F(GapBufferTest, IterationWithGapInMiddle) {
  buf.move_cursor_to(2); // place the gap in the middle
  std::vector<int> fwd(buf.begin(), buf.end());
  EXPECT_EQ(fwd, (std::vector<int>{1, 2, 3, 4, 5}));

  std::vector<int> rev(buf.rbegin(), buf.rend());
  EXPECT_EQ(rev, (std::vector<int>{5, 4, 3, 2, 1}));
}

TEST_F(GapBufferTest, ConstIteration) {
  const GapBuffer<int>& cref = buf;
  std::vector<int>      values(cref.cbegin(), cref.cend());
  EXPECT_EQ(values, (std::vector<int>{1, 2, 3, 4, 5}));
}

//===---------------------------- COMPARISON TESTS -----------------------------===//

TEST_F(GapBufferTest, EqualityAndThreeWay) {
  GapBuffer<int> same{1, 2, 3, 4, 5};
  GapBuffer<int> greater{1, 2, 3, 4, 6};
  EXPECT_TRUE(buf == same);
  EXPECT_TRUE(buf != greater);
  EXPECT_TRUE((buf <=> greater) < 0);
}

TEST_F(GapBufferTest, EqualityIndependentOfCursor) {
  GapBuffer<int> a{1, 2, 3};
  GapBuffer<int> b{1, 2, 3};
  a.move_cursor_to(1);
  b.move_cursor_to(2);
  EXPECT_TRUE(a == b);
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(GapBufferTest, MoveConstruction) {
  GapBuffer<std::string> s;
  s.insert(std::string(20, 'z'));
  s.insert("hi");
  GapBuffer<std::string> moved(std::move(s));
  EXPECT_EQ(moved.size(), 2u);
  EXPECT_EQ(moved[0].size(), 20u);
  EXPECT_EQ(s.size(), 0u);
}

TEST_F(GapBufferTest, IsMoveOnly) {
  static_assert(!std::is_copy_constructible_v<GapBuffer<int>>);
  static_assert(std::is_move_constructible_v<GapBuffer<int>>);
  SUCCEED();
}

TEST_F(GapBufferTest, SatisfiesRandomAccessSequence) {
  static_assert(RandomAccessSequence<GapBuffer<int>>);
  SUCCEED();
}

//===-------------------------- TEXT EDITOR SCENARIO ---------------------------===//

TEST_F(GapBufferTest, TextEditorWorkflow) {
  GapBuffer<char> doc;
  for (char c : std::string("helo")) {
    doc.insert(c);
  }
  doc.move_cursor_to(3); // between 'l' and 'o'
  doc.insert('l');       // "hello"
  EXPECT_EQ(contents(doc), "hello");

  doc.move_cursor_to(doc.size());
  for (char c : std::string(" world")) {
    doc.insert(c);
  }
  EXPECT_EQ(contents(doc), "hello world");
}

//===---------------------------------------------------------------------------===//
