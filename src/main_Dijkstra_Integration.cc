//===--------------------------------------------------------------------------===//
/**
 * @file main_Dijkstra_Integration.cc
 * @author Costantino Lombardi
 * @brief Integration test: Dijkstra's algorithm using Graph + PriorityQueue
 * @version 0.1
 * @date 2025-11-20
 *
 * @copyright MIT License 2025
 */
//===--------------------------------------------------------------------------===//

#include <chrono>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "../include/ads/graphs/Graph_Adjacency_List.hpp"
#include "../include/ads/queues/Priority_Queue.hpp"

using namespace ads::graph;
using namespace ads::queue;

// City graph vertex data
struct City {
  std::string name;
  City() : name("") {}
  explicit City(const std::string& n) : name(n) {}
};

// Compare nodes by distance for priority queue (min-heap)
struct DistanceComparator {
  bool operator()(const std::pair<size_t, double>& a, const std::pair<size_t, double>& b) const {
    return a.second > b.second; // Min-heap: smaller distance has higher priority
  }
};

/**
 * @brief Dijkstra's shortest path algorithm
 * @param graph The graph to search
 * @param start Starting vertex index
 * @return Vector of shortest distances from start to all vertices
 */
std::vector<double> dijkstra(const GraphAdjacencyList<City, double>& graph, size_t start) {
  size_t              n = graph.num_vertices();
  std::vector<double> dist(n, std::numeric_limits<double>::infinity());
  std::vector<bool>   visited(n, false);

  dist[start] = 0.0;

  // Priority queue: pair<vertex_index, distance>
  PriorityQueue<std::pair<size_t, double>, DistanceComparator> pq;
  pq.push({start, 0.0});

  while (!pq.empty()) {
    auto [u, d] = pq.top();
    pq.pop();

    if (visited[u]) {
      continue; // Already processed this vertex
    }
    visited[u] = true;

    // Relax edges from u
    for (const auto& [neighbor, weight] : graph.get_neighbors_with_weights(u)) {
      double new_dist = dist[u] + weight;

      if (new_dist < dist[neighbor]) {
        dist[neighbor] = new_dist;
        pq.push({neighbor, new_dist});
      }
    }
  }

  return dist;
}

/**
 * @brief Print shortest paths from source to all cities
 */
void print_shortest_paths(const GraphAdjacencyList<City, double>& graph, size_t source,
                          const std::vector<double>& distances) {
  std::cout << "\nShortest paths from " << graph.get_vertex_data(source).name << ":\n";
  std::cout << "===========================================\n";

  for (size_t i = 0; i < distances.size(); ++i) {
    std::cout << "  To " << graph.get_vertex_data(i).name << ": ";
    if (distances[i] == std::numeric_limits<double>::infinity()) {
      std::cout << "unreachable\n";
    } else {
      std::cout << distances[i] << " km\n";
    }
  }
}

int main() {
  std::cout << "╔═══════════════════════════════════════════════════════╗\n";
  std::cout << "║   DIJKSTRA'S ALGORITHM - INTEGRATION TEST             ║\n";
  std::cout << "║   Graph (Adjacency List) + Priority Queue            ║\n";
  std::cout << "╚═══════════════════════════════════════════════════════╝\n\n";

  // Create a graph of European cities with distances in km
  GraphAdjacencyList<City, double> cities(false); // Undirected graph

  // Add cities
  size_t rome   = cities.add_vertex(City("Rome"));
  size_t milan  = cities.add_vertex(City("Milan"));
  size_t paris  = cities.add_vertex(City("Paris"));
  size_t berlin = cities.add_vertex(City("Berlin"));
  size_t munich = cities.add_vertex(City("Munich"));
  size_t vienna = cities.add_vertex(City("Vienna"));
  size_t zurich = cities.add_vertex(City("Zurich"));

  // Add roads (edges) with distances
  cities.add_edge(rome, milan, 572);   // Rome - Milan
  cities.add_edge(milan, paris, 851);  // Milan - Paris
  cities.add_edge(milan, zurich, 277); // Milan - Zurich
  cities.add_edge(paris, berlin, 1054);// Paris - Berlin
  cities.add_edge(berlin, munich, 585);// Berlin - Munich
  cities.add_edge(munich, vienna, 434);// Munich - Vienna
  cities.add_edge(munich, zurich, 316);// Munich - Zurich
  cities.add_edge(vienna, zurich, 598);// Vienna - Zurich

  std::cout << "European Cities Road Network:\n";
  std::cout << "-----------------------------\n";
  std::cout << "Vertices: " << cities.num_vertices() << "\n";
  std::cout << "Edges: " << cities.num_edges() << "\n\n";

  // Test Dijkstra from different starting cities
  const std::vector<std::string> test_cities = {"Rome", "Paris", "Berlin"};
  const std::vector<size_t>      test_indices = {rome, paris, berlin};

  for (size_t i = 0; i < test_cities.size(); ++i) {
    std::cout << "\n" << std::string(55, '=') << "\n";
    std::cout << "Computing shortest paths from " << test_cities[i] << "...\n";

    auto distances = dijkstra(cities, test_indices[i]);
    print_shortest_paths(cities, test_indices[i], distances);
  }

  // Performance test with larger graph
  std::cout << "\n\n" << std::string(55, '=') << "\n";
  std::cout << "Performance Test: Random Graph\n";
  std::cout << std::string(55, '=') << "\n";

  const size_t num_vertices = 1000;
  GraphAdjacencyList<City, double> large_graph(false);

  // Add vertices
  for (size_t i = 0; i < num_vertices; ++i) {
    large_graph.add_vertex(City("City_" + std::to_string(i)));
  }

  // Add random edges
  for (size_t i = 0; i < num_vertices; ++i) {
    // Connect to next 5 vertices (circular)
    for (size_t j = 1; j <= 5 && i + j < num_vertices; ++j) {
      large_graph.add_edge(i, i + j, static_cast<double>(j * 10));
    }
  }

  std::cout << "\nGraph size: " << large_graph.num_vertices() << " vertices, "
            << large_graph.num_edges() << " edges\n";

  std::cout << "Running Dijkstra from vertex 0...\n";
  auto start_time = std::chrono::high_resolution_clock::now();
  auto distances  = dijkstra(large_graph, 0);
  auto end_time   = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  std::cout << "Completed in " << duration.count() / 1000.0 << " ms\n";
  std::cout << "Sample distances:\n";
  std::cout << "  To vertex 10: " << distances[10] << "\n";
  std::cout << "  To vertex 100: " << distances[100] << "\n";
  std::cout << "  To vertex 500: " << distances[500] << "\n";
  std::cout << "  To vertex 999: " << distances[999] << "\n";

  std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
  std::cout << "║       INTEGRATION TEST COMPLETED SUCCESSFULLY!        ║\n";
  std::cout << "╚═══════════════════════════════════════════════════════╝\n";

  return 0;
}
//===--------------------------------------------------------------------------===//
