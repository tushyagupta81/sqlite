#include <catch2/catch_test_macros.hpp>

#include "internal_node.hpp"

TEST_CASE("Internal node chooses child") {
  Page page{};

  InternalNode node(page);

  node.init(1);

  node.insert(10, 1, 2);
  node.insert(20, 2, 3);
  node.insert(30, 3, 4);

  REQUIRE(node.getChild(5) == 1);
  REQUIRE(node.getChild(10) == 2);
  REQUIRE(node.getChild(15) == 2);
  REQUIRE(node.getChild(25) == 3);
  REQUIRE(node.getChild(35) == 4);
}

TEST_CASE("Internal insert sorted") {
  Page page{};
  InternalNode node(page);

  node.init(1);

  for (int i = 1; i <= 200; i += 10) {
    node.insert(i, i, i + 10);
  }

  for (int i = 1; i <= 200; i += 10) {
    REQUIRE(node.getChild(i) == i + 10);
  }
}
