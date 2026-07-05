#include <catch2/catch_test_macros.hpp>

#include "leaf_node.hpp"
#include "test_utils.hpp"

TEST_CASE("Leaf insert and lookup") {
  Page page{};

  LeafNode leaf(page);

  leaf.init();

  leaf.insert(10, makeRecord("hello"));

  REQUIRE(leaf.contains(10));

  auto rec = leaf.getRecord(10);

  REQUIRE(std::string(rec.record.begin(), rec.record.end()) == "hello");
}

TEST_CASE("Leaf delete") {
  Page page{};

  LeafNode leaf(page);

  leaf.init();

  leaf.insert(10, makeRecord("hello"));

  leaf.delete_rec(10);

  REQUIRE_FALSE(leaf.contains(10));
}

TEST_CASE("Leaf inserts ordered keys") {
  Page page{};
  LeafNode leaf(page);
  leaf.init();

  for (uint32_t i = 1; i <= 100; ++i) {
    leaf.insert(i, makeRecord(std::to_string(i)));
  }

  REQUIRE(leaf.cellCount() == 100);

  for (uint32_t i = 1; i <= 100; ++i) {
    REQUIRE(leaf.contains(i));
  }
}

TEST_CASE("Leaf keeps slots sorted") {
  Page page{};
  LeafNode leaf(page);
  leaf.init();

  leaf.insert(30, makeRecord("30"));
  leaf.insert(10, makeRecord("10"));
  leaf.insert(20, makeRecord("20"));

  REQUIRE(leaf.contains(10));
  REQUIRE(leaf.contains(20));
  REQUIRE(leaf.contains(30));

  REQUIRE(recordToString(leaf.getRecord(10)) == "10");
  REQUIRE(recordToString(leaf.getRecord(20)) == "20");
  REQUIRE(recordToString(leaf.getRecord(30)) == "30");
}

TEST_CASE("Leaf delete positions") {
  Page page{};
  LeafNode leaf(page);
  leaf.init();

  for (int i = 1; i <= 10; ++i) {
    leaf.insert(i, makeRecord(std::to_string(i)));
  }

  leaf.delete_rec(1);
  leaf.delete_rec(5);
  leaf.delete_rec(10);

  REQUIRE_FALSE(leaf.contains(1));
  REQUIRE_FALSE(leaf.contains(5));
  REQUIRE_FALSE(leaf.contains(10));

  REQUIRE(leaf.contains(2));
  REQUIRE(leaf.contains(9));
}

TEST_CASE("Compact preserves records") {
  Page page{};
  LeafNode leaf(page);
  leaf.init();

  for (int i = 1; i <= 20; ++i) {
    leaf.insert(i, makeRecord(std::to_string(i)));
  }

  leaf.delete_rec(5);
  leaf.delete_rec(12);
  leaf.delete_rec(17);

  leaf.compact();

  for (int i = 1; i <= 20; ++i) {
    if (i == 5 || i == 12 || i == 17) {
      REQUIRE_FALSE(leaf.contains(i));
    } else {
      REQUIRE(leaf.contains(i));
    }
  }
}
