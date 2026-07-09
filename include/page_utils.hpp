#pragma once

#include "page_layout.hpp"
#include "pager.hpp"
#include <cstring>

auto getPageType(Page &page) -> PageType;
void hexDump(const Page &page);

template <typename T> auto read(const uint8_t *ptr) -> T {
  T value;
  memcpy(&value, ptr, sizeof(T));
  return value;
}
