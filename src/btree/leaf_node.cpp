#include "leaf_node.hpp"
#include "btree.hpp"
#include "page_layout.hpp"
#include "page_utils.hpp"
#include "pager.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>

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

auto LeafNode::insert(Key key, const Record &record) -> InsertResult {
  auto *slots = this->slots();
  auto &hdr = header();
  auto n_slots = hdr.cell_cnt;

  uint16_t record_size =
      record.key.size() + sizeof(record.key_size) + record.record.size();
  uint16_t space_needed = sizeof(Slot) + record_size;
  if (space_needed > freeSize()) {
    // No avaialbe area;
    return InsertResult{
        .split = true,
        .old_page = this->page.page_no,
    };
  }

  uint16_t insert_pos = getSlotPos(slots, key);

  std::memmove(&slots[insert_pos + 1], // destination
               &slots[insert_pos],     // source
               (n_slots - insert_pos) * sizeof(Slot));

  uint16_t new_free_end = hdr.free_end - record_size;
  uint16_t new_free_start = hdr.free_start + sizeof(Slot);

  hdr.free_end = new_free_end;
  hdr.free_start = new_free_start;

  slots[insert_pos].offset = new_free_end;
  slots[insert_pos].size = record_size;

  uint16_t write_pos = new_free_end;

  memcpy(page.data.data() + write_pos, &record.key_size,
         sizeof(record.key_size));
  write_pos += sizeof(record.key_size);

  memcpy(page.data.data() + write_pos, record.key.data(), record.key.size());
  write_pos += record.key.size();

  memcpy(page.data.data() + write_pos, record.record.data(),
         record.record.size());

  hdr.cell_cnt += 1;
  return InsertResult{
      .split = false,
  };
}

auto LeafNode::getSlotPos(Slot *slots, Key key) -> uint16_t {
  int l = 0;
  int h = static_cast<int>(header().cell_cnt) - 1;

  while (l <= h) {
    int m = l + (h - l) / 2;

    if (getKey(slots[m]) > key) {
      h = m - 1;
    } else {
      l = m + 1;
    }
  }

  return static_cast<uint16_t>(l);
}

auto LeafNode::freeSize() -> uint16_t {
  return header().free_end - header().free_start;
}

auto LeafNode::getRecord(Key key) const -> Record {
  Slot *slots = this->slots();
  auto hrd = header();
  Record rec{};
  for (uint32_t i = 0; i < hrd.cell_cnt; i++) {
    Key curr_key = getKey(slots[i]);
    if (curr_key == key) {
      auto key_size = key.size();
      auto *offset = page.data.data() + slots[i].offset + sizeof(KeySize) + key_size;

      rec.key = key;
      rec.key_size = key_size;
      rec.record.resize(slots[i].size - key.size() - sizeof(KeySize));
      std::memcpy(rec.record.data(), offset, rec.record.size());
      break;
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
    if (getKey(slots[m]) == key) {
      return true;
    } else if (getKey(slots[m]) < key) {
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

auto LeafNode::entries() -> std::vector<std::pair<Key, Record>> {
  Slot *slots = this->slots();
  uint32_t cell_cnt = this->header().cell_cnt;
  std::vector<std::pair<Key, Record>> krpair(cell_cnt);

  for (uint32_t i = 0; i < cell_cnt; i++) {
    Key key = getKey(slots[i]);
    Record rec = getRecord(key);
    krpair[i] = {key, rec};
  }

  return krpair;
}

void LeafNode::clear() {
  auto &hdr = header();
  hdr.common.page_type = PageType::Leaf;
  hdr.cell_cnt = 0;
  hdr.free_start = sizeof(LeafHeader);
  hdr.free_end = PAGE_SIZE;
}

auto LeafNode::split(Page &new_page, Key key, Record rec) -> SplitResult {
  auto &hdr = this->header();
  auto entries = this->entries();
  entries.emplace_back(key, rec);
  std::sort(begin(entries), end(entries),
            [](auto &a, auto &b) -> auto { return a.first < b.first; });

  LeafNode new_leaf_page(new_page);
  new_leaf_page.init();
  this->clear();
  new_leaf_page.header().next_leaf = hdr.next_leaf;
  hdr.next_leaf = new_page.page_no;

  uint32_t mid = entries.size() / 2;
  Key promoted_key = entries[mid].first;

  uint32_t i = 0;
  for (; i < mid; i++) {
    this->insert(entries[i].first, entries[i].second);
  }
  for (; i < entries.size(); i++) {
    new_leaf_page.insert(entries[i].first, entries[i].second);
  }

  return SplitResult{
      .sep = promoted_key,
      .left_child = this->page.page_no,
      .right_child = new_page.page_no,
  };
}

void LeafNode::delete_rec(Key key) {
  auto &hdr = this->header();
  Slot *slots = this->slots();

  int l = 0;
  int h = static_cast<int>(hdr.cell_cnt) - 1;

  while (l <= h) {
    int m = l + (h - l) / 2;
    if (getKey(slots[m]) == key) {
      std::memmove(&slots[m],     // destination
                   &slots[m + 1], // source
                   (hdr.cell_cnt - m - 1) * sizeof(Slot));
      hdr.cell_cnt -= 1;
      hdr.free_start -= sizeof(Slot);
      return;
    } else if (getKey(slots[m]) < key) {
      l = m + 1;
    } else {
      h = m - 1;
    }
  }
}

void LeafNode::compact() {
  auto entries = this->entries();

  this->clear();

  for (auto &kr : entries) {
    this->insert(kr.first, kr.second);
  }
}

auto LeafNode::getKey(Slot slot) const -> Key {
  auto offset = slot.offset;
  auto key_size = read<KeySize>(page.data.data() + offset);
  offset += sizeof(KeySize);

  Key key(key_size);
  memcpy(key.data(), page.data.data() + offset, key_size);

  return key;
}
