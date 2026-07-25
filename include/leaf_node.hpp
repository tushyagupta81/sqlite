#pragma once

#include "page_layout.hpp"
#include "table_vals.hpp"
#include <cstdint>
#include <optional>

class LeafNode {
private:
  Page &page;

  auto header() -> LeafHeader &;
  [[nodiscard]] auto header() const -> const LeafHeader &;

  auto getSlotPos(Slot *slots, Key key) -> uint16_t;
  auto freeSize() -> uint16_t;
  auto getRecAtSlot(Slot *slot) const -> Record;

  [[nodiscard]] auto slots() const -> Slot *;

  auto entries() -> std::vector<std::pair<Key, Record>>;

  [[nodiscard]] auto getKey(Slot *slot) const -> Key;

public:
  explicit LeafNode(Page &page);

  auto cellCount() -> const uint16_t;
  void setCount(uint16_t new_cnt);

  auto insert(Key key, const Record &record) -> InsertResult;
  void delete_rec(Key key);
  auto split(Page &new_page, Key key, Record rec) -> SplitResult;
  void compact();

  [[nodiscard]] auto getRecord(Key key) const -> Record;
  [[nodiscard]] auto getRecordIdx(Key key) const -> std::optional<uint16_t>;
  [[nodiscard]] auto contains(Key key) const -> bool;

  [[nodiscard]] auto getRecordAt(uint16_t slot_no) const
      -> std::optional<Record>;
  [[nodiscard]] auto nextLeaf() const -> PageId;

  void replacePage(Page &page);

  void init();
  void clear();
};
