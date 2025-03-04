# AlgoDataStruct: Data Structures Library

![License](https://img.shields.io/badge/License-MIT-blue.svg)
![Java](https://img.shields.io/badge/Java-17+-orange.svg)
![C++](https://img.shields.io/badge/C++-17+-purple.svg)

## Description

**AlgoDataStruct** is a comprehensive educational library that implements a wide range of data structures in Java and C++. Designed for students, this library offers clean, well-documented, and efficient implementations of fundamental and advanced data structures.

## Goals

- Provide clear reference implementations for each data structure
- Include comprehensive documentation with theoretical explanations
- Demonstrate implementation differences between Java and C++
- Offer time and space complexity analysis (not always)
- Include unit tests (not always) and usage examples

## Implemented Data Structures

<details>
<summary><strong>Linear Structures</strong></summary>

- **Arrays**
  - Static Array
  - Dynamic Array (ArrayList/Vector)
  - Circular Array
  
- **Linked Lists**
  - Singly Linked List
  - Doubly Linked List
  - Circular Linked List
  - Skip List
  
- **Stacks**
  - Array-based implementation
  - Linked list-based implementation
  
- **Queues**
  - Simple Queue
  - Priority Queue
  - Double-ended Queue (Deque)
  - Circular Queue
</details>

<details>
<summary><strong>Hierarchical Structures</strong></summary>

- **Trees**
  - Binary Tree
  - Binary Search Tree (BST)
  - AVL Tree
  - Red-Black Tree
  - B-Tree
  - B+ Tree
  - 2-3 Tree
  - 2-3-4 Tree
  - Segment Tree
  - Fenwick Tree (Binary Indexed Tree)
  - Trie (Prefix Tree)
  - Suffix Tree
  - Quad-tree and Oct-tree
  
- **Heaps** (+)
  - Binary Heap
  - Fibonacci Heap
  - Binomial Heap
  - Leftist Heap
  - Skew Heap
</details>

<details>
<summary><strong>Associative Structures</strong> (+)</summary>

- **Dictionaries/Maps**
  - Dictionary/Map (generic implementation)
  - HashMap/unordered_map
  - TreeMap/map (ordered map based on tree)
  - LinkedHashMap (ordered by insertion)
  - MultiMap (map with duplicate keys)
  - BiMap (bidirectional map)
  - Immutable Map implementations
</details>

<details>
<summary><strong>Hashing Structures</strong> (+)</summary>

- **Hash Tables**
  - Hash table with chaining
  - Hash table with open addressing (Linear probing, Quadratic probing, Double hashing)
  - Cuckoo Hashing
  - Perfect Hash Table
  - Bloom Filter
  - Count-Min Sketch
  - HyperLogLog
</details>

<details>
<summary><strong>Graph Structures</strong> (+)</summary>

- **Graph Representations**
  - Adjacency Matrix
  - Adjacency List
  - Disjoint Sets (Union-Find)
  - Weighted Graphs
  - Directed Graphs
  - Multigraphs
  - Hypergraphs
</details>

<details>
<summary><strong>String Data Structures</strong> (+)</summary>

- **String Processing**
  - Suffix Tree
  - Suffix Array
  - Suffix Automaton
  - FM-index
  - Rope (for efficient string manipulation)
</details>

<details>
<summary><strong>Spatial Data Structures</strong> (+)</summary>

- **Spatial Search Structures**
  - KD-Tree
  - R-Tree
  - Quad-tree and Oct-tree
  - Voronoi Diagram
  - Range Trees (multidimensional)
  
- **Nearest Neighbor Search Structures**
  - HNSW (Hierarchical Navigable Small World)
  - VP-Tree (Vantage-Point Tree)
  - Cover Tree
  - ANNOY (Approximate Nearest Neighbors Oh Yeah)
</details>

<details>
<summary><strong>Advanced Structures</strong> (+)</summary>

- **Range Query Structures**
  - Sparse Table
  - 2D Segment Tree
  - Range Trees
  
- **Probabilistic Structures**
  - Skip List
  - Treap
  - Splay Tree
  - MinHash
  
- **Concurrent Structures**
  - Thread-safe Lists
  - Concurrent Hash Maps
  - Lock-free Data Structures
  
- **Persistent Data Structures**
  - Persistent Arrays
  - Persistent Lists
  - Persistent Maps
  
- **Big Data Structures**
  - Log-Structured Merge Tree (LSM Tree)
  - External Memory Data Structures
  
- **Other Structures**
  - LRU Cache
  - LFU Cache
  - Time Series Data Structures
</details>

(+) *Not implemented yet but I'm working on it!*

## Repository Organization

```
AlgoDataStruct/
├── java/
│   ├── linear/
│   ├── trees/
│   ├── heaps/
│   ├── associative/
│   ├── hash/
│   ├── graph/
│   ├── string/
│   ├── spatial/
│   ├── concurrent/
│   ├── persistent/
│   ├── advanced/
│   └── utils/
├── cpp/
│   ├── linear/
│   ├── trees/
│   ├── heaps/
│   ├── associative/
│   ├── hash/
│   ├── graph/
│   ├── string/
│   ├── spatial/
│   ├── concurrent/
│   ├── persistent/
│   ├── advanced/
│   └── utils/
├── docs/
│   ├── complexity-analysis.md
│   ├── implementation-guide.md
│   └── structure-specific/
├── examples/
│   ├── java/
│   └── cpp/
└── tests/
    ├── java/
    └── cpp/
```

## Code Conventions

- **Java**: I follow Google Java Style Guide conventions
- **C++**: I follow Google C++ Style Guide conventions
- Each data structure includes:
  - Complete documentation with JavaDoc/Doxygen
  - Complexity analysis (not always)
  - Unit tests (not always)
  - Usage examples

## Usage

### Java

```java
// Example usage of an AVL Tree
import datastructures.trees.AVLTree;

public class Main {
    public static void main(String[] args) {
        AVLTree<Integer> avlTree = new AVLTree<>();
        
        avlTree.insert(10);
        avlTree.insert(20);
        avlTree.insert(30);
        
        System.out.println("Contains 20? " + avlTree.contains(20)); // true
        avlTree.delete(20);
        System.out.println("Contains 20? " + avlTree.contains(20)); // false
        
        System.out.println("In-order traversal: ");
        avlTree.inOrderTraversal(value -> System.out.print(value + " "));
    }
}
```

### C++

```cpp
// Example usage of an AVL Tree
#include "avl_tree.h" // Make file does the work
#include <iostream>

int main() {
    AVLTree<int> avlTree;
    
    avlTree.insert(10);
    avlTree.insert(20);
    avlTree.insert(30);
    
    std::cout << "Contains 20? " << (avlTree.contains(20) ? "Yes" : "No") << std::endl; // Yes
    avlTree.remove(20);
    std::cout << "Contains 20? " << (avlTree.contains(20) ? "Yes" : "No") << std::endl; // No
    
    std::cout << "In-order traversal: ";
    avlTree.inOrderTraversal([](int value) {
        std::cout << value << " ";
    });
    
    return 0;
}
```

## Example: Using HashMap

```java
// Java HashMap example
import datastructures.associative.HashMap;

public class Main {
    public static void main(String[] args) {
        HashMap<String, Integer> scores = new HashMap<>();
        
        // Adding key-value pairs
        scores.put("Alice", 95);
        scores.put("Bob", 89);
        scores.put("Charlie", 92);
        
        // Retrieving values
        System.out.println("Bob's score: " + scores.get("Bob")); // 89
        
        // Checking if a key exists
        System.out.println("Does David have a score? " + scores.containsKey("David")); // false
        
        // Updating a value
        scores.put("Bob", 91);
        System.out.println("Bob's updated score: " + scores.get("Bob")); // 91
        
        // Removing a key-value pair
        scores.remove("Charlie");
        System.out.println("Charlie's score exists? " + scores.containsKey("Charlie")); // false
    }
}
```

## Contributions

Contributions are welcome! Please read our contribution guidelines before submitting a pull request.

1. **Missing implementations**: Check the list of pending structures
2. **Performance improvements**: Optimizations without compromising clarity
3. **Documentation**: Improvements to comments, examples, or guides
4. **Tests**: Add more test cases or improve existing ones

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