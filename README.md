# AlgoDataStruct: Data Structures Library

![License](https://img.shields.io/badge/License-MIT-blue.svg)
![Java](https://img.shields.io/badge/Java-17+-orange.svg)
![C++](https://img.shields.io/badge/C++-17+-purple.svg)

## Description

**Data-Structures** is a comprehensive educational library that implements a wide range of data structures in Java and C++. Designed for students, this library offers clean, well-documented, and efficient implementations of fundamental and advanced data structures.

## Goals

- Provide clear reference implementations for each data structure
- Include comprehensive documentation with theoretical explanations
- Demonstrate implementation differences between Java and C++
- Offer time and space complexity analysis (not always)
- Include unit tests and usage examples (not always)

## Implemented Data Structures

### Linear Structures

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

### Hierarchical Structures

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
  
- **Heaps**
  - Binary Heap
  - Fibonacci Heap
  - Binomial Heap
  - Leftist Heap
  - Skew Heap

### Hashing Structures

- **Hash Tables**
  - Hash table with chaining
  - Hash table with open addressing (Linear probing, Quadratic probing, Double hashing)
  - Cuckoo Hashing
  - Perfect Hash Table
  - Bloom Filter

### Advanced Structures

- **Range Query Structures**
  - Sparse Table
  - 2D Segment Tree
  - Range Trees
  
- **Spatial Search Structures**
  - KD-Tree
  - R-Tree
  
- **Graphs**
  - Adjacency Matrix
  - Adjacency List
  - Disjoint Sets (Union-Find)
  
- **Probabilistic Structures**
  - Skip List
  - Treap
  - Splay Tree
  
- **Other Structures**
  - Rope (for efficient string manipulation)
  - LRU Cache
  - Persistent Data Structures

## Repository Organization

```
AlgoDataStruct/
├── java/
│   ├── linear/
│   ├── trees/
│   ├── heaps/
│   ├── hash/
│   ├── advanced/
│   └── utils/
├── cpp/
│   ├── linear/
│   ├── trees/
│   ├── heaps/
│   ├── hash/
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
#include "datastructures/trees/avl_tree.h"
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

## Contributions

Contributions are welcome! Please read our contribution guidelines before submitting a pull request.

1. **Missing implementations**: Check the list of pending structures
2. **Performance improvements**: Optimizations without compromising clarity
3. **Documentation**: Improvements to comments, examples, or guides
4. **Tests**: Add more test cases or improve existing ones

## Learning Resources

- [Introduction to Algorithms](https://mitpress.mit.edu/books/introduction-algorithms-third-edition) - Cormen, Leiserson, Rivest, Stein
- [Algorithms, 4th Edition](https://algs4.cs.princeton.edu/home/) - Sedgewick & Wayne
- [Data Structure in C++](https://www.wiley.com/en-us/Data+Structures+and+Algorithms+in+C%2B%2B%2C+2nd+Edition-p-9780470383278) - Michael T. Goodrich, Roberto Tamassia, David M. Mount
- [The Art of Computer Programming](https://www-cs-faculty.stanford.edu/~knuth/taocp.html) - Knuth

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contact

For questions or suggestions, please open an issue in this repository.