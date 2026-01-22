# DoublyLinkedList

A sophisticated implementation of a doubly linked list in C++ with modern design patterns and advanced programming constructs.

The `DoublyLinkedList<T>` class is a complete implementation of a doubly linked list data structure in modern C++. It offers an intuitive and robust interface with support for various element management and access operations, while leveraging advanced mechanisms to ensure efficient memory management and code safety.

## Key Features

- **Generic template**: Supports any data type
- **Automatic memory management**: Uses smart pointers to prevent memory leaks
- **Bidirectional iterators**: Compatible with Standard Template Library (STL) algorithms
- **Move semantics**: Implementation of move constructors and operators
- **Exception safety**: Robust exception handling
- **Optimized performance**: Efficient algorithm implementation

## Implementation Details

### Node Structure

The core of the list is the `Node` structure, defined internally:

```cpp
struct Node {
    T data;
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;

    explicit Node(const T& value) : data(value), next(nullptr) {}
    explicit Node(T&& value) : data(std::move(value)), next(nullptr) {}

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
};
```

Significant points:

- `std::shared_ptr<Node> next`: "Strong" pointer to the next node, ensures the node exists as long as needed
- `std::weak_ptr<Node> prev`: "Weak" pointer to the previous node, avoids reference cycles
- Explicit copy and move constructors for value semantics
- Deletion of copy constructors to prevent unintended copies

### Memory Management via Smart Pointers

The implementation makes extensive use of C++11 smart pointers, particularly:

1. **std::shared_ptr**: Used to manage pointers to next nodes, with these properties:
   - Automatically counts references
   - Automatic deallocation when there are no more references
   - Thread safety in reference counting management

2. **std::weak_ptr**: Used for pointers to previous nodes, with these advantages:
   - Avoids reference cycles that could cause memory leaks
   - Allows checking if an object still exists
   - Does not affect reference counting

This combination ensures that memory is managed correctly, preventing both memory leaks and dangling pointers.

### Iterator System

The class implements a complete bidirectional iterator system that supports traversing the list in both directions:

1. **Iterator**: Internal class that implements a complete bidirectional iterator with all standard operations:
   - Dereferencing (`operator*` and `operator->`)
   - Increment/decrement (pre and post versions)
   - Comparison (operators `==` and `!=`)

2. **ConstIterator**: Version of the iterator for read-only traversal:
   - Prevents modification of elements during iteration
   - Implicitly convertible from `Iterator` for ease of use
   - Maintains the same traversal functionality as `Iterator`

The iterators are compatible with STL algorithms and support range-based for loops.

### Main Operations

#### Insertion

The implementation offers several insertion strategies:

1. **push_front() and push_back()**: Efficient insertion at the beginning or end of the list (O(1) complexity)
   - Both have versions for copy and move of values
   - Correctly handle edge cases (empty list)

2. **insert()**: Insertion at an arbitrary position specified by an iterator
   - Optimized for insertions at the beginning and end
   - Correctly maintains node links in all situations

Each insertion operation correctly updates the pointers between nodes and increments the size counter.

#### Removal

The class offers several methods for removing elements:

1. **pop_front() and pop_back()**: Efficient removal from the beginning or end (O(1) complexity)
   - Throw exceptions if the list is empty
   - Correctly handle the case of the last element

2. **erase()**: Removal of an element at an arbitrary position
   - Separately handles cases of removal at the beginning, end, and middle
   - Returns an iterator to the next position

3. **clear()**: Removal of all elements
   - Implementation that avoids reference counting problems
   - Explicit reset of pointers for efficient cleanup

#### Element Access

The list provides several ways to access elements:

1. **front() and back()**: Direct access to beginning and end (O(1) complexity)
   - Constant and non-constant versions for flexibility of use
   - Throw exceptions if the list is empty

2. **find()**: Search for an element in the list
   - Returns an iterator to the position of the element if found
   - Returns end() if the element is not present

3. **for_each()**: Application of a function to each element
   - Accepts any callable (functions, lambda functions, functors)
   - Constant and non-constant versions

### Exception Safety

The implementation adheres to the principle of strong exception safety. In particular:

1. **Basic guarantee**: Every operation keeps the list in a coherent state even in case of exceptions
2. **Strong guarantee**: Operations that modify the list (such as insert, erase) restore the original state in case of exceptions
3. **Explicit checks**: Every potentially invalid access is checked and generates appropriate exceptions

Examples of checks:

- Checks for operations on an empty list
- Checks for dereferencing invalid iterators
- Checks for comparison operations between iterators

### RAII (Resource Acquisition Is Initialization)

The class follows the RAII principle, ensuring that all resources (in this case the memory for nodes) are:

- Acquired during initialization (constructors)
- Automatically released during destruction (destructor)

This approach, combined with the use of smart pointers, virtually eliminates the possibility of memory leaks.

### Overloaded Operators

To provide an intuitive interface, the class overloads several operators:

1. **Comparison operators**: `==` and `!=` for comparing lists
2. **Output operator**: `<<` for formatted printing
3. **Assignment operators**: `=` for both copy and move

### Performance Optimizations

Several implementation choices improve performance:

1. **Tail pointer caching**: Reduces the complexity of push_back/pop_back to O(1)
2. **Size caching**: Avoids the need to traverse the list to determine its length
3. **Move semantics**: Reduces unnecessary copies for large data types
4. **In-place access**: Where possible, operations modify data in place rather than creating copies

## Computational Complexity

| Operation  | Time Complexity | Notes                                                                     |
| ---------- | --------------- | ------------------------------------------------------------------------- |
| push_front | O(1)            | Constant insertion at the beginning                                       |
| push_back  | O(1)            | Constant insertion at the end thanks to the tail pointer                  |
| pop_front  | O(1)            | Constant removal from the beginning                                       |
| pop_back   | O(1)            | Constant removal from the end thanks to the tail pointer                  |
| insert     | O(1) - O(n)     | O(1) for beginning/end, O(n) in the worst case for intermediate positions |
| erase      | O(1) - O(n)     | O(1) for beginning/end, O(n) in the worst case for intermediate positions |
| find       | O(n)            | Linear search through the list                                            |
| size       | O(1)            | Constant time thanks to size caching                                      |
| clear      | O(n)            | Must deallocate all nodes                                                 |

## Conclusions

This implementation of the doubly linked list represents an example of modern design in C++, balancing functionality, safety, and performance. The use of advanced design patterns such as RAII, smart pointers, and generic programming ensures that the class is robust and easily adaptable to different usage contexts.

## Usage Example

```cpp
#include "DoublyLinkedList.hpp"
#include <string>
#include <iostream>

int main() {
    // Creation and initialization
    ads::DoublyLinkedList<std::string> names = {"Alice", "Bob", "Charlie"};

    // Adding elements
    names.push_front("Zack");
    names.push_back("Diana");

    // Iteration
    std::cout << "Names: ";
    for (const auto& name : names) {
        std::cout << name << " ";
    }
    std::cout << std::endl;

    // Search and modification
    auto it = names.find("Bob");
    if (it != names.end()) {
        *it = "Bobby";
    }

    // Printing the modified list
    std::cout << "Modified list: " << names << std::endl;

    return 0;
}
```
