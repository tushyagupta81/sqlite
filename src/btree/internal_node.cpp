#include "internal_node.hpp"
#include "page_layout.hpp"
#include "pager.hpp"
#include <cassert>
#include <cstdint>
#include <cstring>

InternalNode::InternalNode(Page &page) : page(page) {}

auto InternalNode::header() -> InternalHeader & {
  return *reinterpret_cast<InternalHeader *>(page.data.data());
}

auto InternalNode::cells() -> InternalCell * {
  return reinterpret_cast<InternalCell *>(page.data.data() +
                                          sizeof(InternalHeader));
}

auto InternalNode::header() const -> const InternalHeader & {
  return *reinterpret_cast<const InternalHeader *>(page.data.data());
}

auto InternalNode::child(uint32_t i) -> PageId {
  if (i == this->header().key_cnt) {
    return this->header().right_child;
  }

  assert(i < this->header().key_cnt);

  return this->cells()[i].page_id;
}

auto InternalNode::key(uint32_t i) -> Key {
  assert(i < this->header().key_cnt);

  return this->cells()[i].key;
}

auto InternalNode::getCellPos(Key key) -> int {
  InternalCell *cells = this->cells();
  int l = 0;
  int h = static_cast<int>(header().key_cnt) - 1;
  while (l <= h) {
    int m = l + (h - l) / 2;
    if (cells[m].key > key) {
      h = m - 1;
    } else {
      l = m + 1;
    }
  }
  return l;
}

void InternalNode::init(PageId right_child) {
  auto &hdr = this->header();

  hdr.common.page_type = PageType::Internal;
  hdr.key_cnt = 0;
  hdr.right_child = right_child;
}

void InternalNode::clear(PageId right_child) {
  auto &hdr = this->header();

  hdr.common.page_type = PageType::Internal;
  hdr.key_cnt = 0;
  hdr.right_child = right_child;
}

auto InternalNode::getChild(Key key) -> PageId {
  InternalHeader &hdr = this->header();

  int l = this->getCellPos(key);

  if (l == static_cast<int>(hdr.key_cnt)) {
    return hdr.right_child;
  }

  return this->child(l);
}

void InternalNode::insert(Key sep, PageId left_child, PageId right_child) {
  uint16_t max_cells =
      (PAGE_SIZE - sizeof(InternalHeader)) / sizeof(InternalCell);
  InternalHeader &hdr = this->header();
  if (hdr.key_cnt == max_cells) {
    // split
    return;
  }
  InternalCell *cells = this->cells();

  // Case 1: the split happened on the current rightmost child.
  if (hdr.right_child == left_child) {
    cells[hdr.key_cnt].key = sep;
    cells[hdr.key_cnt].page_id = left_child;

    hdr.right_child = right_child;
    hdr.key_cnt++;

    return;
  }

  // Find the cell whose child matches left_child.
  int pos = -1;
  for (uint32_t i = 0; i < hdr.key_cnt; i++) {
    if (cells[i].page_id == left_child) {
      pos = static_cast<int>(i);
      break;
    }
  }

  if (pos == -1) {
    // Tree corruption / programming error.
    return;
  }

  // Make room after the matching child.
  std::memmove(&cells[pos + 2], &cells[pos + 1],
               (hdr.key_cnt - pos - 1) * sizeof(InternalCell));

  cells[pos + 1].key = sep;
  cells[pos + 1].page_id = right_child;

  hdr.key_cnt++;
}
