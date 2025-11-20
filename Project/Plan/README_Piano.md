# AlgoDataStruct: Piano di Sviluppo Completo

## Indice dei Documenti

Questo piano è composto da 5 documenti organizzati per facilitare l'implementazione progressiva:

### 📋 00_Piano_Generale.md

**Panoramica del progetto e organizzazione**

- Obiettivi e scope del piano
- Stato attuale del repository
- Strutture da implementare (13 totali)
- Organizzazione in 3 fasi
- Principi guida e convenzioni
- Roadmap temporale (7-10 giorni)

### 🔨 01_Fase1_Liste_Heap.md

**Fase 1: Fondamenta (2-3 giorni)**

Strutture da implementare:

- **Singly Linked List**: Lista concatenata semplice con iteratore forward
- **Min Heap**: Heap binario minimo array-based
- **Max Heap**: Heap binario massimo array-based

Include:

- Specifiche dettagliate per ogni struttura
- Membri privati e operazioni pubbliche
- Algoritmi chiave (heapify_up, heapify_down, build_heap)
- Complessità attese
- Dettagli critici implementativi
- Checklist di completamento

### ⚖️ 02_Fase2_AVL_HashTable.md

**Fase 2: Strutture Intermedie (4-5 giorni)**

Strutture da implementare:

- **AVL Tree**: Albero binario auto-bilanciante con rotazioni
- **Hash Table (Chaining)**: Dizionario con gestione collisioni tramite liste
- **Hash Table (Open Addressing)**: Dizionario con probing (lineare, quadratico, double hashing)

Include:

- Implementazione dettagliata delle 4 rotazioni AVL (LL, RR, LR, RL)
- Algoritmi di bilanciamento per insert e remove
- Gestione del load factor e rehashing
- Strategie di probing e tombstones
- Confronto tra le diverse implementazioni

### 🌐 03_Fase3_Grafi_HashMap_PriorityQueue.md

**Fase 3: Strutture Composite (5-7 giorni)**

Strutture da implementare:

- **Graph**: Due rappresentazioni (Adjacency List e Adjacency Matrix)
- **HashMap**: Dizionario con iteratori, API user-friendly
- **Priority Queue**: Coda con priorità basata su heap

Include:

- Supporto per grafi diretti/non-diretti, pesati/non-pesati
- Algoritmi BFS, DFS, path finding
- Implementazione completa di iteratori per HashMap
- Applicazioni integrate (Dijkstra, word frequency, task scheduling)
- Test di integrazione tra strutture

### 🚀 04_Guida_Implementazione.md

**Guida pratica per Claude Code**

Include:

- Workflow dettagliato per implementazione
- Ordine consigliato per ogni fase
- Pattern comuni e riuso del codice
- Guidelines per debugging e testing
- Convenzioni di stile e naming
- Suggerimenti per performance
- Prompt efficaci per Claude Code
- Checklist finale pre-consegna

---

## Quick Start

### Per iniziare immediatamente

1. **Leggi** `00_Piano_Generale.md` per visione d'insieme
2. **Scegli** la fase da cui partire (consigliato: Fase 1)
3. **Apri** il documento della fase selezionata
4. **Segui** le specifiche per la prima struttura
5. **Usa** `04_Guida_Implementazione.md` come riferimento durante lo sviluppo

### Approccio Consigliato

```
Settimana 1: Fase 1
├─ Giorno 1-2: Singly Linked List
├─ Giorno 2-3: Min Heap
└─ Giorno 3: Max Heap + testing

Settimana 2: Fase 2
├─ Giorno 1-3: AVL Tree (la più complessa)
├─ Giorno 3-4: Hash Table Chaining
└─ Giorno 4-5: Hash Table Open Addressing

Settimana 3: Fase 3
├─ Giorno 1-2: Graph (entrambe rappresentazioni)
├─ Giorno 3-4: HashMap + Priority Queue
└─ Giorno 5-7: Testing integrazione, performance, documentazione
```

---

## Strutture per Categoria

### Lineari

- ✅ Doubly Linked List (esistente)
- 🔨 Singly Linked List (Fase 1)
- ✅ Stack - Array e Linked (esistenti)
- ✅ Queue - Circular Array e Linked (esistenti)

### Alberi

- ✅ Binary Search Tree (esistente)
- ⚖️ AVL Tree (Fase 2)

### Heap

- 🔨 Min Heap (Fase 1)
- 🔨 Max Heap (Fase 1)

### Hash-based

- ⚖️ Hash Table Chaining (Fase 2)
- ⚖️ Hash Table Open Addressing (Fase 2)
- 🌐 HashMap (Fase 3)

### Grafi

- 🌐 Graph Adjacency List (Fase 3)
- 🌐 Graph Adjacency Matrix (Fase 3)

### Code Prioritarie

- 🌐 Priority Queue (Fase 3)

**Legenda**: ✅ Esistente | 🔨 Fase 1 | ⚖️ Fase 2 | 🌐 Fase 3

---

## Complessità a Colpo d'Occhio

### Operazioni Base

| Struttura          | Insert      | Delete      | Search    | Access |
| ------------------ | ----------- | ----------- | --------- | ------ |
| Singly Linked List | O(1)ᶠ O(n)ᵇ | O(1)ᶠ O(n)ᵇ | O(n)      | O(n)   |
| Doubly Linked List | O(1)        | O(1)        | O(n)      | O(n)   |
| Binary Search Tree | O(h)*       | O(h)*       | O(h)*     | -      |
| AVL Tree           | O(log n)    | O(log n)    | O(log n)  | -      |
| Min/Max Heap       | O(log n)    | O(log n)    | O(n)      | O(1)ᵗ  |
| Hash Table         | O(1)ᵃ       | O(1)ᵃ       | O(1)ᵃ     | O(1)ᵃ  |
| Graph (Adj List)   | O(1)ᵉ       | O(degree)   | O(degree) | -      |
| Graph (Adj Matrix) | O(1)ᵉ       | O(1)        | O(1)      | -      |
| Priority Queue     | O(log n)    | O(log n)    | -         | O(1)ᵗ  |

**Note**:

- ᶠ front operation
- ᵇ back operation
- ᵃ average case (worst case O(n))
- ᵉ edge insertion
- ᵗ top/min/max access
- \* h = height (log n per AVL, fino a n per BST sbilanciato)

---

## Metriche di Successo

### Obiettivi Quantitativi

- **13 strutture** completamente implementate
- **13+ file main** di test (uno per struttura + integrazione)
- **Copertura test** > 80% delle operazioni
- **Zero memory leaks** verificati con valgrind
- **Performance** comparabile o migliore di std:: equivalenti

### Obiettivi Qualitativi

- Codice **leggibile e ben commentato**
- **Documentazione** Doxygen completa
- **Esempi** applicativi per ogni struttura principale
- **Consistenza** di stile e convenzioni
- **Didatticità**: codice che insegna, non solo funziona

---

## Dipendenze tra Fasi

```
Fase 1 (Fondamenta)
    ↓
    ├─→ Fase 2 (AVL, Hash Tables)
    │   ↓
    └───┴─→ Fase 3 (Grafi, HashMap, Priority Queue)
            ↓
        [Repository Completo]
```

**Nota**: Fase 2 e Fase 3 possono iniziare solo dopo completamento Fase 1, ma all'interno di ogni fase l'ordine è flessibile (suggerito ma non vincolante).

---

## Files da Creare (37 totali)

### Fase 1 (9 files)

```
include/ads/lists/Singly_Linked_List.hpp
src/ads/lists/Singly_Linked_List.tpp
src/main_Singly_Linked_List.cc

include/ads/heaps/Min_Heap.hpp
src/ads/heaps/Min_Heap.tpp

include/ads/heaps/Max_Heap.hpp
src/ads/heaps/Max_Heap.tpp

src/main_Heaps.cc
src/main_Phase1.cc
```

### Fase 2 (10 files)

```
include/ads/trees/AVL_Tree.hpp
src/ads/trees/AVL_Tree.tpp
src/main_AVL_Tree.cc

include/ads/hash/Hash_Table_Chaining.hpp
src/ads/hash/Hash_Table_Chaining.tpp
src/main_Hash_Table_Chaining.cc

include/ads/hash/Hash_Table_Open_Addressing.hpp
src/ads/hash/Hash_Table_Open_Addressing.tpp
src/main_Hash_Table_Open_Addressing.cc

src/main_Phase2_Performance.cc
```

### Fase 3 (13 files)

```
include/ads/graphs/Graph_Adjacency_List.hpp
src/ads/graphs/Graph_Adjacency_List.tpp

include/ads/graphs/Graph_Adjacency_Matrix.hpp
src/ads/graphs/Graph_Adjacency_Matrix.tpp

src/main_Graphs.cc

include/ads/associative/Hash_Map.hpp
src/ads/associative/Hash_Map.tpp
src/main_Hash_Map.cc

include/ads/queues/Priority_Queue.hpp
src/ads/queues/Priority_Queue.tpp
src/main_Priority_Queue.cc

src/main_Phase3_Integration.cc
```

### Documentazione (5 files)

```
docs/complexity_table.md
examples/dijkstra.cc
examples/word_frequency.cc
examples/task_scheduler.cc
examples/social_network.cc
```

---

## Risorse Aggiuntive

### Durante lo Sviluppo

**Per domande su C++**:

- [cppreference.com](https://en.cppreference.com/) - Reference completo
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/) - Best practices

**Per algoritmi**:

- Cormen et al., "Introduction to Algorithms" (CLRS)
- Goodrich et al., "Data Structures and Algorithms in C++"

**Per debugging**:

- Valgrind documentation
- GDB quick reference

### Dopo il Completamento

Il repository può diventare base per:

- **Portfolio personale**: Dimostra competenze solide
- **Preparazione colloqui**: Implementazioni pronte da rivedere
- **Insegnamento**: Materiale didattico per altri
- **Progetti più grandi**: Fondamenta riusabili

---

## Contatti e Supporto

Per domande durante l'implementazione:

1. Consulta i documenti specifici della fase
2. Rivedi la Guida Implementazione per pattern comuni
3. Usa Claude Code con prompt mirati (vedi 04_Guida_Implementazione.md)
4. Se necessario, apri issue nel repository con dettagli specifici

---

## Note Finali

Questo piano è stato progettato per essere:

- **Completo**: Copre tutte le strutture fondamentali
- **Progressivo**: Da semplice a complesso
- **Pratico**: Focus su implementazione concreta
- **Didattico**: Spiega il "perché" oltre al "come"
- **Testabile**: Ogni fase verificabile indipendentemente

**Obiettivo finale**: Un repository di strutture dati di qualità professionale, completamente testato, ben documentato, e pronto per uso didattico o in progetti reali.

Ricorda: **Qualità > Velocità**. Meglio 3 strutture perfette che 10 mediocri.

---

## Versioning

- **v1.0**: Piano completo in 3 fasi (questo documento)
- Aggiornamenti futuri potrebbero includere:
  - Strutture dati avanzate (B-Tree, Trie avanzato, etc.)
  - Versioni thread-safe
  - Benchmark più estensivi
  - Binding per altri linguaggi

**Stato attuale**: Piano pronto per implementazione

**Last updated**: 2025-01-20

---

**Buon sviluppo!**
