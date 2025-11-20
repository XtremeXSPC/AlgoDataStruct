//===--------------------------------------------------------------------------===//
/**
 * @file Graph_Adjacency_Matrix.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the GraphAdjacencyMatrix class
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#include "../../../include/ads/graphs/Graph_Adjacency_Matrix.hpp"
#include <algorithm>

namespace ads::graph {

//========== CONSTRUCTORS AND ASSIGNMENT ==========//

template <typename VertexData, typename EdgeWeight>
GraphAdjacencyMatrix<VertexData, EdgeWeight>::GraphAdjacencyMatrix(bool is_directed)
    : vertices_(), matrix_(), is_directed_(is_directed), num_edges_(0) {
}

template <typename VertexData, typename EdgeWeight>
GraphAdjacencyMatrix<VertexData, EdgeWeight>::GraphAdjacencyMatrix(size_t num_vertices, bool is_directed)
    : vertices_(), matrix_(num_vertices, std::vector<std::optional<EdgeWeight>>(num_vertices)), is_directed_(is_directed),
      num_edges_(0) {
  vertices_.reserve(num_vertices);
}

template <typename VertexData, typename EdgeWeight>
GraphAdjacencyMatrix<VertexData, EdgeWeight>::GraphAdjacencyMatrix(GraphAdjacencyMatrix&& other) noexcept
    : vertices_(std::move(other.vertices_)), matrix_(std::move(other.matrix_)), is_directed_(other.is_directed_),
      num_edges_(other.num_edges_) {
  other.num_edges_ = 0;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::operator=(GraphAdjacencyMatrix&& other) noexcept -> GraphAdjacencyMatrix& {
  if (this != &other) {
    vertices_    = std::move(other.vertices_);
    matrix_      = std::move(other.matrix_);
    is_directed_ = other.is_directed_;
    num_edges_   = other.num_edges_;
    other.num_edges_ = 0;
  }
  return *this;
}

//========== VERTEX OPERATIONS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::add_vertex(const VertexData& data) -> size_t {
  vertices_.emplace_back(data);
  resize_matrix(vertices_.size());
  return vertices_.size() - 1;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::add_vertex(VertexData&& data) -> size_t {
  vertices_.emplace_back(std::move(data));
  resize_matrix(vertices_.size());
  return vertices_.size() - 1;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::get_vertex_data(size_t vertex_id) -> VertexData& {
  validate_vertex(vertex_id);
  return vertices_[vertex_id].data;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::get_vertex_data(size_t vertex_id) const -> const VertexData& {
  validate_vertex(vertex_id);
  return vertices_[vertex_id].data;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::has_vertex(size_t vertex_id) const noexcept -> bool {
  return vertex_id < vertices_.size();
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::num_vertices() const noexcept -> size_t {
  return vertices_.size();
}

//========== EDGE OPERATIONS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::add_edge(size_t from, size_t to, EdgeWeight weight) -> void {
  validate_vertex(from);
  validate_vertex(to);

  if (!matrix_[from][to].has_value()) {
    matrix_[from][to] = weight;
    ++num_edges_;

    if (!is_directed_ && from != to) {
      matrix_[to][from] = weight;
    }
  }
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::remove_edge(size_t from, size_t to) -> void {
  validate_vertex(from);
  validate_vertex(to);

  if (matrix_[from][to].has_value()) {
    matrix_[from][to].reset();
    --num_edges_;

    if (!is_directed_ && from != to) {
      matrix_[to][from].reset();
    }
  }
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::has_edge(size_t from, size_t to) const -> bool {
  if (!has_vertex(from) || !has_vertex(to)) {
    return false;
  }
  return matrix_[from][to].has_value();
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::get_edge_weight(size_t from, size_t to) const -> std::optional<EdgeWeight> {
  if (!has_vertex(from) || !has_vertex(to)) {
    return std::nullopt;
  }
  return matrix_[from][to];
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::num_edges() const noexcept -> size_t {
  return num_edges_;
}

//========== NAVIGATION OPERATIONS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::get_neighbors(size_t vertex_id) const -> std::vector<size_t> {
  validate_vertex(vertex_id);

  std::vector<size_t> neighbors;
  for (size_t i = 0; i < matrix_[vertex_id].size(); ++i) {
    if (matrix_[vertex_id][i].has_value()) {
      neighbors.push_back(i);
    }
  }
  return neighbors;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::get_neighbors_with_weights(size_t vertex_id) const
    -> std::vector<std::pair<size_t, EdgeWeight>> {
  validate_vertex(vertex_id);

  std::vector<std::pair<size_t, EdgeWeight>> neighbors;
  for (size_t i = 0; i < matrix_[vertex_id].size(); ++i) {
    if (matrix_[vertex_id][i].has_value()) {
      neighbors.emplace_back(i, *matrix_[vertex_id][i]);
    }
  }
  return neighbors;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::degree(size_t vertex_id) const -> size_t {
  validate_vertex(vertex_id);

  size_t deg = 0;
  for (size_t i = 0; i < matrix_[vertex_id].size(); ++i) {
    if (matrix_[vertex_id][i].has_value()) {
      ++deg;
    }
  }
  return deg;
}

//========== QUERY OPERATIONS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::is_directed() const noexcept -> bool {
  return is_directed_;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::is_empty() const noexcept -> bool {
  return vertices_.empty();
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::clear() -> void {
  vertices_.clear();
  matrix_.clear();
  num_edges_ = 0;
}

//========== TRAVERSAL ALGORITHMS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::bfs(size_t start_vertex) const -> std::vector<size_t> {
  validate_vertex(start_vertex);

  std::vector<size_t> result;
  std::vector<bool>   visited(vertices_.size(), false);
  std::queue<size_t>  queue;

  queue.push(start_vertex);
  visited[start_vertex] = true;

  while (!queue.empty()) {
    size_t current = queue.front();
    queue.pop();
    result.push_back(current);

    for (size_t i = 0; i < matrix_[current].size(); ++i) {
      if (matrix_[current][i].has_value() && !visited[i]) {
        visited[i] = true;
        queue.push(i);
      }
    }
  }

  return result;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::dfs(size_t start_vertex) const -> std::vector<size_t> {
  validate_vertex(start_vertex);

  std::vector<size_t> result;
  std::vector<bool>   visited(vertices_.size(), false);

  dfs_helper(start_vertex, visited, result);

  return result;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::find_path(size_t from, size_t to) const -> std::optional<std::vector<size_t>> {
  validate_vertex(from);
  validate_vertex(to);

  if (from == to) {
    return std::vector<size_t>{from};
  }

  std::vector<bool>   visited(vertices_.size(), false);
  std::vector<size_t> parent(vertices_.size(), SIZE_MAX);
  std::queue<size_t>  queue;

  queue.push(from);
  visited[from] = true;

  while (!queue.empty()) {
    size_t current = queue.front();
    queue.pop();

    if (current == to) {
      std::vector<size_t> path;
      size_t              node = to;

      while (node != from) {
        path.push_back(node);
        node = parent[node];
      }
      path.push_back(from);

      std::reverse(path.begin(), path.end());
      return path;
    }

    for (size_t i = 0; i < matrix_[current].size(); ++i) {
      if (matrix_[current][i].has_value() && !visited[i]) {
        visited[i] = true;
        parent[i]  = current;
        queue.push(i);
      }
    }
  }

  return std::nullopt;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::is_connected(size_t v1, size_t v2) const -> bool {
  return find_path(v1, v2).has_value();
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::connected_components() const -> std::vector<std::vector<size_t>> {
  std::vector<std::vector<size_t>> components;
  std::vector<bool>                visited(vertices_.size(), false);

  for (size_t i = 0; i < vertices_.size(); ++i) {
    if (!visited[i]) {
      std::vector<size_t> component;
      std::queue<size_t>  queue;

      queue.push(i);
      visited[i] = true;

      while (!queue.empty()) {
        size_t current = queue.front();
        queue.pop();
        component.push_back(current);

        for (size_t j = 0; j < matrix_[current].size(); ++j) {
          if (matrix_[current][j].has_value() && !visited[j]) {
            visited[j] = true;
            queue.push(j);
          }
        }
      }

      components.push_back(std::move(component));
    }
  }

  return components;
}

//========== PRIVATE HELPER METHODS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::validate_vertex(size_t vertex_id) const -> void {
  if (vertex_id >= vertices_.size()) {
    throw GraphMatrixException("Invalid vertex ID: " + std::to_string(vertex_id));
  }
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::dfs_helper(size_t vertex_id, std::vector<bool>& visited,
                                                               std::vector<size_t>& result) const -> void {
  visited[vertex_id] = true;
  result.push_back(vertex_id);

  for (size_t i = 0; i < matrix_[vertex_id].size(); ++i) {
    if (matrix_[vertex_id][i].has_value() && !visited[i]) {
      dfs_helper(i, visited, result);
    }
  }
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::resize_matrix(size_t new_size) -> void {
  // Resize rows
  matrix_.resize(new_size);

  // Resize columns
  for (auto& row : matrix_) {
    row.resize(new_size);
  }
}

}  // namespace ads::graph
//===--------------------------------------------------------------------------===//
