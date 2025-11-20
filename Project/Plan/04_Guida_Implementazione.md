# Guida Implementazione con Claude Code

## Panoramica delle Tre Fasi

Questo documento fornisce una sintesi operativa del piano di sviluppo e linee guida per utilizzare Claude Code nell'implementazione.

### Strutture per Fase

**Fase 1: Fondamenta (2-3 giorni)**

- Singly Linked List
- Min Heap
- Max Heap

**Fase 2: Strutture Intermedie (4-5 giorni)**

- AVL Tree
- Hash Table (Chaining)
- Hash Table (Open Addressing)

**Fase 3: Strutture Composite (5-7 giorni)**

- Graph (Adjacency List & Matrix)
- HashMap (con iteratori)
- Priority Queue

**Totale**: ~13 strutture, 11-15 giorni di sviluppo

---

## Approccio Consigliato con Claude Code

### 1. Preparazione Workspace

Prima di iniziare ogni fase:

```bash
# Assicurati di essere nella root del repository
cd AlgoDataStruct

# Verifica la struttura esistente
tree -L 3 include/ src/

# Crea branch per la fase
git checkout -b phase-N-implementation
```

### 2. Ordine di Implementazione per Fase

#### Fase 1

1. **Singly Linked List** (inizia con la più semplice)
   - Header: `include/ads/lists/Singly_Linked_List.hpp`
   - Implementation: `src/ads/lists/Singly_Linked_List.tpp`
   - Test: `src/main_Singly_Linked_List.cc`
   - Compila e testa prima di procedere

2. **Min Heap**
   - Implementa completamente prima del Max Heap
   - Potrai riusare gran parte della logica per Max Heap

3. **Max Heap**
   - Quasi identico a Min Heap, cambia solo comparazioni
   - Considera di creare test condivisi

#### Fase 2

1. **AVL Tree** (la più complessa, inizia quando sei concentrato)
   - Implementa rotazioni una alla volta
   - Testa ogni rotazione indipendentemente
   - Solo dopo aggiungi insert/remove completi

2. **Hash Table Chaining** (più semplice delle due)
   - Usa std::list per i bucket
   - Testa rehashing accuratamente

3. **Hash Table Open Addressing**
   - Linear probing prima, altre strategie dopo
   - Tombstones richiedono attenzione particolare

#### Fase 3

1. **Graph Adjacency List** (inizia con questo)
   - Più naturale e comune
   - Implementa BFS/DFS contestualmente

2. **Graph Adjacency Matrix**
   - Più semplice strutturalmente
   - Riusa algoritmi da Adjacency List dove possibile

3. **HashMap**
   - Wrapper relativamente semplice
   - Focus sugli iteratori

4. **Priority Queue**
   - Veloce se hai già heap funzionanti
   - Testa con applicazioni reali (Dijkstra)

### 3. Workflow per Ogni Struttura

**Step 1: Header File**

```
- Definisci classe e membri privati
- Dichiara tutti i metodi pubblici
- Aggiungi documentazione Doxygen
- Non implementare ancora
```

**Step 2: Implementation File**

```
- Implementa costruttori/distruttore per primi
- Poi operazioni base (insert/remove)
- Poi operazioni query (search/contains)
- Infine utility e advanced features
```

**Step 3: Test File**

```
- Testa costruttori e empty state
- Testa ogni operazione singolarmente
- Testa edge cases
- Testa exception handling
- Testa move semantics
- Testa performance con grandi dataset
```

**Step 4: Compilation e Testing**

```bash
# Compila
make clean
make

# Esegui test
./build/main_StructureName

# Verifica memory leaks
valgrind --leak-check=full ./build/main_StructureName

# Se tutto OK, commit
git add .
git commit -m "Implement StructureName with tests"
```

---

## Pattern Comuni e Riuso del Codice

### Pattern 1: Gestione Memoria con unique_ptr

Ogni struttura node-based usa questo pattern:

```cpp
struct Node {
  T data;
  std::unique_ptr<Node> next;  // Ownership
  Node* prev;                   // Non-owning pointer (se doubly linked)
};

std::unique_ptr<Node> head_;  // Owning pointer to first
Node* tail_;                  // Non-owning pointer to last
```

### Pattern 2: Move Semantics

Template standard per tutte le strutture:

```cpp
// Move constructor
Structure(Structure&& other) noexcept
  : member1_(std::move(other.member1_)),
    member2_(other.member2_) {
  other.member2_ = nullptr;
  other.size_ = 0;
}

// Move assignment
Structure& operator=(Structure&& other) noexcept {
  if (this != &other) {
    clear();  // Release current resources
    member1_ = std::move(other.member1_);
    member2_ = other.member2_;
    other.member2_ = nullptr;
    other.size_ = 0;
  }
  return *this;
}
```

### Pattern 3: Exception Hierarchy

```cpp
// In namespace ads::category
class CategoryException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class SpecificException : public CategoryException {
public:
  SpecificException(const std::string& msg)
    : CategoryException(msg) {}
};

// Usage
if (is_empty()) {
  throw StructureException("Operation on empty structure");
}
```

### Pattern 4: Iterator Implementation

```cpp
class iterator {
private:
  NodeType* current_;
  const StructureType* container_;  // For bounds checking

public:
  // Iterator traits
  using iterator_category = std::forward_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using reference = T&;

  // Required operations
  reference operator*() const { return current_->data; }
  pointer operator->() const { return &current_->data; }
  iterator& operator++() {
    current_ = current_->next.get();
    return *this;
  }
  iterator operator++(int) {
    iterator temp = *this;
    ++(*this);
    return temp;
  }
  bool operator==(const iterator& other) const {
    return current_ == other.current_;
  }
  bool operator!=(const iterator& other) const {
    return !(*this == other);
  }
};
```

---

## Debugging e Testing Guidelines

### Test Checklist per Ogni Struttura

Verifica sistematicamente:

- [ ] **Empty state**: Tutte le operazioni su struttura vuota
- [ ] **Single element**: Comportamento con un solo elemento
- [ ] **Multiple elements**: Operazioni standard
- [ ] **Boundary conditions**: Primo/ultimo elemento
- [ ] **Exceptions**: Operazioni non valide lanciano eccezioni corrette
- [ ] **Move semantics**: Costruttore e assignment funzionano
- [ ] **Iterators**: begin/end corretti, iterazione completa
- [ ] **Large dataset**: Performance con migliaia di elementi
- [ ] **Memory**: Valgrind conferma no leaks

### Debugging Suggerimenti

**Per liste e alberi**:

```cpp
// Aggiungi metodo di debug printing
void print_structure() const {
  // Stampa struttura visivamente
  // Utile per verificare puntatori
}
```

**Per hash tables**:

```cpp
// Mostra distribuzione
void print_distribution() const {
  for (size_t i = 0; i < capacity_; ++i) {
    std::cout << "Bucket " << i << ": "
              << bucket_[i].size() << " elements\n";
  }
}
```

**Per grafi**:

```cpp
// Visualizza struttura
void print_graph() const {
  for (size_t i = 0; i < num_vertices(); ++i) {
    std::cout << i << " -> ";
    for (auto neighbor : get_neighbors(i)) {
      std::cout << neighbor << " ";
    }
    std::cout << "\n";
  }
}
```

### Valgrind Usage

```bash
# Compilazione con simboli debug
g++ -g -O0 -std=c++17 src/main_Test.cc -o test

# Full memory check
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         ./test

# Output in file per analisi dettagliata
valgrind --leak-check=full \
         --log-file=valgrind-report.txt \
         ./test
```

**Interpretazione output**:

- `definitely lost`: Memory leak, da fixare
- `indirectly lost`: Conseguenza di definitely lost
- `possibly lost`: Investigare
- `still reachable`: Spesso OK (memoria allocata ma referenced)

---

## Stile e Convenzioni

### Naming

```cpp
// Classes: PascalCase
class BinarySearchTree {};
class HashTableChaining {};

// Methods: snake_case
void push_front();
bool is_empty() const;

// Private members: snake_case with trailing underscore
T* head_;
size_t size_;

// Constants: kPascalCase
static constexpr size_t kInitialCapacity = 16;

// Template parameters: Single letter or PascalCase
template <typename T>
template <typename Key, typename Value>
```

### Comments

```cpp
/**
 * @brief Brief description
 *
 * Detailed description of what this does.
 * Can span multiple lines.
 *
 * @param param1 Description of param1
 * @param param2 Description of param2
 * @return Description of return value
 * @throws ExceptionType When this exception is thrown
 *
 * @complexity Time O(n), Space O(1)
 *
 * @example
 * ```cpp
 * Structure s;
 * s.method(arg);
 * ```
 */
void method(Type param1, Type param2);
```

### File Organization

Ogni header file:

```cpp
//===--------------------------------------------------------------------------===//
/**
 * @file FileName.hpp
 * @author Costantino Lombardi
 * @brief Brief description
 * @version 0.1
 * @date YYYY-MM-DD
 *
 * @copyright MIT License YYYY
 */
//===--------------------------------------------------------------------------===//
#pragma once

#include <dependencies>

namespace ads::category {

// Class definition

}  // namespace ads::category
//===--------------------------------------------------------------------------===//
```

---

## Performance Considerations

### Quando Ottimizzare

**Non ottimizzare prematuramente**. Priorità:

1. Correttezza
2. Chiarezza del codice
3. Completezza dei test
4. Performance

**Eccezioni**: Se un'implementazione standard è O(n²) ma esiste variante O(n log n) senza complicazioni maggiori, usa la più efficiente.

### Casi dove Performance Conta

**Heap**: `build_heap` deve essere O(n), non O(n log n)

```cpp
// Bottom-up heapify è O(n)
for (int i = size_/2 - 1; i >= 0; --i) {
  heapify_down(i);
}
```

**AVL rotazioni**: Devono essere O(1)

```cpp
// Single rotation, no loops
std::unique_ptr<Node> rotate_right(std::unique_ptr<Node> y) {
  auto x = std::move(y->left);
  y->left = std::move(x->right);
  update_height(y.get());
  update_height(x.get());
  x->right = std::move(y);
  return x;
}
```

**Hash table rehashing**: Preallocare quando possibile

```cpp
void reserve(size_t new_capacity) {
  if (new_capacity > capacity_) {
    // Rehash all elements
  }
}
```

### Benchmarking

Usa `<chrono>` per misurazioni precise:

```cpp
auto start = std::chrono::high_resolution_clock::now();

// Operazione da misurare
for (int i = 0; i < iterations; ++i) {
  structure.operation();
}

auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

std::cout << "Tempo: " << duration.count() << " ms\n";
```

---

## Interazione con Claude Code

### Prompt Efficaci

**Per implementazione completa**:

```
Implementa [StructureName] seguendo le specifiche nel documento
[PhaseN_filename.md]. Includi:
- Header file con documentazione completa
- Implementation file con gestione memoria moderna
- File di test completo con tutti i casi
- Segui lo stile e le convenzioni del repository esistente
```

**Per debugging**:

```
Ho un problema con [specific_operation] in [StructureName].
Sintomo: [describe the issue]
Codice rilevante: [paste code]
Output/errore: [paste output]
Aiutami a identificare e fixare il problema.
```

**Per ottimizzazione**:

```
La mia implementazione di [operation] è corretta ma potrebbe essere
più efficiente. Complessità attuale: [current], desiderata: [target].
Suggerisci ottimizzazioni mantenendo chiarezza del codice.
```

### Cosa Fornire a Claude Code

Per risultati migliori, fornisci sempre:

1. **Contesto**: Link ai documenti di specifica
2. **Codice esistente**: Strutture già implementate come riferimento
3. **Vincoli**: Pattern e convenzioni da seguire
4. **Test attesi**: Comportamento desiderato

### Iterazione e Revisione

Non aspettarti perfezione al primo tentativo:

1. Claude Code genera implementazione iniziale
2. Compila e testa
3. Identifica problemi specifici
4. Richiedi fix mirati
5. Ripeti fino a soddisfazione

---

## Checklist Finale Pre-Consegna

Quando tutte le fasi sono complete:

### Codice

- [ ] Tutte le strutture compilano senza warning
- [ ] Tutti i test passano
- [ ] Valgrind clean su tutti i test
- [ ] Code coverage > 80% (se misurabile)
- [ ] Nessun TODO o FIXME rimasto
- [ ] Codice formattato consistentemente (considera clang-format)

### Documentazione

- [ ] README.md aggiornato con tutte le strutture
- [ ] Ogni classe ha documentazione Doxygen completa
- [ ] Esempi d'uso per strutture principali
- [ ] Tabella di complessità in docs/
- [ ] CONTRIBUTING.md con linee guida

### Testing

- [ ] Test unitari per ogni struttura
- [ ] Test di integrazione per combinazioni
- [ ] Performance benchmarks documentati
- [ ] Edge cases coperti

### Repository

- [ ] .gitignore appropriato (ignora build/, binaries)
- [ ] Makefile funzionante e chiaro
- [ ] LICENSE file presente
- [ ] Branch main pulito e stabile
- [ ] Tag versione (v1.0.0)

---

## Risorse e Riferimenti

### Durante l'Implementazione

**Domande frequenti**:

- Unique_ptr vs shared_ptr? → Unique per ownership singola
- Quando usare const? → Sempre quando possibile
- Move o copy? → Move quando non serve mantenere originale
- Raw pointer o smart pointer? → Smart pointer per ownership

**Reference C++**:

- <https://en.cppreference.com/>
- <https://cplusplus.com/>

**Algoritmi**:

- Cormen et al., "Introduction to Algorithms"
- Sedgewick & Wayne, "Algorithms, 4th Edition"

### Dopo il Completamento

Il repository può essere esteso con:

1. **Algoritmi aggiuntivi**: Sort, graph algorithms avanzati
2. **Strutture persistenti**: Versioni immutable
3. **Concurrent versions**: Thread-safe variants
4. **Python bindings**: PyBind11 per uso da Python
5. **Visualization tools**: Graphviz export per debugging

---

## Conclusione

Questo piano fornisce una roadmap completa e dettagliata per completare AlgoDataStruct con tutte le strutture dati fondamentali.

**Punti di forza dell'approccio**:

- Progressione logica: dalle strutture semplici a quelle complesse
- Riuso: Strutture successive usano quelle precedenti
- Testing: Validazione continua ad ogni step
- Flessibilità: Ogni fase è indipendente

**Suggerimento finale**: Procedi con costanza, una struttura alla volta. La qualità è più importante della velocità. Un'implementazione ben testata e documentata vale molto di più di codice frettoloso.

Buon lavoro con Claude Code! 🚀
