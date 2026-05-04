//===---------------------------------------------------------------------------===//
/**
 * @file Graph_Adjacency_List.tpp
 * @author Costantino Lombardi
 * @brief Implementation of the GraphAdjacencyList class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/graphs/Graph_Adjacency_List.hpp"

namespace ads::graphs {

// DynamicArray stores vertices/adjacency without STL storage; CircularArrayQueue backs BFS worklists.

//===----------------------- CONSTRUCTORS AND ASSIGNMENT -----------------------===//

template <typename VertexData, typename EdgeWeight>
GraphAdjacencyList<VertexData, EdgeWeight>::GraphAdjacencyList(bool is_directed) :
    vertices_(),
    is_directed_(is_directed),
    num_edges_(0) {
}

template <typename VertexData, typename EdgeWeight>
GraphAdjacencyList<VertexData, EdgeWeight>::GraphAdjacencyList(size_t num_vertices, bool is_directed) :
    vertices_(),
    is_directed_(is_directed),
    num_edges_(0) {
  vertices_.reserve(num_vertices);
}

template <typename VertexData, typename EdgeWeight>
GraphAdjacencyList<VertexData, EdgeWeight>::GraphAdjacencyList(GraphAdjacencyList&& other) noexcept :
    vertices_(std::move(other.vertices_)),
    is_directed_(other.is_directed_),
    num_edges_(other.num_edges_) {
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

//===---------------------------- VERTEX OPERATIONS ----------------------------===//

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

//===----------------------------- EDGE OPERATIONS -----------------------------===//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::add_edge(size_t from, size_t to, EdgeWeight weight) -> void {
  validate_vertex(from);
  validate_vertex(to);

  // Check if edge already exists.
  if (!has_edge(from, to)) {
    vertices_[from].adjacency.emplace_back(to, weight);
    ++num_edges_;

    // For undirected graphs, add reverse edge.
    if (!is_directed_ && from != to) {
      vertices_[to].adjacency.emplace_back(from, weight);
      // Note: num_edges_ is only incremented once for undirected edges.
    }
  }
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::remove_edge(size_t from, size_t to) -> void {
  validate_vertex(from);
  validate_vertex(to);

  const auto edge_index = find_edge_index(from, to);

  if (edge_index.has_value()) {
    vertices_[from].adjacency.erase(*edge_index);
    --num_edges_;

    // For undirected graphs, remove reverse edge.
    if (!is_directed_ && from != to) {
      const auto reverse_edge_index = find_edge_index(to, from);
      if (reverse_edge_index.has_value()) {
        vertices_[to].adjacency.erase(*reverse_edge_index);
      }
    }
  }
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::has_edge(size_t from, size_t to) const -> bool {
  if (!has_vertex(from) || !has_vertex(to)) {
    return false;
  }

  return find_edge_index(from, to).has_value();
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_edge_weight(size_t from, size_t to) const
    -> std::optional<EdgeWeight> {
  if (!has_vertex(from) || !has_vertex(to)) {
    return std::nullopt;
  }

  const auto edge_index = find_edge_index(from, to);
  if (edge_index.has_value()) {
    return vertices_[from].adjacency[*edge_index].weight;
  }

  return std::nullopt;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::num_edges() const noexcept -> size_t {
  return num_edges_;
}

//===-------------------------- NAVIGATION OPERATIONS --------------------------===//

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

//===---------------------------- QUERY OPERATIONS -----------------------------===//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::is_directed() const noexcept -> bool {
  return is_directed_;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::is_empty() const noexcept -> bool {
  return vertices_.is_empty();
}

//===----------------------------- CLEAR OPERATION -----------------------------===//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::clear() -> void {
  vertices_.clear();
  num_edges_ = 0;
}

//===-------------------------- TRAVERSAL ALGORITHMS ---------------------------===//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::bfs(size_t start_vertex) const -> std::vector<size_t> {
  validate_vertex(start_vertex);

  std::vector<size_t> result;
  result.reserve(vertices_.size());

  ads::arrays::DynamicArray<bool>         visited(vertices_.size(), false);
  ads::queues::CircularArrayQueue<size_t> queue(vertices_.size());

  // Start BFS from start_vertex.
  queue.enqueue(start_vertex);
  visited[start_vertex] = true;

  while (!queue.is_empty()) {
    const size_t current = queue.front();
    queue.dequeue();
    result.push_back(current);

    // Visit all neighbors.
    for (const auto& edge : vertices_[current].adjacency) {
      if (!visited[edge.destination]) {
        visited[edge.destination] = true;
        queue.enqueue(edge.destination);
      }
    }
  }

  return result;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::dfs(size_t start_vertex) const -> std::vector<size_t> {
  validate_vertex(start_vertex);

  std::vector<size_t> result;
  result.reserve(vertices_.size());

  ads::arrays::DynamicArray<bool> visited(vertices_.size(), false);

  dfs_helper(start_vertex, visited, result);

  return result;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::find_path(size_t from, size_t to) const
    -> std::optional<std::vector<size_t>> {
  validate_vertex(from);
  validate_vertex(to);

  if (from == to) {
    return std::vector<size_t>{from};
  }

  ads::arrays::DynamicArray<bool>         visited(vertices_.size(), false);
  ads::arrays::DynamicArray<size_t>       parent(vertices_.size(), kNoParent);
  ads::queues::CircularArrayQueue<size_t> queue(vertices_.size());

  // BFS to find path.
  queue.enqueue(from);
  visited[from] = true;

  while (!queue.is_empty()) {
    const size_t current = queue.front();
    queue.dequeue();

    if (current == to) {
      // Reconstruct path.
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

    // Visit neighbors.
    for (const auto& edge : vertices_[current].adjacency) {
      if (!visited[edge.destination]) {
        visited[edge.destination] = true;
        parent[edge.destination]  = current;
        queue.enqueue(edge.destination);
      }
    }
  }

  // No path found.
  return std::nullopt;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::is_connected(size_t v1, size_t v2) const -> bool {
  return find_path(v1, v2).has_value();
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::connected_components() const -> std::vector<std::vector<size_t>> {
  std::vector<std::vector<size_t>> components;
  ads::arrays::DynamicArray<bool>  visited(vertices_.size(), false);

  for (size_t i = 0; i < vertices_.size(); ++i) {
    if (!visited[i]) {
      // Start a new component.
      std::vector<size_t>                     component;
      ads::queues::CircularArrayQueue<size_t> queue(vertices_.size());

      queue.enqueue(i);
      visited[i] = true;

      while (!queue.is_empty()) {
        const size_t current = queue.front();
        queue.dequeue();
        component.push_back(current);

        // Visit all neighbors.
        for (const auto& edge : vertices_[current].adjacency) {
          if (!visited[edge.destination]) {
            visited[edge.destination] = true;
            queue.enqueue(edge.destination);
          }
        }
      }

      components.push_back(std::move(component));
    }
  }

  return components;
}

//=================================================================================//
//===------------------------- PRIVATE HELPER METHODS --------------------------===//

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::validate_vertex(size_t vertex_id) const -> void {
  if (vertex_id >= vertices_.size()) {
    throw GraphException("Invalid vertex ID: " + std::to_string(vertex_id));
  }
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::find_edge_index(size_t from, size_t to) const
    -> std::optional<size_t> {
  const auto& adjacency = vertices_[from].adjacency;

  for (size_t index = 0; index < adjacency.size(); ++index) {
    if (adjacency[index].destination == to) {
      return index;
    }
  }

  return std::nullopt;
}

template <typename VertexData, typename EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::dfs_helper(
    size_t vertex_id, ads::arrays::DynamicArray<bool>& visited, std::vector<size_t>& result) const -> void {
  visited[vertex_id] = true;
  result.push_back(vertex_id);

  // Visit all unvisited neighbors.
  for (const auto& edge : vertices_[vertex_id].adjacency) {
    if (!visited[edge.destination]) {
      dfs_helper(edge.destination, visited, result);
    }
  }
}

} // namespace ads::graphs

//===---------------------------------------------------------------------------===//
