//===--------------------------------------------------------------------------===//
/**
 * @file Graph_Adjacency_List.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the GraphAdjacencyList class
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#include "../../../include/ads/graphs/Graph_Adjacency_List.hpp"
#include <algorithm>

namespace ads::graph {

//========== CONSTRUCTORS AND ASSIGNMENT ==========//

template <typename VertexData, typename EdgeWeight>
GraphAdjacencyList<VertexData, EdgeWeight>::GraphAdjacencyList(bool is_directed) : vertices_(), is_directed_(is_directed), num_edges_(0) {
}

template <typename VertexData, typename EdgeWeight>
GraphAdjacencyList<VertexData, EdgeWeight>::GraphAdjacencyList(size_t num_vertices, bool is_directed) :
    vertices_(), is_directed_(is_directed), num_edges_(0) {
  vertices_.reserve(num_vertices);
}

template <typename VertexData, typename EdgeWeight>
GraphAdjacencyList<VertexData, EdgeWeight>::GraphAdjacencyList(GraphAdjacencyList&& other) noexcept :
    vertices_(std::move(other.vertices_)), is_directed_(other.is_directed_), num_edges_(other.num_edges_) {
  other.num_edges_ = 0;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::operator=(GraphAdjacencyList&& other) noexcept -> GraphAdjacencyList& {
  if (this != &other) {
    vertices_    = std::move(other.vertices_);
    is_directed_ = other.is_directed_;
    num_edges_   = other.num_edges_;

    other.num_edges_ = 0;
  }
  return *this;
}

//========== VERTEX OPERATIONS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::add_vertex(const VertexData& data) -> size_t {
  vertices_.emplace_back(data);
  return vertices_.size() - 1;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::add_vertex(VertexData&& data) -> size_t {
  vertices_.emplace_back(std::move(data));
  return vertices_.size() - 1;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_vertex_data(size_t vertex_id) -> VertexData& {
  validate_vertex(vertex_id);
  return vertices_[vertex_id].data;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_vertex_data(size_t vertex_id) const -> const VertexData& {
  validate_vertex(vertex_id);
  return vertices_[vertex_id].data;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::has_vertex(size_t vertex_id) const noexcept -> bool {
  return vertex_id < vertices_.size();
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::num_vertices() const noexcept -> size_t {
  return vertices_.size();
}

//========== EDGE OPERATIONS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::add_edge(size_t from, size_t to, EdgeWeight weight) -> void {
  validate_vertex(from);
  validate_vertex(to);

  // Check if edge already exists
  if (!has_edge(from, to)) {
    vertices_[from].adjacency.emplace_back(to, weight);
    ++num_edges_;

    // For undirected graphs, add reverse edge
    if (!is_directed_ && from != to) {
      vertices_[to].adjacency.emplace_back(from, weight);
      // Note: num_edges_ is only incremented once for undirected edges
    }
  }
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::remove_edge(size_t from, size_t to) -> void {
  validate_vertex(from);
  validate_vertex(to);

  // Remove edge from -> to
  auto& adj = vertices_[from].adjacency;
  auto  it  = std::find_if(adj.begin(), adj.end(), [to](const Edge& e) -> auto { return e.destination == to; });

  if (it != adj.end()) {
    adj.erase(it);
    --num_edges_;

    // For undirected graphs, remove reverse edge
    if (!is_directed_ && from != to) {
      auto& adj_reverse = vertices_[to].adjacency;
      auto  it_reverse  = std::find_if(adj_reverse.begin(), adj_reverse.end(), [from](const Edge& e) -> auto { return e.destination == from; });

      if (it_reverse != adj_reverse.end()) {
        adj_reverse.erase(it_reverse);
      }
    }
  }
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::has_edge(size_t from, size_t to) const -> bool {
  if (!has_vertex(from) || !has_vertex(to)) {
    return false;
  }

  const auto& adj = vertices_[from].adjacency;
  return std::any_of(adj.begin(), adj.end(), [to](const Edge& e) -> auto { return e.destination == to; });
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_edge_weight(size_t from, size_t to) const -> std::optional<EdgeWeight> {
  if (!has_vertex(from) || !has_vertex(to)) {
    return std::nullopt;
  }

  const auto& adj = vertices_[from].adjacency;
  auto        it  = std::find_if(adj.begin(), adj.end(), [to](const Edge& e) -> auto { return e.destination == to; });

  if (it != adj.end()) {
    return it->weight;
  }

  return std::nullopt;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::num_edges() const noexcept -> size_t {
  return num_edges_;
}

//========== NAVIGATION OPERATIONS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_neighbors(size_t vertex_id) const -> std::vector<size_t> {
  validate_vertex(vertex_id);

  std::vector<size_t> neighbors;
  neighbors.reserve(vertices_[vertex_id].adjacency.size());

  for (const auto& edge : vertices_[vertex_id].adjacency) {
    neighbors.push_back(edge.destination);
  }

  return neighbors;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_neighbors_with_weights(size_t vertex_id) const
    -> std::vector<std::pair<size_t, EdgeWeight>> {
  validate_vertex(vertex_id);

  std::vector<std::pair<size_t, EdgeWeight>> neighbors;
  neighbors.reserve(vertices_[vertex_id].adjacency.size());

  for (const auto& edge : vertices_[vertex_id].adjacency) {
    neighbors.emplace_back(edge.destination, edge.weight);
  }

  return neighbors;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::degree(size_t vertex_id) const -> size_t {
  validate_vertex(vertex_id);
  return vertices_[vertex_id].adjacency.size();
}

//========== QUERY OPERATIONS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::is_directed() const noexcept -> bool {
  return is_directed_;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::is_empty() const noexcept -> bool {
  return vertices_.empty();
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::clear() -> void {
  vertices_.clear();
  num_edges_ = 0;
}

//========== TRAVERSAL ALGORITHMS ==========//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::bfs(size_t start_vertex) const -> std::vector<size_t> {
  validate_vertex(start_vertex);

  std::vector<size_t> result;
  std::vector<bool>   visited(vertices_.size(), false);
  std::queue<size_t>  queue;

  // Start BFS from start_vertex
  queue.push(start_vertex);
  visited[start_vertex] = true;

  while (!queue.empty()) {
    size_t current = queue.front();
    queue.pop();
    result.push_back(current);

    // Visit all neighbors
    for (const auto& edge : vertices_[current].adjacency) {
      if (!visited[edge.destination]) {
        visited[edge.destination] = true;
        queue.push(edge.destination);
      }
    }
  }

  return result;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::dfs(size_t start_vertex) const -> std::vector<size_t> {
  validate_vertex(start_vertex);

  std::vector<size_t> result;
  std::vector<bool>   visited(vertices_.size(), false);

  dfs_helper(start_vertex, visited, result);

  return result;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::find_path(size_t from, size_t to) const -> std::optional<std::vector<size_t>> {
  validate_vertex(from);
  validate_vertex(to);

  if (from == to) {
    return std::vector<size_t>{from};
  }

  std::vector<bool>   visited(vertices_.size(), false);
  std::vector<size_t> parent(vertices_.size(), SIZE_MAX);
  std::queue<size_t>  queue;

  // BFS to find path
  queue.push(from);
  visited[from] = true;

  while (!queue.empty()) {
    size_t current = queue.front();
    queue.pop();

    if (current == to) {
      // Reconstruct path
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

    // Visit neighbors
    for (const auto& edge : vertices_[current].adjacency) {
      if (!visited[edge.destination]) {
        visited[edge.destination] = true;
        parent[edge.destination]  = current;
        queue.push(edge.destination);
      }
    }
  }

  // No path found
  return std::nullopt;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::is_connected(size_t v1, size_t v2) const -> bool {
  return find_path(v1, v2).has_value();
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::connected_components() const -> std::vector<std::vector<size_t>> {
  std::vector<std::vector<size_t>> components;
  std::vector<bool>                visited(vertices_.size(), false);

  for (size_t i = 0; i < vertices_.size(); ++i) {
    if (!visited[i]) {
      // Start a new component
      std::vector<size_t> component;
      std::queue<size_t>  queue;

      queue.push(i);
      visited[i] = true;

      while (!queue.empty()) {
        size_t current = queue.front();
        queue.pop();
        component.push_back(current);

        // Visit all neighbors
        for (const auto& edge : vertices_[current].adjacency) {
          if (!visited[edge.destination]) {
            visited[edge.destination] = true;
            queue.push(edge.destination);
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
auto GraphAdjacencyList<VertexData, EdgeWeight>::validate_vertex(size_t vertex_id) const -> void {
  if (vertex_id >= vertices_.size()) {
    throw GraphException("Invalid vertex ID: " + std::to_string(vertex_id));
  }
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::dfs_helper(size_t vertex_id, std::vector<bool>& visited, std::vector<size_t>& result) const
    -> void {
  visited[vertex_id] = true;
  result.push_back(vertex_id);

  // Visit all unvisited neighbors
  for (const auto& edge : vertices_[vertex_id].adjacency) {
    if (!visited[edge.destination]) {
      dfs_helper(edge.destination, visited, result);
    }
  }
}

} // namespace ads::graph
//===--------------------------------------------------------------------------===//
