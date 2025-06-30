//===--------------------------------------------------------------------------===//
/**
 * @file Test_DoublyLinkedList.cpp
 * @author Costantino Lombardi
 * @brief Test cases per DoublyLinkedList
 * @version 0.1
 * @date 2025-06-30
 *
 * @copyright Copyright (c) 2025
 *
 */
//===--------------------------------------------------------------------------===//

#include "../include/ads/lists/Doubly_Linked_List.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace ads::list;

// Test fixture per inizializzare una lista pulita per ogni test
class DoublyLinkedListTest : public ::testing::Test {
protected:
  DoublyLinkedList<int> list;
};

// --- Test di base e stato ---

TEST_F(DoublyLinkedListTest, IsEmptyOnConstruction) {
  EXPECT_EQ(list.size(), 0);
  EXPECT_TRUE(list.is_empty());
}

TEST_F(DoublyLinkedListTest, Clear) {
  list.push_back(10);
  list.push_back(20);
  EXPECT_FALSE(list.is_empty());

  list.clear();
  EXPECT_EQ(list.size(), 0);
  EXPECT_TRUE(list.is_empty());
  // Verifichiamo che l'accesso fallisca dopo clear()
  EXPECT_THROW(list.front(), ListException);
}

// --- Test sui modificatori (push/pop/emplace) ---

TEST_F(DoublyLinkedListTest, PushFront) {
  list.push_front(10);
  EXPECT_EQ(list.size(), 1);
  EXPECT_FALSE(list.is_empty());
  EXPECT_EQ(list.front(), 10);
  EXPECT_EQ(list.back(), 10);

  list.push_front(20);
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.front(), 20);
  EXPECT_EQ(list.back(), 10);
}

TEST_F(DoublyLinkedListTest, PushBack) {
  list.push_back(10);
  EXPECT_EQ(list.size(), 1);
  EXPECT_FALSE(list.is_empty());
  EXPECT_EQ(list.front(), 10);
  EXPECT_EQ(list.back(), 10);

  list.push_back(20);
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.front(), 10);
  EXPECT_EQ(list.back(), 20);
}

TEST_F(DoublyLinkedListTest, PopFront) {
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);

  list.pop_front();
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.front(), 20);

  list.pop_front();
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.front(), 30);

  list.pop_front();
  EXPECT_TRUE(list.is_empty());
  // Ora ci aspettiamo la nostra eccezione personalizzata
  EXPECT_THROW(list.pop_front(), ListException);
}

TEST_F(DoublyLinkedListTest, PopBack) {
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);

  list.pop_back();
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.back(), 20);

  list.pop_back();
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.back(), 10);

  list.pop_back();
  EXPECT_TRUE(list.is_empty());
  // Qui ci aspettiamo l'eccezione definita
  EXPECT_THROW(list.pop_back(), ListException);
}

// Test per emplace con un tipo non banale come std::string
TEST(DoublyLinkedListStringTest, Emplace) {
  DoublyLinkedList<std::string> str_list;
  str_list.emplace_front("world");
  str_list.emplace_back("!");
  str_list.emplace_front("Hello");

  EXPECT_EQ(str_list.size(), 3);
  EXPECT_EQ(str_list.front(), "Hello");
  EXPECT_EQ(str_list.back(), "!");
}

// --- Test di accesso e iteratori ---

TEST_F(DoublyLinkedListTest, AccessOnEmptyThrows) {
  EXPECT_THROW(list.front(), ListException);
  EXPECT_THROW(list.back(), ListException);

  const auto& const_list = list;
  EXPECT_THROW(const_list.front(), ListException);
  EXPECT_THROW(const_list.back(), ListException);
}

TEST_F(DoublyLinkedListTest, ConstCorrectness) {
  list.push_back(10);
  list.push_back(20);

  const auto& const_list = list;

  // Verifica che l'iterazione su una lista const funzioni correttamente
  auto it = const_list.cbegin();
  ASSERT_NE(it, const_list.cend());
  EXPECT_EQ(*it, 10);
  ++it;
  ASSERT_NE(it, const_list.cend());
  EXPECT_EQ(*it, 20);
  ++it;
  EXPECT_EQ(it, const_list.cend());

  // Il codice seguente NON deve compilare, il che prova la correttezza di const_iterator:
  // *const_list.cbegin() = 5;

  // Verifica che il range-based for loop funzioni con una lista const
  std::vector<int> actual;
  for (const int& val : const_list) {
    actual.push_back(val);
  }
  std::vector<int> expected = {10, 20};
  EXPECT_EQ(actual, expected);
}

// --- Test degli algoritmi (insert, erase, reverse) ---

TEST_F(DoublyLinkedListTest, InsertComprehensive) {
  // 1. Inserimento in una lista vuota (equivalente a push_front)
  list.insert(list.begin(), 10);
  EXPECT_EQ(list.front(), 10);
  EXPECT_EQ(list.size(), 1);

  // 2. Inserimento alla fine
  list.insert(list.end(), 30);
  EXPECT_EQ(list.back(), 30);
  EXPECT_EQ(list.size(), 2);

  // 3. Inserimento in mezzo
  auto it = list.begin();
  ++it; // it punta a 30
  list.insert(it, 20);
  EXPECT_EQ(list.size(), 3);

  std::vector<int> expected = {10, 20, 30};
  std::vector<int> actual;
  for (int val : list) {
    actual.push_back(val);
  }
  EXPECT_EQ(actual, expected);
}

TEST_F(DoublyLinkedListTest, EraseComprehensive) {
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);
  list.push_back(40);

  // 1. Rimuovi dalla metà
  auto it = list.begin();
  ++it; // it punta a 20
  it = list.erase(it);
  EXPECT_EQ(*it, 30);
  EXPECT_EQ(list.size(), 3);

  // 2. Rimuovi dalla testa
  it = list.erase(list.begin());
  EXPECT_EQ(*it, 30);
  EXPECT_EQ(list.size(), 2);

  // 3. Rimuovi dalla coda
  auto it_to_tail = list.begin();
  ++it_to_tail; // Ora punta a 40 (l'ultimo elemento)
  it = list.erase(it_to_tail);

  EXPECT_EQ(it, list.end()); // L'iteratore restituito è end()
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.back(), 30);
}

TEST_F(DoublyLinkedListTest, Reverse) {
  list.push_back(1);
  list.push_back(2);
  list.push_back(3);

  list.reverse();
  EXPECT_EQ(list.front(), 3);
  EXPECT_EQ(list.back(), 1);

  std::vector<int> expected = {3, 2, 1};
  std::vector<int> actual;
  for (int val : list) {
    actual.push_back(val);
  }
  EXPECT_EQ(actual, expected);
}

TEST_F(DoublyLinkedListTest, ReverseEdgeCases) {
  // 1. Inversione di una lista vuota
  list.reverse();
  EXPECT_TRUE(list.is_empty());

  // 2. Inversione di una lista con un solo elemento
  list.push_back(42);
  list.reverse();
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.front(), 42);
  EXPECT_EQ(list.back(), 42);
}

// --- Test sulla semantica speciale ---

TEST_F(DoublyLinkedListTest, MoveSemantics) {
  list.push_back(10);
  list.push_back(20);

  // Test del costruttore di spostamento
  DoublyLinkedList<int> moved_list_ctor = std::move(list);
  EXPECT_TRUE(list.is_empty()); // La lista originale è vuota
  EXPECT_EQ(list.size(), 0);
  EXPECT_EQ(moved_list_ctor.size(), 2);
  EXPECT_EQ(moved_list_ctor.front(), 10);

  // Test dell'assegnazione per spostamento
  list = std::move(moved_list_ctor);
  EXPECT_TRUE(moved_list_ctor.is_empty());
  EXPECT_EQ(moved_list_ctor.size(), 0);
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.back(), 20);
}