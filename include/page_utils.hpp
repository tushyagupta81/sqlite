#pragma once

#include "page_layout.hpp"
#include "pager.hpp"
#include <cstring>
#include <vector>

auto getPageType(Page &page) -> PageType;
void hexDump(const Page &page);

template <typename T> auto read(const uint8_t *ptr) -> T {
  T value;
  memcpy(&value, ptr, sizeof(T));
  return value;
}

template <typename T> auto readBigEndian(const std::byte *data) -> T {
  using U = std::make_unsigned_t<T>;

  U value = 0;
  for (size_t i = 0; i < sizeof(T); ++i) {
    value <<= 8;
    value |= static_cast<U>(std::to_integer<uint8_t>(data[i]));
  }

  return static_cast<T>(value);
}

template <typename T>
void appendBigEndian(std::vector<std::byte> &out, T value) {
  using U = std::make_unsigned_t<T>;
  U u = static_cast<U>(value);

  for (int i = sizeof(T) - 1; i >= 0; --i) {
    out.push_back(static_cast<std::byte>((u >> (i * 8)) & 0xFF));
  }
}
