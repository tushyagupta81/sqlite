#include "internal_node.hpp"
#include "btree.hpp"
#include "page_layout.hpp"
#include "pager.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>

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

auto InternalNode::insert(Key sep, PageId left_child, PageId right_child)
    -> InsertResult {
  uint16_t max_cells =
      (PAGE_SIZE - sizeof(InternalHeader)) / sizeof(InternalCell);
  InternalHeader &hdr = this->header();
  if (hdr.key_cnt == max_cells) {
    return InsertResult{
        .split = true,
        .old_page = this->page.page_no,
    };
  }
  InternalCell *cells = this->cells();

  // Case 1: the split happened on the current rightmost child.
  if (hdr.right_child == left_child) {
    cells[hdr.key_cnt].key = sep;
    cells[hdr.key_cnt].page_id = left_child;

    hdr.right_child = right_child;
    hdr.key_cnt++;

    return InsertResult{
        .split = false,
        .old_page = this->page.page_no,
    };
  }

  // Find the cell whose child matches left_child.
  int pos = -1;
  for (uint32_t i = 0; i < hdr.key_cnt; i++) {
    if (cells[i].page_id == left_child) {
      pos = static_cast<int>(i);
      break;
    }
  }

  assert(pos != -1);

  // Make room after the matching child.
  std::memmove(&cells[pos + 2], &cells[pos + 1],
               (hdr.key_cnt - pos - 1) * sizeof(InternalCell));

  cells[pos + 1].key = cells[pos].key;
  cells[pos].key = sep;
  cells[pos + 1].page_id = right_child;

  hdr.key_cnt++;

  return InsertResult{
      .split = false,
      .old_page = this->page.page_no,
  };
}

auto InternalNode::split(Page &new_page, Key sep, PageId left_child,
                         PageId right_child) -> SplitResult {
  auto &hdr = header();
  InternalCell *cells = this->cells();

  std::vector<PageId> children;
  std::vector<Key> keys;

  children.reserve(hdr.key_cnt + 2);
  keys.reserve(hdr.key_cnt + 1);

  for (uint32_t i = 0; i < hdr.key_cnt; ++i) {
    children.push_back(cells[i].page_id);
    keys.push_back(cells[i].key);
  }

  children.push_back(hdr.right_child);

  auto it = std::find(children.begin(), children.end(), left_child);
  assert(it != children.end());

  long child_pos = std::distance(children.begin(), it);

  children.insert(children.begin() + child_pos + 1, right_child);
  keys.insert(keys.begin() + child_pos, sep);

  size_t promote = keys.size() / 2;
  Key promoted_key = keys[promote];

  InternalNode right(new_page);

  this->clear(children.front());
  right.init(children[promote + 1]);

  for (size_t i = 0; i < promote; ++i) {
    this->insert(keys[i], children[i], children[i + 1]);
  }

  for (size_t i = promote + 1; i < keys.size(); ++i) {
    right.insert(keys[i], children[i], children[i + 1]);
  }

  return SplitResult{
      .sep = promoted_key,
      .left_child = this->page.page_no,
      .right_child = new_page.page_no,
  };
}
