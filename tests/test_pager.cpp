#include <catch2/catch_test_macros.hpp>

#include "pager.hpp"
#include "test_utils.hpp"

TEST_CASE("Pager allocates sequential pages") {
  TempDB db("pager_allocate");

  Pager pager(db.file());

  REQUIRE(pager.allocatePage() == 1);
  REQUIRE(pager.allocatePage() == 2);
  REQUIRE(pager.allocatePage() == 3);
}

TEST_CASE("Pager can read allocated page") {
  TempDB db("pager_read");

  Pager pager(db.file());

  PageId id = pager.allocatePage();

  Page &page = pager.read(id);

  REQUIRE(page.page_no == id);
}

TEST_CASE("Pager persists pages") {
  TempDB db("pager_persist");

  PageId id{};

  {
    Pager pager(db.file());

    id = pager.allocatePage();

    auto &page = pager.read(id);

    page.data[100] = 42;

    pager.flushPage(id);
  }

  {
    Pager pager(db.file());

    auto &page = pager.read(id);

    REQUIRE(page.data[100] == 42);
  }
}

TEST_CASE("Pager allocates sequential ids") {
  TempDB db("pager_many");

  Pager pager(db.file());

  for (uint32_t i = 1; i <= 1000; ++i) {
    REQUIRE(pager.allocatePage() == i);
  }
}
