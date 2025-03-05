# Axiomatic Specification of Doubly Linked List

## 1. Description

This specification presents a formal description of the doubly linked list data structure, defining both syntactic aspects (structure and interface) and semantic aspects (behavior of operations) through an axiomatic approach.

## 2. Syntactic Definition

### 2.1 Types and Structures

$\text{DoublyLinkedList}<T>$ is a data structure parameterized over a generic type $T$ that represents a sequence of elements connected in both directions.

#### 2.1.1 Internal Structures

- $\text{Node}<T>$: internal structure that contains:

    - $\text{data} : T$ - the value of the element
    - $\text{next} : \text{shared\_ptr} \ <\text{Node}<T>>$ - pointer to the next node
    - $\text{prev} : \text{weak\_ptr}<\text{Node}<T>>$ - pointer to the previous node

- $\text{Iterator}$: internal class that provides access to elements
    
- $\text{ConstIterator}$: internal class for read-only access to elements

#### 2.1.2 Main Attributes

$\text{DoublyLinkedList}<T>$ maintains the following attributes:

- $\text{head} : \text{shared\_ptr}<\text{Node}<T>>$ - pointer to the first node
- $\text{tail} : \text{shared\_ptr}<\text{Node}<T>>$ - pointer to the last node
- $\text{size\_} : \text{size\_t}$ - number of elements in the list

## 3. Syntactic Interface

### 3.1 Constructors and Destructor

```
DoublyLinkedList()
DoublyLinkedList(std::initializer_list<T> list)
DoublyLinkedList(const DoublyLinkedList& other)
DoublyLinkedList(DoublyLinkedList&& other)
~DoublyLinkedList()
```

### 3.2 Assignment Operators

```
DoublyLinkedList& operator=(const DoublyLinkedList& other)
DoublyLinkedList& operator=(DoublyLinkedList&& other)
```

### 3.3 Access Operations

```
T& front()
const T& front() const
T& back()
const T& back() const
bool empty() const
size_t size() const
```

### 3.4 Modification Operations

```
void push_front(const T& value)
void push_front(T&& value)
void push_back(const T& value)
void push_back(T&& value)
void pop_front()
void pop_back()
Iterator insert(Iterator pos, const T& value)
Iterator insert(Iterator pos, T&& value)
Iterator erase(Iterator pos)
void clear()
```

### 3.5 Search and Traversal Operations

```
Iterator find(const T& value)
ConstIterator find(const T& value) const
void for_each(const std::function<void(T&)>& func)
void for_each(const std::function<void(const T&)>& func) const
```

### 3.6 Comparison Operations

```
bool operator==(const DoublyLinkedList& other) const
bool operator!=(const DoublyLinkedList& other) const
```

### 3.7 Iterators

```
Iterator begin()
Iterator end()
ConstIterator begin() const
ConstIterator end() const
ConstIterator cbegin() const
ConstIterator cend() const
```

## 4. Axiomatic Semantic Specification

We now define the semantic behavior of each operation through axioms that specify preconditions and postconditions.

### 4.1 Notation

- $L$ denotes a list of type $\text{DoublyLinkedList}<T>$
- $L[i]$ denotes the element at position $i$ (0-based)
- $|L|$ denotes the length (size) of list $L$
- $L = [e_0, e_1, ..., e_{n-1}]$ represents a list of $n$ elements
- $L'$ denotes the state of the list after an operation
- $\emptyset$ denotes an empty list

### 4.2 Structure Invariants

$\forall L : \text{DoublyLinkedList}<T>$:

1. $L.\mbox{size\_t} = |L|$
2. If $|L| = 0$ then $L.\text{head} = L.\text{tail} = \text{nullptr}$
3. If $|L| > 0$ then $L.\text{head} \neq \text{nullptr} \land L.\text{tail} \neq \text{nullptr}$
4. If $|L| = 1$ then $L.\text{head} = L.\text{tail}$
5. If $|L| > 1$ then $\forall i \in {0, 1, ..., |L|-2}$ the node containing $L[i]$ has its $\text{next}$ field pointing to the node containing $L[i+1]$
6. If $|L| > 1$ then $\forall i \in {1, 2, ..., |L|-1}$ the node containing $L[i]$ has its $\text{prev}$ field pointing to the node containing $L[i-1]$
7. The node containing $L[0]$ has its $\text{prev}$ field configured as expired
8. The node containing $L[|L|-1]$ has its $\text{next}$ field set to $\text{nullptr}$

### 4.3 Constructors

#### Default Constructor

$\text{DoublyLinkedList}<T>() \Rightarrow L'$, where:

- $|L'| = 0$
- $L'.\text{head} = L'.\text{tail} = \text{nullptr}$

#### Initializer List Constructor

$\text{DoublyLinkedList}<T>({a_1, a_2, ..., a_n}) \Rightarrow L'$, where:

- $|L'| = n$
- $\forall i \in {0, 1, ..., n-1}: L'[i] = a_{i+1}$

#### Copy Constructor

$\text{DoublyLinkedList}<T>(L) \Rightarrow L'$, where:

- $|L'| = |L|$
- $\forall i \in {0, 1, ..., |L|-1}: L'[i] = L[i]$

#### Move Constructor

$\text{DoublyLinkedList}<T>(\&\&L) \Rightarrow L', L'$, where:

- $L'$ contains all elements originally in $L$
- $L''$ (state of $L$ after the operation) is an empty list

### 4.4 Access Operations

#### front

$\text{Pre: } |L| > 0$  
$L.\text{front}() \Rightarrow e$, where $e = L[0]$

#### back

$\text{Pre: } |L| > 0$  
$L.\text{back}() \Rightarrow e$, where $e = L[|L|-1]$

#### empty

$L.\text{empty}() \Rightarrow b$, where $b = (|L| = 0)$

#### size

$L.\text{size}() \Rightarrow s$, where $s = |L|$

### 4.5 Modification Operations

#### push_front

$L$.$\text{push\_front}(e) \Rightarrow L'$, where:

- $|L'| = |L| + 1$
- $L'[0] = e$
- $\forall i \in {1, 2, ..., |L|}: L'[i] = L[i-1]$

#### push_back

$L.\text{push\_back}(e) \Rightarrow L'$, where:

- $|L'| = |L| + 1$
- $L'[|L'|-1] = e$
- $\forall i \in {0, 1, ..., |L|-1}: L'[i] = L[i]$

#### pop_front

$\text{Pre: } |L| > 0$  
$L.\text{pop\_front}() \Rightarrow L'$, where:

- $|L'| = |L| - 1$
- $\forall i \in {0, 1, ..., |L'|-1}: L'[i] = L[i+1]$

#### pop_back

$\text{Pre: } |L| > 0$  
$L.\text{pop\_back}() \Rightarrow L'$, where:

- $|L'| = |L| - 1$
- $\forall i \in {0, 1, ..., |L'|-1}: L'[i] = L[i]$

#### insert

$\text{Pre: } 0 \leq \text{pos} \leq |L|$  
$L.\text{insert}(\text{pos}, e) \Rightarrow L'$, where:

- $|L'| = |L| + 1$
- $L'[\text{pos}] = e$
- $\forall i \in {0, 1, ..., \text{pos}-1}: L'[i] = L[i]$
- $\forall i \in {\text{pos}, \text{pos}+1, ..., |L|-1}: L'[i+1] = L[i]$

#### erase

$\text{Pre: } 0 \leq \text{pos} < |L|$  
$L.\text{erase}(\text{pos}) \Rightarrow L'$, where:

- $|L'| = |L| - 1$
- $\forall i \in {0, 1, ..., \text{pos}-1}: L'[i] = L[i]$
- $\forall i \in {\text{pos}+1, \text{pos}+2, ..., |L|-1}: L'[i-1] = L[i]$

#### clear

$L.\text{clear}() \Rightarrow L'$, where:

- $|L'| = 0$
- $L'.\text{head} = L'.\text{tail} = \text{nullptr}$

### 4.6 Search Operations

#### find

$L.\text{find}(e) \Rightarrow it$, where:

- If $\exists i \in {0, 1, ..., |L|-1}$ such that $L[i] = e$, then $it$ is an iterator pointing to element $L[i]$ with the minimum $i$ that satisfies the condition
- Otherwise, $it = L.\text{end}()$

### 4.7 Comparison Operations

#### operator==

$L_1 == L_2 \Rightarrow b$, where $b$ is true if and only if:

- $|L_1| = |L_2|$
- $\forall i \in {0, 1, ..., |L_1|-1}: L_1[i] = L_2[i]$

#### operator!=

$L_1 != L_2 \Rightarrow b$, where $b$ is true if and only if $\lnot(L_1 == L_2)$

### 4.8 Iterators

#### begin/cbegin

$L.\text{begin}() \Rightarrow it$, where:

- If $|L| > 0$, then $it$ is an iterator pointing to $L[0]$
- If $|L| = 0$, then $it = L.\text{end}()$

#### end/cend

$L.\text{end}() \Rightarrow it$, where $it$ is an iterator pointing beyond the last element of the list (sentinel).

## 5. Time Complexity

|Operation|Complexity|Description|
|---|---|---|
|Constructor|$O(1)$ or $O(n)$|$O(1)$ for default, $O(n)$ for initializer_list and copy|
|Destructor|$O(n)$|Where $n$ is the number of elements|
|front/back|$O(1)$|Direct access via head/tail pointers|
|empty/size|$O(1)$|Immediate check on attributes|
|push_front/push_back|$O(1)$|Only updates head/tail pointers|
|pop_front/pop_back|$O(1)$|Only updates head/tail pointers|
|insert|$O(1)$|For insertion at beginning/end, $O(n)$ in worst case|
|erase|$O(1)$|For removal at beginning/end, $O(n)$ in worst case|
|clear|$O(n)$|Requires deallocation of all nodes|
|find|$O(n)$|Linear search in the list|
|operator==|$O(n)$|Requires comparison of all elements|

## 6. Iterator Invariants

For every valid iterator $it$ on a list $L$:

1. If $it = L.\text{begin}()$ and $|L| > 0$, then $*it = L[0]$
2. If $it$ points to $L[i]$ for some $i \in {0, 1, ..., |L|-2}$, then $*(++it) = L[i+1]$
3. If $it$ points to $L[i]$ for some $i \in {1, 2, ..., |L|-1}$, then $*(--it) = L[i-1]$
4. $L.\text{end}()$ is not dereferenceable
5. If $it_1$ and $it_2$ are iterators pointing to $L[i]$ and $L[j]$ respectively, then $it_1 == it_2$ if and only if $i = j$

---
