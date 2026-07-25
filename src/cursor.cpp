#include "cursor.hpp"

Cursor::Cursor(Pager &pager, Page &page, uint16_t slot_no)
    : pager(pager), page(page), leaf(LeafNode(page)), slot_no(slot_no) {}

auto Cursor::end() -> bool {
  return slot_no == leaf.cellCount() && leaf.nextLeaf() == 0;
}

auto Cursor::inc() -> bool {
  if (this->end()) {
    return false;
  }
  this->slot_no += 1;
  if (this->slot_no == leaf.cellCount()) {
    if (leaf.nextLeaf() == 0) {
      return false;
    }
    Page &new_page = pager.read(leaf.nextLeaf());
    page = new_page;
    leaf.replacePage(new_page);
    this->slot_no = 0;
  }

  return true;
}

auto Cursor::getRec() -> std::optional<Record> {
  auto rec = leaf.getRecordAt(this->slot_no);

  this->inc();

  return rec;
}
