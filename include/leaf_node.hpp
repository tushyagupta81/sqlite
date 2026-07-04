#pragma once

#include "btree.hpp"
#include "page_layout.hpp"
#include <cstdint>

class LeafNode {
  private:
    Page &page;

    auto header() -> LeafHeader&;
    [[nodiscard]] auto header() const -> const LeafHeader&;

    auto getSlotPos(Slot*slots, Key key) -> uint16_t;
    auto freeSize() -> uint16_t;
    [[nodiscard]] auto slots() const -> Slot*;
  public:
    explicit LeafNode(Page &page);
    auto cellCount() -> const uint16_t;
    void setCount(uint16_t new_cnt);
    void insert(Key key, const Record &record);
    [[nodiscard]] auto getRecord(Key key) const -> Record;
    [[nodiscard]] auto contains(Key key) const -> bool;
    void init();
};
