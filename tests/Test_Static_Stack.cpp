//===---------------------------------------------------------------------------===//
/**
 * @file Test_Static_Stack.cpp
 * @brief Google Test unit tests for StaticStack.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/stacks/Static_Stack.hpp"

#include <gtest/gtest.h>

#include <string>
#include <utility>

using namespace ads::stacks;

// Test fixture for StaticStack.
class StaticStackTest : public ::testing::Test {
protected:
  StaticStack<int, 5> stack;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(StaticStackTest, EmptyOnConstruction) {
  EXPECT_TRUE(stack.is_empty());
  EXPECT_FALSE(stack.is_full());
  EXPECT_EQ(stack.size(), 0u);
  EXPECT_EQ(stack.capacity(), 5u);
  EXPECT_EQ(decltype(stack)::max_size(), 5u);
}

TEST_F(StaticStackTest, PushUntilFull) {
  for (int i = 0; i < 5; ++i) {
    stack.push(i);
  }
  EXPECT_TRUE(stack.is_full());
  EXPECT_EQ(stack.size(), 5u);
  EXPECT_EQ(stack.top(), 4);
}

TEST_F(StaticStackTest, PushWhenFullThrowsAndLeavesIntact) {
  for (int i = 0; i < 5; ++i) {
    stack.push(i);
  }
  EXPECT_THROW(stack.push(99), StackOverflowException);
  EXPECT_EQ(stack.size(), 5u);
  EXPECT_EQ(stack.top(), 4);
}

TEST_F(StaticStackTest, EmplaceReturnsReference) {
  StaticStack<std::string, 4> s;
  auto&                       ref = s.emplace(5, 'a');
  EXPECT_EQ(ref, "aaaaa");
  EXPECT_EQ(s.top(), "aaaaa");
}

//===------------------------------ REMOVAL TESTS ------------------------------===//

TEST_F(StaticStackTest, PopReturnsToLifoOrder) {
  stack.push(1);
  stack.push(2);
  stack.push(3);
  EXPECT_EQ(stack.top(), 3);
  stack.pop();
  EXPECT_EQ(stack.top(), 2);
  stack.pop();
  EXPECT_EQ(stack.top(), 1);
}

TEST_F(StaticStackTest, PopOnEmptyThrows) {
  EXPECT_THROW(stack.pop(), StackUnderflowException);
}

TEST_F(StaticStackTest, TopOnEmptyThrows) {
  EXPECT_THROW(stack.top(), StackUnderflowException);
}

TEST_F(StaticStackTest, Clear) {
  stack.push(1);
  stack.push(2);
  stack.clear();
  EXPECT_TRUE(stack.is_empty());
  EXPECT_EQ(stack.size(), 0u);
}

//===--------------------------- POLYMORPHISM TESTS ----------------------------===//

TEST_F(StaticStackTest, UsableThroughStackInterface) {
  stack.push(10);
  stack.push(20);
  Stack<int>& base = stack;
  EXPECT_EQ(base.size(), 2u);
  EXPECT_EQ(base.top(), 20);
  base.pop();
  EXPECT_EQ(base.top(), 10);
  EXPECT_FALSE(base.is_empty());
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(StaticStackTest, MoveConstructionLeavesSourceEmpty) {
  StaticStack<std::string, 4> s;
  s.emplace(10, 'x');
  s.emplace(5, 'y');
  StaticStack<std::string, 4> moved(std::move(s));
  EXPECT_EQ(moved.size(), 2u);
  EXPECT_EQ(moved.top(), std::string(5, 'y'));
  EXPECT_TRUE(s.is_empty());
}

TEST_F(StaticStackTest, MoveAssignmentLeavesSourceEmpty) {
  StaticStack<std::string, 4> s;
  s.emplace("hello");
  StaticStack<std::string, 4> dst;
  dst.emplace("discarded");
  dst = std::move(s);
  EXPECT_EQ(dst.size(), 1u);
  EXPECT_EQ(dst.top(), "hello");
  EXPECT_TRUE(s.is_empty());
}

TEST_F(StaticStackTest, IsMoveOnly) {
  static_assert(!std::is_copy_constructible_v<StaticStack<int, 4>>);
  static_assert(!std::is_copy_assignable_v<StaticStack<int, 4>>);
  static_assert(std::is_move_constructible_v<StaticStack<int, 4>>);
  SUCCEED();
}

//===---------------------------------------------------------------------------===//
