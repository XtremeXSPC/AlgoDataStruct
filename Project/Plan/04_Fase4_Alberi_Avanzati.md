# Fase 4: Alberi Avanzati

## Panoramica

La Fase 4 completa il progetto AlgoDataStruct con l'implementazione di strutture dati ad albero avanzate. Queste strutture rappresentano algoritmi sofisticati di bilanciamento e organizzazione dei dati, essenziali per applicazioni ad alte prestazioni.

## Obiettivi della Fase

Implementare tre strutture dati ad albero avanzate:

1. **Red-Black Tree** - Albero bilanciato con proprietà colore
2. **B-Tree** - Albero multi-way per database e file system
3. **Trie** - Albero prefisso per ricerca stringhe efficiente

**Tempo stimato**: 6-8 giorni

## Prerequisiti Completati

✅ **Fase 1**: Singly Linked List, Min/Max Heap
✅ **Fase 2**: AVL Tree, Hash Tables
✅ **Fase 3**: Graph, HashMap, Priority Queue

## Strutture da Implementare

---

### 1. Red-Black Tree

#### Descrizione

Un Red-Black Tree è un albero binario di ricerca auto-bilanciante dove ogni nodo ha un colore (rosso o nero). Il bilanciamento è mantenuto attraverso 5 proprietà fondamentali e operazioni di colorazione + rotazione.

#### Proprietà Fondamentali

1. Ogni nodo è rosso o nero
2. La radice è sempre nera
3. Tutte le foglie (NIL) sono nere
4. Se un nodo è rosso, entrambi i figli sono neri
5. Ogni percorso da radice a foglia contiene lo stesso numero di nodi neri (black height)

#### Vantaggi rispetto ad AVL

- Meno rotazioni durante insert/delete (più efficiente per insert-heavy workloads)
- Bilanciamento più "rilassato" (altezza max = 2 log(n+1))
- Utilizzato in librerie standard (std::map, std::set in C++)

#### Operazioni Richieste

**Core Operations**:
- `insert(T value)` - Inserimento con bilanciamento
- `remove(T value)` - Rimozione con bilanciamento
- `search(T value) -> bool` - Ricerca standard BST
- `contains(T value) -> bool` - Alias per search

**Traversal**:
- `in_order_traversal()`
- `pre_order_traversal()`
- `post_order_traversal()`

**Utility**:
- `size() -> size_t`
- `height() -> int`
- `is_empty() -> bool`
- `clear()`
- `get_black_height() -> int` - Altezza nera dell'albero
- `validate_rb_properties() -> bool` - Debug: verifica proprietà

#### Membri Privati

```cpp
struct Node {
  T data;
  Color color;  // enum class Color { Red, Black }
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
  Node* parent;  // Non-owning (necessario per rotazioni)

  Node(T val, Color col = Color::Red)
    : data(val), color(col), left(nullptr), right(nullptr), parent(nullptr) {}
};

std::unique_ptr<Node> root_;
size_t size_;
```

#### Algoritmi Chiave

**Insert Fixup**:
```
After standard BST insert (new node is RED):
WHILE parent is RED:
  IF parent is left child of grandparent:
    uncle = grandparent->right
    IF uncle is RED:
      // Case 1: Uncle is red
      parent.color = BLACK
      uncle.color = BLACK
      grandparent.color = RED
      node = grandparent
    ELSE:
      IF node is right child:
        // Case 2: Node is right child
        node = parent
        left_rotate(node)
      // Case 3: Node is left child
      parent.color = BLACK
      grandparent.color = RED
      right_rotate(grandparent)
  ELSE:
    (symmetric cases)
root.color = BLACK
```

**Delete Fixup**:
```
After BST delete, if deleted node was BLACK:
WHILE node != root AND node.color == BLACK:
  IF node is left child:
    sibling = parent->right
    IF sibling.color == RED:
      // Case 1: Sibling is red
      sibling.color = BLACK
      parent.color = RED
      left_rotate(parent)
      sibling = parent->right
    IF sibling->left.color == BLACK AND sibling->right.color == BLACK:
      // Case 2: Sibling's children are black
      sibling.color = RED
      node = parent
    ELSE:
      IF sibling->right.color == BLACK:
        // Case 3: Sibling's right child is black
        sibling->left.color = BLACK
        sibling.color = RED
        right_rotate(sibling)
        sibling = parent->right
      // Case 4: Sibling's right child is red
      sibling.color = parent.color
      parent.color = BLACK
      sibling->right.color = BLACK
      left_rotate(parent)
      node = root
  ELSE:
    (symmetric cases)
node.color = BLACK
```

#### Complessità

| Operazione | Tempo       | Note |
|------------|-------------|------|
| Insert     | O(log n)    | Al massimo 2 rotazioni |
| Delete     | O(log n)    | Al massimo 3 rotazioni |
| Search     | O(log n)    | Come BST |
| Space      | O(n)        | 1 bit extra per colore |

#### Dettagli Implementativi Critici

1. **Gestione NIL Nodes**: Usare `nullptr` per NIL (considerati neri implicitamente)
2. **Parent Pointer**: Necessario per risalire durante fixup
3. **Color Management**: enum class per type safety
4. **Rotazioni**: Identiche ad AVL ma con aggiornamento colori
5. **Root Color**: Sempre nero dopo ogni operazione

#### File da Creare

```
include/ads/trees/Red_Black_Tree.hpp
src/ads/trees/Red_Black_Tree.tpp
src/main_Red_Black_Tree.cc
```

#### Test da Includere

- Insert sequenziale e verificare bilanciamento
- Insert random e verificare proprietà RB
- Delete foglie, nodi con 1 figlio, nodi con 2 figli
- Stress test con migliaia di elementi
- Verifica black height costante
- Comparazione performance con AVL

---

### 2. B-Tree

#### Descrizione

Un B-Tree è un albero di ricerca bilanciato multi-way (ogni nodo può avere più di 2 figli) ottimizzato per sistemi che leggono/scrivono grandi blocchi di dati (database, file system).

#### Proprietà Fondamentali

Dato `t` = minimum degree (t ≥ 2):

1. Ogni nodo ha al massimo `2t - 1` chiavi
2. Ogni nodo (eccetto root) ha almeno `t - 1` chiavi
3. Root ha almeno 1 chiave (se non vuoto)
4. Tutti i nodi foglia sono allo stesso livello
5. Nodo interno con k chiavi ha k+1 figli

#### Vantaggi

- Ottimizzato per I/O su disco (riduce accessi)
- Altezza molto bassa (logaritmica in base t)
- Cache-friendly per grandi dataset
- Usato in database (B+ Tree variante)

#### Operazioni Richieste

**Core Operations**:
- `insert(T key)` - Inserimento con split se necessario
- `remove(T key)` - Rimozione con merge/redistribute
- `search(T key) -> bool` - Ricerca multi-level
- `contains(T key) -> bool` - Alias

**Traversal**:
- `in_order_traversal()` - Visita in ordine

**Utility**:
- `size() -> size_t`
- `height() -> int`
- `is_empty() -> bool`
- `clear()`
- `get_minimum_degree() -> int`

#### Membri Privati

```cpp
struct Node {
  std::vector<T> keys;                  // Chiavi ordinate
  std::vector<std::unique_ptr<Node>> children;  // Figli
  bool is_leaf;
  size_t n;  // Numero corrente di chiavi

  Node(bool leaf) : is_leaf(leaf), n(0) {}
};

std::unique_ptr<Node> root_;
int t_;  // Minimum degree
size_t size_;
```

#### Algoritmi Chiave

**Insert Algorithm**:
```
IF root is full:
  Create new root
  Make old root child of new root
  Split old root
  Insert key in appropriate child
ELSE:
  Insert in non-full root

insert_non_full(node, key):
  IF node is leaf:
    Insert key in sorted position
  ELSE:
    Find child to insert
    IF child is full:
      Split child
      IF key > median:
        Move to next child
    Recursively insert in child
```

**Split Child**:
```
split_child(parent, index):
  full_child = parent->children[index]
  new_child = create new Node

  // Move half keys to new child
  Copy upper half of full_child.keys to new_child.keys

  // If not leaf, move corresponding children
  IF not full_child.is_leaf:
    Copy upper half of full_child.children to new_child.children

  // Median goes up to parent
  parent.keys.insert(index, full_child.keys[t-1])
  parent.children.insert(index+1, new_child)

  // Reduce full_child size
  full_child.n = t - 1
```

**Delete Algorithm** (complesso):
```
Case 1: Key in leaf
  - Simply remove it

Case 2: Key in internal node
  2a: Left child has >= t keys
      Replace with predecessor, delete predecessor
  2b: Right child has >= t keys
      Replace with successor, delete successor
  2c: Both children have t-1 keys
      Merge with right child, recursively delete

Case 3: Key not in node
  3a: Child has >= t keys
      Recursively delete from child
  3b: Child has t-1 keys
      - If sibling has >= t keys: borrow from sibling
      - Else: merge with sibling
```

#### Complessità

| Operazione | Tempo           | Note |
|------------|-----------------|------|
| Insert     | O(t log_t n)    | t = min degree |
| Delete     | O(t log_t n)    | Complesso da implementare |
| Search     | O(t log_t n)    | Multi-level |
| Height     | log_t n         | Molto basso |
| Space      | O(n)            | Ma con overhead per vettori |

#### Dettagli Implementativi Critici

1. **Minimum Degree**: Tipicamente t = 2 (2-3-4 tree) o t = 3-5 per B-Tree generale
2. **Proactive Splitting**: Split durante discesa (semplifica insert)
3. **Merge vs Borrow**: Delete richiede logica complessa di merge/redistribute
4. **Vector Usage**: std::vector per keys e children (dynamic sizing)
5. **Leaf Detection**: Flag is_leaf per distinguere foglie da interni

#### File da Creare

```
include/ads/trees/B_Tree.hpp
src/ads/trees/B_Tree.tpp
src/main_B_Tree.cc
```

#### Test da Includere

- Insert sequenziale con visualizzazione split
- Insert/delete random
- Verificare altezza rimane log_t(n)
- Stress test con vari gradi (t = 2, 3, 5)
- Verificare tutti i nodi rispettano vincoli B-Tree
- Caso speciale: albero diventa vuoto

---

### 3. Trie (Prefix Tree)

#### Descrizione

Un Trie è una struttura dati ad albero usata per memorizzare stringhe in modo che permetta ricerca, inserimento e prefix matching molto efficienti. Ogni nodo rappresenta un carattere di una stringa.

#### Caratteristiche

- Ogni percorso dalla radice a foglia rappresenta una parola
- Condivide prefissi comuni (space-efficient per dizionari con prefissi comuni)
- Supporta operazioni di prefix matching (autocomplete, spell checking)
- Tempo di ricerca O(m) dove m = lunghezza stringa (indipendente da n = numero parole)

#### Operazioni Richieste

**Core Operations**:
- `insert(const std::string& word)` - Inserisce parola
- `search(const std::string& word) -> bool` - Cerca parola esatta
- `starts_with(const std::string& prefix) -> bool` - Verifica se esiste prefisso
- `remove(const std::string& word)` - Rimuove parola (mantenendo prefissi)

**Advanced Operations**:
- `get_all_words_with_prefix(const std::string& prefix) -> std::vector<std::string>`
- `longest_common_prefix() -> std::string`
- `count_words_with_prefix(const std::string& prefix) -> size_t`

**Utility**:
- `size() -> size_t` - Numero di parole memorizzate
- `is_empty() -> bool`
- `clear()`

#### Membri Privati

```cpp
struct TrieNode {
  std::unordered_map<char, std::unique_ptr<TrieNode>> children;
  bool is_end_of_word;

  TrieNode() : is_end_of_word(false) {}
};

std::unique_ptr<TrieNode> root_;
size_t word_count_;
```

**Alternative Implementation** (per alfabeto limitato):
```cpp
struct TrieNode {
  std::array<std::unique_ptr<TrieNode>, 26> children;  // a-z
  bool is_end_of_word;

  TrieNode() : is_end_of_word(false) {
    children.fill(nullptr);
  }
};
```

#### Algoritmi Chiave

**Insert**:
```
insert(word):
  node = root
  FOR each char in word:
    IF char not in node.children:
      node.children[char] = new TrieNode()
    node = node.children[char]
  node.is_end_of_word = true
  word_count++
```

**Search**:
```
search(word):
  node = root
  FOR each char in word:
    IF char not in node.children:
      RETURN false
    node = node.children[char]
  RETURN node.is_end_of_word
```

**Starts With**:
```
starts_with(prefix):
  node = root
  FOR each char in prefix:
    IF char not in node.children:
      RETURN false
    node = node.children[char]
  RETURN true  // Found prefix
```

**Remove** (ricorsivo):
```
remove_helper(node, word, depth):
  IF node is nullptr:
    RETURN false

  IF depth == word.length():
    IF node.is_end_of_word:
      node.is_end_of_word = false
      word_count--
      RETURN node.children.empty()  // Can delete if no children
    RETURN false

  char = word[depth]
  IF remove_helper(node.children[char], word, depth+1):
    node.children.erase(char)
    RETURN (!node.is_end_of_word AND node.children.empty())

  RETURN false
```

**Get All Words With Prefix**:
```
get_all_words_with_prefix(prefix):
  node = find_node(prefix)
  IF node is nullptr:
    RETURN empty vector

  results = []
  dfs_collect(node, prefix, results)
  RETURN results

dfs_collect(node, current_word, results):
  IF node.is_end_of_word:
    results.push_back(current_word)

  FOR each (char, child) in node.children:
    dfs_collect(child, current_word + char, results)
```

#### Complessità

| Operazione | Tempo  | Spazio | Note |
|------------|--------|--------|------|
| Insert     | O(m)   | O(m)   | m = lunghezza parola |
| Search     | O(m)   | O(1)   | |
| Starts With| O(m)   | O(1)   | m = lunghezza prefisso |
| Remove     | O(m)   | O(1)   | |
| Get All    | O(p+n) | O(n)   | p = prefisso, n = risultati |
| Space Total| O(ALPHABET_SIZE * N * M) | | Worst case |

**Space Optimization**: Con unordered_map, spazio = O(numero_nodi_effettivi)

#### Dettagli Implementativi Critici

1. **Children Representation**:
   - `unordered_map<char, unique_ptr<Node>>` - Flessibile, supporta qualsiasi char
   - `array<unique_ptr<Node>, 26>` - Più veloce ma solo lowercase a-z

2. **End of Word Flag**: Essenziale per distinguere "car" da "card"

3. **Memory Management**: unique_ptr gestisce ownership della struttura ad albero

4. **Case Sensitivity**: Decidere se normalizzare input a lowercase

5. **Character Validation**: Verificare input contiene solo caratteri validi

#### Applicazioni Reali

- **Autocomplete**: Suggerimenti durante digitazione
- **Spell Checking**: Dizionari con prefix matching
- **IP Routing**: Longest prefix matching
- **Text Editors**: Word completion
- **Search Engines**: Query suggestions

#### File da Creare

```
include/ads/trees/Trie.hpp
src/ads/trees/Trie.tpp
src/main_Trie.cc
```

#### Test da Includere

- Insert parole e verificare search
- Prefix matching: "car" matches "car", "card", "carpet"
- Remove: rimuovere "car" mantiene "card"
- Get all words with prefix "ca" -> ["car", "card", "cat", ...]
- Longest common prefix
- Stress test con dizionario inglese (migliaia di parole)
- Case sensitivity tests
- Empty trie operations

---

## Ordine di Implementazione Consigliato

### 1. Trie (Inizia qui - La più semplice)
- Struttura concettualmente diretta
- Nessun bilanciamento complesso
- Buon warm-up per alberi multi-child
- **Tempo stimato**: 1.5-2 giorni

### 2. Red-Black Tree (Medio-complesso)
- Riusa concetti da AVL (rotazioni)
- Aggiunge logica di colorazione
- Algoritmi ben documentati
- **Tempo stimato**: 2.5-3 giorni

### 3. B-Tree (Il più complesso)
- Richiede logica di split/merge sofisticata
- Delete è particolarmente complesso
- Beneficia di esperienza con altri alberi
- **Tempo stimato**: 2.5-3 giorni

## Pattern Comuni alle Tre Strutture

### Memory Management
```cpp
// Tutti usano unique_ptr per ownership
std::unique_ptr<Node> root_;

// Trie e B-Tree usano containers di unique_ptr
std::unordered_map<char, std::unique_ptr<TrieNode>> children;  // Trie
std::vector<std::unique_ptr<Node>> children;  // B-Tree
```

### Exception Handling
```cpp
// Riusa TreeException esistente
#include "ads/trees/Binary_Tree_Exception.hpp"

namespace ads::trees {
  using TreeException = Binary_Tree_Exception;
}
```

### Iteratori (Opzionale per Fase 4)
- Red-Black Tree: Può riusare iteratore in-order di BST/AVL
- B-Tree: Iteratore in-order più complesso (multi-key per nodo)
- Trie: Iteratore sulle parole (ritorna std::string)

Se il tempo permette, aggiungerli; altrimenti focus su correttezza delle operazioni core.

## Testing Strategy

### Unit Tests per Ogni Struttura

1. **Basic Operations**
   - Empty tree operations
   - Single element insert/delete
   - Multiple elements

2. **Correctness**
   - Verify tree properties after every operation
   - Red-Black: 5 properties
   - B-Tree: min/max keys, level uniformity
   - Trie: word integrity

3. **Edge Cases**
   - Duplicate inserts
   - Delete non-existent keys
   - Large datasets

4. **Performance**
   - Insert N elements, measure time
   - Search random elements
   - Compare with std:: equivalents

### Integration Tests

- Usare Red-Black Tree in combinazione con HashMap
- Trie per autocomplete in sistema di ricerca
- B-Tree per simulated database index

### Memory Tests

```bash
valgrind --leak-check=full ./build/main_Red_Black_Tree
valgrind --leak-check=full ./build/main_B_Tree
valgrind --leak-check=full ./build/main_Trie
```

**Obiettivo**: 0 memory leaks, 0 errors

## Deliverables della Fase 4

### File da Creare (9 totali)

**Headers** (3):
- `include/ads/trees/Red_Black_Tree.hpp`
- `include/ads/trees/B_Tree.hpp`
- `include/ads/trees/Trie.hpp`

**Implementations** (3):
- `src/ads/trees/Red_Black_Tree.tpp`
- `src/ads/trees/B_Tree.tpp`
- `src/ads/trees/Trie.tpp`

**Tests** (3):
- `src/main_Red_Black_Tree.cc`
- `src/main_B_Tree.cc`
- `src/main_Trie.cc`

### Documentazione

- Aggiornare `README.md` con nuove strutture
- Aggiungere sezione "Advanced Trees" in docs
- Tabella comparativa: RB vs AVL, B-Tree uso, Trie applicazioni

### Opzionale (Se Tempo Disponibile)

- `src/main_Phase4_Advanced_Trees_Comparison.cc` - Benchmark comparativo
- Esempio applicativo: Dictionary con Trie + autocomplete
- Esempio applicativo: Database index simulation con B-Tree

## Checklist di Completamento

### Per Ogni Struttura

- [ ] Header con documentazione Doxygen completa
- [ ] Implementation con complessità annotate
- [ ] Test file con casi completi
- [ ] Compilazione senza warning
- [ ] Tutti i test passano
- [ ] Valgrind clean (0 leaks)
- [ ] Proprietà struttura verificate (RB properties, B-Tree invariants)

### Fase 4 Generale

- [ ] Tutte e 3 le strutture completate
- [ ] README aggiornato
- [ ] Commit con messaggi descrittivi
- [ ] CMakeLists.txt aggiornato (se necessario)
- [ ] Branch merged o PR creata

## Risorse di Riferimento

### Red-Black Tree

- CLRS "Introduction to Algorithms", Chapter 13
- [Wikipedia: Red-Black Tree](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree)
- Sedgewick, "Algorithms", Left-Leaning Red-Black Trees

### B-Tree

- CLRS "Introduction to Algorithms", Chapter 18
- [Wikipedia: B-Tree](https://en.wikipedia.org/wiki/B-tree)
- Comer, "The Ubiquitous B-Tree" (ACM Computing Surveys)

### Trie

- [Wikipedia: Trie](https://en.wikipedia.org/wiki/Trie)
- Knuth, "The Art of Computer Programming", Vol 3, Section 6.3
- Practical applications in text editors and search engines

## Note Finali

La Fase 4 completa il repository AlgoDataStruct con strutture dati di livello avanzato, portando il progetto a un livello professionale. Queste tre strutture coprono:

- **Red-Black Tree**: Bilanciamento pratico usato in STL
- **B-Tree**: Fondamentale per database e file systems
- **Trie**: Essenziale per text processing e search

Dopo questa fase, il repository conterrà:
- 13+ strutture dati fondamentali (Fasi 1-3)
- 3 strutture avanzate (Fase 4)
- Testing completo con valgrind
- Documentazione professionale

**Prossimi Passi** (Opzionali, Fasi Future):
- Fase 5: Strutture Spaziali (KD-Tree, Quad-Tree, R-Tree)
- Fase 6: Strutture Probabilistiche (Skip List, Bloom Filter, LRU Cache)

---

**Versione**: 1.0
**Data**: 2025-11-21
**Autore**: Costantino Lombardi
**Stato**: Pronto per implementazione

Buon lavoro! 🚀
