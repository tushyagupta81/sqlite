#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>

#include "leaf_node.hpp"
#include "table.hpp"
#include "test_utils.hpp"

TEST_CASE("Insert and find single row") {
  TempDB db("table_single");

  Pager pager(db.file());
  Btree tree(pager);

  PageId root = pager.allocatePage();
  LeafNode(pager.read(root)).init();

  RowEncoder re;

  Table table(tree, {.table_name = "students", .root_page = root},
              table_table_schema, re);

  Row row = {
      int32_t(1),
      std::string("Alice"),
      int32_t(4),
      int32_t(5),
      std::string("CREATE TABLE students"),
  };

  table.insert(row);

  auto key = table.serializeKey(row);
  auto found = table.find(key);

  REQUIRE(found.has_value());
  REQUIRE(found.value() == row);
}

TEST_CASE("Insert many rows") {
  TempDB db("table_many");

  Pager pager(db.file());
  Btree tree(pager);

  PageId root = pager.allocatePage();
  LeafNode(pager.read(root)).init();

  RowEncoder re;

  Table table(tree, {.table_name = "students", .root_page = root},
              table_table_schema, re);

  std::vector<Key> keys;
  for (int i = 1; i <= 1000; ++i) {
    Row v = {
        int32_t(i), std::to_string(i), int32_t(i), int32_t(5), "create",
    };
    keys.push_back(table.serializeKey(v));
    table.insert(v);
  }

  for (int i = 1; i <= 1000; ++i) {
    auto key = keys[i - 1];
    auto row = table.find(key);

    REQUIRE(row.has_value());
    REQUIRE(row.value()[0] == Value(int32_t(i)));
    REQUIRE(row.value()[1] == Value(std::to_string(i)));
    REQUIRE(row.value()[2] == Value(int32_t(i)));
    REQUIRE(row.value()[3] == Value(int32_t(5)));
    REQUIRE(row.value()[4] == Value(std::string("create")));
  }
}

TEST_CASE("Find missing row") {
  TempDB db("table_missing");

  Pager pager(db.file());
  Btree tree(pager);

  PageId root = pager.allocatePage();
  LeafNode(pager.read(root)).init();

  RowEncoder re;

  Table table(tree, {.table_name = "students", .root_page = root},
              table_table_schema, re);

  auto key = makeKey<uint32_t>(1);

  REQUIRE_FALSE(table.find(key).has_value());
}

TEST_CASE("Remove rows") {
  TempDB db("table_remove");

  Pager pager(db.file());
  Btree tree(pager);

  PageId root = pager.allocatePage();
  LeafNode(pager.read(root)).init();

  RowEncoder re;

  Table table(tree, {.table_name = "students", .root_page = root},
              table_table_schema, re);

  std::vector<Key> keys;
  for (int i = 1; i <= 500; ++i) {
    Row row = {
        int32_t(i), std::to_string(i), int32_t(i), int32_t(5), "create",
    };
    keys.push_back(table.serializeKey(row));
    table.insert(row);
  }

  for (int i = 2; i <= 500; i += 2) {
    auto key = keys[i - 1];
    table.remove(key);
  }

  for (int i = 1; i <= 500; ++i) {
    auto key = keys[i - 1];
    auto row = table.find(key);

    if ((i % 2) != 0) {
      REQUIRE(row.has_value());
      REQUIRE(row.value()[0] == Value(int32_t(i)));
      REQUIRE(row.value()[1] == Value(std::to_string(i)));
      REQUIRE(row.value()[2] == Value(int32_t(i)));
      REQUIRE(row.value()[3] == Value(int32_t(5)));
      REQUIRE(row.value()[4] == Value(std::string("create")));
    } else {
      REQUIRE_FALSE(row.has_value());
    }
  }
}

TEST_CASE("Insert variable length strings") {
  TempDB db("table_strings");

  Pager pager(db.file());
  Btree tree(pager);

  PageId root = pager.allocatePage();
  LeafNode(pager.read(root)).init();

  RowEncoder re;

  Table table(tree, {.table_name = "students", .root_page = root},
              table_table_schema, re);

  Row row = {
      int32_t(1), std::string(500, 'x'),  int32_t(10),
      int32_t(5), std::string(1000, 'y'),
  };

  table.insert(row);

  auto key = table.serializeKey(row);
  auto found = table.find(key);

  REQUIRE(found.has_value());
  REQUIRE(found.value()[0] == Value(int32_t(1)));
  REQUIRE(found.value()[1] == Value(std::string(500, 'x')));
  REQUIRE(found.value()[2] == Value(int32_t(10)));
  REQUIRE(found.value()[3] == Value(int32_t(5)));
  REQUIRE(found.value()[4] == Value(std::string(1000, 'y')));
}
