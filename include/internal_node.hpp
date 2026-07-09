#pragma once

#include "btree.hpp"
#include "page_layout.hpp"
#include <cstdint>

struct CellSlot {
  uint16_t offset;
  uint16_t size;
};

struct InternalCell {
  PageId page_id;
  KeySize key_size;
  Key key;
};

class InternalNode {
private:
  Page &page;

  auto header() -> InternalHeader &;
  [[nodiscard]] auto header() const -> const InternalHeader &;

  [[nodiscard]] auto readCell(uint16_t i) const -> InternalCell;
  void writeCell(InternalCell cell, uint8_t *dst);
  [[nodiscard]] auto slots() const -> CellSlot *;
  auto getCellPos(Key key) -> int;
  void appendCell(const InternalCell &cell);

public:
  explicit InternalNode(Page &page);
  auto child(uint32_t i) -> PageId;
  auto key(uint32_t i) -> Key;

  auto getChild(Key key) -> PageId;

  auto insert(Key sep, PageId left_child, PageId right_child) -> InsertResult;

  void init(PageId right_child);
  void clear(PageId right_child);

  auto split(Page &new_page, Key sep, PageId left_child, PageId right_child)
      -> SplitResult;
  void compact();
};
