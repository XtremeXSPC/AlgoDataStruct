//===---------------------------------------------------------------------------===//
/**
 * @file main_Graph_Adjacency_Matrix.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for GraphAdjacencyMatrix class with
 *        performance comparison.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the GraphAdjacencyMatrix data structure,
 * showcasing its construction, vertex/edge management, traversal algorithms, and
 * performance comparison with GraphAdjacencyList.
 */
//===---------------------------------------------------------------------------===//

#include <chrono>
#include <format>
#include <iostream>
#include <string>

#include "../include/ads/graphs/Graph_Adjacency_List.hpp"
#include "../include/ads/graphs/Graph_Adjacency_Matrix.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::to_string;
using std::vector;

using namespace ads::graphs;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Print a separator with title.
void print_separator(const string& title) {
  cout << "\n=====---------- " << title << " ----------=====\n";
}

template <typename T>
void print_vector(const vector<T>& vec, const string& prefix = "") {
  cout << prefix;
  for (const auto& elem : vec) {
    cout << elem << " ";
  }
  cout << '\n';
}

//===------------------------------ MATRIX DEMOS -------------------------------===//

// Test construction and basic properties.
void demo_matrix_construction() {
  print_separator("Matrix - Construction and Properties");

  GraphAdjacencyMatrix<int> graph(false);
  cout << "Created undirected graph matrix\n";
  cout << std::format("Empty: {}\n", graph.is_empty());
  cout << std::format("Directed: {}\n", graph.is_directed());
  cout << "Vertices: " << graph.num_vertices() << '\n';
  cout << "Edges: " << graph.num_edges() << '\n';
}

// Test vertex and edge operations.
void demo_matrix_operations() {
  print_separator("Matrix - Basic Operations");

  GraphAdjacencyMatrix<string> graph(false);

  auto v0 = graph.add_vertex("A");
  auto v1 = graph.add_vertex("B");
  auto v2 = graph.add_vertex("C");
  auto v3 = graph.add_vertex("D");

  cout << "Added 4 vertices\n";
  cout << "Number of vertices: " << graph.num_vertices() << '\n';

  graph.add_edge(v0, v1, 1.5);
  graph.add_edge(v0, v2, 2.0);
  graph.add_edge(v1, v3, 3.0);
  graph.add_edge(v2, v3, 4.0);

  cout << "Added 4 edges\n";
  cout << "Number of edges: " << graph.num_edges() << '\n';

  cout << "\nEdge lookups (O(1) for matrix):\n";
  cout << std::format("Has edge A->B: {}\n", graph.has_edge(v0, v1));
  cout << std::format("Has edge B->A: {} (undirected)\n", graph.has_edge(v1, v0));
  cout << std::format("Has edge A->D: {}\n", graph.has_edge(v0, v3));

  auto weight = graph.get_edge_weight(v0, v1);
  cout << "\nEdge A->B weight: " << (weight ? to_string(*weight) : "none") << '\n';

  cout << "\nNeighbors of A: ";
  auto neighbors = graph.get_neighbors(v0);
  for (auto n : neighbors) {
    cout << graph.get_vertex_data(n) << " ";
  }
  cout << '\n';
}

//===---------------------------- MATRIX TRAVERSAL -----------------------------===//

// Test traversal algorithms.
void demo_matrix_traversal() {
  print_separator("Matrix - Graph Traversal");

  GraphAdjacencyMatrix<int> graph(false);

  for (int i = 0; i < 6; ++i) {
    graph.add_vertex(i);
  }

  graph.add_edge(0, 1);
  graph.add_edge(0, 3);
  graph.add_edge(1, 2);
  graph.add_edge(1, 4);
  graph.add_edge(2, 5);
  graph.add_edge(3, 4);

  cout << "Graph with 6 vertices created\n";

  cout << "\nBFS from vertex 0:\n";
  auto bfs_result = graph.bfs(0);
  print_vector(bfs_result, "Traversal order: ");

  cout << "\nDFS from vertex 0:\n";
  auto dfs_result = graph.dfs(0);
  print_vector(dfs_result, "Traversal order: ");

  cout << "\nPath from 0 to 5:\n";
  auto path = graph.find_path(0, 5);
  if (path) {
    print_vector(*path, "Path: ");
  }
}

//===----------------------- MATRIX CONNECTED COMPONENTS -----------------------===//

// Test connected components.
void demo_matrix_connected_components() {
  print_separator("Matrix - Connected Components");

  GraphAdjacencyMatrix<int> graph(false);

  for (int i = 0; i < 6; ++i) {
    graph.add_vertex(i);
  }

  graph.add_edge(0, 1);
  graph.add_edge(1, 2);
  graph.add_edge(3, 4);

  cout << "Graph with 6 vertices, 3 components\n";

  auto components = graph.connected_components();
  cout << "Number of connected components: " << components.size() << '\n';

  for (size_t i = 0; i < components.size(); ++i) {
    cout << "Component " << i + 1 << ": ";
    print_vector(components[i]);
  }
}

//===------------------------- PERFORMANCE COMPARISON --------------------------===//

void compare_performance() {
  print_separator("Performance Comparison - List vs Matrix");

  const size_t N = 1000;

  // Create both representations.
  GraphAdjacencyList<int>   list_graph(false);
  GraphAdjacencyMatrix<int> matrix_graph(false);

  // Add vertices
  cout << "Adding " << N << " vertices...\n";
  for (size_t i = 0; i < N; ++i) {
    list_graph.add_vertex(static_cast<int>(i));
    matrix_graph.add_vertex(static_cast<int>(i));
  }

  // Create sparse graph: each vertex connected to 5 neighbors.
  cout << "Creating sparse graph (5 edges per vertex)...\n";
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 1; j <= 5 && i + j < N; ++j) {
      list_graph.add_edge(i, i + j);
      matrix_graph.add_edge(i, i + j);
    }
  }

  cout << "Total edges: " << list_graph.num_edges() << '\n';

  // Test 1: Edge lookup.
  cout << "\n[Test 1] Edge lookup (checking 10000 edges):\n";

  auto start = std::chrono::high_resolution_clock::now();

  volatile size_t count = 0;
  for (size_t i = 0; i < N && i < 10000; ++i) {
    for (size_t j = 0; j < 10 && j < N; ++j) {
      if (list_graph.has_edge(i, j))
        count = count + 1;
    }
  }

  auto end = std::chrono::high_resolution_clock::now();

  // Measure time.
  auto list_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  cout << "  List:   " << list_time << " µs (O(degree) lookup)\n";

  // Matrix lookup.
  start = std::chrono::high_resolution_clock::now();
  count = 0;
  for (size_t i = 0; i < N && i < 10000; ++i) {
    for (size_t j = 0; j < 10 && j < N; ++j) {
      if (matrix_graph.has_edge(i, j))
        count = count + 1;
    }
  }
  end = std::chrono::high_resolution_clock::now();

  // Measure time.
  auto matrix_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  cout << "  Matrix: " << matrix_time << " µs (O(1) lookup)\n";
  cout << "  Winner: " << (matrix_time < list_time ? "Matrix" : "List") << " ("
       << (100.0 * std::abs(static_cast<double>(matrix_time) - static_cast<double>(list_time))
           / std::max(static_cast<double>(matrix_time), static_cast<double>(list_time)))
       << "% faster)\n";

  // Test 2: BFS.
  cout << "\n[Test 2] BFS traversal:\n";

  start = std::chrono::high_resolution_clock::now();

  auto list_bfs = list_graph.bfs(0);

  end = std::chrono::high_resolution_clock::now();

  list_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  cout << "  List:   " << list_time << " µs (visited " << list_bfs.size() << " vertices)\n";

  start = std::chrono::high_resolution_clock::now();

  auto matrix_bfs = matrix_graph.bfs(0);

  end = std::chrono::high_resolution_clock::now();

  matrix_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  cout << "  Matrix: " << matrix_time << " µs (visited " << matrix_bfs.size() << " vertices)\n";
  cout << "  Winner: " << (matrix_time < list_time ? "Matrix" : "List") << '\n';

  // Test 3: Neighbor iteration.
  cout << "\n[Test 3] Iterating neighbors (first 100 vertices):\n";

  start = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < 100 && i < N; ++i) {
    auto neighbors = list_graph.get_neighbors(i);
    count          = count + neighbors.size();
  }
  end = std::chrono::high_resolution_clock::now();

  list_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  cout << "  List:   " << list_time << " µs (O(degree) iteration)\n";

  start = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < 100 && i < N; ++i) {
    auto neighbors = matrix_graph.get_neighbors(i);
    count          = count + neighbors.size();
  }
  end = std::chrono::high_resolution_clock::now();

  matrix_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  cout << "  Matrix: " << matrix_time << " µs (O(V) iteration)\n";
  cout << "  Winner: " << (matrix_time < list_time ? "Matrix" : "List") << '\n';

  // Memory usage estimation.
  cout << "\n[Memory Usage] Estimated for sparse graph:\n";
  size_t list_memory   = N * sizeof(int) + list_graph.num_edges() * 2 * (sizeof(size_t) + sizeof(double));
  size_t matrix_memory = N * sizeof(int) + N * N * sizeof(std::optional<double>);
  cout << "  List:   ~" << list_memory / 1024 << " KB (O(V + E))\n";
  cout << "  Matrix: ~" << matrix_memory / 1024 << " KB (O(V²))\n";
  cout << "  Winner: List (Matrix uses " << (100 * matrix_memory / list_memory) << "% more memory)\n";

  cout << "\nConclusion for sparse graphs:\n";
  cout << "  - Matrix: Better for O(1) edge lookup\n";
  cout << "  - List:   Better for memory efficiency and neighbor iteration\n";
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  cout << "╔═══----------------------------------------------------═══╗\n";
  cout << "          GRAPH ADJACENCY MATRIX - EXAMPLES TESTS           \n";
  cout << "╚═══----------------------------------------------------═══╝\n";

  try {
    // Basic tests.
    demo_matrix_construction();
    demo_matrix_operations();
    demo_matrix_traversal();
    demo_matrix_connected_components();

    // Performance comparison.
    compare_performance();

    cout << "\n";
    cout << "╔═══----------------------------------------------------═══╗\n";
    cout << "             ALL DEMOS COMPLETED SUCCESSFULLY!              \n";
    cout << "╚═══----------------------------------------------------═══╝\n";

  } catch (const exception& e) {
    cerr << "\nTest failed with exception: " << e.what() << '\n';
    return 1;
  }

  return 0;
}

//===---------------------------------------------------------------------------===//
