#pragma once

#include "btree.hpp"
#include "page_layout.hpp"
#include <cstdint>

struct InternalCell {
  PageId page_id;
  Key key;
};

class InternalNode {
private:
  Page &page;

  auto header() -> InternalHeader &;
  [[nodiscard]] auto header() const -> const InternalHeader &;

  auto cells() -> InternalCell *;
  auto getCellPos(Key key) -> int;

public:
  explicit InternalNode(Page &page);
  auto child(uint32_t i) -> PageId;
  auto key(uint32_t i) -> Key;

  auto getChild(Key key) -> PageId;

  auto insert(Key sep, PageId left_child, PageId right_child) -> InsertResult;

  void init(PageId right_child);
  void clear(PageId right_child);

  auto split(Page &new_page, Key sep, PageId left_child, PageId right_child) -> SplitResult;
};
