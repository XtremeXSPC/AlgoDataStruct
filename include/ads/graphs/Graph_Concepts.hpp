//===---------------------------------------------------------------------------===//
/**
 * @file Graph_Concepts.hpp
 * @author Costantino Lombardi
 * @brief Reusable C++20 concepts for graph containers and algorithms.
 * @version 0.1
 * @date 2026-06-07
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef GRAPH_CONCEPTS_HPP
#define GRAPH_CONCEPTS_HPP

#include "../../support/Support.hpp"

#include <concepts>
#include <cstddef>
#include <limits>
#include <utility>

namespace ads::graphs {

namespace sup = ads::support;

template <typename T> concept VertexPayload = sup::NonReferenceDestructible<T>;

template <typename T> concept EdgeWeightValue = sup::NonReferenceDestructible<T>;

template <typename T> concept CopyVertexPayload = VertexPayload<T> && sup::Copyable<T>;

template <typename T> concept MoveVertexPayload = VertexPayload<T> && sup::Movable<T>;

template <typename T> concept CopyAdjacencyEdge = sup::NonReferenceDestructible<T> && sup::Copyable<T>;

template <typename T> concept PathWeight =
    EdgeWeightValue<T> && std::totally_ordered<T> && sup::DefaultInitializable<T> && requires(const T& lhs, const T& rhs) {
      { lhs + rhs } -> std::convertible_to<T>;
      { std::numeric_limits<T>::max() } -> std::convertible_to<T>;
      { std::numeric_limits<T>::lowest() } -> std::convertible_to<T>;
    };

template <typename T> concept FloatingPathWeight = PathWeight<T> && std::floating_point<T>;

template <typename T> concept SignedIntegralPathWeight = PathWeight<T> && std::signed_integral<T>;

template <typename Visitor, typename Weight> concept WeightedNeighborVisitor =
    EdgeWeightValue<Weight> && requires(Visitor&& visitor, size_t vertex_id, const Weight& weight) {
      { std::forward<Visitor>(visitor)(vertex_id, weight) } -> std::same_as<void>;
    };

template <PathWeight Weight>
struct WeightedNeighborVisitorProbe {
  auto operator()(size_t, const Weight&) const -> void {}
};

template <typename Graph> concept WeightedGraph =
    requires {
      typename Graph::edge_weight_type;
      typename Graph::vertex_id_type;
    } && PathWeight<typename Graph::edge_weight_type> && std::same_as<typename Graph::vertex_id_type, size_t>
    && requires(const Graph& graph, size_t vertex_id) {
         { graph.num_vertices() } noexcept -> std::same_as<size_t>;
         { graph.is_directed() } noexcept -> std::same_as<bool>;
         {
           graph.for_each_weighted_neighbor(vertex_id, WeightedNeighborVisitorProbe<typename Graph::edge_weight_type>{})
         } -> std::same_as<void>;
       };

} // namespace ads::graphs

#endif // GRAPH_CONCEPTS_HPP

//===---------------------------------------------------------------------------===//
