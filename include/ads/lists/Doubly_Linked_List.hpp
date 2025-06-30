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

template <typename T>
class DoublyLinkedList : public List<T> {
private:
  struct Node;

public:
  class iterator;

  // --- COSTRUTTORI, DISTRUTTORE, ASSEGNAZIONE ---
  DoublyLinkedList();
  ~DoublyLinkedList() override;

  DoublyLinkedList(const DoublyLinkedList&)            = delete;
  DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

  DoublyLinkedList(DoublyLinkedList&&) noexcept;
  DoublyLinkedList& operator=(DoublyLinkedList&&) noexcept;

  // --- INTERFACCIA EREDITATA DA List<T> ---
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

  // --- FUNZIONALITÀ AGGIUNTIVE ---
  template <typename... Args>
  T& emplace_back(Args&&... args);
  template <typename... Args>
  T& emplace_front(Args&&... args);

  iterator insert(iterator pos, const T& value);
  iterator insert(iterator pos, T&& value);

  iterator erase(iterator pos);
  void     reverse() noexcept;

  // --- ACCESSO AGLI ITERATORI ---
  iterator begin() noexcept;
  iterator end() noexcept;

public:
  // --- CLASSE ITERATORE ---
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

private:
  // --- NODO INTERNO ---
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