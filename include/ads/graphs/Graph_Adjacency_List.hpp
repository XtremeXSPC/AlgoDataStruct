//===---------------------------------------------------------------------------===//
/**
 * @file Graph_Adjacency_List.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the GraphAdjacencyList class
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef GRAPH_ADJACENCY_LIST_HPP
#define GRAPH_ADJACENCY_LIST_HPP

#include "../arrays/Dynamic_Array.hpp"
#include "../queues/Circular_Array_Queue.hpp"
#include "../stacks/Array_Stack.hpp"
#include "Graph_Concepts.hpp"
#include "Graph_Exception.hpp"

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace ads::graphs {

using ads::arrays::DynamicArray;

///@brief Exception class for graph operations.
class GraphException : public GraphError {
public:
  using GraphError::GraphError;
};

/**
 * @brief A graph implemented using adjacency list representation.
 *
 * @details This class implements a graph using adjacency lists, where each
 *          vertex maintains a list of its neighbors. This representation is
 *          memory-efficient for sparse graphs (few edges relative to vertices).
 *
 *          Space complexity: O(V + E)
 *          Edge lookup: O(degree(v))
 *          Neighbor iteration: O(degree(v))
 *
 *          Supports:
 *          - Directed and undirected graphs.
 *          - Weighted and unweighted edges.
 *          - Custom vertex data types.
 *          - Graph traversal algorithms (BFS, DFS).
 *          - Path finding.
 *          - Connected components (for undirected graphs).
 *
 * @tparam VertexData Type of data stored in vertices.
 * @tparam EdgeWeight Type of edge weights (default: double).
 */
template <VertexPayload VertexDataT = int, EdgeWeightValue EdgeWeightT = double>
class GraphAdjacencyList {
public:
  using VertexData = VertexDataT;
  using EdgeWeight = EdgeWeightT;
  using VertexId   = size_t;
  using size_type  = size_t;

  ///@brief Represents an edge in the graph.
  struct Edge {
    VertexId   destination; ///< Destination vertex ID.
    EdgeWeight weight;      ///< Edge weight.

    explicit Edge(VertexId dest, EdgeWeight w = EdgeWeight{}) : destination(dest), weight(w) {}
  };

  ///@brief Represents a vertex in the graph.
  struct Vertex {
    VertexData         data;      ///< Vertex data.
    DynamicArray<Edge> adjacency; ///< List of adjacent edges.

    /**
     * @brief Construct a new Vertex object.
     * @param d The data to store in the vertex.
     */
    explicit Vertex(const VertexData& d) : data(d), adjacency() {}

    /**
     * @brief Construct a new Vertex object (move version).
     * @param d The data to store in the vertex.
     */
    explicit Vertex(VertexData&& d) : data(std::move(d)), adjacency() {}

    /**
     * @brief Copy constructor.
     * @param other Vertex to copy.
     */
    Vertex(const Vertex& other)
      requires CopyVertexPayload<VertexData> && CopyAdjacencyEdge<Edge>
        : data(other.data), adjacency(other.adjacency.begin(), other.adjacency.end()) {}

    /**
     * @brief Move constructor.
     * @param other Vertex to move from.
     */
    Vertex(Vertex&& other) noexcept(std::is_nothrow_move_constructible_v<VertexData>) = default;

    /**
     * @brief Move assignment operator.
     * @param other Vertex to move from.
     * @return Reference to this.
     */
    auto operator=(Vertex&& other) noexcept(std::is_nothrow_move_assignable_v<VertexData>) -> Vertex& = default;

    // Copy assignment is disabled because adjacency storage is move-only.
    auto operator=(const Vertex&) -> Vertex& = delete;
  };

  //===----- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------------------===//

  /**
   * @brief Constructs an empty graph.
   * @param is_directed If true, creates a directed graph; otherwise undirected.
   * @complexity Time O(1), Space O(1)
   */
  explicit GraphAdjacencyList(bool is_directed = false);

  /**
   * @brief Constructs a graph with preallocated vertices.
   * @param num_vertices Number of vertices to preallocate.
   * @param is_directed If true, creates a directed graph; otherwise undirected.
   * @complexity Time O(n), Space O(n)
   */
  explicit GraphAdjacencyList(size_type num_vertices, bool is_directed = false);

  /**
   * @brief Move constructor.
   * @param other The graph to move from.
   * @complexity Time O(1), Space O(1)
   */
  GraphAdjacencyList(GraphAdjacencyList&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The graph to move from.
   * @return Reference to this.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(GraphAdjacencyList&& other) noexcept -> GraphAdjacencyList&;

  // Copy constructor and assignment are disabled (move-only type).
  GraphAdjacencyList(const GraphAdjacencyList&)                    = delete;
  auto operator=(const GraphAdjacencyList&) -> GraphAdjacencyList& = delete;

  /**
   * @brief Destructor.
   * @complexity Time O(V + E), Space O(1)
   */
  ~GraphAdjacencyList() = default;

  //===----- VERTEX OPERATIONS -------------------------------------------------===//

  /**
   * @brief Adds a vertex to the graph.
   * @param data The data to store in the vertex.
   * @return The ID of the newly added vertex.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto add_vertex(const VertexData& data) -> VertexId;

  /**
   * @brief Adds a vertex to the graph (move version).
   * @param data The data to store in the vertex.
   * @return The ID of the newly added vertex.
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto add_vertex(VertexData&& data) -> VertexId;

  /**
   * @brief Gets a reference to vertex data.
   * @param vertex_id The ID of the vertex.
   * @return Reference to the vertex data.
   * @throws GraphException if vertex_id is invalid.
   * @complexity Time O(1), Space O(1)
   */
  auto get_vertex_data(VertexId vertex_id) -> VertexData&;

  /**
   * @brief Gets a const reference to vertex data.
   * @param vertex_id The ID of the vertex.
   * @return Const reference to the vertex data.
   * @throws GraphException if vertex_id is invalid.
   * @complexity Time O(1), Space O(1)
   */
  auto get_vertex_data(VertexId vertex_id) const -> const VertexData&;

  /**
   * @brief Checks if a vertex exists.
   * @param vertex_id The ID of the vertex.
   * @return true if vertex exists, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto has_vertex(VertexId vertex_id) const noexcept -> bool;

  /**
   * @brief Returns the number of vertices in the graph.
   * @return Number of vertices.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto num_vertices() const noexcept -> size_type;

  //===----- EDGE OPERATIONS ---------------------------------------------------===//

  /**
   * @brief Adds an edge to the graph.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @param weight Edge weight (default: EdgeWeight{}).
   * @throws GraphException if vertex IDs are invalid.
   * @complexity Time O(1), Space O(1)
   * @note For undirected graphs, also adds edge from to -> from.
   */
  auto add_edge(VertexId from, VertexId to, EdgeWeight weight = EdgeWeight{}) -> void;

  /**
   * @brief Removes an edge from the graph.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @throws GraphException if vertex IDs are invalid.
   * @complexity Time O(degree(from)), Space O(1)
   * @note For undirected graphs, also removes edge from to -> from.
   */
  auto remove_edge(VertexId from, VertexId to) -> void;

  /**
   * @brief Checks if an edge exists.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @return true if edge exists, false otherwise.
   * @complexity Time O(degree(from)), Space O(1)
   */
  [[nodiscard]] auto has_edge(VertexId from, VertexId to) const -> bool;

  /**
   * @brief Gets the weight of an edge.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @return Optional containing edge weight if edge exists, nullopt otherwise.
   * @complexity Time O(degree(from)), Space O(1)
   */
  [[nodiscard]] auto get_edge_weight(VertexId from, VertexId to) const -> std::optional<EdgeWeight>;

  /**
   * @brief Returns the number of edges in the graph.
   * @return Number of edges.
   * @complexity Time O(1), Space O(1)
   * @note For undirected graphs, each edge is counted once.
   */
  [[nodiscard]] auto num_edges() const noexcept -> size_type;

  //===----- NAVIGATION OPERATIONS ---------------------------------------------===//

  /**
   * @brief Gets the list of neighbor vertex IDs.
   * @param vertex_id The vertex ID.
   * @return Vector of neighbor vertex IDs.
   * @throws GraphException if vertex_id is invalid.
   * @complexity Time O(degree(vertex_id)), Space O(degree(vertex_id))
   */
  [[nodiscard]] auto get_neighbors(VertexId vertex_id) const -> std::vector<VertexId>;

  /**
   * @brief Gets the list of neighbors with edge weights.
   * @param vertex_id The vertex ID.
   * @return Vector of pairs (neighbor_id, weight).
   * @throws GraphException if vertex_id is invalid.
   * @complexity Time O(degree(vertex_id)), Space O(degree(vertex_id))
   */
  [[nodiscard]] auto get_neighbors_with_weights(VertexId vertex_id) const
      -> std::vector<std::pair<VertexId, EdgeWeight>>;

  /**
   * @brief Visits all outgoing weighted neighbors without building a temporary container.
   * @tparam Visitor Callable invocable as visitor(size_t neighbor_id, const EdgeWeight& weight).
   * @param vertex_id Source vertex ID.
   * @param visitor Callback invoked once for each outgoing edge.
   * @throws GraphException if vertex_id is invalid.
   * @complexity Time O(degree(vertex_id)), Space O(1) excluding visitor state.
   */
  template <typename Visitor>
  auto for_each_weighted_neighbor(VertexId vertex_id, Visitor&& visitor) const -> void
    requires WeightedNeighborVisitor<Visitor, EdgeWeight>;

  /**
   * @brief Gets the degree of a vertex (number of outgoing edges).
   * @param vertex_id The vertex ID.
   * @return The degree of the vertex.
   * @throws GraphException if vertex_id is invalid.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto degree(VertexId vertex_id) const -> size_type;

  /**
   * @brief Gets the in-degree of a vertex (number of incoming edges).
   * @param vertex_id The vertex ID.
   * @return The number of edges directed into the vertex.
   * @throws GraphException if vertex_id is invalid.
   * @complexity Time O(V + E), Space O(1)
   * @note For undirected graphs this equals degree(vertex_id).
   */
  [[nodiscard]] auto in_degree(VertexId vertex_id) const -> size_type;

  //===----- QUERY OPERATIONS --------------------------------------------------===//

  /**
   * @brief Checks if the graph is directed.
   * @return true if directed, false if undirected.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_directed() const noexcept -> bool;

  /**
   * @brief Checks if the graph is empty.
   * @return true if graph has no vertices, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  //===----- CLEAR OPERATION ---------------------------------------------------===//

  /**
   * @brief Removes all vertices and edges from the graph.
   * @complexity Time O(V + E), Space O(1)
   */
  auto clear() -> void;

  //===----- TRAVERSAL ALGORITHMS ----------------------------------------------===//

  /**
   * @brief Performs breadth-first search from a starting vertex.
   * @param start_vertex The starting vertex ID.
   * @return Vector of vertex IDs in BFS order.
   * @throws GraphException if start_vertex is invalid.
   * @complexity Time O(V + E), Space O(V)
   */
  [[nodiscard]] auto bfs(VertexId start_vertex) const -> std::vector<VertexId>;

  /**
   * @brief Performs depth-first search from a starting vertex.
   * @param start_vertex The starting vertex ID.
   * @return Vector of vertex IDs in DFS order.
   * @throws GraphException if start_vertex is invalid.
   * @complexity Time O(V + E), Space O(V)
   */
  [[nodiscard]] auto dfs(VertexId start_vertex) const -> std::vector<VertexId>;

  /**
   * @brief Finds a path between two vertices using BFS.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @return Optional containing the path if it exists, nullopt otherwise.
   * @throws GraphException if vertex IDs are invalid.
   * @complexity Time O(V + E), Space O(V)
   */
  [[nodiscard]] auto find_path(VertexId from, VertexId to) const -> std::optional<std::vector<VertexId>>;

  /**
   * @brief Checks if two vertices are connected.
   * @param v1 First vertex ID.
   * @param v2 Second vertex ID.
   * @return true if there's a path from v1 to v2, false otherwise.
   * @throws GraphException if vertex IDs are invalid.
   * @complexity Time O(V + E), Space O(V)
   */
  [[nodiscard]] auto is_connected(VertexId v1, VertexId v2) const -> bool;

  /**
   * @brief Finds all connected components in an undirected graph.
   * @return Vector of components, each component is a vector of vertex IDs.
   * @complexity Time O(V + E), Space O(V)
   * @note Only meaningful for undirected graphs.
   */
  [[nodiscard]] auto connected_components() const -> std::vector<std::vector<VertexId>>;

private:
  //===----- PRIVATE HELPER METHODS --------------------------------------------===//

  /**
   * @brief Validates a vertex ID.
   * @param vertex_id The vertex ID to validate.
   * @throws GraphException if vertex_id is invalid.
   */
  auto validate_vertex(VertexId vertex_id) const -> void;

  /**
   * @brief Finds an edge index inside a source adjacency list.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @return Edge index if found, nullopt otherwise.
   */
  [[nodiscard]] auto find_edge_index(VertexId from, VertexId to) const -> std::optional<size_t>;

  //===----- DATA MEMBERS ------------------------------------------------------===//

  DynamicArray<Vertex> vertices_;    ///< Dynamic array of all vertices.
  bool                 is_directed_; ///< True if graph is directed.
  size_type            num_edges_;   ///< Number of edges.

  static constexpr VertexId kNoParent = std::numeric_limits<VertexId>::max(); ///< Sentinel for path reconstruction.
};

} // namespace ads::graphs

// Include the implementation file for templates.
#include "../../../src/ads/graphs/Graph_Adjacency_List.tpp"

#endif // GRAPH_ADJACENCY_LIST_HPP

//===---------------------------------------------------------------------------===//
