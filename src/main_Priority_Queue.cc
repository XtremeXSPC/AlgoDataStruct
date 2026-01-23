//===---------------------------------------------------------------------------===//
/**
 * @file main_Priority_Queue.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for PriorityQueue class.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of the PriorityQueue data structure,
 * showcasing various operations such as insertion, extraction, and move semantics.
 */
//===---------------------------------------------------------------------------===//

#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "../include/ads/queues/Priority_Queue.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::to_string;
using std::vector;

using namespace ads::queues;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//


//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Priority Queue basic operations.
void demo_priority_queue_basic() {
  ads::demo::print_section("Priority Queue - Basic Operations (Max-Heap)");

  PriorityQueue<int> pq;

  cout << std::format("Empty queue created. empty(): {}\n", pq.empty());
  cout << "Size: " << pq.size() << '\n';

  cout << "\nPushing elements: 5, 3, 7, 1, 9, 2\n";
  pq.push(5);
  pq.push(3);
  pq.push(7);
  pq.push(1);
  pq.push(9);
  pq.push(2);

  cout << "Size: " << pq.size() << '\n';
  cout << "Top (max): " << pq.top() << '\n';

  cout << "\nExtracting all elements (should be sorted descending for max-heap):\n";
  while (!pq.empty()) {
    cout << pq.top() << ' ';
    pq.pop();
  }
  cout << '\n';
  cout << std::format("Queue is now empty: {}\n", pq.empty());
}

//===----------------------------- MIN HEAP DEMOS ------------------------------===//

// Min-Heap Priority Queue operations.
void demo_priority_queue_min_heap() {
  ads::demo::print_section("Priority Queue - Min-Heap with greater");

  PriorityQueue<int, std::greater<>> min_pq;

  cout << "Pushing elements: 5, 3, 7, 1, 9, 2\n";
  min_pq.push(5);
  min_pq.push(3);
  min_pq.push(7);
  min_pq.push(1);
  min_pq.push(9);
  min_pq.push(2);

  cout << "Top (min): " << min_pq.top() << '\n';

  cout << "\nExtracting all elements (should be sorted ascending for min-heap):\n";
  while (!min_pq.empty()) {
    cout << min_pq.top() << ' ';
    min_pq.pop();
  }
  cout << '\n';
}

//===---------------------- CONSTRUCTION FROM VECTOR DEMO ----------------------===//

// Priority Queue construction from vector.
void demo_priority_queue_from_vector() {
  ads::demo::print_section("Priority Queue - Construction from Vector");

  vector<int> data = {15, 10, 20, 8, 12, 25, 18};
  cout << "Original vector: ";
  for (int val : data) {
    cout << val << ' ';
  }
  cout << '\n';

  PriorityQueue<int> pq(data);
  cout << "Queue size: " << pq.size() << '\n';
  cout << "Top (max): " << pq.top() << '\n';

  cout << "Extracting all elements:\n";
  while (!pq.empty()) {
    cout << pq.top() << ' ';
    pq.pop();
  }
  cout << '\n';
}

//===------------------- INITIALIZER LIST CONSTRUCTION DEMO --------------------===//

// Priority Queue construction from initializer list.
void demo_priority_queue_initializer_list() {
  ads::demo::print_section("Priority Queue - Initializer List Construction");

  PriorityQueue<int> pq = {3, 1, 4, 1, 5, 9, 2, 6};

  cout << "Created from initializer list: {3, 1, 4, 1, 5, 9, 2, 6}\n";
  cout << "Size: " << pq.size() << ", Top: " << pq.top() << '\n';

  cout << "All elements: ";
  while (!pq.empty()) {
    cout << pq.top() << ' ';
    pq.pop();
  }
  cout << '\n';
}

//===--------------------------- MOVE SEMANTICS DEMO ---------------------------===//

// Priority Queue move semantics.
void demo_priority_queue_move_semantics() {
  ads::demo::print_section("Priority Queue - Move Semantics");

  PriorityQueue<int> pq1;
  for (int i = 1; i <= 5; ++i) {
    pq1.push(i * 10);
  }

  cout << "PQ1 size: " << pq1.size() << ", top: " << pq1.top() << '\n';

  cout << "\nMoving pq1 to pq2 (move constructor)...\n";
  PriorityQueue<int> pq2(std::move(pq1));

  cout << "PQ2 size: " << pq2.size() << ", top: " << pq2.top() << '\n';
  // Note: pq1 is in a valid but unspecified state after move.

  PriorityQueue<int> pq3;
  pq3.push(99);

  cout << "\nMoving pq2 to pq3 (move assignment)...\n";
  pq3 = std::move(pq2);

  cout << "PQ3 size: " << pq3.size() << ", top: " << pq3.top() << '\n';
  // Note: pq2 is in a valid but unspecified state after move.
}

//===------------------------- EMPLACE OPERATIONS DEMO -------------------------===//

// Priority Queue emplace operations.
void demo_priority_queue_emplace() {
  ads::demo::print_section("Priority Queue - Emplace Operations");

  PriorityQueue<string> pq;

  pq.emplace("World");
  pq.emplace("Hello");
  pq.emplace("Test");
  pq.emplace("Algorithms");
  pq.emplace("Data");

  cout << "Queue size: " << pq.size() << '\n';
  cout << "Top: " << pq.top() << '\n';

  cout << "All strings in priority order:\n";
  while (!pq.empty()) {
    cout << pq.top() << '\n';
    pq.pop();
  }
}

//===------------------------- EXCEPTION HANDLING DEMO -------------------------===//

// Priority Queue exception handling.
void demo_priority_queue_exception_handling() {
  ads::demo::print_section("Priority Queue - Exception Handling");

  PriorityQueue<int> pq;

  try {
    pq.top();
    cout << "ERROR: top() should throw on empty queue\n";
  } catch (const QueueException& e) {
    cout << "Caught expected exception for top(): " << e.what() << '\n';
  }

  try {
    pq.pop();
    cout << "ERROR: pop() should throw on empty queue\n";
  } catch (const QueueException& e) {
    cout << "Caught expected exception for pop(): " << e.what() << '\n';
  }
}

//===--------------------- SORTED ELEMENTS EXTRACTION DEMO ---------------------===//

// Priority Queue sorted elements extraction.
void demo_priority_queue_sorted_elements() {
  ads::demo::print_section("Priority Queue - Sorted Elements Extraction");

  PriorityQueue<int> pq = {8, 3, 10, 1, 6, 14, 4, 7, 13};

  cout << "Original queue size: " << pq.size() << '\n';
  cout << "Extracting all elements in sorted order:\n";

  auto sorted = pq.sorted_elements();

  cout << "Sorted (descending): ";
  for (int val : sorted) {
    cout << val << ' ';
  }
  cout << '\n';

  cout << std::format("Queue is now empty: {}\n", pq.empty());
}

//===---------------------------- APPLICATION DEMOS ----------------------------===//

// Task struct for scheduling demo.
struct Task {
  string name;
  int    priority;

  Task(string n, int p) : name(std::move(n)), priority(p) {}
};

// Comparator for tasks (higher priority number = higher priority).
struct TaskCompare {
  auto operator()(const Task& a, const Task& b) const -> bool { return a.priority < b.priority; }
};

//===-------------------------- TASK SCHEDULING DEMO ---------------------------===//

// Task scheduling application demo.
void demo_task_scheduling() {
  ads::demo::print_section("Application - Task Scheduling");

  PriorityQueue<Task, TaskCompare> task_queue;

  // Add tasks with different priorities.
  task_queue.emplace("Send email", 2);
  task_queue.emplace("Critical bug fix", 10);
  task_queue.emplace("Coffee break", 1);
  task_queue.emplace("Code review", 5);
  task_queue.emplace("Deploy to production", 9);
  task_queue.emplace("Update documentation", 3);

  cout << "Tasks in execution order (by priority):\n";
  int task_number = 1;
  while (!task_queue.empty()) {
    const Task& task = task_queue.top();
    cout << task_number << ". [Priority " << task.priority << "] " << task.name << '\n';
    ++task_number;
    task_queue.pop();
  }
}

//===-------------------------- EVENT SIMULATION DEMO --------------------------===//

// Event struct for simulation demo.
struct Event {
  string name;
  double timestamp;

  Event(string n, double t) : name(std::move(n)), timestamp(t) {}
};

// Comparator for events (earlier timestamp = higher priority).
struct EventCompare {
  auto operator()(const Event& a, const Event& b) const -> bool { return a.timestamp > b.timestamp; }
};

// Event simulation application demo.
void demo_event_simulation() {
  ads::demo::print_section("Application - Event Simulation");

  PriorityQueue<Event, EventCompare> event_queue;

  // Add events with timestamps (min-heap: earliest events first).
  event_queue.emplace("System startup", 0.0);
  event_queue.emplace("User login", 2.5);
  event_queue.emplace("Database query", 3.1);
  event_queue.emplace("Network timeout", 5.0);
  event_queue.emplace("Cache invalidation", 1.8);
  event_queue.emplace("Request received", 0.5);

  cout << "Events in chronological order:\n";
  while (!event_queue.empty()) {
    const Event& event = event_queue.top();
    cout << "[t=" << event.timestamp << "s] " << event.name << '\n';
    event_queue.pop();
  }
}

//===--------------------------- TOP-K ELEMENTS DEMO ---------------------------===//

// Top-K largest elements application demo.
void demo_top_k_elements() {
  ads::demo::print_section("Application - Top-K Largest Elements");

  // Find top 5 largest elements from a stream using a min-heap of size 5.
  const int                          k = 5;
  PriorityQueue<int, std::greater<>> min_heap; // Min-heap

  vector<int> stream = {12, 5, 787, 1, 23, 100, 34, 56, 89, 45, 678, 234, 98, 345, 567};

  cout << "Stream: ";
  for (int val : stream) {
    cout << val << ' ';
  }
  cout << '\n';

  cout << "\nFinding top " << k << " largest elements...\n";

  for (int val : stream) {
    if (min_heap.size() < static_cast<size_t>(k)) {
      min_heap.push(val);
    } else if (val > min_heap.top()) {
      min_heap.pop();
      min_heap.push(val);
    }
  }

  cout << "Top " << k << " largest elements (ascending order):\n";
  auto result = min_heap.sorted_elements();
  for (int val : result) {
    cout << val << ' ';
  }
  cout << '\n';
}

//===------------------------ LARGE DATASET PERFORMANCE ------------------------===//

// Priority Queue performance with large dataset.
void demo_priority_queue_large() {
  ads::demo::print_section("Priority Queue - Large Dataset Performance");

  const int          N = 100000;
  PriorityQueue<int> pq;
  pq.reserve(N);

  cout << "Inserting " << N << " elements...\n";
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = N; i > 0; --i) {
    pq.push(i);
  }

  auto end      = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "Insertion time: " << duration.count() << " ms\n";
  cout << "Queue size: " << pq.size() << '\n';
  cout << "Top element: " << pq.top() << '\n';

  cout << "\nExtracting all elements...\n";
  start = std::chrono::high_resolution_clock::now();

  int count = 0;
  while (!pq.empty()) {
    pq.pop();
    ++count;
  }

  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "Extraction time: " << duration.count() << " ms\n";
  cout << "Elements extracted: " << count << '\n';
}

//===-------------------- HEAPIFY CONSTRUCTION PERFORMANCE ---------------------===//

// Heapify construction performance demo.
void demo_heapify_construction_performance() {
  ads::demo::print_section("Priority Queue - Heapify Construction Performance");

  const int   N = 100000;
  vector<int> data;
  data.reserve(N);

  for (int i = 0; i < N; ++i) {
    data.push_back(i);
  }

  cout << "Constructing priority queue from vector of " << N << " elements...\n";
  auto start = std::chrono::high_resolution_clock::now();

  PriorityQueue<int> pq(data);

  auto end      = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "Heapify construction time: " << duration.count() << " ms\n";
  cout << "Queue size: " << pq.size() << ", Top: " << pq.top() << '\n';
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  ads::demo::print_header("PRIORITY QUEUE - COMPREHENSIVE DEMO");

  try {
    // Basic tests.
    demo_priority_queue_basic();
    demo_priority_queue_min_heap();
    demo_priority_queue_from_vector();
    demo_priority_queue_initializer_list();
    demo_priority_queue_move_semantics();
    demo_priority_queue_emplace();
    demo_priority_queue_exception_handling();
    demo_priority_queue_sorted_elements();

    // Practical applications.
    demo_task_scheduling();
    demo_event_simulation();
    demo_top_k_elements();

    // Performance tests.
    demo_priority_queue_large();
    demo_heapify_construction_performance();

    ads::demo::print_footer();

  } catch (const exception& e) {
    cerr << "\nTest failed with exception: " << e.what() << '\n';
    return 1;
  }

  return 0;
}

//===---------------------------------------------------------------------------===//
