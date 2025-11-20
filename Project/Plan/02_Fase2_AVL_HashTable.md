# Fase 2: Alberi Bilanciati e Hashing

## Obiettivi

Implementare strutture dati intermedie che introducono concetti di bilanciamento automatico e hashing.

**Strutture da implementare:**

1. AVL Tree
2. Hash Table (con Chaining)
3. Hash Table (con Open Addressing)

## 1. AVL Tree

### Descrizione

Albero binario di ricerca auto-bilanciante dove la differenza di altezza tra sottoalberi sinistro e destro di ogni nodo (fattore di bilanciamento) non supera mai 1. Garantisce operazioni in O(log n) nel caso peggiore, a differenza del BST non bilanciato che può degradare a O(n).

### Posizione File

```
include/ads/trees/AVL_Tree.hpp
src/ads/trees/AVL_Tree.tpp
src/main_AVL_Tree.cc
```

### Struttura Nodo

Il nodo estende quello del BST con informazione aggiuntiva:

```cpp
struct Node {
  T data;
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
  int height;  // Altezza del sottoalbero radicato in questo nodo

  Node(const T& val) : data(val), left(nullptr), right(nullptr), height(1) {}
  Node(T&& val) : data(std::move(val)), left(nullptr), right(nullptr), height(1) {}
};
```

**Nota**: L'altezza di una foglia è 1, di un nodo nullptr è 0.

### Membri Privati

- `std::unique_ptr<Node> root_` - radice dell'albero
- `size_t size_` - numero di nodi

### Operazioni Pubbliche

Stesse del BST:

- `bool insert(const T&)` / `insert(T&&)` / `emplace(Args&&...)`
- `bool remove(const T&)`
- `bool contains(const T&) const`
- `const T& find_min() const`
- `const T& find_max() const`
- `void clear() noexcept`
- `bool is_empty() const noexcept`
- `size_t size() const noexcept`
- `int height() const noexcept`
- Traversal: `in_order`, `pre_order`, `post_order`, `level_order`
- Iteratori in-order come BST

### Operazioni Private Aggiuntive

#### Gestione Altezze

```cpp
int get_height(const Node* node) const noexcept {
  return node ? node->height : 0;
}

void update_height(Node* node) noexcept {
  if (node) {
    node->height = 1 + std::max(get_height(node->left.get()),
                                  get_height(node->right.get()));
  }
}
```

#### Fattore di Bilanciamento

```cpp
int get_balance_factor(const Node* node) const noexcept {
  return node ? get_height(node->left.get()) - get_height(node->right.get()) : 0;
}
```

**Interpretazione**:

- Balance factor > 1: sottoalbero sinistro troppo alto (left-heavy)
- Balance factor < -1: sottoalbero destro troppo alto (right-heavy)
- Balance factor ∈ [-1, 1]: nodo bilanciato

#### Rotazioni

Le rotazioni sono la chiave del bilanciamento AVL. Modificano la struttura dell'albero preservando l'ordine BST.

**Rotazione Destra** (Right Rotation):

```
     y                    x
    / \                  / \
   x   C    ===>        A   y
  / \                      / \
 A   B                    B   C
```

Implementazione:

```cpp
std::unique_ptr<Node> rotate_right(std::unique_ptr<Node> y) {
  auto x = std::move(y->left);
  y->left = std::move(x->right);

  update_height(y.get());
  update_height(x.get());

  x->right = std::move(y);
  return x;
}
```

**Rotazione Sinistra** (Left Rotation):

```
   x                      y
  / \                    / \
 A   y      ===>        x   C
    / \                / \
   B   C              A   B
```

Implementazione analoga a rotate_right ma speculare.

**Rotazione Doppia Sinistra-Destra** (Left-Right):
Quando inserimento causa sbilanciamento left-right:

1. Ruota sinistra sul figlio sinistro
2. Ruota destra sul nodo

```cpp
std::unique_ptr<Node> rotate_left_right(std::unique_ptr<Node> node) {
  node->left = rotate_left(std::move(node->left));
  return rotate_right(std::move(node));
}
```

**Rotazione Doppia Destra-Sinistra** (Right-Left):
Analoga ma speculare.

### Algoritmi di Insert e Remove

#### Insert

Dopo l'inserimento ricorsivo (come BST), risalendo:

1. Aggiorna altezza del nodo corrente
2. Calcola balance factor
3. Se |balance factor| > 1, applica rotazioni appropriate

**Quattro casi di sbilanciamento**:

1. **Left-Left (LL)**: Inserimento nel sottoalbero sinistro del figlio sinistro
   - Soluzione: rotazione destra

2. **Left-Right (LR)**: Inserimento nel sottoalbero destro del figlio sinistro
   - Soluzione: rotazione sinistra-destra

3. **Right-Right (RR)**: Inserimento nel sottoalbero destro del figlio destro
   - Soluzione: rotazione sinistra

4. **Right-Left (RL)**: Inserimento nel sottoalbero sinistro del figlio destro
   - Soluzione: rotazione destra-sinistra

```cpp
std::unique_ptr<Node> insert_helper(std::unique_ptr<Node> node, const T& value, bool& inserted) {
  // Base case: inserimento standard BST
  if (!node) {
    inserted = true;
    return std::make_unique<Node>(value);
  }

  // Ricorsione BST standard
  if (value < node->data) {
    node->left = insert_helper(std::move(node->left), value, inserted);
  } else if (value > node->data) {
    node->right = insert_helper(std::move(node->right), value, inserted);
  } else {
    inserted = false;
    return node; // Duplicato
  }

  // Aggiorna altezza
  update_height(node.get());

  // Bilanciamento
  int balance = get_balance_factor(node.get());

  // Left-Left case
  if (balance > 1 && value < node->left->data) {
    return rotate_right(std::move(node));
  }

  // Right-Right case
  if (balance < -1 && value > node->right->data) {
    return rotate_left(std::move(node));
  }

  // Left-Right case
  if (balance > 1 && value > node->left->data) {
    return rotate_left_right(std::move(node));
  }

  // Right-Left case
  if (balance < -1 && value < node->right->data) {
    return rotate_right_left(std::move(node));
  }

  return node;
}
```

#### Remove

Simile all'insert: dopo rimozione BST standard, risalendo applica rotazioni per mantenere bilanciamento.

La differenza principale: potrebbero servire multiple rotazioni mentre si risale (non solo una come nell'insert).

### Complessità

Tutte le operazioni (insert, remove, search): **O(log n)** garantito nel caso peggiore, grazie al bilanciamento che mantiene altezza massima ≈ 1.44 * log(n).

### Test da Includere

1. Inserimenti che causano tutti e 4 i tipi di rotazioni
2. Sequenza ordinata crescente/decrescente (verifica bilanciamento automatico)
3. Rimozioni multiple con verifica bilanciamento
4. Confronto altezza AVL vs BST per stessi dati
5. Operazioni su grande dataset (milioni di elementi)
6. Verifica che in-order traversal restituisce elementi ordinati

---

## 2. Hash Table con Chaining

### Descrizione

Struttura dati che implementa un dizionario (mappa chiave-valore) con accesso medio O(1). Usa una funzione hash per mappare chiavi a indici di un array. Le collisioni (chiavi diverse che mappano allo stesso indice) sono gestite con liste concatenate (chaining).

### Posizione File

```
include/ads/hash/Hash_Table_Chaining.hpp
src/ads/hash/Hash_Table_Chaining.tpp
src/main_Hash_Table_Chaining.cc
```

### Struttura

```cpp
template <typename Key, typename Value>
class HashTableChaining {
private:
  struct Entry {
    Key key;
    Value value;
    Entry(const Key& k, const Value& v) : key(k), value(v) {}
    Entry(Key&& k, Value&& v) : key(std::move(k)), value(std::move(v)) {}
  };

  using Bucket = std::list<Entry>;
  std::unique_ptr<Bucket[]> buckets_;
  size_t capacity_;
  size_t size_;
  float max_load_factor_;

  static constexpr size_t kInitialCapacity = 16;
  static constexpr float kDefaultMaxLoadFactor = 0.75f;
};
```

### Funzione Hash

Implementare hash generico usando `std::hash<Key>`:

```cpp
size_t hash(const Key& key) const {
  return std::hash<Key>{}(key) % capacity_;
}
```

Per chiavi custom, l'utente deve specializzare `std::hash` o fornire un functor custom.

### Load Factor e Rehashing

**Load factor** = size / capacity (numero medio di elementi per bucket)

Quando load factor supera `max_load_factor_`, eseguire **rehashing**:

1. Alloca nuovo array di bucket con capacità doppia
2. Per ogni entry nella vecchia tabella, ricalcola hash e inserisci nel nuovo array
3. Sostituisci vecchio array con nuovo

Questo mantiene operazioni O(1) ammortizzate.

### Operazioni da Implementare

#### Costruttori

- `HashTableChaining(size_t initial_capacity, float max_load_factor)`
- Move constructor e assignment
- Distruttore (automatic con unique_ptr e list)

#### Inserimento/Modifica

- `void insert(const Key& key, const Value& value)` - inserisce o aggiorna
- `void insert(Key&& key, Value&& value)` - versione move
- `template<typename... Args> Value& emplace(const Key& key, Args&&... args)` - costruisce value in-place

#### Accesso

- `Value& at(const Key& key)` - accesso con bounds checking (throw se non trovato)
- `Value& operator[](const Key& key)` - accesso, inserisce default se non esiste
- `const Value& at(const Key& key) const` - versione const

#### Ricerca

- `bool contains(const Key& key) const` - verifica esistenza chiave
- `Value* find(const Key& key)` - restituisce puntatore a value o nullptr
- `const Value* find(const Key& key) const` - versione const

#### Rimozione

- `bool erase(const Key& key)` - rimuove entry, ritorna true se trovata

#### Query

- `size_t size() const noexcept`
- `size_t capacity() const noexcept`
- `bool is_empty() const noexcept`
- `float load_factor() const noexcept`
- `void clear() noexcept`

#### Configurazione

- `void reserve(size_t new_capacity)` - forza rehashing a nuova capacità
- `void set_max_load_factor(float mlf)`

### Dettagli Implementativi

**Ricerca in bucket**: Per trovare una chiave in un bucket, iterare sulla lista e confrontare chiavi.

**Insert**:

1. Calcola hash
2. Cerca nel bucket corrispondente
3. Se chiave esiste, aggiorna value
4. Altrimenti, aggiungi nuovo entry
5. Incrementa size
6. Se load factor supera max, rehash

**operator[]**:
Se chiave non esiste, inserisci con value default-constructed. Questo richiede che Value sia default-constructible.

### Complessità

- Insert, find, erase: O(1) medio, O(n) peggiore (se tutte le chiavi collidono)
- Rehashing: O(n) ma ammortizzato su inserimenti

---

## 3. Hash Table con Open Addressing

### Descrizione

Alternativa al chaining: tutte le entry sono memorizzate direttamente nell'array. Le collisioni sono risolte cercando il prossimo slot libero secondo una sequenza di probing.

### Posizione File

```
include/ads/hash/Hash_Table_Open_Addressing.hpp
src/ads/hash/Hash_Table_Open_Addressing.tpp
src/main_Hash_Table_Open_Addressing.cc
```

### Strategie di Probing

#### Linear Probing

Sequenza: h(k), h(k)+1, h(k)+2, ...

```cpp
size_t probe(const Key& key, size_t i) const {
  return (hash(key) + i) % capacity_;
}
```

**Pro**: Semplice, buona cache locality
**Contro**: Primary clustering (cluster di slot occupati rallentano ricerche)

#### Quadratic Probing

Sequenza: h(k), h(k)+1², h(k)+2², h(k)+3², ...

```cpp
size_t probe(const Key& key, size_t i) const {
  return (hash(key) + i*i) % capacity_;
}
```

**Pro**: Riduce primary clustering
**Contro**: Secondary clustering, può non visitare tutti gli slot

#### Double Hashing

Sequenza: h(k), h(k)+h₂(k), h(k)+2*h₂(k), ...

```cpp
size_t probe(const Key& key, size_t i) const {
  return (hash1(key) + i * hash2(key)) % capacity_;
}
```

**Pro**: Minimizza clustering
**Contro**: Più complesso, richiede seconda funzione hash

**Implementazione consigliata**: Iniziare con linear probing per semplicità, eventualmente aggiungere quadratic e double come opzioni.

### Gestione Slot

Ogni slot può essere in tre stati:

- **Empty**: Mai usato
- **Occupied**: Contiene entry valida
- **Deleted**: Conteneva entry che è stata rimossa (tombstone)

```cpp
enum class SlotState { Empty, Occupied, Deleted };

struct Slot {
  std::optional<Entry> entry;  // Entry se Occupied
  SlotState state;

  Slot() : entry(std::nullopt), state(SlotState::Empty) {}
};

std::unique_ptr<Slot[]> table_;
```

**Tombstones**: Necessari per non interrompere catene di probing. Quando cerchiamo una chiave e incontriamo un deleted slot, continuiamo la ricerca (potrebbe esserci la chiave più avanti).

### Operazioni

#### Insert

1. Calcola hash iniziale
2. Proba fino a trovare slot Empty o Deleted
3. Inserisci entry e marca Occupied
4. Se load factor > max (es. 0.5), rehash

**Nota**: Max load factor deve essere < 1 (tipicamente 0.5-0.7) altrimenti performance degradano.

#### Find

1. Calcola hash iniziale
2. Proba finché non trovi:
   - Slot Occupied con chiave corrispondente → trovato
   - Slot Empty → non trovato
   - Ripeti per max `capacity_` probe (loop detection)

#### Erase

1. Find per trovare slot
2. Se trovato, marca Deleted (non Empty!)
3. Entry viene distrutta

#### Rehashing

Più critico che con chaining:

1. Crea nuova tabella con capacità doppia
2. Per ogni slot Occupied nella vecchia tabella, re-insert nella nuova
3. **Importante**: Non copiare tombstones, solo entry valide
4. Questo "pulisce" la tabella da deleted slots

### Complessità

Dipende da load factor α = n/m:

- Insert/Find/Erase medio: O(1/(1-α))
- Quando α → 1, performance → O(n)
- Rehashing mantiene α basso → O(1) ammortizzato

### Confronto Chaining vs Open Addressing

**Chaining**:

- Pro: Load factor può superare 1, gestione semplice dei duplicati
- Contro: Overhead di puntatori, peggiore cache locality

**Open Addressing**:

- Pro: Migliore cache locality, no overhead puntatori
- Contro: Sensitivo a load factor, tombstones complicano gestione

### Test da Includere per Hash Tables

1. Inserimenti e ricerche base
2. Aggiornamenti (insert su chiave esistente)
3. Rimozioni multiple
4. Trigger di rehashing (inserire molti elementi)
5. Collisioni intenzionali (chiavi con stesso hash)
6. Operazioni su tabella vuota
7. Performance test: confronto chaining vs open addressing
8. Test con chiavi custom (struct con operator==)

---

## Note Implementative Comuni

### Exception Safety

```cpp
namespace ads {
namespace tree {
  class TreeException : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };
}

namespace hash {
  class HashException : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  class KeyNotFoundException : public HashException {
  public:
    KeyNotFoundException(const std::string& key)
      : HashException("Key not found: " + key) {}
  };
}
}
```

### Performance Testing

Creare `src/main_Phase2_Performance.cc` che confronta:

- AVL vs BST: inserimenti ordinati
- Chaining vs Open Addressing: vari load factors
- Misurazioni con `std::chrono`

### Visualizzazione (opzionale)

Per debug, funzioni helper che stampano:

- AVL: struttura albero con altezze e balance factors
- Hash Table: distribuzione elementi per bucket/slot

## Checklist Completamento Fase 2

- [ ] AVL Tree implementato con tutte le rotazioni
- [ ] AVL Tree testato (inclusi casi specifici per ogni rotazione)
- [ ] Hash Table Chaining implementata
- [ ] Hash Table Open Addressing implementata
- [ ] Test completi per entrambe hash tables
- [ ] Performance comparison script
- [ ] Valgrind clean (no memory leaks)
- [ ] README.md aggiornato
- [ ] Documentazione completa

## Stima Temporale

- AVL Tree: 10-12 ore (rotazioni richiedono attenzione)
- Hash Table Chaining: 6-8 ore
- Hash Table Open Addressing: 8-10 ore (tombstones aumentano complessità)
- Testing e performance: 6-8 ore
- Documentazione: 3-4 ore

**Totale Fase 2**: 33-42 ore (4-5 giorni)
