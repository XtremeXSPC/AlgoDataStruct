//===--------------------------------------------------------------------------===//
/**
 * @file List.hpp
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
#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>

namespace ads ::list {

template <typename T>
class List {
public:
  // Un distruttore virtuale e' obbligatorio nelle classi base polimorfiche.
  virtual ~List() = default;

  // Aggiunge un elemento in coda alla lista.
  virtual void push_back(const T& value) = 0;
  virtual void push_back(T&& value)      = 0;

  // Aggiunge un elemento in testa alla coda.
  virtual void push_front(const T& value) = 0;
  virtual void push_front(T&& value)      = 0;

  // Rimuove l'elemento in coda alla lista.
  virtual void pop_back() = 0;

  // Rimuove l'elemento in testa alla lista.
  virtual void pop_front() = 0;

  // Restituisce un riferimento all'elemento in testa.
  virtual T&       front()       = 0;
  virtual const T& front() const = 0;

  // Restituisce un riferimento all'elemento in coda.
  virtual T&       back()       = 0;
  virtual const T& back() const = 0;

  // Verifica se la lista è vuota.
  virtual bool is_empty() const noexcept = 0;

  // Restituisce il numero di elementi presenti nella lista.
  virtual size_t size() const noexcept = 0;

  // Svuota la lista.
  virtual void clear() = 0;
};

} // namespace ads::list

#endif // LIST_HPP