#include "../include/ads/lists/Doubly_Linked_List.hpp"
#include <iostream>
#include <string>
#include <vector>

void print_list(const auto& list, const std::string& name) {
  std::cout << "Contenuto di '" << name << "' (size: " << list.size() << "):\n  ";
  for (const auto& item : list) {
    std::cout << item << " <-> ";
  }
  std::cout << "nullptr" << std::endl;
}

int main() {
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
  myList.insert(it, 7); // Inserisce 7 prima di 15
  print_list(myList, "myList dopo insert");

  it = myList.begin();   // it punta a 5
  ++it;                  // it punta a 7
  it = myList.erase(it); // Elimina 7, it ora punta a 15
  std::cout << "Elemento dopo quello eliminato: " << *it << std::endl;
  print_list(myList, "myList dopo erase");

  std::cout << "\n--- Inversione lista ---" << std::endl;
  myList.reverse();
  print_list(myList, "myList invertita");

  std::cout << "\n--- Test Spostamento ---" << std::endl;
  ads::list::DoublyLinkedList<int> anotherList = std::move(myList);
  print_list(anotherList, "anotherList (spostata)");
  print_list(myList, "myList (vuota dopo spostamento)");

  return 0;
}