#pragma once

#include "page_layout.hpp"
#include "table.hpp"
#include <cstdint>
#include <memory>
#include <optional>

const uint32_t magic = 0x89ABCDEF;

struct DatabaseHeader {
  uint32_t magic_number;
  CommonPage page_type;
  PageId page_no;
  PageId trunk_page;
  PageId main_table;
};

class Catalog {
  private:
    DatabaseHeader db_header{};
    Pager &pager;
    RowEncoder row_encoder;
    Btree btree;
    std::unique_ptr<Table> main_table;
  public:
    explicit Catalog(Pager &pager);
    ~Catalog();
    auto getTable(std::string table_name) -> std::optional<Table>;
    void createTable();
};
