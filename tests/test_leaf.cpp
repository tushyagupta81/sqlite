#include <catch2/catch_test_macros.hpp>
#include <string>

#include "leaf_node.hpp"
#include "test_utils.hpp"

TEST_CASE("Leaf insert and lookup") {
  Page page{};

  LeafNode leaf(page);

  leaf.init();

  Key key = makeKey<uint32_t>(10);
  auto rec = makeRecord("hello", key);
  leaf.insert(key, rec);

  REQUIRE(leaf.contains(key));

  rec = leaf.getRecord(key);

  REQUIRE(std::string(rec.record.begin(), rec.record.end()) == "hello");
}

TEST_CASE("Leaf delete") {
  Page page{};

  LeafNode leaf(page);

  leaf.init();

  Key key = makeKey<uint32_t>(10);
  auto rec = makeRecord("hello", key);
  leaf.insert(key, rec);

  leaf.delete_rec(key);

  REQUIRE_FALSE(leaf.contains(key));
}

TEST_CASE("Leaf inserts ordered keys") {
  Page page{};
  LeafNode leaf(page);
  leaf.init();

  for (uint32_t i = 1; i <= 100; ++i) {
    Key key = makeKey<uint32_t>(i);
    auto rec = makeRecord(std::to_string(i), key);
    leaf.insert(key, rec);
  }

  REQUIRE(leaf.cellCount() == 100);

  for (uint32_t i = 1; i <= 100; ++i) {
    Key key = makeKey<uint32_t>(i);
    REQUIRE(leaf.contains(key));
  }
}

TEST_CASE("Leaf keeps slots sorted") {
  Page page{};
  LeafNode leaf(page);
  leaf.init();

  auto key30 = makeKey<uint32_t>(30);
  auto key20 = makeKey<uint32_t>(20);
  auto key10 = makeKey<uint32_t>(10);

  leaf.insert(key30, makeRecord("30", key30));
  leaf.insert(key10, makeRecord("10", key10));
  leaf.insert(key20, makeRecord("20", key20));

  REQUIRE(leaf.contains(key10));
  REQUIRE(leaf.contains(key20));
  REQUIRE(leaf.contains(key30));

  REQUIRE(recordToString(leaf.getRecord(key10)) == "10");
  REQUIRE(recordToString(leaf.getRecord(key20)) == "20");
  REQUIRE(recordToString(leaf.getRecord(key30)) == "30");
}

TEST_CASE("Leaf delete positions") {
  Page page{};
  LeafNode leaf(page);
  leaf.init();

  for (int i = 1; i <= 10; ++i) {
    auto key = makeKey<uint32_t>(i);
    leaf.insert(key, makeRecord(std::to_string(i), key));
  }

  auto key1 = makeKey<uint32_t>(1);
  auto key5 = makeKey<uint32_t>(5);
  auto key10 = makeKey<uint32_t>(10);
  auto key2 = makeKey<uint32_t>(2);
  auto key9 = makeKey<uint32_t>(9);

  leaf.delete_rec(key1);
  leaf.delete_rec(key5);
  leaf.delete_rec(key10);

  REQUIRE_FALSE(leaf.contains(key1));
  REQUIRE_FALSE(leaf.contains(key5));
  REQUIRE_FALSE(leaf.contains(key10));

  REQUIRE(leaf.contains(key2));
  REQUIRE(leaf.contains(key9));
}

TEST_CASE("Compact preserves records") {
  Page page{};
  LeafNode leaf(page);
  leaf.init();

  for (int i = 1; i <= 20; ++i) {
    auto key = makeKey<uint32_t>(i);
    leaf.insert(key, makeRecord(std::to_string(i), key));
  }

  auto key5 = makeKey<uint32_t>(5);
  auto key12 = makeKey<uint32_t>(12);
  auto key17 = makeKey<uint32_t>(17);

  leaf.delete_rec(key5);
  leaf.delete_rec(key12);
  leaf.delete_rec(key17);

  leaf.compact();

  for (int i = 1; i <= 20; ++i) {
    auto key = makeKey<uint32_t>(i);

    if (i == 5 || i == 12 || i == 17) {
      REQUIRE_FALSE(leaf.contains(key));
    } else {
      REQUIRE(leaf.contains(key));
      REQUIRE(recordToString(leaf.getRecord(key)) == std::to_string(i));
    }
  }
}
