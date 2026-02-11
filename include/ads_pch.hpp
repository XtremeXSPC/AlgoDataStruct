//===---------------------------------------------------------------------------===//
/**
 * @file ads_pch.hpp
 * @brief Common STL precompiled header for AlgoDataStruct targets.
 *
 * Keep this list small and stable: only frequently used standard headers.
 * Project headers and third-party headers should remain outside this file.
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef ADS_PCH_HPP
#define ADS_PCH_HPP

// Core language and utility headers.
#include <cstddef>
#include <cstdint>
#include <limits>
#include <new>
#include <utility>

// Memory and type system.
#include <concepts>
#include <memory>
#include <type_traits>

// Containers and algorithms.
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

// Error handling and I/O.
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>

#endif // ADS_PCH_HPP
