#include "pager.hpp"

auto BufferPool::contains(PageId page_no) -> bool {
  return pages.find(page_no) != pages.end();
}

void BufferPool::storePage(Page &&page) {
  pages.emplace(page.page_no, std::move(page));
}

void BufferPool::remove(PageId page_no) { pages.erase(page_no); }

auto BufferPool::getPage(PageId page_no) -> Page & { return pages[page_no]; }

auto BufferPool::tryGetPage(PageId page_no) -> Page * {
  auto itterator = pages.find(page_no);

  if (itterator != pages.end()) {
    return &itterator->second;
  }

  return nullptr;
}
