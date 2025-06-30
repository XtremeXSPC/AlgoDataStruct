//===--------------------------------------------------------------------------===//
/**
 * @file List.hpp
 * @author Costantino Lombardi
 * @brief Interfaccia astratta per una struttura dati di tipo Lista.
 * @version 0.2
 * @date 2025-06-30
 *
 * @copyright Copyright (c) 2024
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once
#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>

namespace ads::list {

/**
 * @brief Interfaccia (classe base astratta) che definisce le operazioni comuni
 *        per una struttura dati di tipo lista.
 *
 * @tparam T Il tipo di dato da memorizzare nella lista.
 */
template <typename T>
class List {
public:
  // Un distruttore virtuale è obbligatorio nelle classi base polimorfiche.
  virtual ~List() = default;

  /**
   * @brief Aggiunge un elemento in coda alla lista (copia).
   * @param value Il valore da aggiungere.
   */
  virtual void push_back(const T& value) = 0;

  /**
   * @brief Aggiunge un elemento in coda alla lista (spostamento).
   * @param value Il valore (r-value) da spostare.
   */
  virtual void push_back(T&& value) = 0;

  /**
   * @brief Aggiunge un elemento in testa alla lista (copia).
   * @param value Il valore da aggiungere.
   */
  virtual void push_front(const T& value) = 0;

  /**
   * @brief Aggiunge un elemento in testa alla lista (spostamento).
   * @param value Il valore (r-value) da spostare.
   */
  virtual void push_front(T&& value) = 0;

  /**
   * @brief Rimuove l'elemento in coda alla lista.
   * @details Il comportamento non è definito se la lista è vuota.
   */
  virtual void pop_back() = 0;

  /**
   * @brief Rimuove l'elemento in testa alla lista.
   * @details Il comportamento non è definito se la lista è vuota.
   */
  virtual void pop_front() = 0;

  /**
   * @brief Restituisce un riferimento all'elemento in testa.
   * @return T& Riferimento al primo elemento.
   */
  virtual T& front() = 0;

  /**
   * @brief Restituisce un riferimento costante all'elemento in testa.
   * @return const T& Riferimento costante al primo elemento.
   */
  virtual const T& front() const = 0;

  /**
   * @brief Restituisce un riferimento all'elemento in coda.
   * @return T& Riferimento all'ultimo elemento.
   */
  virtual T& back() = 0;

  /**
   * @brief Restituisce un riferimento costante all'elemento in coda.
   * @return const T& Riferimento costante all'ultimo elemento.
   */
  virtual const T& back() const = 0;

  /**
   * @brief Verifica se la lista è vuota.
   * @return true se la lista non contiene elementi.
   * @return false altrimenti.
   */
  virtual bool is_empty() const noexcept = 0;

  /**
   * @brief Restituisce il numero di elementi presenti nella lista.
   * @return size_t Il numero di elementi.
   */
  virtual size_t size() const noexcept = 0;

  /**
   * @brief Svuota la lista, rimuovendo tutti gli elementi.
   */
  virtual void clear() = 0;
};

} // namespace ads::list

#endif // LIST_HPP

//===--------------------------------------------------------------------------===//