#include "page_layout.hpp"
#include "page_utils.hpp"
#include "pager.hpp"
#include <cctype>
#include <cstdint>
#include <iomanip>
#include <iostream>

auto getPageType(Page &page) -> PageType {
  auto *page_header = reinterpret_cast<CommonPage *>(page.data.data());
  return page_header->page_type;
}

void hexDump(const Page &page) {
  const auto *data = page.data.data();
  constexpr size_t width = 16;

  for (size_t i = 0; i < PAGE_SIZE; i += width) {
    // Offset
    std::cout << std::hex << std::setw(8) << std::setfill('0') << i << "  ";

    // Hex bytes
    for (size_t j = 0; j < width; ++j) {
      if (i + j < PAGE_SIZE) {
        std::cout << std::setw(2) << static_cast<unsigned>(data[i + j]) << ' ';
      } else {
        std::cout << "   ";
      }

      if (j == 7) {
        std::cout << ' ';
      }
    }

    std::cout << " |";

    // ASCII
    for (size_t j = 0; j < width && i + j < PAGE_SIZE; ++j) {
      uint8_t c = data[i + j];
      std::cout << ((std::isprint(c) != 0) ? static_cast<char>(c) : '.');
    }

    std::cout << "|\n";
  }

  std::cout << std::dec;
}
