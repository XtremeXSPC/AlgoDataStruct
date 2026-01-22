//===---------------------------------------------------------------------------===//
/**
 * @file main_Graph_Adjacency_List.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for GraphAdjacencyList class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the GraphAdjacencyList data structure,
 * showcasing its construction, vertex/edge management, and traversal algorithms.
 */
//===---------------------------------------------------------------------------===//

#include <chrono>
#include <format>
#include <iostream>
#include <string>

#include "../include/ads/graphs/Graph_Adjacency_List.hpp"

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

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Test construction and basic properties.
void demo_graph_construction() {
  print_separator("Graph - Construction and Basic Properties");

  // Undirected graph.
  GraphAdjacencyList<int> g_undirected(false);
  cout << "Created undirected graph\n";
  cout << std::format("Empty: {}\n", g_undirected.is_empty());
  cout << std::format("Directed: {}\n", g_undirected.is_directed());
  cout << "Vertices: " << g_undirected.num_vertices() << '\n';
  cout << "Edges: " << g_undirected.num_edges() << '\n';

  // Directed graph.
  GraphAdjacencyList<int> g_directed(true);
  cout << "\nCreated directed graph\n";
  cout << "Directed: " << g_directed.is_directed() << '\n';
}

//===----------------------- VERTEX AND EDGE OPERATIONS ------------------------===//

// Test adding vertices.
void demo_graph_add_vertices() {
  print_separator("Graph - Adding Vertices");

  GraphAdjacencyList<string> graph(false);

  auto v0 = graph.add_vertex("A");
  auto v1 = graph.add_vertex("B");
  auto v2 = graph.add_vertex("C");
  auto v3 = graph.add_vertex("D");

  cout << "Added 4 vertices: A, B, C, D\n";
  cout << "Number of vertices: " << graph.num_vertices() << '\n';
  cout << "Vertex 0 data: " << graph.get_vertex_data(v0) << '\n';
  cout << "Vertex 1 data: " << graph.get_vertex_data(v1) << '\n';
  cout << "Vertex 2 data: " << graph.get_vertex_data(v2) << '\n';
  cout << "Vertex 3 data: " << graph.get_vertex_data(v3) << '\n';

  cout << std::format("\nHas vertex 0: {}\n", graph.has_vertex(v0));
  cout << std::format("Has vertex 10: {}\n", graph.has_vertex(10));
}

// Test adding edges in undirected graph.
void demo_graph_add_edges_undirected() {
  print_separator("Graph - Adding Edges (Undirected)");

  GraphAdjacencyList<int> graph(false);

  // Create a simple graph:
  //   0 --- 1
  //   |     |
  //   2 --- 3

  auto v0 = graph.add_vertex(0);
  auto v1 = graph.add_vertex(1);
  auto v2 = graph.add_vertex(2);
  auto v3 = graph.add_vertex(3);

  graph.add_edge(v0, v1, 1.0);
  graph.add_edge(v0, v2, 2.0);
  graph.add_edge(v1, v3, 3.0);
  graph.add_edge(v2, v3, 4.0);

  cout << "Created graph with 4 vertices and 4 edges\n";
  cout << "Number of edges: " << graph.num_edges() << '\n';

  cout << "\nEdge checks:\n";
  cout << std::format("Has edge 0->1: {}\n", graph.has_edge(v0, v1));
  cout << std::format("Has edge 1->0: {} (undirected)\n", graph.has_edge(v1, v0));
  cout << std::format("Has edge 0->3: {}\n", graph.has_edge(v0, v3));

  cout << "\nEdge weights:\n";
  auto w1 = graph.get_edge_weight(v0, v1);
  cout << "Weight 0->1: " << (w1 ? to_string(*w1) : "none") << '\n';
  auto w2 = graph.get_edge_weight(v2, v3);
  cout << "Weight 2->3: " << (w2 ? to_string(*w2) : "none") << '\n';

  cout << "\nVertex degrees:\n";
  for (size_t i = 0; i < graph.num_vertices(); ++i) {
    cout << "Vertex " << i << " degree: " << graph.degree(i) << '\n';
  }
}

// Test adding edges in directed graph.
void demo_graph_add_edges_directed() {
  print_separator("Graph - Adding Edges (Directed)");

  GraphAdjacencyList<int> graph(true);

  // Create a directed graph:
  //   0 --> 1
  //   |     |
  //   v     v
  //   2 --> 3

  auto v0 = graph.add_vertex(0);
  auto v1 = graph.add_vertex(1);
  auto v2 = graph.add_vertex(2);
  auto v3 = graph.add_vertex(3);

  graph.add_edge(v0, v1);
  graph.add_edge(v0, v2);
  graph.add_edge(v1, v3);
  graph.add_edge(v2, v3);

  cout << "Created directed graph with 4 vertices and 4 edges\n";
  cout << "Number of edges: " << graph.num_edges() << '\n';

  cout << "\nEdge checks (directed):\n";
  cout << std::format("Has edge 0->1: {}\n", graph.has_edge(v0, v1));
  cout << std::format("Has edge 1->0: {} (reverse)\n", graph.has_edge(v1, v0));
  cout << std::format("Has edge 1->3: {}\n", graph.has_edge(v1, v3));
  cout << std::format("Has edge 3->1: {} (reverse)\n", graph.has_edge(v3, v1));
}

//===----------------------- NEIGHBORS AND REMOVAL DEMO ------------------------===//

// Test getting neighbors.
void demo_graph_neighbors() {
  print_separator("Graph - Getting Neighbors");

  GraphAdjacencyList<char> graph(false);

  auto v0 = graph.add_vertex('A');
  auto v1 = graph.add_vertex('B');
  auto v2 = graph.add_vertex('C');
  auto v3 = graph.add_vertex('D');

  graph.add_edge(v0, v1, 1.5);
  graph.add_edge(v0, v2, 2.5);
  graph.add_edge(v1, v3, 3.5);

  cout << "Graph structure:\n";
  for (size_t i = 0; i < graph.num_vertices(); ++i) {
    cout << graph.get_vertex_data(i) << " -> ";
    auto neighbors = graph.get_neighbors(i);
    for (auto n : neighbors) {
      cout << graph.get_vertex_data(n) << " ";
    }
    cout << '\n';
  }

  cout << "\nNeighbors with weights for vertex A:\n";
  auto neighbors_weighted = graph.get_neighbors_with_weights(v0);
  for (const auto& [neighbor, weight] : neighbors_weighted) {
    cout << graph.get_vertex_data(neighbor) << " (weight: " << weight << ")\n";
  }
}

//===------------------------- REMOVAL OPERATIONS DEMO -------------------------===//

// Test removing edges.
void demo_graph_remove_edge() {
  print_separator("Graph - Removing Edges");

  GraphAdjacencyList<int> graph(false);

  auto v0 = graph.add_vertex(0);
  auto v1 = graph.add_vertex(1);
  auto v2 = graph.add_vertex(2);

  graph.add_edge(v0, v1);
  graph.add_edge(v1, v2);
  graph.add_edge(v0, v2);

  cout << "Initial graph: " << graph.num_edges() << " edges\n";
  cout << std::format("Has edge 0->1: {}\n", graph.has_edge(v0, v1));

  graph.remove_edge(v0, v1);
  cout << "\nAfter removing edge 0->1:\n";
  cout << "Number of edges: " << graph.num_edges() << '\n';
  cout << std::format("Has edge 0->1: {}\n", graph.has_edge(v0, v1));
  cout << std::format("Has edge 1->0: {} (undirected)\n", graph.has_edge(v1, v0));
}

//===------------------------- REMOVAL OPERATIONS DEMO -------------------------===//

// Test removing vertices.
void demo_graph_bfs() {
  print_separator("Graph - Breadth-First Search (BFS)");

  GraphAdjacencyList<int> graph(false);

  // Create a more complex graph:
  //     0 ---- 1 ---- 2
  //     |      |      |
  //     3 ---- 4      5

  auto v0 = graph.add_vertex(0);
  auto v1 = graph.add_vertex(1);
  auto v2 = graph.add_vertex(2);
  auto v3 = graph.add_vertex(3);
  auto v4 = graph.add_vertex(4);
  auto v5 = graph.add_vertex(5);

  graph.add_edge(v0, v1);
  graph.add_edge(v0, v3);
  graph.add_edge(v1, v2);
  graph.add_edge(v1, v4);
  graph.add_edge(v2, v5);
  graph.add_edge(v3, v4);

  cout << "Graph structure created with 6 vertices\n";

  cout << "\nBFS from vertex 0:\n";
  auto bfs_result = graph.bfs(v0);
  print_vector(bfs_result, "Traversal order: ");

  cout << "\nBFS from vertex 2:\n";
  bfs_result = graph.bfs(v2);
  print_vector(bfs_result, "Traversal order: ");
}

//===------------------------- DEPTH-FIRST SEARCH DEMO -------------------------===//

// Test depth-first search.
void demo_graph_dfs() {
  print_separator("Graph - Depth-First Search (DFS)");

  GraphAdjacencyList<int> graph(false);

  // Same graph as BFS test.
  auto v0 = graph.add_vertex(0);
  auto v1 = graph.add_vertex(1);
  auto v2 = graph.add_vertex(2);
  auto v3 = graph.add_vertex(3);
  auto v4 = graph.add_vertex(4);
  auto v5 = graph.add_vertex(5);

  graph.add_edge(v0, v1);
  graph.add_edge(v0, v3);
  graph.add_edge(v1, v2);
  graph.add_edge(v1, v4);
  graph.add_edge(v2, v5);
  graph.add_edge(v3, v4);

  cout << "Graph structure created with 6 vertices\n";

  cout << "\nDFS from vertex 0:\n";
  auto dfs_result = graph.dfs(v0);
  print_vector(dfs_result, "Traversal order: ");

  cout << "\nDFS from vertex 2:\n";
  dfs_result = graph.dfs(v2);
  print_vector(dfs_result, "Traversal order: ");
}

//===---------------------------- PATH FINDING DEMO ----------------------------===//

// Test path finding.
void demo_graph_path_finding() {
  print_separator("Graph - Path Finding");

  GraphAdjacencyList<string> graph(false);

  // Create a graph representing cities.
  auto london    = graph.add_vertex("London");
  auto paris     = graph.add_vertex("Paris");
  auto berlin    = graph.add_vertex("Berlin");
  auto rome      = graph.add_vertex("Rome");
  auto madrid    = graph.add_vertex("Madrid");
  auto amsterdam = graph.add_vertex("Amsterdam");

  // Add connections.
  graph.add_edge(london, paris);
  graph.add_edge(london, amsterdam);
  graph.add_edge(paris, berlin);
  graph.add_edge(berlin, rome);
  graph.add_edge(madrid, paris);
  graph.add_edge(amsterdam, berlin);

  cout << "Finding path from London to Rome:\n";
  auto path = graph.find_path(london, rome);
  if (path) {
    cout << "Path found: ";
    for (size_t vertex_id : *path) {
      cout << graph.get_vertex_data(vertex_id);
      if (vertex_id != path->back())
        cout << " -> ";
    }
    cout << '\n';
  } else {
    cout << "No path found\n";
  }

  cout << "\nFinding path from Madrid to Amsterdam:\n";
  path = graph.find_path(madrid, amsterdam);
  if (path) {
    cout << "Path found: ";
    for (size_t vertex_id : *path) {
      cout << graph.get_vertex_data(vertex_id);
      if (vertex_id != path->back())
        cout << " -> ";
    }
    cout << '\n';
  } else {
    cout << "No path found\n";
  }

  cout << "\nConnectivity checks:\n";
  cout << std::format("London connected to Rome: {}\n", graph.is_connected(london, rome));
  cout << std::format("Madrid connected to Berlin: {}\n", graph.is_connected(madrid, berlin));
}

//===------------------------ CONNECTED COMPONENTS DEMO ------------------------===//

// Test connected components.
void demo_graph_connected_components() {
  print_separator("Graph - Connected Components");

  GraphAdjacencyList<int> graph(false);

  // Create graph with multiple components:
  // Component 1: 0-1-2
  // Component 2: 3-4
  // Component 3: 5

  for (int i = 0; i < 6; ++i) {
    graph.add_vertex(i);
  }

  graph.add_edge(0, 1);
  graph.add_edge(1, 2);
  graph.add_edge(3, 4);

  cout << "Graph with 6 vertices and 3 edges\n";
  cout << "Structure: 0-1-2, 3-4, 5 (isolated)\n";

  auto components = graph.connected_components();
  cout << "\nNumber of connected components: " << components.size() << '\n';

  for (size_t i = 0; i < components.size(); ++i) {
    cout << "Component " << i + 1 << ": ";
    print_vector(components[i]);
  }
}

//===--------------------------- MOVE SEMANTICS DEMO ---------------------------===//

// Test move semantics.
void demo_graph_move_semantics() {
  print_separator("Graph - Move Semantics");

  GraphAdjacencyList<int> graph1(false);
  graph1.add_vertex(0);
  graph1.add_vertex(1);
  graph1.add_vertex(2);
  graph1.add_edge(0, 1);
  graph1.add_edge(1, 2);

  cout << "Graph1: " << graph1.num_vertices() << " vertices, " << graph1.num_edges() << " edges\n";

  cout << "\nMoving graph1 to graph2 (move constructor)...\n";
  GraphAdjacencyList<int> graph2(std::move(graph1));

  cout << "Graph2: " << graph2.num_vertices() << " vertices, " << graph2.num_edges() << " edges\n";
  cout << "Graph1 after move: (moved-from state)\n";

  GraphAdjacencyList<int> graph3(false);
  cout << "\nMoving graph2 to graph3 (move assignment)...\n";
  graph3 = std::move(graph2);

  cout << "Graph3: " << graph3.num_vertices() << " vertices, " << graph3.num_edges() << " edges\n";
}

//===------------------------- EXCEPTION HANDLING DEMO -------------------------===//

// Test exception handling.
void demo_graph_exception_handling() {
  print_separator("Graph - Exception Handling");

  GraphAdjacencyList<int> graph(false);
  graph.add_vertex(0);
  graph.add_vertex(1);

  try {
    graph.get_vertex_data(10);
    cout << "ERROR: Should have thrown exception for invalid vertex\n";
  } catch (const GraphException& e) {
    cout << "Caught expected exception: " << e.what() << '\n';
  }

  try {
    graph.add_edge(0, 10);
    cout << "ERROR: Should have thrown exception for invalid edge\n";
  } catch (const GraphException& e) {
    cout << "Caught expected exception: " << e.what() << '\n';
  }

  try {
    auto result = graph.bfs(10);
    cout << "ERROR: Should have thrown exception for invalid start vertex\n";
  } catch (const GraphException& e) {
    cout << "Caught expected exception: " << e.what() << '\n';
  }
}

//===---------------------------- PERFORMANCE DEMO -----------------------------===//

// Test performance on large graph.
void demo_graph_large_performance() {
  print_separator("Graph - Large Dataset Performance");

  const size_t            N = 10000;
  GraphAdjacencyList<int> graph(false);

  cout << "Creating graph with " << N << " vertices...\n";
  auto start = std::chrono::high_resolution_clock::now();

  for (size_t i = 0; i < N; ++i) {
    graph.add_vertex(static_cast<int>(i));
  }

  auto end      = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  cout << "Vertex creation time: " << duration.count() << " ms\n";

  cout << "\nAdding " << N * 5 << " edges (creating a sparse graph)...\n";
  start = std::chrono::high_resolution_clock::now();

  for (size_t i = 0; i < N; ++i) {
    // Connect each vertex to 5 neighbors.
    for (size_t j = 1; j <= 5; ++j) {
      if (i + j < N) {
        graph.add_edge(i, i + j);
      }
    }
  }

  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  cout << "Edge creation time: " << duration.count() << " ms\n";
  cout << "Total edges: " << graph.num_edges() << '\n';

  cout << "\nPerforming BFS from vertex 0...\n";
  start = std::chrono::high_resolution_clock::now();

  auto bfs_result = graph.bfs(0);

  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  cout << "BFS time: " << duration.count() << " ms\n";
  cout << "Vertices visited: " << bfs_result.size() << '\n';

  cout << "\nPerforming DFS from vertex 0...\n";
  start = std::chrono::high_resolution_clock::now();

  auto dfs_result = graph.dfs(0);

  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  cout << "DFS time: " << duration.count() << " ms\n";
  cout << "Vertices visited: " << dfs_result.size() << '\n';
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  cout << "╔═══----------------------------------------------------═══╗\n";
  cout << "        GRAPH ADJACENCY LIST - EXAMPLES FOR TESTING         \n";
  cout << "╚═══----------------------------------------------------═══╝\n";

  try {
    // Basic tests.
    demo_graph_construction();
    demo_graph_add_vertices();
    demo_graph_add_edges_undirected();
    demo_graph_add_edges_directed();
    demo_graph_neighbors();
    demo_graph_remove_edge();

    // Traversal algorithm tests.
    demo_graph_bfs();
    demo_graph_dfs();
    demo_graph_path_finding();
    demo_graph_connected_components();

    // Advanced tests.
    demo_graph_move_semantics();
    demo_graph_exception_handling();

    // Performance tests.
    demo_graph_large_performance();

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

//===--------------------------------------------------------------------------===//
