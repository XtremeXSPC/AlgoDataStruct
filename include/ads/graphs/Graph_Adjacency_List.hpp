//===--------------------------------------------------------------------------===//
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
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef GRAPH_ADJACENCY_LIST_HPP
#define GRAPH_ADJACENCY_LIST_HPP

#include <optional>
#include <queue>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ads::graph {

/**
 * @brief Exception class for graph operations
 */
class GraphException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

/**
 * @brief A graph implemented using adjacency list representation
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
 *          - Directed and undirected graphs
 *          - Weighted and unweighted edges
 *          - Custom vertex data types
 *          - Graph traversal algorithms (BFS, DFS)
 *          - Path finding
 *          - Connected components (for undirected graphs)
 *
 * @tparam VertexData Type of data stored in vertices
 * @tparam EdgeWeight Type of edge weights (default: double)
 *
 * @example
 * ```cpp
 * // Create an undirected, weighted graph
 * GraphAdjacencyList<std::string, double> graph(false);
 *
 * // Add vertices
 * size_t v0 = graph.add_vertex("A");
 * size_t v1 = graph.add_vertex("B");
 * size_t v2 = graph.add_vertex("C");
 *
 * // Add weighted edges
 * graph.add_edge(v0, v1, 5.0);
 * graph.add_edge(v1, v2, 3.0);
 *
 * // Traverse the graph
 * auto order = graph.bfs(v0);
 * ```
 */
template <typename VertexData = int, typename EdgeWeight = double>
class GraphAdjacencyList {
public:
  /**
   * @brief Represents an edge in the graph
   */
  struct Edge {
    size_t     destination; ///< Destination vertex ID
    EdgeWeight weight;      ///< Edge weight

    Edge(size_t dest, EdgeWeight w = EdgeWeight{}) : destination(dest), weight(w) {}
  };

  /**
   * @brief Represents a vertex in the graph
   */
  struct Vertex {
    VertexData        data;      ///< Vertex data
    std::vector<Edge> adjacency; ///< List of adjacent edges

    explicit Vertex(const VertexData& d) : data(d), adjacency() {}
    explicit Vertex(VertexData&& d) : data(std::move(d)), adjacency() {}
  };

  //========== CONSTRUCTORS AND ASSIGNMENT ==========//

  /**
   * @brief Constructs an empty graph
   * @param is_directed If true, creates a directed graph; otherwise undirected
   * @complexity Time O(1), Space O(1)
   */
  explicit GraphAdjacencyList(bool is_directed = false);

  /**
   * @brief Constructs a graph with preallocated vertices
   * @param num_vertices Number of vertices to preallocate
   * @param is_directed If true, creates a directed graph; otherwise undirected
   * @complexity Time O(n), Space O(n)
   */
  GraphAdjacencyList(size_t num_vertices, bool is_directed = false);

  /**
   * @brief Move constructor
   * @param other The graph to move from
   * @complexity Time O(1), Space O(1)
   */
  GraphAdjacencyList(GraphAdjacencyList&& other) noexcept;

  /**
   * @brief Move assignment operator
   * @param other The graph to move from
   * @return Reference to this
   * @complexity Time O(1), Space O(1)
   */
  auto operator=(GraphAdjacencyList&& other) noexcept -> GraphAdjacencyList&;

  // Disable copy constructor and copy assignment (can be enabled if needed)
  GraphAdjacencyList(const GraphAdjacencyList&)                    = delete;
  auto operator=(const GraphAdjacencyList&) -> GraphAdjacencyList& = delete;

  /**
   * @brief Destructor
   * @complexity Time O(V + E), Space O(1)
   */
  ~GraphAdjacencyList() = default;

  //========== VERTEX OPERATIONS ==========//

  /**
   * @brief Adds a vertex to the graph
   * @param data The data to store in the vertex
   * @return The ID of the newly added vertex
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto add_vertex(const VertexData& data) -> size_t;

  /**
   * @brief Adds a vertex to the graph (move version)
   * @param data The data to store in the vertex
   * @return The ID of the newly added vertex
   * @complexity Time O(1) amortized, Space O(1)
   */
  auto add_vertex(VertexData&& data) -> size_t;

  /**
   * @brief Gets a reference to vertex data
   * @param vertex_id The ID of the vertex
   * @return Reference to the vertex data
   * @throws GraphException if vertex_id is invalid
   * @complexity Time O(1), Space O(1)
   */
  auto get_vertex_data(size_t vertex_id) -> VertexData&;

  /**
   * @brief Gets a const reference to vertex data
   * @param vertex_id The ID of the vertex
   * @return Const reference to the vertex data
   * @throws GraphException if vertex_id is invalid
   * @complexity Time O(1), Space O(1)
   */
  auto get_vertex_data(size_t vertex_id) const -> const VertexData&;

  /**
   * @brief Checks if a vertex exists
   * @param vertex_id The ID of the vertex
   * @return true if vertex exists, false otherwise
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto has_vertex(size_t vertex_id) const noexcept -> bool;

  /**
   * @brief Returns the number of vertices in the graph
   * @return Number of vertices
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto num_vertices() const noexcept -> size_t;

  //========== EDGE OPERATIONS ==========//

  /**
   * @brief Adds an edge to the graph
   * @param from Source vertex ID
   * @param to Destination vertex ID
   * @param weight Edge weight (default: EdgeWeight{})
   * @throws GraphException if vertex IDs are invalid
   * @complexity Time O(1), Space O(1)
   * @note For undirected graphs, also adds edge from to -> from
   */
  auto add_edge(size_t from, size_t to, EdgeWeight weight = EdgeWeight{}) -> void;

  /**
   * @brief Removes an edge from the graph
   * @param from Source vertex ID
   * @param to Destination vertex ID
   * @throws GraphException if vertex IDs are invalid
   * @complexity Time O(degree(from)), Space O(1)
   * @note For undirected graphs, also removes edge from to -> from
   */
  auto remove_edge(size_t from, size_t to) -> void;

  /**
   * @brief Checks if an edge exists
   * @param from Source vertex ID
   * @param to Destination vertex ID
   * @return true if edge exists, false otherwise
   * @complexity Time O(degree(from)), Space O(1)
   */
  [[nodiscard]] auto has_edge(size_t from, size_t to) const -> bool;

  /**
   * @brief Gets the weight of an edge
   * @param from Source vertex ID
   * @param to Destination vertex ID
   * @return Optional containing edge weight if edge exists, nullopt otherwise
   * @complexity Time O(degree(from)), Space O(1)
   */
  [[nodiscard]] auto get_edge_weight(size_t from, size_t to) const -> std::optional<EdgeWeight>;

  /**
   * @brief Returns the number of edges in the graph
   * @return Number of edges
   * @complexity Time O(1), Space O(1)
   * @note For undirected graphs, each edge is counted once
   */
  [[nodiscard]] auto num_edges() const noexcept -> size_t;

  //========== NAVIGATION OPERATIONS ==========//

  /**
   * @brief Gets the list of neighbor vertex IDs
   * @param vertex_id The vertex ID
   * @return Vector of neighbor vertex IDs
   * @throws GraphException if vertex_id is invalid
   * @complexity Time O(degree(vertex_id)), Space O(degree(vertex_id))
   */
  [[nodiscard]] auto get_neighbors(size_t vertex_id) const -> std::vector<size_t>;

  /**
   * @brief Gets the list of neighbors with edge weights
   * @param vertex_id The vertex ID
   * @return Vector of pairs (neighbor_id, weight)
   * @throws GraphException if vertex_id is invalid
   * @complexity Time O(degree(vertex_id)), Space O(degree(vertex_id))
   */
  [[nodiscard]] auto get_neighbors_with_weights(size_t vertex_id) const -> std::vector<std::pair<size_t, EdgeWeight>>;

  /**
   * @brief Gets the degree of a vertex (number of outgoing edges)
   * @param vertex_id The vertex ID
   * @return The degree of the vertex
   * @throws GraphException if vertex_id is invalid
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto degree(size_t vertex_id) const -> size_t;

  //========== QUERY OPERATIONS ==========//

  /**
   * @brief Checks if the graph is directed
   * @return true if directed, false if undirected
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_directed() const noexcept -> bool;

  /**
   * @brief Checks if the graph is empty
   * @return true if graph has no vertices, false otherwise
   * @complexity Time O(1), Space O(1)
   */
  [[nodiscard]] auto is_empty() const noexcept -> bool;

  /**
   * @brief Removes all vertices and edges from the graph
   * @complexity Time O(V + E), Space O(1)
   */
  auto clear() -> void;

  //========== TRAVERSAL ALGORITHMS ==========//

  /**
   * @brief Performs breadth-first search from a starting vertex
   * @param start_vertex The starting vertex ID
   * @return Vector of vertex IDs in BFS order
   * @throws GraphException if start_vertex is invalid
   * @complexity Time O(V + E), Space O(V)
   */
  [[nodiscard]] auto bfs(size_t start_vertex) const -> std::vector<size_t>;

  /**
   * @brief Performs depth-first search from a starting vertex
   * @param start_vertex The starting vertex ID
   * @return Vector of vertex IDs in DFS order
   * @throws GraphException if start_vertex is invalid
   * @complexity Time O(V + E), Space O(V)
   */
  [[nodiscard]] auto dfs(size_t start_vertex) const -> std::vector<size_t>;

  /**
   * @brief Finds a path between two vertices using BFS
   * @param from Source vertex ID
   * @param to Destination vertex ID
   * @return Optional containing the path if it exists, nullopt otherwise
   * @throws GraphException if vertex IDs are invalid
   * @complexity Time O(V + E), Space O(V)
   */
  [[nodiscard]] auto find_path(size_t from, size_t to) const -> std::optional<std::vector<size_t>>;

  /**
   * @brief Checks if two vertices are connected
   * @param v1 First vertex ID
   * @param v2 Second vertex ID
   * @return true if there's a path from v1 to v2, false otherwise
   * @throws GraphException if vertex IDs are invalid
   * @complexity Time O(V + E), Space O(V)
   */
  [[nodiscard]] auto is_connected(size_t v1, size_t v2) const -> bool;

  /**
   * @brief Finds all connected components in an undirected graph
   * @return Vector of components, each component is a vector of vertex IDs
   * @complexity Time O(V + E), Space O(V)
   * @note Only meaningful for undirected graphs
   */
  [[nodiscard]] auto connected_components() const -> std::vector<std::vector<size_t>>;

private:
  //========== PRIVATE MEMBERS ==========//

  std::vector<Vertex> vertices_;    ///< Vector of all vertices
  bool                is_directed_; ///< True if graph is directed
  size_t              num_edges_;   ///< Number of edges

  //========== PRIVATE HELPER METHODS ==========//

  /**
   * @brief Validates a vertex ID
   * @param vertex_id The vertex ID to validate
   * @throws GraphException if vertex_id is invalid
   */
  auto validate_vertex(size_t vertex_id) const -> void;

  /**
   * @brief Helper for DFS traversal (recursive)
   * @param vertex_id Current vertex ID
   * @param visited Vector tracking visited vertices
   * @param result Vector storing DFS order
   */
  auto dfs_helper(size_t vertex_id, std::vector<bool>& visited, std::vector<size_t>& result) const -> void;
};

} // namespace ads::graph

#include "../../../src/ads/graphs/Graph_Adjacency_List.tpp"

#endif // GRAPH_ADJACENCY_LIST_HPP
//===--------------------------------------------------------------------------===//
