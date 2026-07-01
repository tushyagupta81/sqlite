#include "leaf_node.hpp"
#include "btree.hpp"
#include "page_layout.hpp"
#include "pager.hpp"
#include <cstdint>
#include <cstring>

LeafNode::LeafNode(Page &page) : page(page) {}

auto LeafNode::header() -> LeafHeader & {
  return *reinterpret_cast<LeafHeader *>(page.data.data());
}

auto LeafNode::header() const -> const LeafHeader & {
  return *reinterpret_cast<const LeafHeader *>(page.data.data());
}

auto LeafNode::cellCount() -> const uint16_t { return header().cell_cnt; }

void LeafNode::setCount(uint16_t new_cnt) { header().cell_cnt = new_cnt; }

auto LeafNode::slots() const -> Slot * {
  return reinterpret_cast<Slot *>(page.data.data() + sizeof(LeafHeader));
}

void LeafNode::insert(Key key, const Record &record) {
  auto *slots = this->slots();
  auto &hdr = header();
  auto n_slots = hdr.cell_cnt;

  uint16_t record_size = record.record.size();
  uint16_t space_needed = sizeof(Slot) + record_size;
  if (space_needed > freeSize()) {
    // No avaialbe area;
    return;
  }

  uint16_t insert_pos = getSlotPos(slots, key);

  std::memmove(&slots[insert_pos + 1], // destination
               &slots[insert_pos],     // source
               (n_slots - insert_pos) * sizeof(Slot));

  uint16_t new_free_end = hdr.free_end - record_size;
  uint16_t new_free_start = hdr.free_start + sizeof(Slot);

  slots[insert_pos].offset = new_free_end;
  slots[insert_pos].size = record_size;
  slots[insert_pos].key = key;

  memcpy(page.data.data() + new_free_end, record.record.data(), record_size);

  hdr.cell_cnt += 1;
  hdr.free_end = new_free_end;
  hdr.free_start = new_free_start;
}

auto LeafNode::getSlotPos(Slot *slots, Key key) -> uint16_t {
  for (uint32_t i = 0; i < header().cell_cnt; i++) {
    if (slots[i].key > key) {
      return i;
    }
  }
  return header().cell_cnt;
}

auto LeafNode::freeSize() -> uint16_t {
  return header().free_end - header().free_start;
}

auto LeafNode::getRecord(Key key) const -> Record {
  Slot *slots = this->slots();
  auto hrd = header();
  Record rec{};
  for (uint32_t i = 0; i < hrd.cell_cnt; i++) {
    if (slots[i].key == key) {
      rec.record.resize(slots[i].size);
      std::memcpy(rec.record.data(), page.data.data() + slots[i].offset,
                  slots[i].size);
    }
  }
  return rec;
}

auto LeafNode::contains(Key key) const -> bool {
  Slot *slots = this->slots();
  auto hdr = header();
  if (hdr.cell_cnt == 0) {
    return false;
  }
  int l = 0;
  int h = hdr.cell_cnt - 1;
  while (l <= h) {
    int m = l + ((h - l) / 2);
    if (slots[m].key == key) {
      return true;
    } else if (slots[m].key < key) {
      l = m + 1;
    } else {
      h = m - 1;
    }
  }
  return false;
}

void LeafNode::init() {
  auto &hdr = header();
  hdr.common.page_type = PageType::Leaf;
  hdr.cell_cnt = 0;
  hdr.free_start = sizeof(LeafHeader);
  hdr.free_end = PAGE_SIZE;
  hdr.next_leaf = 0;
}
