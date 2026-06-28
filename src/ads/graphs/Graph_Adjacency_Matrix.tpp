//===---------------------------------------------------------------------------===//
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
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/graphs/Graph_Adjacency_Matrix.hpp"

namespace ads::graphs {

//===----- CONSTRUCTORS AND ASSIGNMENT -----------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
GraphAdjacencyMatrix<VertexData, EdgeWeight>::GraphAdjacencyMatrix(bool is_directed) :
    vertices_(),
    matrix_(),
    is_directed_(is_directed),
    num_edges_(0) {
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
GraphAdjacencyMatrix<VertexData, EdgeWeight>::GraphAdjacencyMatrix(size_type num_vertices, bool is_directed) :
    vertices_(),
    matrix_(),
    is_directed_(is_directed),
    num_edges_(0) {
  vertices_.reserve(num_vertices);
  matrix_.reserve(num_vertices);
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
GraphAdjacencyMatrix<VertexData, EdgeWeight>::GraphAdjacencyMatrix(GraphAdjacencyMatrix&& other) noexcept :
    vertices_(std::move(other.vertices_)),
    matrix_(std::move(other.matrix_)),
    is_directed_(other.is_directed_),
    num_edges_(other.num_edges_) {
  other.num_edges_ = 0;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::operator=(GraphAdjacencyMatrix&& other) noexcept
    -> GraphAdjacencyMatrix& {
  if (this != &other) {
    vertices_        = std::move(other.vertices_);
    matrix_          = std::move(other.matrix_);
    is_directed_     = other.is_directed_;
    num_edges_       = other.num_edges_;
    other.num_edges_ = 0;
  }
  return *this;
}

//===----- VERTEX OPERATIONS ---------------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::add_vertex(const VertexData& data) -> VertexId {
  vertices_.emplace_back(data);
  // Keep the adjacency matrix square and aligned with the vertex count.
  resize_matrix(vertices_.size());
  return vertices_.size() - 1;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::add_vertex(VertexData&& data) -> VertexId {
  vertices_.emplace_back(std::move(data));
  // Keep the adjacency matrix square and aligned with the vertex count.
  resize_matrix(vertices_.size());
  return vertices_.size() - 1;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::get_vertex_data(VertexId vertex_id) -> VertexData& {
  validate_vertex(vertex_id);
  return vertices_[vertex_id].data;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::get_vertex_data(VertexId vertex_id) const -> const VertexData& {
  validate_vertex(vertex_id);
  return vertices_[vertex_id].data;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::has_vertex(VertexId vertex_id) const noexcept -> bool {
  return vertex_id < vertices_.size();
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::num_vertices() const noexcept -> size_type {
  return vertices_.size();
}

//===----- EDGE OPERATIONS -----------------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::add_edge(VertexId from, VertexId to, EdgeWeight weight) -> void {
  validate_vertex(from);
  validate_vertex(to);

  // A fresh edge bumps the count; an existing one simply adopts the new weight.
  const bool is_new_edge = !matrix_[from][to].has_value();
  matrix_[from][to]      = weight;
  if (is_new_edge) {
    ++num_edges_;
  }

  if (!is_directed_ && from != to) {
    matrix_[to][from] = weight;
    // For undirected graphs, num_edges_ counts the edge only once.
  }
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::remove_edge(VertexId from, VertexId to) -> void {
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

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::has_edge(VertexId from, VertexId to) const -> bool {
  if (!has_vertex(from) || !has_vertex(to)) {
    return false;
  }
  return matrix_[from][to].has_value();
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::get_edge_weight(VertexId from, VertexId to) const
    -> std::optional<EdgeWeight> {
  if (!has_vertex(from) || !has_vertex(to)) {
    return std::nullopt;
  }
  return matrix_[from][to];
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::num_edges() const noexcept -> size_type {
  return num_edges_;
}

//===----- NAVIGATION OPERATIONS -----------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::get_neighbors(VertexId vertex_id) const -> std::vector<VertexId> {
  validate_vertex(vertex_id);

  std::vector<VertexId> neighbors;
  for (size_t i = 0; i < matrix_[vertex_id].size(); ++i) {
    if (matrix_[vertex_id][i].has_value()) {
      neighbors.push_back(i);
    }
  }
  return neighbors;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::get_neighbors_with_weights(VertexId vertex_id) const
    -> std::vector<std::pair<VertexId, EdgeWeight>> {
  validate_vertex(vertex_id);

  std::vector<std::pair<VertexId, EdgeWeight>> neighbors;
  for (size_t i = 0; i < matrix_[vertex_id].size(); ++i) {
    if (matrix_[vertex_id][i].has_value()) {
      neighbors.emplace_back(i, *matrix_[vertex_id][i]);
    }
  }
  return neighbors;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
template <typename Visitor>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::for_each_weighted_neighbor(
    VertexId vertex_id, Visitor&& visitor) const -> void
  requires WeightedNeighborVisitor<Visitor, EdgeWeight>
{
  validate_vertex(vertex_id);

  for (size_t i = 0; i < matrix_[vertex_id].size(); ++i) {
    if (matrix_[vertex_id][i].has_value()) {
      std::forward<Visitor>(visitor)(i, *matrix_[vertex_id][i]);
    }
  }
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::degree(VertexId vertex_id) const -> size_type {
  validate_vertex(vertex_id);

  size_type deg = 0;
  for (size_t i = 0; i < matrix_[vertex_id].size(); ++i) {
    if (matrix_[vertex_id][i].has_value()) {
      ++deg;
    }
  }
  return deg;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::in_degree(VertexId vertex_id) const -> size_type {
  validate_vertex(vertex_id);

  size_type count = 0;
  for (size_t source = 0; source < vertices_.size(); ++source) {
    if (matrix_[source][vertex_id].has_value()) {
      ++count;
    }
  }
  return count;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::is_directed() const noexcept -> bool {
  return is_directed_;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::is_empty() const noexcept -> bool {
  return vertices_.is_empty();
}

//===----- CLEAR OPERATION -----------------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::clear() -> void {
  vertices_.clear();
  matrix_.clear();
  num_edges_ = 0;
}

//===----- TRAVERSAL ALGORITHMS ------------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::bfs(VertexId start_vertex) const -> std::vector<VertexId> {
  validate_vertex(start_vertex);

  std::vector<VertexId> result;
  result.reserve(vertices_.size());

  ads::arrays::DynamicArray<bool>           visited(vertices_.size(), false);
  ads::queues::CircularArrayQueue<VertexId> queue(vertices_.size());

  queue.enqueue(start_vertex);
  visited[start_vertex] = true;

  while (!queue.is_empty()) {
    const VertexId current = queue.front();
    queue.dequeue();
    result.push_back(current);

    for (size_t i = 0; i < matrix_[current].size(); ++i) {
      if (matrix_[current][i].has_value() && !visited[i]) {
        visited[i] = true;
        queue.enqueue(i);
      }
    }
  }

  return result;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::dfs(VertexId start_vertex) const -> std::vector<VertexId> {
  validate_vertex(start_vertex);

  std::vector<VertexId> result;
  result.reserve(vertices_.size());

  ads::arrays::DynamicArray<bool>   visited(vertices_.size(), false);
  ads::stacks::ArrayStack<VertexId> stack(vertices_.size());

  // Mark on push so the first discovered parent owns the DFS order, matching recursive traversal.
  visited[start_vertex] = true;
  stack.push(start_vertex);

  while (!stack.is_empty()) {
    const VertexId current = stack.top();
    stack.pop();
    result.push_back(current);

    const auto& row = matrix_[current];
    for (size_t i = row.size(); i > 0; --i) {
      const VertexId destination = i - 1;
      if (row[destination].has_value() && !visited[destination]) {
        visited[destination] = true;
        stack.push(destination);
      }
    }
  }

  return result;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::find_path(VertexId from, VertexId to) const
    -> std::optional<std::vector<VertexId>> {
  validate_vertex(from);
  validate_vertex(to);

  if (from == to) {
    return std::vector<VertexId>{from};
  }

  ads::arrays::DynamicArray<bool>           visited(vertices_.size(), false);
  ads::arrays::DynamicArray<VertexId>       parent(vertices_.size(), kNoParent);
  ads::queues::CircularArrayQueue<VertexId> queue(vertices_.size());

  queue.enqueue(from);
  visited[from] = true;

  while (!queue.is_empty()) {
    const VertexId current = queue.front();
    queue.dequeue();

    if (current == to) {
      std::vector<VertexId> path;
      VertexId              node = to;

      // Walk back using the parent vector to reconstruct the shortest path.
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
        queue.enqueue(i);
      }
    }
  }

  return std::nullopt;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::is_connected(VertexId v1, VertexId v2) const -> bool {
  return find_path(v1, v2).has_value();
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::connected_components() const -> std::vector<std::vector<VertexId>> {
  std::vector<std::vector<VertexId>> components;
  ads::arrays::DynamicArray<bool>    visited(vertices_.size(), false);

  for (size_t i = 0; i < vertices_.size(); ++i) {
    if (!visited[i]) {
      std::vector<VertexId>                     component;
      ads::queues::CircularArrayQueue<VertexId> queue(vertices_.size());

      queue.enqueue(i);
      visited[i] = true;

      while (!queue.is_empty()) {
        const VertexId current = queue.front();
        queue.dequeue();
        component.push_back(current);

        for (size_t j = 0; j < matrix_[current].size(); ++j) {
          if (matrix_[current][j].has_value() && !visited[j]) {
            visited[j] = true;
            queue.enqueue(j);
          }
        }
      }

      components.push_back(std::move(component));
    }
  }

  return components;
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::validate_vertex(VertexId vertex_id) const -> void {
  if (vertex_id >= vertices_.size()) {
    throw GraphMatrixException("Invalid vertex ID: " + std::to_string(vertex_id));
  }
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyMatrix<VertexData, EdgeWeight>::resize_matrix(size_type new_size) -> void {
  matrix_.resize(new_size);

  for (auto& row : matrix_) {
    row.resize(new_size);
  }
}

} // namespace ads::graphs

//===---------------------------------------------------------------------------===//
