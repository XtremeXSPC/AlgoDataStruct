# AlgoDataStruct — A Modern C++20 Data Structures Library

![License](https://img.shields.io/badge/License-MIT-blue.svg)
![C++20](https://img.shields.io/badge/C%2B%2B-20-purple.svg)
![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)

**AlgoDataStruct** is an educational, header-only C++20 library of classic and
advanced data structures. It favours clear invariants, small well-documented
APIs, and rigorously tested behaviour over micro-optimisation — the goal is code
you can *read and learn from*, backed by tests strong enough to trust.

## Highlights

- **C++20 throughout** — reusable concepts express container requirements across
  the core structure families, catching many API mistakes at compile time.
- **Shared algebra policies** — semigroups, monoids, commutative groups, and acted
  monoids give range-query structures one explicit, stateful-capable definition of
  their mathematical operations instead of unrelated function-object bundles.
- **Header + `.tpp` split** — each public header in `include/ads/<category>/`
  includes its implementation from `src/ads/<category>/`; consumers only include
  the header.
- **Explicit ownership** — structures use project containers, smart pointers,
  and focused `std::` storage where each choice best expresses the invariant;
  temporary algorithm buffers are kept local to the operation that needs them.
- **Disciplined resource handling** — move semantics and `clear`/`reset`
  throughout, with move-only types where copying would be unsafe.
- **Custom exceptions** per category (e.g. `HeapException`, `ArrayOutOfRangeException`).
- **Doxygen docs** describing public operations, invariants, errors, and
  complexity where applicable.
- **Tested hard** — GoogleTest coverage includes construction / move / clear,
  empty / single / duplicate / large workloads, algorithmic guarantees, and
  comparisons against exact oracles; sanitizer presets are available as a
  separate validation profile.

## Implemented Data Structures

<details open>
<summary><strong>Arrays</strong> — <code>include/ads/arrays/</code></summary>

- Static Array (fixed size)
- Static Vector (fixed-capacity, inline storage)
- Dynamic Array (vector-like growth)
- Circular Array (wrap-around indexing)
- Array View (non-owning span)
- Gap Buffer (edit-friendly text buffer)
- Dynamic Bitset (resizable packed-bit sequence)

</details>

<details>
<summary><strong>Linked Lists</strong> — <code>include/ads/lists/</code></summary>

- Singly Linked List
- Doubly Linked List
- Circular Linked List

</details>

<details>
<summary><strong>Stacks &amp; Queues</strong> — <code>include/ads/stacks/</code>, <code>include/ads/queues/</code></summary>

- Array Stack, Linked Stack, Static Stack
- Linked Queue, Static Queue
- Circular Array Queue, Circular Array Deque
- Priority Queue (binary heap)

</details>

<details>
<summary><strong>Heaps</strong> — <code>include/ads/heaps/</code></summary>

- Min Heap, Max Heap
- D-ary Heap (configurable arity, min/max semantics)
- **Meldable family** (share a `MeldableHeap` concept):
  - Leftist Heap
  - Skew Heap
  - Binomial Heap — handle-based `decrease_key` / `erase`
  - Pairing Heap — handle-based `decrease_key` / `erase`
  - Fibonacci Heap — O(1) amortized `decrease_key` (the Dijkstra/Prim bound)

</details>

<details>
<summary><strong>Trees</strong> — <code>include/ads/trees/</code></summary>

- **Search** (`trees/search/`): Binary Search Tree, AVL Tree, Red-Black Tree,
  Splay Tree, Treap, B-Tree, B+ Tree, Cartesian Tree, k-d Tree, Interval Tree
- **Trie** (`trees/trie/`): Trie (prefix tree)
- Complete Binary Tree, N-ary Tree

</details>

<details>
<summary><strong>Algebra Policies</strong> — <code>include/ads/algebra/</code></summary>

- Concepts for semigroups, idempotent semigroups, monoids, groups, commutative
  groups, and acted monoids
- Built-in additive, min/max, GCD, assignment, and affine-transformation policies
- Ready-to-use range-add/sum, range-add/min, range-add/max, range-assign/sum, and
  range-affine/sum acted monoids
- Object-based policy storage: empty policies have zero overhead, while stateful
  policies remain supported

</details>

<details>
<summary><strong>Range Queries</strong> — <code>include/ads/range/</code></summary>

- Fenwick Tree and range-update Fenwick Tree over commutative groups
- Segment Tree over monoids, including non-commutative aggregates and custom leaf
  builders
- Lazy Segment Tree and Sqrt Decomposition over the same acted-monoid interface
- Sparse Table over idempotent semigroups (no artificial identity required)
- Transactional point/range modifications with strong exception guarantees

</details>

<details>
<summary><strong>Hashing &amp; Associative</strong> — <code>include/ads/hash/</code>, <code>include/ads/associative/</code></summary>

- Hash Table with chaining
- Hash Table with open addressing (linear / quadratic / double hashing)
- Dictionary interface, HashMap, TreeMap, HashSet, TreeSet, Set

</details>

<details>
<summary><strong>Graphs &amp; Matrices</strong> — <code>include/ads/graphs/</code>, <code>include/ads/matrices/</code></summary>

- Adjacency List, Adjacency Matrix
- Disjoint Set Union (Union-Find)
- Graph algorithms
- Sparse Matrix (CSR)

</details>

<details>
<summary><strong>Probabilistic</strong> — <code>include/ads/probabilistic/</code></summary>

- Bloom Filter (approximate set membership)
- Count-Min Sketch (frequency estimation)
- HyperLogLog (distinct-cardinality estimation)
- Cuckoo Filter (approximate membership with deletion)

</details>

<details>
<summary><strong>Algorithms</strong> — <code>include/ads/algorithms/</code></summary>

- Sorting algorithms
- String algorithms

</details>

See [`ROADMAP.md`](ROADMAP.md) for what is planned next and the tiered build-out
plan. Current focus: the T2 themed blocks — the meldable-heap family is complete,
and the probabilistic-sketch block is complete with Count-Min Sketch, HyperLogLog,
and Cuckoo Filter; the spatial/augmented block now includes the B+ Tree; and the
array primitives now include Dynamic Bitset.

## Repository Layout

```text
AlgoDataStruct/
├── include/ads/            # public headers, one folder per category
│   ├── arrays/  lists/  stacks/  queues/  heaps/
│   ├── trees/{search,trie}/
│   ├── algebra/  range/
│   ├── hash/  associative/  graphs/  matrices/
│   └── probabilistic/  algorithms/
├── include/support/        # generic concepts, aliases, and demo utilities
├── src/ads/                # .tpp implementations, mirroring include/ads
├── examples/<category>/    # runnable demos: main_<Name>.cc
├── tests/<category>/       # GoogleTest unit tests: Test_<Name>.cpp
├── cmake/                  # build modules (library, GoogleTest, ...)
├── docs/                   # Doxygen output (Doxyfile at the root)
├── CMakeLists.txt
├── CMakePresets.json
├── clang-toolchain.cmake
├── gcc-toolchain.cmake
└── ROADMAP.md
```

Both tests and demos use **library-rooted includes** — e.g.
`#include "ads/heaps/Fibonacci_Heap.hpp"` — resolved through the `ads_lib` target's
include directory.

## Building, Testing, and Sanitizers

The project uses CMake presets (Clang by default; GCC variants have a `-gcc`
suffix). Requires a C++20 compiler and CMake ≥ 3.20.

```sh
# Configure + build (Clang, Debug)
cmake --preset debug
cmake --build --preset build-debug

# Build and run the test suite
cmake --preset debug-tests
cmake --build --preset build-debug-tests
ctest --test-dir build/clang/debug-tests --output-on-failure
# (or run the runner directly)
./build/clang/debug-tests/bin/runTests

# AddressSanitizer + UndefinedBehaviorSanitizer
cmake --preset sanitize-tests
cmake --build --preset build-sanitize-tests
ctest --test-dir build/clang/sanitize-tests --output-on-failure
```

Demo executables are placed in `build/<toolchain>/<profile>/bin/`; a demo
`examples/heaps/main_Fibonacci_Heap.cc` builds to the target `Fibonacci_Heap`.
Available profiles include `debug`, `release`, `debug-tests`, `sanitize-tests`,
and `thread-sanitize-tests` (each with a `-gcc` counterpart).

Because the library is header-only, you can also skip CMake entirely and just add
`include/` (and `src/`) to your include path.

## Usage

### Balanced BST — AVL Tree

```cpp
#include "ads/trees/search/AVL_Tree.hpp"
#include <iostream>

int main() {
  ads::trees::AVLTree<int> tree;
  tree.insert(10);
  tree.insert(20);
  tree.insert(30);

  std::cout << std::boolalpha << tree.contains(20) << "\n"; // true
  tree.remove(20);
  std::cout << std::boolalpha << tree.contains(20) << "\n"; // false

  std::cout << "in-order: ";
  tree.in_order_traversal([](const int& v) { std::cout << v << ' '; });
  std::cout << "\n";
}
```

### Key/Value Map — HashMap

```cpp
#include "ads/associative/Hash_Map.hpp"
#include <iostream>
#include <string>

int main() {
  ads::associative::HashMap<std::string, int> scores;
  scores.put("Alice", 95);
  scores.put("Bob", 89);

  std::cout << scores.at("Bob") << "\n";    // 89
  std::cout << std::boolalpha << scores.contains("Eve") << "\n";

  scores.put("Bob", 91);    // update
  scores.erase("Alice");    // remove
  std::cout << scores.size() << "\n";
}
```

### Priority Queue with `decrease_key` — Fibonacci Heap

```cpp
#include "ads/heaps/Fibonacci_Heap.hpp"
#include <iostream>

int main() {
  ads::heaps::MinFibonacciHeap<int> heap;
  heap.insert(50);
  auto handle = heap.emplace(80);   // keep a stable handle to this element
  heap.insert(20);

  std::cout << heap.top() << "\n";  // 20

  heap.decrease_key(handle, 5);     // 80 -> 5, now the minimum
  std::cout << heap.extract_top() << "\n"; // 5
  std::cout << heap.extract_top() << "\n"; // 20
}
```

### Frequency Estimation — Count-Min Sketch

```cpp
#include "ads/probabilistic/Count_Min_Sketch.hpp"
#include <iostream>
#include <string>

int main() {
  // Size the sketch for a target error/confidence.
  auto sketch = ads::probabilistic::CountMinSketch<std::string>::from_estimates(
      /*epsilon=*/0.001, /*delta=*/0.01);

  sketch.add("apple", 50);
  sketch.add("banana", 10);

  // Never underestimates the true frequency.
  std::cout << sketch.estimate("apple")  << "\n"; // >= 50
  std::cout << sketch.estimate("cherry") << "\n"; // 0 (never added)
}
```

### Shared Algebra — Lazy Range Structures

```cpp
#include "ads/algebra/Algebra.hpp"
#include "ads/range/Range.hpp"

#include <vector>

int main() {
  using Value  = long long;
  using Policy = ads::algebra::RangeAffineRangeSum<Value>;

  const std::vector<Value> values{1, 2, 3, 4};
  ads::range::LazySegmentTree<Value, Policy> segment(values);
  ads::range::SqrtDecomposition<Value, Policy> sqrt(values);

  // Apply x -> 2*x + 3 to the inclusive range [1, 3].
  const ads::algebra::AffineTransform<Value> action{2, 3};
  segment.range_apply(1, 3, action);
  sqrt.range_apply(1, 3, action);

  // Both lazy structures also expose the same point-replacement operation.
  segment.set(0, 10);
  sqrt.set(0, 10);

  return segment.total() == sqrt.total() ? 0 : 1;
}
```

The policy is the full contract: it defines the value monoid, action monoid,
length-aware application, and chronological composition. `FenwickTree` instead
requires a commutative group, `SegmentTree` a monoid, and `SparseTable` an
idempotent semigroup. The concepts check each API shape; algebraic laws are semantic
requirements covered by dedicated law and differential tests.

## Conventions

- **C++20**, LLVM-based formatting (`.clang-format`), lint via `.clang-tidy`.
- Public headers in `include/ads/<category>/`, implementations in
  `src/ads/<category>/*.tpp`, included at the bottom of each header.
- Container categories expose domain concepts (`*_Concepts.hpp`) where shared
  semantic constraints are useful; multi-facility modules provide umbrella
  headers such as `Heaps.hpp`, `Algebra.hpp`, and `Range.hpp`.
- Doxygen comments with complexity notes; English by default.
- Tests in `tests/<category>/Test_<Name>.cpp`, demos in
  `examples/<category>/main_<Name>.cc`.

## Contributing

Contributions are welcome. When adding a structure `X` in category `cat`:

- Public header `include/ads/cat/X.hpp`, using the category concept when one
  expresses the structure's semantic requirements, and implementation
  `src/ads/cat/X.tpp`.
- Add `X.hpp` to the category umbrella header.
- Unit test `tests/cat/Test_X.cpp` covering construction / move / clear, edge
  cases (empty / single / duplicate / large), algorithmic-guarantee tests, and an
  oracle comparison; keep `runTests` green and the sanitizer build clean.
- A demo `examples/cat/main_X.cc` following the existing demo style.
- Update this README when the public surface changes.

## Learning Resources

- [Introduction to Algorithms](https://mitpress.mit.edu/9780262046305/introduction-to-algorithms/) — Cormen, Leiserson, Rivest, Stein
- [Algorithms, 4th Edition](https://algs4.cs.princeton.edu/home/) — Sedgewick & Wayne
- [Data Structures and Algorithms in C++](https://www.wiley.com/en-us/Data+Structures+and+Algorithms+in+C%2B%2B%2C+2nd+Edition-p-9780470383278) — Goodrich, Tamassia, Mount
- [The Art of Computer Programming](https://www-cs-faculty.stanford.edu/~knuth/taocp.html) — Knuth
- [Advanced Data Structures](https://www.cambridge.org/core/books/advanced-data-structures/D58D5C2D12F23464E0406320554A251B) — Peter Brass
- [Data Structures and Network Algorithms](https://epubs.siam.org/doi/book/10.1137/1.9781611970265) — Robert Endre Tarjan
- [Algorithms and Data Structures: The Basic Toolbox](https://people.mpi-inf.mpg.de/~mehlhorn/Toolbox.html) — Mehlhorn & Sanders
- [Competitive Programmer's Handbook](https://cses.fi/book/book.pdf) — Antti Laaksonen

## License

Licensed under the **MIT License** — each source file carries the MIT header.

## Contact

For questions or suggestions, please open an issue in this repository.
