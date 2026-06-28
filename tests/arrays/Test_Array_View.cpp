//===---------------------------------------------------------------------------===//
/**
 * @file Test_Array_View.cpp
 * @brief Google Test unit tests for ArrayView.
 * @version 0.1
 * @date 2026-06-08
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Array_View.hpp"
#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/arrays/Static_Array.hpp"
#include "ads/arrays/Static_Vector.hpp"

#include <gtest/gtest.h>

#include <type_traits>
#include <vector>

using namespace ads::arrays;

// Test fixture for ArrayView.
class ArrayViewTest : public ::testing::Test {
protected:
  int raw[5] = {1, 2, 3, 4, 5};

  ArrayView<int> view{raw, 5};
};

//===----- BASIC STATE TESTS ---------------------------------------------------===//

TEST_F(ArrayViewTest, SizeAndEmptiness) {
  EXPECT_EQ(view.size(), 5u);
  EXPECT_FALSE(view.is_empty());

  ArrayView<int> empty;
  EXPECT_TRUE(empty.is_empty());
  EXPECT_EQ(empty.size(), 0u);
}

TEST_F(ArrayViewTest, PointerLengthConstruction) {
  ArrayView<int> v(raw, 3);
  EXPECT_EQ(v.size(), 3u);
  EXPECT_EQ(v.back(), 3);
}

TEST_F(ArrayViewTest, PointerRangeConstruction) {
  ArrayView<int> v(raw, raw + 4);
  EXPECT_EQ(v.size(), 4u);
  EXPECT_EQ(v.back(), 4);
}

//===----- CONTAINER VIEWS -----------------------------------------------------===//

TEST_F(ArrayViewTest, ViewOverDynamicArray) {
  DynamicArray<int> da{10, 20, 30};
  ArrayView<int>    v(da);
  EXPECT_EQ(v.size(), 3u);
  EXPECT_EQ(v.front(), 10);
  EXPECT_EQ(v.back(), 30);
}

TEST_F(ArrayViewTest, ViewOverStaticArray) {
  StaticArray<int, 3> sa{7, 8, 9};
  ArrayView<int>      v(sa);
  EXPECT_EQ(v[1], 8);
}

TEST_F(ArrayViewTest, ViewOverStaticVector) {
  StaticVector<int, 8> sv;
  sv.push_back(1);
  sv.push_back(2);
  ArrayView<int> v(sv);
  EXPECT_EQ(v.size(), 2u);
}

TEST_F(ArrayViewTest, ReadOnlyViewOverConstContainer) {
  DynamicArray<int>        da{10, 20, 30};
  const DynamicArray<int>& cda = da;
  ArrayView<const int>     rov(cda);
  EXPECT_EQ(rov.front(), 10);
  EXPECT_EQ(rov.size(), 3u);
}

//===----- ACCESS TESTS --------------------------------------------------------===//

TEST_F(ArrayViewTest, AtWithBoundsChecking) {
  EXPECT_EQ(view.at(0), 1);
  EXPECT_THROW(view.at(5), ArrayOutOfRangeException);
}

TEST_F(ArrayViewTest, FrontBackOnEmptyThrows) {
  ArrayView<int> empty;
  EXPECT_THROW(empty.front(), ArrayUnderflowException);
  EXPECT_THROW(empty.back(), ArrayUnderflowException);
}

TEST_F(ArrayViewTest, MutationWritesThrough) {
  view[0] = 100;
  EXPECT_EQ(raw[0], 100);
  view.front() = 200;
  EXPECT_EQ(raw[0], 200);
}

//===----- SUBVIEW TESTS -------------------------------------------------------===//

TEST_F(ArrayViewTest, FirstAndLast) {
  EXPECT_EQ(view.first(2).size(), 2u);
  EXPECT_EQ(view.first(2).back(), 2);
  EXPECT_EQ(view.last(2).front(), 4);
}

TEST_F(ArrayViewTest, Subview) {
  auto mid = view.subview(1, 3);
  EXPECT_EQ(mid.size(), 3u);
  EXPECT_EQ(mid.front(), 2);
  EXPECT_EQ(mid.back(), 4);
}

TEST_F(ArrayViewTest, SubviewOutOfRangeThrows) {
  EXPECT_THROW(view.subview(3, 5), ArrayOutOfRangeException);
  EXPECT_THROW(view.first(6), ArrayOutOfRangeException);
  EXPECT_THROW(view.last(6), ArrayOutOfRangeException);
}

//===----- ITERATION TESTS -----------------------------------------------------===//

TEST_F(ArrayViewTest, RangeBasedIteration) {
  std::vector<int> values;
  for (int x : view) {
    values.push_back(x);
  }
  EXPECT_EQ(values, (std::vector<int>{1, 2, 3, 4, 5}));
}

TEST_F(ArrayViewTest, ReverseIteration) {
  std::vector<int> values(view.rbegin(), view.rend());
  EXPECT_EQ(values, (std::vector<int>{5, 4, 3, 2, 1}));
}

//===----- COMPARISON TESTS ----------------------------------------------------===//

TEST_F(ArrayViewTest, EqualityAndThreeWay) {
  int other[3] = {2, 3, 4};

  ArrayView<int> a(raw + 1, 3); // {2,3,4}
  ArrayView<int> b(other, 3);
  EXPECT_TRUE(a == b);

  int bigger[3] = {2, 3, 5};

  ArrayView<int> c(bigger, 3);
  EXPECT_TRUE(a != c);
  EXPECT_TRUE((a <=> c) < 0);
}

//===----- TRAIT TESTS ---------------------------------------------------------===//

TEST_F(ArrayViewTest, IsCopyableValueType) {
  static_assert(std::is_copy_constructible_v<ArrayView<int>>);
  static_assert(std::is_copy_assignable_v<ArrayView<int>>);
  static_assert(std::is_trivially_copyable_v<ArrayView<int>>);
  ArrayView<int> copy = view;
  EXPECT_EQ(copy.size(), view.size());
  EXPECT_EQ(copy.data(), view.data());
}

//===---------------------------------------------------------------------------===//
