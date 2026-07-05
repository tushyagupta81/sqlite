#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <numeric>
#include <random>

#include "btree.hpp"
#include "leaf_node.hpp"
#include "test_utils.hpp"

TEST_CASE("Insert 1000 records") {
  TempDB db("btree_insert");

  Pager pager(db.file());

  Btree tree(pager);

  PageId root = pager.allocatePage();

  LeafNode(pager.read(root)).init();

  for (uint32_t i = 1; i <= 1000; ++i) {
    root = tree.insert(root, i, makeRecord(std::to_string(i)));
  }

  for (uint32_t i = 1; i <= 1000; ++i) {
    REQUIRE(tree.contains(root, i));

    auto rec = tree.search(root, i).value();

    REQUIRE(std::string(rec.record.begin(), rec.record.end()) ==
            std::to_string(i));
  }
}

TEST_CASE("Delete every even key") {
  TempDB db("btree_delete");

  Pager pager(db.file());

  Btree tree(pager);

  PageId root = pager.allocatePage();

  LeafNode(pager.read(root)).init();

  for (int i = 1; i <= 500; ++i) {
    root = tree.insert(root, i, makeRecord(std::to_string(i)));
  }

  for (int i = 2; i <= 500; i += 2) {
    tree.remove(root, i);
  }

  for (int i = 1; i <= 500; ++i) {
    if ((i % 2) != 0) {
      REQUIRE(tree.contains(root, i));
    } else {
      REQUIRE_FALSE(tree.contains(root, i));
    }
  }
}

TEST_CASE("Insert ascending") {
  TempDB db("btree_asec");

  Pager pager(db.file());

  Btree tree(pager);

  PageId root = pager.allocatePage();

  LeafNode(pager.read(root)).init();
  for (int i = 1; i <= 1000; ++i) {
    root = tree.insert(root, i, makeRecord(std::to_string(i)));
  }

  for (int i = 1; i <= 1000; ++i) {
    REQUIRE(tree.contains(root, i));
  }
}

TEST_CASE("Insert descending") {
  TempDB db("btree_desc");

  Pager pager(db.file());

  Btree tree(pager);

  PageId root = pager.allocatePage();

  LeafNode(pager.read(root)).init();

  for (int i = 1000; i >= 1; --i) {
    root = tree.insert(root, i, makeRecord(std::to_string(i)));
  }

  for (int i = 1; i <= 1000; ++i) {
    REQUIRE(tree.contains(root, i));
  }
}

TEST_CASE("Random insert") {
  std::mt19937 rng(1234);

  std::vector<int> keys(1000);
  std::iota(keys.begin(), keys.end(), 1);

  std::shuffle(keys.begin(), keys.end(), rng);

  TempDB db("btree_random_ins");

  Pager pager(db.file());

  Btree tree(pager);

  PageId root = pager.allocatePage();

  LeafNode(pager.read(root)).init();

  for (int k : keys) {
    root = tree.insert(root, k, makeRecord(std::to_string(k)));
  }

  for (int k : keys) {
    INFO(k);
    REQUIRE(tree.contains(root, k));
  }
}
