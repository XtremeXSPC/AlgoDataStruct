# Fase 3: Grafi e Strutture di Alto Livello

## Obiettivi

Implementare strutture dati composite che si basano su quelle delle fasi precedenti e rappresentano concetti di alto livello.

**Strutture da implementare:**

1. Graph (con Adjacency List e Adjacency Matrix)
2. HashMap (implementazione concreta basata su Hash Table)
3. Priority Queue (basata su Heap)

## 1. Graph

### Descrizione

Un grafo G = (V, E) è una collezione di vertici (nodi) e archi (collegamenti tra nodi). I grafi sono fondamentali per modellare reti, relazioni, percorsi, e molti altri problemi reali.

### Tipi di Grafo da Supportare

- **Diretto vs Non-diretto**: Archi con o senza direzione
- **Pesato vs Non-pesato**: Archi con o senza peso
- **Supporto per etichette**: Vertici e archi possono avere dati associati

### Posizione File

```
include/ads/graphs/Graph_Adjacency_List.hpp
include/ads/graphs/Graph_Adjacency_Matrix.hpp
src/ads/graphs/Graph_Adjacency_List.tpp
src/ads/graphs/Graph_Adjacency_Matrix.tpp
src/main_Graphs.cc
```

### Rappresentazioni

#### Adjacency List

Ogni vertice mantiene una lista dei suoi vicini.

**Struttura**:

```cpp
template <typename VertexData = int, typename EdgeWeight = double>
class GraphAdjacencyList {
private:
  struct Edge {
    size_t destination;
    EdgeWeight weight;
    Edge(size_t dest, EdgeWeight w = EdgeWeight{})
      : destination(dest), weight(w) {}
  };

  struct Vertex {
    VertexData data;
    std::vector<Edge> adjacency;
    explicit Vertex(const VertexData& d) : data(d) {}
    explicit Vertex(VertexData&& d) : data(std::move(d)) {}
  };

  std::vector<Vertex> vertices_;
  bool is_directed_;
  size_t num_edges_;
};
```

**Vantaggi**: Efficiente per grafi sparsi (pochi archi), iterazione veloce sui vicini
**Svantaggi**: Verificare esistenza arco è O(degree), più complesso da implementare

**Complessità spaziale**: O(V + E)

#### Adjacency Matrix

Matrice 2D dove `matrix[i][j]` indica se esiste arco da i a j (e il suo peso).

**Struttura**:

```cpp
template <typename VertexData = int, typename EdgeWeight = double>
class GraphAdjacencyMatrix {
private:
  struct Vertex {
    VertexData data;
    explicit Vertex(const VertexData& d) : data(d) {}
  };

  std::vector<Vertex> vertices_;
  std::vector<std::vector<std::optional<EdgeWeight>>> matrix_;
  bool is_directed_;
  size_t num_edges_;
};
```

**Note**:

- `std::optional<EdgeWeight>` permette di distinguere tra "nessun arco" (nullopt) e "arco con peso 0" (0)
- Per grafi non pesati, si può usare `std::vector<std::vector<bool>>`

**Vantaggi**: Verificare esistenza arco è O(1), semplice da implementare
**Svantaggi**: Richiede O(V²) spazio anche per grafi sparsi, iterare sui vicini è O(V)

**Complessità spaziale**: O(V²)

### Operazioni da Implementare (entrambe le rappresentazioni)

#### Costruzione

- `Graph(bool is_directed = false)` - costruttore
- `Graph(size_t num_vertices, bool is_directed = false)` - con preallocazione
- Move constructor e assignment

#### Gestione Vertici

- `size_t add_vertex(const VertexData& data)` - aggiunge vertice, ritorna ID
- `size_t add_vertex(VertexData&& data)` - versione move
- `VertexData& get_vertex_data(size_t vertex_id)` - accesso dati vertice
- `bool has_vertex(size_t vertex_id) const` - verifica esistenza
- `size_t num_vertices() const noexcept`

#### Gestione Archi

- `void add_edge(size_t from, size_t to, EdgeWeight weight = EdgeWeight{})` - aggiunge arco
- `void remove_edge(size_t from, size_t to)` - rimuove arco
- `bool has_edge(size_t from, size_t to) const` - verifica esistenza arco
- `std::optional<EdgeWeight> get_edge_weight(size_t from, size_t to) const` - ottiene peso
- `size_t num_edges() const noexcept`

#### Navigazione

- `std::vector<size_t> get_neighbors(size_t vertex_id) const` - lista ID vicini
- `std::vector<std::pair<size_t, EdgeWeight>> get_neighbors_with_weights(size_t vertex_id) const`
- `size_t degree(size_t vertex_id) const` - numero di vicini

#### Query

- `bool is_directed() const noexcept`
- `bool is_empty() const noexcept`
- `void clear()` - rimuove tutti vertici e archi

### Comportamento per Grafi Non-diretti

Quando `is_directed_ == false`, l'aggiunta di un arco (u, v) deve automaticamente aggiungere anche (v, u). La rimozione analogamente.

**Importante**: `num_edges_` deve contare ogni arco non-diretto una sola volta, non due.

### Algoritmi di Attraversamento (da implementare come metodi o funzioni esterne)

#### Breadth-First Search (BFS)

```cpp
std::vector<size_t> bfs(size_t start_vertex) const;
```

Visita livello per livello usando una coda.

**Complessità**: O(V + E) per adjacency list, O(V²) per adjacency matrix

#### Depth-First Search (DFS)

```cpp
std::vector<size_t> dfs(size_t start_vertex) const;
```

Visita andando in profondità usando uno stack (ricorsivo o iterativo).

**Complessità**: O(V + E) per adjacency list, O(V²) per adjacency matrix

#### Path Finding

```cpp
std::optional<std::vector<size_t>> find_path(size_t from, size_t to) const;
```

Restituisce un percorso da `from` a `to` se esiste, usando BFS.

#### Connectivity

```cpp
bool is_connected(size_t v1, size_t v2) const;
std::vector<std::vector<size_t>> connected_components() const;  // Per non-diretti
```

### Test da Includere per Grafi

1. Costruzione grafo diretto e non-diretto
2. Aggiunta/rimozione vertici e archi
3. Grafi pesati e non pesati
4. BFS e DFS su grafi di varie forme (lineare, ciclico, albero, completo)
5. Path finding
6. Componenti connesse
7. Grafi con self-loops e archi paralleli (se supportati)
8. Performance: grafi grandi (migliaia di vertici/archi)
9. Confronto performance adjacency list vs matrix

---

## 2. HashMap

### Descrizione

HashMap è un'implementazione concreta di dizionario che fornisce un'interfaccia semplice e moderna, basandosi sulla Hash Table con chaining implementata nella Fase 2.

Questa è essenzialmente un wrapper user-friendly attorno a `HashTableChaining` con API più intuitiva.

### Posizione File

```
include/ads/associative/Hash_Map.hpp
src/ads/associative/Hash_Map.tpp
src/main_Hash_Map.cc
```

### Struttura

```cpp
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class HashMap {
private:
  HashTableChaining<Key, Value> table_;

public:
  using key_type = Key;
  using mapped_type = Value;
  using value_type = std::pair<const Key, Value>;
};
```

### Differenze rispetto a HashTableChaining

HashMap fornisce:

1. **Iteratori**: Per attraversare tutte le entry
2. **API simile a std::unordered_map**: Convenzioni e nomi familiari
3. **Metodi convenience**: `keys()`, `values()`, `entries()`

### Operazioni da Implementare

#### Costruttori

- `HashMap(size_t initial_capacity = 16, float max_load_factor = 0.75)`
- `HashMap(std::initializer_list<value_type>)` - costruzione da lista
- Move constructor e assignment

#### Inserimento/Modifica

- `std::pair<iterator, bool> insert(const value_type&)` - ritorna iterator e se inserito
- `std::pair<iterator, bool> insert(value_type&&)`
- `template<typename... Args> std::pair<iterator, bool> emplace(Args&&...)`
- `Value& operator[](const Key&)` - insert se non esiste

#### Accesso

- `Value& at(const Key&)` - con bounds checking
- `const Value& at(const Key&) const`
- `Value* find_ptr(const Key&)` - nullptr se non trovato
- `iterator find(const Key&)` - end() se non trovato

#### Ricerca

- `bool contains(const Key&) const`
- `size_t count(const Key&) const` - ritorna 0 o 1 (no duplicati)

#### Rimozione

- `size_t erase(const Key&)` - ritorna num elementi rimossi (0 o 1)
- `iterator erase(iterator pos)` - rimuove elemento puntato da iterator

#### Query

- `size_t size() const noexcept`
- `bool empty() const noexcept`
- `void clear() noexcept`
- `float load_factor() const noexcept`

#### Utility

- `std::vector<Key> keys() const` - tutti i keys
- `std::vector<Value> values() const` - tutti i values
- `std::vector<value_type> entries() const` - tutte le coppie

### Iteratori

Implementare un forward iterator che attraversa tutte le entry nella hash table:

```cpp
class iterator {
private:
  HashMap* map_;
  size_t bucket_index_;
  typename std::list<Entry>::iterator list_it_;

public:
  using value_type = std::pair<const Key, Value>;
  using reference = value_type&;
  using pointer = value_type*;

  reference operator*() const;
  pointer operator->() const;
  iterator& operator++();
  iterator operator++(int);
  bool operator==(const iterator&) const;
  bool operator!=(const iterator&) const;
};

iterator begin();
iterator end();
const_iterator begin() const;
const_iterator end() const;
const_iterator cbegin() const;
const_iterator cend() const;
```

**Implementazione iterator++**:

1. Avanza nell'attuale bucket list
2. Se fine bucket, passa al prossimo bucket non vuoto
3. Se fine tabella, diventa end()

### Range-based For Loop

Con gli iteratori, supporta:

```cpp
HashMap<std::string, int> map;
for (auto& [key, value] : map) {
  // ...
}
```

### Test da Includere

1. Inserimenti e accessi base
2. Iterazione con range-based for
3. Metodi convenience (keys, values, entries)
4. Confronto con std::unordered_map (correttezza, non performance)
5. Rehashing automatico durante inserimenti
6. Operazioni su map vuota

---

## 3. Priority Queue

### Descrizione

Priority Queue è una coda dove gli elementi sono serviti in base alla loro priorità (non FIFO). Fondamentale per algoritmi come Dijkstra, Prim, task scheduling, ecc.

Implementazione basata sul Min Heap della Fase 1.

### Posizione File

```
include/ads/queues/Priority_Queue.hpp
src/ads/queues/Priority_Queue.tpp
src/main_Priority_Queue.cc
```

### Struttura

```cpp
template <typename T, typename Compare = std::less<T>>
class PriorityQueue {
private:
  std::vector<T> heap_;
  Compare comp_;

  // Helper methods
  size_t parent(size_t i) const { return (i - 1) / 2; }
  size_t left_child(size_t i) const { return 2 * i + 1; }
  size_t right_child(size_t i) const { return 2 * i + 2; }

  void heapify_up(size_t index);
  void heapify_down(size_t index);
};
```

**Note**:

- `Compare = std::less<T>` → max-heap (priorità maggiore = maggior valore)
- `Compare = std::greater<T>` → min-heap (priorità maggiore = minor valore)

### Operazioni da Implementare

#### Costruttori

- `PriorityQueue(Compare comp = Compare{})` - default
- `PriorityQueue(const std::vector<T>&, Compare comp = Compare{})` - da vector (heapify)
- `PriorityQueue(std::initializer_list<T>, Compare comp = Compare{})`
- Move constructor e assignment

#### Inserimento

- `void push(const T&)` - inserisce elemento
- `void push(T&&)` - versione move
- `template<typename... Args> void emplace(Args&&...)` - costruisce in-place

#### Rimozione/Accesso

- `void pop()` - rimuove elemento con priorità massima (throw se vuota)
- `const T& top() const` - accesso all'elemento con priorità massima (throw se vuota)

#### Query

- `bool empty() const noexcept`
- `size_t size() const noexcept`
- `void clear() noexcept`

#### Utility (opzionali ma utili)

- `void reserve(size_t capacity)` - preallocazione
- `std::vector<T> sorted_elements() const` - estrae tutti gli elementi in ordine (destructive: svuota la coda)

### Heap Operations

Riutilizzare logica del Min/Max Heap della Fase 1:

**heapify_up**: Dopo push, sposta elemento verso l'alto
**heapify_down**: Dopo pop, sposta elemento verso il basso

### Confronto con std::priority_queue

La nostra implementazione dovrebbe offrire API simile ma più ricca:

- `sorted_elements()` per estrarre tutto ordinato
- Supporto per comparatori custom via template
- Possibile estensione con `decrease_key()` / `increase_key()` per algoritmi di grafi

### Complessità

- `push`: O(log n)
- `pop`: O(log n)
- `top`: O(1)
- Costruzione da vector (heapify): O(n)

### Applicazioni da Dimostrare nei Test

1. **Task Scheduling**: Struct con priorità e nome task
2. **Event Simulation**: Eventi con timestamp
3. **Top-K Elements**: Mantieni k elementi più piccoli/grandi da un stream
4. **Merge K Sorted Lists**: Usa priority queue per efficienza

### Test da Includere

1. Inserimenti e estrazioni base (verifica ordine)
2. Max-heap e min-heap con comparatori diversi
3. Costruzione da vector
4. Priority queue di struct custom
5. Applicazioni pratiche (task scheduling, merge k lists)
6. Performance: milioni di operazioni

---

## Integrazione e Testing

### Test Integrato

Creare `src/main_Phase3_Integration.cc` che dimostra uso combinato delle strutture:

**Esempio 1: Dijkstra's Algorithm**
Usa Graph + Priority Queue per trovare shortest path in grafo pesato.

```cpp
std::vector<double> dijkstra(const GraphAdjacencyList<int, double>& graph,
                               size_t start);
```

**Esempio 2: Word Frequency Counter**
Usa HashMap per contare frequenze, Priority Queue per top-K parole più frequenti.

```cpp
std::vector<std::pair<std::string, int>> top_k_words(
  const std::string& text, size_t k);
```

**Esempio 3: Social Network**
Usa Graph per rappresentare amicizie, BFS per degrees of separation.

```cpp
int degrees_of_separation(const GraphAdjacencyList<std::string>& network,
                          size_t person1, size_t person2);
```

### Performance Benchmarks

Confrontare:

1. **Grafi**: Adjacency List vs Matrix per operazioni comuni
2. **HashMap**: Nostra implementazione vs std::unordered_map
3. **Priority Queue**: Nostra implementazione vs std::priority_queue

### Valgrind e Memory Safety

Eseguire tutti i test con valgrind:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./main_Phase3
```

Verificare:

- No memory leaks
- No invalid reads/writes
- No use of uninitialized values

---

## Documentazione Finale

### Aggiornamento README

Aggiornare sezioni complete:

- Linear Structures: ✓
- Hierarchical Structures: BST ✓, AVL ✓, Heaps ✓
- Associative Structures: HashMap ✓
- Hashing Structures: Hash Tables ✓
- Graph Structures: Graph ✓
- Advanced Structures: Priority Queue ✓

### Esempi d'Uso

Aggiungere nella cartella `examples/`:

- `dijkstra.cc` - shortest path
- `word_frequency.cc` - text analysis
- `task_scheduler.cc` - priority-based scheduling
- `social_network.cc` - graph applications

### Complexity Reference

Creare `docs/complexity_table.md` con tabella riassuntiva delle complessità di tutte le strutture implementate.

---

## Checklist Completamento Fase 3

- [ ] Graph Adjacency List implementato
- [ ] Graph Adjacency Matrix implementato
- [ ] Algoritmi BFS/DFS implementati
- [ ] HashMap implementato con iteratori
- [ ] Priority Queue implementato
- [ ] Test completi per tutte le strutture
- [ ] Test integrazione (Dijkstra, word frequency, ecc.)
- [ ] Performance benchmarks
- [ ] Valgrind clean
- [ ] Esempi applicativi
- [ ] README.md aggiornato completamente
- [ ] Documentazione tecnica completa
- [ ] Complexity reference table

## Stima Temporale

- Graph Adjacency List: 8-10 ore
- Graph Adjacency Matrix: 5-6 ore
- Algoritmi su grafi: 4-5 ore
- HashMap con iteratori: 6-8 ore
- Priority Queue: 4-5 ore
- Testing e integrazione: 8-10 ore
- Benchmarks: 3-4 ore
- Documentazione ed esempi: 4-5 ore

**Totale Fase 3**: 42-53 ore (5-7 giorni)

---

## Conclusione del Progetto

Al completamento della Fase 3, il repository AlgoDataStruct avrà:

✓ **13+ strutture dati fondamentali** completamente implementate
✓ **Testing completo** con casi limite ed eccezioni
✓ **Performance benchmarks** per confronti oggettivi
✓ **Esempi applicativi** di uso reale
✓ **Documentazione tecnica** estensiva
✓ **Memory safety** verificata con valgrind

Il repository sarà una risorsa didattica completa per un corso base di Algoritmi e Strutture Dati, con codice professionale, ben testato, e pronto per l'uso in progetti reali o come riferimento per lo studio.
