//===--------------------------------------------------------------------------===//
/**
 * @file Exception.hpp
 * @author Costantino Lombardi
 * @brief Definisce eccezioni personalizzate per le strutture dati.
 * @version 0.2
 * @date 2025-06-30
 *
 * @copyright Copyright (c) 2024
 *
 */
//===--------------------------------------------------------------------------===//
#pragma once

#ifndef ADS_EXCEPTION_HPP
#define ADS_EXCEPTION_HPP

#include <stdexcept>

namespace ads::list {

/**
 * @brief Eccezione base per errori logici nelle operazioni sulle liste.
 *
 * Viene lanciata quando un'operazione viene chiamata in uno stato non valido
 * (es. accedere a un elemento di una lista vuota).
 */
class ListException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

} // namespace ads::list

#endif // ADS_EXCEPTION_HPP

//===--------------------------------------------------------------------------===//