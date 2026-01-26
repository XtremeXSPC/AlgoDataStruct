//===---------------------------------------------------------------------------===//
/**
 * @file Test_Stacks.cpp
 * @brief Google Test unit tests for Stack implementations (ArrayStack, LinkedStack).
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===---------------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/ads/stacks/Array_Stack.hpp"
#include "../include/ads/stacks/Linked_Stack.hpp"

using namespace ads::stacks;

// Test fixture for ArrayStack.
class ArrayStackTest : public ::testing::Test {
protected:
  ArrayStack<int> stack;
};

//===---------------------------- ARRAY STACK TESTS ----------------------------===//

TEST_F(ArrayStackTest, IsEmptyOnConstruction) {
  EXPECT_EQ(stack.size(), 0);
  EXPECT_TRUE(stack.is_empty());
}

TEST_F(ArrayStackTest, PushAndTop) {
  stack.push(10);
  EXPECT_EQ(stack.size(), 1);
  EXPECT_EQ(stack.top(), 10);

  stack.push(20);
  EXPECT_EQ(stack.size(), 2);
  EXPECT_EQ(stack.top(), 20);
}

TEST_F(ArrayStackTest, Pop) {
  stack.push(10);
  stack.push(20);
  stack.push(30);

  stack.pop();
  EXPECT_EQ(stack.top(), 20);

  stack.pop();
  EXPECT_EQ(stack.top(), 10);

  stack.pop();
  EXPECT_TRUE(stack.is_empty());
}

TEST_F(ArrayStackTest, PopOnEmptyThrows) {
  EXPECT_THROW(stack.pop(), StackUnderflowException);
}

TEST_F(ArrayStackTest, TopOnEmptyThrows) {
  EXPECT_THROW(stack.top(), StackUnderflowException);
}

TEST_F(ArrayStackTest, Clear) {
  stack.push(10);
  stack.push(20);

  stack.clear();
  EXPECT_TRUE(stack.is_empty());
  EXPECT_EQ(stack.size(), 0);
}

TEST_F(ArrayStackTest, Emplace) {
  ArrayStack<std::string> str_stack;
  str_stack.emplace("Hello");
  str_stack.emplace("World");

  EXPECT_EQ(str_stack.top(), "World");
  str_stack.pop();
  EXPECT_EQ(str_stack.top(), "Hello");
}

TEST_F(ArrayStackTest, MoveSemantics) {
  stack.push(10);
  stack.push(20);

  // Move constructor.
  ArrayStack<int> moved_stack = std::move(stack);
  EXPECT_TRUE(stack.is_empty());
  EXPECT_EQ(moved_stack.size(), 2);
  EXPECT_EQ(moved_stack.top(), 20);

  // Move assignment.
  stack = std::move(moved_stack);
  EXPECT_TRUE(moved_stack.is_empty());
  EXPECT_EQ(stack.size(), 2);
}

TEST_F(ArrayStackTest, LIFOOrder) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  for (int val : input) {
    stack.push(val);
  }

  std::vector<int> output;
  while (!stack.is_empty()) {
    output.push_back(stack.top());
    stack.pop();
  }

  std::vector<int> expected = {5, 4, 3, 2, 1};
  EXPECT_EQ(output, expected);
}

TEST_F(ArrayStackTest, LargeStackOperations) {
  const int N = 10000;
  for (int i = 0; i < N; ++i) {
    stack.push(i);
  }

  EXPECT_EQ(stack.size(), N);
  EXPECT_EQ(stack.top(), N - 1);

  for (int i = 0; i < N; ++i) {
    stack.pop();
  }

  EXPECT_TRUE(stack.is_empty());
}

//===--------------------------- LINKED STACK TESTS ----------------------------===//

class LinkedStackTest : public ::testing::Test {
protected:
  LinkedStack<int> stack;
};

TEST_F(LinkedStackTest, IsEmptyOnConstruction) {
  EXPECT_EQ(stack.size(), 0);
  EXPECT_TRUE(stack.is_empty());
}

TEST_F(LinkedStackTest, PushAndTop) {
  stack.push(10);
  EXPECT_EQ(stack.size(), 1);
  EXPECT_EQ(stack.top(), 10);

  stack.push(20);
  EXPECT_EQ(stack.size(), 2);
  EXPECT_EQ(stack.top(), 20);
}

TEST_F(LinkedStackTest, Pop) {
  stack.push(10);
  stack.push(20);
  stack.push(30);

  stack.pop();
  EXPECT_EQ(stack.top(), 20);

  stack.pop();
  EXPECT_EQ(stack.top(), 10);

  stack.pop();
  EXPECT_TRUE(stack.is_empty());
}

TEST_F(LinkedStackTest, PopOnEmptyThrows) {
  EXPECT_THROW(stack.pop(), StackUnderflowException);
}

TEST_F(LinkedStackTest, TopOnEmptyThrows) {
  EXPECT_THROW(stack.top(), StackUnderflowException);
}

TEST_F(LinkedStackTest, Clear) {
  stack.push(10);
  stack.push(20);

  stack.clear();
  EXPECT_TRUE(stack.is_empty());
  EXPECT_EQ(stack.size(), 0);
}

TEST_F(LinkedStackTest, Emplace) {
  LinkedStack<std::string> str_stack;
  str_stack.emplace("Hello");
  str_stack.emplace("World");

  EXPECT_EQ(str_stack.top(), "World");
  str_stack.pop();
  EXPECT_EQ(str_stack.top(), "Hello");
}

TEST_F(LinkedStackTest, MoveSemantics) {
  stack.push(10);
  stack.push(20);

  // Move constructor.
  LinkedStack<int> moved_stack = std::move(stack);
  EXPECT_TRUE(stack.is_empty());
  EXPECT_EQ(moved_stack.size(), 2);
  EXPECT_EQ(moved_stack.top(), 20);

  // Move assignment.
  stack = std::move(moved_stack);
  EXPECT_TRUE(moved_stack.is_empty());
  EXPECT_EQ(stack.size(), 2);
}

TEST_F(LinkedStackTest, LIFOOrder) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  for (int val : input) {
    stack.push(val);
  }

  std::vector<int> output;
  while (!stack.is_empty()) {
    output.push_back(stack.top());
    stack.pop();
  }

  std::vector<int> expected = {5, 4, 3, 2, 1};
  EXPECT_EQ(output, expected);
}

TEST_F(LinkedStackTest, LargeStackOperations) {
  const int N = 10000;
  for (int i = 0; i < N; ++i) {
    stack.push(i);
  }

  EXPECT_EQ(stack.size(), N);
  EXPECT_EQ(stack.top(), N - 1);

  for (int i = 0; i < N; ++i) {
    stack.pop();
  }

  EXPECT_TRUE(stack.is_empty());
}

//===------------------------ STACK POLYMORPHISM TESTS -------------------------===//

TEST(StackPolymorphismTest, ArrayStackThroughInterface) {
  ArrayStack<int> concrete_stack;
  Stack<int>*     stack = &concrete_stack;

  stack->push(10);
  stack->push(20);

  EXPECT_EQ(stack->top(), 20);
  EXPECT_EQ(stack->size(), 2);
}

TEST(StackPolymorphismTest, LinkedStackThroughInterface) {
  LinkedStack<int> concrete_stack;
  Stack<int>*      stack = &concrete_stack;

  stack->push(10);
  stack->push(20);

  EXPECT_EQ(stack->top(), 20);
  EXPECT_EQ(stack->size(), 2);
}

//===---------------------------------------------------------------------------===//
