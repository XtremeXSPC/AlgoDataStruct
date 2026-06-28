//===---------------------------------------------------------------------------===//
/**
 * @file Test_Graph_Algorithms.cpp
 * @brief Google Test unit tests for reusable graph algorithms.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/graphs/Graph_Algorithms.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <unordered_map>
#include <vector>

using namespace ads::graphs;

// A valid edge payload that is deliberately NOT a PathWeight (no ordering/arithmetic).
struct OpaqueWeight {};

// The structural concept must accept a graph whose weight type is non-numeric,
// while the weighted refinement must reject it.
static_assert(TraversableGraph<GraphAdjacencyList<int, double>>);
static_assert(WeightedGraph<GraphAdjacencyList<int, double>>);
static_assert(TraversableGraph<GraphAdjacencyList<int, OpaqueWeight>>);
static_assert(!WeightedGraph<GraphAdjacencyList<int, OpaqueWeight>>);
static_assert(TraversableGraph<GraphAdjacencyMatrix<int, OpaqueWeight>>);
static_assert(!WeightedGraph<GraphAdjacencyMatrix<int, OpaqueWeight>>);

namespace {

//===----- TEST UTILITIES ------------------------------------------------------===//

auto make_path(std::initializer_list<size_t> vertices) -> ads::arrays::DynamicArray<size_t> {
  ads::arrays::DynamicArray<size_t> path;
  path.reserve(vertices.size());
  for (size_t vertex_id : vertices) {
    path.push_back(vertex_id);
  }
  return path;
}

auto expect_path_eq(const std::optional<ads::arrays::DynamicArray<size_t>>& actual, std::initializer_list<size_t> expected_vertices)
    -> void {
  ASSERT_TRUE(actual.has_value());

  const auto expected = make_path(expected_vertices);
  ASSERT_EQ(actual->size(), expected.size());
  for (size_t index = 0; index < expected.size(); ++index) {
    EXPECT_EQ((*actual)[index], expected[index]);
  }
}

auto expect_valid_topological_order(
    const ads::arrays::DynamicArray<size_t>& order, const std::vector<std::pair<size_t, size_t>>& edges, size_t vertex_count) -> void {
  ASSERT_EQ(order.size(), vertex_count);

  std::unordered_map<size_t, size_t> position;
  for (size_t index = 0; index < order.size(); ++index) {
    position.emplace(order[index], index);
  }

  ASSERT_EQ(position.size(), vertex_count);
  for (const auto& [from, to] : edges) {
    ASSERT_TRUE(position.contains(from));
    ASSERT_TRUE(position.contains(to));
    EXPECT_LT(position[from], position[to]);
  }
}

auto normalize_components(const SccResult& result) -> std::vector<std::vector<size_t>> {
  std::vector<std::vector<size_t>> normalized;
  normalized.reserve(result.component_count());

  for (const auto& component : result.components()) {
    std::vector<size_t> vertices;
    vertices.reserve(component.size());
    for (size_t vertex_id : component) {
      vertices.push_back(vertex_id);
    }
    std::ranges::sort(vertices);
    normalized.push_back(std::move(vertices));
  }

  std::ranges::sort(normalized);
  return normalized;
}

} // namespace

//===----- DIJKSTRA TESTS ------------------------------------------------------===//

TEST(GraphAlgorithmsDijkstraListTest, ComputesShortestDistancesAndPaths) {
  GraphAdjacencyList<int, double> graph(false);

  for (int vertex = 0; vertex < 6; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 7.0);
  graph.add_edge(0, 2, 9.0);
  graph.add_edge(0, 5, 14.0);
  graph.add_edge(1, 2, 10.0);
  graph.add_edge(1, 3, 15.0);
  graph.add_edge(2, 3, 11.0);
  graph.add_edge(2, 5, 2.0);
  graph.add_edge(3, 4, 6.0);
  graph.add_edge(4, 5, 9.0);

  auto shortest_paths = dijkstra(graph, 0);

  EXPECT_EQ(shortest_paths.source(), 0U);
  EXPECT_EQ(shortest_paths.vertex_count(), 6U);
  EXPECT_DOUBLE_EQ(shortest_paths.distance_to(0), 0.0);
  EXPECT_DOUBLE_EQ(shortest_paths.distance_to(1), 7.0);
  EXPECT_DOUBLE_EQ(shortest_paths.distance_to(2), 9.0);
  EXPECT_DOUBLE_EQ(shortest_paths.distance_to(3), 20.0);
  EXPECT_DOUBLE_EQ(shortest_paths.distance_to(4), 20.0);
  EXPECT_DOUBLE_EQ(shortest_paths.distance_to(5), 11.0);
  expect_path_eq(shortest_paths.path_to(4), {0, 2, 5, 4});
}

TEST(GraphAlgorithmsDijkstraMatrixTest, WorksWithAdjacencyMatrixGraphs) {
  GraphAdjacencyMatrix<int, int> graph(false);

  for (int vertex = 0; vertex < 5; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 4);
  graph.add_edge(0, 2, 1);
  graph.add_edge(2, 1, 2);
  graph.add_edge(1, 3, 1);
  graph.add_edge(2, 3, 5);
  graph.add_edge(3, 4, 3);

  auto shortest_paths = dijkstra(graph, 0);

  EXPECT_EQ(shortest_paths.distance_to(0), 0);
  EXPECT_EQ(shortest_paths.distance_to(1), 3);
  EXPECT_EQ(shortest_paths.distance_to(2), 1);
  EXPECT_EQ(shortest_paths.distance_to(3), 4);
  EXPECT_EQ(shortest_paths.distance_to(4), 7);
  expect_path_eq(shortest_paths.path_to(4), {0, 2, 1, 3, 4});
}

TEST(GraphAlgorithmsDijkstraTest, UnreachableVerticesKeepSentinelDistance) {
  GraphAdjacencyList<int, double> graph(true);

  for (int vertex = 0; vertex < 4; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 3.0);
  graph.add_edge(1, 2, 4.0);

  auto shortest_paths = dijkstra(graph, 0);

  EXPECT_TRUE(shortest_paths.has_path_to(0));
  EXPECT_TRUE(shortest_paths.has_path_to(2));
  EXPECT_FALSE(shortest_paths.has_path_to(3));
  EXPECT_EQ(shortest_paths.path_to(3), std::nullopt);
  EXPECT_EQ(shortest_paths.predecessor_of(3), ShortestPathsResult<double>::kNoPredecessor);
  EXPECT_EQ(shortest_paths.distance_to(3), std::numeric_limits<double>::infinity());
}

TEST(GraphAlgorithmsDijkstraTest, RejectsNegativeEdgeWeights) {
  GraphAdjacencyList<int, int> graph(true);

  graph.add_vertex(0);
  graph.add_vertex(1);
  graph.add_edge(0, 1, -5);

  EXPECT_THROW(static_cast<void>(dijkstra(graph, 0)), GraphAlgorithmException);
}

TEST(GraphAlgorithmsDijkstraTest, RejectsInvalidSourceVertex) {
  GraphAdjacencyList<int> graph;
  graph.add_vertex(0);

  EXPECT_THROW(static_cast<void>(dijkstra(graph, 1)), GraphAlgorithmException);
}

TEST(GraphAlgorithmsDijkstraTest, SourcePathIsSingleVertex) {
  GraphAdjacencyList<int> graph;
  graph.add_vertex(42);

  auto shortest_paths = dijkstra(graph, 0);

  EXPECT_TRUE(shortest_paths.has_path_to(0));
  expect_path_eq(shortest_paths.path_to(0), {0});
}

//===----- BELLMAN-FORD TESTS --------------------------------------------------===//

TEST(GraphAlgorithmsBellmanFordTest, HandlesNegativeEdgesWithoutNegativeCycles) {
  GraphAdjacencyList<int, int> graph(true);

  for (int vertex = 0; vertex < 5; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 6);
  graph.add_edge(0, 2, 7);
  graph.add_edge(1, 2, 8);
  graph.add_edge(1, 3, 5);
  graph.add_edge(1, 4, -4);
  graph.add_edge(2, 3, -3);
  graph.add_edge(2, 4, 9);
  graph.add_edge(3, 1, -2);
  graph.add_edge(4, 0, 2);
  graph.add_edge(4, 3, 7);

  auto shortest_paths = bellman_ford(graph, 0);

  EXPECT_EQ(shortest_paths.distance_to(0), 0);
  EXPECT_EQ(shortest_paths.distance_to(1), 2);
  EXPECT_EQ(shortest_paths.distance_to(2), 7);
  EXPECT_EQ(shortest_paths.distance_to(3), 4);
  EXPECT_EQ(shortest_paths.distance_to(4), -2);
  expect_path_eq(shortest_paths.path_to(4), {0, 2, 3, 1, 4});
}

TEST(GraphAlgorithmsBellmanFordTest, DetectsReachableNegativeCycles) {
  GraphAdjacencyList<int, int> graph(true);

  for (int vertex = 0; vertex < 3; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 1);
  graph.add_edge(1, 2, -3);
  graph.add_edge(2, 1, 1);

  EXPECT_THROW(static_cast<void>(bellman_ford(graph, 0)), GraphAlgorithmException);
}

TEST(GraphAlgorithmsBellmanFordMatrixTest, WorksWithAdjacencyMatrixGraphs) {
  GraphAdjacencyMatrix<int, int> graph(true);

  for (int vertex = 0; vertex < 4; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 4);
  graph.add_edge(0, 2, 5);
  graph.add_edge(1, 2, -2);
  graph.add_edge(2, 3, 3);

  auto shortest_paths = bellman_ford(graph, 0);

  EXPECT_EQ(shortest_paths.distance_to(0), 0);
  EXPECT_EQ(shortest_paths.distance_to(1), 4);
  EXPECT_EQ(shortest_paths.distance_to(2), 2);
  EXPECT_EQ(shortest_paths.distance_to(3), 5);
  expect_path_eq(shortest_paths.path_to(3), {0, 1, 2, 3});
}

//===----- PRIM TESTS ----------------------------------------------------------===//

TEST(GraphAlgorithmsPrimTest, BuildsMinimumSpanningForestForUndirectedGraph) {
  GraphAdjacencyList<int, int> graph(false);

  for (int vertex = 0; vertex < 6; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 4);
  graph.add_edge(0, 2, 3);
  graph.add_edge(1, 2, 1);
  graph.add_edge(1, 3, 2);
  graph.add_edge(2, 3, 4);
  graph.add_edge(4, 5, 7);

  auto forest = prim(graph);

  EXPECT_EQ(forest.component_count(), 2U);
  EXPECT_EQ(forest.edge_count(), 4U);
  EXPECT_EQ(forest.total_weight(), 13);
  EXPECT_FALSE(forest.spans_all_vertices(graph.num_vertices()));
}

TEST(GraphAlgorithmsPrimMatrixTest, WorksWithAdjacencyMatrixGraphs) {
  GraphAdjacencyMatrix<int, int> graph(false);

  for (int vertex = 0; vertex < 4; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 10);
  graph.add_edge(0, 2, 6);
  graph.add_edge(0, 3, 5);
  graph.add_edge(1, 3, 15);
  graph.add_edge(2, 3, 4);

  auto forest = prim(graph);

  EXPECT_EQ(forest.component_count(), 1U);
  EXPECT_EQ(forest.edge_count(), 3U);
  EXPECT_EQ(forest.total_weight(), 19);
  EXPECT_TRUE(forest.spans_all_vertices(graph.num_vertices()));
}

TEST(GraphAlgorithmsPrimTest, RejectsDirectedGraphs) {
  GraphAdjacencyList<int, int> graph(true);
  graph.add_vertex(0);
  graph.add_vertex(1);
  graph.add_edge(0, 1, 3);

  EXPECT_THROW(static_cast<void>(prim(graph)), GraphAlgorithmException);
}

//===----- KRUSKAL TESTS -------------------------------------------------------===//

TEST(GraphAlgorithmsKruskalTest, BuildsMinimumSpanningForestForUndirectedGraph) {
  GraphAdjacencyList<int, int> graph(false);

  for (int vertex = 0; vertex < 6; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 4);
  graph.add_edge(0, 2, 3);
  graph.add_edge(1, 2, 1);
  graph.add_edge(1, 3, 2);
  graph.add_edge(2, 3, 4);
  graph.add_edge(4, 5, 7);

  auto forest = kruskal(graph);

  EXPECT_EQ(forest.component_count(), 2U);
  EXPECT_EQ(forest.edge_count(), 4U);
  EXPECT_EQ(forest.total_weight(), 13);
  EXPECT_FALSE(forest.spans_all_vertices(graph.num_vertices()));
}

TEST(GraphAlgorithmsKruskalMatrixTest, WorksWithAdjacencyMatrixGraphs) {
  GraphAdjacencyMatrix<int, int> graph(false);

  for (int vertex = 0; vertex < 4; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 10);
  graph.add_edge(0, 2, 6);
  graph.add_edge(0, 3, 5);
  graph.add_edge(1, 3, 15);
  graph.add_edge(2, 3, 4);

  auto forest = kruskal(graph);

  EXPECT_EQ(forest.component_count(), 1U);
  EXPECT_EQ(forest.edge_count(), 3U);
  EXPECT_EQ(forest.total_weight(), 19);
  EXPECT_TRUE(forest.spans_all_vertices(graph.num_vertices()));
}

TEST(GraphAlgorithmsKruskalTest, HandlesNegativeEdgeWeights) {
  GraphAdjacencyList<int, int> graph(false);

  for (int vertex = 0; vertex < 4; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, -3);
  graph.add_edge(1, 2, 2);
  graph.add_edge(2, 3, 1);
  graph.add_edge(0, 3, 4);
  graph.add_edge(0, 2, 5);

  auto forest = kruskal(graph);

  EXPECT_EQ(forest.component_count(), 1U);
  EXPECT_EQ(forest.edge_count(), 3U);
  EXPECT_EQ(forest.total_weight(), 0);
  EXPECT_TRUE(forest.spans_all_vertices(graph.num_vertices()));
}

TEST(GraphAlgorithmsKruskalTest, MatchesPrimOnConnectedGraph) {
  GraphAdjacencyList<int, int> graph(false);

  for (int vertex = 0; vertex < 5; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 2);
  graph.add_edge(0, 3, 6);
  graph.add_edge(1, 2, 3);
  graph.add_edge(1, 3, 8);
  graph.add_edge(1, 4, 5);
  graph.add_edge(2, 4, 7);
  graph.add_edge(3, 4, 9);

  const auto prim_forest    = prim(graph);
  const auto kruskal_forest = kruskal(graph);

  EXPECT_EQ(kruskal_forest.component_count(), prim_forest.component_count());
  EXPECT_EQ(kruskal_forest.edge_count(), prim_forest.edge_count());
  EXPECT_EQ(kruskal_forest.total_weight(), prim_forest.total_weight());
  EXPECT_TRUE(kruskal_forest.spans_all_vertices(graph.num_vertices()));
}

TEST(GraphAlgorithmsKruskalTest, RejectsDirectedGraphs) {
  GraphAdjacencyList<int, int> graph(true);
  graph.add_vertex(0);
  graph.add_vertex(1);
  graph.add_edge(0, 1, 3);

  EXPECT_THROW(static_cast<void>(kruskal(graph)), GraphAlgorithmException);
}

//===----- FLOYD-WARSHALL TESTS ------------------------------------------------===//

TEST(GraphAlgorithmsFloydWarshallMatrixTest, ComputesAllPairsShortestPaths) {
  GraphAdjacencyMatrix<int, int> graph(true);

  for (int vertex = 0; vertex < 4; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 5);
  graph.add_edge(0, 3, 10);
  graph.add_edge(1, 2, 3);
  graph.add_edge(2, 3, 1);

  const auto result = floyd_warshall(graph);

  EXPECT_EQ(result.vertex_count(), 4U);
  EXPECT_EQ(result.distance(0, 0), 0);
  EXPECT_EQ(result.distance(0, 1), 5);
  EXPECT_EQ(result.distance(0, 2), 8);
  EXPECT_EQ(result.distance(0, 3), 9);
  EXPECT_TRUE(result.has_path(0, 3));
  expect_path_eq(result.path(0, 3), {0, 1, 2, 3});
}

TEST(GraphAlgorithmsFloydWarshallListTest, WorksWithAdjacencyListGraphs) {
  GraphAdjacencyList<int, int> graph(true);

  for (int vertex = 0; vertex < 4; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 4);
  graph.add_edge(0, 2, 11);
  graph.add_edge(1, 2, -2);
  graph.add_edge(2, 3, 3);

  const auto result = floyd_warshall(graph);

  EXPECT_EQ(result.distance(0, 2), 2);
  EXPECT_EQ(result.distance(0, 3), 5);
  EXPECT_TRUE(result.has_path(0, 3));
  expect_path_eq(result.path(0, 3), {0, 1, 2, 3});
}

TEST(GraphAlgorithmsFloydWarshallTest, UnreachablePairsKeepSentinelDistance) {
  GraphAdjacencyList<int, double> graph(true);

  for (int vertex = 0; vertex < 3; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 2.5);

  const auto result = floyd_warshall(graph);

  EXPECT_TRUE(result.has_path(0, 0));
  EXPECT_TRUE(result.has_path(0, 1));
  EXPECT_FALSE(result.has_path(0, 2));
  EXPECT_EQ(result.path(0, 2), std::nullopt);
  EXPECT_EQ(result.distance(0, 2), std::numeric_limits<double>::infinity());
}

TEST(GraphAlgorithmsFloydWarshallTest, DetectsNegativeCycles) {
  GraphAdjacencyList<int, int> graph(true);

  for (int vertex = 0; vertex < 3; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 1);
  graph.add_edge(1, 2, -3);
  graph.add_edge(2, 0, 1);

  EXPECT_THROW(static_cast<void>(floyd_warshall(graph)), GraphAlgorithmException);
}

//===----- SCC TESTS -----------------------------------------------------------===//

TEST(GraphAlgorithmsSccTest, ReturnsSingletonComponentsForDag) {
  GraphAdjacencyList<int, int> graph(true);

  for (int vertex = 0; vertex < 5; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 1);
  graph.add_edge(1, 2, 1);
  graph.add_edge(1, 3, 1);
  graph.add_edge(3, 4, 1);

  const auto result = strongly_connected_components(graph);

  EXPECT_EQ(result.vertex_count(), 5U);
  EXPECT_EQ(result.component_count(), 5U);
  EXPECT_FALSE(result.are_strongly_connected(0, 1));
  EXPECT_FALSE(result.are_strongly_connected(3, 4));

  const auto normalized = normalize_components(result);
  EXPECT_EQ(normalized, (std::vector<std::vector<size_t>>{{0}, {1}, {2}, {3}, {4}}));
}

TEST(GraphAlgorithmsSccTest, CollapsesSingleCycleIntoOneComponent) {
  GraphAdjacencyList<int, int> graph(true);

  for (int vertex = 0; vertex < 4; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 1);
  graph.add_edge(1, 2, 1);
  graph.add_edge(2, 3, 1);
  graph.add_edge(3, 0, 1);

  const auto result = strongly_connected_components(graph);

  EXPECT_EQ(result.component_count(), 1U);
  EXPECT_TRUE(result.are_strongly_connected(0, 3));
  EXPECT_EQ(result.component(0).size(), 4U);

  const auto normalized = normalize_components(result);
  EXPECT_EQ(normalized, (std::vector<std::vector<size_t>>{{0, 1, 2, 3}}));
}

TEST(GraphAlgorithmsSccTest, SeparatesMultipleConnectedRegions) {
  GraphAdjacencyList<int, int> graph(true);

  for (int vertex = 0; vertex < 8; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 1);
  graph.add_edge(1, 2, 1);
  graph.add_edge(2, 0, 1);
  graph.add_edge(2, 3, 1);
  graph.add_edge(3, 4, 1);
  graph.add_edge(4, 5, 1);
  graph.add_edge(5, 3, 1);
  graph.add_edge(5, 6, 1);
  graph.add_edge(6, 7, 1);
  graph.add_edge(7, 6, 1);

  const auto result = strongly_connected_components(graph);

  EXPECT_EQ(result.component_count(), 3U);
  EXPECT_TRUE(result.are_strongly_connected(0, 2));
  EXPECT_TRUE(result.are_strongly_connected(3, 5));
  EXPECT_TRUE(result.are_strongly_connected(6, 7));
  EXPECT_FALSE(result.are_strongly_connected(2, 3));
  EXPECT_FALSE(result.are_strongly_connected(5, 6));

  const auto normalized = normalize_components(result);
  EXPECT_EQ(normalized, (std::vector<std::vector<size_t>>{{0, 1, 2}, {3, 4, 5}, {6, 7}}));
}

TEST(GraphAlgorithmsSccMatrixTest, WorksWithAdjacencyMatrixGraphs) {
  GraphAdjacencyMatrix<int, int> graph(true);

  for (int vertex = 0; vertex < 5; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 1);
  graph.add_edge(1, 0, 1);
  graph.add_edge(1, 2, 1);
  graph.add_edge(2, 3, 1);
  graph.add_edge(3, 4, 1);
  graph.add_edge(4, 2, 1);

  const auto result = strongly_connected_components(graph);

  EXPECT_EQ(result.component_count(), 2U);
  EXPECT_TRUE(result.are_strongly_connected(0, 1));
  EXPECT_TRUE(result.are_strongly_connected(2, 4));
  EXPECT_FALSE(result.are_strongly_connected(1, 2));

  const auto normalized = normalize_components(result);
  EXPECT_EQ(normalized, (std::vector<std::vector<size_t>>{{0, 1}, {2, 3, 4}}));
}

TEST(GraphAlgorithmsSccTest, HandlesEmptyGraphs) {
  GraphAdjacencyList<int, int> graph(true);

  const auto result = strongly_connected_components(graph);

  EXPECT_EQ(result.vertex_count(), 0U);
  EXPECT_EQ(result.component_count(), 0U);
  EXPECT_TRUE(result.components().is_empty());
}

TEST(GraphAlgorithmsSccTest, RejectsUndirectedGraphs) {
  GraphAdjacencyList<int, int> graph(false);
  graph.add_vertex(0);
  graph.add_vertex(1);
  graph.add_edge(0, 1, 1);

  EXPECT_THROW(static_cast<void>(strongly_connected_components(graph)), GraphAlgorithmException);
}

//===----- TOPOLOGICAL SORT TESTS ----------------------------------------------===//

TEST(GraphAlgorithmsTopologicalSortTest, ProducesValidOrderForDag) {
  GraphAdjacencyList<int, int> graph(true);

  for (int vertex = 0; vertex < 6; ++vertex) {
    graph.add_vertex(vertex);
  }

  std::vector<std::pair<size_t, size_t>> edges = {
      {5, 2}, {5, 0}, {4, 0}, {4, 1}, {2, 3}, {3, 1},
  };

  for (const auto& [from, to] : edges) {
    graph.add_edge(from, to, 1);
  }

  const auto order = topological_sort(graph);
  expect_valid_topological_order(order, edges, graph.num_vertices());
}

TEST(GraphAlgorithmsTopologicalSortMatrixTest, WorksWithAdjacencyMatrixGraphs) {
  GraphAdjacencyMatrix<int, int> graph(true);

  for (int vertex = 0; vertex < 4; ++vertex) {
    graph.add_vertex(vertex);
  }

  const std::vector<std::pair<size_t, size_t>> edges = {
      {0, 1},
      {0, 2},
      {1, 3},
      {2, 3},
  };

  for (const auto& [from, to] : edges) {
    graph.add_edge(from, to, 1);
  }

  const auto order = topological_sort(graph);
  expect_valid_topological_order(order, edges, graph.num_vertices());
}

TEST(GraphAlgorithmsTopologicalSortTest, RejectsCycles) {
  GraphAdjacencyList<int, int> graph(true);

  for (int vertex = 0; vertex < 3; ++vertex) {
    graph.add_vertex(vertex);
  }

  graph.add_edge(0, 1, 1);
  graph.add_edge(1, 2, 1);
  graph.add_edge(2, 0, 1);

  EXPECT_THROW(static_cast<void>(topological_sort(graph)), GraphAlgorithmException);
}

TEST(GraphAlgorithmsTopologicalSortTest, RejectsUndirectedGraphs) {
  GraphAdjacencyList<int, int> graph(false);
  graph.add_vertex(0);
  graph.add_vertex(1);
  graph.add_edge(0, 1, 1);

  EXPECT_THROW(static_cast<void>(topological_sort(graph)), GraphAlgorithmException);
}

//===---------------------------------------------------------------------------===//
