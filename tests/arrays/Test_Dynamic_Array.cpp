//===---------------------------------------------------------------------------===//
/**
 * @file Test_Dynamic_Array.cpp
 * @brief Google Test unit tests for DynamicArray.
 * @version 0.1
 * @date 2026-01-23
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"

#include <gtest/gtest.h>

#include <array>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace ads::arrays;

namespace {

struct ThrowOnMoveAssign {
  inline static int live_count = 0;
  inline static int calls      = 0;
  inline static int throw_at   = -1;

  int value = 0;

  ThrowOnMoveAssign() { ++live_count; }

  explicit ThrowOnMoveAssign(int v) : value(v) { ++live_count; }

  ThrowOnMoveAssign(const ThrowOnMoveAssign& other) : value(other.value) { ++live_count; }

  ThrowOnMoveAssign(ThrowOnMoveAssign&& other) noexcept : value(other.value) { ++live_count; }

  auto operator=(const ThrowOnMoveAssign& other) -> ThrowOnMoveAssign& {
    value = other.value;
    return *this;
  }

  auto operator=(ThrowOnMoveAssign&& other) -> ThrowOnMoveAssign& {
    ++calls;
    if (throw_at > 0 && calls >= throw_at) {
      throw std::runtime_error("move assignment failed");
    }
    value = other.value;
    return *this;
  }

  ~ThrowOnMoveAssign() { --live_count; }

  static auto reset() -> void {
    live_count = 0;
    calls      = 0;
    throw_at   = -1;
  }
};

struct ThrowOnDefaultCtor {
  inline static int live_count = 0;
  inline static int calls      = 0;
  inline static int throw_at   = -1;
  int               value      = 0;

  ThrowOnDefaultCtor() {
    ++calls;
    if (throw_at > 0 && calls == throw_at) {
      throw std::runtime_error("default construction failed");
    }
    ++live_count;
  }

  explicit ThrowOnDefaultCtor(int v) : value(v) { ++live_count; }

  ThrowOnDefaultCtor(const ThrowOnDefaultCtor& other) : value(other.value) { ++live_count; }

  ThrowOnDefaultCtor(ThrowOnDefaultCtor&& other) noexcept : value(other.value) { ++live_count; }

  auto operator=(const ThrowOnDefaultCtor& other) -> ThrowOnDefaultCtor& {
    value = other.value;
    return *this;
  }

  auto operator=(ThrowOnDefaultCtor&& other) noexcept -> ThrowOnDefaultCtor& {
    value = other.value;
    return *this;
  }

  ~ThrowOnDefaultCtor() { --live_count; }

  static auto reset() -> void {
    live_count = 0;
    calls      = 0;
    throw_at   = -1;
  }
};

struct ThrowOnCopyCtor {
  inline static int live_count = 0;
  inline static int calls      = 0;
  inline static int throw_at   = -1;
  int               value      = 0;

  explicit ThrowOnCopyCtor(int v = 0) : value(v) { ++live_count; }

  ThrowOnCopyCtor(const ThrowOnCopyCtor& other) : value(other.value) {
    ++calls;
    if (throw_at > 0 && calls == throw_at) {
      throw std::runtime_error("copy construction failed");
    }
    ++live_count;
  }

  ThrowOnCopyCtor(ThrowOnCopyCtor&& other) noexcept : value(other.value) { ++live_count; }

  auto operator=(const ThrowOnCopyCtor& other) -> ThrowOnCopyCtor& {
    value = other.value;
    return *this;
  }

  auto operator=(ThrowOnCopyCtor&& other) noexcept -> ThrowOnCopyCtor& {
    value = other.value;
    return *this;
  }

  ~ThrowOnCopyCtor() { --live_count; }

  static auto reset() -> void {
    live_count = 0;
    calls      = 0;
    throw_at   = -1;
  }
};

} // namespace

//===----- TEST FIXTURE --------------------------------------------------------===//

class DynamicArrayTest : public ::testing::Test {
protected:
  DynamicArray<int> array;
};

//===----- BASIC STATE TESTS ---------------------------------------------------===//

TEST_F(DynamicArrayTest, IsEmptyOnConstruction) {
  EXPECT_EQ(array.size(), 0);
  EXPECT_TRUE(array.is_empty());
}

TEST_F(DynamicArrayTest, PushBackAndAccess) {
  array.push_back(10);
  array.push_back(20);

  EXPECT_EQ(array.size(), 2);
  EXPECT_EQ(array.front(), 10);
  EXPECT_EQ(array.back(), 20);
  EXPECT_EQ(array[1], 20);
}

TEST_F(DynamicArrayTest, PopBack) {
  array.push_back(5);
  array.push_back(15);

  array.pop_back();
  EXPECT_EQ(array.size(), 1);
  EXPECT_EQ(array.back(), 5);
}

TEST_F(DynamicArrayTest, InsertAndErase) {
  array.push_back(10);
  array.push_back(30);

  array.insert(1, 20);
  EXPECT_EQ(array.size(), 3);
  EXPECT_EQ(array[1], 20);

  array.erase(1);
  EXPECT_EQ(array.size(), 2);
  EXPECT_EQ(array[1], 30);
}

TEST_F(DynamicArrayTest, EmplaceBack) {
  DynamicArray<std::string> strings;
  strings.emplace_back("Hello");
  strings.emplace_back("World");

  EXPECT_EQ(strings.size(), 2);
  EXPECT_EQ(strings.back(), "World");
}

//===----- ERROR HANDLING TESTS ------------------------------------------------===//

TEST_F(DynamicArrayTest, PopBackOnEmptyThrows) {
  EXPECT_THROW(array.pop_back(), ArrayUnderflowException);
}

TEST_F(DynamicArrayTest, AtThrowsOnOutOfRange) {
  array.push_back(1);
  EXPECT_THROW(array.at(1), ArrayOutOfRangeException);
}

//===----- MOVE SEMANTICS TESTS ------------------------------------------------===//

TEST_F(DynamicArrayTest, MoveSemantics) {
  array.push_back(10);
  array.push_back(20);

  DynamicArray<int> moved = std::move(array);
  EXPECT_TRUE(array.is_empty());
  EXPECT_EQ(moved.size(), 2);

  array = std::move(moved);
  EXPECT_TRUE(moved.is_empty());
  EXPECT_EQ(array.size(), 2);
}

TEST_F(DynamicArrayTest, RangeBasedIteration) {
  for (int i = 1; i <= 4; ++i) {
    array.push_back(i);
  }

  std::vector<int> values;
  for (const auto& value : array) {
    values.push_back(value);
  }

  std::vector<int> expected = {1, 2, 3, 4};
  EXPECT_EQ(values, expected);
}

TEST_F(DynamicArrayTest, ConstructAndAssignFromRanges) {
  std::vector<int> values = {1, 2, 3};

  DynamicArray<int> from_range(values.begin(), values.end());
  EXPECT_EQ(from_range.size(), 3u);
  EXPECT_EQ(from_range[0], 1);
  EXPECT_EQ(from_range[2], 3);

  from_range.assign({4, 5});
  EXPECT_EQ(from_range.size(), 2u);
  EXPECT_EQ(from_range.front(), 4);
  EXPECT_EQ(from_range.back(), 5);

  from_range.assign(3, 9);
  EXPECT_EQ(from_range.size(), 3u);
  EXPECT_EQ(from_range[0], 9);
  EXPECT_EQ(from_range[2], 9);
}

TEST_F(DynamicArrayTest, AssignFromMoveIteratorRange) {
  std::vector<std::unique_ptr<int>> values;
  values.push_back(std::make_unique<int>(10));
  values.push_back(std::make_unique<int>(20));

  DynamicArray<std::unique_ptr<int>> moved_values;
  moved_values.assign(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()));

  EXPECT_EQ(moved_values.size(), 2u);
  EXPECT_EQ(*moved_values[0], 10);
  EXPECT_EQ(*moved_values[1], 20);
  EXPECT_EQ(values[0], nullptr);
  EXPECT_EQ(values[1], nullptr);
}

TEST_F(DynamicArrayTest, EmplaceExceptionDoesNotLeakElements) {
  ThrowOnMoveAssign::reset();

  {
    DynamicArray<ThrowOnMoveAssign> arr;
    arr.emplace_back(1);
    arr.emplace_back(2);
    arr.emplace_back(3);

    ThrowOnMoveAssign::calls    = 0;
    ThrowOnMoveAssign::throw_at = 1;

    EXPECT_THROW(arr.emplace(0, 99), std::runtime_error);
    EXPECT_EQ(arr.size(), 3u);
  }

  EXPECT_EQ(ThrowOnMoveAssign::live_count, 0);
}

TEST_F(DynamicArrayTest, ResizeDefaultConstructionExceptionKeepsArrayConsistent) {
  ThrowOnDefaultCtor::reset();

  {
    DynamicArray<ThrowOnDefaultCtor> arr;
    arr.emplace_back(10);

    ThrowOnDefaultCtor::calls    = 0;
    ThrowOnDefaultCtor::throw_at = 2;

    EXPECT_THROW(arr.resize(4), std::runtime_error);
    EXPECT_EQ(arr.size(), 2u);
  }

  EXPECT_EQ(ThrowOnDefaultCtor::live_count, 0);
}

TEST_F(DynamicArrayTest, AssignRangeExceptionKeepsExistingArray) {
  ThrowOnCopyCtor::reset();

  {
    DynamicArray<ThrowOnCopyCtor> arr;
    arr.emplace_back(1);
    arr.emplace_back(2);

    std::array<ThrowOnCopyCtor, 3> replacements = {ThrowOnCopyCtor(10), ThrowOnCopyCtor(20), ThrowOnCopyCtor(30)};

    ThrowOnCopyCtor::calls    = 0;
    ThrowOnCopyCtor::throw_at = 2;

    EXPECT_THROW(arr.assign(replacements.begin(), replacements.end()), std::runtime_error);
    EXPECT_EQ(arr.size(), 2u);
    EXPECT_EQ(arr[0].value, 1);
    EXPECT_EQ(arr[1].value, 2);
  }

  EXPECT_EQ(ThrowOnCopyCtor::live_count, 0);
}

//===---------------------------------------------------------------------------===//
