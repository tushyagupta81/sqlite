#pragma once

#include "page_layout.hpp"

class InternalNode {
  private:
    Page &page;
    auto header() -> InternalHeader&;
    [[nodiscard]] auto header() const -> const InternalHeader&;
    
  public:
    explicit InternalNode(Page& page);
    auto child(uint32_t i) -> PageId;
    auto key(uint32_t i) -> PageId;
};
