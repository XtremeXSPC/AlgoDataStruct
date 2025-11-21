//===--------------------------------------------------------------------------===//
/**
 * @file Trie.tpp
 * @author Costantino Lombardi
 * @brief Trie template implementation
 * @version 1.0
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * Note: This file is intentionally minimal. Due to the template nature of the
 * Trie class and the complexity of having both map-based and array-based
 * implementations controlled by a template parameter, all implementations
 * are included directly in the header file (Trie.hpp) using constexpr if.
 *
 * This approach ensures:
 * 1. Better inlining opportunities for the compiler
 * 2. Clearer visibility of conditional compilation paths
 * 3. Easier maintenance of the dual-implementation strategy
 *
 * For implementation details, refer to Trie.hpp.
 */
//===--------------------------------------------------------------------------===//

#pragma once

// All implementation is in Trie.hpp due to template nature and constexpr if usage
// This file exists to maintain consistency with other data structures in the library

namespace ads::trees {

// Template implementations are fully contained in Trie.hpp
// No additional definitions needed here

}  // namespace ads::trees

//===--------------------------------------------------------------------------===//
