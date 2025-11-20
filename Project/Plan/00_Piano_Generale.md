# Piano di Sviluppo: Strutture Dati Fondamentali

## Obiettivo

Completare il repository **AlgoDataStruct** con le strutture dati fondamentali necessarie per un corso base di Algoritmi e Strutture Dati, mantenendo la qualità e lo stile già stabiliti nelle implementazioni esistenti.

## Stato Attuale

Il repository contiene già implementazioni complete e ben testate di:

- **Doubly Linked List** - Lista doppiamente linkata con iteratori bidirezionali
- **Stack** - Due implementazioni (array-based e linked-based)
- **Queue** - Due implementazioni (circular array e linked-based)
- **Binary Search Tree** - Albero binario di ricerca con iteratori in-order

## Strutture da Implementare

### Priorità 1: Strutture Lineari e Heap

1. **Singly Linked List** - Versione semplificata della lista
2. **Min Heap e Max Heap** - Strutture heap fondamentali per code prioritarie

### Priorità 2: Alberi Bilanciati e Hashing

3. **AVL Tree** - Primo albero auto-bilanciante con rotazioni
4. **Hash Table** - Implementazione con chaining e open addressing

### Priorità 3: Grafi e Dizionari

5. **Graph** - Rappresentazioni con adjacency list e adjacency matrix
6. **HashMap/Dictionary** - Implementazione concreta di dizionario
7. **Priority Queue** - Interfaccia basata su heap

## Organizzazione del Lavoro

Il piano è suddiviso in **tre fasi implementative**:

### Fase 1: Liste e Heap (Fondamenta)

- Completamento delle liste base
- Implementazione degli heap come prerequisito per altre strutture
- Focus su correttezza e gestione della memoria

### Fase 2: AVL e Hash Table (Strutture Intermedie)

- Introduzione al bilanciamento negli alberi
- Implementazione dell'hashing come tecnica fondamentale
- Focus su complessità e performance

### Fase 3: Grafi e Dizionari (Strutture Composite)

- Rappresentazioni dei grafi
- Dizionari come astrazione di alto livello
- Focus su versatilità e casi d'uso

## Principi Guida

Ogni implementazione deve seguire i pattern già stabiliti:

1. **Gestione Memoria Moderna**
   - Uso di `std::unique_ptr` per ownership automatica
   - Move semantics per efficienza
   - RAII per acquisizione/rilascio risorse

2. **Interfacce Consistenti**
   - Separazione header/implementation per template
   - Metodi const/non-const appropriati
   - Gestione eccezioni uniforme

3. **Iteratori STL-like**
   - Quando applicabile, fornire iteratori conformi
   - Supporto per range-based for loop
   - Iteratori const per immutabilità

4. **Testing Completo**
   - File main dedicato per ogni struttura
   - Test di operazioni base
   - Test di edge cases ed eccezioni
   - Test di move semantics

5. **Documentazione**
   - Commenti Doxygen-style
   - Complessità temporale e spaziale
   - Esempi d'uso nel main

## Dettagli Implementativi

### Namespace e Organizzazione

```
ads/
├── linear/      (liste)
├── trees/       (BST, AVL)
├── heaps/       (min/max heap)
├── hash/        (hash table, hash map)
├── graphs/      (graph representations)
└── utils/       (utilità condivise)
```

### Convenzioni di Naming

- Classi: `PascalCase` (es. `SinglyLinkedList`)
- Metodi: `snake_case` (es. `push_front`)
- Membri privati: `snake_case_` con underscore finale
- Template: `typename T` per tipi generici

### Gestione Eccezioni

Ogni categoria di strutture ha le proprie eccezioni custom:

- `ListException` per liste
- `StackException`, `QueueException` per stack/queue
- `TreeException` per alberi
- `GraphException` per grafi
- Tutte derivano da `std::runtime_error`

## Roadmap Temporale

- **Fase 1**: 2-3 giorni (liste semplici, heap base)
- **Fase 2**: 3-4 giorni (AVL più complesso, hash table con varianti)
- **Fase 3**: 2-3 giorni (grafi versatili, dizionari)

**Totale stimato**: 7-10 giorni di sviluppo

## Note Importanti

- Ogni fase può essere completata indipendentemente
- Le fasi successive possono usare strutture delle fasi precedenti
- Priority Queue (Fase 3) dipende da Heap (Fase 1)
- HashMap (Fase 3) dipende da Hash Table (Fase 2)
- Focus sulla didattica: codice chiaro e ben commentato
- Performance secondaria alla chiarezza, ma comunque ottimale

## Prossimi Passi

1. Leggere il documento della Fase 1
2. Implementare le strutture seguendo le specifiche
3. Testare completamente prima di passare alla fase successiva
4. Aggiornare README.md con le nuove strutture implementate
