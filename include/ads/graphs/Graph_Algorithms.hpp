//===---------------------------------------------------------------------------===//
/**
 * @file Graph_Algorithms.hpp
 * @author Costantino Lombardi
 * @brief Reusable graph algorithms built on top of the graph containers.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef GRAPH_ALGORITHMS_HPP
#define GRAPH_ALGORITHMS_HPP

#include "../algorithms/Sorting.hpp"
#include "../arrays/Dynamic_Array.hpp"
#include "../queues/Circular_Array_Queue.hpp"
#include "../queues/Priority_Queue.hpp"
#include "Disjoint_Set_Union.hpp"
#include "Graph_Adjacency_List.hpp"
#include "Graph_Adjacency_Matrix.hpp"

#include <concepts>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace ads::graphs {

/**
 * @brief Exception class for reusable graph algorithms.
 */
class GraphAlgorithmException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

//===------------------------ GRAPH CAPABILITY CONCEPTS ------------------------===//

/**
 * @brief Minimal callable used to probe weighted-neighbor traversal support.
 * @tparam EdgeWeight Edge weight type emitted by the graph.
 */
template <typename EdgeWeight>
struct WeightedNeighborVisitorProbe {
  auto operator()(size_t, const EdgeWeight&) const -> void {}
};

/**
 * @brief Concept for graphs exposing reusable weighted-neighbor traversal.
 * @tparam Graph Graph type to validate.
 */
template <typename Graph> concept WeightedGraph = requires(const Graph& graph, size_t vertex_id) {
  typename Graph::edge_weight_type;
  typename Graph::vertex_id_type;
  { graph.num_vertices() } noexcept -> std::same_as<size_t>;
  { graph.is_directed() } noexcept -> std::same_as<bool>;
  { graph.for_each_weighted_neighbor(vertex_id, WeightedNeighborVisitorProbe<typename Graph::edge_weight_type>{}) } -> std::same_as<void>;
};

//===------------------------------ RESULT TYPES -------------------------------===//

/**
 * @brief Result of a single-source shortest path computation.
 *
 * @tparam EdgeWeight Numeric edge weight type.
 */
template <typename EdgeWeight>
class ShortestPathsResult {
public:
  static constexpr size_t kNoPredecessor = std::numeric_limits<size_t>::max();

  ShortestPathsResult(size_t source, ads::arrays::DynamicArray<EdgeWeight>&& distances, ads::arrays::DynamicArray<size_t>&& predecessors);
  ShortestPathsResult(ShortestPathsResult&& other) noexcept                    = default;
  auto operator=(ShortestPathsResult&& other) noexcept -> ShortestPathsResult& = default;
  ~ShortestPathsResult()                                                       = default;

  ShortestPathsResult(const ShortestPathsResult&)                    = delete;
  auto operator=(const ShortestPathsResult&) -> ShortestPathsResult& = delete;

  /**
   * @brief Returns the source vertex.
   * @return Source vertex ID.
   */
  [[nodiscard]] auto source() const noexcept -> size_t;

  /**
   * @brief Returns the number of tracked vertices.
   * @return Number of vertices in the graph.
   */
  [[nodiscard]] auto vertex_count() const noexcept -> size_t;

  /**
   * @brief Returns the raw distances container.
   * @return Const reference to the distance array.
   */
  [[nodiscard]] auto distances() const noexcept -> const ads::arrays::DynamicArray<EdgeWeight>&;

  /**
   * @brief Returns the predecessor of a vertex.
   * @param vertex_id Target vertex ID.
   * @return Predecessor vertex or kNoPredecessor if unreachable/source.
   * @throws GraphAlgorithmException if vertex_id is invalid.
   */
  [[nodiscard]] auto predecessor_of(size_t vertex_id) const -> size_t;

  /**
   * @brief Returns the computed shortest-path distance to a vertex.
   * @param vertex_id Target vertex ID.
   * @return Distance value or the unreachable sentinel if no path exists.
   * @throws GraphAlgorithmException if vertex_id is invalid.
   */
  [[nodiscard]] auto distance_to(size_t vertex_id) const -> const EdgeWeight&;

  /**
   * @brief Checks whether the source can reach a vertex.
   * @param vertex_id Target vertex ID.
   * @return true if a path exists, false otherwise.
   * @throws GraphAlgorithmException if vertex_id is invalid.
   */
  [[nodiscard]] auto has_path_to(size_t vertex_id) const -> bool;

  /**
   * @brief Reconstructs the path from the source to a vertex.
   * @param vertex_id Target vertex ID.
   * @return Ordered vertex sequence, or std::nullopt if unreachable.
   * @throws GraphAlgorithmException if vertex_id is invalid.
   */
  [[nodiscard]] auto path_to(size_t vertex_id) const -> std::optional<ads::arrays::DynamicArray<size_t>>;

  /**
   * @brief Returns the sentinel used for unreachable distances.
   * @return Infinity for floating-point weights, max() otherwise.
   */
  [[nodiscard]] static auto unreachable_distance() noexcept -> EdgeWeight;

private:
  size_t                                source_;
  ads::arrays::DynamicArray<EdgeWeight> distances_;
  ads::arrays::DynamicArray<size_t>     predecessors_;

  auto validate_vertex(size_t vertex_id) const -> void;
};

/**
 * @brief Result of a minimum spanning forest computation.
 *
 * @tparam EdgeWeight Numeric edge weight type.
 */
template <typename EdgeWeight>
class MinimumSpanningForestResult {
public:
  /**
   * @brief Edge selected for the forest.
   */
  struct Edge {
    size_t     from;
    size_t     to;
    EdgeWeight weight;

    auto operator==(const Edge& other) const -> bool = default;
  };

  MinimumSpanningForestResult(EdgeWeight total_weight, size_t component_count, ads::arrays::DynamicArray<Edge>&& edges);
  MinimumSpanningForestResult(MinimumSpanningForestResult&& other) noexcept                    = default;
  auto operator=(MinimumSpanningForestResult&& other) noexcept -> MinimumSpanningForestResult& = default;
  ~MinimumSpanningForestResult()                                                               = default;

  MinimumSpanningForestResult(const MinimumSpanningForestResult&)                    = delete;
  auto operator=(const MinimumSpanningForestResult&) -> MinimumSpanningForestResult& = delete;

  /**
   * @brief Returns the selected forest edges.
   * @return Const reference to the ordered forest edges.
   */
  [[nodiscard]] auto edges() const noexcept -> const ads::arrays::DynamicArray<Edge>&;

  /**
   * @brief Returns the total weight of the forest.
   * @return Sum of all selected edge weights.
   */
  [[nodiscard]] auto total_weight() const noexcept -> const EdgeWeight&;

  /**
   * @brief Returns the number of connected components spanned by the forest.
   * @return Component count.
   */
  [[nodiscard]] auto component_count() const noexcept -> size_t;

  /**
   * @brief Returns the number of selected edges.
   * @return Forest edge count.
   */
  [[nodiscard]] auto edge_count() const noexcept -> size_t;

  /**
   * @brief Checks whether the forest spans all vertices with a single tree.
   * @param vertex_count Number of vertices in the original graph.
   * @return true for a connected MST, false for a disconnected forest.
   */
  [[nodiscard]] auto spans_all_vertices(size_t vertex_count) const noexcept -> bool;

private:
  EdgeWeight                      total_weight_;
  size_t                          component_count_;
  ads::arrays::DynamicArray<Edge> edges_;
};

/**
 * @brief Result of an all-pairs shortest path computation.
 *
 * @tparam EdgeWeight Numeric edge weight type.
 */
template <typename EdgeWeight>
class AllPairsShortestPathsResult {
public:
  static constexpr size_t kNoNextVertex = std::numeric_limits<size_t>::max();

  AllPairsShortestPathsResult(
      ads::arrays::DynamicArray<ads::arrays::DynamicArray<EdgeWeight>>&& distances,
      ads::arrays::DynamicArray<ads::arrays::DynamicArray<size_t>>&&     next_vertices);
  AllPairsShortestPathsResult(AllPairsShortestPathsResult&& other) noexcept                    = default;
  auto operator=(AllPairsShortestPathsResult&& other) noexcept -> AllPairsShortestPathsResult& = default;
  ~AllPairsShortestPathsResult()                                                               = default;

  AllPairsShortestPathsResult(const AllPairsShortestPathsResult&)                    = delete;
  auto operator=(const AllPairsShortestPathsResult&) -> AllPairsShortestPathsResult& = delete;

  /**
   * @brief Returns the number of vertices covered by the result matrices.
   * @return Vertex count.
   */
  [[nodiscard]] auto vertex_count() const noexcept -> size_t;

  /**
   * @brief Returns the raw distance matrix.
   * @return Const reference to the distance matrix.
   */
  [[nodiscard]] auto distances() const noexcept -> const ads::arrays::DynamicArray<ads::arrays::DynamicArray<EdgeWeight>>&;

  /**
   * @brief Returns the shortest-path distance from source to target.
   * @param source Source vertex ID.
   * @param target Target vertex ID.
   * @return Distance value or the unreachable sentinel if no path exists.
   * @throws GraphAlgorithmException if either vertex is invalid.
   */
  [[nodiscard]] auto distance(size_t source, size_t target) const -> const EdgeWeight&;

  /**
   * @brief Checks whether a path exists from source to target.
   * @param source Source vertex ID.
   * @param target Target vertex ID.
   * @return true if reachable, false otherwise.
   * @throws GraphAlgorithmException if either vertex is invalid.
   */
  [[nodiscard]] auto has_path(size_t source, size_t target) const -> bool;

  /**
   * @brief Reconstructs the shortest path from source to target.
   * @param source Source vertex ID.
   * @param target Target vertex ID.
   * @return Ordered vertex sequence, or std::nullopt if unreachable.
   * @throws GraphAlgorithmException if either vertex is invalid.
   */
  [[nodiscard]] auto path(size_t source, size_t target) const -> std::optional<ads::arrays::DynamicArray<size_t>>;

  /**
   * @brief Returns the sentinel used for unreachable distances.
   * @return Infinity for floating-point weights, max() otherwise.
   */
  [[nodiscard]] static auto unreachable_distance() noexcept -> EdgeWeight;

private:
  ads::arrays::DynamicArray<ads::arrays::DynamicArray<EdgeWeight>> distances_;
  ads::arrays::DynamicArray<ads::arrays::DynamicArray<size_t>>     next_vertices_;

  auto validate_vertex(size_t vertex_id) const -> void;
};

/**
 * @brief Result of a strongly connected components computation.
 */
class StronglyConnectedComponentsResult {
public:
  StronglyConnectedComponentsResult(
      ads::arrays::DynamicArray<ads::arrays::DynamicArray<size_t>>&& components, ads::arrays::DynamicArray<size_t>&& component_ids);
  StronglyConnectedComponentsResult(StronglyConnectedComponentsResult&& other) noexcept                    = default;
  auto operator=(StronglyConnectedComponentsResult&& other) noexcept -> StronglyConnectedComponentsResult& = default;
  ~StronglyConnectedComponentsResult()                                                                     = default;

  StronglyConnectedComponentsResult(const StronglyConnectedComponentsResult&)                    = delete;
  auto operator=(const StronglyConnectedComponentsResult&) -> StronglyConnectedComponentsResult& = delete;

  /**
   * @brief Returns the number of vertices covered by the result.
   * @return Vertex count.
   */
  [[nodiscard]] auto vertex_count() const noexcept -> size_t;

  /**
   * @brief Returns the number of strongly connected components.
   * @return Component count.
   */
  [[nodiscard]] auto component_count() const noexcept -> size_t;

  /**
   * @brief Returns the raw component collection.
   * @return Const reference to the component list.
   */
  [[nodiscard]] auto components() const noexcept -> const ads::arrays::DynamicArray<ads::arrays::DynamicArray<size_t>>&;

  /**
   * @brief Returns the vertex IDs belonging to one component.
   * @param component_id Component index.
   * @return Const reference to the component vertices.
   * @throws GraphAlgorithmException if component_id is invalid.
   */
  [[nodiscard]] auto component(size_t component_id) const -> const ads::arrays::DynamicArray<size_t>&;

  /**
   * @brief Returns the component index assigned to a vertex.
   * @param vertex_id Vertex ID.
   * @return Component index for the vertex.
   * @throws GraphAlgorithmException if vertex_id is invalid.
   */
  [[nodiscard]] auto component_id_of(size_t vertex_id) const -> size_t;

  /**
   * @brief Checks whether two vertices belong to the same SCC.
   * @param lhs First vertex ID.
   * @param rhs Second vertex ID.
   * @return true if both vertices are in the same component.
   * @throws GraphAlgorithmException if either vertex is invalid.
   */
  [[nodiscard]] auto are_strongly_connected(size_t lhs, size_t rhs) const -> bool;

private:
  ads::arrays::DynamicArray<ads::arrays::DynamicArray<size_t>> components_;
  ads::arrays::DynamicArray<size_t>                            component_ids_;

  auto validate_vertex(size_t vertex_id) const -> void;
  auto validate_component(size_t component_id) const -> void;
};

//===--------------------------- ALGORITHM INTERFACE ---------------------------===//

/**
 * @brief Computes single-source shortest paths with Dijkstra's algorithm.
 *
 * @tparam Graph Graph type exposing weighted neighbor traversal.
 * @param graph Weighted graph instance.
 * @param source Source vertex ID.
 * @return Distances and predecessor chain for the source.
 * @throws GraphAlgorithmException if source is invalid or if a negative edge is found.
 * @complexity Time O((V + E) log V) for sparse graphs when neighbor traversal is efficient.
 */
template <WeightedGraph Graph>
auto dijkstra_shortest_paths(const Graph& graph, size_t source) -> ShortestPathsResult<typename Graph::edge_weight_type>;

/**
 * @brief Computes single-source shortest paths with Bellman-Ford.
 *
 * @tparam Graph Graph type exposing weighted neighbor traversal.
 * @param graph Weighted graph instance.
 * @param source Source vertex ID.
 * @return Distances and predecessor chain for the source.
 * @throws GraphAlgorithmException if source is invalid or if a negative cycle is reachable from source.
 * @complexity Time O(VE), Space O(V).
 */
template <WeightedGraph Graph>
auto bellman_ford_shortest_paths(const Graph& graph, size_t source) -> ShortestPathsResult<typename Graph::edge_weight_type>;

/**
 * @brief Computes a minimum spanning forest with Prim's algorithm.
 *
 * @tparam Graph Undirected weighted graph type.
 * @param graph Weighted graph instance.
 * @return Forest edges and total weight. Disconnected graphs produce a forest.
 * @throws GraphAlgorithmException if the graph is directed.
 * @complexity Time O(E log E) with adjacency-list style neighbor traversal.
 */
template <WeightedGraph Graph>
auto prim_minimum_spanning_forest(const Graph& graph) -> MinimumSpanningForestResult<typename Graph::edge_weight_type>;

/**
 * @brief Computes a minimum spanning forest with Kruskal's algorithm.
 *
 * @tparam Graph Undirected weighted graph type.
 * @param graph Weighted graph instance.
 * @return Forest edges and total weight. Disconnected graphs produce a forest.
 * @throws GraphAlgorithmException if the graph is directed.
 * @complexity Time O(E log E), Space O(V + E).
 */
template <WeightedGraph Graph>
auto kruskal_minimum_spanning_forest(const Graph& graph) -> MinimumSpanningForestResult<typename Graph::edge_weight_type>;

/**
 * @brief Computes all-pairs shortest paths with the Floyd-Warshall algorithm.
 *
 * @tparam Graph Weighted graph type.
 * @param graph Weighted graph instance.
 * @return Distance and next-hop matrices for every ordered pair of vertices.
 * @throws GraphAlgorithmException if a negative cycle is detected.
 * @complexity Time O(V^3), Space O(V^2).
 */
template <WeightedGraph Graph>
auto floyd_warshall_all_pairs_shortest_paths(const Graph& graph) -> AllPairsShortestPathsResult<typename Graph::edge_weight_type>;

/**
 * @brief Computes strongly connected components with Kosaraju's algorithm.
 *
 * @tparam Graph Directed graph type.
 * @param graph Directed graph instance.
 * @return Strongly connected components and vertex-to-component mapping.
 * @throws GraphAlgorithmException if the graph is undirected.
 * @complexity Time O(V + E), Space O(V + E).
 */
template <WeightedGraph Graph>
auto strongly_connected_components(const Graph& graph) -> StronglyConnectedComponentsResult;

/**
 * @brief Computes a topological ordering with Kahn's algorithm.
 *
 * @tparam Graph Directed graph type.
 * @param graph Directed acyclic graph instance.
 * @return Topological order covering every vertex exactly once.
 * @throws GraphAlgorithmException if the graph is undirected or contains a cycle.
 * @complexity Time O(V + E), Space O(V).
 */
template <WeightedGraph Graph>
auto topological_sort(const Graph& graph) -> ads::arrays::DynamicArray<size_t>;

} // namespace ads::graphs

#include "../../../src/ads/graphs/Graph_Algorithms.tpp"

#endif // GRAPH_ALGORITHMS_HPP

//===---------------------------------------------------------------------------===//
