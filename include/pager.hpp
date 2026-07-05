#pragma once

#include "disk.hpp"
#include <array>
#include <cstdint>
#include <unordered_map>

static constexpr auto PAGE_SIZE = 4096;
using PageId = uint32_t;

struct Page{
  PageId page_no;
  bool dirty;

  std::array<uint8_t, PAGE_SIZE> data;
};

class BufferPool{
  private:
    std::unordered_map<PageId, Page> pages;
  public:
    auto contains(PageId page_no) -> bool;
    auto getPage(PageId page_no) -> Page&;
    auto tryGetPage(PageId page_no) -> Page*;
    void storePage(Page &&page);
    void remove(PageId page_no);
};

class Pager{
  private:
    BufferPool buffer;
    DiskManager dbfile;
    PageId page_cnt;

    void loadPage(PageId page_no);
    void write(Page &page);
  public:
    explicit Pager(std::filesystem::path filepath);
    auto read(PageId page_no) -> Page&;

    void flushPage(PageId page_no);
    auto allocatePage() -> PageId;
};
