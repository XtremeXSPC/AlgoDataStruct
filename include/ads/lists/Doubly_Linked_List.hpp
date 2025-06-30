//===--------------------------------------------------------------------------===//
/**
 * @file Doubly_Linked_List.hpp
 * @author Costantino Lombardi
 * @brief Implementazione di una lista doppiamente concatenata.
 * @version 0.2
 * @date 2025-06-30
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

#include "Exception.hpp"
#include "List.hpp"
#include <iterator>
#include <memory>
#include <utility>

namespace ads::list {

/**
 * @brief Una implementazione di una lista doppiamente concatenata.
 *
 * @details Questa classe implementa l'interfaccia `ads::list::List<T>` utilizzando
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
  //========== CLASSE ITERATOR ==========//
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
    bool      operator==(const iterator& other) const { return node_ptr_ == other.node_ptr_; }
    bool      operator!=(const iterator& other) const { return node_ptr_ != other.node_ptr_; }

  private:
    Node* node_ptr_;
    friend class DoublyLinkedList<T>;
  };

  //========== CLASSE CONST_ITERATOR ==========//
  class const_iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    const_iterator(const Node* ptr = nullptr) : node_ptr_(ptr) {}

    reference       operator*() const;
    pointer         operator->() const;
    const_iterator& operator++();
    const_iterator  operator++(int);
    const_iterator& operator--();
    const_iterator  operator--(int);
    bool            operator==(const const_iterator& other) const { return node_ptr_ == other.node_ptr_; }
    bool            operator!=(const const_iterator& other) const { return node_ptr_ != other.node_ptr_; }

  private:
    const Node* node_ptr_;
    friend class DoublyLinkedList<T>;
  };

  //========== COSTRUTTORI, DISTRUTTORE, ASSEGNAZIONE ==========//

  /** @brief Costruisce una lista vuota. */
  DoublyLinkedList();

  /** @brief Distruttore. Svuota la lista e dealloca tutti i nodi. */
  ~DoublyLinkedList() override;

  // Costruttore e assegnazione di copia sono disabilitati.
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
   * @return Un riferimento a questa istanza.
   */
  DoublyLinkedList& operator=(DoublyLinkedList&&) noexcept;

  //========== INTERFACCIA EREDITATA DA List<T> ==========//

  void push_back(const T& value) override;
  void push_back(T&& value) override;

  void push_front(const T& value) override;
  void push_front(T&& value) override;

  void pop_back() override;
  void pop_front() override;

  T&       front() override;
  const T& front() const override;

  T&       back() override;
  const T& back() const override;

  bool   is_empty() const noexcept override;
  size_t size() const noexcept override;
  void   clear() noexcept override;

  //=========== FUNZIONALITÀ AGGIUNTIVE ==========//

  template <typename... Args>
  T& emplace_back(Args&&... args);
  template <typename... Args>
  T& emplace_front(Args&&... args);

  iterator insert(iterator pos, const T& value);
  iterator insert(iterator pos, T&& value);

  iterator erase(iterator pos);
  void     reverse() noexcept;

  //=========== ACCESSO AGLI ITERATORI ===========//
  // (non-const)
  iterator begin() noexcept;
  iterator end() noexcept;
  // (const)
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

private:
  //================ NODO INTERNO ================//
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

// Inclusione del file di implementazione per i template
#include "../../../src/ads/lists/Doubly_Linked_List.tpp"

} // namespace ads::list

#endif // DOUBLY_LINKED_LIST_HPP

//===--------------------------------------------------------------------------===//
