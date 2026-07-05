#include "pager.hpp"
#include <cstdint>

auto Pager::read(PageId page_no) -> Page & {
  if (buffer.contains(page_no)) {
    return buffer.getPage(page_no);
  }

  loadPage(page_no);

  return buffer.getPage(page_no);
}

void Pager::loadPage(PageId page_no) {
  Page page{};
  page.page_no = page_no;
  page.dirty = false;

  dbfile.read(static_cast<uint64_t>(page_no - 1) * PAGE_SIZE, page.data.data(),
              PAGE_SIZE);

  buffer.storePage(std::move(page));
}

void Pager::flushPage(PageId page_no) {
  if (!buffer.contains(page_no)) {
    return;
  }

  Page &page = buffer.getPage(page_no);

  if (!page.dirty) {
    return;
  }

  write(page);

  page.dirty = false;
}

void Pager::write(Page &page) {
  uint32_t page_no = page.page_no;

  dbfile.write(static_cast<uint64_t>(page_no - 1) * PAGE_SIZE, page.data.data(),
               PAGE_SIZE);
}

auto Pager::allocatePage() -> PageId {
  page_cnt += 1;

  dbfile.resize(static_cast<uint64_t>(page_cnt - 1) * PAGE_SIZE);

  return page_cnt;
}

Pager::Pager(std::filesystem::path filepath)
    : dbfile(std::move(filepath)),
      page_cnt(0)
{
    uint64_t bytes = dbfile.filesize();

    if (bytes % PAGE_SIZE != 0) {
        throw std::runtime_error("Database file is corrupt");
    }

    page_cnt = static_cast<PageId>(bytes / PAGE_SIZE);
}
