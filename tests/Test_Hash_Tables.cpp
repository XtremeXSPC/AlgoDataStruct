//===---------------------------------------------------------------------------===//
/**
 * @file Test_HashTables.cpp
 * @brief Google Test unit tests for Hash Table implementations.
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 */
//===---------------------------------------------------------------------------===//

#include "../include/ads/hash/Hash_Table_Chaining.hpp"
#include "../include/ads/hash/Hash_Table_Open_Addressing.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ads::hash;

// Test fixture for HashTableChaining.
class HashTableChainingTest : public ::testing::Test {
protected:
  HashTableChaining<int, std::string> table;
};

//===---------------------------- BASIC STATE TESTS ----------------------------===//

TEST_F(HashTableChainingTest, IsEmptyOnConstruction) {
  EXPECT_EQ(table.size(), 0);
  EXPECT_TRUE(table.is_empty());
}

TEST_F(HashTableChainingTest, InsertAndGet) {
  table.insert(1, "one");
  EXPECT_EQ(table.size(), 1);
  EXPECT_EQ(table.at(1), "one");

  table.insert(2, "two");
  EXPECT_EQ(table.size(), 2);
  EXPECT_EQ(table.at(2), "two");
}

TEST_F(HashTableChainingTest, InsertDuplicateUpdates) {
  table.insert(1, "one");
  table.insert(1, "ONE");
  EXPECT_EQ(table.size(), 1);
  EXPECT_EQ(table.at(1), "ONE");
}

TEST_F(HashTableChainingTest, Contains) {
  table.insert(1, "one");
  table.insert(2, "two");

  EXPECT_TRUE(table.contains(1));
  EXPECT_TRUE(table.contains(2));
  EXPECT_FALSE(table.contains(3));
}

TEST_F(HashTableChainingTest, GetNonExistentThrows) {
  EXPECT_THROW(table.at(1), KeyNotFoundException);
}

//===--------------------------- MODIFICATION TESTS ----------------------------===//

TEST_F(HashTableChainingTest, Remove) {
  table.insert(1, "one");
  table.insert(2, "two");
  table.insert(3, "three");

  EXPECT_TRUE(table.erase(2));
  EXPECT_EQ(table.size(), 2);
  EXPECT_FALSE(table.contains(2));

  EXPECT_FALSE(table.erase(99)); // Non-existent.
}

TEST_F(HashTableChainingTest, Clear) {
  table.insert(1, "one");
  table.insert(2, "two");

  table.clear();
  EXPECT_TRUE(table.is_empty());
  EXPECT_EQ(table.size(), 0);
}

TEST_F(HashTableChainingTest, CollisionHandling) {
  // Insert many elements to force collisions.
  for (int i = 0; i < 100; ++i) {
    table.insert(i, "value" + std::to_string(i));
  }

  EXPECT_EQ(table.size(), 100);

  // Verify all elements are accessible.
  for (int i = 0; i < 100; ++i) {
    EXPECT_TRUE(table.contains(i));
    EXPECT_EQ(table.at(i), "value" + std::to_string(i));
  }
}

//===-------------------------- MOVE SEMANTICS TESTS ---------------------------===//

TEST_F(HashTableChainingTest, MoveSemantics) {
  table.insert(1, "one");
  table.insert(2, "two");

  // Move constructor.
  HashTableChaining<int, std::string> moved_table = std::move(table);
  EXPECT_TRUE(table.is_empty());
  EXPECT_EQ(moved_table.size(), 2);
  EXPECT_EQ(moved_table.at(1), "one");

  // Move assignment.
  table = std::move(moved_table);
  EXPECT_TRUE(moved_table.is_empty());
  EXPECT_EQ(table.size(), 2);
}

TEST_F(HashTableChainingTest, LoadFactorAndRehash) {
  // Insert enough elements to trigger rehashing.
  for (int i = 0; i < 1'000; ++i) {
    table.insert(i, std::to_string(i));
  }

  EXPECT_EQ(table.size(), 1'000);
  EXPECT_LE(table.load_factor(), 1.0); // Load factor should be reasonable.

  // Verify data integrity after rehash.
  for (int i = 0; i < 1'000; ++i) {
    EXPECT_EQ(table.at(i), std::to_string(i));
  }
}

//===-------------------------- OPEN ADDRESSING TESTS --------------------------===//

class HashTableOpenAddressingTest : public ::testing::Test {
protected:
  HashTableOpenAddressing<int, std::string> table;
};

TEST_F(HashTableOpenAddressingTest, IsEmptyOnConstruction) {
  EXPECT_EQ(table.size(), 0);
  EXPECT_TRUE(table.is_empty());
}

TEST_F(HashTableOpenAddressingTest, InsertAndGet) {
  table.insert(1, "one");
  EXPECT_EQ(table.size(), 1);
  EXPECT_EQ(table.at(1), "one");

  table.insert(2, "two");
  EXPECT_EQ(table.size(), 2);
  EXPECT_EQ(table.at(2), "two");
}

TEST_F(HashTableOpenAddressingTest, InsertDuplicateUpdates) {
  table.insert(1, "one");
  table.insert(1, "ONE");
  EXPECT_EQ(table.size(), 1);
  EXPECT_EQ(table.at(1), "ONE");
}

TEST_F(HashTableOpenAddressingTest, Contains) {
  table.insert(1, "one");
  table.insert(2, "two");

  EXPECT_TRUE(table.contains(1));
  EXPECT_TRUE(table.contains(2));
  EXPECT_FALSE(table.contains(3));
}

TEST_F(HashTableOpenAddressingTest, GetNonExistentThrows) {
  EXPECT_THROW(table.at(1), KeyNotFoundException);
}

TEST_F(HashTableOpenAddressingTest, Remove) {
  table.insert(1, "one");
  table.insert(2, "two");
  table.insert(3, "three");

  EXPECT_TRUE(table.erase(2));
  EXPECT_EQ(table.size(), 2);
  EXPECT_FALSE(table.contains(2));

  EXPECT_FALSE(table.erase(99)); // Non-existent.
}

TEST_F(HashTableOpenAddressingTest, Clear) {
  table.insert(1, "one");
  table.insert(2, "two");

  table.clear();
  EXPECT_TRUE(table.is_empty());
  EXPECT_EQ(table.size(), 0);
}

TEST_F(HashTableOpenAddressingTest, ProbeSequence) {
  // Insert elements that may collide.
  for (int i = 0; i < 50; ++i) {
    table.insert(i, "value" + std::to_string(i));
  }

  EXPECT_EQ(table.size(), 50);

  for (int i = 0; i < 50; ++i) {
    EXPECT_TRUE(table.contains(i));
    EXPECT_EQ(table.at(i), "value" + std::to_string(i));
  }
}

TEST_F(HashTableOpenAddressingTest, RemoveAndReinsert) {
  table.insert(1, "one");
  table.insert(2, "two");
  table.erase(1);
  table.insert(1, "ONE");

  EXPECT_EQ(table.at(1), "ONE");
  EXPECT_EQ(table.size(), 2);
}

TEST_F(HashTableOpenAddressingTest, MoveSemantics) {
  table.insert(1, "one");
  table.insert(2, "two");

  // Move constructor.
  HashTableOpenAddressing<int, std::string> moved_table = std::move(table);
  EXPECT_TRUE(table.is_empty());
  EXPECT_EQ(moved_table.size(), 2);
  EXPECT_EQ(moved_table.at(1), "one");

  // Move assignment.
  table = std::move(moved_table);
  EXPECT_TRUE(moved_table.is_empty());
  EXPECT_EQ(table.size(), 2);
}

TEST_F(HashTableOpenAddressingTest, LoadFactorAndRehash) {
  for (int i = 0; i < 500; ++i) {
    table.insert(i, std::to_string(i));
  }

  EXPECT_EQ(table.size(), 500);

  for (int i = 0; i < 500; ++i) {
    EXPECT_EQ(table.at(i), std::to_string(i));
  }
}

//===---------------------------- STRING KEY TESTS -----------------------------===//

TEST(HashTableStringKeyTest, ChainingWithStringKeys) {
  HashTableChaining<std::string, int> table;

  table.insert("apple", 1);
  table.insert("banana", 2);
  table.insert("cherry", 3);

  EXPECT_EQ(table.at("apple"), 1);
  EXPECT_EQ(table.at("banana"), 2);
  EXPECT_EQ(table.at("cherry"), 3);
}

TEST(HashTableStringKeyTest, OpenAddressingWithStringKeys) {
  HashTableOpenAddressing<std::string, int> table;

  table.insert("apple", 1);
  table.insert("banana", 2);
  table.insert("cherry", 3);

  EXPECT_EQ(table.at("apple"), 1);
  EXPECT_EQ(table.at("banana"), 2);
  EXPECT_EQ(table.at("cherry"), 3);
}

//===---------------------------------------------------------------------------===//
