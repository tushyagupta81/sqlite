#pragma once

#include "pager.hpp"
#include <cstdint>

struct Slot {
  uint16_t offset;
  uint16_t size;
};

enum class PageType: uint8_t {
  Metadata,
  Leaf,
  Internal,
  FreeListTrunk,
  Overflow
};

struct CommonPage{
  PageType page_type;
};

struct LeafHeader{
  CommonPage common;
  uint16_t cell_cnt;
  uint16_t free_start;
  uint16_t free_end;
  PageId next_leaf;
};

struct InternalHeader{
  CommonPage common;
  uint16_t key_cnt;
  PageId right_child;
  uint16_t free_start;
  uint16_t free_end;
};

struct FreeListHeader{
  PageId next_trunk;
  uint32_t page_cnt;
};
