//===--------------------------------------------------------------------------===//
/**
 * @file main.cc
 * @author Costantino Lombardi
 * @brief Tester per le funzioni della lista
 * @version 0.2
 * @date 2025-06-30
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//

#include "../include/ads/lists/Doubly_Linked_List.hpp"
#include <iostream>
#include <string>
#include <vector>

// Funzione helper per stampare la lista e le sue proprietà
void print_list(const ads::list::DoublyLinkedList<int>& list, const std::string& name) {
  std::cout << "Contenuto di '" << name << "' (size: " << list.size() << "):\n  ";
  // Usiamo cbegin() e cend() per iterare su una lista const
  for (auto it = list.cbegin(); it != list.cend(); ++it) {
    std::cout << *it << " <-> ";
  }
  std::cout << "nullptr\n";

  // Stampa anche in ordine inverso per verificare i puntatori `prev`
  std::cout << "  (Inverso): nullptr";
  if (!list.is_empty()) {
    auto it = list.cend();
    do {
      --it;
      std::cout << " <-> " << *it;
    } while (it != list.cbegin());
  }
  std::cout << std::endl;
}

int main() {
  try {
    ads::list::DoublyLinkedList<int> myList;

    std::cout << "--- Aggiunta elementi ---" << std::endl;
    myList.push_back(10);
    myList.push_back(20);
    myList.push_front(5);
    myList.emplace_back(30); // Usa emplace
    print_list(myList, "myList");

    std::cout << "\n--- Iterazione e manipolazione ---" << std::endl;
    auto it = myList.begin();
    ++it;     // it punta a 10
    *it = 15; // Modifica il valore
    print_list(myList, "myList dopo modifica");

    std::cout << "\n--- Inserimento ed eliminazione ---" << std::endl;
    it = myList.insert(it, 7); // Inserisce 7 prima di 15, it ora punta a 7
    print_list(myList, "myList dopo insert");

    ++it;                  // it ora punta a 15
    it = myList.erase(it); // Elimina 15, it ora punta a 20
    std::cout << "Elemento dopo quello eliminato: " << *it << std::endl;
    print_list(myList, "myList dopo erase");

    std::cout << "\n--- Inversione lista ---" << std::endl;
    myList.reverse();
    print_list(myList, "myList invertita");

    std::cout << "\n--- Test Spostamento ---" << std::endl;
    ads::list::DoublyLinkedList<int> anotherList = std::move(myList);
    print_list(anotherList, "anotherList (spostata)");
    print_list(myList, "myList (vuota dopo spostamento)");

    // --- Test Gestione Eccezioni ---
    std::cout << "\n--- Test Gestione Eccezioni ---" << std::endl;
    std::cout << "Tento di chiamare front() su una lista vuota..." << std::endl;
    // myList è vuota dopo lo spostamento
    // Questa chiamata lancerà un'eccezione
    myList.front();

  } catch (const ads::list::ListException& e) {
    std::cerr << "ERRORE CATTURATO CORRETTAMENTE: " << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Errore generico non previsto: " << e.what() << std::endl;
  }

  return 0;
}