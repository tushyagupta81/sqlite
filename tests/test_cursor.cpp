#include <catch2/catch_test_macros.hpp>

#include "cursor.hpp"
#include "leaf_node.hpp"
#include "test_utils.hpp"

TEST_CASE("Cursor on empty leaf") {
  TempDB db("cursor_empty");

  Pager pager(db.file());

  auto root = pager.allocatePage();
  auto &page = pager.read(root);

  LeafNode(page).init();

  Cursor cursor(pager, page, 0);

  REQUIRE(cursor.end());
  REQUIRE_FALSE(cursor.getRec().has_value());
}

TEST_CASE("Cursor returns first record") {
  TempDB db("cursor_first");

  Pager pager(db.file());

  auto root = pager.allocatePage();
  auto &page = pager.read(root);

  LeafNode(page).init();

  LeafNode leaf(page);

  leaf.insert(makeKey<uint32_t>(1),
              makeRecord("1", makeKey<uint32_t>(1)));
  leaf.insert(makeKey<uint32_t>(2),
              makeRecord("2", makeKey<uint32_t>(2)));

  Cursor cursor(pager, page, 0);

  auto rec = cursor.getRec();

  REQUIRE(rec.has_value());
  REQUIRE(rec->key == makeKey<uint32_t>(1));
  REQUIRE(std::string(rec->record.begin(), rec->record.end()) == "1");
}

TEST_CASE("Cursor returns second record") {
  TempDB db("cursor_second");

  Pager pager(db.file());

  auto root = pager.allocatePage();
  auto &page = pager.read(root);

  LeafNode(page).init();

  LeafNode leaf(page);

  leaf.insert(makeKey<uint32_t>(1),
              makeRecord("1", makeKey<uint32_t>(1)));
  leaf.insert(makeKey<uint32_t>(2),
              makeRecord("2", makeKey<uint32_t>(2)));

  Cursor cursor(pager, page, 1);

  auto rec = cursor.getRec();

  REQUIRE(rec.has_value());
  REQUIRE(rec->key == makeKey<uint32_t>(2));
  REQUIRE(std::string(rec->record.begin(), rec->record.end()) == "2");
}

TEST_CASE("Cursor past last slot") {
  TempDB db("cursor_past_end");

  Pager pager(db.file());

  auto root = pager.allocatePage();
  auto &page = pager.read(root);

  LeafNode(page).init();

  LeafNode(page).insert(makeKey<uint32_t>(1),
                        makeRecord("1", makeKey<uint32_t>(1)));

  Cursor cursor(pager, page, 1);

  REQUIRE(cursor.end());
  REQUIRE_FALSE(cursor.getRec().has_value());
}

TEST_CASE("Cursor on full leaf") {
  TempDB db("cursor_many");

  Pager pager(db.file());

  auto root = pager.allocatePage();
  auto &page = pager.read(root);

  LeafNode(page).init();

  LeafNode leaf(page);

  for (uint32_t i = 1; i <= 100; ++i) {
    leaf.insert(makeKey<uint32_t>(i),
                makeRecord(std::to_string(i), makeKey<uint32_t>(i)));
  }

  Cursor first(pager, page, 0);
  Cursor middle(pager, page, 49);
  Cursor last(pager, page, 99);

  auto r1 = first.getRec();
  auto r2 = middle.getRec();
  auto r3 = last.getRec();

  REQUIRE(r1.has_value());
  REQUIRE(r2.has_value());
  REQUIRE(r3.has_value());

  REQUIRE(std::string(r1->record.begin(), r1->record.end()) == "1");
  REQUIRE(std::string(r2->record.begin(), r2->record.end()) == "50");
  REQUIRE(std::string(r3->record.begin(), r3->record.end()) == "100");
}
