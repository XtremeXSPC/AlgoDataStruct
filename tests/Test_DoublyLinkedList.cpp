#include "../include/ads/lists/Doubly_Linked_List.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

// Test fixture per la nostra lista
class DoublyLinkedListTest : public ::testing::Test {
protected:
  ads::list::DoublyLinkedList<int> list;
};

// Test per la costruzione di default
TEST_F(DoublyLinkedListTest, IsEmptyOnConstruction) {
  EXPECT_EQ(list.size(), 0);
  EXPECT_TRUE(list.is_empty());
}

// Test per push_front e front
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

// Test per push_back e back
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

// Test per pop_front
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
  EXPECT_THROW(list.pop_front(), std::logic_error);
}

// Test per pop_back
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
  EXPECT_THROW(list.pop_back(), std::logic_error);
}

// Test per l'inversione
TEST_F(DoublyLinkedListTest, Reverse) {
  list.push_back(1);
  list.push_back(2);
  list.push_back(3);

  list.reverse();
  EXPECT_EQ(list.front(), 3);
  EXPECT_EQ(list.back(), 1);

  // Controlla l'ordine con gli iteratori
  std::vector<int> expected = {3, 2, 1};
  std::vector<int> actual;
  for (int val : list) {
    actual.push_back(val);
  }
  EXPECT_EQ(actual, expected);
}

// Test per insert
TEST_F(DoublyLinkedListTest, Insert) {
  list.push_back(10);
  list.push_back(30);

  auto it = list.begin();
  ++it; // it punta a 30

  list.insert(it, 20); // Inserisce 20 prima di 30
  EXPECT_EQ(list.size(), 3);

  std::vector<int> expected = {10, 20, 30};
  std::vector<int> actual;
  for (int val : list) {
    actual.push_back(val);
  }
  EXPECT_EQ(actual, expected);
}

// Test per erase
TEST_F(DoublyLinkedListTest, Erase) {
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);

  auto it = list.begin();
  ++it; // it punta a 20

  it = list.erase(it); // Elimina 20
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.front(), 10);
  EXPECT_EQ(list.back(), 30);
  EXPECT_EQ(*it, 30); // L'iteratore restituito punta a 30
}

// Test sulla semantica di spostamento
TEST_F(DoublyLinkedListTest, MoveSemantics) {
  list.push_back(10);
  list.push_back(20);

  // Test del costruttore di spostamento
  ads::list::DoublyLinkedList<int> moved_list = std::move(list);

  EXPECT_TRUE(list.is_empty()); // La lista originale è vuota
  EXPECT_EQ(moved_list.size(), 2);
  EXPECT_EQ(moved_list.front(), 10);

  // Test dell'assegnazione per spostamento
  list = std::move(moved_list);
  EXPECT_TRUE(moved_list.is_empty());
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.back(), 20);
}