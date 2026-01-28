# AlgoDataStruct: Data Structures Library (C++)

![License](https://img.shields.io/badge/License-MIT-blue.svg)
![C++20](https://img.shields.io/badge/C++-20-purple.svg)

## Description

**AlgoDataStruct** is an educational, header-only C++ library that implements classic data
structures with modern C++20. The project favors clear invariants, clean APIs, and
well-documented behavior over micro-optimizations.

Key characteristics:

- Header-only templates with `.tpp` implementation files included by the public headers
- STL-friendly iteration/traversal where applicable and explicit error handling via custom exceptions
- Focused, testable implementations intended for study and extension

## Implemented Data Structures

<details open>
<summary><strong>Linear Structures</strong></summary>

- **Arrays**
  - Static Array
  - Dynamic Array (vector-like growth)
  - Circular Array (wrap-around indexing)

- **Linked Lists**
  - Singly Linked List
  - Doubly Linked List
  - Circular Linked List

- **Stacks**
  - Array-based Stack
  - Linked-list-based Stack

- **Queues**
  - Linked Queue
  - Circular Queue
  - Circular Deque
  - Priority Queue (binary heap)

</details>

<details>
<summary><strong>Trees</strong></summary>

- Complete Binary Tree (level-order insertion)
- Binary Search Tree (BST)
- AVL Tree
- Red-Black Tree
- B-Tree
- Trie (Prefix Tree)
- Fenwick Tree (Binary Indexed Tree)

</details>

<details>
<summary><strong>Heaps</strong></summary>

- Min Heap
- Max Heap

</details>

<details>
<summary><strong>Hashing Structures</strong></summary>

- Hash table with chaining
- Hash table with open addressing (linear, quadratic, double hashing)

</details>

<details>
<summary><strong>Associative Structures</strong></summary>

- Dictionary interface
- HashMap
- TreeMap
- HashSet
- TreeSet

</details>

<details>
<summary><strong>Graph Structures</strong></summary>

- Adjacency List
- Adjacency Matrix
- Disjoint Sets (Union-Find)

</details>

## Roadmap (Advanced)

Planned, not implemented yet:

- Skip List (advanced)
- B+ Tree, 2-3 Tree, 2-3-4 Tree
- Segment Tree
- String structures (Suffix Tree/Array, Suffix Automaton, Rope)
- Probabilistic structures (Bloom Filter, Count-Min Sketch, HyperLogLog)
- Spatial structures (KD-Tree, R-Tree, Quad/Oct-tree)

## Repository Organization

```path
AlgoDataStruct/
├── include/
│   └── ads/
│       ├── arrays/
│       ├── lists/
│       ├── stacks/
│       ├── queues/
│       ├── trees/
│       ├── heaps/
│       ├── hash/
│       ├── associative/
│       └── graphs/
├── src/
│   ├── ads/               # .tpp implementation files
│   └── main_*.cc          # usage demos / sample programs
├── tests/                 # unit tests (standalone)
├── docs/                  # generated documentation
│   └── html/
├── Documentation/         # additional notes and PDFs
├── CMakeLists.txt
├── clang-toolchain.cmake
├── gcc-toolchain.cmake
└── Makefile
```

## Code Conventions

- **C++**: Google C++ Style Guide conventions
- Header-only templates: public headers in `include/ads`, implementations in `src/ads`
- Many structures include iterators or traversal callbacks where they make sense
- Documentation is written in Doxygen style; complexity notes are included when available
- Tests and demos live in `tests/` and `src/main_*.cc`

## Build and Run (optional)

The library is header-only; you can include the headers directly. To build the demo
executables in `src/main_*.cc`:

```sh
cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -B build
cmake --build build
```

Executables are placed in `build/bin/`. Use `gcc-toolchain.cmake` if you prefer GCC.

## Usage

### C++

```cpp
// Example usage of an AVL Tree
#include "ads/trees/AVL_Tree.hpp"
#include <iostream>

int main() {
    ads::trees::AVLTree<int> avlTree;

    avlTree.insert(10);
    avlTree.insert(20);
    avlTree.insert(30);

    std::cout << std::boolalpha << avlTree.contains(20) << "\n"; // true
    avlTree.remove(20);
    std::cout << std::boolalpha << avlTree.contains(20) << "\n"; // false

    std::cout << "In-order traversal: ";
    avlTree.in_order_traversal([](const int& value) {
        std::cout << value << " ";
    });
    std::cout << "\n";

    return 0;
}
```

## Example: Using HashMap

```cpp
#include "ads/associative/Hash_Map.hpp"
#include <iostream>
#include <string>

int main() {
    ads::associative::HashMap<std::string, int> scores;

    // Adding key-value pairs
    scores.put("Alice", 95);
    scores.put("Bob", 89);
    scores.put("Charlie", 92);

    // Retrieving values
    std::cout << "Bob's score: " << scores.get("Bob") << "\n"; // 89

    // Checking if a key exists
    std::cout << "Does David have a score? " << std::boolalpha
              << scores.contains("David") << "\n"; // false

    // Updating a value
    scores.put("Bob", 91);
    std::cout << "Bob's updated score: " << scores.get("Bob") << "\n"; // 91

    // Removing a key-value pair
    scores.erase("Charlie");
    std::cout << "Charlie's score exists? " << std::boolalpha
              << scores.contains("Charlie") << "\n"; // false

    return 0;
}
```

## Contributions

Contributions are welcome! Please read our contribution guidelines before submitting a pull request.

1. **Missing implementations**: Check the list of pending structures
2. **Performance improvements**: Optimizations without compromising clarity
3. **Documentation**: Improvements to comments, examples, or guides
4. **Tests**: Add more test cases or improve existing ones

When adding a new structure, please include:

- Public header in `include/ads/<area>/`
- `.tpp` implementation in `src/ads/<area>/`
- A demo in `src/main_<Structure>.cc` and a unit test in `tests/`
- An update to this README if it changes the public surface

## Learning Resources

### Essential References

- [Introduction to Algorithms](https://mitpress.mit.edu/books/introduction-algorithms-third-edition) - Cormen, Leiserson, Rivest, Stein
- [Algorithms, 4th Edition](https://algs4.cs.princeton.edu/home/) - Sedgewick & Wayne
- [Data Structure in C++](https://www.wiley.com/en-us/Data+Structures+and+Algorithms+in+C%2B%2B%2C+2nd+Edition-p-9780470383278) - Michael T. Goodrich, Roberto Tamassia, David M. Mount
- [The Art of Computer Programming](https://www-cs-faculty.stanford.edu/~knuth/taocp.html) - Knuth
- [Algorithm Design Manual](https://www.springer.com/gp/book/9781848000698) - Steven S. Skiena

### Advanced Topics

- [Advanced Data Structures](https://www.cambridge.org/core/books/advanced-data-structures/D58D5C2D12F23464E0406320554A251B) - Peter Brass
- [Purely Functional Data Structures](https://www.cambridge.org/core/books/purely-functional-data-structures/0409255DA1B48FA731859AC72E34D494) - Chris Okasaki
- [Graph Algorithms](https://www.amazon.com/Graph-Algorithms-Shimon-Even/dp/0716780453) - Shimon Even
- [Data Structures and Network Algorithms](https://epubs.siam.org/doi/book/10.1137/1.9781611970265) - Robert Endre Tarjan
- [Handbook of Data Structures and Applications](https://www.routledge.com/Handbook-of-Data-Structures-and-Applications/Mehta-Sahni/p/book/9780367571481) - Dinesh P. Mehta, Sartaj Sahni

### Specialized Resources

- [Pearls of Functional Algorithm Design](https://www.cambridge.org/core/books/pearls-of-functional-algorithm-design/B0CF0A666A65F6A1E3B1C509AF8BDAAA) - Richard Bird
- [Algorithms and Data Structures: The Basic Toolbox](https://www.springer.com/gp/book/9783540779773) - Kurt Mehlhorn, Peter Sanders
- [Competitive Programmer's Handbook](https://cses.fi/book/book.pdf) - Antti Laaksonen
- [Real-World Algorithms: A Beginner's Guide](https://mitpress.mit.edu/books/real-world-algorithms) - Panos Louridas

### Spatial and Nearest Neighbor Search

- [Similarity Search: The Metric Space Approach](https://www.springer.com/gp/book/9780387291468) - Pavel Zezula, Giuseppe Amato, Vlastislav Dohnal, Michal Batko
- [Nearest Neighbor Methods in Learning and Vision](https://mitpress.mit.edu/books/nearest-neighbor-methods-learning-and-vision) - Gregory Shakhnarovich, Trevor Darrell, Piotr Indyk
- [Foundations of Multidimensional and Metric Data Structures](https://www.sciencedirect.com/book/9780123694461/foundations-of-multidimensional-and-metric-data-structures) - Hanan Samet
- [Computational Geometry: Algorithms and Applications](https://www.springer.com/gp/book/9783540779735) - Mark de Berg, Otfried Cheong, Marc van Kreveld, Mark Overmars

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contact

For questions or suggestions, please open an issue in this repository.
