#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <numeric>
#include <random>

#include "btree.hpp"
#include "leaf_node.hpp"
#include "test_utils.hpp"

TEST_CASE("Insert 10000 records") {
  TempDB db("btree_insert");

  Pager pager(db.file());

  Btree tree(pager);

  PageId root = pager.allocatePage();

  LeafNode(pager.read(root)).init();

  for (uint32_t i = 1; i <= 10000; ++i) {
    Key key = makeKey(i);
    root = tree.insert(root, key, makeRecord(std::to_string(i), key));
  }

  for (uint32_t i = 1; i <= 10000; ++i) {
    Key key = makeKey(i);
    REQUIRE(tree.contains(root, key));

    auto rec = tree.search(root, key).value();

    REQUIRE(std::string(rec.record.begin(), rec.record.end()) ==
            std::to_string(i));
  }
}

TEST_CASE("Insert 1000 records") {
  TempDB db("btree_insert");

  Pager pager(db.file());

  Btree tree(pager);

  PageId root = pager.allocatePage();

  LeafNode(pager.read(root)).init();

  for (uint32_t i = 1; i <= 1000; ++i) {
    Key key = makeKey(i);
    root = tree.insert(root, key, makeRecord(std::to_string(i), key));
  }

  for (uint32_t i = 1; i <= 1000; ++i) {
    Key key = makeKey(i);
    REQUIRE(tree.contains(root, key));

    auto rec = tree.search(root, key).value();

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
    Key key = makeKey(i);
    root = tree.insert(root, key, makeRecord(std::to_string(i), key));
  }

  for (int i = 2; i <= 500; i += 2) {
    Key key = makeKey(i);
    tree.remove(root, key);
  }

  for (int i = 1; i <= 500; ++i) {
    Key key = makeKey(i);
    if ((i % 2) != 0) {
      REQUIRE(tree.contains(root, key));
    } else {
      REQUIRE_FALSE(tree.contains(root, key));
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
    Key key = makeKey(i);
    root = tree.insert(root, key, makeRecord(std::to_string(i), key));
  }

  for (int i = 1; i <= 1000; ++i) {
    Key key = makeKey(i);
    REQUIRE(tree.contains(root, key));
  }
}

TEST_CASE("Insert descending") {
  TempDB db("btree_desc");

  Pager pager(db.file());

  Btree tree(pager);

  PageId root = pager.allocatePage();

  LeafNode(pager.read(root)).init();

  for (int i = 1000; i >= 1; --i) {
    Key key = makeKey(i);
    root = tree.insert(root, key, makeRecord(std::to_string(i), key));
  }

  for (int i = 1; i <= 1000; ++i) {
    Key key = makeKey(i);
    REQUIRE(tree.contains(root, key));
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
    Key key = makeKey(k);
    root = tree.insert(root, key, makeRecord(std::to_string(k), key));
  }

  for (int k : keys) {
    Key key = makeKey(k);
    REQUIRE(tree.contains(root, key));
  }
}
