//===---------------------------------------------------------------------------===//
/**
 * @file Test_Array_Concepts.cpp
 * @brief Compile-time checks for array concepts and constrained APIs.
 * @version 0.1
 * @date 2026-02-06
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/arrays/Circular_Array.hpp"
#include "../include/ads/arrays/Dynamic_Array.hpp"
#include "../include/ads/arrays/Static_Array.hpp"

#include <gtest/gtest.h>

using namespace ads::arrays;

namespace {

struct NonDestructibleElement {
  ~NonDestructibleElement() = delete;
};

struct NonShiftAssignableElement {
  NonShiftAssignableElement()                                                    = default;
  NonShiftAssignableElement(const NonShiftAssignableElement&)                    = default;
  NonShiftAssignableElement(NonShiftAssignableElement&&)                         = default;
  auto operator=(const NonShiftAssignableElement&) -> NonShiftAssignableElement& = delete;
  auto operator=(NonShiftAssignableElement&&) -> NonShiftAssignableElement&      = delete;
};

static_assert(!ArrayElement<NonDestructibleElement>);
static_assert(!ShiftAssignableArrayElement<NonShiftAssignableElement>);
static_assert(RelocatableArrayElement<NonShiftAssignableElement>);
static_assert(ArrayElement<int>);
static_assert(RelocatableArrayElement<int>);

static_assert(requires(DynamicArray<int>& values) { values.emplace_back(1); });
static_assert(requires(CircularArray<int>& values) { values.emplace_front(1); });
static_assert(requires(StaticArray<int, 4>& values) { values.fill(7); });

} // namespace

TEST(ArrayConceptsTest, ConstraintsCompile) {
  SUCCEED();
}

//===---------------------------------------------------------------------------===//
