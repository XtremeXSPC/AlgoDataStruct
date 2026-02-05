//===---------------------------------------------------------------------------===//
/**
 * @file Graph_Adjacency_Matrix.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the GraphAdjacencyMatrix class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef GRAPH_ADJACENCY_MATRIX_HPP
#define GRAPH_ADJACENCY_MATRIX_HPP

#include <algorithm>
#include <cstdint>
#include <optional>
#include <queue>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ads::graphs {

/**
 * @brief Exception class for adjacency matrix graph operations.
 */
class GraphMatrixException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

/**
 * @brief A graph implemented using adjacency matrix representation.
 *
 * @details This class implements a graph using an adjacency matrix, where
 *          matrix[i][j] indicates if there's an edge from vertex i to j.
 *          This representation is memory-efficient for dense graphs but
 *          uses O(V²) space even for sparse graphs.
 *
 *          Space complexity: O(V²)
 *          Edge lookup: O(1)
 *          Neighbor iteration: O(V)
 *
 *          Trade-offs vs Adjacency List:
 *          + Edge lookup is O(1) instead of O(degree).
 *          + Simple and cache-friendly for dense graphs.
 *          - Uses O(V²) space even for sparse graphs.
 *          - Iterating neighbors is O(V) instead of O(degree).
 *
 * @tparam VertexData Type of data stored in vertices.
 * @tparam EdgeWeight Type of edge weights (default: double).
 */
template <typename VertexData = int, typename EdgeWeight = double>
class GraphAdjacencyMatrix {
public:
  /**
   * @brief Represents a vertex in the graph
   */
  struct Vertex {
    VertexData data; ///< Vertex data

    explicit Vertex(const VertexData& d) : data(d) {}

    explicit Vertex(VertexData&& d) : data(std::move(d)) {}
  };

  //===----------------- CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT ------------------===//

  /**
   * @brief Constructs an empty graph.
   * @param is_directed If true, creates a directed graph; otherwise undirected.
   * @complexity Time O(1), Space O(1)
   */
  explicit GraphAdjacencyMatrix(bool is_directed = false);

  /**
   * @brief Constructs a graph with preallocated vertices.
   * @param num_vertices Number of vertices to preallocate.
   * @param is_directed If true, creates a directed graph; otherwise undirected.
   * @complexity Time O(V^2), Space O(V^2)
   */
  explicit GraphAdjacencyMatrix(size_t num_vertices, bool is_directed = false);

  /**
   * @brief Move constructor.
   * @param other The graph to move from.
   * @complexity Time O(1), Space O(1)
   */
  GraphAdjacencyMatrix(GraphAdjacencyMatrix&& other) noexcept;

  /**
   * @brief Destructor.
   * @complexity Time O(V^2), Space O(1)
   */
  ~GraphAdjacencyMatrix() = default;

  /**
   * @brief Move assignment operator.
   * @param other The graph to move from.
   * @return Reference to this.
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(GraphAdjacencyMatrix&& other) noexcept -> GraphAdjacencyMatrix&;

  // Copy constructor and assignment are disabled (move-only type).
  GraphAdjacencyMatrix(const GraphAdjacencyMatrix&)                    = delete;
  auto operator=(const GraphAdjacencyMatrix&) -> GraphAdjacencyMatrix& = delete;

  //===--------------------------- VERTEX OPERATIONS ---------------------------===//

  /**
   * @brief Adds a vertex to the graph.
   * @param data The data to store in the vertex.
   * @return The ID of the newly added vertex.
   * @complexity Time O(V), Space O(V)
   * @note Resizes the adjacency matrix to remain square.
   */
  auto add_vertex(const VertexData& data) -> size_t;

  /**
   * @brief Adds a vertex to the graph (move version).
   * @param data The data to store in the vertex.
   * @return The ID of the newly added vertex.
   * @complexity Time O(V), Space O(V)
   * @note Resizes the adjacency matrix to remain square.
   */
  auto add_vertex(VertexData&& data) -> size_t;

  /**
   * @brief Gets a reference to vertex data.
   * @param vertex_id The ID of the vertex.
   * @return Reference to the vertex data.
   * @throws GraphMatrixException if vertex_id is invalid.
   * @complexity Time O(1), Space O(1)
   */
  auto get_vertex_data(size_t vertex_id) -> VertexData&;

  /**
   * @brief Gets a const reference to vertex data.
   * @param vertex_id The ID of the vertex.
   * @return Const reference to the vertex data.
   * @throws GraphMatrixException if vertex_id is invalid.
   * @complexity Time O(1), Space O(1)
   */
  auto get_vertex_data(size_t vertex_id) const -> const VertexData&;

  /**
   * @brief Checks if a vertex exists.
   * @param vertex_id The ID of the vertex.
   * @return true if vertex exists, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto has_vertex(size_t vertex_id) const noexcept -> bool;

  /**
   * @brief Returns the number of vertices in the graph.
   * @return Number of vertices.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto num_vertices() const noexcept -> size_t;

  //===---------------------------- EDGE OPERATIONS ----------------------------===//

  /**
   * @brief Adds an edge to the graph.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @param weight Edge weight (default: EdgeWeight{}).
   * @throws GraphMatrixException if vertex IDs are invalid.
   * @complexity Time O(1), Space O(1)
   * @note For undirected graphs, also adds edge from to -> from.
   */
  auto add_edge(size_t from, size_t to, EdgeWeight weight = EdgeWeight{}) -> void;

  /**
   * @brief Removes an edge from the graph.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @throws GraphMatrixException if vertex IDs are invalid.
   * @complexity Time O(1), Space O(1)
   * @note For undirected graphs, also removes edge from to -> from.
   */
  auto remove_edge(size_t from, size_t to) -> void;

  /**
   * @brief Checks if an edge exists.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @return true if edge exists, false otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto has_edge(size_t from, size_t to) const -> bool;

  /**
   * @brief Gets the weight of an edge.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @return Optional containing edge weight if edge exists, nullopt otherwise.
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto get_edge_weight(size_t from, size_t to) const -> std::optional<EdgeWeight>;

  /**
   * @brief Returns the number of edges in the graph.
   * @return Number of edges.
   * @complexity Time O(1), Space O(1)
   * @note For undirected graphs, each edge is counted once.
   */
  [[nodiscard]] auto num_edges() const noexcept -> size_t;

  //===------------------------- NAVIGATION OPERATIONS -------------------------===//

  /**
   * @brief Gets the list of neighbor vertex IDs.
   * @param vertex_id The vertex ID.
   * @return Vector of neighbor vertex IDs.
   * @throws GraphMatrixException if vertex_id is invalid.
   * @complexity Time O(V), Space O(V)
   */
  [[nodiscard]] auto get_neighbors(size_t vertex_id) const -> std::vector<size_t>;

  /**
   * @brief Gets the list of neighbors with edge weights.
   * @param vertex_id The vertex ID.
   * @return Vector of pairs (neighbor_id, weight).
   * @throws GraphMatrixException if vertex_id is invalid.
   * @complexity Time O(V), Space O(V)
   */
  [[nodiscard]] auto get_neighbors_with_weights(size_t vertex_id) const -> std::vector<std::pair<size_t, EdgeWeight>>;

  /**
   * @brief Gets the degree of a vertex (number of outgoing edges).
   * @param vertex_id The vertex ID.
   * @return The degree of the vertex.
   * @throws GraphMatrixException if vertex_id is invalid.
   * @complexity Time O(V), Space O(1)
   */
  [[nodiscard]] auto degree(size_t vertex_id) const -> size_t;

  //===--------------------------- QUERY OPERATIONS ----------------------------===//

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

  //===---------------------------- CLEAR OPERATION ----------------------------===//

  /**
   * @brief Removes all vertices and edges from the graph.
   * @complexity Time O(V^2), Space O(1)
   */
  auto clear() -> void;

  //===------------------------- TRAVERSAL ALGORITHMS --------------------------===//

  /**
   * @brief Performs breadth-first search from a starting vertex.
   * @param start_vertex The starting vertex ID.
   * @return Vector of vertex IDs in BFS order.
   * @throws GraphMatrixException if start_vertex is invalid.
   * @complexity Time O(V^2), Space O(V)
   */
  [[nodiscard]] auto bfs(size_t start_vertex) const -> std::vector<size_t>;

  /**
   * @brief Performs depth-first search from a starting vertex.
   * @param start_vertex The starting vertex ID.
   * @return Vector of vertex IDs in DFS order.
   * @throws GraphMatrixException if start_vertex is invalid.
   * @complexity Time O(V^2), Space O(V)
   */
  [[nodiscard]] auto dfs(size_t start_vertex) const -> std::vector<size_t>;

  /**
   * @brief Finds a path between two vertices using BFS.
   * @param from Source vertex ID.
   * @param to Destination vertex ID.
   * @return Optional containing the path if it exists, nullopt otherwise.
   * @throws GraphMatrixException if vertex IDs are invalid.
   * @complexity Time O(V^2), Space O(V)
   */
  [[nodiscard]] auto find_path(size_t from, size_t to) const -> std::optional<std::vector<size_t>>;

  /**
   * @brief Checks if two vertices are connected.
   * @param v1 First vertex ID.
   * @param v2 Second vertex ID.
   * @return true if there's a path from v1 to v2, false otherwise.
   * @throws GraphMatrixException if vertex IDs are invalid.
   * @complexity Time O(V^2), Space O(V)
   */
  [[nodiscard]] auto is_connected(size_t v1, size_t v2) const -> bool;

  /**
   * @brief Finds all connected components in an undirected graph.
   * @return Vector of components, each component is a vector of vertex IDs.
   * @complexity Time O(V^2), Space O(V)
   * @note Only meaningful for undirected graphs.
   */
  [[nodiscard]] auto connected_components() const -> std::vector<std::vector<size_t>>;

private:
  //===----------------------------- DATA MEMBERS ------------------------------===//
  template <typename T>
  using VVC = std::vector<std::vector<T>>;

  std::vector<Vertex>            vertices_; ///< Vector of vertices.
  VVC<std::optional<EdgeWeight>> matrix_;   ///< Adjacency matrix.

  bool   is_directed_; ///< Graph type.
  size_t num_edges_;   ///< Edge count.

  //===============================================================================//
  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /**
   * @brief Validates a vertex ID.
   * @param vertex_id The vertex ID to validate.
   * @throws GraphMatrixException if vertex_id is invalid.
   */
  auto validate_vertex(size_t vertex_id) const -> void;

  /**
   * @brief Helper for DFS traversal (recursive).
   * @param vertex_id Current vertex ID.
   * @param visited Vector tracking visited vertices.
   * @param result Vector storing DFS order.
   */
  auto dfs_helper(size_t vertex_id, std::vector<bool>& visited, std::vector<size_t>& result) const -> void;

  /**
   * @brief Resizes the adjacency matrix to new_size x new_size.
   * @param new_size New matrix dimension.
   */
  auto resize_matrix(size_t new_size) -> void;
};

} // namespace ads::graphs

// Include the implementation file for templates.
#include "../../../src/ads/graphs/Graph_Adjacency_Matrix.tpp"

#endif // GRAPH_ADJACENCY_MATRIX_HPP

//===--------------------------------------------------------------------------===//
