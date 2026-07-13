#include <catch2/catch_test_macros.hpp>

#include "internal_node.hpp"
#include "test_utils.hpp"

TEST_CASE("Internal node chooses child") {
  Page page{};

  InternalNode node(page);

  node.init(1);

  node.insert(makeKey<uint32_t>(10), 1, 2);
  node.insert(makeKey<uint32_t>(20), 2, 3);
  node.insert(makeKey<uint32_t>(30), 3, 4);

  REQUIRE(node.getChild(makeKey<uint32_t>(5)) == 1);
  REQUIRE(node.getChild(makeKey<uint32_t>(10)) == 2);
  REQUIRE(node.getChild(makeKey<uint32_t>(15)) == 2);
  REQUIRE(node.getChild(makeKey<uint32_t>(25)) == 3);
  REQUIRE(node.getChild(makeKey<uint32_t>(35)) == 4);
}

TEST_CASE("Internal insert sorted") {
  Page page{};
  InternalNode node(page);

  node.init(1);

  for (int i = 1; i <= 200; i += 10) {
    node.insert(makeKey<uint32_t>(i), i, i + 10);
  }

  for (int i = 1; i <= 200; i += 10) {
    REQUIRE(node.getChild(makeKey<uint32_t>(i)) == i + 10);
  }
}

TEST_CASE("Internal node inserts separator after middle child split") {
  Page page{};
  InternalNode node(page);

  node.init(1);

  // Initial tree:
  // 1 |20| 2 |40| 3
  node.insert(makeKey<uint32_t>(20), 1, 2);
  node.insert(makeKey<uint32_t>(40), 2, 3);

  // Simulate child 2 splitting into (2, 4) with separator 30:
  // Expected:
  // 1 |20| 2 |30| 4 |40| 3
  node.insert(makeKey<uint32_t>(30), 2, 4);

  REQUIRE(node.getChild(makeKey<uint32_t>(5)) == 1);
  REQUIRE(node.getChild(makeKey<uint32_t>(20)) == 2);
  REQUIRE(node.getChild(makeKey<uint32_t>(25)) == 2);

  REQUIRE(node.getChild(makeKey<uint32_t>(30)) == 4);
  REQUIRE(node.getChild(makeKey<uint32_t>(35)) == 4);

  REQUIRE(node.getChild(makeKey<uint32_t>(40)) == 3);
  REQUIRE(node.getChild(makeKey<uint32_t>(45)) == 3);
}
