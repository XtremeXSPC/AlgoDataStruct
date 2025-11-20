# Fase 1: Liste Semplici e Heap

## Obiettivi

Implementare le strutture dati lineari fondamentali mancanti e gli heap come base per code prioritarie.

**Strutture da implementare:**

1. Singly Linked List
2. Min Heap (array-based)
3. Max Heap (array-based)

## 1. Singly Linked List

### Descrizione

Lista concatenata semplice dove ogni nodo ha un puntatore solo al nodo successivo (non al precedente). Più semplice e leggera della Doubly Linked List già implementata, ma con limitazioni: navigazione solo in avanti, nessun iteratore bidirezionale.

### Posizione File

```
include/ads/lists/Singly_Linked_List.hpp
src/ads/lists/Singly_Linked_List.tpp
src/main_Singly_Linked_List.cc
```

### Struttura Nodo

Il nodo deve contenere:

- `T data` - il dato memorizzato
- `std::unique_ptr<Node> next` - puntatore al prossimo nodo (ownership)

### Membri Privati della Lista

- `std::unique_ptr<Node> head_` - puntatore al primo nodo
- `Node* tail_` - puntatore raw all'ultimo nodo (per push_back efficiente)
- `size_t size_` - numero di elementi

### Operazioni da Implementare

#### Costruttori e Distruttori

- `SinglyLinkedList()` - costruttore default
- `~SinglyLinkedList()` - distruttore (iterativo per evitare stack overflow)
- `SinglyLinkedList(SinglyLinkedList&&)` - move constructor
- `operator=(SinglyLinkedList&&)` - move assignment
- Disabilitare copy constructor e copy assignment (non necessari per ora)

#### Inserimento

- `void push_front(const T&)` - inserisce all'inizio
- `void push_front(T&&)` - versione move
- `void push_back(const T&)` - inserisce alla fine
- `void push_back(T&&)` - versione move
- `template<typename... Args> T& emplace_front(Args&&...)` - costruisce in-place all'inizio
- `template<typename... Args> T& emplace_back(Args&&...)` - costruisce in-place alla fine

#### Rimozione

- `void pop_front()` - rimuove il primo elemento (throw se vuota)
- `void pop_back()` - rimuove l'ultimo elemento (throw se vuota, O(n)!)

#### Accesso

- `T& front()` - accesso al primo elemento (const e non-const)
- `T& back()` - accesso all'ultimo elemento (const e non-const)

#### Query

- `bool is_empty() const noexcept` - verifica se vuota
- `size_t size() const noexcept` - restituisce dimensione
- `void clear() noexcept` - rimuove tutti gli elementi

#### Utility

- `void reverse() noexcept` - inverte l'ordine (modifica puntatori, non copia dati)

### Iteratore

Implementare un **forward iterator** (solo direzione avanti):

- Operatori: `*`, `->`, `++` (pre e post), `==`, `!=`
- Metodi della lista: `begin()`, `end()`, `cbegin()`, `cend()`
- **NON** implementare `operator--` (lista singola non supporta andare indietro)

### Complessità Attese

- `push_front`: O(1)
- `push_back`: O(1) grazie al puntatore tail
- `pop_front`: O(1)
- `pop_back`: O(n) - necessario attraversare tutta la lista per trovare penultimo
- `front`, `back`: O(1)
- `size`: O(1)
- `reverse`: O(n)

### Dettagli Implementativi Critici

**pop_back è O(n)**: Non c'è modo di rendere pop_back O(1) in una singly linked list perché per aggiornare `tail_` dopo la rimozione dell'ultimo elemento, dobbiamo trovare il penultimo nodo, che richiede di attraversare l'intera lista.

**Reverse**: L'algoritmo deve invertire i puntatori next di tutti i nodi. Approccio iterativo:

1. Mantieni tre puntatori: `prev` (nullptr inizialmente), `current` (head), `next`
2. Per ogni nodo: salva `next`, inverti `current->next` per puntare a `prev`, avanza
3. Aggiorna `head_` e `tail_`

**Distruzione iterativa**: Nel distruttore, usa un loop per deallocare nodi invece di fare affidamento sulla distruzione ricorsiva di unique_ptr (che può causare stack overflow con liste molto lunghe).

### Test da Includere

1. Inserimenti/rimozioni base (front e back)
2. Test con lista vuota (eccezioni)
3. Iterazione con range-based for
4. Test di reverse
5. Move semantics
6. Grandi quantità di elementi (verificare no stack overflow)

---

## 2. Min Heap e Max Heap

### Descrizione

Heap binario implementato con array. Il Min Heap mantiene il minimo alla radice (parent ≤ children), il Max Heap mantiene il massimo alla radice (parent ≥ children). Fondamentali per priority queue e algoritmi di ordinamento (heapsort).

### Posizione File

```
include/ads/heaps/Min_Heap.hpp
include/ads/heaps/Max_Heap.hpp
src/ads/heaps/Min_Heap.tpp
src/ads/heaps/Max_Heap.tpp
src/main_Heaps.cc
```

### Rappresentazione Array

L'heap è un albero binario completo memorizzato in un array:

- Radice: indice 0
- Per un nodo all'indice `i`:
  - Parent: `(i - 1) / 2`
  - Left child: `2 * i + 1`
  - Right child: `2 * i + 2`

### Membri Privati

- `std::unique_ptr<T[], void(*)(T*)> data_` - array dinamico con custom deleter
- `size_t size_` - numero di elementi attuali
- `size_t capacity_` - capacità allocata
- `static constexpr size_t kInitialCapacity = 16`
- `static constexpr size_t kGrowthFactor = 2`

### Operazioni da Implementare

#### Costruttori

- `MinHeap(size_t initial_capacity = kInitialCapacity)` - costruttore con capacità iniziale
- `MinHeap(const std::vector<T>&)` - costruzione da vector (heapify in O(n))
- Move constructor e move assignment
- Distruttore che dealloca array e distrugge elementi

#### Inserimento

- `void insert(const T&)` - inserisce elemento mantenendo proprietà heap
- `void insert(T&&)` - versione move
- `template<typename... Args> T& emplace(Args&&...)` - costruisce in-place

#### Rimozione

- `void extract_min()` / `extract_max()` - rimuove e restituisce radice (throw se vuoto)
- `T& top()` - accesso alla radice senza rimozione (const e non-const)

#### Query

- `bool is_empty() const noexcept`
- `size_t size() const noexcept`
- `void clear() noexcept`

#### Operazioni Avanzate (opzionali ma consigliate)

- `void decrease_key(size_t index, const T& new_value)` - diminuisce valore (solo MinHeap)
- `void increase_key(size_t index, const T& new_value)` - aumenta valore (solo MaxHeap)

### Algoritmi Interni

#### heapify_up (bubble up)

Usato dopo insert: sposta l'elemento in su finché la proprietà heap è soddisfatta.

```
Pseudocodice (MinHeap):
heapify_up(index):
  while index > 0:
    parent = (index - 1) / 2
    if data[index] >= data[parent]:
      break
    swap(data[index], data[parent])
    index = parent
```

#### heapify_down (bubble down)

Usato dopo extract: sposta l'elemento in giù finché la proprietà heap è soddisfatta.

```
Pseudocodice (MinHeap):
heapify_down(index):
  while true:
    left = 2 * index + 1
    right = 2 * index + 2
    smallest = index

    if left < size && data[left] < data[smallest]:
      smallest = left
    if right < size && data[right] < data[smallest]:
      smallest = right

    if smallest == index:
      break

    swap(data[index], data[smallest])
    index = smallest
```

#### build_heap (heapify)

Costruisce heap da array non ordinato in O(n):

```
Pseudocodice:
build_heap():
  // Inizia dal primo nodo non-foglia e vai indietro
  for i from (size/2 - 1) down to 0:
    heapify_down(i)
```

### Gestione Memoria

**Allocazione**: Usare `operator new[]` per allocare memoria raw, poi placement new per costruire elementi.

**Grow**: Quando `size_ == capacity_`, riallocare con capacità doppia:

1. Alloca nuovo array
2. Move (o copy se T non è nothrow move constructible) elementi
3. Distruggi vecchi elementi
4. Dealloca vecchio array

**Clear**: Distruggi esplicitamente tutti gli elementi chiamando distruttori, poi resetta size a 0.

### Complessità Attese

- `insert`: O(log n) ammortizzato
- `extract_min/max`: O(log n)
- `top`: O(1)
- `build_heap` (costruttore da vector): O(n)
- `is_empty`, `size`: O(1)

### MinHeap vs MaxHeap

Le due classi sono quasi identiche, differiscono solo nella comparazione:

- MinHeap: `data[i] < data[parent]` per heap property
- MaxHeap: `data[i] > data[parent]` per heap property

Si può considerare di creare una classe template base `Heap<T, Compare>` e poi specializzare con `std::less<T>` e `std::greater<T>`, ma per scopi didattici due classi separate sono più chiare.

### Test da Includere

1. Inserimenti multipli e verifica ordinamento
2. Costruzione da vector disordinato
3. Extract ripetuti fino a svuotamento
4. Test con heap vuoto (eccezioni)
5. Test con tipi custom (struct con operator<)
6. Verifica performance con grandi quantità di dati
7. Heapsort (test applicativo): inserisci n elementi, extract n volte

---

## Note Implementative Comuni

### Exception Safety

Tutte le operazioni che possono fallire (pop su struttura vuota, accesso a front/back/top su struttura vuota) devono lanciare eccezioni appropriate:

```cpp
namespace ads {
namespace list {
  class ListException : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };
}

namespace heap {
  class HeapException : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };
}
}
```

### Move Semantics

Per tutte le strutture:

- Dopo un move, l'oggetto sorgente deve essere in uno stato valido ma non specificato
- Tipicamente: puntatori a nullptr, size a 0
- Il destinatario assume ownership completa delle risorse

### File Main di Test

Creare `src/main_Phase1.cc` che testa tutte le strutture della Fase 1:

- Sezioni separate per ogni struttura
- Output formattato per leggibilità
- Test di correttezza e performance
- Gestione eccezioni

### Aggiornamento README

Dopo implementazione, aggiornare la sezione "Implemented Data Structures" nel README.md:

- Rimuovere (+) da Linear Structures e Heaps
- Aggiungere checkmarks alle strutture completate

## Checklist Completamento Fase 1

- [ ] Singly Linked List implementata
- [ ] Singly Linked List testata completamente
- [ ] Min Heap implementato
- [ ] Max Heap implementato
- [ ] Heap testati completamente
- [ ] File main_Phase1.cc creato e funzionante
- [ ] Tutti i test passano senza memory leak (valgrind)
- [ ] README.md aggiornato
- [ ] Documentazione Doxygen completa per tutte le classi

## Stima Temporale

- Singly Linked List: 4-6 ore
- Min/Max Heap: 6-8 ore
- Testing e debugging: 4-5 ore
- Documentazione: 2-3 ore

**Totale Fase 1**: 16-22 ore (2-3 giorni)
