#include "internal_node.hpp"
#include "btree.hpp"
#include "page_layout.hpp"
#include "page_utils.hpp"
#include "pager.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

InternalNode::InternalNode(Page &page) : page(page) {}

auto InternalNode::header() -> InternalHeader & {
  return *reinterpret_cast<InternalHeader *>(page.data.data());
}

auto InternalNode::slots() const -> CellSlot * {
  return reinterpret_cast<CellSlot *>(page.data.data() + sizeof(LeafHeader));
}

auto InternalNode::readCell(uint16_t i) const -> InternalCell {
  auto slot = slots()[i];

  InternalCell cell;

  auto *ptr = page.data.data() + slot.offset;
  cell.page_id = read<PageId>(ptr);
  ptr += sizeof(PageId);

  cell.key_size = read<KeySize>(ptr);
  ptr += sizeof(KeySize);

  cell.key.resize(cell.key_size);
  memcpy(cell.key.data(), ptr, cell.key_size);

  return cell;
}

auto InternalNode::header() const -> const InternalHeader & {
  return *reinterpret_cast<const InternalHeader *>(page.data.data());
}

auto InternalNode::child(uint32_t i) -> PageId {
  if (i == this->header().key_cnt) {
    return this->header().right_child;
  }

  assert(i < this->header().key_cnt);

  return readCell(i).page_id;
}

auto InternalNode::key(uint32_t i) -> Key {
  assert(i < this->header().key_cnt);

  return readCell(i).key;
}

auto InternalNode::getCellPos(Key key) -> int {
  int l = 0;
  int h = static_cast<int>(header().key_cnt) - 1;
  while (l <= h) {
    int m = l + (h - l) / 2;
    if (readCell(m).key > key) {
      h = m - 1;
    } else {
      l = m + 1;
    }
  }
  return l;
}

void InternalNode::writeCell(InternalCell cell, uint8_t *dst) {
  memcpy(dst, &cell.page_id, sizeof(cell.page_id));
  dst += sizeof(cell.page_id);
  memcpy(dst, &cell.key_size, sizeof(cell.key_size));
  dst += sizeof(cell.key_size);
  memcpy(dst, cell.key.data(), cell.key.size());
}

void InternalNode::init(PageId right_child) {
  auto &hdr = this->header();

  hdr.common.page_type = PageType::Internal;
  hdr.key_cnt = 0;
  hdr.free_start = sizeof(InternalHeader);
  hdr.free_end = PAGE_SIZE;
  hdr.right_child = right_child;
}

void InternalNode::clear(PageId right_child) {
  auto &hdr = this->header();

  hdr.common.page_type = PageType::Internal;
  hdr.key_cnt = 0;
  hdr.free_start = sizeof(InternalHeader);
  hdr.free_end = PAGE_SIZE;
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
  InternalHeader &hdr = this->header();
  auto cell_size = sizeof(PageId) + sizeof(KeySize) + sep.size();
  auto left_space = hdr.free_end - hdr.free_start;
  auto needed_space = cell_size + sizeof(CellSlot);
  if (left_space < needed_space) {
    return InsertResult{
        .split = true,
        .old_page = this->page.page_no,
    };
  }

  auto *slots = this->slots();
  auto page_offset = hdr.free_end - cell_size;
  auto *offset = page.data.data() + page_offset;

  // Case 1: the split happened on the current rightmost child.
  if (hdr.right_child == left_child) {
    InternalCell cell{};
    cell.key = sep;
    cell.key_size = sep.size();
    cell.page_id = left_child;

    hdr.right_child = right_child;

    writeCell(cell, offset);

    slots[hdr.key_cnt].offset = page_offset;
    slots[hdr.key_cnt].size = cell_size;

    hdr.key_cnt++;
    hdr.free_end = page_offset;
    hdr.free_start = hdr.free_start + sizeof(CellSlot);
    return InsertResult{
        .split = false,
        .old_page = this->page.page_no,
    };
  }

  // Find the cell whose child matches left_child.
  int pos = -1;
  for (uint32_t i = 0; i < hdr.key_cnt; i++) {
    if (readCell(i).page_id == left_child) {
      pos = static_cast<int>(i);
      break;
    }
  }

  assert(pos != -1);

  // Make room after the matching child.
  std::memmove(&slots[pos + 2], &slots[pos + 1],
               (hdr.key_cnt - pos - 1) * sizeof(CellSlot));

  InternalCell new_cell{};
  InternalCell old_cell = readCell(pos);

  // auto old_cell_ori_size = old_cell.key.size();

  // reuse cell for new slot
  // assign new cell for old slot
  new_cell.key = old_cell.key;
  new_cell.page_id = right_child;
  new_cell.key_size = new_cell.key.size();

  old_cell.key = sep;
  old_cell.key_size = sep.size();

  // auto new_cell_size = new_cell.key.size();
  // auto old_cell_size = old_cell.key.size();
  // std::cout << old_cell_ori_size << " " << new_cell_size << " " <<
  // old_cell_size
  //           << "\n";

  writeCell(new_cell, page.data.data() + slots[pos].offset);
  slots[pos + 1].offset = slots[pos].offset;
  slots[pos + 1].size = slots[pos].size;

  writeCell(old_cell, offset);
  slots[pos].offset = page_offset;
  slots[pos].size = cell_size;

  hdr.key_cnt++;
  hdr.free_end = page_offset;
  hdr.free_start = hdr.free_start + sizeof(CellSlot);

  return InsertResult{
      .split = false,
      .old_page = this->page.page_no,
  };
}

auto InternalNode::split(Page &new_page, Key sep, PageId left_child,
                         PageId right_child) -> SplitResult {
  auto &hdr = header();

  std::vector<PageId> children;
  std::vector<Key> keys;

  children.reserve(hdr.key_cnt + 2);
  keys.reserve(hdr.key_cnt + 1);

  for (uint32_t i = 0; i < hdr.key_cnt; ++i) {
    auto cell = readCell(i);
    children.push_back(cell.page_id);
    keys.push_back(cell.key);
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

void InternalNode::compact() {
  auto &hdr = header();

  if (hdr.key_cnt == 0) {
    return;
  }

  std::vector<InternalCell> cells;
  cells.reserve(hdr.key_cnt);

  PageId leftmost = child(0);

  for (uint32_t i = 0; i < hdr.key_cnt; ++i) {
    cells.push_back(readCell(i));
  }

  clear(leftmost);

  for (const auto &cell : cells) {
    appendCell(cell);
  }
}

auto InternalNode::appendCell(const InternalCell &cell) -> void {
  auto &hdr = header();
  auto *slots = this->slots();

  uint16_t cell_size = sizeof(PageId) + sizeof(KeySize) + cell.key.size();

  uint16_t page_offset = hdr.free_end - cell_size;
  uint8_t *ptr = page.data.data() + page_offset;

  writeCell(cell, ptr);

  slots[hdr.key_cnt].offset = page_offset;
  slots[hdr.key_cnt].size = cell_size;

  hdr.free_end = page_offset;
  hdr.free_start += sizeof(CellSlot);
  hdr.key_cnt++;
}
