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

#include "../include/ads/associative/Hash_Map.hpp"
#include "../include/ads/hash/Hash_Table_Chaining.hpp"
#include "../include/ads/hash/Hash_Table_Open_Addressing.hpp"

#include <gtest/gtest.h>

#include <concepts>
#include <memory>
#include <string>
#include <vector>

using namespace ads::hash;

namespace {

struct CustomHashKey {
  int value;

  auto operator==(const CustomHashKey& other) const -> bool { return value == other.value; }
};

struct CustomHash {
  auto operator()(const CustomHashKey& key) const -> size_t { return static_cast<size_t>(key.value % 4); }
};

template <typename Map, typename Key, typename Value>
concept HasHashMapCopyPut = requires(Map& map, const Key& key, const Value& value) {
  { map.put(key, value) } -> std::same_as<void>;
};

static_assert(
    ads::associative::Dictionary<ads::associative::HashMap<int, std::unique_ptr<int>>, int, std::unique_ptr<int>>);
static_assert(ads::associative::CopyPutDictionary<ads::associative::HashMap<int, std::string>, int, std::string>);
static_assert(!HasHashMapCopyPut<ads::associative::HashMap<int, std::unique_ptr<int>>, int, std::unique_ptr<int>>);

} // namespace

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

TEST(HashTableChainingMoveOnlyTest, SupportsMoveOnlyValues) {
  HashTableChaining<int, std::unique_ptr<int>> table;
  int                                          copied_key = 2;

  table.insert(1, std::make_unique<int>(10));
  table.insert(copied_key, std::make_unique<int>(20));
  table.insert(1, std::make_unique<int>(30));

  ASSERT_NE(table.find(1), nullptr);
  ASSERT_NE(table.find(2), nullptr);
  EXPECT_EQ(*table.at(1), 30);
  EXPECT_EQ(*table.at(2), 20);
  EXPECT_EQ(table.size(), 2);

  EXPECT_TRUE(table.erase(2));
  EXPECT_FALSE(table.contains(2));
}

TEST(HashTableChainingMoveOnlyTest, RehashPreservesMoveOnlyValues) {
  HashTableChaining<int, std::unique_ptr<int>> table(2, 0.5f);

  for (int i = 0; i < 50; ++i) {
    table.insert(i, std::make_unique<int>(i * 10));
  }

  EXPECT_EQ(table.size(), 50);
  EXPECT_LE(table.load_factor(), table.max_load_factor());

  for (int i = 0; i < 50; ++i) {
    ASSERT_NE(table.find(i), nullptr);
    EXPECT_EQ(*table.at(i), i * 10);
  }
}

TEST(HashTableChainingCustomHashTest, UsesCustomHashFunctor) {
  HashTableChaining<CustomHashKey, std::string, CustomHash> table(4);

  table.insert(CustomHashKey{1}, std::string("one"));
  table.insert(CustomHashKey{5}, std::string("five"));

  EXPECT_EQ(table.size(), 2);
  EXPECT_EQ(table.at(CustomHashKey{1}), "one");
  EXPECT_EQ(table.at(CustomHashKey{5}), "five");
}

TEST(HashMapMoveOnlyTest, SupportsMoveOnlyValues) {
  ads::associative::HashMap<int, std::unique_ptr<int>> map;
  int                                                  copied_key = 4;

  map.put(1, std::make_unique<int>(10));
  map.put(copied_key, std::make_unique<int>(40));
  auto [it, inserted] = map.emplace(2, std::make_unique<int>(20));
  map[3]              = std::make_unique<int>(30);

  EXPECT_TRUE(inserted);
  EXPECT_EQ(it->first, 2);
  EXPECT_EQ(*it->second, 20);
  EXPECT_EQ(*map.at(1), 10);
  EXPECT_EQ(*map.at(3), 30);
  EXPECT_EQ(*map.at(4), 40);
  EXPECT_TRUE(map.contains(2));
  EXPECT_EQ(map.erase(1), 1U);
  EXPECT_FALSE(map.contains(1));
}

TEST(HashMapCustomHashTest, UsesCustomHashFunctor) {
  ads::associative::HashMap<CustomHashKey, std::string, CustomHash> map;

  map.put(CustomHashKey{1}, std::string("one"));
  map.put(CustomHashKey{5}, std::string("five"));

  EXPECT_EQ(map.size(), 2);
  EXPECT_TRUE(map.contains(CustomHashKey{1}));
  EXPECT_EQ(map.at(CustomHashKey{5}), "five");
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

TEST(HashTableOpenAddressingConstructionTest, ZeroCapacityStillAcceptsInsertions) {
  HashTableOpenAddressing<int, std::string> table(0);

  EXPECT_GE(table.capacity(), 2U);
  EXPECT_TRUE(table.insert(1, "one"));
  EXPECT_EQ(table.at(1), "one");
}

TEST(HashTableOpenAddressingMoveOnlyTest, SupportsMoveOnlyValues) {
  HashTableOpenAddressing<int, std::unique_ptr<int>> table(4, ProbingStrategy::LINEAR, 0.75f);
  int                                                copied_key = 2;

  table.insert(1, std::make_unique<int>(10));
  table.insert(copied_key, std::make_unique<int>(20));
  table.insert(1, std::make_unique<int>(30));

  ASSERT_NE(table.find(1), nullptr);
  ASSERT_NE(table.find(2), nullptr);
  EXPECT_EQ(*table.at(1), 30);
  EXPECT_EQ(*table.at(2), 20);
  EXPECT_EQ(table.size(), 2);
}

TEST(HashTableOpenAddressingCustomHashTest, UsesCustomHashFunctor) {
  HashTableOpenAddressing<CustomHashKey, std::string, CustomHash> table(8, ProbingStrategy::LINEAR, 0.75f);

  table.insert(CustomHashKey{1}, std::string("one"));
  table.insert(CustomHashKey{5}, std::string("five"));

  EXPECT_EQ(table.size(), 2);
  EXPECT_EQ(table.at(CustomHashKey{1}), "one");
  EXPECT_EQ(table.at(CustomHashKey{5}), "five");
}

TEST(HashTableOpenAddressingTombstoneTest, RehashesWhenTombstonesDominateProbeSpace) {
  HashTableOpenAddressing<int, int> table(8, ProbingStrategy::LINEAR, 0.75f);

  for (int i = 0; i < 6; ++i) {
    table.insert(i, i);
  }
  for (int i = 0; i < 6; ++i) {
    EXPECT_TRUE(table.erase(i));
  }
  for (int i = 10; i < 30; ++i) {
    EXPECT_TRUE(table.insert(i, i * 10));
  }

  EXPECT_EQ(table.size(), 20U);
  for (int i = 10; i < 30; ++i) {
    EXPECT_EQ(table.at(i), i * 10);
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
