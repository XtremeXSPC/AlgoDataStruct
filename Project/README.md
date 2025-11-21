## C++ Implementation Status

The following data structures are currently implemented in C++ with full tests and documentation:

### Phase 1 - Linear Structures & Basic Heaps

- **Doubly Linked List** - Bidirectional list with iterators
- **Singly Linked List** - Forward-only list with forward iterators
- **Stack** - Array-based and Linked implementations
- **Queue** - Circular Array and Linked implementations
- **Min Heap** - Array-based binary min heap
- **Max Heap** - Array-based binary max heap

### Phase 2 - Balanced Trees & Hash Tables

- **AVL Tree** - Self-balancing BST with all 4 rotations (LL, RR, LR, RL)
  - O(log n) guaranteed for insert/remove/search
  - Height: ~1.44*log(n) vs BST: O(n) for sorted input
  - Comprehensive test suite with rotation verification

- **Hash Table (Chaining)** - Collision resolution via linked lists
  - O(1) average time for insert/find/erase
  - Dynamic rehashing with configurable load factor
  - Move semantics and exception handling

- **Hash Table (Open Addressing)** - Direct slot probing
  - Three probing strategies: Linear, Quadratic, Double Hashing
  - Tombstone handling for proper deletion
  - 2.4x faster than chaining for sequential access
  - Superior cache locality

### Phase 3 - Graphs, Priority Queues & Associative Containers

- **Graph (Adjacency List)** - Dynamic graph with template vertex data and edge weights
  - O(1) add vertex, O(E) add edge
  - BFS and DFS traversals with path reconstruction
  - Neighbor queries with/without weights
  - Support for directed and undirected graphs

- **Graph (Adjacency Matrix)** - Dense graph representation
  - O(1) edge lookup and modification
  - O(V²) space complexity
  - Superior performance for dense graphs
  - Complete graph operations (transpose, degree, etc.)

- **Priority Queue** - Heap-based priority queue with custom comparators
  - O(log n) push/pop, O(1) top
  - Min-heap and max-heap support via comparator
  - Template-based for any comparable type

- **HashMap** - STL-compatible hash map with iterators
  - Built on HashTableChaining with std::pair<const Key, Value>
  - Full iterator support (forward iterators)
  - Range-based for loops and structured bindings
  - Utility methods: keys(), values(), entries()
  - Initializer list constructor

- **Dijkstra Integration Test** - Real-world graph algorithm demonstration
  - Uses Graph + PriorityQueue together
  - European cities road network example
  - Performance test on 1000-vertex graph (< 1ms)

### Trees (Classic)

- **Binary Search Tree** - Classic BST with in-order iterators

**All implemented structures include:**

- Comprehensive test suites
- Memory leak verification (valgrind)
- Move semantics support
- Doxygen documentation
- O(n) complexity analysis
