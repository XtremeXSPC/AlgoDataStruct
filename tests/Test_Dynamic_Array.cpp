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

#include "../include/ads/arrays/Dynamic_Array.hpp"

#include <gtest/gtest.h>

#include <array>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace ads::arrays;

namespace {

struct ThrowingMoveAssignmentType {
  inline static int live_count                = 0;
  inline static int move_assignment_calls     = 0;
  inline static int throw_on_move_assign_call = -1;

  int value = 0;

  ThrowingMoveAssignmentType() { ++live_count; }

  explicit ThrowingMoveAssignmentType(int v) : value(v) { ++live_count; }

  ThrowingMoveAssignmentType(const ThrowingMoveAssignmentType& other) : value(other.value) { ++live_count; }

  ThrowingMoveAssignmentType(ThrowingMoveAssignmentType&& other) noexcept : value(other.value) { ++live_count; }

  auto operator=(const ThrowingMoveAssignmentType& other) -> ThrowingMoveAssignmentType& {
    value = other.value;
    return *this;
  }

  auto operator=(ThrowingMoveAssignmentType&& other) -> ThrowingMoveAssignmentType& {
    ++move_assignment_calls;
    if (throw_on_move_assign_call > 0 && move_assignment_calls >= throw_on_move_assign_call) {
      throw std::runtime_error("move assignment failed");
    }
    value = other.value;
    return *this;
  }

  ~ThrowingMoveAssignmentType() { --live_count; }

  static auto reset() -> void {
    live_count                = 0;
    move_assignment_calls     = 0;
    throw_on_move_assign_call = -1;
  }
};

struct ThrowingDefaultConstructionType {
  inline static int live_count                = 0;
  inline static int default_construction_call = 0;
  inline static int throw_on_default_call     = -1;
  int               value                     = 0;

  ThrowingDefaultConstructionType() {
    ++default_construction_call;
    if (throw_on_default_call > 0 && default_construction_call == throw_on_default_call) {
      throw std::runtime_error("default construction failed");
    }
    ++live_count;
  }

  explicit ThrowingDefaultConstructionType(int v) : value(v) { ++live_count; }

  ThrowingDefaultConstructionType(const ThrowingDefaultConstructionType& other) : value(other.value) { ++live_count; }

  ThrowingDefaultConstructionType(ThrowingDefaultConstructionType&& other) noexcept : value(other.value) {
    ++live_count;
  }

  auto operator=(const ThrowingDefaultConstructionType& other) -> ThrowingDefaultConstructionType& {
    value = other.value;
    return *this;
  }

  auto operator=(ThrowingDefaultConstructionType&& other) noexcept -> ThrowingDefaultConstructionType& {
    value = other.value;
    return *this;
  }

  ~ThrowingDefaultConstructionType() { --live_count; }

  static auto reset() -> void {
    live_count                = 0;
    default_construction_call = 0;
    throw_on_default_call     = -1;
  }
};

struct ThrowingCopyConstructionType {
  inline static int live_count         = 0;
  inline static int copy_call          = 0;
  inline static int throw_on_copy_call = -1;
  int               value              = 0;

  explicit ThrowingCopyConstructionType(int v = 0) : value(v) { ++live_count; }

  ThrowingCopyConstructionType(const ThrowingCopyConstructionType& other) : value(other.value) {
    ++copy_call;
    if (throw_on_copy_call > 0 && copy_call == throw_on_copy_call) {
      throw std::runtime_error("copy construction failed");
    }
    ++live_count;
  }

  ThrowingCopyConstructionType(ThrowingCopyConstructionType&& other) noexcept : value(other.value) { ++live_count; }

  auto operator=(const ThrowingCopyConstructionType& other) -> ThrowingCopyConstructionType& {
    value = other.value;
    return *this;
  }

  auto operator=(ThrowingCopyConstructionType&& other) noexcept -> ThrowingCopyConstructionType& {
    value = other.value;
    return *this;
  }

  ~ThrowingCopyConstructionType() { --live_count; }

  static auto reset() -> void {
    live_count         = 0;
    copy_call          = 0;
    throw_on_copy_call = -1;
  }
};

} // namespace

// Test fixture for DynamicArray.
class DynamicArrayTest : public ::testing::Test {
protected:
  DynamicArray<int> array;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

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

//===-------------------------- ERROR HANDLING TESTS ---------------------------===//

TEST_F(DynamicArrayTest, PopBackOnEmptyThrows) {
  EXPECT_THROW(array.pop_back(), ArrayUnderflowException);
}

TEST_F(DynamicArrayTest, AtThrowsOnOutOfRange) {
  array.push_back(1);
  EXPECT_THROW(array.at(1), ArrayOutOfRangeException);
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

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
  ThrowingMoveAssignmentType::reset();

  {
    DynamicArray<ThrowingMoveAssignmentType> throwing_array;
    throwing_array.emplace_back(1);
    throwing_array.emplace_back(2);
    throwing_array.emplace_back(3);

    ThrowingMoveAssignmentType::move_assignment_calls     = 0;
    ThrowingMoveAssignmentType::throw_on_move_assign_call = 1;

    EXPECT_THROW(throwing_array.emplace(0, 99), std::runtime_error);
    EXPECT_EQ(throwing_array.size(), 3u);
  }

  EXPECT_EQ(ThrowingMoveAssignmentType::live_count, 0);
}

TEST_F(DynamicArrayTest, ResizeDefaultConstructionExceptionKeepsArrayConsistent) {
  ThrowingDefaultConstructionType::reset();

  {
    DynamicArray<ThrowingDefaultConstructionType> throwing_array;
    throwing_array.emplace_back(10);

    ThrowingDefaultConstructionType::default_construction_call = 0;
    ThrowingDefaultConstructionType::throw_on_default_call     = 2;

    EXPECT_THROW(throwing_array.resize(4), std::runtime_error);
    EXPECT_EQ(throwing_array.size(), 2u);
  }

  EXPECT_EQ(ThrowingDefaultConstructionType::live_count, 0);
}

TEST_F(DynamicArrayTest, AssignRangeExceptionKeepsExistingArray) {
  ThrowingCopyConstructionType::reset();

  {
    DynamicArray<ThrowingCopyConstructionType> throwing_array;
    throwing_array.emplace_back(1);
    throwing_array.emplace_back(2);

    std::array<ThrowingCopyConstructionType, 3> replacements = {
        ThrowingCopyConstructionType(10), ThrowingCopyConstructionType(20), ThrowingCopyConstructionType(30)};

    ThrowingCopyConstructionType::copy_call          = 0;
    ThrowingCopyConstructionType::throw_on_copy_call = 2;

    EXPECT_THROW(throwing_array.assign(replacements.begin(), replacements.end()), std::runtime_error);
    EXPECT_EQ(throwing_array.size(), 2u);
    EXPECT_EQ(throwing_array[0].value, 1);
    EXPECT_EQ(throwing_array[1].value, 2);
  }

  EXPECT_EQ(ThrowingCopyConstructionType::live_count, 0);
}

//===---------------------------------------------------------------------------===//
