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

//===----- CONSTRUCTORS AND ASSIGNMENT -----------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
GraphAdjacencyList<VertexData, EdgeWeight>::GraphAdjacencyList(bool is_directed) :
    vertices_(),
    is_directed_(is_directed),
    num_edges_(0) {
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
GraphAdjacencyList<VertexData, EdgeWeight>::GraphAdjacencyList(size_type num_vertices, bool is_directed) :
    vertices_(),
    is_directed_(is_directed),
    num_edges_(0) {
  vertices_.reserve(num_vertices);
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
GraphAdjacencyList<VertexData, EdgeWeight>::GraphAdjacencyList(GraphAdjacencyList&& other) noexcept :
    vertices_(std::move(other.vertices_)),
    is_directed_(other.is_directed_),
    num_edges_(other.num_edges_) {
  other.num_edges_ = 0;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::operator=(GraphAdjacencyList&& other) noexcept -> GraphAdjacencyList& {
  if (this != &other) {
    vertices_    = std::move(other.vertices_);
    is_directed_ = other.is_directed_;
    num_edges_   = other.num_edges_;

    other.num_edges_ = 0;
  }
  return *this;
}

//===----- VERTEX OPERATIONS ---------------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::add_vertex(const VertexData& data) -> VertexId {
  vertices_.emplace_back(data);
  return vertices_.size() - 1;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::add_vertex(VertexData&& data) -> VertexId {
  vertices_.emplace_back(std::move(data));
  return vertices_.size() - 1;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_vertex_data(VertexId vertex_id) -> VertexData& {
  validate_vertex(vertex_id);
  return vertices_[vertex_id].data;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_vertex_data(VertexId vertex_id) const -> const VertexData& {
  validate_vertex(vertex_id);
  return vertices_[vertex_id].data;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::has_vertex(VertexId vertex_id) const noexcept -> bool {
  return vertex_id < vertices_.size();
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::num_vertices() const noexcept -> size_type {
  return vertices_.size();
}

//===----- EDGE OPERATIONS -----------------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::add_edge(VertexId from, VertexId to, EdgeWeight weight) -> void {
  validate_vertex(from);
  validate_vertex(to);

  // An existing edge keeps its slot but adopts the new weight (insert-or-update).
  if (const auto edge_index = find_edge_index(from, to)) {
    vertices_[from].adjacency[*edge_index].weight = weight;

    if (!is_directed_ && from != to) {
      if (const auto reverse_index = find_edge_index(to, from)) {
        vertices_[to].adjacency[*reverse_index].weight = weight;
      }
    }
    return;
  }

  vertices_[from].adjacency.emplace_back(to, weight);
  ++num_edges_;

  // For undirected graphs, add reverse edge.
  if (!is_directed_ && from != to) {
    vertices_[to].adjacency.emplace_back(from, weight);
    // Note: num_edges_ is only incremented once for undirected edges.
  }
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::remove_edge(VertexId from, VertexId to) -> void {
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

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::has_edge(VertexId from, VertexId to) const -> bool {
  if (!has_vertex(from) || !has_vertex(to)) {
    return false;
  }

  return find_edge_index(from, to).has_value();
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_edge_weight(VertexId from, VertexId to) const
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

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::num_edges() const noexcept -> size_type {
  return num_edges_;
}

//===----- NAVIGATION OPERATIONS -----------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_neighbors(VertexId vertex_id) const -> std::vector<VertexId> {
  validate_vertex(vertex_id);

  std::vector<VertexId> neighbors;
  neighbors.reserve(vertices_[vertex_id].adjacency.size());

  for (const auto& edge : vertices_[vertex_id].adjacency) {
    neighbors.push_back(edge.destination);
  }

  return neighbors;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::get_neighbors_with_weights(VertexId vertex_id) const
    -> std::vector<std::pair<VertexId, EdgeWeight>> {
  validate_vertex(vertex_id);

  std::vector<std::pair<VertexId, EdgeWeight>> neighbors;
  neighbors.reserve(vertices_[vertex_id].adjacency.size());

  for (const auto& edge : vertices_[vertex_id].adjacency) {
    neighbors.emplace_back(edge.destination, edge.weight);
  }

  return neighbors;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
template <typename Visitor>
auto GraphAdjacencyList<VertexData, EdgeWeight>::for_each_weighted_neighbor(VertexId vertex_id, Visitor&& visitor) const
    -> void
  requires WeightedNeighborVisitor<Visitor, EdgeWeight>
{
  validate_vertex(vertex_id);

  for (const auto& edge : vertices_[vertex_id].adjacency) {
    std::forward<Visitor>(visitor)(edge.destination, edge.weight);
  }
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::degree(VertexId vertex_id) const -> size_type {
  validate_vertex(vertex_id);
  return vertices_[vertex_id].adjacency.size();
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::in_degree(VertexId vertex_id) const -> size_type {
  validate_vertex(vertex_id);

  size_type count = 0;
  for (size_t source = 0; source < vertices_.size(); ++source) {
    for (const auto& edge : vertices_[source].adjacency) {
      if (edge.destination == vertex_id) {
        ++count;
      }
    }
  }
  return count;
}

//===----- QUERY OPERATIONS ----------------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::is_directed() const noexcept -> bool {
  return is_directed_;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::is_empty() const noexcept -> bool {
  return vertices_.is_empty();
}

//===----- CLEAR OPERATION -----------------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::clear() -> void {
  vertices_.clear();
  num_edges_ = 0;
}

//===----- TRAVERSAL ALGORITHMS ------------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::bfs(VertexId start_vertex) const -> std::vector<VertexId> {
  validate_vertex(start_vertex);

  std::vector<VertexId> result;
  result.reserve(vertices_.size());

  ads::arrays::DynamicArray<bool>           visited(vertices_.size(), false);
  ads::queues::CircularArrayQueue<VertexId> queue(vertices_.size());

  // Start BFS from start_vertex.
  queue.enqueue(start_vertex);
  visited[start_vertex] = true;

  while (!queue.is_empty()) {
    const VertexId current = queue.front();
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

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::dfs(VertexId start_vertex) const -> std::vector<VertexId> {
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

    const auto& adjacency = vertices_[current].adjacency;
    for (size_t i = adjacency.size(); i > 0; --i) {
      const VertexId destination = adjacency[i - 1].destination;
      if (!visited[destination]) {
        visited[destination] = true;
        stack.push(destination);
      }
    }
  }

  return result;
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::find_path(VertexId from, VertexId to) const
    -> std::optional<std::vector<VertexId>> {
  validate_vertex(from);
  validate_vertex(to);

  if (from == to) {
    return std::vector<VertexId>{from};
  }

  ads::arrays::DynamicArray<bool>           visited(vertices_.size(), false);
  ads::arrays::DynamicArray<VertexId>       parent(vertices_.size(), kNoParent);
  ads::queues::CircularArrayQueue<VertexId> queue(vertices_.size());

  // BFS to find path.
  queue.enqueue(from);
  visited[from] = true;

  while (!queue.is_empty()) {
    const VertexId current = queue.front();
    queue.dequeue();

    if (current == to) {
      // Reconstruct path.
      std::vector<VertexId> path;
      VertexId              node = to;

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

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::is_connected(VertexId v1, VertexId v2) const -> bool {
  return find_path(v1, v2).has_value();
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::connected_components() const -> std::vector<std::vector<VertexId>> {
  std::vector<std::vector<VertexId>> components;
  ads::arrays::DynamicArray<bool>    visited(vertices_.size(), false);

  for (size_t i = 0; i < vertices_.size(); ++i) {
    if (!visited[i]) {
      // Start a new component.
      std::vector<VertexId>                     component;
      ads::queues::CircularArrayQueue<VertexId> queue(vertices_.size());

      queue.enqueue(i);
      visited[i] = true;

      while (!queue.is_empty()) {
        const VertexId current = queue.front();
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
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::validate_vertex(VertexId vertex_id) const -> void {
  if (vertex_id >= vertices_.size()) {
    throw GraphException("Invalid vertex ID: " + std::to_string(vertex_id));
  }
}

template <VertexPayload VertexData, EdgeWeightValue EdgeWeight>
auto GraphAdjacencyList<VertexData, EdgeWeight>::find_edge_index(VertexId from, VertexId to) const
    -> std::optional<size_t> {
  const auto& adjacency = vertices_[from].adjacency;

  for (size_t index = 0; index < adjacency.size(); ++index) {
    if (adjacency[index].destination == to) {
      return index;
    }
  }

  return std::nullopt;
}

} // namespace ads::graphs

//===---------------------------------------------------------------------------===//
