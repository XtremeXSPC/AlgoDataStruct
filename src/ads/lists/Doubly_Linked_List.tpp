//===--------------------------------------------------------------------------===//
/**
 * @file Doubly_Linked_List.tpp
 * @author Costantino Lombardi
 * @brief Implementazione dei metodi template di DoublyLinkedList.
 * @version 0.2
 * @date 2025-06-30
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#include "../../../include/ads/lists/Doubly_Linked_List.hpp"
#include <utility>

using namespace ads::list;

//===---------------------- Implementazione iterator -------------------------====//
template <typename T>
typename DoublyLinkedList<T>::iterator::reference DoublyLinkedList<T>::iterator::operator*() const {
  return node_ptr_->data;
}

template <typename T>
typename DoublyLinkedList<T>::iterator::pointer DoublyLinkedList<T>::iterator::operator->() const {
  return &node_ptr_->data;
}

template <typename T>
typename DoublyLinkedList<T>::iterator& DoublyLinkedList<T>::iterator::operator++() {
  node_ptr_ = node_ptr_->next.get();
  return *this;
}

template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::iterator::operator++(int) {
  iterator temp = *this;
  ++(*this);
  return temp;
}

template <typename T>
typename DoublyLinkedList<T>::iterator& DoublyLinkedList<T>::iterator::operator--() {
  node_ptr_ = node_ptr_->prev;
  return *this;
}

template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::iterator::operator--(int) {
  iterator temp = *this;
  --(*this);
  return temp;
}

//===--------------------- Implementazione const_iterator --------------------====//
template <typename T>
typename DoublyLinkedList<T>::const_iterator::reference DoublyLinkedList<T>::const_iterator::operator*() const {
  return node_ptr_->data;
}

template <typename T>
typename DoublyLinkedList<T>::const_iterator::pointer DoublyLinkedList<T>::const_iterator::operator->() const {
  return &node_ptr_->data;
}

template <typename T>
typename DoublyLinkedList<T>::const_iterator& DoublyLinkedList<T>::const_iterator::operator++() {
  node_ptr_ = node_ptr_->next.get();
  return *this;
}

template <typename T>
typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::const_iterator::operator++(int) {
  const_iterator temp = *this;
  ++(*this);
  return temp;
}

template <typename T>
typename DoublyLinkedList<T>::const_iterator& DoublyLinkedList<T>::const_iterator::operator--() {
  node_ptr_ = node_ptr_->prev;
  return *this;
}

template <typename T>
typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::const_iterator::operator--(int) {
  const_iterator temp = *this;
  --(*this);
  return temp;
}

//===------------------- Implementazione DoublyLinkedList --------------------====//

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {
}

template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
  clear();
}

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(DoublyLinkedList&& other) noexcept :
    head_(std::move(other.head_)), tail_(other.tail_), size_(other.size_) {
  other.tail_ = nullptr;
  other.size_ = 0;
}

template <typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(DoublyLinkedList&& other) noexcept {
  if (this != &other) {
    clear();
    head_       = std::move(other.head_);
    tail_       = other.tail_;
    size_       = other.size_;
    other.tail_ = nullptr;
    other.size_ = 0;
  }
  return *this;
}

template <typename T>
template <typename... Args>
auto DoublyLinkedList<T>::emplace_back(Args&&... args) {
  if (!tail_) { // Lista vuota
    head_ = std::make_unique<Node>(nullptr, std::forward<Args>(args)...);
    tail_ = head_.get();
  } else {
    tail_->next = std::make_unique<Node>(tail_, std::forward<Args>(args)...);
    tail_       = tail_->next.get();
  }
  size_++;
  return tail_->data;
}

template <typename T>
void DoublyLinkedList<T>::push_back(const T& value) {
  emplace_back(value);
}

template <typename T>
void DoublyLinkedList<T>::push_back(T&& value) {
  emplace_back(std::move(value));
}

template <typename T>
template <typename... Args>
auto DoublyLinkedList<T>::emplace_front(Args&&... args) {
  auto newNode = std::make_unique<Node>(nullptr, std::forward<Args>(args)...);
  if (head_) {
    head_->prev   = newNode.get();
    newNode->next = std::move(head_);
  } else { // Lista era vuota
    tail_ = newNode.get();
  }
  head_ = std::move(newNode);
  size_++;
  return head_->data;
}

template <typename T>
void DoublyLinkedList<T>::push_front(const T& value) {
  emplace_front(value);
}

template <typename T>
void DoublyLinkedList<T>::push_front(T&& value) {
  emplace_front(std::move(value));
}

template <typename T>
void DoublyLinkedList<T>::pop_front() {
  if (is_empty()) {
    throw ListException("pop_front on empty list");
  }
  head_ = std::move(head_->next);
  if (head_) {
    head_->prev = nullptr;
  } else { // La lista è diventata vuota
    tail_ = nullptr;
  }
  size_--;
}

template <typename T>
void DoublyLinkedList<T>::pop_back() {
  if (is_empty()) {
    throw ListException("pop_back on empty list");
  }
  if (size_ == 1) {
    pop_front();
  } else {
    tail_ = tail_->prev;
    tail_->next.reset(); // Dealloca il vecchio nodo di coda
    size_--;
  }
}

template <typename T>
T& DoublyLinkedList<T>::front() {
  if (is_empty()) {
    throw ListException("front on empty list");
  }
  return head_->data;
}

template <typename T>
const T& DoublyLinkedList<T>::front() const {
  if (is_empty()) {
    throw ListException("front on empty list");
  }
  return head_->data;
}

template <typename T>
T& DoublyLinkedList<T>::back() {
  if (is_empty()) {
    throw ListException("back on empty list");
  }
  return tail_->data;
}

template <typename T>
const T& DoublyLinkedList<T>::back() const {
  if (is_empty()) {
    throw ListException("back on empty list");
  }
  return tail_->data;
}

template <typename T>
bool DoublyLinkedList<T>::is_empty() const noexcept {
  return size_ == 0;
}

template <typename T>
size_t DoublyLinkedList<T>::size() const noexcept {
  return size_;
}

template <typename T>
void DoublyLinkedList<T>::clear() noexcept {
  head_.reset(); // La distruzione a catena dei "unique_ptr" dealloca tutti i nodi.
  tail_ = nullptr;
  size_ = 0;
}

template <typename T>
auto DoublyLinkedList<T>::insert(iterator pos, const T& value) {
  return insert(pos, T(value)); // Chiama la versione T&&
}

template <typename T>
auto DoublyLinkedList<T>::insert(iterator pos, T&& value) {
  // Casi limite: inserimento in testa o in coda
  if (pos == begin()) {
    push_front(std::move(value));
    return begin();
  }
  if (pos == end()) {
    push_back(std::move(value));
    // Dopo push_back, `tail_` punta al nuovo nodo
    return iterator(tail_);
  }

  // Inserimento nel mezzo
  Node* pos_node  = pos.node_ptr_;
  Node* prev_node = pos_node->prev;

  // 1. Crea il nuovo nodo, che punta al precedente
  auto     new_node = std::make_unique<Node>(prev_node, std::move(value));
  iterator new_it(new_node.get());

  // 2. Il nuovo nodo prende possesso del puntatore al nodo successivo
  new_node->next = std::move(prev_node->next);
  // 3. Il nodo `pos` (successivo) ora deve puntare indietro al nuovo nodo
  pos_node->prev = new_node.get();
  // 4. Il nodo precedente ora punta al nuovo nodo, trasferendo la proprietà
  prev_node->next = std::move(new_node);

  size_++;
  return new_it;
}

template <typename T>
auto DoublyLinkedList<T>::erase(iterator pos) {
  if (pos == end() || is_empty()) {
    throw ListException("cannot erase an invalid or end iterator");
  }

  // 1. Identifica i nodi precedente e successivo a quello da rimuovere.
  Node* node_to_remove = pos.node_ptr_;
  Node* prev_node      = node_to_remove->prev;
  Node* next_node      = node_to_remove->next.get();

  // 2. Ricollega il puntatore `next` del nodo precedente.
  if (prev_node) {
    // Se c'è un predecessore, il suo `next` deve puntare al successore del nodo da rimuovere.
    // std::move trasferisce la proprietà del unique_ptr.
    // Questo è il momento in cui node_to_remove viene deallocato.
    prev_node->next = std::move(node_to_remove->next);
  } else {
    // Se non c'è un predecessore, stiamo rimuovendo la testa. Aggiorniamo `head_`.
    head_ = std::move(node_to_remove->next);
  }

  // 3. Ricollega il puntatore `prev` del nodo successivo.
  if (next_node) {
    // Se c'è un successore, il suo `prev` deve puntare al predecessore del nodo rimosso.
    next_node->prev = prev_node;
  } else {
    // Se non c'è un successore, stiamo rimuovendo la coda. Aggiorniamo `tail_`.
    tail_ = prev_node;
  }

  // 4. Aggiorna la dimensione e restituisci l'iteratore al nodo successivo.
  size_--;
  return iterator(next_node);
}

template <typename T>
void DoublyLinkedList<T>::reverse() noexcept {
  if (size() < 2) {
    return;
  }

  // L'approccio è quello di staccare i nodi dalla vecchia lista
  // e di inserirli uno a uno in testa a una nuova lista (che riutilizza head_).
  // `current_list` mantiene i nodi non ancora processati.
  std::unique_ptr<Node> current_list = std::move(head_);
  tail_                              = current_list.get(); // La vecchia testa diventa la nuova coda

  while (current_list) {
    // 1. Stacca il nodo in testa a `current_list`
    auto detached_node = std::move(current_list);
    // 2. Avanza `current_list` al prossimo nodo
    current_list = std::move(detached_node->next);

    // 3. Inserisci `detached_node` in testa alla lista risultante (gestita da `head_`)
    detached_node->next = std::move(head_);
    if (detached_node->next) { // Se la lista non era vuota
      detached_node->next->prev = detached_node.get();
    }
    head_       = std::move(detached_node);
    head_->prev = nullptr;
  }
}

//---------------------------- ACCESSO AGLI ITERATORI ----------------------------//
// (non-const)
template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::begin() noexcept {
  return iterator(head_.get());
}

template <typename T>
typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::end() noexcept {
  return iterator(nullptr); // end() è un iteratore nullo
}

// (const)
template <typename T>
typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::begin() const noexcept {
  return const_iterator(head_.get());
}

template <typename T>
typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::end() const noexcept {
  return const_iterator(nullptr);
}

template <typename T>
typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::cbegin() const noexcept {
  return begin();
}

template <typename T>
typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::cend() const noexcept {
  return end();
}

//===--------------------------------------------------------------------------===//