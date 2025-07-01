# DoublyLinkedList

Una implementazione moderna e robusta di una lista doppiamente concatenata in C++, progettata con pattern di design avanzati per garantire sicurezza ed efficienza.

La classe `DoublyLinkedList<T>` è un'implementazione completa di una lista doppiamente concatenata. Offre un'interfaccia intuitiva per la gestione e l'accesso agli elementi, sfruttando i meccanismi moderni di C++ per garantire una gestione della memoria efficiente e sicura e un'elevata robustezza del codice.

## Caratteristiche Chiave

- **Template generico**: supporta qualsiasi tipo di dato.
- **Gestione automatica della memoria**: utilizza `std::unique_ptr` per la gestione della proprietà e per prevenire memory leak.
- **Iteratori bidirezionali**: pienamente compatibile con gli algoritmi della Standard Template Library (STL) e i `range-based for loops`.
- **Semantica di spostamento (Move-only)**: progettata per essere efficiente, evitando copie profonde e costose tramite costruttori e operatori di spostamento.
- **Sicurezza delle eccezioni**: gestione robusta delle eccezioni per mantenere uno stato consistente.
- **Performance ottimizzate**: complessità `O(1)` per le operazioni di inserimento e rimozione in testa e in coda.

## Dettagli Implementativi

### Struttura del Nodo

Il cuore della lista è la struttura `Node`, definita internamente, che gestisce i collegamenti e la proprietà dei dati:

```cpp
struct Node {
    T data;
    std::unique_ptr<Node> next;
    Node* prev; // Puntatore non proprietario
    
    // Costruttore variadico per l'emplace
    template <typename... Args>
    Node(Node* p, Args&&... args);
};
```

Punti significativi:

- **`std::unique_ptr<Node> next`**: un puntatore "forte" e proprietario al nodo successivo. Questo crea una catena di proprietà: la lista possiede la testa (`head_`), che a sua volta possiede il secondo nodo, e così via. Quando un `unique_ptr` viene distrutto, distrugge ricorsivamente il nodo che possiede, garantendo una deallocazione automatica e senza cicli di riferimento.
- **`Node* prev`**: un puntatore "raw" (grezzo) e non proprietario al nodo precedente. Questo puntatore serve solo per la navigazione all'indietro e non è coinvolto nella gestione della vita dell'oggetto, evitando così i cicli di riferimento che si verificherebbero con puntatori intelligenti bidirezionali (come `std::shared_ptr`).

### Gestione della memoria (RAII)

L'implementazione segue rigorosamente il principio **RAII (Resource Acquisition Is Initialization)**. La gestione della memoria è interamente automatizzata tramite `std::unique_ptr`, che assicura:

- **Proprietà esclusiva**: ogni nodo (tranne la testa) è posseduto da un solo altro nodo.
- **Deallocazione a cascata**: quando la lista viene distrutta, il `unique_ptr` `head_` viene deallocato. Questo innesca una catena di distruttori che dealloca in modo pulito e sicuro tutti i nodi della lista.
- **Nessun memory leak**: questo modello previene in modo elegante i memory leak senza la complessità e l'overhead dei `shared_ptr`.

### Tipo Move-Only

La classe `DoublyLinkedList` è un tipo **move-only**, il che significa che i costruttori di copia e gli operatori di assegnazione per copia sono stati deliberatamente disabilitati (`= delete`). Questa è una scelta di design consapevole per:

1. **Prevenire copie costose**: copiare una lista intera (deep copy) può essere un'operazione molto onerosa.
2. **Chiarezza semantica**: forzare l'uso della semantica di spostamento (`std::move`) rende esplicito il trasferimento di proprietà delle risorse, migliorando la leggibilità e la manutenibilità del codice.

### Sistema di Iteratori

La classe implementa un sistema completo di iteratori bidirezionali:

1. **`iterator`**: classe interna che implementa un iteratore standard per la lettura e la scrittura degli elementi. Supporta tutte le operazioni canoniche: dereferenziazione (`operator*`, `operator->`), incremento/decremento (pre e post) e confronto (`==`, `!=`).
2. **`const_iterator`**: versione dell'iteratore per la navigazione in sola lettura, essenziale per garantire la `const-correctness` e permettere l'uso di algoritmi su istanze costanti della lista.

Gli iteratori sono compatibili con gli algoritmi STL e abilitano l'uso dei `range-based for loop`.

### Operazioni Principali

#### Inserimento

- **`push_front()` / `push_back()`**: inserimento efficiente `O(1)` in testa e in coda, con versioni per copia e spostamento.
- **`emplace_front()` / `emplace_back()`**: costruzione "in loco" degli elementi, evitando copie o spostamenti intermedi.
- **`insert()`**: inserimento in una posizione arbitraria specificata da un iteratore, con complessità `O(1)`.

#### Rimozione

- **`pop_front()` / `pop_back()`**: rimozione efficiente `O(1)` dalla testa e dalla coda. Lanciano un'eccezione `ListException` se la lista è vuota.
- **`erase()`**: rimozione di un elemento in posizione arbitraria `O(1)` dato un iteratore. Restituisce un iteratore all'elemento successivo.
- **`clear()`**: rimozione di tutti gli elementi in `O(n)`.

#### Accesso e Stato

- **`front()` / `back()`**: accesso diretto `O(1)` al primo e all'ultimo elemento.
- **`is_empty()` / `size()`**: controllo dello stato in `O(1)` grazie al caching della dimensione.

### Complessità Computazionale

| Operazione | Complessità Temporale | Note |
|---|---|---|
| `push_front` / `push_back` | O(1) | Inserimento costante in testa/coda. |
| `pop_front` / `pop_back` | O(1) | Rimozione costante da testa/coda. |
| `insert` (con iteratore) | O(1) | Complessità costante data la posizione. |
| `erase` (con iteratore) | O(1) | Complessità costante data la posizione. |
| `size` / `is_empty` | O(1) | Tempo costante grazie al caching della dimensione. |
| `reverse` | O(n) | Scansione lineare della lista. |
| `clear` | O(n) | Deve deallocare tutti i nodi. |

## Conclusioni

In ultima analisi, questa implementazione della lista doppiamente concatenata rappresenta un esempio di design moderno in C++, bilanciando funzionalità, sicurezza e performance. L'uso di `std::unique_ptr`, semantica move-only e un'interfaccia pulita la rendono uno strumento robusto e facilmente adattabile a diversi contesti di utilizzo.

## Esempio d'uso

```cpp
#include "DoublyLinkedList.hpp" // Assumendo i path corretti
#include <string>
#include <iostream>

int main() {
    // Creazione e inizializzazione
    ads::list::DoublyLinkedList<std::string> names;
    names.push_back("Alice");
    names.push_back("Bob");
    names.push_back("Charlie");

    // Aggiunta di elementi
    names.push_front("Zoe");
    names.emplace_back("Diana"); // Costruzione sul posto

    // Iterazione e stampa
    std::cout << "Nomi: ";
    for (const auto& name : names) {
        std::cout << name << " ";
    }
    std::cout << std::endl; // Output: Zoe Alice Bob Charlie Diana

    // Rimozione di un elemento
    auto it = names.begin();
    ++it; // it punta ad "Alice"
    it = names.erase(it); // Rimuove "Alice", it ora punta a "Bob"

    std::cout << "Dopo la rimozione: ";
    for (const auto& name : names) {
        std::cout << name << " ";
    }
    std::cout << std::endl; // Output: Zoe Bob Charlie Diana

    return 0;
}
```
