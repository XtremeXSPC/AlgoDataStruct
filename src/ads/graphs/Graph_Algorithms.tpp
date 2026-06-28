//===---------------------------------------------------------------------------===//
/**
 * @file Graph_Algorithms.tpp
 * @author Costantino Lombardi
 * @brief Implementation of reusable graph algorithms.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#pragma once
#include "../../../include/ads/graphs/Graph_Algorithms.hpp"

#include <algorithm>

namespace ads::graphs {

//===----- INTERNAL UTILITIES --------------------------------------------------===//

namespace graph_algorithm_detail {

template <PathWeight EdgeWeight>
struct DijkstraQueueCompare {
  auto operator()(const std::pair<size_t, EdgeWeight>& lhs, const std::pair<size_t, EdgeWeight>& rhs) const -> bool {
    return lhs.second > rhs.second;
  }
};

template <PathWeight EdgeWeight>
struct ForestEdgeLess {
  auto operator()(
      const typename MinimumSpanningForestResult<EdgeWeight>::Edge& lhs,
      const typename MinimumSpanningForestResult<EdgeWeight>::Edge& rhs) const -> bool {
    if (lhs.weight != rhs.weight) {
      return lhs.weight < rhs.weight;
    }
    if (lhs.from != rhs.from) {
      return lhs.from < rhs.from;
    }
    return lhs.to < rhs.to;
  }
};

template <PathWeight EdgeWeight>
auto add_with_saturation(const EdgeWeight& lhs, const EdgeWeight& rhs, const EdgeWeight& limit) -> EdgeWeight {
  if constexpr (FloatingPathWeight<EdgeWeight>) {
    return lhs + rhs;
  } else {
    if (lhs >= limit || rhs >= limit || lhs > limit - rhs) {
      return limit;
    }
    return static_cast<EdgeWeight>(lhs + rhs);
  }
}

template <PathWeight EdgeWeight>
auto add_with_negative_support(const EdgeWeight& lhs, const EdgeWeight& rhs, const EdgeWeight& unreachable) -> EdgeWeight {
  if (lhs == unreachable) {
    return unreachable;
  }

  if constexpr (FloatingPathWeight<EdgeWeight>) {
    return lhs + rhs;
  } else if constexpr (SignedIntegralPathWeight<EdgeWeight>) {
    if (rhs > 0 && lhs > std::numeric_limits<EdgeWeight>::max() - rhs) {
      return std::numeric_limits<EdgeWeight>::max();
    }
    if (rhs < 0 && lhs < std::numeric_limits<EdgeWeight>::lowest() - rhs) {
      return std::numeric_limits<EdgeWeight>::lowest();
    }
    return static_cast<EdgeWeight>(lhs + rhs);
  } else {
    return add_with_saturation(lhs, rhs, unreachable);
  }
}

} // namespace graph_algorithm_detail

//===----- RESULT TYPE METHODS -------------------------------------------------===//

template <PathWeight EdgeWeight>
ShortestPathsResult<EdgeWeight>::ShortestPathsResult(
    VertexId source, DynamicArray<EdgeWeight>&& distances, DynamicArray<VertexId>&& predecessors) :
    source_(source),
    distances_(std::move(distances)),
    predecessors_(std::move(predecessors)) {
}

template <PathWeight EdgeWeight>
auto ShortestPathsResult<EdgeWeight>::source() const noexcept -> VertexId {
  return source_;
}

template <PathWeight EdgeWeight>
auto ShortestPathsResult<EdgeWeight>::vertex_count() const noexcept -> size_type {
  return distances_.size();
}

template <PathWeight EdgeWeight>
auto ShortestPathsResult<EdgeWeight>::distances() const noexcept -> const DynamicArray<EdgeWeight>& {
  return distances_;
}

template <PathWeight EdgeWeight>
auto ShortestPathsResult<EdgeWeight>::predecessor_of(VertexId vertex_id) const -> VertexId {
  validate_vertex(vertex_id);
  return predecessors_[vertex_id];
}

template <PathWeight EdgeWeight>
auto ShortestPathsResult<EdgeWeight>::distance_to(VertexId vertex_id) const -> const EdgeWeight& {
  validate_vertex(vertex_id);
  return distances_[vertex_id];
}

template <PathWeight EdgeWeight>
auto ShortestPathsResult<EdgeWeight>::has_path_to(VertexId vertex_id) const -> bool {
  validate_vertex(vertex_id);
  return vertex_id == source_ || predecessors_[vertex_id] != kNoPredecessor;
}

template <PathWeight EdgeWeight>
auto ShortestPathsResult<EdgeWeight>::path_to(VertexId vertex_id) const -> std::optional<DynamicArray<VertexId>> {
  validate_vertex(vertex_id);

  if (!has_path_to(vertex_id)) {
    return std::nullopt;
  }

  DynamicArray<VertexId> reversed_path;
  for (VertexId current = vertex_id;; current = predecessors_[current]) {
    reversed_path.push_back(current);
    if (current == source_) {
      break;
    }
  }

  DynamicArray<VertexId> path;
  path.reserve(reversed_path.size());
  for (size_t index = reversed_path.size(); index > 0; --index) {
    path.push_back(reversed_path[index - 1]);
  }
  return path;
}

template <PathWeight EdgeWeight>
auto ShortestPathsResult<EdgeWeight>::unreachable_distance() noexcept -> EdgeWeight {
  if constexpr (FloatingPathWeight<EdgeWeight>) {
    return std::numeric_limits<EdgeWeight>::infinity();
  } else {
    return std::numeric_limits<EdgeWeight>::max();
  }
}

template <PathWeight EdgeWeight>
auto ShortestPathsResult<EdgeWeight>::validate_vertex(VertexId vertex_id) const -> void {
  if (vertex_id >= distances_.size()) {
    throw GraphAlgorithmException("Invalid vertex ID in shortest path result");
  }
}

template <PathWeight EdgeWeight>
MinimumSpanningForestResult<EdgeWeight>::MinimumSpanningForestResult(
    EdgeWeight total_weight, size_type component_count, DynamicArray<Edge>&& edges) :
    total_weight_(std::move(total_weight)),
    component_count_(component_count),
    edges_(std::move(edges)) {
}

template <PathWeight EdgeWeight>
auto MinimumSpanningForestResult<EdgeWeight>::edges() const noexcept -> const DynamicArray<Edge>& {
  return edges_;
}

template <PathWeight EdgeWeight>
auto MinimumSpanningForestResult<EdgeWeight>::total_weight() const noexcept -> const EdgeWeight& {
  return total_weight_;
}

template <PathWeight EdgeWeight>
auto MinimumSpanningForestResult<EdgeWeight>::component_count() const noexcept -> size_type {
  return component_count_;
}

template <PathWeight EdgeWeight>
auto MinimumSpanningForestResult<EdgeWeight>::edge_count() const noexcept -> size_type {
  return edges_.size();
}

template <PathWeight EdgeWeight>
auto MinimumSpanningForestResult<EdgeWeight>::spans_all_vertices(size_type vertex_count) const noexcept -> bool {
  return vertex_count <= 1 || (component_count_ == 1 && edges_.size() + 1 == vertex_count);
}

template <PathWeight EdgeWeight>
AllPairsResult<EdgeWeight>::AllPairsResult(
    DynamicArray<DynamicArray<EdgeWeight>>&& distances, DynamicArray<DynamicArray<VertexId>>&& next_vertices) :
    distances_(std::move(distances)),
    next_vertices_(std::move(next_vertices)) {
}

template <PathWeight EdgeWeight>
auto AllPairsResult<EdgeWeight>::vertex_count() const noexcept -> size_type {
  return distances_.size();
}

template <PathWeight EdgeWeight>
auto AllPairsResult<EdgeWeight>::distances() const noexcept -> const DynamicArray<DynamicArray<EdgeWeight>>& {
  return distances_;
}

template <PathWeight EdgeWeight>
auto AllPairsResult<EdgeWeight>::distance(VertexId source, VertexId target) const -> const EdgeWeight& {
  validate_vertex(source);
  validate_vertex(target);
  return distances_[source][target];
}

template <PathWeight EdgeWeight>
auto AllPairsResult<EdgeWeight>::has_path(VertexId source, VertexId target) const -> bool {
  validate_vertex(source);
  validate_vertex(target);
  return source == target || next_vertices_[source][target] != kNoNextVertex;
}

template <PathWeight EdgeWeight>
auto AllPairsResult<EdgeWeight>::path(VertexId source, VertexId target) const -> std::optional<DynamicArray<VertexId>> {
  validate_vertex(source);
  validate_vertex(target);

  if (!has_path(source, target)) {
    return std::nullopt;
  }

  DynamicArray<VertexId> vertices;
  vertices.push_back(source);

  VertexId current = source;
  while (current != target) {
    current = next_vertices_[current][target];
    if (current == kNoNextVertex) {
      throw GraphAlgorithmException("Broken next-hop matrix in Floyd-Warshall result");
    }
    vertices.push_back(current);
  }

  return vertices;
}

template <PathWeight EdgeWeight>
auto AllPairsResult<EdgeWeight>::unreachable_distance() noexcept -> EdgeWeight {
  if constexpr (FloatingPathWeight<EdgeWeight>) {
    return std::numeric_limits<EdgeWeight>::infinity();
  } else {
    return std::numeric_limits<EdgeWeight>::max();
  }
}

template <PathWeight EdgeWeight>
auto AllPairsResult<EdgeWeight>::validate_vertex(VertexId vertex_id) const -> void {
  if (vertex_id >= distances_.size()) {
    throw GraphAlgorithmException("Invalid vertex ID in all-pairs shortest path result");
  }
}

SccResult::SccResult(DynamicArray<DynamicArray<VertexId>>&& components, DynamicArray<size_type>&& component_ids) :
    components_(std::move(components)),
    component_ids_(std::move(component_ids)) {
}

auto SccResult::vertex_count() const noexcept -> size_type {
  return component_ids_.size();
}

auto SccResult::component_count() const noexcept -> size_type {
  return components_.size();
}

auto SccResult::components() const noexcept -> const DynamicArray<DynamicArray<VertexId>>& {
  return components_;
}

auto SccResult::component(size_type component_id) const -> const DynamicArray<VertexId>& {
  validate_component(component_id);
  return components_[component_id];
}

auto SccResult::component_id_of(VertexId vertex_id) const -> size_type {
  validate_vertex(vertex_id);
  return component_ids_[vertex_id];
}

auto SccResult::are_strongly_connected(VertexId lhs, VertexId rhs) const -> bool {
  validate_vertex(lhs);
  validate_vertex(rhs);
  return component_ids_[lhs] == component_ids_[rhs];
}

auto SccResult::validate_vertex(VertexId vertex_id) const -> void {
  if (vertex_id >= component_ids_.size()) {
    throw GraphAlgorithmException("Invalid vertex ID in SCC result");
  }
}

auto SccResult::validate_component(size_type component_id) const -> void {
  if (component_id >= components_.size()) {
    throw GraphAlgorithmException("Invalid component ID in SCC result");
  }
}

//=================================================================================//
//===----- PRIVATE HELPER METHODS ----------------------------------------------===//

template <WeightedGraph Graph>
auto collect_weighted_edges_once(const Graph& graph)
    -> DynamicArray<typename MinimumSpanningForestResult<typename Graph::EdgeWeight>::Edge> {
  using EdgeWeight = typename Graph::EdgeWeight;
  using VertexId   = typename Graph::VertexId;
  using ForestEdge = typename MinimumSpanningForestResult<EdgeWeight>::Edge;

  DynamicArray<ForestEdge> edges;
  edges.reserve(graph.num_edges());

  for (VertexId from = 0; from < graph.num_vertices(); ++from) {
    graph.for_each_weighted_neighbor(from, [&](VertexId to, const EdgeWeight& weight) {
      if (graph.is_directed() || from < to) {
        edges.push_back({from, to, weight});
      }
    });
  }

  return edges;
}

template <TraversableGraph Graph>
auto build_unweighted_adjacency(const Graph& graph) -> DynamicArray<DynamicArray<size_t>> {
  using VertexId = typename Graph::VertexId;
  DynamicArray<DynamicArray<VertexId>> adjacency;
  adjacency.reserve(graph.num_vertices());

  for (VertexId vertex_id = 0; vertex_id < graph.num_vertices(); ++vertex_id) {
    adjacency.push_back(DynamicArray<VertexId>{});
  }

  for (VertexId from = 0; from < graph.num_vertices(); ++from) {
    graph.for_each_weighted_neighbor(from, [&](VertexId to, const auto& weight) {
      static_cast<void>(weight);
      adjacency[from].push_back(to);
    });
  }

  return adjacency;
}

auto transpose_unweighted_adjacency(const DynamicArray<DynamicArray<size_t>>& adjacency) -> DynamicArray<DynamicArray<size_t>> {
  using VertexId = std::size_t;
  DynamicArray<DynamicArray<VertexId>> transpose;
  transpose.reserve(adjacency.size());

  for (VertexId vertex_id = 0; vertex_id < adjacency.size(); ++vertex_id) {
    transpose.push_back(DynamicArray<VertexId>{});
  }

  for (VertexId from = 0; from < adjacency.size(); ++from) {
    for (VertexId to : adjacency[from]) {
      transpose[to].push_back(from);
    }
  }

  return transpose;
}

auto compute_finish_order(const DynamicArray<DynamicArray<size_t>>& adjacency) -> DynamicArray<size_t> {
  using VertexId  = std::size_t;
  using size_type = std::size_t;

  struct DfsFrame {
    VertexId vertex;
    size_t   next_neighbor_index;
  };

  const size_type        vertex_count = adjacency.size();
  DynamicArray<bool>     visited(vertex_count, false);
  DynamicArray<VertexId> finish_order;
  finish_order.reserve(vertex_count);

  for (size_t start = 0; start < vertex_count; ++start) {
    if (visited[start]) {
      continue;
    }

    DynamicArray<DfsFrame> stack;
    stack.push_back({start, 0});
    visited[start] = true;

    while (!stack.is_empty()) {
      DfsFrame& frame = stack.back();

      if (frame.next_neighbor_index < adjacency[frame.vertex].size()) {
        const VertexId neighbor = adjacency[frame.vertex][frame.next_neighbor_index++];
        if (!visited[neighbor]) {
          visited[neighbor] = true;
          stack.push_back({neighbor, 0});
        }
        continue;
      }

      finish_order.push_back(frame.vertex);
      stack.pop_back();
    }
  }

  return finish_order;
}

auto collect_component_from_transpose(const DynamicArray<DynamicArray<size_t>>& transpose, size_t start, DynamicArray<bool>& visited)
    -> DynamicArray<size_t> {
  using VertexId = std::size_t;
  DynamicArray<VertexId> component;
  DynamicArray<VertexId> stack;

  stack.push_back(start);
  visited[start] = true;

  while (!stack.is_empty()) {
    const VertexId vertex_id = stack.back();
    stack.pop_back();
    component.push_back(vertex_id);

    for (VertexId neighbor : transpose[vertex_id]) {
      if (!visited[neighbor]) {
        visited[neighbor] = true;
        stack.push_back(neighbor);
      }
    }
  }

  return component;
}

//===----- ALGORITHM IMPLEMENTATIONS -------------------------------------------===//

template <WeightedGraph Graph>
auto dijkstra(const Graph& graph, size_t source) -> ShortestPathsResult<typename Graph::EdgeWeight> {
  using EdgeWeight = typename Graph::EdgeWeight;
  using VertexId   = typename Graph::VertexId;
  using size_type  = std::size_t;
  using QueueEntry = std::pair<size_t, EdgeWeight>;

  const size_type vertex_count = graph.num_vertices();
  if (source >= vertex_count) {
    throw GraphAlgorithmException("Dijkstra source vertex is out of range");
  }

  const EdgeWeight unreachable = ShortestPathsResult<EdgeWeight>::unreachable_distance();

  DynamicArray<EdgeWeight> distances(vertex_count, unreachable);
  DynamicArray<VertexId>   predecessors(vertex_count, ShortestPathsResult<EdgeWeight>::kNoPredecessor);
  DynamicArray<bool>       visited(vertex_count, false);
  ads::queues::PriorityQueue<QueueEntry, graph_algorithm_detail::DijkstraQueueCompare<EdgeWeight>> frontier;

  distances[source] = EdgeWeight{};
  frontier.push({source, EdgeWeight{}});

  while (!frontier.is_empty()) {
    auto [vertex_id, best_distance] = frontier.top();
    frontier.pop();

    if (visited[vertex_id]) {
      continue;
    }
    visited[vertex_id] = true;

    if (best_distance > distances[vertex_id]) {
      continue;
    }

    graph.for_each_weighted_neighbor(vertex_id, [&](VertexId neighbor_id, const EdgeWeight& edge_weight) {
      if (edge_weight < EdgeWeight{}) {
        throw GraphAlgorithmException("Dijkstra does not support negative edge weights");
      }

      const EdgeWeight candidate = graph_algorithm_detail::add_with_saturation(distances[vertex_id], edge_weight, unreachable);
      if (candidate < distances[neighbor_id]) {
        distances[neighbor_id]    = candidate;
        predecessors[neighbor_id] = vertex_id;
        frontier.push({neighbor_id, candidate});
      }
    });
  }

  return ShortestPathsResult<EdgeWeight>(source, std::move(distances), std::move(predecessors));
}

template <WeightedGraph Graph>
auto bellman_ford(const Graph& graph, size_t source) -> ShortestPathsResult<typename Graph::EdgeWeight> {
  using EdgeWeight = typename Graph::EdgeWeight;
  using VertexId   = typename Graph::VertexId;
  using size_type  = std::size_t;

  const size_type vertex_count = graph.num_vertices();
  if (source >= vertex_count) {
    throw GraphAlgorithmException("Bellman-Ford source vertex is out of range");
  }

  const EdgeWeight unreachable = ShortestPathsResult<EdgeWeight>::unreachable_distance();

  DynamicArray<EdgeWeight> distances(vertex_count, unreachable);
  DynamicArray<VertexId>   predecessors(vertex_count, ShortestPathsResult<EdgeWeight>::kNoPredecessor);

  distances[source] = EdgeWeight{};

  for (size_t pass = 1; pass < vertex_count; ++pass) {
    bool changed = false;

    for (VertexId from = 0; from < vertex_count; ++from) {
      if (distances[from] == unreachable) {
        continue;
      }

      graph.for_each_weighted_neighbor(from, [&](VertexId to, const EdgeWeight& weight) {
        const EdgeWeight candidate = graph_algorithm_detail::add_with_negative_support(distances[from], weight, unreachable);
        if (candidate < distances[to]) {
          distances[to]    = candidate;
          predecessors[to] = from;
          changed          = true;
        }
      });
    }

    if (!changed) {
      break;
    }
  }

  for (VertexId from = 0; from < vertex_count; ++from) {
    if (distances[from] == unreachable) {
      continue;
    }

    graph.for_each_weighted_neighbor(from, [&](VertexId to, const auto& weight) {
      const EdgeWeight candidate = graph_algorithm_detail::add_with_negative_support(distances[from], weight, unreachable);
      if (candidate < distances[to]) {
        throw GraphAlgorithmException("Bellman-Ford detected a negative cycle reachable from the source");
      }
    });
  }

  return ShortestPathsResult<EdgeWeight>(source, std::move(distances), std::move(predecessors));
}

template <WeightedGraph Graph>
auto prim(const Graph& graph) -> MinimumSpanningForestResult<typename Graph::EdgeWeight> {
  using EdgeWeight = typename Graph::EdgeWeight;
  using VertexId   = typename Graph::VertexId;
  using size_type  = std::size_t;

  struct CandidateEdge {
    VertexId   from;
    VertexId   to;
    EdgeWeight weight;
  };

  struct CandidateCompare {
    auto operator()(const CandidateEdge& lhs, const CandidateEdge& rhs) const -> bool { return lhs.weight > rhs.weight; }
  };

  if (graph.is_directed()) {
    throw GraphAlgorithmException("Prim's algorithm requires an undirected graph");
  }

  const size_type    vertex_count = graph.num_vertices();
  DynamicArray<bool> visited(vertex_count, false);

  DynamicArray<typename MinimumSpanningForestResult<EdgeWeight>::Edge> forest_edges;
  forest_edges.reserve(vertex_count > 0 ? vertex_count - 1 : 0);

  ads::queues::PriorityQueue<CandidateEdge, CandidateCompare> frontier;
  EdgeWeight                                                  total_weight    = EdgeWeight{};
  size_type                                                   component_count = 0;

  for (VertexId root = 0; root < vertex_count; ++root) {
    if (visited[root]) {
      continue;
    }

    ++component_count;
    visited[root] = true;

    graph.for_each_weighted_neighbor(root, [&](VertexId neighbor_id, const EdgeWeight& weight) -> auto {
      if (!visited[neighbor_id]) {
        frontier.push({root, neighbor_id, weight});
      }
    });

    while (!frontier.is_empty()) {
      CandidateEdge edge = frontier.top();
      frontier.pop();

      if (visited[edge.to]) {
        continue;
      }

      visited[edge.to] = true;
      total_weight     = static_cast<EdgeWeight>(total_weight + edge.weight);
      forest_edges.push_back({edge.from, edge.to, edge.weight});

      graph.for_each_weighted_neighbor(edge.to, [&](VertexId neighbor_id, const EdgeWeight& weight) {
        if (!visited[neighbor_id]) {
          frontier.push({edge.to, neighbor_id, weight});
        }
      });
    }
  }

  return MinimumSpanningForestResult<EdgeWeight>(total_weight, component_count, std::move(forest_edges));
}

template <WeightedGraph Graph>
auto kruskal(const Graph& graph) -> MinimumSpanningForestResult<typename Graph::EdgeWeight> {
  using EdgeWeight = typename Graph::EdgeWeight;
  using size_type  = std::size_t;
  using ForestEdge = typename MinimumSpanningForestResult<EdgeWeight>::Edge;

  if (graph.is_directed()) {
    throw GraphAlgorithmException("Kruskal's algorithm requires an undirected graph");
  }

  const size_type vertex_count = graph.num_vertices();
  auto            sorted_edges = collect_weighted_edges_once(graph);
  ads::algorithms::quick_sort(sorted_edges.begin(), sorted_edges.end(), graph_algorithm_detail::ForestEdgeLess<EdgeWeight>{});

  DisjointSetUnion         components(vertex_count);
  DynamicArray<ForestEdge> forest_edges;
  forest_edges.reserve(vertex_count > 0 ? vertex_count - 1 : 0);

  EdgeWeight total_weight = EdgeWeight{};
  for (const auto& edge : sorted_edges) {
    if (components.union_sets(edge.from, edge.to)) {
      total_weight = static_cast<EdgeWeight>(total_weight + edge.weight);
      forest_edges.push_back(edge);
    }
  }

  return MinimumSpanningForestResult<EdgeWeight>(total_weight, components.set_count(), std::move(forest_edges));
}

template <WeightedGraph Graph>
auto floyd_warshall(const Graph& graph) -> AllPairsResult<typename Graph::EdgeWeight> {
  using EdgeWeight = typename Graph::EdgeWeight;
  using VertexId   = typename Graph::VertexId;
  using size_type  = std::size_t;

  const size_type  vertex_count = graph.num_vertices();
  const EdgeWeight unreachable  = AllPairsResult<EdgeWeight>::unreachable_distance();

  DynamicArray<DynamicArray<EdgeWeight>> distances;
  DynamicArray<DynamicArray<VertexId>>   next_vertices;
  distances.reserve(vertex_count);
  next_vertices.reserve(vertex_count);

  for (VertexId from = 0; from < vertex_count; ++from) {
    DynamicArray<EdgeWeight> row(vertex_count, unreachable);
    DynamicArray<VertexId>   next_row(vertex_count, AllPairsResult<EdgeWeight>::kNoNextVertex);

    row[from]      = EdgeWeight{};
    next_row[from] = from;

    graph.for_each_weighted_neighbor(from, [&](VertexId to, const EdgeWeight& weight) {
      if (weight < row[to]) {
        row[to]      = weight;
        next_row[to] = to;
      }
    });

    distances.push_back(std::move(row));
    next_vertices.push_back(std::move(next_row));
  }

  for (size_t via = 0; via < vertex_count; ++via) {
    for (VertexId from = 0; from < vertex_count; ++from) {
      if (distances[from][via] == unreachable) {
        continue;
      }

      for (VertexId to = 0; to < vertex_count; ++to) {
        if (distances[via][to] == unreachable) {
          continue;
        }

        const EdgeWeight candidate =
            graph_algorithm_detail::add_with_negative_support(distances[from][via], distances[via][to], unreachable);
        if (candidate < distances[from][to]) {
          distances[from][to]     = candidate;
          next_vertices[from][to] = next_vertices[from][via];
        }
      }
    }
  }

  for (VertexId vertex_id = 0; vertex_id < vertex_count; ++vertex_id) {
    if (distances[vertex_id][vertex_id] < EdgeWeight{}) {
      throw GraphAlgorithmException("Floyd-Warshall detected a negative cycle");
    }
  }

  return AllPairsResult<EdgeWeight>(std::move(distances), std::move(next_vertices));
}

template <TraversableGraph Graph>
auto strongly_connected_components(const Graph& graph) -> SccResult {
  using VertexId  = typename Graph::VertexId;
  using size_type = std::size_t;
  if (!graph.is_directed()) {
    throw GraphAlgorithmException("Strongly connected components require a directed graph");
  }

  const auto adjacency    = build_unweighted_adjacency(graph);
  const auto transpose    = transpose_unweighted_adjacency(adjacency);
  const auto finish_order = compute_finish_order(adjacency);

  DynamicArray<bool>                   visited(graph.num_vertices(), false);
  DynamicArray<size_type>              component_ids(graph.num_vertices(), 0U);
  DynamicArray<DynamicArray<VertexId>> components;
  components.reserve(graph.num_vertices());

  for (size_t index = finish_order.size(); index > 0; --index) {
    const VertexId root = finish_order[index - 1];
    if (visited[root]) {
      continue;
    }

    auto            component    = collect_component_from_transpose(transpose, root, visited);
    const size_type component_id = components.size();
    for (VertexId vertex_id : component) {
      component_ids[vertex_id] = component_id;
    }
    components.push_back(std::move(component));
  }

  return SccResult(std::move(components), std::move(component_ids));
}

template <TraversableGraph Graph>
auto topological_sort(const Graph& graph) -> DynamicArray<size_t> {
  using VertexId  = typename Graph::VertexId;
  using size_type = std::size_t;
  if (!graph.is_directed()) {
    throw GraphAlgorithmException("Topological sort requires a directed graph");
  }

  const size_type         vertex_count = graph.num_vertices();
  DynamicArray<size_type> indegree(vertex_count, 0U);

  for (VertexId from = 0; from < vertex_count; ++from) {
    graph.for_each_weighted_neighbor(from, [&](VertexId to, const auto& weight) {
      static_cast<void>(weight);
      ++indegree[to];
    });
  }

  ads::queues::CircularArrayQueue<VertexId> queue(std::max<size_t>(vertex_count, 1));
  for (VertexId vertex_id = 0; vertex_id < vertex_count; ++vertex_id) {
    if (indegree[vertex_id] == 0) {
      queue.enqueue(vertex_id);
    }
  }

  DynamicArray<VertexId> order;
  order.reserve(vertex_count);

  while (!queue.is_empty()) {
    const VertexId vertex_id = queue.front();
    queue.dequeue();
    order.push_back(vertex_id);

    graph.for_each_weighted_neighbor(vertex_id, [&](VertexId to, const auto& weight) {
      static_cast<void>(weight);
      --indegree[to];
      if (indegree[to] == 0) {
        queue.enqueue(to);
      }
    });
  }

  if (order.size() != vertex_count) {
    throw GraphAlgorithmException("Topological sort detected a directed cycle");
  }

  return order;
}

} // namespace ads::graphs

//===---------------------------------------------------------------------------===//
