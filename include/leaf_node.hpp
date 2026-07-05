#pragma once

#include "btree.hpp"
#include "page_layout.hpp"
#include <cstdint>
#include <vector>

class LeafNode {
  private:
    Page &page;

    auto header() -> LeafHeader&;
    [[nodiscard]] auto header() const -> const LeafHeader&;

    auto getSlotPos(Slot*slots, Key key) -> uint16_t;
    auto freeSize() -> uint16_t;
    [[nodiscard]] auto slots() const -> Slot*;

    auto entries() -> std::vector<std::pair<Key, Record>>;
  public:
    explicit LeafNode(Page &page);

    auto cellCount() -> const uint16_t;
    void setCount(uint16_t new_cnt);

    auto insert(Key key, const Record &record) -> InsertResult;
    void delete_rec(Key key);
    auto split(Page &new_page, Key key, Record rec) -> SplitResult;
    void compact();

    [[nodiscard]] auto getRecord(Key key) const -> Record;
    [[nodiscard]] auto contains(Key key) const -> bool;

    void init();
    void clear();
};
