//===--------------------------------------------------------------------------===//
/**
 * @file Graph_Adjacency_Matrix.hpp
 * @author Costantino Lombardi
 * @brief Declaration of the GraphAdjacencyMatrix class
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//

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

// Reuse GraphException from Graph_Adjacency_List.hpp
class GraphMatrixException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

/**
 * @brief A graph implemented using adjacency matrix representation
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
 *          + Edge lookup is O(1) instead of O(degree)
 *          + Simple and cache-friendly for dense graphs
 *          - Uses O(V²) space even for sparse graphs
 *          - Iterating neighbors is O(V) instead of O(degree)
 *
 * @tparam VertexData Type of data stored in vertices
 * @tparam EdgeWeight Type of edge weights (default: double)
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

  //========== CONSTRUCTORS AND ASSIGNMENT ==========//
  explicit GraphAdjacencyMatrix(bool is_directed = false);
  GraphAdjacencyMatrix(size_t num_vertices, bool is_directed = false);
  GraphAdjacencyMatrix(GraphAdjacencyMatrix&& other) noexcept;
  auto operator=(GraphAdjacencyMatrix&& other) noexcept -> GraphAdjacencyMatrix&;

  // Disable copy
  GraphAdjacencyMatrix(const GraphAdjacencyMatrix&)                    = delete;
  auto operator=(const GraphAdjacencyMatrix&) -> GraphAdjacencyMatrix& = delete;
  ~GraphAdjacencyMatrix()                                              = default;

  //========== VERTEX OPERATIONS ==========//
  auto               add_vertex(const VertexData& data) -> size_t;
  auto               add_vertex(VertexData&& data) -> size_t;
  auto               get_vertex_data(size_t vertex_id) -> VertexData&;
  auto               get_vertex_data(size_t vertex_id) const -> const VertexData&;
  [[nodiscard]] auto has_vertex(size_t vertex_id) const noexcept -> bool;
  [[nodiscard]] auto num_vertices() const noexcept -> size_t;

  //========== EDGE OPERATIONS ==========//
  auto               add_edge(size_t from, size_t to, EdgeWeight weight = EdgeWeight{}) -> void;
  auto               remove_edge(size_t from, size_t to) -> void;
  [[nodiscard]] auto has_edge(size_t from, size_t to) const -> bool;
  [[nodiscard]] auto get_edge_weight(size_t from, size_t to) const -> std::optional<EdgeWeight>;
  [[nodiscard]] auto num_edges() const noexcept -> size_t;

  //========== NAVIGATION OPERATIONS ==========//
  [[nodiscard]] auto get_neighbors(size_t vertex_id) const -> std::vector<size_t>;
  [[nodiscard]] auto get_neighbors_with_weights(size_t vertex_id) const -> std::vector<std::pair<size_t, EdgeWeight>>;
  [[nodiscard]] auto degree(size_t vertex_id) const -> size_t;

  //========== QUERY OPERATIONS ==========//
  [[nodiscard]] auto is_directed() const noexcept -> bool;
  [[nodiscard]] auto is_empty() const noexcept -> bool;
  auto               clear() -> void;

  //========== TRAVERSAL ALGORITHMS ==========//
  [[nodiscard]] auto bfs(size_t start_vertex) const -> std::vector<size_t>;
  [[nodiscard]] auto dfs(size_t start_vertex) const -> std::vector<size_t>;
  [[nodiscard]] auto find_path(size_t from, size_t to) const -> std::optional<std::vector<size_t>>;
  [[nodiscard]] auto is_connected(size_t v1, size_t v2) const -> bool;
  [[nodiscard]] auto connected_components() const -> std::vector<std::vector<size_t>>;

private:
  std::vector<Vertex>                                 vertices_;    ///< Vector of vertices
  std::vector<std::vector<std::optional<EdgeWeight>>> matrix_;      ///< Adjacency matrix
  bool                                                is_directed_; ///< Graph type
  size_t                                              num_edges_;   ///< Edge count

  auto validate_vertex(size_t vertex_id) const -> void;
  auto dfs_helper(size_t vertex_id, std::vector<bool>& visited, std::vector<size_t>& result) const -> void;
  auto resize_matrix(size_t new_size) -> void;
};

} // namespace ads::graph

#include "../../../src/ads/graphs/Graph_Adjacency_Matrix.tpp"

#endif // GRAPH_ADJACENCY_MATRIX_HPP
//===--------------------------------------------------------------------------===//
