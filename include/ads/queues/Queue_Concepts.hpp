//===---------------------------------------------------------------------------===//
/**
 * @file Queue_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Reusable C++20 concepts for queue containers.
 * @version 0.1
 * @date 2026-06-09
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef QUEUE_CONCEPTS_HPP
#define QUEUE_CONCEPTS_HPP

#include "../../support/Support.hpp"

namespace ads::queues {

namespace sup = ads::support;

template <typename T> concept QueueValue = sup::NonReferenceDestructible<T>;

template <typename T> concept CopyQueueValue = QueueValue<T> && sup::Copyable<T>;

template <typename T> concept MoveQueueValue = QueueValue<T> && sup::Movable<T>;

template <typename T, typename... Args> concept EmplaceQueueValue = QueueValue<T> && sup::ConstructibleFrom<T, Args...>;

template <typename InputIt, typename T> concept QueueRangeValue = QueueValue<T> && sup::InputConstructibleFor<InputIt, T>;

} // namespace ads::queues

#endif // QUEUE_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
