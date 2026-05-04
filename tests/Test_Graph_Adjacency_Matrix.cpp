//===---------------------------------------------------------------------------===//
/**
 * @file Test_Graph_Adjacency_Matrix.cpp
 * @brief Google Test unit tests for GraphAdjacencyMatrix.
 * @version 0.1
 * @date 2026-05-05
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/graphs/Graph_Adjacency_Matrix.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ads::graphs;

class GraphAdjacencyMatrixTest : public ::testing::Test {
protected:
  GraphAdjacencyMatrix<int> graph;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(GraphAdjacencyMatrixTest, IsEmptyOnConstruction) {
  EXPECT_TRUE(graph.is_empty());
  EXPECT_FALSE(graph.is_directed());
  EXPECT_EQ(graph.num_vertices(), 0);
  EXPECT_EQ(graph.num_edges(), 0);
}

TEST(GraphAdjacencyMatrixConstructionTest, PreallocatedConstructorStartsEmpty) {
  GraphAdjacencyMatrix<int> graph(size_t{8});

  EXPECT_TRUE(graph.is_empty());
  EXPECT_EQ(graph.num_vertices(), 0);

  graph.add_vertex(10);
  graph.add_vertex(20);
  graph.add_edge(0, 1);

  EXPECT_EQ(graph.num_vertices(), 2);
  EXPECT_TRUE(graph.has_edge(0, 1));
}

TEST_F(GraphAdjacencyMatrixTest, AddVertexStoresData) {
  const size_t first  = graph.add_vertex(10);
  const size_t second = graph.add_vertex(20);

  EXPECT_EQ(first, 0);
  EXPECT_EQ(second, 1);
  EXPECT_EQ(graph.num_vertices(), 2);
  EXPECT_EQ(graph.get_vertex_data(first), 10);

  graph.get_vertex_data(second) = 30;
  EXPECT_EQ(graph.get_vertex_data(second), 30);
}

TEST(GraphAdjacencyMatrixStringTest, AddVertexMovesData) {
  GraphAdjacencyMatrix<std::string> graph;

  const size_t id = graph.add_vertex(std::string("Rome"));

  EXPECT_EQ(graph.get_vertex_data(id), "Rome");
}

//===------------------------------- EDGE TESTS --------------------------------===//

TEST_F(GraphAdjacencyMatrixTest, UndirectedEdgesAreStoredSymmetrically) {
  graph.add_vertex(0);
  graph.add_vertex(1);
  graph.add_vertex(2);

  graph.add_edge(0, 1, 2.5);
  graph.add_edge(0, 2, 4.5);

  EXPECT_EQ(graph.num_edges(), 2);
  EXPECT_TRUE(graph.has_edge(0, 1));
  EXPECT_TRUE(graph.has_edge(1, 0));
  EXPECT_EQ(graph.get_edge_weight(0, 1), 2.5);
  EXPECT_EQ(graph.get_edge_weight(1, 0), 2.5);

  EXPECT_EQ(graph.get_neighbors(0), (std::vector<size_t>{1, 2}));
  EXPECT_EQ(graph.degree(0), 2);
}

TEST(GraphAdjacencyMatrixDirectedTest, DirectedEdgesDoNotAddReverseEdge) {
  GraphAdjacencyMatrix<int> graph(true);
  graph.add_vertex(0);
  graph.add_vertex(1);

  graph.add_edge(0, 1);
  graph.add_edge(0, 1);

  EXPECT_EQ(graph.num_edges(), 1);
  EXPECT_TRUE(graph.has_edge(0, 1));
  EXPECT_FALSE(graph.has_edge(1, 0));
}

TEST_F(GraphAdjacencyMatrixTest, RemoveUndirectedEdgeRemovesReverseEdge) {
  graph.add_vertex(0);
  graph.add_vertex(1);

  graph.add_edge(0, 1);
  graph.remove_edge(0, 1);

  EXPECT_EQ(graph.num_edges(), 0);
  EXPECT_FALSE(graph.has_edge(0, 1));
  EXPECT_FALSE(graph.has_edge(1, 0));
}

//===----------------------------- TRAVERSAL TESTS -----------------------------===//

TEST_F(GraphAdjacencyMatrixTest, BfsVisitsVerticesByLevel) {
  for (int value = 0; value < 6; ++value) {
    graph.add_vertex(value);
  }

  graph.add_edge(0, 1);
  graph.add_edge(0, 3);
  graph.add_edge(1, 2);
  graph.add_edge(1, 4);
  graph.add_edge(2, 5);
  graph.add_edge(3, 4);

  EXPECT_EQ(graph.bfs(0), (std::vector<size_t>{0, 1, 3, 2, 4, 5}));
}

TEST_F(GraphAdjacencyMatrixTest, DfsFollowsVertexOrder) {
  for (int value = 0; value < 6; ++value) {
    graph.add_vertex(value);
  }

  graph.add_edge(0, 1);
  graph.add_edge(0, 3);
  graph.add_edge(1, 2);
  graph.add_edge(1, 4);
  graph.add_edge(2, 5);
  graph.add_edge(3, 4);

  EXPECT_EQ(graph.dfs(0), (std::vector<size_t>{0, 1, 2, 5, 4, 3}));
}

TEST_F(GraphAdjacencyMatrixTest, FindPathUsesBfsParentChain) {
  for (int value = 0; value < 6; ++value) {
    graph.add_vertex(value);
  }

  graph.add_edge(0, 1);
  graph.add_edge(0, 3);
  graph.add_edge(1, 2);
  graph.add_edge(1, 4);
  graph.add_edge(2, 5);
  graph.add_edge(3, 4);

  const auto path = graph.find_path(0, 5);

  ASSERT_TRUE(path.has_value());
  EXPECT_EQ(path.value(), (std::vector<size_t>{0, 1, 2, 5}));
  EXPECT_TRUE(graph.is_connected(0, 5));
}

TEST_F(GraphAdjacencyMatrixTest, ConnectedComponentsGroupsUndirectedGraph) {
  for (int value = 0; value < 6; ++value) {
    graph.add_vertex(value);
  }

  graph.add_edge(0, 1);
  graph.add_edge(1, 2);
  graph.add_edge(3, 4);

  const auto components = graph.connected_components();

  ASSERT_EQ(components.size(), 3);
  EXPECT_EQ(components[0], (std::vector<size_t>{0, 1, 2}));
  EXPECT_EQ(components[1], (std::vector<size_t>{3, 4}));
  EXPECT_EQ(components[2], (std::vector<size_t>{5}));
}

//===------------------------------- ERROR TESTS -------------------------------===//

TEST_F(GraphAdjacencyMatrixTest, InvalidVerticesThrow) {
  graph.add_vertex(0);

  EXPECT_THROW(graph.add_edge(0, 1), GraphMatrixException);
  EXPECT_THROW(static_cast<void>(graph.bfs(1)), GraphMatrixException);
  EXPECT_THROW(graph.get_vertex_data(1), GraphMatrixException);
}

//===------------------------------- MOVE TESTS --------------------------------===//

TEST_F(GraphAdjacencyMatrixTest, MoveSemanticsTransferStorage) {
  graph.add_vertex(0);
  graph.add_vertex(1);
  graph.add_edge(0, 1);

  GraphAdjacencyMatrix<int> moved(std::move(graph));

  EXPECT_TRUE(graph.is_empty());
  EXPECT_EQ(graph.num_edges(), 0);
  EXPECT_EQ(moved.num_vertices(), 2);
  EXPECT_EQ(moved.num_edges(), 1);
  EXPECT_TRUE(moved.has_edge(0, 1));

  GraphAdjacencyMatrix<int> assigned(true);
  assigned = std::move(moved);

  EXPECT_TRUE(moved.is_empty());
  EXPECT_EQ(moved.num_edges(), 0);
  EXPECT_FALSE(assigned.is_directed());
  EXPECT_TRUE(assigned.has_edge(1, 0));
}

//===---------------------------------------------------------------------------===//
