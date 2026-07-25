#pragma once

#include "leaf_node.hpp"
#include "pager.hpp"
#include <cstdint>
#include <optional>

class Cursor {
private:
  Page &page;
  Pager &pager;
  LeafNode leaf;
  uint16_t slot_no;

  auto inc() -> bool;

public:
  explicit Cursor(Pager &pager,Page &page, uint16_t slot_no);

  auto getRec() -> std::optional<Record>;

  auto end() -> bool;
};
