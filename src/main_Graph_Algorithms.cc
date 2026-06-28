//===---------------------------------------------------------------------------===//
/**
 * @file main_Graph_Algorithms.cc
 * @author Costantino Lombardi
 * @brief Demo for reusable graph algorithms.
 * @version 0.1
 * @date 2026-06-11
 *
 * @copyright MIT License 2026
 *
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/graphs/Graph_Algorithms.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>
using std::cerr;
using std::cout;
using std::exception;
using std::string;

using namespace ads::graphs;

namespace {

//===----- DEMO HELPERS --------------------------------------------------------===//

auto print_path(const std::optional<ads::arrays::DynamicArray<size_t>>& path) -> void {
  if (!path.has_value()) {
    cout << "unreachable\n";
    return;
  }

  for (size_t index = 0; index < path->size(); ++index) {
    cout << (*path)[index];
    if (index + 1 < path->size()) {
      cout << " -> ";
    }
  }
  cout << '\n';
}

//===----- ALGORITHM DEMOS -----------------------------------------------------===//

auto demo_bellman_ford() -> void {
  ads::demo::print_section("Bellman-Ford");

  GraphAdjacencyList<string, int> graph(true);
  for (const string& city : {"Rome", "Milan", "Paris", "Berlin", "Prague"}) {
    graph.add_vertex(city);
  }

  graph.add_edge(0, 1, 4);
  graph.add_edge(0, 2, 5);
  graph.add_edge(1, 2, -2);
  graph.add_edge(2, 3, 3);
  graph.add_edge(3, 4, 2);

  const auto result = bellman_ford(graph, 0);
  cout << "Distance Rome -> Prague: " << result.distance_to(4) << '\n';
  cout << "Path Rome -> Prague: ";
  print_path(result.path_to(4));
}

auto demo_prim() -> void {
  ads::demo::print_section("Prim Minimum Spanning Forest");

  GraphAdjacencyList<string, int> graph(false);
  for (const string& hub : {"A", "B", "C", "D", "E"}) {
    graph.add_vertex(hub);
  }

  graph.add_edge(0, 1, 4);
  graph.add_edge(0, 2, 3);
  graph.add_edge(1, 2, 1);
  graph.add_edge(1, 3, 2);
  graph.add_edge(2, 3, 4);
  graph.add_edge(3, 4, 2);

  const auto forest = prim(graph);
  cout << "Total weight: " << forest.total_weight() << '\n';
  cout << "Edges:\n";
  for (const auto& edge : forest.edges()) {
    cout << "  " << graph.get_vertex_data(edge.from) << " - " << graph.get_vertex_data(edge.to) << " (" << edge.weight
         << ")\n";
  }
}

auto demo_topological_sort() -> void {
  ads::demo::print_section("Topological Sort");

  GraphAdjacencyList<string, int> graph(true);
  for (const string& task : {"Parse", "Analyze", "Optimize", "Generate", "Package"}) {
    graph.add_vertex(task);
  }

  graph.add_edge(0, 1, 1);
  graph.add_edge(1, 2, 1);
  graph.add_edge(2, 3, 1);
  graph.add_edge(1, 4, 1);
  graph.add_edge(3, 4, 1);

  const auto order = topological_sort(graph);
  cout << "Execution order:\n";
  for (size_t vertex_id : order) {
    cout << "  - " << graph.get_vertex_data(vertex_id) << '\n';
  }
}

auto demo_kruskal() -> void {
  ads::demo::print_section("Kruskal Minimum Spanning Forest");

  GraphAdjacencyList<string, int> graph(false);
  for (const string& hub : {"Alpha", "Beta", "Gamma", "Delta", "Epsilon"}) {
    graph.add_vertex(hub);
  }

  graph.add_edge(0, 1, 10);
  graph.add_edge(0, 2, 6);
  graph.add_edge(0, 3, 5);
  graph.add_edge(1, 3, 15);
  graph.add_edge(2, 3, 4);
  graph.add_edge(3, 4, 2);

  const auto forest = kruskal(graph);
  cout << "Total weight: " << forest.total_weight() << '\n';
  cout << "Edges:\n";
  for (const auto& edge : forest.edges()) {
    cout << "  " << graph.get_vertex_data(edge.from) << " - " << graph.get_vertex_data(edge.to) << " (" << edge.weight
         << ")\n";
  }
}

auto demo_floyd_warshall() -> void {
  ads::demo::print_section("Floyd-Warshall");

  GraphAdjacencyMatrix<string, int> graph(true);
  for (const string& city : {"Rome", "Milan", "Paris", "Berlin"}) {
    graph.add_vertex(city);
  }

  graph.add_edge(0, 1, 5);
  graph.add_edge(0, 3, 10);
  graph.add_edge(1, 2, 3);
  graph.add_edge(2, 3, 1);

  const auto result = floyd_warshall(graph);
  cout << "Distance Rome -> Berlin: " << result.distance(0, 3) << '\n';
  cout << "Path Rome -> Berlin: ";
  print_path(result.path(0, 3));
}

auto demo_strongly_connected_components() -> void {
  ads::demo::print_section("Strongly Connected Components");

  GraphAdjacencyList<string, int> graph(true);
  for (const string& service : {"Gateway", "Auth", "Profile", "Search", "Index", "Cache"}) {
    graph.add_vertex(service);
  }

  graph.add_edge(0, 1, 1);
  graph.add_edge(1, 2, 1);
  graph.add_edge(2, 0, 1);
  graph.add_edge(2, 3, 1);
  graph.add_edge(3, 4, 1);
  graph.add_edge(4, 5, 1);
  graph.add_edge(5, 3, 1);

  const auto result = strongly_connected_components(graph);
  cout << "Component count: " << result.component_count() << '\n';
  for (size_t component_id = 0; component_id < result.component_count(); ++component_id) {
    cout << "  Component " << component_id << ": ";
    const auto& component = result.component(component_id);
    for (size_t index = 0; index < component.size(); ++index) {
      cout << graph.get_vertex_data(component[index]);
      if (index + 1 < component.size()) {
        cout << ", ";
      }
    }
    cout << '\n';
  }
}

} // namespace

//===----- MAIN FUNCTION -------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("GRAPH ALGORITHMS - COMPREHENSIVE DEMO");

    demo_bellman_ford();
    demo_prim();
    demo_kruskal();
    demo_floyd_warshall();
    demo_strongly_connected_components();
    demo_topological_sort();

    ads::demo::print_footer();
  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}

//===---------------------------------------------------------------------------===//
