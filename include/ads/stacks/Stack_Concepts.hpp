//===---------------------------------------------------------------------------===//
/**
 * @file Stack_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Reusable C++20 concepts for stack containers.
 * @version 0.1
 * @date 2026-06-13
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef STACK_CONCEPTS_HPP
#define STACK_CONCEPTS_HPP

#include "../../support/Support.hpp"

namespace ads::stacks {

namespace sup = ads::support;

template <typename T> concept StackValue = sup::NonReferenceDestructible<T>;

template <typename T> concept CopyStackValue = StackValue<T> && sup::Copyable<T>;

template <typename T> concept MoveStackValue = StackValue<T> && sup::Movable<T>;

template <typename T, typename... Args> concept EmplaceStackValue = StackValue<T> && sup::ConstructibleFrom<T, Args...>;

} // namespace ads::stacks

#endif // STACK_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
