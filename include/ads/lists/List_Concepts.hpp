//===---------------------------------------------------------------------------===//
/**
 * @file List_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Defines reusable C++20 concepts for linked-list containers.
 * @version 0.1
 * @date 2026-06-07
 *
 * @details These concepts form the semantic list layer built on top of the
 *          generic `ads::support` vocabulary. They describe what it means for
 *          a type to behave well as a list payload without leaking raw STL
 *          constraint spellings throughout the public headers.
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef LIST_CONCEPTS_HPP
#define LIST_CONCEPTS_HPP

#include "../../support/Support.hpp"

namespace ads::lists {

namespace sup = ads::support;

template <typename T> concept ListElement = sup::NonReferenceDestructible<T>;

template <typename T, typename... Args> concept EmplaceListElement = ListElement<T> && sup::ConstructibleFrom<T, Args...>;

template <typename T> concept CopyListElement = ListElement<T> && sup::Copyable<T>;

template <typename T> concept MoveListElement = ListElement<T> && sup::Movable<T>;

template <typename T> concept EqualityComparableListElement = ListElement<T> && sup::EqualityComparable<T>;

} // namespace ads::lists

#endif // LIST_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
