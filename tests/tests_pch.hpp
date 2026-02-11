//===---------------------------------------------------------------------------===//
/**
 * @file tests_pch.hpp
 * @brief Precompiled header for GoogleTest-based unit tests.
 *
 * Keep this file focused on GoogleTest and the most common STL headers used
 * across test sources.
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef TESTS_PCH_HPP
#define TESTS_PCH_HPP

// GoogleTest.
#include <gtest/gtest.h>

// Common STL headers for tests.
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#endif // TESTS_PCH_HPP
