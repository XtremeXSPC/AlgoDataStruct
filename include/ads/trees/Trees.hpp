//===---------------------------------------------------------------------------===//
/**
 * @file Trees.hpp
 * @author Costantino Lombardi
 * @brief Umbrella include for the tree module.
 * @version 0.1
 * @date 2026-06-07
 *
 * @details Prefer this header in demos, exercises, tests, or integration code
 *          that needs multiple tree facilities at once. Concrete container
 *          headers should still include only the specific dependencies they use.
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef TREES_HPP
#define TREES_HPP

// Ordered search trees.
#include "search/AVL_Tree.hpp"
#include "search/B_Tree.hpp"
#include "search/Binary_Search_Tree.hpp"
#include "search/Red_Black_Tree.hpp"
#include "search/Splay_Tree.hpp"
#include "search/Treap.hpp"

// Array-backed range-query structures.
#include "range/Fenwick_Tree.hpp"
#include "range/Fenwick_Tree_Range_Update.hpp"
#include "range/Lazy_Segment_Tree.hpp"
#include "range/Segment_Tree.hpp"

// Digital (prefix) trees.
#include "trie/Trie.hpp"

// Structural trees.
#include "Complete_Binary_Tree.hpp"
#include "Nary_Tree.hpp"

// Shared concepts and exceptions.
#include "Tree_Concepts.hpp"
#include "exceptions/Binary_Tree_Exception.hpp"
#include "exceptions/Fenwick_Tree_Exception.hpp"
#include "exceptions/Segment_Tree_Exception.hpp"

#endif // TREES_HPP

//===---------------------------------------------------------------------------===//
