//===--------------------------------------------------------------------------===//
/**
 * @file Doubly_Linekd_List.hpp
 * @author Costantino Lombardi
 * @brief
 * @version 0.1
 * @date 2025-06-30
 *
 * @copyright Copyright (c) 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP
#include "List.hpp"
#include <iterator>
#include <memory>
#include <utility>

// Namespace per AlgoDataStruct
namespace ads::list {

/**
 * @brief Una implementazione di una lista doppiamente concatenata.
 *
 * @details Questa classe implementa l'interfaccia `ads::List::List<T>` utilizzando
 *          una struttura a nodi doppiamente concatenati. Offre inserimento e
 *          cancellazione in tempo costante (O(1)) all'inizio e alla fine, e
 *          in tempo costante se si dispone di un iteratore alla posizione.
 *          La gestione della memoria è automatizzata tramite std::unique_ptr.
 *
 * @tparam T Il tipo di dato da memorizzare nella lista.
 */
template <typename T>
class DoublyLinkedList : public List<T> {
private:
  struct Node;

public:
  //========== CLASSE ITERATORE ==========//
  class iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    iterator(Node* ptr = nullptr) : node_ptr_(ptr) {}

    reference operator*() const;
    pointer   operator->() const;

    iterator& operator++();
    iterator  operator++(int);
    iterator& operator--();
    iterator  operator--(int);

    bool operator==(const iterator& other) const { return node_ptr_ == other.node_ptr_; }
    bool operator!=(const iterator& other) const { return node_ptr_ != other.node_ptr_; }

  private:
    Node* node_ptr_;
    friend class DoublyLinkedList<T>;
  };

  //========== COSTRUTTORI, DISTRUTTORE, ASSEGNAZIONE ==========//

  /**
   * @brief Costruisce una lista vuota.
   */
  DoublyLinkedList();

  /**
   * @brief Distruttore. Svuota la lista e dealloca tutti i nodi.
   */
  ~DoublyLinkedList() override;

  DoublyLinkedList(const DoublyLinkedList&)            = delete;
  DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

  /**
   * @brief Costruttore di spostamento.
   * @param other La lista da cui spostare le risorse.
   */
  DoublyLinkedList(DoublyLinkedList&&) noexcept;

  /**
   * @brief Operatore di assegnazione per spostamento.
   * @param other La lista da cui spostare le risorse.
   * @return DoublyLinkedList& Un riferimento a questa istanza.
   */
  DoublyLinkedList& operator=(DoublyLinkedList&&) noexcept;

  //========== INTERFACCIA EREDITATA DA List<T> ==========//

  /**
   * @brief Aggiunge un elemento alla fine della lista (copia).
   * @param value Il valore da aggiungere.
   */
  void push_back(const T& value) override;

  /**
   * @brief Aggiunge un elemento alla fine della lista (spostamento).
   * @param value Il valore (r-value) da spostare.
   */
  void push_back(T&& value) override;

  /**
   * @brief Aggiunge un elemento all'inizio della lista (copia).
   * @param value Il valore da aggiungere.
   */
  void push_front(const T& value) override;

  /**
   * @brief Aggiunge un elemento all'inizio della lista (spostamento).
   * @param value Il valore (r-value) da spostare.
   */
  void push_front(T&& value) override;

  /**
   * @brief Rimuove l'ultimo elemento della lista.
   * @throw std::logic_error se la lista è vuota.
   */
  void pop_back() override;

  /**
   * @brief Rimuove il primo elemento della lista.
   * @throw std::logic_error se la lista è vuota.
   */
  void pop_front() override;

  /**
   * @brief Restituisce un riferimento al primo elemento.
   * @throw std::logic_error se la lista è vuota.
   * @return T& Riferimento al primo elemento.
   */
  T&       front() override;
  const T& front() const override;

  /**
   * @brief Restituisce un riferimento all'ultimo elemento.
   * @throw std::logic_error se la lista è vuota.
   * @return T& Riferimento all'ultimo elemento.
   */
  T&       back() override;
  const T& back() const override;

  /**
   * @brief Controlla se la lista è vuota.
   * @return true se la lista non contiene elementi, false altrimenti.
   */
  bool is_empty() const noexcept override;

  /**
   * @brief Restituisce il numero di elementi nella lista.
   * @return size_t Il numero di elementi.
   */
  size_t size() const noexcept override;

  /**
   * @brief Rimuove tutti gli elementi dalla lista.
   */
  void clear() noexcept override;

  //========== FUNZIONALITÀ AGGIUNTIVE ==========//

  /**
   * @brief Costruisce un elemento sul posto alla fine della lista.
   * @tparam Args Tipi degli argomenti del costruttore dell'elemento.
   * @param args Argomenti da inoltrare al costruttore di T.
   * @return T& Riferimento all'elemento creato.
   */
  template <typename... Args>
  T& emplace_back(Args&&... args);

  /**
   * @brief Costruisce un elemento sul posto all'inizio della lista.
   * @tparam Args Tipi degli argomenti del costruttore dell'elemento.
   * @param args Argomenti da inoltrare al costruttore di T.
   * @return T& Riferimento all'elemento creato.
   */
  template <typename... Args>
  T& emplace_front(Args&&... args);

  /**
   * @brief Inserisce un elemento in una data posizione.
   * @param pos Iteratore alla posizione prima della quale inserire il nuovo elemento.
   * @param value Il valore da inserire.
   * @return Un iteratore all'elemento appena inserito.
   */
  iterator insert(iterator pos, const T& value);
  iterator insert(iterator pos, T&& value);

  /**
   * @brief Rimuove l'elemento in una data posizione.
   * @param pos Iteratore all'elemento da rimuovere.
   * @return Un iteratore all'elemento successivo a quello rimosso.
   */
  iterator erase(iterator pos);

  /**
   * @brief Inverte l'ordine degli elementi nella lista.
   */
  void reverse() noexcept;

  //========== ACCESSO AGLI ITERATORI (non-const) ==========//
  iterator begin() noexcept;
  iterator end() noexcept;

  //========== ACCESSO AGLI ITERATORI (const) ==========//
  iterator begin() const noexcept;
  iterator end() const noexcept;

private:
  //==========  FUNZIONI AUSILIARIE ==========//
  template <typename U>
  iterator insert_impl(DoublyLinkedList<T>& list, typename DoublyLinkedList<T>::iterator pos, U&& value);

  //========== NODO INTERNO ==========//
  struct Node {
    T                     data;
    std::unique_ptr<Node> next;
    Node*                 prev;
    template <typename... Args>
    Node(Node* p, Args&&... args) : data(std::forward<Args>(args)...), next(nullptr), prev(p) {}
  };

  std::unique_ptr<Node> head_;
  Node*                 tail_;
  size_t                size_;
};

// Il path di inclusione corretto secondo la nuova struttura
#include "../../../src/ads/lists/Doubly_Linked_List.tpp"

} // namespace ads::list

#endif // DOUBLY_LINKED_LIST_HPP

//===--------------------------------------------------------------------------===//
