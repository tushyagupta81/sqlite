#include "page_layout.hpp"
#include "pager.hpp"

auto getPageType(Page &page) -> PageType {
  auto *page_header = reinterpret_cast<CommonPage *>(page.data.data());
  return page_header->page_type;
}
